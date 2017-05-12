// lattice_resistance.cpp	-- by Thatcher Ulrich http://tulrich.com

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// quickie visualization/sim to find resistance between two points on
// infinite lattice of resistors


#include "SDL.h"
#include <stdlib.h>
#include <string.h>
#include "base/ogl.h"
#include "base/utility.h"
#include "base/container.h"



void	draw_filled_circle(float x, float y, float radius)
// Draw a circle at the given spot w/ given radius.
{
	int	divisions = (int) floorf(radius);
	if (divisions < 4)
	{
		divisions = 4;
	}

//	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < divisions; i++)
	{
		float	angle = 2.0f * float(M_PI) * (i / float(divisions));
		glVertex3f(x + cosf(angle) * radius, y + sinf(angle) * radius, 1);
	}
//	glVertex3f(x + radius, y, 1);
	glEnd();
}


void	draw_circle(float x, float y, float radius)
// Draw a circle at the given spot w/ given radius.
{
	int	divisions = (int) floorf(radius);
	if (divisions < 4)
	{
		divisions = 4;
	}

//	glColor3f(1, 1, 1);
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

const int WIDTH = 100;
const int HEIGHT = 100;

// Each node in the lattice is connected to its N,S,E,W neighbors by a 1ohm resistor.
static float	s_voltage[2][HEIGHT][WIDTH];
static float	(*s_active)[HEIGHT][WIDTH] = &(s_voltage[0]);


const int POINT0X = WIDTH/2 - 1;
const int POINT0Y = HEIGHT/2 - 1;
const int POINT1X = POINT0X + 1;
const int POINT1Y = POINT0Y + 2;


void	init()
// Return resistance between POINT0 and POINT1.
{
	// Clear.
	for (int i = 0; i < 2; i++)
	{
		for (int y = 0; y < HEIGHT; y++)
		{
			for (int x = 0; x < WIDTH; x++)
			{
				s_voltage[i][y][x] = 0.5f;
			}
		}
	}
}


float	compute_current(int x, int y)
// Compute current entering point (x,y) from its immediate neighbors.
{
	float	current = 0;

	// I = V/R, R = 1 --> I = V

	if (y > 0) current += (*s_active)[y - 1][x] - (*s_active)[y][x];
	if (y < HEIGHT-1) current += (*s_active)[y + 1][x] - (*s_active)[y][x];
	if (x > 0) current += (*s_active)[y][x - 1] - (*s_active)[y][x];
	if (x < WIDTH-1) current += (*s_active)[y][x + 1]- (*s_active)[y][x];

	return current;
}


float	s_scale = 0.24f;	// adjust for speed/stability


void	sim_step()
{
	float	(*next_lattice)[HEIGHT][WIDTH];
	if (s_active == &s_voltage[0])
	{
		next_lattice = &s_voltage[1];
	}
	else
	{
		next_lattice = &s_voltage[0];
	}

	// The defined points are voltage sources; force their voltages to known values.
	(*s_active)[POINT0Y][POINT0X] = 0;
	(*s_active)[POINT1Y][POINT1X] = 1;

	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			// trickle a little charge based on voltage delta.
			float	current = compute_current(x, y);

			// charge flows based on voltage difference	
			float	charge = s_scale * current;

			(*next_lattice)[y][x] = (*s_active)[y][x] + charge;
		}
	}

	s_active = next_lattice;

	// The defined points are voltage sources; force their voltages to known values.
	(*s_active)[POINT0Y][POINT0X] = 0;
	(*s_active)[POINT1Y][POINT1X] = 1;
}


float	measure_resistance()
{
	// V = IR --> R = V/I
	
	// Measure I entering POINT0
	float	I = compute_current(POINT0X, POINT0Y);

	// All the current entering POINT0 came from POINT1, so the R
	// between POINT0 and POINT1 is total voltage drop (1V) over
	// total current flow (I)
	if (fabsf(I) > 1e-6f)
	{
		return 1.0f / I;
	}
	else
	{
		return 1e6f;
	}
}


// S-curve, for enhancing contrast
float	scurve(float f)
{
	// h2 Hermite basis
	f = fclamp(f, 0, 1);
	return -2 * f * f * f + 3 * f * f;
}


void	set_color(float voltage)
// Set a color based on the voltage.
{
	// Red --> Green --> Blue gradient
	float	f = scurve(scurve(voltage));	// enhance contrast!
	glColor3f(
		fclamp(2 * (f - 0.5f), 0, 1),
		1 - 2 * fabs(f - 0.5f),
		fclamp(2 * (0.5f - f), 0, 1));
}


void	draw_stuff()
{
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			set_color((*s_active)[y][x]);
			float	draw_x = float(x - WIDTH/2 + 5) * 100;
			float	draw_y = float(y - HEIGHT/2 + 5) * 100;
			draw_filled_circle(draw_x, draw_y, 30.f);

			if ((x == POINT0X && y == POINT0Y)
			    || (x == POINT1X && y == POINT1Y))
			{
				glColor3f(1, 1, 1);
				draw_circle(draw_x, draw_y, 35.f);
			}
		}
	}
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

// 	float	lod_tweak = 1.0f;

	init();

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
					s_scale *= 1.111111f;
 				}
 				else if (key == SDLK_j)
 				{
					s_scale *= 0.9f;
 				}
				else if (key == SDLK_p)
				{
					// Print measured resistance.
					float	r = measure_resistance();
					printf("measured r = %f ohms\n", r);
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

		for (int i = 0; i < 100; i++)
		{
			sim_step();
		}
		draw_stuff();

		SDL_GL_SwapBuffers();

//		SDL_Delay(10);

		printf("measured r = %f ohms\n", measure_resistance());
	}

	return 0;
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
