// bspdemo.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2001

// This program is in the public domain.

// Test program for using BSP as a volume representation.  Load a
// polygon soup from a .ply file, make a bsp volume out of the data,
// and then raytrace the shape.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

#include "base/utility.h"
#include "geometry/bsp.h"


void	get_ply_data( const char* filename );
bsp_node*	make_bsp_tree();
void	render( SDL_Surface* output, bsp_node* shape );
void	measure_performance( bsp_node* tree, float model_extent );


const int	HEIGHT = 200;
const int	WIDTH = 200;


SDL_Surface*	screen = NULL;


vec3	viewpoint( -1, -0.05, 0 );
vec3	direction( 1, 0.05, 0 );
vec3	light_direction( 0.4, -1, 0.7 );
float	model_extent = 0.01f;	// The distance of the furthest vertex from the origin.
vec3	average_center = vec3( 0, 0, 0 );

bool	raycast_render = 1;
bool	point_sample_render = 0;
bool	splat_render = 0;

#ifndef __MACH__
#undef main	// some wacky SDL thing...
#endif
int	main( int argc, char *argv[] )
{
	// parse a .ply file...

	if ( argc < 2 ) {
		printf( "bspdemo, program to convert poly soup to a bsp, and raytrace it\n"
				"usage:\n"
				"\tbspdemo <file.ply>\n"
			);
		exit( 1 );
	}

	get_ply_data( argv[1] );

	bsp_node*	root = make_bsp_tree();

	// Make a window to draw into.
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf( stderr, "Couldn't initialize SDL: %s\n", SDL_GetError() );
		exit( 1 );
	}
	int	videoflags = SDL_SWSURFACE;

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, videoflags);
	if ( screen == NULL )
	{
		fprintf( stderr, "Couldn't set display mode: %s\n", SDL_GetError() );
		SDL_Quit();
		exit( 1 );
	}

	direction.normalize();
	light_direction.normalize();

	float	theta = 0;
	float	phi = 0;

	int	key;
	bool	done = false;
	while ( ! done ) {
		SDL_Event	event;
		while(SDL_PollEvent(&event))
		{
			switch (event.type)
			{
/*
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
  if ( event.button.state == SDL_PRESSED )
  buttonstate|=1<<event.button.button;
  else
  buttonstate&=~(1<<event.button.button);
  mousex=event.button.x;
  mousey=event.button.y;
  if(!ispaused && buttonstate&2) trydisk();
  break;
  case SDL_MOUSEMOTION:
  mousex=event.motion.x;
  mousey=event.motion.y;
  if(!ispaused && buttonstate&2) trydisk();
  break;
*/
			case SDL_KEYDOWN:
				key =event.key.keysym.sym;
				if ( key == SDLK_q || key == SDLK_ESCAPE ) {
					done = true;
				}

//				if ( key == SDLK_p ) {
//					point_sample_render = !point_sample_render;
//				}
				if ( key == SDLK_r ) {
					raycast_render = !raycast_render;
				}
				if ( key == SDLK_s ) {
					splat_render = !splat_render;
				}
				if ( key == SDLK_p ) {
					// Do a performance-measurement cycle.
					measure_performance( root, model_extent );
				}
				break;
			case SDL_QUIT:
				done = true;
				break;
			default:
				break;
			}
		}

		Uint8*	keystate = SDL_GetKeyState( NULL );

		if ( keystate[ SDLK_UP ] || keystate[ SDLK_KP8 ] ) {
			phi = fclamp( phi + 0.05f, -M_PI/2 + 0.01f, M_PI/2 - 0.01f );
		}
		if ( keystate[ SDLK_DOWN ] || keystate[ SDLK_KP2 ] ) {
			phi = fclamp( phi - 0.05f, -M_PI/2 + 0.01f, M_PI/2 - 0.01f );
		}
		if ( keystate[ SDLK_RIGHT ] || keystate[ SDLK_KP6 ] ) {
			theta -= 0.05f;
		}
		if ( keystate[ SDLK_LEFT ] || keystate[ SDLK_KP4 ] ) {
			theta += 0.05f;
		}
		if ( keystate[ SDLK_PAGEDOWN ] ) {
			model_extent = model_extent * 1.111f;
		}
		if ( keystate[ SDLK_PAGEUP ] ) {
			model_extent = fmax( 0.01f, model_extent * 0.9 );
		}

		direction = vec3( -cos(theta), -tan(phi), -sin(theta ) );
		direction.normalize();
		viewpoint = -direction * model_extent * 1.2f;

		// Raytrace the model and display it.
		render( screen, root );
		SDL_UpdateRect( screen, 0, 0, 0, 0 );

	}

	SDL_Quit();

	return 0;
}


