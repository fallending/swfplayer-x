#!/usr/bin/perl -w

#$year = 2003;
#$year = 2004;
$year = 2005;


for ($month = 1; $month <= 12; $month++)
{
	$src = `cal -1 $month $year`;

	@src_lines = split("\n", $src);

	($month_name) = $src_lines[0] =~ /(\w+)/;

	print "\t{ \"$month_name\", $year, {\n";

	# Extract days.
	my $row_count = 0;
	for ($i = 2; $i < scalar(@src_lines); $i++)
	{
		$row_count++;

		@days = unpack("a2xa2xa2xa2xa2xa2xa2", $src_lines[$i] . "                  ");

		print("\t\t{");
		for ($j = 0; $j < scalar(@days); $j++)
		{
			my $day = $days[$j];
			if ($day eq '  ') { $day = ' 0'; }
			print("$day,");
		}
		print("},\n");
	}
	while ($row_count < 6)
	{
		$row_count++;
		print("\t\t{ 0, 0, 0, 0, 0, 0, 0,},\n");
	}

	print("\t}},\n");
}
