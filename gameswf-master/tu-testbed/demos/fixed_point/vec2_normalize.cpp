// vec2_normalize.cpp	-- Thatcher Ulrich <http://tulrich.com> 2005

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Experiments in normalizing fixed-point vectors.


#include "base/ogl.h"
#include "base/demo.h"
#include "base/tu_random.h"
#include "net/http_server.h"
#include "net/webtweaker.h"
#include <float.h>
#include "SDL.h"


// A simple 16.16 fixed-point type.
struct fixed
{
	fixed() : m_(0) {}
	fixed(int i) : m_(i << 16) {}
	fixed(float f) : m_(int32(f * 65536.0f)) {}
	fixed(double f) : m_(int32(f * 65536.0)) {}
	fixed(const fixed& f) : m_(f.m_) {}

	enum from_raw {
		FROM_RAW
	};
	fixed(from_raw fr, int i) : m_(i) {}

	const fixed& operator=(const fixed& f) { m_ = f.m_; return *this; }

	void from_raw(int i) {
		m_ = i;
	}

	int32 to_raw() const { return m_; }
	float to_float() const { return m_ / 65536.0f; }
	fixed abs() const { return m_ < 0 ? fixed(FROM_RAW, -m_) : fixed(FROM_RAW, m_); }

	bool operator<(const fixed& f) const { return m_ < f.m_; }

	const fixed operator>>(int ct) const { return fixed(FROM_RAW, m_ >> ct); }
	const fixed operator<<(int ct) const { return fixed(FROM_RAW, m_ << ct); }
	
	const fixed operator+(fixed f) const { return fixed(FROM_RAW, m_ + f.m_); }
	const fixed operator-(fixed f) const { return fixed(FROM_RAW, m_ - f.m_); }

	const fixed& operator>>=(int ct) { m_ >>= ct; return *this; }
	const fixed& operator+=(fixed f) { m_ += f.m_; return *this; }
	const fixed& operator-=(fixed f) { m_ -= f.m_; return *this; }
	
private:
	int32 m_;
};



fixed max(fixed a, fixed b)
{
	if (a < b) return b;
	else return a;
}


void gl_vertex(fixed x, fixed y)
{
	glVertex2f(x.to_float(), y.to_float());
}


template<int constant_rshift>
int32 rshift(int32 a) {
	if (constant_rshift < 0) {
		return a << (-constant_rshift);
	}
	return a >> (constant_rshift);
}


template<int pre_rshift>
const fixed mul(fixed a_in, fixed b_in) {
	int32 a = rshift<pre_rshift>(a_in.to_raw());
	int32 b = rshift<pre_rshift>(b_in.to_raw());
	int32 c = a * b;

	return fixed(fixed::FROM_RAW, rshift<16 - pre_rshift - pre_rshift>(c));
}


void draw_unit_circle()
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (float angle = 0; angle < 2 * M_PI + 0.1f; angle += 0.005f) {
		glVertex2f(cosf(angle), sinf(angle));
	}
	glEnd();
}


void draw_axes()
{
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex2f(-1, 0);
	glVertex2f(1, 0);
	glVertex2f(0, -1);
	glVertex2f(0, 1);
	glEnd();
}


void bezier_approximation(fixed* x, fixed* y, fixed t)
// Return an approximate unit vector whose angle depends on t.  t==0
// corresponds to (1,0), while t==1 corresponds to (0,1).  Uses a
// quadratic bezier approximation that at worst is within about 0.77%
// of a true unit vector.
{
	static const fixed c2(0.925f * 2.0f);
	
	fixed one_minus_t(fixed(1) - t);
	fixed c_term(mul<2>(c2, mul<2>(t, one_minus_t)));
	*x = mul<2>(one_minus_t, one_minus_t) + c_term;
	*y = c_term + mul<4>(t, t);
	
	// 4 multiplies
}


