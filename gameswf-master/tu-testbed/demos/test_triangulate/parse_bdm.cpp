// parse_bdm.cpp	-- Thatcher Ulrich <http://tulrich.com> 2007

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Parse a simple text format representing 2D polygon data.

#include "demos/test_triangulate/parse_bdm.h"

static bool	is_all_whitespace(const char* str)
// Return true if the given string is all whitespace.
{
	while (int c = *str++)
	{
		if (c == '\n'
		    || c == ' '
		    || c == '\r'
		    || c == '\t')
		{
			// Whitespace.
		}
		else
		{
			// Non-whitespace.
			return false;
		}
	}

	return true;
}

void parse_bdm(FILE* fp, array< array< float > >* paths)
{
	int verts_left_in_path = 0;
	int line_number = 0;
	for (;;) {
		char line[80];
		if (fgets(line, 80, fp) == NULL) {
			// EOF.
			break;
		}
		line_number++;

		if (line[0] == '#'
		    || is_all_whitespace(line)
		    || strncmp(line, "begin", 5) == 0
		    || strncmp(line, "end", 3) == 0
		    || strncmp(line, "type", 4) == 0) {
			// comment or other irrelevant input
			continue;
		} else if (verts_left_in_path == 0) {
			verts_left_in_path = atoi(line);
			if (verts_left_in_path > 0) {
				paths->resize(paths->size() + 1);
			} else {
				verts_left_in_path = 0;
			}
		} else {
			assert(verts_left_in_path > 0);

			float x = 0, y = 0;
			int coord_count = sscanf(line, "%f %f", &x, &y);
			if (coord_count != 2) {
				fprintf(stderr, "parse_bdm error: can't read 2 coordinates at line %d.\n", line_number);
				exit(1);
			}

			// Add another point to the current path.
			paths->back().push_back(x);
			paths->back().push_back(y);

			verts_left_in_path--;
		}
	}
	if (verts_left_in_path) {
		fprintf(stderr, "parse_bdm error: expected %d more vertices in path\n", verts_left_in_path);
		exit(1);
	}
}
