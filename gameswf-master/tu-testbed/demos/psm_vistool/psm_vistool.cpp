// quickie 2D visualization of Perspective Shadow Map sampling density


#include "SDL.h"
#include <stdlib.h>
#include <string.h>
#include "base/ogl.h"
#include "base/utility.h"
#include "base/container.h"
#include "geometry/geometry.h"


struct controls
{
	// Mouse position.
	float	m_mouse_x, m_mouse_y;

	// Mouse buttons.
	bool	m_mouse_left;
	bool	m_mouse_right;
	bool	m_mouse_left_click;
	bool	m_mouse_right_click;

	// Keys
	bool	m_alt;
	bool	m_ctl;
	bool	m_shift;

	controls()
		:
		m_mouse_x(0),
		m_mouse_y(0),
		m_mouse_left(false),
		m_mouse_right(false),
		m_mouse_left_click(false),
		m_mouse_right_click(false),
		m_alt(false),
		m_ctl(false),
		m_shift(false)
	{
	}
};


void	draw_circle(float x, float y, float radius)
// Draw a circle at the given spot w/ given radius.
{
	int	divisions = (int) floorf(radius);
	if (divisions < 4)
	{
		divisions = 4;
	}

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < divisions; i++)
	{
		float	angle = 2.0f * float(M_PI) * (i / float(divisions));
		glVertex3f(x + cosf(angle) * radius, y + sinf(angle) * radius, 1);
	}
	glVertex3f(x + radius, y, 1);
	glEnd();
}


void	draw_filled_circle(float x, float y, float radius)
// Draw a filled circle at the given spot w/ given radius.
{
	int	divisions = (int) floorf(radius);
	if (divisions < 4)
	{
		divisions = 4;
	}

	glBegin(GL_POLYGON);
	for (int i = 0; i < divisions; i++)
	{
		float	angle = 2.0f * float(M_PI) * (i / float(divisions));
		glVertex3f(x + cosf(angle) * radius, y + sinf(angle) * radius, 1);
	}
	glVertex3f(x + radius, y, 1);
	glEnd();
}


void	draw_square(float x0, float y0, float x1, float y1, float xc, float yc, float threshold)
{

	// Draw this square.
	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(x0, y0, 1);
	glVertex3f(x1, y0, 1);
	glVertex3f(x1, y1, 1);
	glVertex3f(x0, y1, 1);
	glVertex3f(x0, y0, 1);
	glEnd();

	if (x1 - x0 > 10)
	{
		// Decide if we should split.

		float	sx = (x0 + x1) / 2;
		float	sy = (y0 + y1) / 2;

		float	dx = sx - xc;
		float	dy = sy - yc;
		float	dist = sqrtf(dx * dx + dy * dy);

		if (dist < threshold)
		{
			// Subdivide.
			float	half_x = (x1 - x0) / 2;
			float	half_y = (y1 - y0) / 2;
			for (int j = 0; j < 2; j++)
			{
				for (int i = 0; i < 2; i++)
				{
					draw_square(x0 + i * half_x, y0 + j * half_y,	
						    x0 + i * half_x + half_x, y0 + j * half_y + half_y,
						    xc, yc,
						    threshold / 2);
				}
			}

			return;
		}
		else
		{
			// Draw a little circle at our center.
			draw_circle(sx, sy, threshold / 16.0f);
		}
	}
}


void	draw_segment(const vec3& a, const vec3& b)
// Draw a line segment between the two points.
{
	glBegin(GL_LINE_STRIP);
	glVertex2f(a.x, a.y);
	glVertex2f(b.x, b.y);
	glEnd();
}


static float	s_shadow_falloff_length = 60.0f;


struct occluder
// Basically a circular object, to stand in for a shadow occluder.
{
	vec3	m_position;
	float	m_radius;

	occluder()
		:
		m_position(vec3::zero),
		m_radius(1.0f)
	{
	}

	occluder(const vec3& pos, float radius)
		:
		m_position(pos),
		m_radius(radius)
	{
	}

	void	draw();

	bool	hit(const vec3& v)
	{
		return (v - m_position).magnitude() <= m_radius;
	}

	float	get_max_projection(const vec3& axis);
	bool	has_visible_shadow();
};


static float	s_viewer_y = -250;
static float	s_view_tan = 1.0f;
static float	s_near_plane_distance = 10;
static float	s_far_plane_distance = 500;
static vec3	s_light_arrow_spot(150, -200, 0);

static vec3	s_light_direction(1, 0, 0);
static vec3	s_light_right(0, -1, 0);

static array<occluder>	s_occluders;


