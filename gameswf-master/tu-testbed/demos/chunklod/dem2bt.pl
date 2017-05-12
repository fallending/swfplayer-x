#!/usr/bin/perl

# Script to convert .DEM files to .BT -- Thatcher Ulrich <tu@tulrich.com> 2002

# This source code has been donated to the Public Domain.  Do
# whatever you want with it.



use strict;
use warnings;
use POSIX;


sub print_usage
{
	print "dem2bt.pl -- convert .DEM terrain file to .BT format.\n";
	print "usage: dem2bt.pl <dem_file.dem> <output_file.bt>\n";
	print "[on Win32, you may need to type 'perl dem2bt.pl ...']\n";
}


sub pack_double
{
	my ($val) = @_;

	my $sign = 0;
	if ($val < 0) {
		$sign = 1;
		$val = - $val;
	}

	if ($val == 0)
	{
		return reverse(pack("B64", $sign . ("0" x 63)));
	}

	my $exponent = POSIX::floor(log($val)/log(2));
	my $mantissa = ($val / (1 << $exponent));

	# Keep 24 bits of precision.  Much more than that and we lose it during the conversion.
	my $mantissa_string = sprintf("%024b0000000000000000000000000000", $mantissa * (1 << 24));
	$mantissa_string = substr($mantissa_string, 1);  # lose the first character; (implied 1)

#	print "val = $val, sign = $sign, exp = $exponent, man = $mantissa\nmanstr = $mantissa_string, len = " . length($mantissa_string) . "\n";

	my $bit_string = sprintf("%1b%011b", $sign, $exponent + 1023) . $mantissa_string;
#	print $bit_string . "\n";
	my $bin_data = reverse(pack("B64", $bit_string));

#	print "len bitstr = " . length($bin_data) . "\n";
#	print "bin data = ";
#	for (my $i = 0; $i < 8; $i++) { printf "%02x", ord(substr($bin_data, $i, 1)); }
#	print "\n";

	return $bin_data;
}


sub main
{
	# Get args.
	my $infile = $ARGV[0] || '';
	my $outfile = $ARGV[1] || '';

	if ($infile eq '')
	{
		print_usage();
		exit(1);
	}

	if ($outfile eq '')
	{
		# Try to construct an output filename by tacking on ".bt".
		my $stem = ($infile =~ /^(.*)\.(\w+)$/)[0] || $infile;
		$outfile = $stem . ".bt";
		if ($outfile eq $infile)
		{
			print_usage();
			exit(1);
		}
	}
	
	if (! -f $infile)
	{
		print "file $infile does not exist or is not an ordinary file\n";
		print_usage();
		exit(1);
	}

	#
	# Do the conversion.
	#

	open IN, "<$infile" or die "can't open $infile: $!";
	open OUT, ">$outfile" or die "can't open $outfile: $!";
	binmode(OUT, ":raw");

	my $header_block;
	if (read(IN, $header_block, 1024) != 1024)
	{
		die "error reading header block: $!";
	}

	# parse header.
	my ($fname, $text, $filler, $SE_long, $SE_lat,
		$process_code,
		$page3,  # $filler2, $sectional, $origin_code, $dem_level_code, $elevation_pattern,
		$planimetric_reference_system, $zone_in_planimetric_reference_system,
		$map_projection_params, $planimetric_units, $elevation_units, $polygon_side_count,
		$quadrangle_boundary, $min_and_max_elevation, $reference_angle, $accuracy_code,
		$spatial_resolution, $rows, $cols
		) =
		unpack("A40A40A29A13A13" .
			   "a1" .
			   "a20" .
			   "A6A6" .
			   "A360A6A6A6" .
			   "A192A48A24A6" .
			   "A36A6A6", $header_block);
	# whew -- there's more, but I think we can get away with just these...

	# Print the header info.
	print "dataset name '$fname'\n";
	print "description = '$text'\n";
	print "units_code = $elevation_units\n";
	print "rows = $rows, columns = $cols\n";

	# Note that "rows" should actually be '1', because it refers to
	# the number of "rows" in each column of the subsequent data.
	if ($rows != 1)
	{
		die "Error; rows should be 1!";
	}

	my $vertical_scale = 1;
	if ($elevation_units == 1)
	{
		# Elevation is in feet.  .BT wants the data in meters.
		$vertical_scale = 0.3048;
	}

	# TODO convert lat/long to UTM, to be put in .BT header.

	# Write the .BT header.
	print OUT pack("A10VVvvvvv", ("binterr1.1", $cols, $cols,
								  2, 0,  # sample size, float flag
								  0, 0, 0));  # UTM flag, UTM zone, datum
	print OUT pack_double(0);  # left
	print OUT pack_double($cols);  # right
	print OUT pack_double(0);  # bottom
	print OUT pack_double($cols);  # top
	print OUT "\0" x 196;

	# Read columns from the DEM, and write them to the BT.
	for (my $col = 0; $col < $cols; $col++)
	{
		my $record;
		(read(IN, $record, 1024) == 1024) or die "error reading record (column $col): $!";

#		print $record . "\n";  #xxx
#		$_ = <STDIN>;  #xxx

		my ($r, $c, $rcount, $ccount, $x, $y, $datum_height, $min_height, $max_height, $elevations)
			= unpack("A6A6A6A6A24A24A24A24A24A878", $record);

		($c == $col + 1) or die "expected col " . ($col + 1) . " but got col $c: $!";
		($r == 1) or die "expected row 1 but got row $r: $!";
		($rcount == $cols) or die "expected rcount $cols but got rcount $rcount: $!";
		($ccount == 1) or die "expect ccount 1 but got ccount $ccount: $!";
		
		my $elem_count = 0;
		do {
			my @vals = split(' ', $elevations);
			my $val_count = scalar(@vals);
			($val_count > 0) or die "got no elevations from data: '$elevations'";
			($elem_count + $val_count <= $cols) or die "too many values: got " . ($elem_count + $val_count);
			for (my $i = 0; $i < $val_count; $i++)
			{
#				print (($elem_count + $i) . " " . $vals[$i] . "\n");  # xxxx
				print OUT pack("v", $vals[$i] + 0);
			}
			
			$elem_count += $val_count;
			if ($elem_count < $cols)
			{
				# Read another record...
				read(IN, $elevations, 1024) == 1024 or die "error reading record (column $col, row $elem_count): $!";

#				print $elevations . "\n";  #xxx
#				$_ = <STDIN>;  #xxx
			}

		} while ($elem_count < $cols);
	}

	close IN;
	close OUT;
}


main();
exit(0);

