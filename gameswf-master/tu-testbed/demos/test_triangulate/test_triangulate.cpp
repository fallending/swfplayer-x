// test_triangulate.cpp	-- Thatcher Ulrich <http://tulrich.com> 2004

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Test program for tu_testbed triangulation code.


#include "base/triangulate.h"
#include "base/ear_clip_triangulate.h"
#include "base/postscript.h"
#include "base/tu_file.h"
#include "base/ogl.h"
#include "base/demo.h"
#include "net/http_server.h"
#include "net/webtweaker.h"
#include <float.h>
#include "SDL.h"


void	compute_bounding_box(float* x0, float* y0, float* x1, float* y1, const array<float>& coord_list)
// Compute bounding box around given coord_list of (x,y) pairs.
{
	assert((coord_list.size() & 1) == 0);

	*x0 = *y0 = FLT_MAX;
	*x1 = *y1 = -FLT_MAX;

	for (int i = 0; i < coord_list.size(); )
	{
		if (coord_list[i] > *x1) *x1 = coord_list[i];
		if (coord_list[i] < *x0) *x0 = coord_list[i];
		i++;
		if (coord_list[i] > *y1) *y1 = coord_list[i];
		if (coord_list[i] < *y0) *y0 = coord_list[i];
		i++;
	}
}


void render_diagram(array<float>& trilist, const array<array<float> >& input_paths, const array<float>& debug_path,
		    bool flip_vertical, bool show_direction, bool show_verts)
// Render an OpenGL diagram of the given info.  trilist is the output
// of the triangulation algo; input_paths is the input.  debug_path is
// the incremental state of the path being tesselated, if the
// tesselation has been stopped in advance.
{
	int	tricount = trilist.size() / 6;
	int	trilabel = 0;

	// TODO: highlight one of the tris, by number
	
	// Draw the triangle mesh.
	assert(trilist.size() % 6 == 0);
	glColor3f(0, 0, 0.5f);  // blue
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < trilist.size(); i += 6)
	{
		glVertex2f(trilist[i + 0], trilist[i + 1]);
		glVertex2f(trilist[i + 2], trilist[i + 3]);
		glVertex2f(trilist[i + 4], trilist[i + 5]);
	}
	glEnd();

	// Draw the internal edges.
	glColor3f(0, 0.25f, 0);  // green
	glBegin(GL_LINES);
	{for (int i = 0; i < trilist.size(); i += 6)
	{
		glVertex2f(trilist[i + 0], trilist[i + 1]);
		glVertex2f(trilist[i + 2], trilist[i + 3]);
		
		glVertex2f(trilist[i + 2], trilist[i + 3]);
		glVertex2f(trilist[i + 4], trilist[i + 5]);

		glVertex2f(trilist[i + 4], trilist[i + 5]);
		glVertex2f(trilist[i + 0], trilist[i + 1]);
	}}
	glEnd();

	// Draw the input paths.

	glColor3f(0.25f, 0, 0);  // red
	{for (int j = 0; j < input_paths.size(); j++) {
		const array<float>& path = input_paths[j];
		assert((path.size() & 1) == 0);
		if (path.size() == 0) {
			continue;
		}
		glBegin(GL_LINE_STRIP);
		if (flip_vertical) {
			for (int i = 0; i < path.size(); i += 2) {
				glVertex2f(path[i], -path[i + 1]);
			}
			glVertex2f(path[0], -path[1]);
		} else {
			for (int i = 0; i < path.size(); i += 2) {
				glVertex2f(path[i], path[i + 1]);
			}
			glVertex2f(path[0], path[1]);
		}
		glEnd();
	}}

	if (show_direction || show_verts) {
		float x0 = FLT_MAX;
		float x1 = -FLT_MAX;
		float y0 = FLT_MAX;
		float y1 = -FLT_MAX;
		for (int j = 0; j < input_paths.size(); j++) {
			const array<float>& path = input_paths[j];
			for (int i = 0; i < path.size(); i += 2) {
				if (path[i] < x0) x0 = path[i];
				if (path[i] > x1) x1 = path[i];
				if (path[i + 1] < y0) y0 = path[i + 1];
				if (path[i + 1] > y1) y1 = path[i + 1];
			}
		}

		if (show_direction) {
			float arrow_size = (x1 - x0) / 200.0f;
			glColor3f(0.5f, 0, 0);  // red
			glBegin(GL_LINES);
			for (int j = 0; j < input_paths.size(); j++) {
				const array<float>& path = input_paths[j];
				for (int i = 0; i < path.size(); i += 2) {
					float x0 = path[i];
					float y0 = path[i + 1];
					float x1, y1;
					if (i + 2 >= path.size()) {
						x1 = path[0];
						y1 = path[1];
					} else {
						x1 = path[i + 2];
						y1 = path[i + 3];
					}
					if (flip_vertical) {
						y0 = -y0;
						y1 = -y1;
					}
					float dx = x1 - x0;
					float dy = y1 - y0;
					float mag = sqrtf(dx * dx + dy * dy);
					if (mag > 0) {
						float rmag = 1.0f / mag;
						float xmid = (x1 - x0) * 2 / 3 + x0;
						float ymid = (y1 - y0) * 2 / 3 + y0;
						float ax = (dy - dx) * rmag * arrow_size;
						float ay = (-dx - dy) * rmag * arrow_size;
						glVertex2f(xmid, ymid);
						glVertex2f(xmid + ax, ymid + ay);
					}
				}
			}
			glEnd();
		}
	}

	// Draw the debug path.
	glColor3f(0.25f, 0.25f, 0.25f);
	assert((debug_path.size() & 1) == 0);
	if (debug_path.size() > 0) {
		glBegin(GL_LINES);
		for (int i = 0; i < debug_path.size(); i += 2) {
			glVertex2f(debug_path[i], debug_path[i + 1]);
		}
		glEnd();
	}
}


