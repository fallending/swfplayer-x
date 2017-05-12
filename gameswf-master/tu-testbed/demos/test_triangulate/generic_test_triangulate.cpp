// generic_test_triangulate.cpp	-- Thatcher Ulrich <http://tulrich.com> 2007

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Simplified test program for tu_testbed triangulation code, that
// takes file input and produces file output.  No UI or graphical
// hooks.


#include "base/tu_timer.h"
#include "base/ear_clip_triangulate.h"
#include "demos/test_triangulate/parse_bdm.h"

void print_usage()
{
	fprintf(stderr, "generic_test_triangulate -- a program to triangulate arbitrary\n");
	fprintf(stderr, "non-self-intersecting polygonal shapes.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "    generic_test_triangulate [-t] < input > output\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Input is in 'BDM' format.  BDM is a text file in ASCII format that defines\n");
	fprintf(stderr, "any number of closed polygonal paths.  Each path consists of a line containing\n");
	fprintf(stderr, "the vertex count, followed by that many lines containing the vertex coords.\n");
	fprintf(stderr, "Blank lines are ignored.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "The internal data format for coordinates is single-precision float.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "The output is a triangle list: one triangle per line, six coordinates per triangle.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "    -t    Prints to stderr the time taken by the triangulation operation\n");
	fprintf(stderr, "          (not counting file input and output).\n");
}

int main(int argc, const char** argv)
{
	array<float> result;
	array<array<float> > paths;
	bool do_timing = false;

	for (int arg = 1; arg < argc; arg++) {
		if (strcmp(argv[arg], "-t") == 0) {
			do_timing = true;
		} else {
			// Unknown arg.
			printf("Unknown arg '%s'\n", argv[arg]);
			print_usage();
			exit(1);
		}
	}

	// Take input from stdin, in "BDM" format.
	// Vert count on a line, followed by verts,
	// one per line.  '#' starts a comment.
	// Ignore blank lines.
	// Repeat for each path.
	parse_bdm(stdin, &paths);

	if (paths.size() == 0) {
		print_usage();
		exit(1);
	}

	// Do it.
	uint64 start_ticks = tu_timer::get_ticks();
	ear_clip_triangulate::compute(
		&result, paths.size(), &(paths[0]), -1, NULL);
	uint64 stop_ticks = tu_timer::get_ticks();

	if (do_timing) {
		fprintf(stderr, "output size = %d triangles, triangulated in %f seconds.\n",
			result.size() / 6,
			tu_timer::ticks_to_seconds(stop_ticks - start_ticks));
	}

	// Output the results.
	for (int i = 0; i < result.size(); i += 6) {
		printf("%f %f %f %f %f %f\n",
		       result[i + 0], result[i + 1],
		       result[i + 2], result[i + 3],
		       result[i + 4], result[i + 5]);
	}

	return 0;
}