void draw_bezier_circle_approximation()
{
	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (float t = -0.1f; t < 1.1f; t += 0.005f) {
		fixed x, y;
		bezier_approximation(&x, &y, fixed(t));
		gl_vertex(x, y);
	}
	glEnd();
}


void find_best_c()
// Find a value of c for the quadratic bezier curve based on
// (1,0);(c,c);(0,1), that minimizes the distance to the unit
// quarter-circle.
{
	float a0[] = { 1, 0 };
	float a1[] = { 0, 1 };

	float best_c = 0;
	float best_c_error_percent = 100;
	for (float c = 0.90f; c < 1.1f; c += 0.0001f) {
		float worst_error_percent = 0;
		float worst_t = 0;
	
		for (float t = -0.1f; t < 1.1f; t += 0.005f) {
			float x = a0[0] * (1 - t) * (1 - t) + 2 * c * t * (1 - t) + a1[0] * t * t;
			float y = a0[1] * (1 - t) * (1 - t) + 2 * c * t * (1 - t) + a1[1] * t * t;

			float length = sqrtf(x*x + y*y);
			float percent_error = fabs(length * 100 - 100);
			if (t > 0 && t < 1 && percent_error > worst_error_percent) {
				worst_error_percent = percent_error;
				worst_t = t;
			}
		}

		if (worst_error_percent < best_c_error_percent) {
			best_c_error_percent = worst_error_percent;
			best_c = c;
		}
	}

	printf("best c error = %f%%, c = %f\n", best_c_error_percent, best_c);
}


void initial_prescale(float* x, float* y, float x_in, float y_in)
// Prescale (x,y) so it has the same direction as (x_in, y_in), but
// the max coord is >=1 and <2.
{
	// (In fixed point, this would be a shift by
	// count-leading-zeros.  In float, if we cared about
	// efficiency, we would just set the exponent to 128.)

	float m = fmax(x_in, y_in);
	int sig_bits = int(floorf(log2(m)));
	if (sig_bits > 0) {
		*x = x_in / (1 << sig_bits);
		*y = y_in / (1 << sig_bits);
	} else {
		*x = x_in * (1 << -sig_bits);
		*y = y_in * (1 << -sig_bits);
	}
}


int log2i(int value_in)
// Return the position of the most significant bit in value_in.
{
	int value = value_in;
	
	assert(value > 0);

	int msb = 0;
	value >>= 1;
	while (value) {
		msb++;
		value >>= 1;
	}

	assert((1 << msb) <= value_in);
	assert((2 << msb) > value_in);
	
	return msb;
}


void initial_prescale_fixed(fixed* x, fixed* y, fixed x_in, fixed y_in)
// Prescale (x,y) so it has the same direction as (x_in, y_in), but
// the max coord is >=1 and <2.  Convert to fixed-point.
{
	fixed m = max(x_in.abs(), y_in.abs());
	int sig_bits = log2i(m.to_raw()) - 16;

	if (sig_bits > 0) {
		*x = x_in >> sig_bits;
		*y = y_in >> sig_bits;
	} else {
		*x = x_in << -sig_bits;
		*y = y_in << -sig_bits;
	}

	assert(x->to_float() < 2.0f);
	assert(y->to_float() < 2.0f);
	assert(fabsf(x->to_float()) >= 1.0f || fabsf(y->to_float()) >= 1.0f);
}


void draw_x(float x, float y)
// Draw an X over the given point.
{
	glBegin(GL_LINES);
	glVertex2f(x - 0.05f, y - 0.05f);
	glVertex2f(x + 0.05f, y + 0.05f);
	glVertex2f(x - 0.05f, y + 0.05f);
	glVertex2f(x + 0.05f, y - 0.05f);
	glEnd();
}