int	vertex_count = 0;
int	face_count = 0;

vec3*	vertices = NULL;
int*	indices = NULL;


void	get_ply_data( const char* filename )
// Read polygon soup data from a .ply file.
{
	FILE*	in = fopen( filename, "r" );
	if ( in == NULL ) {
		printf( "can't open %s\n", filename );
		exit( 1 );
	}

	char	buffer[1000];

	// Read the header.
	for (;;) {
		char*	ret = fgets( buffer, 1000, in );
		if ( ret == NULL ) {
			// Can't read any more.
			break;
		}

//		printf( buffer );//xxxx

		sscanf( buffer, "element vertex %d", &vertex_count );
		sscanf( buffer, "element face %d", &face_count );

		if ( strncmp( buffer, "end_header", strlen( "end_header" ) ) == 0 ) {
			break;
		}
	}

	// Read the data.

	// Read vertices.
	vertices = new vec3[ vertex_count ];
	int	i;
	for ( i = 0; i < vertex_count; i++ ) {
		char*	ret = fgets( buffer, 1000, in );
		if ( ret == NULL ) {
			// Can't read any more.
			break;
		}

		float	x = 0, y = 0, z = 0;
		sscanf( buffer, "%f %f %f", &x, &y, &z );

//		vertices[ i ] = vec3( x, y, z );
		vertices[ i ] = vec3( x, z, -y );	// rotate

		average_center += vertices[ i ];
	}

	average_center /= vertex_count;

	model_extent = 0.01f;
	for ( i = 0; i < vertex_count; i++ ) {
		vertices[ i ] -= average_center;

		float	d = vertices[ i ].magnitude();
		if ( d > model_extent ) {
			model_extent = d;
		}
	}

	// Read face indices.
	indices = new int[ face_count * 3 ];
	for ( i = 0; i < face_count; i++ ) {
		char*	ret = fgets( buffer, 1000, in );
		if ( ret == NULL ) {
			// Can't read any more.
			break;
		}

		int	a = -1, b = -1, c = -1;
		sscanf( buffer, "%*d %d %d %d", &a, &b, &c );

		if ( a < 0 || a >= vertex_count
			 || b < 0 || b >= vertex_count
			 || c < 0 || c >= vertex_count )
		{
			printf( "ERROR reading face... " );
		}

//		printf( "face[%d] = (%d, %d, %d)\n", i, a, b, c );
		indices[ i*3 + 0 ] = a;
		indices[ i*3 + 1 ] = b;
		indices[ i*3 + 2 ] = c;
	}

	fclose( in );

	printf( "read %d verts, %d faces\n", vertex_count, face_count );
}