void	output_diagram(array<float>& trilist, const array<array<float> >& input_paths)
// Emit a Postscript diagram of the given info.  trilist is the output
// of the triangulation algo; input_paths is the input.
{
	// Bounding box, so we know how to scale the output.
	float	x0, y0, x1, y1;
	compute_bounding_box(&x0, &y0, &x1, &y1, trilist);

	// Compute a uniform scale that makes a nice-sized diagram.
	const float	DIMENSION = 72 * 7.5f;	// 72 Postscript units per inch
	const float	OFFSET = 36;

	float	scale = 1.0f;
	if (x1 - x0 > 0)
	{
		float	xscale = DIMENSION / (x1 - x0);
		scale = xscale;
	}
	if (y1 - y0 > 0)
	{
		float	yscale = DIMENSION / (y1 - y0);
		if (yscale < scale)
		{
			scale = yscale;
		}
	}

	// Setup postscript output.
	tu_file	out("output.ps", "wb");
	if (out.get_error())
	{
		fprintf(stderr, "can't open output.ps, error = %d\n", out.get_error());
		exit(1);
	}

	postscript	ps(&out, "Triangulation test diagram", true /* encapsulated */);

	int	tricount = trilist.size() / 6;
	int	trilabel = 0;

#define DRAW_AS_MESH
//#define DRAW_AS_POLY

#ifdef DRAW_AS_MESH
	for (int i = 0; i < trilist.size(); )
	{
		float	x[3], y[3];
		x[0] = trilist[i++];
		y[0] = trilist[i++];
		x[1] = trilist[i++];
		y[1] = trilist[i++];
		x[2] = trilist[i++];
		y[2] = trilist[i++];

		#define TRANSFORM(x, y)  (((x) - x0) * scale) + OFFSET, (((y) - y0) * scale) + OFFSET

//#define WIREFRAME
#ifdef WIREFRAME
		ps.gray(0);
		ps.line(TRANSFORM(x[0], y[0]), TRANSFORM(x[1], y[1]));
		ps.line(TRANSFORM(x[1], y[1]), TRANSFORM(x[2], y[2]));
		ps.line(TRANSFORM(x[2], y[2]), TRANSFORM(x[0], y[0]));
#else	// not WIREFRAME
		ps.gray(((i/6) / float(tricount) + 0.25f) * 0.5f);
		ps.moveto(TRANSFORM(x[0], y[0]));
		ps.lineto(TRANSFORM(x[1], y[1]));
		ps.lineto(TRANSFORM(x[2], y[2]));
		ps.fill();
#endif // not WIREFRAME

#if 0
		// Show verts: this should be the apex of the original ear
		ps.disk(TRANSFORM(x[1], y[1]), 2);
#endif // 0

#if 0
		// Label the tri.
		ps.gray(0.9f);
		float	cent_x = (x[0] + x[1] + x[2]) / 3;
		float	cent_y = (y[0] + y[1] + y[2]) / 3;
		ps.printf(TRANSFORM(cent_x, cent_y), "%d", trilabel);
		trilabel++;

		ps.line(TRANSFORM(cent_x, cent_y), TRANSFORM(x[1], y[1]));	// line to the ear
#endif // 0

	}
#endif // DRAW_AS_MESH

#ifdef DRAW_AS_POLY
	ps.gray(0);
	// xxxx debug only, draw the coords as if they form a poly.
	for (int i = 0; i < trilist.size() - 2; i += 2)
	{
		#define TRANSFORM(x, y)  (((x) - x0) * scale) + OFFSET, (((y) - y0) * scale) + OFFSET
		float	xv0 = trilist[i];
		float	yv0 = trilist[i + 1];
		float	xv1 = trilist[i + 2];
		float	yv1 = trilist[i + 3];
		ps.line(TRANSFORM(xv0, yv0), TRANSFORM(xv1, yv1));
	}
#endif // DRAW_AS_POLY
}