void draw_vector_calculation1(float x_in, float y_in)
// Illustrate the calculation of the normal, using the bezier thing.
{
	// Force input into the positive quadrant.
	if (x_in < -0.1f) { x_in = -0.1f; }
	if (y_in < -0.1f) { y_in = -0.1f; }
	
	// draw the input
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(x_in, y_in);
	glEnd();
	
	// Prescale the input.
	fixed x_scaled, y_scaled;
	initial_prescale_fixed(&x_scaled, &y_scaled, x_in, y_in);

	// draw the input, prescaled.
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	gl_vertex(x_scaled, y_scaled);
	glEnd();

	// Map the input into 16 cases.
	int lookup = ((x_scaled.to_raw() >> 15) & 3) + (((y_scaled.to_raw() >> 15) & 3) << 2);
	static const struct {
		float K;
		float T0;
	} table[16] = {
		// TODO: crunch some numbers to optimize these values.
		{ 0.25f, 0.50f }, { 0.25f, 0.50f }, { 0.37f, 0.10f }, { 0.37f, 0.07f },
		{ 0.25f, 0.50f }, { 0.25f, 0.50f }, { 0.50f, 0.30f }, { 0.25f, 0.25f },
		{ 0.37f, 0.90f }, { 0.50f, 0.70f }, { 0.37f, 0.50f }, { 0.25f, 0.40f },
		{ 0.37f, 0.93f }, { 0.25f, 0.75f }, { 0.25f, 0.60f }, { 0.25f, 0.50f },
	};

	// Show the approximation square.
	glColor3f(0.5, 0, 0);
	glBegin(GL_LINE_STRIP);
	float lx = (lookup & 3) * 0.5f;
	float ly = (lookup >> 2) * 0.5f;
	glVertex2f(lx, ly);
	glVertex2f(lx + 0.5f, ly);
	glVertex2f(lx + 0.5f, ly + 0.5f);
	glVertex2f(lx, ly + 0.5f);
	glVertex2f(lx, ly);
	glEnd();
	
	glColor3f(1, 0, 1);
	glBegin(GL_LINE_STRIP);

	// Initial approximation.
	float radius = 0.5f;	// for display.
	fixed t(table[lookup].T0);
	fixed x, y;

	bezier_approximation(&x, &y, t);
	glVertex2f(x.to_float() * radius, y.to_float() * radius);
	radius += 0.2f;

	fixed K(table[lookup].K);
	
	// do some iterations.
	static const int ITERATIONS = 2;
	for (int i = 0; i < ITERATIONS; i++) {
		fixed error = mul<2>(x, y_scaled) - mul<2>(y, x_scaled);
		t += mul<4>(K, error);
		bezier_approximation(&x, &y, t);
		
		glVertex2f(x.to_float() * radius, y.to_float() * radius);
		radius += 0.2f;
	}
	glEnd();

	// Draw an X over the final result.
	glColor3f(1, 1, 1);
	draw_x(x.to_float(), y.to_float());

	// approx 4 + 6*ITERATIONS multiplies
}


void draw_vector_calculation2(float x_in, float y_in)
// Illustrate the calculation of the normal, using binary search for unit length.
{
	// Force input into the positive quadrant.
	if (x_in < -0.1f) { x_in = -0.1f; }
	if (y_in < -0.1f) { y_in = -0.1f; }
	
	// draw the input
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	glVertex2f(x_in, y_in);
	glEnd();
	
	// Prescale the input.
	fixed x_scaled, y_scaled;
	initial_prescale_fixed(&x_scaled, &y_scaled, x_in, y_in);

	// draw the input, prescaled.
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(0, 0);
	gl_vertex(x_scaled, y_scaled);
	glEnd();

	// Initial approximation.
	fixed x = x_scaled;
	fixed y = y_scaled;

	glColor3f(1, 1, 1);
	draw_x(x.to_float(), y.to_float());
	
	// do some iterations.
	static const int ITERATIONS = 8;
	fixed x_delta = x >> 1;
	fixed y_delta = y >> 1;
	for (int i = 0; i < ITERATIONS; i++) {
		fixed mag2 = mul<2>(x, x) + mul<2>(y, y);
		if (mag2.to_raw() > (1 << 16)) {
			x -= x_delta;
			y -= y_delta;
		} else {
			x += x_delta;
			y += y_delta;
		}
		x_delta >>= 1;
		y_delta >>= 1;

		if (i < ITERATIONS - 1) {
			glColor3f(0.25f, 0.25f, 0.25f);
		} else {
			glColor3f(1, 1, 1);
		}
		draw_x(x.to_float(), y.to_float());
	}
}