vec3	get_frustum_point(int i)
// Returns the i'th frustum vertex (i in [0,3]).
// Goes ccw around the boundary.
{
	assert(i >= 0 && i < 4);

	float	d = i < 2 ? s_near_plane_distance : s_far_plane_distance;
	float	w = d * s_view_tan / 2;
	if (i == 0 || i == 3)
	{
		w = -w;
	}

	return vec3(w, s_viewer_y + d, 0);
}


vec3	get_frustum_normal(int i)
// Returns the i'th frustum plane normal (i in [0,3]).
// Goes ccw around the boundary.
{
	assert(i >= 0 && i < 4);

	vec3	p0 = get_frustum_point(i);
	vec3	p1 = get_frustum_point((i + 1) & 3);

	vec3	n = p1 - p0;
	n = vec3(n.y, -n.x, 0);	// get the perpendicular.
	n.normalize();

	return n;
}


void	draw_frustum()
// Draw a schematic top-view of a view frustum.
{
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	glVertex3fv(get_frustum_point(0));
	glVertex3fv(get_frustum_point(1));
	glVertex3fv(get_frustum_point(2));
	glVertex3fv(get_frustum_point(3));
	glVertex3fv(get_frustum_point(0));
	glEnd();
}


float	frustum_max_projection(const vec3& axis)
// Return the maximum projection of any frustum point onto the given
// axis.
{
	float	max = -1000000.f;
	for (int i = 0; i < 4; i++)
	{
		vec3	v = get_frustum_point(i);
		float	proj = v * axis;
		if (proj > max)
		{
			max = proj;
		}
	}

	return max;
}


void	draw_occluders()
// Draw all occluders.
{
	for (int i = 0; i < s_occluders.size(); i++)
	{
		s_occluders[i].draw();
	}
}


float	circle_max_projection(const vec3& axis, const vec3& center, float radius)
// Returns the maximum projection of any point on the circle onto the
// given axis.
{
	return center * axis + radius;
}


void	occluder::draw()
{
	// Shadow: dark gray shadow circle & sides.
	vec3	shadow_spot = m_position + s_light_direction * s_shadow_falloff_length;

	// Shadow circle.
	glColor3f(0.25f, 0.25f, 0.25f);
	draw_filled_circle(shadow_spot.x, shadow_spot.y, m_radius);

	// Rectangle connecting object w/ shadow circle.
	glBegin(GL_TRIANGLE_STRIP);
	glVertex2fv(m_position + s_light_right * m_radius);
	glVertex2fv(shadow_spot + s_light_right * m_radius);
	glVertex2fv(m_position - s_light_right * m_radius);
	glVertex2fv(shadow_spot - s_light_right * m_radius);
	glEnd();

	// Filled blue circle for the object itself.
	glColor3f(0.5f, 0.5f, 1);
	draw_filled_circle(m_position.x, m_position.y, m_radius);
}


float	occluder::get_max_projection(const vec3& axis)
// Return our maximum projection onto the given axis.
{
	return circle_max_projection(axis, m_position, m_radius);
}


bool	occluder::has_visible_shadow()
// Return true if this occluder can cast a shadow into the view
// frustum.
{
	// The usual conservative frustum test: see if object is
	// excluded by any of our frustum planes.
	//
	// @@ I'm curious to see if a tighter test would be helpful...

	for (int i = 0; i < 4; i++)
	{
		vec3	frustum_normal = get_frustum_normal(i);
		vec3	frustum_point = get_frustum_point(i);

		float	dist_from_plane =
			fmin(
				frustum_normal * m_position,
				frustum_normal * (m_position + s_light_direction * s_shadow_falloff_length))
			- m_radius;;

		if (dist_from_plane > frustum_normal * frustum_point)
		{
			// This plane excludes us; we can't affect the visible frustum.
			return false;
		}
	}

	return true;
}


float	occluders_max_projection(const vec3& axis)
// Returns the maximum projection of any occluder point onto the given
// axis.
{
	float	max = -1000000.f;
	for (int i = 0; i < s_occluders.size(); i++)
	{
		if (s_occluders[i].has_visible_shadow())
		{
			float	proj = s_occluders[i].get_max_projection(axis);
			if (proj > max)
			{
				max = proj;
			}
		}
	}

	return max;
}


void	find_projection_bounds(float* min_proj, float* max_proj, const vec3& axis)
// Find the maximum projection onto the axis, of any possible shadow
// that can intersect the view frustum.
{
	vec3	my_axis = axis;

	float	fp0 = -frustum_max_projection(-axis);
	float	fp1 = frustum_max_projection(axis);

	if (fp0 > fp1)
	{
		my_axis = -axis;
		swap(&fp0, &fp1);
	}

	// Check occluders.
	float	op0 = -occluders_max_projection(-my_axis);
	float	op1 = occluders_max_projection(my_axis);
	assert(op0 <= op1);

	*min_proj = fmax(op0, fp0);
	*max_proj = fmin(op1, fp1);
}


vec3	to_persp(const vec3& v)
// Transform v from normal space into perspective space.
{
	float	vy = v.y - s_viewer_y;
	float	one_over_vy;
	if (fabs(vy) < 0.000001f)
	{
		one_over_vy = 1000000.0f;	// "big"
	}
	else
	{
		one_over_vy = 1.0f / vy;
	}

	return vec3(v.x * one_over_vy, one_over_vy, 0);
}


vec3	from_persp(const vec3& v)
// Transform v from perspective space to normal space.
{
	float	vy = v.y;
	float	one_over_vy;
	if (fabs(vy) < 0.000001f)
	{
		one_over_vy = 1000000.0f;	// "big"
	}
	else
	{
		one_over_vy = 1.0f / vy;
	}

	return vec3(v.x * one_over_vy, one_over_vy + s_viewer_y, 0);
}


void	draw_light_arrow()
// Draw an arrow icon showing the direction of the light.
{
	vec3	tip(s_light_arrow_spot + s_light_direction * 50.0f);
	vec3	tip_l(tip - s_light_direction * 5.0f - s_light_right * 5.0f);
	vec3	tip_r(tip - s_light_direction * 5.0f + s_light_right * 5.0f);

	glColor3f(1, 1, 0.5f);
	glBegin(GL_LINE_STRIP);
	glVertex2fv(s_light_arrow_spot);
	glVertex2fv(tip);
	glVertex2fv(tip_l);
	glVertex2fv(tip_r);
	glVertex2fv(tip);
	glEnd();
}


void	draw_light_rays(float density)
// Show post-perspective light rays in ordinary space.
{
	// Find the max necessary boundary of our light ray projections.
	float	proj0, proj1;
	find_projection_bounds(&proj0, &proj1, s_light_right);
	vec3	extreme_0 = s_light_right * proj0;
	vec3	extreme_1 = s_light_right * proj1;

	vec3	extreme_0_prime = to_persp(extreme_0);
	vec3	extreme_1_prime = to_persp(extreme_1);


	// Draw a bunch of rays.  Light-buffer ray sampling is linear
	// in *perspective* space, which means it'll be something else
	// in world-space.
 	int	ray_count = iclamp(int(30 * density), 0, 1000);
	ray_count = ray_count * 2 + 1;	// make it odd
 	for (int i = 0; i < ray_count; i++)
 	{
		float	f = 0.0f;
		if (ray_count > 1)
		{
			f = (float(i) / (ray_count - 1));
		}

		vec3	start = from_persp(extreme_0_prime * (1-f) + extreme_1_prime * f);

		vec3	v0 = start - s_light_direction * 100000.0f;
		vec3	v1 = start + s_light_direction * 100000.0f;
		glColor3f(1, 1, 0);
		draw_segment(v0, v1);
	}
}


void	draw_stuff(const controls& c, float density)
{
	float	mx = c.m_mouse_x - 500.0f;
	float	my = 500.0f - c.m_mouse_y;
	vec3	mouse_pos(mx, my, 0);

	if (c.m_mouse_right)
	{
		// Re-compute light direction.
		s_light_direction = mouse_pos - s_light_arrow_spot;
		s_light_direction.normalize();

		// Direction perpendicular to the light.
		s_light_right = vec3(s_light_direction.y, -s_light_direction.x, 0);

		// Draw a white line to the mouse, so the user can see
		// what they're orbiting around.
		glColor3f(1, 1, 1);
		draw_segment(s_light_arrow_spot, mouse_pos);
	}

	if (c.m_mouse_left_click)
	{
		// Add or delete an occluder.

		// If we're on an occluder, then delete it.
		bool	deleted = false;
		for (int i = 0; i < s_occluders.size(); i++)
		{
			if (s_occluders[i].hit(mouse_pos))
			{
				// Remove this guy.
				s_occluders[i] = s_occluders.back();
				s_occluders.resize(s_occluders.size() - 1);
				deleted = true;
				break;
			}
		}

		if (!deleted)
		{
			// If we didn't delete, then the user want to
			// add an occluder.
			s_occluders.push_back(occluder(mouse_pos, 20));
		}
	}

	draw_light_arrow();
	draw_light_rays(density);
	draw_occluders();

	// Draw a line at the "near clip plane".
	glColor3f(0, 0, 1);
	draw_segment(vec3(-1000, s_viewer_y, 0), vec3(1000, s_viewer_y, 0));

	draw_frustum();
}


void	print_usage()
// Instructions.
{
	printf("psm_vistool by Thatcher Ulrich http://tulrich.com\n"
	       "This program has been donated to the Public Domain\n"
	       "\n"
	       "2D visualization of perspective shadow-maps.  The goal is to help get some\n"
	       "intuition about PSM's.  Enjoy!\n"
	       "\n"
	       "Controls:\n"
	       "\n"
	       "   mouse-left    -- place or delete an occluder (blue circle)\n"
	       "\n"
	       "   mouse-right   -- change the light direction\n"
		);
}


int	main(int argc, char *argv[])
{
	print_usage();

	// Initialize the SDL subsystems we're using.
	if (SDL_Init(SDL_INIT_VIDEO /* | SDL_INIT_JOYSTICK | SDL_INIT_CDROM | SDL_INIT_AUDIO*/))
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int	width = 1000;
	int	height = 1000;

	// Set the video mode.
	if (SDL_SetVideoMode(width, height, 16 /* 32 */, SDL_OPENGL) == 0)
	{
		fprintf(stderr, "SDL_SetVideoMode() failed.");
		exit(1);
	}

	ogl::open();

	// Turn on alpha blending.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0, 1000, 0, 1000, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(500, 500, 0);

	float	density = 1.0f;


	// Add a couple occluders.
	s_occluders.push_back(occluder(vec3(0,0,0), 20));
	s_occluders.push_back(occluder(vec3(100,50,0), 20));
	s_occluders.push_back(occluder(vec3(-100,50,0), 20));

	// Mouse state.
	controls	c;
//	int	mouse_x = 0;
//	int	mouse_y = 0;
//	int	mouse_buttons = 0;

	bool	paused = false;
	float	speed_scale = 1.0f;
	Uint32	last_ticks = SDL_GetTicks();
	for (;;)
	{
		Uint32	ticks = SDL_GetTicks();
		int	delta_ticks = ticks - last_ticks;
		float	delta_t = delta_ticks / 1000.f;
		last_ticks = ticks;

		if (paused == true)
		{
			delta_t = 0.0f;
		}

		// Handle input.
		c.m_mouse_left_click = false;
		c.m_mouse_right_click = false;
		SDL_Event	event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
			{
				int	key = event.key.keysym.sym;

				if (key == SDLK_q || key == SDLK_ESCAPE)
				{
					exit(0);
				}
				else if (key == SDLK_k)
				{
					density = fclamp(density + 0.1f, 0.1f, 3.0f);
					printf("density = %f\n", density);
				}
				else if (key == SDLK_j)
				{
					density = fclamp(density - 0.1f, 0.1f, 3.0f);
					printf("density = %f\n", density);
				}
				else if (key == SDLK_LCTRL || key == SDLK_RCTRL)
				{
					c.m_ctl = true;
				}
				else if (key == SDLK_LSHIFT || key == SDLK_RSHIFT)
				{
					c.m_shift = true;
				}
				else if (key == SDLK_LALT || key == SDLK_RALT)
				{
					c.m_alt = true;
				}
				break;
			}

			case SDL_KEYUP:
			{
				int	key = event.key.keysym.sym;

				if (key == SDLK_LCTRL || key == SDLK_RCTRL)
				{
					c.m_ctl = false;
				}
				else if (key == SDLK_LSHIFT || key == SDLK_RSHIFT)
				{
					c.m_shift = false;
				}
				else if (key == SDLK_LALT || key == SDLK_RALT)
				{
					c.m_alt = false;
				}
				break;
			}

			case SDL_MOUSEMOTION:
				c.m_mouse_x = float((int) (event.motion.x));
				c.m_mouse_y = float((int) (event.motion.y));
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				int	mask = 1 << (event.button.button);
				bool	down = (event.button.state == SDL_PRESSED);
				bool	clicked = (event.type == SDL_MOUSEBUTTONDOWN);
				if (event.button.button == 1)
				{
					c.m_mouse_left = down;
					c.m_mouse_left_click = clicked;
				}
				if (event.button.button == 3)
				{
					c.m_mouse_right = down;
					c.m_mouse_right_click = clicked;
				}

				break;
			}

			case SDL_QUIT:
				exit(0);
				break;

			default:
				break;
			}
		}

		glDisable(GL_DEPTH_TEST);	// Disable depth testing.
		glDrawBuffer(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_stuff(c, density);

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