void	make_star(array<float>* out, float inner_radius, float outer_radius, int points)
// Generate a star-shaped polygon.  Point coordinates go into *out.
{
	assert(points > 2);
	assert(out);

	for (int i = 0; i < points; i++)
	{
		float	angle = (i * 2) / float(points * 2) * 2 * float(M_PI);
		out->push_back(cosf(angle) * outer_radius);
		out->push_back(sinf(angle) * outer_radius);

		angle = (i * 2 + 1) / float(points * 2) * 2 * float(M_PI);
		out->push_back(cosf(angle) * inner_radius);
		out->push_back(sinf(angle) * inner_radius);
	}
}


void	make_square(array<float>* out, float size)
// Generate a square with the given side size.
{
	assert(out);

	float	radius = size / 2;

	out->push_back(-radius);
	out->push_back(-radius);
	out->push_back( radius);
	out->push_back(-radius);
	out->push_back( radius);
	out->push_back( radius);
	out->push_back(-radius);
	out->push_back( radius);
}


void	make_spiral(array<float>* out, float width, float radians)
// Generate a spiral.
{
	assert(out);

	float	angle = 1;
	
	while (angle < radians)
	{
		float	radius = width * angle / 2 / float(M_PI);

		out->push_back(radius * cosf(angle));
		out->push_back(radius * sinf(angle));

		if (radius < 6)
		{
			angle += 0.2f;
		}
		else
		{
			float	step = 1.0f / radius;
			angle += step;
		}
	}

	// spiral back in towards the center.
	for (int i = 0, n = out->size(); i < n; i += 2)
	{
		float	x = (*out)[(n - 2) - i];
		float	y = (*out)[(n - 2) - i + 1];

		float	dist = sqrt(x * x + y * y);
		float	thickness = width / 5;
		if (dist > thickness)
		{
			out->push_back(x * (dist - width / 5) / dist);
			out->push_back(y * (dist - width / 5) / dist);
		}
		else
		{
			out->push_back(x * 0.8f);
			out->push_back(y * 0.8f);
		}
	}
}


void	reverse_path(array<float>* path)
// Reverse the order of the path coords (i.e. for turning a poly into
// an island).
{
	assert((path->size() & 1) == 0);

	int	coord_ct = path->size() >> 1;
	int	ct = coord_ct >> 1;

	for (int i = 0; i < path->size() >> 1; i += 2)
	{
		swap(&(*path)[i], &(*path)[path->size() - 2 - i]);	// x coord
		swap(&(*path)[i + 1], &(*path)[path->size() - 1 - i]);	// y coord
	}
}


void	offset_path(array<float>* path, float x, float y)
// Offset the given path coords.
{
	assert((path->size() & 1) == 0);

	for (int i = 0; i < path->size(); i += 2)
	{
		(*path)[i] += x;
		(*path)[i + 1] += y;
	}
}


void	rotate_coord_order(array<float>* path, int rotate_count)
// Reorder the verts, but preserve the exact same poly shape.
//
// rotate_count specifies how 
{
	assert(rotate_count >= 0);
	assert((path->size() & 1) == 0);

	if (path->size() == 0)
	{
		return;
	}

	// Stupid impl...
	while (rotate_count-- > 0)
	{
		// shift coordinates down, rotate first coord to end of array.
		float	tempx = (*path)[0];
		float	tempy = (*path)[1];

		int	i;
		for (i = 0; i < path->size() - 2; i += 2)
		{
			(*path)[i] = (*path)[i + 2];
			(*path)[i + 1] = (*path)[i + 3];
		}
		(*path)[i] = tempx;
		(*path)[i + 1] = tempy;
	}
}


void	set_to_array(array<float>* path, int array_size, const float array_data[])
{
	assert(path->size() == 0);
	assert((array_size & 1) == 0);

	path->reserve(array_size);

	for (int i = 0; i < array_size; i++)
	{
		path->push_back(array_data[i]);
	}
}


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