int nr_lookup_index(fixed mag2, int table_size)
// Given a vector magnitude squared value and a lookup table size,
// return the index into the table containing the best approximation
// of 1/sqrt(mag2).
//
// mag2 should be in [1, 8), so the most significant bit will
// be bit 18.
{
	return ((mag2.to_raw() >> (19 - log2i(table_size))) & (table_size - 1));
}


int nr_lookup_index_checked(fixed mag2, int table_size)
// As above, but with asserts on correct input.
{
	assert(((table_size - 1) & table_size) == 0);  // must be 2^x
	assert(mag2.to_float() >= 1 && mag2.to_float() < 8);  // mag2 must be from a prescaled vector

	return nr_lookup_index(mag2, table_size);
}


float evaluate_nr_error(int32 candidate_value_raw, int32 mag2_min_raw, int32 mag2_max_raw, int iterations)
// Given an initial approximation of 1/sqrt(mag2), a range of mag2
// values, and the number of Newton-Raphson refinement iterations to
// perform, compute the worst-case output error in percent.
{
	float worst_error = 0;

	// Try the extrema, and a few points in between.
	for (int i = 0; i <= 8; i++) {
		fixed mag2(fixed::FROM_RAW, (i * mag2_min_raw + (8 - i) * mag2_max_raw) / 8);
		float true_value = 1.0f / sqrtf(mag2.to_float());

		fixed val(fixed::FROM_RAW, candidate_value_raw);
		for (int j = 0; j < iterations; j++) {
			val = mul<3>(val, (fixed(1.5f) - mul<3>((mag2 >> 1), mul<3>(val, val))));
		}

		float error_pct = fabsf((val.to_float() - true_value) / true_value) * 100;
		if (error_pct > worst_error) {
			worst_error = error_pct;
		}
	}
	
	return worst_error;
}


void build_nr_lookup_table(fixed table[], int size, int precision_bits, int iterations)
// Build a lookup table for the initial step of Newton-Raphson.
//
// table values are in the range (0.353, 1).  1.0 is
// technically a legal value, but we approximate it as
// 0.111..., since otherwise it wastes almost a whole bit.
{
	// Slow, dumb but thorough search for best table values.

	assert(precision_bits <= 8);  // this exhaustive search will be very slow for more precision!
	
	float worst_error_pct = 0;
	
	for (int i = 1; i < size; i++) {
		int32 mask = ((1 << precision_bits) - 1) << (16 - precision_bits);

		// Compute the min/max input values that will map to
		// this table entry.
		int32 mag2_min_raw = i << (19 - log2i(size));
		int32 mag2_max_raw = ((i + 1) << (19 - log2i(size))) - 1;

		assert(nr_lookup_index(fixed(fixed::FROM_RAW, mag2_min_raw), size) == i);
		assert(nr_lookup_index(fixed(fixed::FROM_RAW, mag2_max_raw), size) == i);

		// Try each possible value for this table entry.
		float best_error = 100;
		fixed best_value(0);
		for (int j = 0; j < (1 << precision_bits); j++) {
			int32 candidate_value = j << (16 - precision_bits);
			float error = evaluate_nr_error(candidate_value, mag2_min_raw, mag2_max_raw, iterations);
			if (error < best_error) {
				best_error = error;
				best_value.from_raw(candidate_value);
			}
		}

		printf("i = %d, best_error = %f, best_value = %f\n", i, best_error, best_value.to_float());//xxxxx

		if (mag2_max_raw >= 65536 && best_error > worst_error_pct) {
			worst_error_pct = best_error;
		}

		table[i] = best_value;
	}
	table[0] = table[1];

	printf("size = %d, precision_bits = %d, iterations = %d, worst error pct = %f\n",
	       size, precision_bits, iterations, worst_error_pct);//xxxxx

	/* Computed table for 32 entries, 8 bits each, 1 iteration, <0.53% error:
	fixed table_32_8_1[32] = {
		0.996094,
		0.996094,
		0.996094,
		0.996094,
		0.945313,
		0.851563,
		0.785156,
		0.730469,
		0.687500,
		0.648438,
		0.617188,
		0.589844,
		0.566406,
		0.542969,
		0.523438,
		0.507813,
		0.492188,
		0.476563,
		0.464844,
		0.453125,
		0.441406,
		0.429688,
		0.421875,
		0.414063,
		0.402344,
		0.398438,
		0.386719,
		0.382813,
		0.375000,
		0.367188,
		0.363281,
		0.355469,
	};
	*/
}