bsp_node*	make_bsp_tree()
// Make a bsp tree volume using the previously loaded triangle soup.
{
	bsp_node*	root = NULL;
	int	i, j;

	// Do some face sorting.
	int*	face_list = new int[ face_count ];
	for ( i = 0; i < face_count; i ++ ) {
		face_list[ i ] = i;
	}

	// Shuffle randomly.
	for ( j = 0; j < 0; j++ ) {
		for ( i = 0; i < face_count; i++ ) {
			int	swap_index = rand() % face_count;

			// Swap.
			int	temp = face_list[ i ];
			face_list[ i ] = face_list[ swap_index ];
			face_list[ swap_index ] = temp;
		}
	}

	// NOTE: If I were going to make this wicked fast, I would not put
	// these partitioning planes in the bsp.  Instead, I would make a
	// grid of square or cubical cells, and make one BSP per cell.  I
	// would clip the ray to the cell during a DDA traversal, and call
	// the bsp code for each cell until I got a hit.  I gather this is
	// roughly what raytracing folks generally do.

	// Get started; put a box around the model with partitioning planes.
	root = new bsp_node( plane_info( vec3::x_axis, -model_extent ) );
	root->add_partition( plane_info( vec3::x_axis, model_extent ) );
	root->add_partition( plane_info( vec3::y_axis, -model_extent ) );
	root->add_partition( plane_info( vec3::y_axis, model_extent ) );
	root->add_partition( plane_info( vec3::z_axis, -model_extent ) );
	root->add_partition( plane_info( vec3::z_axis, model_extent ) );

	// Make an octree using partitioning planes, to get a basic sane
	// partitioning of the model space.
	//
	// For game models, you probably would parameterize the tree_depth
	// based on the world-size of the model -- big models get
	// partitioned more than little models.
	const int	tree_depth = 1;
	for ( j = 0; j < tree_depth; j++ ) {
		printf( "tree level %d\n", j );
		int	plane_count = 1 << j;
		float	plane_spacing = model_extent * 2 / plane_count;
		float	plane_start = model_extent - plane_spacing / 2;

		for ( i = 0; i < plane_count; i++ ) {
			root->add_partition( plane_info( vec3::x_axis, plane_start + i * plane_spacing ) );
			root->add_partition( plane_info( vec3::y_axis, plane_start + i * plane_spacing ) );
//			root->add_partition( plane_info( vec3::z_axis, plane_start + i * plane_spacing ) );
		}
	}
	
	float	bsp_slop = model_extent / 1000000.f;

	// Insert each face into the bsp.
	for ( i = 0; i < face_count; i ++ ) {
		plane_info	p;

		int	face = face_list[ i ];

		vec3&	a = vertices[ indices[ face * 3 + 0 ] ];
		vec3&	b = vertices[ indices[ face * 3 + 1 ] ];
		vec3&	c = vertices[ indices[ face * 3 + 2 ] ];

		p.normal = ( b - a ).cross( c - a );
		p.normal.normalize();
		p.d = p.normal * a;

		// Add the face to the tree.
		printf( "%d:\n", i );//xxxxx
		root->add_face( a, b, c, p, face, bsp_slop );
	}

	return root;
}


int	shade( const vec3& normal )
// Returns a value from 0-255 for the brightness of a point with the
// given normal.
{
	return iclamp( ( normal * light_direction - 1 ) / 2 * -255, 0, 255 );
}


bool	face_test_callback( const vec3& normal, const vec3& point, int face_index )
// Returns true if the given point is inside the specified face.  The
// point must be in the plane of the face.  normal is the plane normal
// of the face.
{
	const vec3&	v0 = vertices[ indices[ face_index * 3 + 0 ] ];
	const vec3&	v1 = vertices[ indices[ face_index * 3 + 1 ] ];
	const vec3&	v2 = vertices[ indices[ face_index * 3 + 2 ] ];

	return ( ( v1 - v0 ).cross( point - v0 ) ) * normal >= 0
		&& ( ( v2 - v0 ).cross( point - v0 ) ) * normal <= 0
		&& ( ( v2 - v1 ).cross( point - v1 ) ) * normal >= 0
		;
}