void generate_test_shape(int shape_number, array<array<float> >* paths_out)
// Fill *paths_out with one of the test shapes.
{
	assert(paths_out);
	
	// Use a reference, slightly more concise.
	array<array<float> >& paths = *paths_out;

	switch (shape_number) {
	default:
		fprintf(stderr, "Invalid test shape number\n");
		exit(1);
		break;

	case 0:
		// Make a square.
		paths.resize(paths.size() + 1);
		make_square(&paths.back(), 1100);

		// Another square.
		paths.resize(paths.size() + 1);
		make_square(&paths.back(), 1100);
		offset_path(&paths.back(), 1200, 100);

		break;

	case 1:
		// Make a square.
		paths.resize(paths.size() + 1);
		make_square(&paths.back(), 1100);

		// Make a little square inside.
		paths.resize(paths.size() + 1);
		make_square(&paths.back(), 100);
		reverse_path(&paths.back());

		// Make a little star island.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 50, 250, 3);
		offset_path(&paths.back(), -300, -300);
		reverse_path(&paths.back());

		// Make a circle.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 200, 200, 20);
		offset_path(&paths.back(), 300, 300);
		reverse_path(&paths.back());

		// Make a circle inside the circle.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 50, 50, 5);
		offset_path(&paths.back(), 300, 300);

		// Make a circle outside the big square.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 50, 50, 8);
		offset_path(&paths.back(), 1200, 300);

		break;

	case 2:
	{
		// Lots of circles.

		// @@ set this to 100 for a good performance torture test of bridge-finding.
		const int	TEST_DIM = 40;	// 20, 30
		{for (int x = 0; x < TEST_DIM; x++)
		{
			for (int y = 0; y < TEST_DIM; y++)
			{
				paths.resize(paths.size() + 1);
				make_star(&paths.back(), 10, 10, 10);	// (... 9, 9, 10)
				offset_path(&paths.back(), float(x) * 20, float(y) * 20);
			}
		}}

		// 2005-1-1  TEST_DIM=40, join poly = 4.9  s (ouch!)
		// 2005-1-2                         = 6.3  s (with grid_index_box) (double ouch!  index makes things slower!  what's wrong?)
		//                                           (what's wrong is that the vert remap on the whole edge index gets expensive!)
		// 2005-1-2                         = 2.94 s (optimize dupe_verts!  cull grid update (big win), more efficient vert insertion (win))

		// 2006-7-31 TEST_DIM=40, join poly = .087 s (new ear-clip code) (new code wins big!  On a laptop even!  (IBM T41p, 1.7GHz) but note, it's sint16)
		//                                  = .097 s (as above, but with float coords, still a huge win)
		//                                  = .231 s (old FIST-like imple, same machine as above)
		//
		// 2006-7-31: Including clipping time, the new code is
		// about twice as fast as the old code on the same
		// machine/compiler/etc.  I'm not really sure why;
		// they're doing roughly the same thing.  Actually the
		// new code works a little harder per-vert since it
		// analyzes coincident verts.  The big differences are
		// that the new code proceeds in sort order (i.e. it
		// scans the vertex list instead of traveling around
		// the poly loop), and doesn't mess with any pseudo
		// priority queue.  The sort order thing may help with
		// cache coherency.  Though I don't think the old code
		// had a big problem there.  The new code is a bit
		// simpler in many other small ways, like it never
		// bothers to update the reflex point index, once it's
		// built.
		break;
	}

	case 3:
	{
		// Lots of concentric circles.
		static int	CIRCLE_COUNT = 10;	// CIRCLE_COUNT >= 10 is a good performance test.
		{for (int i = 0; i < CIRCLE_COUNT * 2 + 1; i++)
		{
			paths.resize(paths.size() + 1);
			make_star(&paths.back(), 2 + float(i), 2 + float(i), 10 + i * 6);
			if (i & 1) reverse_path(&paths.back());
		}}

		break;
	}

	case 4:
		// test some degenerates.
		paths.resize(paths.size() + 1);
		paths.back().push_back(0);
		paths.back().push_back(0);
		paths.back().push_back(100);
		paths.back().push_back(-50);
		paths.back().push_back(120);
		paths.back().push_back(0);
		paths.back().push_back(100);
		paths.back().push_back(50);
		paths.back().push_back(0);
		paths.back().push_back(0);
		paths.back().push_back(-100);
		paths.back().push_back(-100);
		rotate_coord_order(&paths.back(), 5);
		break;

	case 5:
		// Make a star.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 2300, 3000, 20);

		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 1100, 2200, 20);
		reverse_path(&paths.back());

		paths.resize(paths.size() + 1);
		make_star(&paths.back(),  800, 1800, 20);

		// Make a star island.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 100, 500, 3);
		reverse_path(&paths.back());
		break;

	case 6:
		// This one has tricky coincident verts, good test case for
		// some code paths.

		// Stars with touching verts on different paths.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 2300, 3000, 20);

		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 2300, 1100, 20);
		reverse_path(&paths.back());

		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 800, 1100, 20);

		// Make a star island.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 100, 300, 3);
		reverse_path(&paths.back());
		break;

	case 7:
		// A direct, simplified expression of the "tricky triple dupe".
		//
		// The tricky duped vert is at (2,2).
		paths.resize(paths.size() + 1);

		paths.back().push_back(0);
		paths.back().push_back(0);

		paths.back().push_back(3);
		paths.back().push_back(0);

		paths.back().push_back(3);
		paths.back().push_back(2);

		paths.back().push_back(2);
		paths.back().push_back(2);

		paths.back().push_back(0);
		paths.back().push_back(2);

		paths.back().push_back(0);
		paths.back().push_back(3);

		paths.back().push_back(1);
		paths.back().push_back(3);

		paths.back().push_back(2);
		paths.back().push_back(2);

		paths.back().push_back(2.5f);
		paths.back().push_back(0.5f);

		paths.back().push_back(1.5f);
		paths.back().push_back(0.5f);

		paths.back().push_back(2);
		paths.back().push_back(2);

		paths.back().push_back(1);
		paths.back().push_back(3);

		paths.back().push_back(0);
		paths.back().push_back(3);

		paths.back().push_back(0);
		paths.back().push_back(2);
		break;

	case 8:
	{
		// Another direct expression of the "tricky triple dupe".  The
		// actual triple dupe occurs when P2 is attached with a bridge
		// to P (so it depends on the bridge-finding code).
		//
		// The tricky duped vert is at (2,2).
		paths.resize(paths.size() + 1);
		static const float	P[] =
		{
			0,0,  3,0,  3,2,  2,2,  1.5f,1.5f,  1.5f,2.5f,  2,2,  2,3,  0,3
		};
		set_to_array(&paths.back(), sizeof(P)/sizeof(P[0]), P);

		paths.resize(paths.size() + 1);
		static const float	P2[] =
		{
			2.1f,1,  2.3f,1.5f, 2.5f,1
		};
		set_to_array(&paths.back(), sizeof(P2)/sizeof(P2[0]), P2);
		break;
	}

	case 9:
	{
		// As above, pre-combined into one path with a zero-area bridge.
		//
		// The tricky duped vert is at (2,2).
		//
		// Note: this path contains a twist!  So in a sense, it is
		// invalid input.
		paths.resize(paths.size() + 1);
		static const float	P[] =
		{
			0,0,  3,0,  3,2,  2,2,  1.5f,1.5f,  1.5f,2.5f,  2,2,  2.1f,1,  2.3f,1.5f,  2.5f,1,  2.1f,1,  2,2,  2,3,  0,3
		};
		set_to_array(&paths.back(), sizeof(P)/sizeof(P[0]), P);
		break;
	}
	case 10:
		// Spiral.
		//
		// Set radians (3rd arg) to ~100 for a good performance test
		// of poly clipping.
		paths.resize(paths.size() + 1);
		make_spiral(&paths.back(), 10, 20);

		// 2004-12-31 radians = 120, time = 11.15 s
		// 2004-12-31 radians = 120, time =  0.072 s (with grid index, and localized ear clipping)
		// 2004-12-31 radians = 120, time =  0.115 s (as above, with rotate_coord_order(,20) -- speed is quite sensitive to initial m_loop!)
		break;

	case 11:
		// Big star.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 100, 1000, 256);
		break;

	case 12:
	{
		// Holes wanting to join to main loop at the same point.
		paths.resize(paths.size() + 1);
		static const float	P[] =
		{
			0, 0,	3, 0,	0, 3
		};
		set_to_array(&paths.back(), sizeof(P)/sizeof(P[0]), P);
	
		paths.resize(paths.size() + 1);
		static const float	P1[] =
		{
			0.5f, 0.7f,	1.0f, 0.8f,	1.5f, 0.7f
		};
		set_to_array(&paths.back(), sizeof(P1)/sizeof(P1[0]), P1);
	
		paths.resize(paths.size() + 1);
		static const float	P2[] =
		{
			0.5f, 0.5f,	1.0f, 0.6f,	1.5f, 0.5f
		};
		set_to_array(&paths.back(), sizeof(P2)/sizeof(P2[0]), P2);
		break;
	}

	case 13:
	{
		paths.resize(paths.size() + 1);
		static const float P[] =
		{
			0,0,
			1,1,
			3,2,
			0,2,
			1,1,
			0,0,
			1,-1,
			0,-2,
			3,-2,
			1,-1,
			0,0,
		};
		set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		break;
	}
	
	case 14:
	{
		// Coincident edges -- can't determine sidedness
		// locally, so may foul up ear detection.

		paths.resize(paths.size() + 1);
		static const float P[] =
		{
			0,0,
			1,1,
			-1,2,
			-1,-2,
			1,-1,
			0,0,
		};
		set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);

		paths.resize(paths.size() + 1);
		static const float P2[] =
		{
			0,0,
			1,-1,
			3,-2,
			3,2,
			1,1,
			0,0,
		};
		set_to_array(&paths.back(), TU_ARRAYSIZE(P2), P2);
		
		break;
	}

	case 15: {
		// "fish shape" -- coincident edges forming an ear in
		// the fish "tail", but shouldn't be clipped.
		paths.resize(paths.size() + 1);
		static const float P[] =
		{
			-100,100,
			0,0,
			-100,-100,
			0,0,
			100,-100,
			200,0,
			100,100,
			0,0,
			-100,100,
		};
		set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		//rotate_coord_order(&paths.back(), 5);
		break;
	}

	case 16: {
		// checkerboard -- positive square w/ holes
		{
			paths.resize(paths.size() + 1);
			static const float P[] = {
				0, 0,
				1, 0,
				2, 0,
				2, 1,
				2, 2,
				1, 2,
				0, 2,
				0, 1,
			};
			set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		}
		{
			paths.resize(paths.size() + 1);
			static const float P[] = {
				0, 0,
				0, 1,
				1, 1,
				1, 0,
			};
			set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		}
		{
			paths.resize(paths.size() + 1);
			static const float P[] = {
				1, 1,
				1, 2,
				2, 2,
				2, 1,
			};
			set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		}
		break;
	}

	case 17: {
		// Star-like things w/ degeneracies.
		{
			paths.resize(paths.size() + 1);
			static const float P[] = {
				 0,  0,
				 1,  1,
				 2,  1,
				 1,  2,
				 1,  1,
				 0,  0,
				-1,  1,
				-1,  2,
				-2,  1,
				-1,  1,
				 0,  0,
				-1, -1,
				-2, -1,
				-1, -2,
				-1, -1,
				 0,  0,
				 1, -1,
				 1, -2,
				 2, -1,
				 1, -1,
			};
			set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		}
		{
			paths.resize(paths.size() + 1);
			static const float P[] = {
				3, 3,
				4, 3,
				4, 2,
				4, 3,
				5, 2,
				5, 4,
				4, 3,
				4.5, 4,
				3.5, 4,
				4, 3,
			};
			set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		}
		break;
	}

	case 18: {
		// Figure eight like shape -- contains a crossing.
		{
			paths.resize(paths.size() + 1);
			static const float P[] = {
				 0,  0,
				 1,  0,
				 2,  1,  // crossing point
				 3,  2,
				 4,  2,
				 3.5, 1,
				 4,  0,
				 3,  0,
				 2,  1,  // crossing point
				 1,  2,
				 0,  2,
				 0.5, 1,
			};
			set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		}
		{
			paths.resize(paths.size() + 1);
			static const float P[] = {
				4, 3,
				5, 3,
				5, 2,
				5, 3,
				6, 2,
				6, 4,
				5, 3,
				5.5, 4,
				4.5, 4,
				//5, 3,
			};
			set_to_array(&paths.back(), TU_ARRAYSIZE(P), P);
		}
		break;
	}
		
	case 19: {
		// Large spiral.
		paths.resize(paths.size() + 1);
		make_spiral(&paths.back(), 10, 220);
		break;
	}
	
	case 20: {
		// Large star.
		paths.resize(paths.size() + 1);
		make_star(&paths.back(), 100, 1000, 10000);
		break;
	}
	
	case 21: {
		// Lots of circles.
		const int	TEST_DIM = 100;
		{for (int x = 0; x < TEST_DIM; x++)
		{
			for (int y = 0; y < TEST_DIM; y++)
			{
				paths.resize(paths.size() + 1);
				make_star(&paths.back(), 10, 10, 10);	// (... 9, 9, 10)
				offset_path(&paths.back(), float(x) * 20, float(y) * 20);
			}
		}}

		break;
	}
	
	} // end switch
}