void do_vector_calculation3(fixed* x_out, fixed* y_out, fixed x_in, fixed y_in, bool draw)
// Use Newton-Raphson search for inv sqrt.
{
	if (draw) {
		// draw the input
		glColor3f(0, 0, 1);
		glBegin(GL_LINE_STRIP);
		glVertex2f(0, 0);
		gl_vertex(x_in, y_in);
		glEnd();
	}
	
	// Prescale the input.
	fixed x_scaled, y_scaled;
	initial_prescale_fixed(&x_scaled, &y_scaled, x_in, y_in);

	if (draw) {
		// draw the input, prescaled.
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_STRIP);
		glVertex2f(0, 0);
		gl_vertex(x_scaled, y_scaled);
		glEnd();
	}

	fixed x = x_scaled;
	fixed y = y_scaled;
	fixed mag2 = mul<2>(x, x) + mul<2>(y, y);

	// approx_table[] holds approximations to 1/sqrt(mag2).
	//
	// Various configurations w/ observed max error:
	//
	// 32 bit lookup table  ( 8 entries by 4 bits), 2 iterations, <0.38% error
	// 32 bit lookup table  ( 8 entries by 4 bits), 1 iterations, <4.95% error
	//
	// 64 bit lookup table  (16 entries by 4 bits), 2 iterations, <0.11% error
	// 64 bit lookup table  ( 8 entries by 8 bits), 2 iterations, <0.33% error
	// 64 bit lookup table  (16 entries by 4 bits), 1 iterations, <2.26% error
	// 64 bit lookup table  ( 8 entries by 8 bits), 1 iterations, <4.37% error
	//
	// 128 bit lookup table (16 entries by 8 bits), 1 iterations, <1.60% error
	// 128 bit lookup table (32 entries by 4 bits), 1 iterations, <1.03% error
	//
	// 256 bit lookup table (32 entries by 8 bits), 1 iterations, <0.53% error
	
	static const int LOOKUP_SIZE = 32; // number of entries
	static const int LOOKUP_PRECISION_BITS = 8;  // precision of the lookup table
	static const int ITERATIONS = 1;
	static fixed approx_table[LOOKUP_SIZE];
	static bool inited = false;
	if (!inited) {
		build_nr_lookup_table(approx_table, LOOKUP_SIZE, LOOKUP_PRECISION_BITS, ITERATIONS);
		inited = true;
	}

	// Map the input into discrete cases, for lookup.
	int lookup = nr_lookup_index_checked(mag2, LOOKUP_SIZE);
	fixed factor = approx_table[lookup];

#ifdef NO_LOOKUP_TABLE
	// Alternative quadratic approximation, in case you want to
	// avoid any lookup at all.  It's no better than a tiny 32-bit
	// lookup table, so it seems kind of pointless, but here it
	// is.
	//
	// 1 iteration  --> <5.5% error
	// 2 iterations --> <0.5% error.
	static const fixed a[3] = { 0.02f, -0.27116f, 1.242849f };
	factor = mul<8>(fixed(a[0]) << 6, mul<8>(mag2, mag2) >> 6) + mul<3>(a[1], mag2) + a[2];