void	render( SDL_Surface* output, bsp_node* shape )
// Render the given shape to the given output surface.
{
	int	i, j;
	collision_info	ci;

	vec3	up = direction.cross( vec3( 0, 1, 0 ) ).cross( direction );
	up.normalize();
	vec3	right = direction.cross( up );

	for ( j = 0; j < HEIGHT; j++ ) {
		for ( i = 0; i < WIDTH; i++ ) {
			// shoot a ray...
			int	color = (i&4 && j&4) ? 50 : 25;
			vec3	dir = direction
				+ up * -( j / float( HEIGHT ) - 0.5f )
				+ right * ( i / float( WIDTH ) - 0.5f );
			dir.normalize();

			if ( raycast_render ) {
				if ( // i == 100 && j == 100 &&
					shape->ray_cast( &ci, viewpoint, dir, model_extent * 2, face_test_callback ) )
				{
					color = shade( ci.normal );
				}
			}

			if ( point_sample_render ) {
				if ( shape->test_point( up * -( j / float(HEIGHT) - 0.5f ) * model_extent * 2 + right * ( i / float( WIDTH ) - 0.5f ) * model_extent * 2 ) )
				{
					color = 255;
				}
			}

			SDL_Rect	r;
			r.x = i;
			r.y = j;
			r.w = 1;
			r.h = 1;

			SDL_FillRect( output, &r, SDL_MapRGB( output->format, color, color, color ) );
		}
	}

	if ( splat_render ) {
		// splat-render the model...
		for ( i = 0; i < face_count; i += 1 ) {
			plane_info	p;
		
			vec3&	a = vertices[ indices[ i * 3 + 0 ] ];
			vec3&	b = vertices[ indices[ i * 3 + 1 ] ];
			vec3&	c = vertices[ indices[ i * 3 + 2 ] ];

			p.normal = ( b - a ).cross( c - a );
			p.normal.normalize();
			p.d = p.normal * a;

			if ( p.normal * direction > 0 ) {
				// Back facing; don't render.
				continue;
			}

			vec3	point = ( a + b + c ) / 3;

			float	x = right * point / ( model_extent * 2 );
			float	y = up * point / ( model_extent * 2 );

			int	ix = ( x + 0.5f ) * WIDTH;
			int	iy = ( 1.0f - ( y + 0.5f ) ) * HEIGHT;
			if ( ix >= 0 && ix < WIDTH && iy >= 0 && iy < HEIGHT ) {
				SDL_Rect	r;
				r.x = ix;
				r.y = iy;
				r.w = 1;
				r.h = 1;

				int	color = shade( p.normal );

				SDL_FillRect( output, &r, SDL_MapRGB( output->format, color, color, color ) );
			}
		}
	}
}


// Array of rays to use for performance testing.
bool	testray_initialized = false;
const int	testray_count = 1000;
vec3	testray_dir[ testray_count ];
vec3	testray_point[ testray_count ];


void	measure_performance( bsp_node* tree, float model_extent )
// Measure ray-casting performance by shooting a bunch of rays at the
// given model, and timing how long it takes.
{
	int	i, j;
	float	length = model_extent / 50.f;
	collision_info	ci;

	if ( testray_initialized == false ) {
		testray_initialized = true;

		// Compute our set of test rays.  Use only rays that are near geometry.
		srand( 10 );	// always shoot the same rays.

		int	index;
		for ( index = 0; index < testray_count; ) {
			// Generate a random ray.
			vec3	dir( rand() / float(RAND_MAX) - 0.5f, rand() / float(RAND_MAX) - 0.5f, rand() / float(RAND_MAX) - 0.5f );
			dir.normalize();	// @@ biased towards cube corners !!
			vec3	point(
				(rand() / float(RAND_MAX) - 0.5f) * 2 * model_extent,
				(rand() / float(RAND_MAX) - 0.5f) * 2 * model_extent,
				(rand() / float(RAND_MAX) - 0.5f) * 2 * model_extent
				);

			if ( tree->ray_cast( &ci, point, dir, length * 1.5, face_test_callback ) ) {
				// This ray is near geometry, so use it.
				testray_dir[ index ] = dir;
				testray_point[ index ] = point;
				index++;
			}
		}
	}

	// Shoot the rays.
	Uint32	start = SDL_GetTicks();

	int	hit_count = 0;
	int	ray_count = 0;
	int	iterations = 1000000 / testray_count;
	for ( j = 0; j < iterations; j++ ) {
		for ( i = 0; i < testray_count; i++ ) {
			if ( tree->ray_cast( &ci, testray_point[ i ], testray_dir[ i ], length, face_test_callback ) ) {
				hit_count++;
			}
		}
		ray_count += testray_count;
	}
	
	Uint32	ticks = SDL_GetTicks() - start;

	// Print results.

	float	seconds = ticks / 1000.f;
	if ( ticks > 0 ) {
		printf( "shot %d rays in %f seconds, %d hits, %f rays/sec, %f ms/ray\n",
				ray_count,
				seconds,
				hit_count,
				ray_count / seconds,
				seconds * 1000 / ray_count );
	}
}