// Wrapper around constrained triangulator.  Basically converts floats to sint16.
void do_constrained_triangulate_sint16(array<float>* result, int paths_size, array<float> paths[],
				int debug_halt_tri, array<float>* debug_path)
{
	// Adapt input.
	int vert_count = 0;
	float min_x = FLT_MAX, max_x = -FLT_MAX, min_y = FLT_MAX, max_y = -FLT_MAX;

	for (int i = 0; i < paths_size; i++) {
		for (int j = 0; j < paths[i].size(); j += 2) {
			vert_count++;
			float x = paths[i][j];
			float y = paths[i][j + 1];
			if (x < min_x) min_x = x;
			if (x > max_x) max_x = x;
			if (y < min_y) min_y = y;
			if (y > max_y) max_y = y;
		}
	}

	if (vert_count == 0 || min_x == max_x || min_y == max_y) {
		return;
	}

	//const int SCALE = 16384;
	const int SCALE = 65533;
	const int OFFSET = -32767;
	
	array< array<sint16> > spaths;
	{for (int i = 0; i < paths_size; i++) {
		spaths.resize(spaths.size() + 1);
		array<sint16>* p = &spaths.back();
		for (int j = 0; j < paths[i].size(); j += 2) {
			float x = paths[i][j];
			float y = paths[i][j + 1];
			p->push_back(sint16(OFFSET + SCALE * ((x - min_x) / (max_x - min_x))));
			p->push_back(sint16(OFFSET + SCALE * ((y - min_y) / (max_y - min_y))));
		}
	}}

	// Triangulate.
	array<sint16> sresult;
	array<sint16> sdebug_path;
	ear_clip_triangulate::compute(&sresult, paths_size, &spaths[0], debug_halt_tri, &sdebug_path);
	
	// Adapt output.
	{for (int j = 0; j < sresult.size(); j += 2) {
		float x = min_x + ((sresult[j] - float(OFFSET)) / float(SCALE)) * (max_x - min_x);
		float y = min_y + ((sresult[j + 1] - float(OFFSET)) / float(SCALE)) * (max_y - min_y);
		result->push_back(x);
		result->push_back(y);
	}}
	{for (int j = 0; j < sdebug_path.size(); j += 2) {
		float x = min_x + ((sdebug_path[j] - float(OFFSET)) / float(SCALE)) * (max_x - min_x);
		float y = min_y + ((sdebug_path[j + 1] - float(OFFSET)) / float(SCALE)) * (max_y - min_y);
		debug_path->push_back(x);
		debug_path->push_back(y);
	}}
}