#endif // NO_LOOKUP_TABLE

	if (draw) {
		glColor3f(0.50f, 0, 0);
		draw_x(mul<2>(x, factor).to_float(), mul<2>(y, factor).to_float());
	}

	// Improve the approximation.
	for (int i = 0; i < ITERATIONS; i++) {
		factor = mul<4>(factor, (fixed(1.5f) - mul<4>((mag2 >> 1), mul<4>(factor, factor))));

		if (draw) {
			*x_out = mul<4>(factor, x);
			*y_out = mul<4>(factor, y);
			glColor3f(0.5f, float(i + 1) / ITERATIONS, 0.5f);
			draw_x(x_out->to_float(), y_out->to_float());
		}
	}

	*x_out = mul<4>(factor, x);
	*y_out = mul<4>(factor, y);

	// multiplies: 2 + 3*ITERATIONS + 2
}


void render_diagram(float x, float y)
{
	draw_axes();
	draw_unit_circle();

	// Nutty bezier-approximation search-for-angle thing.
	//draw_bezier_circle_approximation();
	//draw_vector_calculation1(x, y);

	// Binary search for mag2 == 1
	//draw_vector_calculation2(x, y);
	
	// By the book Newton-Raphson.
	fixed x_out, y_out;
	do_vector_calculation3(&x_out, &y_out, fixed(x), fixed(y), true);
}


#undef main	// SDL wackiness
int	main(int argc, const char** argv)
{
	if (argc > 1)
	{
		// TODO arg processing code in here...
		{
			fprintf(stderr, "unknown arg.\n");
			fprintf(stderr, "usage: vec2_normalize\n");
			exit(1);
		}
	}

	// Interactive display.
	int	width = 1000;
	int	height = 1000;
	demo::init_video(width, height, 16);

	// A test vector.
	float x = 2;
	float y = 1;

	demo::nav2d_state nav_state;
	nav_state.m_scale = 1.0f / 512.0f;
	nav_state.m_center_x = 0.5f;
	nav_state.m_center_y = 0.5f;
	for (;;) {
		if (demo::update_nav2d(&nav_state)) {
			// User wants to quit.
			break;
		}
		// Check key input.
		for (int i = 0, n = nav_state.m_keys.size(); i < n; i++) {
			int key = nav_state.m_keys[i];
			if (key == SDLK_LEFT) {
				// Something.
			} else if (key == SDLK_RIGHT) {
				// Something.
			} else if (key == SDLK_e) {
				// Evaluate error.  Try a whole bunch of random vectors, and report the
				// worst result.
				tu_random::generator gen;
				float worst_error_pct = 0;
				for (int i = 0; i < 100000; i++) {
					fixed x_in((gen.get_unit_float() - 0.5f) * 100);
					fixed y_in((gen.get_unit_float() - 0.5f) * 100);
					if (x_in.to_raw() == 0 && x_in.to_raw() == 0) continue;
					
					fixed x_out, y_out;
					do_vector_calculation3(&x_out, &y_out, x_in, y_in, false);

					float mag = sqrt(x_out.to_float() * x_out.to_float() + y_out.to_float() * y_out.to_float());
					float error = fabsf(1.0f - mag) * 100;
					if (error > worst_error_pct) {
						worst_error_pct = error;
						// Show the worst case graphically.
						x = x_in.to_float();
						y = y_in.to_float();
					}
				}
				printf("worst error = %f%%\n", worst_error_pct);
			}
		}
		// On right mouse button, change the test vector.
		if (nav_state.m_mouse_buttons & 8) {
			nav_state.mouse_to_world(&x, &y);
		}

		// Turn on alpha blending.
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

 		demo::set_nav2d_viewport(nav_state);

		glDisable(GL_DEPTH_TEST);	// Disable depth testing.
		glDrawBuffer(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT);

		render_diagram(x, y);

		SDL_GL_SwapBuffers();

		SDL_Delay(10);
	}

	return 0;
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:



