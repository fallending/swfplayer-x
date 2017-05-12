// quad_distance_test.cpp	-- by Thatcher Ulrich http://tulrich.com 2003

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// quickie visualization of circles vs. quadtree


#include "SDL.h"
#include <stdlib.h>
#include <string.h>
#include "base/ogl.h"
#include "base/utility.h"
#include "base/container.h"



void	draw_circle(float x, float y, float radius)
// Draw a circle at the given spot w/ given radius.
{
	int	divisions = (int) floorf(radius);
	if (divisions < 4)
	{
		divisions = 4;
	}

	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
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


void	draw_stuff(float xcenter, float ycenter, float lod_tweak)
{
	// Draw a series of concentric circles w/ ^2 radiuses.
	float	radius = 1000.0f * lod_tweak;

	while (radius > 10.0f)
	{
		draw_circle(xcenter, ycenter, radius);
		radius /= 2.0f;
	}

	// Draw quadsquares; recursively split based on distance to
	// {xcenter,ycenter}.
	draw_square(0, 0, 1000, 1000, xcenter, ycenter, 1000 * lod_tweak);
}



#undef main	// SDL wackiness
int	main(int argc, char *argv[])
{
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

	float	lod_tweak = 1.0f;

	// Mouse state.
	int	mouse_x = 0;
	int	mouse_y = 0;
	int	mouse_buttons = 0;

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
					lod_tweak = fclamp(lod_tweak + 0.1f, 0.1f, 3.0f);
					printf("lod_tweak = %f\n", lod_tweak);
				}
				else if (key == SDLK_j)
				{
					lod_tweak = fclamp(lod_tweak - 0.1f, 0.1f, 3.0f);
					printf("lod_tweak = %f\n", lod_tweak);
				}
				break;
			}

			case SDL_MOUSEMOTION:
				mouse_x = (int) (event.motion.x);
				mouse_y = (int) (event.motion.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				int	mask = 1 << (event.button.button);
				if (event.button.state == SDL_PRESSED)
				{
					mouse_buttons |= mask;
				}
				else
				{
					mouse_buttons &= ~mask;
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

		draw_stuff(float(mouse_x), 1000.0f - float(mouse_y), lod_tweak);

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