void flip_paths_vertically(array<array<float> >* paths)
{
	for (int i = 0; i < paths->size(); i++) {
		for (int j = 1; j < (*paths)[i].size(); j += 2) {
			(*paths)[i][j] = -(*paths)[i][j];
		}
	}
}


void reverse_paths(array<array<float> >* paths)
{
	for (int i = 0; i < paths->size(); i++) {
		reverse_path(&(*paths)[i]);
	}
}



#undef main	// SDL wackiness
int	main(int argc, const char** argv)
{
	array<float> result;
	array<array<float> > paths;
	array<float> debug_path;
	bool use_constrained = false;
	int debug_halt_tri = 0;
	int shape_number = -1;
	bool dump_bdm_output = false;
	tu_string vert_type("float");
	bool reverse_input_paths = false;
	bool flip_vertical = false;
	
	for (int arg = 1; arg < argc; arg++) {
		if (strcmp(argv[arg], "-i") == 0)
		{
			// Take input from stdin, in "BDM" format.
			// Vert count on a line, followed by verts,
			// one per line.  '#' starts a comment.
			// Ignore blank lines.
			bool	start_new_path = true;
			int verts_left_in_path = 0;
			int line_number = 0;
			for (;;)
			{
				char	line[80];
				if (fgets(line, 80, stdin) == NULL)
				{
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
						paths.resize(paths.size() + 1);
					} else {
						verts_left_in_path = 0;
					}
				} else {
					assert(verts_left_in_path > 0);
					
					float	x = 0, y = 0;
					int	coord_count = sscanf(line, "%f %f", &x, &y);
					if (coord_count != 2)
					{
						fprintf(stderr, "invalid input format.\n");
						exit(1);
					}

					// Add another point to the current path.
					paths.back().push_back(x);
					paths.back().push_back(y);

					verts_left_in_path--;
				}
			}
		} else if (strcmp(argv[arg], "-n") == 0) {
			arg++;
			if (arg < argc) {
				shape_number = atoi(argv[arg]);
				generate_test_shape(shape_number, &paths);
			}
		} else if (strcmp(argv[arg], "-c") == 0) {
			// Use the constrained triangulator algo.
			use_constrained = true;
		} else if (strcmp(argv[arg], "-d") == 0) {
			arg++;
			if (arg < argc) {
				debug_halt_tri = atoi(argv[arg]);
			}
		} else if (strcmp(argv[arg], "-o") == 0) {
			// Dump test shape to BDM output file.
			dump_bdm_output = true;
		} else if (strcmp(argv[arg], "-vt") == 0) {
			arg++;
			if (arg < argc) {
				vert_type = argv[arg];
				if (vert_type != "sint16" && vert_type != "float") {
					fprintf(stderr, "invalid vert type.  Try 'sint16' or 'float'.\n");
					exit(1);
				}
			}
		} else if (strcmp(argv[arg], "-rp") == 0) {
			reverse_input_paths = true;
		} else if (strcmp(argv[arg], "-fv") == 0) {
			flip_vertical = true;
		} else if (strcmp(argv[arg], "-break") == 0) {
			// Break into debugger.
			#ifdef _WIN32
			_asm { int 3 };
			#endif // _WIN32
		} else {
			fprintf(stderr, "unknown arg.  -i means take loop input from stdin.\n");
			exit(1);
		}
	}

	if (paths.size() == 0) {
		fprintf(stderr, "usage: test_triangulate [-c] [-break] [-d #] ([-n #] | [-i < inputloop.txt]) [-o] [-vt [float|sint16]]\n");
		exit(1);
	}

	if (dump_bdm_output) {
		for (int i = 0; i < paths.size(); i++) {
			printf("%d\n", paths[i].size() / 2);
			for (int j = 0; j < paths[i].size(); j += 2) {
				printf("%f %f\n", paths[i][j], paths[i][j + 1]);
			}
			printf("\n");
		}
		exit(0);
	}

	// Interactive display.
	int	width = 1000;
	int	height = 1000;
	demo::init_video(width, height, 16);

	bool do_triangulate = true;

	demo::nav2d_state nav_state;
	for (;;) {
		if (do_triangulate) {
			// Triangulate.
			do_triangulate = false;
			result.resize(0);
			debug_path.resize(0);

			// Preprocess if desired.
			array<array<float> >* input = &paths;
			array<array<float> > temp_paths;
			bool invert = reverse_input_paths;
			if (invert || flip_vertical) {
				temp_paths = paths;
				input = &temp_paths;

				if (flip_vertical) {
					invert = !invert;
					flip_paths_vertically(&temp_paths);
				}					

				if (invert) {
					reverse_paths(&temp_paths);
				}
			}

			
			if (use_constrained) {
				if (vert_type == "sint16") {
					do_constrained_triangulate_sint16(
						&result, input->size(), &(*input)[0], debug_halt_tri, &debug_path);
				} else {
					ear_clip_triangulate::compute(
						&result, input->size(), &(*input)[0], debug_halt_tri, &debug_path);
				}
			} else {
				triangulate::compute(&result, input->size(), &(*input)[0], debug_halt_tri, &debug_path);
			}
			assert((result.size() % 6) == 0);
		}
		
		if (demo::update_nav2d(&nav_state)) {
			// User wants to quit.
			break;
		}
		// Check key input.
		for (int i = 0, n = nav_state.m_keys.size(); i < n; i++) {
			int key = nav_state.m_keys[i].key;

			// Allow larger increments via shift/ctrl combos.
			int mod = nav_state.m_keys[i].modifier;
			static const int INCREMENT_TABLE[4] = { 1, 10, 100, 1000 };
			int increment = INCREMENT_TABLE[((mod & KMOD_SHIFT) ? 1 : 0) + ((mod & KMOD_CTRL) ? 2 : 0)];
			
			if (key == SDLK_LEFT) {
				debug_halt_tri -= increment;
				if (debug_halt_tri < -1) {
					debug_halt_tri = -1;
				}
				do_triangulate = true;
			} else if (key == SDLK_RIGHT) {	
				debug_halt_tri += increment;
				do_triangulate = true;
			} else if (key == SDLK_RIGHTBRACKET) {
				shape_number++;
				paths.resize(0);
				generate_test_shape(shape_number, &paths);
				do_triangulate = true;
			} else if (key == SDLK_LEFTBRACKET) {
				shape_number--;
				paths.resize(0);
				generate_test_shape(shape_number, &paths);
				do_triangulate = true;
			} else if (key == SDLK_r) {
				do_triangulate = true;
				reverse_input_paths = !reverse_input_paths;
			} else if (key == SDLK_v) {
				do_triangulate = true;
				flip_vertical = !flip_vertical;
			}
		}

		// Turn on alpha blending.
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

 		demo::set_nav2d_viewport(nav_state);

		glDisable(GL_DEPTH_TEST);	// Disable depth testing.
		glDrawBuffer(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT);

		render_diagram(result, paths, debug_path, flip_vertical, true, false);

		SDL_GL_SwapBuffers();

		SDL_Delay(10);
	}

//	output_diagram(result, paths);

	return 0;
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:

