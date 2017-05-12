// chunklod.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2001

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Hardware-friendly chunked LOD.  Chunks are organized in a tree,
// with each successive level having half the max vertex error of its
// parent.  Collapsed vertices in each chunk are morphed according to
// the view distance of the chunk.  Morphing is just simple lerping
// and can be done by the CPU, or with a vertex shader.
//
// A quadtree-tiled texture is used to cover the terrain.


#include <stdlib.h>
#include <string.h>

#include "base/dlmalloc.h"
#include "base/image.h"
#include "base/ogl.h"
#include "base/tu_file.h"
#include "base/utility.h"
#include "geometry/tqt.h"

#include "SDL_thread.h"

#include "chunklod.h"


#ifndef M_PI
#define M_PI 3.141592654
#endif // M_PI


//static ogl::vertex_stream*	s_stream = NULL;

static float	s_vertical_scale = 1.0;

// Some performance stats.
static int	s_chunks_in_use = 0;
static int	s_chunks_with_data = 0;
static int	s_chunks_with_texture = 0;
static int	s_bytes_in_use = 0;
static int	s_textures_bound = 0;


// MAP_ONTO_CUBEMAP is currently just a quick experiment to see if the
// geometry works out.  Texturing doesn't work, and the LOD metric is
// all screwed up, and the initial viewpoint doesn't show the world;
// you'll have to rotate around a little to see anything.  Also,
// morph_vertices() burns much CPU due to the reciprocal sqrt in the
// normalize, so you wouldn't want to use this approach at all unless
// you have a fast vshader version.
//
// The basic idea is to treat the (square) heightfield as a face of a
// cubemap, and warp it onto the corresponding patch of a sphere.  So
// you could make a whole sphere by rendering six heightfields in
// different orientations.  The height values in the heightmap are
// turned into radial distances relative to the sphere center, so that
// all works out.
//
// Anyway, the basic geometry looks like it works out, but this is
// kind of a brute-force method; only viable if you have tons CPU or
// vshader cycles to burn.
//
// One more little footnote: the crack-filling skirts would have to
// take the warping into account, to make sure skirts along long edges
// are properly covered.  Basically, the skirts have to get much more
// conservative; otherwise there are cracks along long curvy edges.
//
//#define MAP_ONTO_CUBEMAP

#ifdef MAP_ONTO_CUBEMAP
// Some experimental hacks for mapping heightfield onto a sphere.
static float	s_max_horizontal_dimension = 1.0f;	// set by top-level render call, used by morph_vertices()
#endif // MAP_ONTO_CUBEMAP


// MAP_ONTO_MERCATOR is another experiment with spherical geometry.
// Take the square terrain, and treat it like a mercator projection
// map.  The geometry gets increasingly stretched towards the poles,
// so to do this properly we would adjust the lod metric to
// compensate.  Also we would not fill in the more detailed quadtree
// nodes towards the poles, since quad nodes cover increasingly less
// geometric area, so requires fewer nodes to achieve visible detail
// targets.
//
// Same caveat as cubemap, w/r/t needing more conservative
// crack-filling skirts on the larger tiles.
//
// Mercator has a couple of nice properties:
//
// * can use a single quadtree, and it's easy to match up all seams;
// just make sure the left & right heightfield edges are identical
// (make them neighbors).
//
// * at lower levels of the quadtree, quads have approximately uniform
// sampling density of the underlying geometry.
//
// It gets very wacky near the poles unfortunately.
//
//#define MAP_ONTO_MERCATOR

#ifdef MAP_ONTO_MERCATOR
// Some experimental hacks for mapping heightfield onto a sphere.
static float	s_max_horizontal_dimension = 1.0f;	// set by top-level render call, used by morph_vertices()
#endif // MAP_ONTO_MERCATOR


namespace lod_tile_freelist
// A little module to hold onto unused textures, for later reuse.
// Cuts down on needless texture creation/destruction.
{
	array<unsigned int>	s_free_textures;
	const int	FREELIST_SIZE_LIMIT = 10;

	void	free_texture(unsigned int texture_id)
	// Dispose of the given texture object.  May be recycled.
	{
		if (texture_id)
		{
			if (s_free_textures.size() < FREELIST_SIZE_LIMIT)
			{
				s_free_textures.push_back(texture_id);
			}
			else
			{
				// We've already got too many free
				// textures; just delete this one.
				glDeleteTextures(1, (GLuint*)&texture_id);
			}
		}
	}

	
	unsigned int	make_texture(image::rgb* surf)
	// Return a texture id, using the given surface info.  FREES
	// THE GIVEN SURFACE!
	//
	// The actual texture object may be recycled.
	{
		if (surf == NULL) {
			return 0;
		}

		assert(surf->m_data);

		unsigned int	texture_id = 0;

		// Do we have a texture on the freelist?
		if (s_free_textures.size() > 0)
		{
			// Recycle.

			// Grab a texture from the end of the list.
			texture_id = s_free_textures.back();
			s_free_textures.resize(s_free_textures.size() - 1);

			glBindTexture(GL_TEXTURE_2D, texture_id);

			// Put the new data in the old texture.
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surf->m_width, surf->m_height, GL_RGB, GL_UNSIGNED_BYTE, surf->m_data);

			// Build mips.
			int	level = 1;
			while (surf->m_width > 1 || surf->m_height > 1) {
				image::make_next_miplevel(surf);
				glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, surf->m_width, surf->m_height, GL_RGB, GL_UNSIGNED_BYTE, surf->m_data);
				level++;
			}
		}
		else
		{
			// Allocate a new texture object.
		
			// Bind a texture id and set up this image as a texture.
			glEnable(GL_TEXTURE_2D);
			glGenTextures(1, (GLuint*)&texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_id);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ogl::get_clamp_mode());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ogl::get_clamp_mode());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surf->m_width, surf->m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, surf->m_data);

			// Build mips.
			int	level = 1;
			while (surf->m_width > 1 || surf->m_height > 1) {
				image::make_next_miplevel(surf);
				glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, surf->m_width, surf->m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, surf->m_data);
				level++;
			}
		}
	
		delete surf;

		return texture_id;
		
	}
};



struct vertex_info {
// Structure for storing morphable vertex mesh info.
	int	vertex_count;
	struct vertex {
		Sint16	x[3];
		Sint16	y_delta;	// delta, to get to morphed y

		void	read(tu_file* in)
		{
			x[0] = in->read_le16();
			x[1] = in->read_le16();
			x[2] = in->read_le16();
			y_delta = in->read_le16();

//			// xxxxxx TEST QUANTIZATION xxxxxxx
//			// lose the bottom 8 bits by rounding
//			y_delta = (Sint16) iclamp((y_delta + 128) & ~0x0FF, -32768, 32767);
//			// xxxxxx TEST QUANTIZATION xxxxxxx
		}
	};
	vertex*	vertices;

	int	index_count;
	Uint16*	indices;

	int	triangle_count;	// for statistics.

//code:
	vertex_info()
		: vertex_count(0),
		  vertices(0),
		  index_count(0),
		  indices(0),
		  triangle_count(0)
	{
	}

	~vertex_info()
	{
		if (vertices) {
//			delete [] vertices;
			dlfree(vertices);
			vertices = NULL;
		}
		if (indices) {
//			delete [] indices;
			dlfree(indices);
			indices = NULL;
		}
	}

	void	read(tu_file* in);

	int	get_data_size() const
	// Return the data bytes used by this object.
	{
		return sizeof(*this) + vertex_count * sizeof(vertices[0]) + index_count * sizeof(indices[0]);
	}
};


void	vertex_info::read(tu_file* in)
// Read vert info from the given file.
{
	vertex_count = in->read_le16();
//	vertices = new vertex[vertex_count];
	vertices = (vertex*) dlmalloc(vertex_count * sizeof(vertex));
	for (int i = 0; i < vertex_count; i++) {
		vertices[i].read(in);
	}
		
	// Load indices.
	index_count = in->read_le32();
	if (index_count > 0) {
//		indices = new Uint16[ index_count ];
		indices = (Uint16*) dlmalloc(index_count * sizeof(Uint16));
	} else {
		indices = NULL;
	}
	{for (int i = 0; i < index_count; i++) {
		indices[i] = in->read_le16();
	}}

	// Load the real triangle count, for computing statistics.
	triangle_count = in->read_le32();

//	printf("vertex_info::read() -- vertex_count = %d, index_count = %d\n", vertex_count, index_count);//xxxxxxxx
}


struct lod_chunk;


// Vertex/mesh data for a chunk.  Can get paged in/out on demand.
struct lod_chunk_data {
	vertex_info	m_verts;	// vertex and mesh info; vertex array w/ morph targets, indices

	//	lod_chunk_data* m_next_data;
	//	lod_chunk_data* m_prev_data;


	lod_chunk_data(tu_file* in)
	// Constructor.  Read our data & set our texture id.
	{
		// Load the main chunk data.
		m_verts.read(in);
	}


	~lod_chunk_data()
	// Destructor.
	{
	}

	int	render(const lod_chunk_tree& c, const lod_chunk& chunk, const view_state& v, cull::result_info cull_info, render_options opt,
		       const vec3& box_center, const vec3& box_extent);

	int	get_data_size() const
	// Return data bytes used by this object.
	{
		return sizeof(*this) + m_verts.get_data_size();
	}
};


struct lod_chunk {
//data:
	lod_chunk*	m_parent;
	lod_chunk*	m_children[4];

	union {
		int	m_label;
		lod_chunk*	m_chunk;
	} m_neighbor[4];

	// Chunk "address" (its position in the quadtree).
	Uint16	m_x, m_z;
	Uint8	m_level;

	bool	m_split;	// true if this node should be rendered by descendents.  @@ pack this somewhere as a bitflag.  LSB of lod?
	Uint16	m_lod;		// LOD of this chunk.  high byte never changes; low byte is the morph parameter.

	// Vertical bounds, for constructing bounding box.
	Sint16	m_min_y, m_max_y;

	long	m_data_file_position;
	lod_chunk_data*	m_data;
	unsigned int	m_texture_id;		// OpenGL texture id for this chunk's texture map.

//methods:
	~lod_chunk()
	{
		if (m_data) {
			delete m_data;
			m_data = 0;
		}

		release_texture();
	}

	void	release_texture()
	{
		if (m_texture_id) {
			lod_tile_freelist::free_texture(m_texture_id);
			m_texture_id = 0;
		}
	}

	void	clear();
	void	update(lod_chunk_tree* tree, const vec3& viewpoint);
	void	update_texture(lod_chunk_tree* tree, const vec3& viewpoint);

	void	do_split(lod_chunk_tree* tree, const vec3& viewpoint);
	bool	can_split(lod_chunk_tree* tree);	// return true if this chunk can split.  Also, request the necessary data for future, if not.
	void	unload_data();

	void	warm_up_data(lod_chunk_tree* tree, float priority);
	void	request_unload_subtree(lod_chunk_tree* tree);

	void	request_unload_textures(lod_chunk_tree* tree);

	int	render(const lod_chunk_tree& c, const view_state& v, cull::result_info cull_info, render_options opt, bool texture_bound);

	void	read(tu_file* in, int recursion_count, lod_chunk_tree* tree, bool vert_data_at_end);
	void	lookup_neighbors(lod_chunk_tree* tree);

	// Utilities.

	bool	has_resident_data() const
	{
		return m_data != NULL;
	}

	bool	has_children() const
	{
		return m_children[0] != NULL;
	}

	int	get_level() const
	{
		return m_level;
	}

	void	compute_bounding_box(const lod_chunk_tree& tree, vec3* box_center, vec3* box_extent)
	{
		float	level_factor = (float) (1 << (tree.m_tree_depth - 1 - m_level));

		box_center->y = (m_max_y + m_min_y) * 0.5f * tree.m_vertical_scale;
		box_extent->y = (m_max_y - m_min_y) * 0.5f * tree.m_vertical_scale;
			     
		box_center->x = (m_x + 0.5f) * level_factor * tree.m_base_chunk_dimension;
		box_center->z = (m_z + 0.5f) * level_factor * tree.m_base_chunk_dimension;
			     
		const float  	EXTRA_BOX_SIZE = 1e-3f;	// this is to make chunks overlap by about a millimeter, to avoid cracks.
		box_extent->x = level_factor * tree.m_base_chunk_dimension * 0.5f + EXTRA_BOX_SIZE;
		box_extent->z = box_extent->get_x();
	}
};


//
// chunk_tree_loader -- helper for lod_chunk_tree that handles the
// background loader thread.
//


class chunk_tree_loader
{
public:
	chunk_tree_loader(lod_chunk_tree* tree, tu_file* src);	// @@ should make tqt* a parameter.
	~chunk_tree_loader();

	void	sync_loader_thread();	// called by lod_chunk_tree::update(), to implement any changes that are ready.
	void	request_chunk_load(lod_chunk* chunk, float urgency);
	void	request_chunk_unload(lod_chunk* chunk);

	void	request_chunk_load_texture(lod_chunk* chunk);
	void	request_chunk_unload_texture(lod_chunk* chunk);

	tu_file*	get_source() { return m_source_stream; }

	// Call this to enable/disable the background loader thread.
	void	set_use_loader_thread(bool use);

	int	loader_thread();		// thread function!
private:
	void	start_loader_thread();
	bool	loader_service_data();		// in loader thread
	bool	loader_service_texture();	// in loader thread
	
	struct pending_load_request
	{
		lod_chunk*	m_chunk;
		float	m_priority;

		pending_load_request() : m_chunk(NULL), m_priority(0.0f) {}
		
		pending_load_request(lod_chunk* chunk, float priority)
			: m_chunk(chunk), m_priority(priority)
		{
		}

		static int	compare(const void* r1, const void* r2)
		// Comparison function for qsort.  Sort based on priority.
		{
			float	p1 = ((pending_load_request*) r1)->m_priority;
			float	p2 = ((pending_load_request*) r2)->m_priority;
			
			if (p1 < p2) { return -1; }
			else if (p1 > p2) { return 1; }
			else { return 0; }
		}
	};

	struct retire_info
	// A struct that associates a chunk with its newly loaded
	// data.  For communicating between m_loader_thread and the
	// main thread.
	{
		lod_chunk*	m_chunk;
		lod_chunk_data*	m_chunk_data;

		retire_info() : m_chunk(0), m_chunk_data(0) {}
	};

	struct retire_texture_info
	// Associates a chunk with its newly loaded texture data.
	{
		lod_chunk*	m_chunk;
		image::rgb*	m_texture_image;

		retire_texture_info() : m_chunk(0), m_texture_image(0) {}
	};

	lod_chunk_tree*	m_tree;
	tu_file*	m_source_stream;

	// These two are for the main thread's use only.  For
	// update()/update_texture() to communicate with
	// sync_loader_thread().
	array<lod_chunk*>	m_unload_queue;
	array<pending_load_request>	m_load_queue;

	array<lod_chunk*>	m_unload_texture_queue;
	array<lod_chunk*>	m_load_texture_queue;

	// These two are for the main thread to communicate with the
	// loader thread & vice versa.
#define REQUEST_BUFFER_SIZE 4
	lod_chunk* volatile	m_request_buffer[REQUEST_BUFFER_SIZE];	// chunks waiting to be loaded; filled will NULLs otherwise.
	retire_info volatile	m_retire_buffer[REQUEST_BUFFER_SIZE];	// chunks waiting to be united with their loaded data.

	lod_chunk* volatile	m_request_texture_buffer[REQUEST_BUFFER_SIZE];		// chunks waiting for their textures to be loaded.
	retire_texture_info volatile	m_retire_texture_buffer[REQUEST_BUFFER_SIZE];	// chunks waiting to be united with their loaded textures.

	// Loader thread stuff.
	SDL_Thread*	m_loader_thread;
	volatile bool	m_run_loader_thread;	// loader thread watches for this to go false, then exits.
	SDL_mutex*	m_mutex;
};


chunk_tree_loader::chunk_tree_loader(lod_chunk_tree* tree, tu_file* src)
// Constructor.  Retains internal copies of the given pointers.
{
	m_tree = tree;
	m_source_stream = src;

	for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
	{
		m_request_buffer[i] = NULL;
		m_retire_buffer[i].m_chunk = NULL;
		m_request_texture_buffer[i] = NULL;
		m_retire_texture_buffer[i].m_chunk = NULL;
	}

	// Set up thread communication stuff.
	m_mutex = SDL_CreateMutex();
	assert(m_mutex);
	start_loader_thread();
}


chunk_tree_loader::~chunk_tree_loader()
// Destructor.  Make sure thread is done.
{
	// Make sure to kill the loader thread.
	set_use_loader_thread(false);

	SDL_DestroyMutex(m_mutex);
	m_mutex = NULL;
}


void	chunk_tree_loader::start_loader_thread()
// Initiate the loader thread, then return.
{
	m_run_loader_thread = true;

	// Thunk to wrap loader_thread(), which is a member fn.
	struct wrapper {
		static int	thread_wrapper(void* loader)
		{
			return ((chunk_tree_loader*) loader)->loader_thread();
		}
	};
	m_loader_thread = SDL_CreateThread(wrapper::thread_wrapper, this);
}


void	chunk_tree_loader::set_use_loader_thread(bool use)
// Call this to enable/disable the use of a background loader thread.
// May take a moment of latency to return, since if the background
// thread is active, then this function has to signal it and wait for
// it to terminate.
{
	if (m_run_loader_thread) {
		assert(m_loader_thread);
		if (use) {
			// We're already using the thread; nothing to do.
			return;
		}
		else {
			// Thread is running -- kill it.
			m_run_loader_thread = false;
			SDL_WaitThread(m_loader_thread, NULL);
			m_loader_thread = NULL;
			return;
		}
	}
	else {
		assert(m_loader_thread == NULL);
		if (use == false) {
			// We're already not using the loader thread; nothing to do.
			return;
		} else {
			// Thread is not running -- start it up.
			start_loader_thread();
			assert(m_loader_thread);
		}
	}
}


void	chunk_tree_loader::sync_loader_thread()
// Call this periodically, to implement previously requested changes
// to the lod_chunk_tree.  Most of the work in preparing changes is
// done in a background thread, so this call is intended to be
// low-latency.
//
// The chunk_tree_loader is not allowed to make any changes to the
// lod_chunk_tree, except in this call.
{
	// mutex section
	SDL_LockMutex(m_mutex);
	{
		// Unload data.
		for (int i = 0; i < m_unload_queue.size(); i++) {
			lod_chunk*	c = m_unload_queue[i];
			// Only unload the chunk if it's not currently in use.
			// Sometimes a chunk will be marked for unloading, but
			// then is still being used due to a dependency in a
			// neighboring part of the hierarchy.  We want to
			// ignore the unload request in that case.
			if (c->m_parent != NULL
			    && c->m_parent->m_split == false)
			{
				c->unload_data();
			}
		}
		m_unload_queue.resize(0);

		// Unload textures.
		{for (int i = 0; i < m_unload_texture_queue.size(); i++) {
			lod_chunk*	c = m_unload_texture_queue[i];
			if (c->m_parent != NULL) {
				assert(c->m_parent->m_texture_id != 0);
				assert(c->has_children() == false
				       || (c->m_children[0]->m_texture_id == 0
					   && c->m_children[1]->m_texture_id == 0
					   && c->m_children[2]->m_texture_id == 0
					   && c->m_children[3]->m_texture_id == 0));
				
				c->release_texture();
			}
		}}
		m_unload_texture_queue.resize(0);

		// Retire any serviced requests.
		{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
		{
			retire_info&	r = const_cast<retire_info&>(m_retire_buffer[i]);	// cast away 'volatile' (we're inside the mutex section)
			if (r.m_chunk)
			{
				assert(r.m_chunk->m_data == NULL);

				if (r.m_chunk->m_parent != NULL
				    && r.m_chunk->m_parent->m_data == NULL)
				{
					// Drat!  Our parent data was unloaded, while we were
					// being loaded.  Only thing to do is discard the newly loaded
					// data, to avoid breaking an invariant.
					// (No big deal; this situation is rare.)
					delete r.m_chunk_data;
				}
				else
				{
					// Connect the chunk with its data!
					r.m_chunk->m_data = r.m_chunk_data;
				}
			}
			// Clear out this entry.
			r.m_chunk = NULL;
			r.m_chunk_data = NULL;
		}}

		// Retire any serviced texture requests.
		{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
		{
			retire_texture_info&	r = const_cast<retire_texture_info&>(m_retire_texture_buffer[i]);
			if (r.m_chunk)
			{
				assert(r.m_chunk->m_texture_id == 0);

				if (r.m_chunk->m_parent != NULL
				    && r.m_chunk->m_parent->m_texture_id == 0)
				{
					// Drat!  Our parent texture was unloaded, while we were
					// being loaded.  Only thing to do is to discard the
					// newly loaded image, to avoid breaking the invariant.
					// (No big deal; this situation is rare.)
					delete r.m_texture_image;
				}
				else
				{
					// Connect the chunk with its texture!
					r.m_chunk->m_texture_id = lod_tile_freelist::make_texture(r.m_texture_image);	// @@ this actually could cause some bad latency, because we build mipmaps...
				}
			}
			// Clear out this entry.
			r.m_chunk = NULL;
			r.m_texture_image = NULL;
		}}

		//
		// Pass new data requests to the loader thread.  Go in
		// order of priority, and only take a few.
		//

		// Wipe out stale requests.
		{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
			m_request_buffer[i] = NULL;
		}}

		// Fill in new requests.
		int	qsize = m_load_queue.size();
		if (qsize > 0)
		{
			int	req_count = 0;

			// Sort by priority.
			qsort(&m_load_queue[0], qsize, sizeof(m_load_queue[0]), pending_load_request::compare);
			{for (int i = 0; i < qsize; i++)
			{
				lod_chunk*	c = m_load_queue[qsize - 1 - i].m_chunk;	// Do the higher priority requests first.
				// Must make sure the chunk wasn't just retired.
				if (c->m_data == NULL
				    && (c->m_parent == NULL || c->m_parent->m_data != NULL))
				{
					// Request this chunk.
					m_request_buffer[req_count++] = c;
					if (req_count >= REQUEST_BUFFER_SIZE) {
						// We've queued up enough requests.
						break;
					}
				}
			}}
		
			m_load_queue.resize(0);	// forget this frame's requests; we'll generate a fresh list during the next update()
		}

		//
		// Pass texture requests.
		//

		// Wipe out stale requests.
		{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
			m_request_texture_buffer[i] = NULL;
		}}

		int	tqsize = m_load_texture_queue.size();
		if (tqsize > 0)
		{
			int	req_count = 0;

			{for (int i = 0; i < tqsize; i++)
			{
				lod_chunk*	c = m_load_texture_queue[i];
				// Must make sure the chunk wasn't
				// just retired, and also that its
				// parent wasn't just unloaded.
				if (c->m_texture_id == 0
				    && (c->m_parent == NULL || c->m_parent->m_texture_id != 0))
				{
					// Request this chunk.
					m_request_texture_buffer[req_count++] = c;
					if (req_count >= REQUEST_BUFFER_SIZE) {
						// We've queued up enough requests.
						break;
					}
				}
			}}
		
			m_load_texture_queue.resize(0);	// forget this frame's requests; we'll generate a fresh list during the next update()
		}
	}
	SDL_UnlockMutex(m_mutex);


	if (m_run_loader_thread == false)
	{
		// Loader thread is not actually running (at client
		// request, via set_use_loader_thread()), so instead,
		// service any requests synchronously, right now.
		int	count;
		for (count = 0; count < 4; count++) {
			bool	loaded = loader_service_data();
			if (loaded == false) break;
		}
		for (count = 0; count < 4; count++) {
			bool	loaded = loader_service_texture();
			if (loaded == false) break;
		}
	}
}


void	chunk_tree_loader::request_chunk_load(lod_chunk* chunk, float urgency)
// Request that the specified chunk have its data loaded.  May
// take a while; data doesn't actually show up & get linked in
// until some future call to sync_loader_thread().
{
	assert(chunk);
	assert(chunk->m_data == NULL);

	// Don't schedule for load unless our parent already has data.
	if (chunk->m_parent == NULL
	    || chunk->m_parent->m_data != NULL)
	{
#if 0
		// See if we're in the request queue already.
		// Ugh, N^2.  TODO use a hash to do this lookup.
		for (int i = 0; i < m_load_queue.size(); i++) {
			if (m_load_queue[i].m_chunk == chunk) {
				// Already listed; use the max priority.
				m_load_queue[i].m_priority = fmax(urgency, m_load_queue[i].m_priority);
				return;
			}
		}
		// Not listed already.
#endif // 0

		m_load_queue.push_back(pending_load_request(chunk, urgency));
		
	}
}


void	chunk_tree_loader::request_chunk_unload(lod_chunk* chunk)
// Request that the specified chunk have its data unloaded;
// happens within short latency.
{
	m_unload_queue.push_back(chunk);
}


void	chunk_tree_loader::request_chunk_load_texture(lod_chunk* chunk)
// Request that the specified chunk have its texture loaded.  May
// take a while; data doesn't actually show up & get linked in
// until some future call to sync_loader_thread().
{
	assert(chunk);
	assert(chunk->m_texture_id == 0);
	assert(m_tree->m_texture_quadtree->get_depth() >= chunk->m_level);

	// Don't schedule for load unless our parent already has a texture.
	if (chunk->m_parent == NULL
	    || chunk->m_parent->m_texture_id != 0)
	{
		m_load_texture_queue.push_back(chunk);
	}
}


void	chunk_tree_loader::request_chunk_unload_texture(lod_chunk* chunk)
// Request that the specified chunk have its texture unloaded; happens
// within short latency.
{
	assert(chunk->m_texture_id != 0);
	
	m_unload_texture_queue.push_back(chunk);
}


int	chunk_tree_loader::loader_thread()
// Thread function for the loader thread.  Sit and load chunk data
// from the request queue, until we get killed.
{
	while (m_run_loader_thread == true)
	{
		bool	loaded = false;
		loaded = loader_service_data() || loaded;
		loaded = loader_service_texture() || loaded;

		if (loaded == false)
		{
			// We seem to be dormant; sleep for a while
			// and then check again.
			SDL_Delay(10);
		}
	}

	return 0;
}


bool	chunk_tree_loader::loader_service_data()
// Service a request for data.  Return true if we actually serviced
// anything; false if there was nothing to service.
{
	// Grab a request.
	lod_chunk*	chunk_to_load = NULL;
	SDL_LockMutex(m_mutex);
	{
		// Get first request that's not already in the
		// retire buffer.
		for (int req = 0; req < REQUEST_BUFFER_SIZE; req++)
		{
			chunk_to_load = m_request_buffer[0];	// (could be NULL)

			// shift requests down.
			int	i;
			for (i = 0; i < REQUEST_BUFFER_SIZE - 1; i++)
			{
				m_request_buffer[i] = m_request_buffer[i + 1];
			}
			m_request_buffer[i] = NULL;	// fill empty slot with NULL

			if (chunk_to_load == NULL) break;
			
			// Make sure the request is not in the retire buffer.
			bool	in_retire_buffer = false;
			{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
				if (m_retire_buffer[i].m_chunk == chunk_to_load) {
					// This request has already been serviced.  Don't
					// service it again.
					chunk_to_load = NULL;
					in_retire_buffer = true;
					break;
				}
			}}
			if (in_retire_buffer == false) break;
		}
	}
	SDL_UnlockMutex(m_mutex);

	if (chunk_to_load == NULL)
	{
		// There's no request to service right now.
		return false;
	}

	assert(chunk_to_load->m_data == NULL);
	assert(chunk_to_load->m_parent == NULL || chunk_to_load->m_parent->m_data != NULL);
	
	// Service the request by loading the chunk's data.  This
	// could take a while, and involves waiting on IO, so we do it
	// with the mutex unlocked so the main update/render thread
	// can hopefully get some work done.
	lod_chunk_data*	loaded_data = NULL;

	// Geometry.
	m_source_stream->set_position(chunk_to_load->m_data_file_position);
	loaded_data = new lod_chunk_data(m_source_stream);

	// "Retire" the request.  Must do this with the mutex locked.
	// The main thread will do "chunk_to_load->m_data = loaded_data".
	SDL_LockMutex(m_mutex);
	{
		for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
		{
			if (m_retire_buffer[i].m_chunk == 0)
			{
				// empty slot; put the info here.
				m_retire_buffer[i].m_chunk = chunk_to_load;
				m_retire_buffer[i].m_chunk_data = loaded_data;
				break;
			}
		}
		// TODO: assert if we didn't find a retire slot!
		// (there should always be one, because it's as big as
		// the request queue)
	}
	SDL_UnlockMutex(m_mutex);

	return true;
}


bool	chunk_tree_loader::loader_service_texture()
// Service a pending texture request.  Return true if we serviced
// something.  Return false if there was nothing to service.
{
	// Grab a request.
	lod_chunk*	chunk_to_load = NULL;
	SDL_LockMutex(m_mutex);
	{
		// Get first request that's not already in the
		// retire buffer.
		for (int req = 0; req < REQUEST_BUFFER_SIZE; req++)
		{
			chunk_to_load = m_request_texture_buffer[0];	// (could be NULL)

			// shift requests down.
			int	i;
			for (i = 0; i < REQUEST_BUFFER_SIZE - 1; i++)
			{
				m_request_texture_buffer[i] = m_request_texture_buffer[i + 1];
			}
			m_request_texture_buffer[i] = NULL;	// fill empty slot with NULL

			if (chunk_to_load == NULL) break;
			assert(chunk_to_load->m_texture_id == 0);
			
			// Make sure the request is not in the retire buffer.
			bool	in_retire_buffer = false;
			{for (int i = 0; i < REQUEST_BUFFER_SIZE; i++) {
				if (m_retire_texture_buffer[i].m_chunk == chunk_to_load) {
					// This request has already been serviced.  Don't
					// service it again.
					chunk_to_load = NULL;
					in_retire_buffer = true;
					break;
				}
			}}
			if (in_retire_buffer == false) break;
		}
	}
	SDL_UnlockMutex(m_mutex);

	if (chunk_to_load == NULL)
	{
		// There's no request to service right now.
		return false;
	}

	assert(chunk_to_load->m_texture_id == 0);
	assert(chunk_to_load->m_parent == NULL || chunk_to_load->m_parent->m_texture_id != 0);
	
	// Service the request by loading the chunk's data.
	// This could take a while, and involves wating on IO,
	// so we do it with the mutex unlocked so the main
	// update/render thread can hopefully get some work
	// done.
	image::rgb*	texture_image = NULL;

	// Texture.
	const tqt*	qt = m_tree->m_texture_quadtree;
	assert(qt && chunk_to_load->m_level < qt->get_depth());

	texture_image = qt->load_image(chunk_to_load->m_level,
				       chunk_to_load->m_x,
				       chunk_to_load->m_z);

	// "Retire" the request.  Must do this with the mutex
	// locked.  The main thread will do
	// "chunk_to_load->m_data = loaded_data".
	SDL_LockMutex(m_mutex);
	{
		for (int i = 0; i < REQUEST_BUFFER_SIZE; i++)
		{
			if (m_retire_texture_buffer[i].m_chunk == 0)
			{
				// empty slot; put the info here.
				m_retire_texture_buffer[i].m_chunk = chunk_to_load;
				m_retire_texture_buffer[i].m_texture_image = texture_image;
				break;
			}
		}
	}
	SDL_UnlockMutex(m_mutex);

	return true;
}


//
// lod_chunk stuff
//


#ifdef MAP_ONTO_CUBEMAP


void	map_heightfield_onto_cubemap(vec3* out, const vec3& in)
// Take a heightfield coordinate, and map it into 3D such that the
// heightfield corresponds to the face of a cube-map, mapped onto a
// sphere.
{
	float	world_radius = s_max_horizontal_dimension/2;

	vec3	v(in.x / world_radius - 1.0f, 1.0f, in.z / world_radius - 1.0f);
	v.normalize();	// ouch

	v *= world_radius + in.y;
	v.y -= world_radius;

	*out = v;
}


void	map_cubemap_onto_heightfield(vec3* out, const vec3& in)
// Given a 3D world coordinate (where the heightfield appears as a
// curved part of a cubemap face), warp it into heightfield
// coordinates.
{
	float	world_radius = s_max_horizontal_dimension/2;

	// Avoid divide by zero
	if (fabs(in.y) < 1e-6)
	{
		*out = vec3(0, -world_radius, 0);
		return;
	}

	vec3	v(in.x, in.y + world_radius, in.z);
	vec3	p(v.x / v.y, 1, v.z / v.y);
	float	r = v.magnitude();

	out->x = (p.x + 1.0f) * world_radius;
	out->y = (r - world_radius);
	out->z = (p.z + 1.0f) * world_radius;
}


#endif // MAP_ONTO_CUBEMAP


#ifdef MAP_ONTO_MERCATOR


void map_heightfield_onto_mercator(vec3* out, const vec3& in)
// Take a heightfield coordinate and map it into 3D such that the
// heightfield corresponds to a mercator projection map, wrapped onto
// a sphere.
{
	// Horizontal dimension == planetary radius.
	float world_radius = s_max_horizontal_dimension / float(2 * M_PI);

	// @@ This is probably full of sign errors, and it obviously
	// is not efficient.

	// @@ precompute M_PI / s_max_horizontal_dimension

	float theta = -((in.x / s_max_horizontal_dimension) - 0.5f) * 2.0f * float(M_PI);

	float sin_phi = ((in.z / s_max_horizontal_dimension) - 0.5f) * 2.0f;
	float phi = asinf(fclamp(sin_phi, -1, 1));
	float r = (in.y + world_radius);

	float sin_theta = sinf(theta);
	float cos_theta = cosf(theta);
	float cos_phi = cosf(phi);

	out->x = cos_theta * cos_phi * r;
	out->y = sin_phi * r;
	out->z = -sin_theta * cos_phi * r;
}


void	map_mercator_onto_heightfield(vec3* out, const vec3& in)
// Given a 3D world coordinate (where the heightfield is a mercator
// projection wrapped around a sphere), warp it into flattened-out
// heightfield coordinates.
{
	// Horizontal dimension == planetary radius.
	float world_radius = s_max_horizontal_dimension / float(2 * M_PI);

	// @@ This may all be totally wrong.

	float	r = in.magnitude();
	float	sin_phi = in.y / r;
	float	phi = float(M_PI);
	float	theta = 0;
	if (fabs(sin_phi) + 1e-6f < 1.0f)
	{
		// Not on either pole.
		theta = atan2f(-in.z, in.x);
	}

	out->x = theta / float(M_PI) * s_max_horizontal_dimension;
	out->z = phi / float(M_PI) * s_max_horizontal_dimension;
	out->y = r / world_radius - 1.0f;
}


#endif // MAP_ONTO_MERCATOR


static void	morph_vertices(
	float* verts,
	const vertex_info& morph_verts,
	const vec3& box_center,
	const vec3& box_extent,
	float f)
// Adjust the positions of our morph vertices according to f, the
// given morph parameter.  verts is the output buffer for processed
// verts.
//
// The input is in chunk-local 16-bit signed coordinates.  The given
// box_center/box_extent parameters are used to produce the correct
// world-space coordinates.  The quantizing to 16 bits is a way to
// compress the input data.
//
// @@ This morphing/decompression functionality should be shifted into
// a vertex program for the GPU where possible.
{
	// Do quantization decompression, output floats.

	const float	sx = box_extent.get_x() / (1 << 14);
	const float	sz = box_extent.get_z() / (1 << 14);

	const float	offsetx = box_center.get_x();
	const float	offsetz = box_center.get_z();

	const float	one_minus_f = (1.0f - f);

	for (int i = 0; i < morph_verts.vertex_count; i++) {
		const vertex_info::vertex&	v = morph_verts.vertices[i];
		verts[i*3 + 0] = offsetx + v.x[0] * sx;
		verts[i*3 + 1] = (v.x[1] + v.y_delta * one_minus_f) * s_vertical_scale;	// lerp the y value of the vert.
		verts[i*3 + 2] = offsetz + v.x[2] * sz;

#ifdef MAP_ONTO_CUBEMAP
		map_heightfield_onto_cubemap((vec3*)(&verts[i*3 + 0]), *(vec3*)(&verts[i*3 + 0]));
#endif // MAP_ONTO_CUBEMAP
#ifdef MAP_ONTO_MERCATOR
		map_heightfield_onto_mercator((vec3*)(&verts[i*3 + 0]), *(vec3*)(&verts[i*3 + 0]));
#endif // MAP_ONTO_MERCATOR
	}
#if 0
	// With a vshader, this routine would be replaced by an initial agp_alloc() & memcpy()/memmap().
#endif // 0
}


static void	bind_texture_tile(unsigned int texid, const vec3& box_center, const vec3& box_extent)
// Bind the given texture and set up texgen so that it stretches over
// the x-z extent of the given box.
{
	s_textures_bound++;	// stats

	glBindTexture(GL_TEXTURE_2D, texid);
	glEnable(GL_TEXTURE_2D);
		
	float	xsize = box_extent.get_x() * 2 * (257.0f / 256.0f);
	float	zsize = box_extent.get_z() * 2 * (257.0f / 256.0f);
	float	x0 = box_center.get_x() - box_extent.get_x() - (xsize / 256.0f) * 0.5f;
	float	z0 = box_center.get_z() - box_extent.get_z() - (xsize / 256.0f) * 0.5f;

	// Set up texgen for this tile.
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	float	p[4] = { 0, 0, 0, 0 };
	p[0] = 1.0f / xsize;
	p[3] = -x0 / xsize;
	glTexGenfv(GL_S, GL_OBJECT_PLANE, p);
	p[0] = 0;
			
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	p[2] = 1.0f / zsize;
	p[3] = -z0 / zsize;
	glTexGenfv(GL_T, GL_OBJECT_PLANE, p);
}


int	lod_chunk_data::render(const lod_chunk_tree& c, const lod_chunk& chunk, const view_state& v, cull::result_info cull_info, render_options opt,
			       const vec3& box_center, const vec3& box_extent)
// Render a chunk.
{
	int	triangle_count = 0;

	// Grab some space to put processed verts.
//	assert(s_stream);
	float*	output_verts = (float*) ogl::stream_get_vertex_memory(sizeof(float) * 3 * m_verts.vertex_count);

	// Process our vertices into the output buffer.
	float	f = (chunk.m_lod & 255) / 255.0f;
	if (opt.morph == false) {
		f = 0;
	}
	morph_vertices(output_verts, m_verts, box_center, box_extent, f);

	if (opt.show_geometry) {
		ogl::stream_flush_combiners();

		// draw this chunk.
		glColor3f(1, 1, 1);
		glVertexPointer(3, GL_FLOAT, 0, output_verts);
		glDrawElements(GL_TRIANGLE_STRIP, m_verts.index_count, GL_UNSIGNED_SHORT, m_verts.indices);
		triangle_count += m_verts.triangle_count;
	}

	return triangle_count;
}


static void	draw_box(const vec3& min, const vec3& max)
// Draw the specified axis-aligned box.
{
	glBegin(GL_LINES);
	glVertex3f(min.get_x(), min.get_y(), min.get_z());
	glVertex3f(min.get_x(), max.get_y(), min.get_z());
	glVertex3f(min.get_x(), min.get_y(), max.get_z());
	glVertex3f(min.get_x(), max.get_y(), max.get_z());
	glVertex3f(max.get_x(), min.get_y(), min.get_z());
	glVertex3f(max.get_x(), max.get_y(), min.get_z());
	glVertex3f(max.get_x(), min.get_y(), max.get_z());
	glVertex3f(max.get_x(), max.get_y(), max.get_z());
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(min.get_x(), min.get_y(), min.get_z());
	glVertex3f(min.get_x(), min.get_y(), max.get_z());
	glVertex3f(max.get_x(), min.get_y(), max.get_z());
	glVertex3f(max.get_x(), min.get_y(), min.get_z());
	glVertex3f(min.get_x(), min.get_y(), min.get_z());
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(min.get_x(), max.get_y(), min.get_z());
	glVertex3f(min.get_x(), max.get_y(), max.get_z());
	glVertex3f(max.get_x(), max.get_y(), max.get_z());
	glVertex3f(max.get_x(), max.get_y(), min.get_z());
	glVertex3f(min.get_x(), max.get_y(), min.get_z());
	glEnd();
}


static void	count_chunk_stats(lod_chunk* c)
// Add up stats from c and its descendents.
{
	if (c->m_data)
	{
		s_chunks_with_data++;
		s_bytes_in_use += c->m_data->get_data_size();

		if (c->has_children()) {
			for (int i = 0; i < 4; i++) {
				count_chunk_stats(c->m_children[i]);
			}
		}
	}
	// else don't recurse; our descendents should not have any
	// data.  TODO: assert on this.
}


void	lod_chunk::clear()
// Clears the m_split values throughout the subtree.  If this node is
// not split, then the recursion does not continue to the child nodes,
// since their m_split values should be false.
//
// Do this before calling update().
{
	assert(m_data != NULL);

	s_chunks_in_use++;
	s_chunks_with_data++;
	s_bytes_in_use += m_data->get_data_size();
	if (m_texture_id) { s_chunks_with_texture++; }

	if (m_split) {
		m_split = false;

		// Recurse to children.
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				m_children[i]->clear();
			}
		}
	}
	else
	{
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				count_chunk_stats(m_children[i]);
			}
		}
	}
}


void	lod_chunk::update(lod_chunk_tree* tree, const vec3& viewpoint)
// Computes 'lod' and split values for this chunk and its subtree,
// based on the given camera parameters and the parameters stored in
// 'base'.  Traverses the tree and forces neighbor chunks to a valid
// LOD, and updates its contained edges to prevent cracks.
//
// Invariant: if a node has m_split == true, then all its ancestors
// have m_split == true.
//
// Invariant: if a node has m_data != NULL, then all its ancestors
// have m_data != NULL.
//
// !!!  For correct results, the tree must have been clear()ed before
// calling update() !!!
{
	vec3	box_center, box_extent;
	compute_bounding_box(*tree, &box_center, &box_extent);

	Uint16	desired_lod = tree->compute_lod(box_center, box_extent, viewpoint);

	if (has_children()
		&& desired_lod > (m_lod | 0x0FF)
		&& can_split(tree))
	{
		do_split(tree, viewpoint);

		// Recurse to children.
		for (int i = 0; i < 4; i++) {
			m_children[i]->update(tree, viewpoint);
		}
	} else {
		// We're good... this chunk can represent its region within the max error tolerance.
		if ((m_lod & 0xFF00) == 0) {
			// Root chunk -- make sure we have valid morph value.
			m_lod = iclamp(desired_lod, m_lod & 0xFF00, m_lod | 0x0FF);

			assert((m_lod >> 8) == m_level);
		}

		// Request residency for our children, and request our
		// grandchildren and further descendents be unloaded.
		if (has_children()) {
			float	priority = 0;
			if (desired_lod > (m_lod & 0xFF00)) {
				priority = (m_lod & 0x0FF) / 255.0f;
			}

			if (priority < 0.5f) {
				for (int i = 0; i < 4; i++) {
					m_children[i]->request_unload_subtree(tree);
				}
			}
			else
			{
				for (int i = 0; i < 4; i++) {
					m_children[i]->warm_up_data(tree, priority);
				}
			}
		}
	}
}


void	lod_chunk::update_texture(lod_chunk_tree* tree, const vec3& viewpoint_in)
// Decides when to load & release textures for this node and its descendents.
//
// Invariant: if a node has a non-zero m_texture_id, then all its
// ancestors have non-zero m_texture_id.
//
// !!! You must do things in this order: clear(), then update(), then texture_update(). !!!
//
// Note that we don't take the viewpoint direction into account when
// loading textures, only the viewpoint distance & mag factor.  That's
// intentional: it means we'll load a lot of textures that are beside
// or behind the frustum, but the viewpoint can rotate rapidly, so we
// really want them available.  The penalty is ~4x texture RAM, for a
// 90-degree FOV, compared to only loading what's in the frustum.
// Interestingly, this approach ceases to work as the view angle gets
// smaller.  LOD in general has real problems as we approach an
// isometric view.  For games this isn't a huge concern, (except for
// sniper-scope views, where it's a real issue, but then there are
// compensating factors there too...).
{
#ifdef MAP_ONTO_CUBEMAP
	vec3	viewpoint;
	map_cubemap_onto_heightfield(&viewpoint, viewpoint_in);
#else
#ifdef MAP_ONTO_MERCATOR
	vec3	viewpoint;
	map_mercator_onto_heightfield(&viewpoint, viewpoint_in);
#else
	const vec3&	viewpoint = viewpoint_in;
#endif
#endif

	assert(tree->m_texture_quadtree != NULL);

	if (m_level >= tree->m_texture_quadtree->get_depth())
	{
		// No texture tiles at this level, so don't bother
		// thinking about them.
		assert(m_texture_id == 0);
		return;
	}

	vec3	box_center, box_extent;
	compute_bounding_box(*tree, &box_center, &box_extent);

	int	desired_tex_level = tree->compute_texture_lod(box_center, box_extent, viewpoint);

	if (m_texture_id != 0)
	{
		assert(m_parent == NULL || m_parent->m_texture_id != 0);

		// Decide if we should release our texture.
		if (m_data == NULL
		    || desired_tex_level < m_level)
		{
			// Release our texture, and the texture of any
			// descendents.  Really should go into a cache
			// or something, in case we want to revive it
			// soon.
			request_unload_textures(tree);
		}
		else
		{
			// Keep status quo for this node, and recurse to children.
			if (has_children()) {
				for (int i = 0; i < 4; i++) {
					m_children[i]->update_texture(tree, viewpoint);
				}
			}
		}
	}
	else
	{
		// Decide if we should load our texture.
		if (desired_tex_level >= m_level
		    && m_data)
		{
			// Yes, we would like to load.
			tree->m_loader->request_chunk_load_texture(this);
		}
		else
		{
			// No need to load anything, or to check children.
		}

#ifndef NDEBUG
		// Check to make sure children don't have m_texture_id's.
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				assert(m_children[i]->m_texture_id == 0);
			}
		}
#endif // not NDEBUG
	}
}


void	lod_chunk::do_split(lod_chunk_tree* tree, const vec3& viewpoint)
// Enable this chunk.  Use the given viewpoint to decide what morph
// level to use.
//
// If able to split this chunk, then the function returns true.  If
// unable to split this chunk, then the function doesn't change the
// tree, and returns false.
{
	if (m_split == false) {
		assert(this->can_split(tree));
		assert(has_resident_data());

		m_split = true;

		if (has_children()) {
			// Make sure children have a valid lod value.
			{for (int i = 0; i < 4; i++) {
				lod_chunk*	c = m_children[i];
				vec3	box_center, box_extent;
				c->compute_bounding_box(*tree, &box_center, &box_extent);
				Uint16	desired_lod = tree->compute_lod(box_center, box_extent, viewpoint);
				c->m_lod = iclamp(desired_lod, c->m_lod & 0xFF00, c->m_lod | 0x0FF);
			}}
		}

		// make sure ancestors are split...
		for (lod_chunk* p = m_parent; p && p->m_split == false; p = p->m_parent) {
			p->do_split(tree, viewpoint);
		}
	}
}


bool	lod_chunk::can_split(lod_chunk_tree* tree)
// Return true if this chunk can be split.  Also, requests the
// necessary data for the chunk children and its dependents.
//
// A chunk won't be able to be split if it doesn't have vertex data,
// or if any of the dependent chunks don't have vertex data.
{
	if (m_split) {
		// Already split.  Also our data & dependents' data is already
		// freshened, so no need to request it again.
		return true;
	}

	if (has_children() == false) {
		// Can't ever split.  No data to request.
		return false;
	}

	bool	can_split = true;

	// Check the data of the children.
	{for (int i = 0; i < 4; i++) {
		lod_chunk*	c = m_children[i];
		if (c->has_resident_data() == false) {
			tree->m_loader->request_chunk_load(c, 1.0f);
			can_split = false;
		}
	}}

	// Make sure ancestors have data...
	for (lod_chunk* p = m_parent; p && p->m_split == false; p = p->m_parent) {
		if (p->can_split(tree) == false) {
			can_split = false;
		}
	}

	// Make sure neighbors have data at a close-enough level in the tree.
	{for (int i = 0; i < 4; i++) {
		lod_chunk*	n = m_neighbor[i].m_chunk;

		const int	MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE = 2;	// allow up to two levels of difference between chunk neighbors.
		{for (int count = 0;
		     n && count < MAXIMUM_ALLOWED_NEIGHBOR_DIFFERENCE;
		     count++)
		{
			n = n->m_parent;
		}}

		if (n && n->can_split(tree) == false) {
			can_split = false;
		}
	}}

	return can_split;
}


void	lod_chunk::unload_data()
// Immediately unload our data.
{
	assert(m_parent != NULL && m_parent->m_split == false);
	assert(m_split == false);

	// debug check -- we should only unload data from the leaves
	// upward.
	if (has_children()) {
		for (int i = 0; i < 4; i++) {
			assert(m_children[i]->m_data == NULL);
		}
	}

	// Do the unloading.
	if (m_data) {
		delete m_data;
		m_data = NULL;
	}
}


void	lod_chunk::warm_up_data(lod_chunk_tree* tree, float priority)
// Schedule this node's data for loading at the given priority.  Also,
// schedule our child/descendent nodes for unloading.
{
	assert(tree);

	if (m_data == NULL)
	{
		// Request our data.
		tree->m_loader->request_chunk_load(this, priority);
	}

	// Request unload.  Skip a generation if our priority is
	// fairly high.
	if (has_children()) {
		if (priority < 0.5f) {
			// Dump our child nodes' data.
			for (int i = 0; i < 4; i++) {
				m_children[i]->request_unload_subtree(tree);
			}
		}
		else
		{
			// Fairly high priority; leave our children
			// loaded, but dump our grandchildren's data.
			for (int i = 0; i < 4; i++) {
				lod_chunk*	c = m_children[i];
				if (c->has_children()) {
					for (int j = 0; j < 4; j++) {
						c->m_children[j]->request_unload_subtree(tree);
					}
				}
			}
		}
	}
}


void	lod_chunk::request_unload_subtree(lod_chunk_tree* tree)
// If we have any data, request that it be unloaded.  Make the same
// request of our descendants.
{
	if (m_data) {
		// Put descendents in the queue first, so they get
		// unloaded first.
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				m_children[i]->request_unload_subtree(tree);
			}
		}

		tree->m_loader->request_chunk_unload(this);
	}
}


void	lod_chunk::request_unload_textures(lod_chunk_tree* tree)
// If we have a texture, request that it be unloaded.  Make the same
// request of our descendants.
{
	if (m_texture_id) {
		// Put descendents in the queue first, so they get
		// unloaded first.
		if (has_children()) {
			for (int i = 0; i < 4; i++) {
				m_children[i]->request_unload_textures(tree);
			}
		}

		tree->m_loader->request_chunk_unload_texture(this);
	}
}


int	lod_chunk::render(const lod_chunk_tree& c, const view_state& v, cull::result_info cull_info, render_options opt, bool texture_bound)
// Draws the given lod tree.  Uses the current state stored in the
// tree w/r/t split & LOD level.
//
// Returns the number of triangles rendered.
{
	assert(m_data != NULL);

	vec3	box_center, box_extent;
	compute_bounding_box(c, &box_center, &box_extent);

	// Frustum culling.
#ifdef MAP_ONTO_CUBEMAP

	// Need to cull differently in this case, or compute the box
	// differently, or something.  For now, don't cull.

#else	// not MAP_ONTO_CUBEMAP
#ifdef MAP_ONTO_MERCATOR

	// TODO figure out how to cull

#else	// not MAP_ONTO_MERCATOR

	if (cull_info.active_planes) {
		cull_info = cull::compute_box_visibility(box_center, box_extent, v.m_frustum, cull_info);
		if (cull_info.culled) {
			// Bounding box is not visible; no need to draw this node or its children.
			return 0;
		}
	}

#endif // not MAP_ONTO_MERCATOR
#endif // not MAP_ONTO_CUBEMAP

	int	triangle_count = 0;

	if (texture_bound == false && opt.show_geometry == true) {
		// Decide whether to bind a texture.
		
		if (m_texture_id == 0)
		{
			// No texture id, so nothing to bind in any case.
			assert(0);	// this should not happen!
		}
		else
		{
			// If there's no possibility of binding a
			// texture further down the tree, then bind
			// now.
			if (m_split == false
			    || m_children[0]->m_texture_id == 0
			    || m_children[1]->m_texture_id == 0
			    || m_children[2]->m_texture_id == 0
			    || m_children[3]->m_texture_id == 0)
			{
				bind_texture_tile(m_texture_id, box_center, box_extent);
				texture_bound = true;
			}
		}
	}


	if (m_split) {
		assert(has_children());

		// Recurse to children.  Some subset of our descendants will be rendered in our stead.
		for (int i = 0; i < 4; i++) {
			static const bool	explode = false;
			// EXPLODE (for showing the chunks very
			// explicitly)
			if (explode) {
				int	tree_height = c.m_tree_depth - ((this->m_lod) >> 8);
				float	offset = 30.f * ((1 << tree_height) / float(1 << c.m_tree_depth));

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				switch (i) {
				default:
				case 0:
					glTranslatef(-offset, 0, -offset);
					break;
				case 1:
					glTranslatef(offset, 0, -offset);
					break;
				case 2:
					glTranslatef(-offset, 0, offset);
					break;
				case 3:
					glTranslatef(offset, 0, offset);
					break;
				}
			}

			triangle_count += m_children[i]->render(c, v, cull_info, opt, texture_bound);

			// EXPLODE
			if (explode) {
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			}
		}
	} else {
		if (opt.show_box) {
			// draw bounding box.
			glDisable(GL_TEXTURE_2D);
			float	f = (m_lod & 255) / 255.0f;	//xxx
			glColor3f(f, 1 - f, 0);
			draw_box(box_center - box_extent, box_center + box_extent);
			glEnable(GL_TEXTURE_2D);
		}

		// Display our data.
		triangle_count += m_data->render(c, *this, v, cull_info, opt, box_center, box_extent);
	}

	return triangle_count;
}


void	lod_chunk::read(tu_file* in, int recurse_count, lod_chunk_tree* tree, bool vert_data_at_end)
// Read chunk data from the given file and initialize this chunk with it.
// Recursively loads child chunks for recurse_count > 0.
{
	m_split = false;
	m_texture_id = 0;
	m_data = NULL;

	// Get this chunk's label, and add it to the table.
	int	chunk_label = in->read_le32();
	if (chunk_label > tree->m_chunk_count)
	{
		assert(0);
		printf("invalid chunk_label: %d, recurse_count = %d\n", chunk_label, recurse_count);
		exit(1);
	}
	assert(tree->m_chunk_table[chunk_label] == 0);
	tree->m_chunk_table[chunk_label] = this;

	// Initialize neighbor links.
	{for (int i = 0; i < 4; i++) {
		m_neighbor[i].m_label = in->read_le32();
	}}

	// Read our chunk address.  We can reconstruct our bounding box
	// from this info, along with the root tree info.
	m_level = in->read_byte();
	m_x = in->read_le16();
	m_z = in->read_le16();

	m_min_y = in->read_le16();
	m_max_y = in->read_le16();

	// Skip the chunk data but remember our filepos, so we can load it
	// when it's demanded.
	if (vert_data_at_end)
	{
		// Format version 9 and later.  Vert data file offset
		// is just stored directly in the header.
		m_data_file_position = in->read_le32();
	}
	else
	{
		// Format version 8: vert data is in-line w/ chunk headers.
		// @@ Remove this code once I get rid of all my version 8 data.
		int	chunk_size = in->read_le32();
		m_data_file_position = in->get_position();
		in->set_position(m_data_file_position + chunk_size);

		assert(m_data_file_position + chunk_size == in->get_position());
	}

	// Recurse to child chunks.
	if (recurse_count > 0) {
		for (int i = 0; i < 4; i++) {
			m_children[i] = &tree->m_chunks[tree->m_chunks_allocated++];
			m_children[i]->m_lod = m_lod + 0x100;
			m_children[i]->m_parent = this;
			m_children[i]->read(in, recurse_count - 1, tree, vert_data_at_end);
		}
	} else {
		for (int i = 0; i < 4; i++) {
			m_children[i] = NULL;
		}
	}
}


void	lod_chunk::lookup_neighbors(lod_chunk_tree* tree)
// Convert our neighbor labels to neighbor pointers.  Recurse to child chunks.
{
	for (int i = 0; i < 4; i++) {
		if (m_neighbor[i].m_label < -1 || m_neighbor[i].m_label >= tree->m_chunk_count)
		{
			assert(0);
			m_neighbor[i].m_label = -1;
		}
		if (m_neighbor[i].m_label == -1) {
			m_neighbor[i].m_chunk = NULL;
		} else {
			m_neighbor[i].m_chunk = tree->m_chunk_table[m_neighbor[i].m_label];
		}
	}

	if (has_children()) {
		{for (int i = 0; i < 4; i++) {
			m_children[i]->lookup_neighbors(tree);
		}}
	}
}


//
// lod_chunk_tree implementation.  lod_chunk_tree is the external
// interface to chunked LOD.
//


lod_chunk_tree::lod_chunk_tree(tu_file* src, const tqt* texture_quadtree)
// Construct and initialize a tree of LOD chunks, using data from the given
// source.  Uses a special .chu file format which is a pretty direct
// encoding of the chunk data.
{
	m_texture_quadtree = texture_quadtree;

	// Read and verify a "CHU\0" header tag.
	Uint32	tag = src->read_le32();
	if (tag != (('C') | ('H' << 8) | ('U' << 16))) {
		printf("Input file is not in .CHU format");
		exit(1);
	}

	int	format_version = src->read_le16();
	if (format_version != 8 && format_version != 9)
	{
		printf("Input format has non-matching version number");
		exit(1);
	}

	bool	vert_data_at_end = false;
	if (format_version >= 9) {
		vert_data_at_end = true;
	}
	
	m_tree_depth = src->read_le16();
	m_error_LODmax = src->read_float32();
	m_vertical_scale = src->read_float32();
	m_base_chunk_dimension = src->read_float32();
	m_chunk_count = src->read_le32();

	// Create a lookup table of chunk labels.
	m_chunk_table = new lod_chunk*[m_chunk_count];
	memset(m_chunk_table, 0, sizeof(m_chunk_table[0]) * m_chunk_count);

	// Compute a sane default value for distance_LODmax, in case the client code
	// neglects to call set_parameters().
	set_parameters(5.0f, 1.0f, 640.0f, 90.0f);

	// Load the chunk tree (not the actual data).
	m_chunks_allocated = 0;
	m_chunks = new lod_chunk[m_chunk_count];

	m_chunks_allocated++;
	m_chunks[0].m_lod = 0;
	m_chunks[0].m_parent = 0;
	m_chunks[0].read(src, m_tree_depth-1, this, vert_data_at_end);
	m_chunks[0].lookup_neighbors(this);

	// Set up our loader.
	m_loader = new chunk_tree_loader(this, src);
}


lod_chunk_tree::~lod_chunk_tree()
// Destructor.
{
	delete [] m_chunk_table;
	delete [] m_chunks;
	delete m_loader;

	m_chunk_table = NULL;
	m_chunks = NULL;
	m_loader = NULL;
}


void	lod_chunk_tree::update(const vec3& viewpoint)
// Initializes tree state, so it can be rendered.  The given viewpoint
// is used to do distance-based LOD switching on our contained chunks.
{
	if (m_chunks[0].m_data == NULL)
	{
		// Get root-node data!
		m_loader->request_chunk_load(&m_chunks[0], 1.0f);
	}

	// Performance stats.  Count them up during clear().
	s_chunks_in_use = 0;
	s_chunks_with_data = 0;
	s_chunks_with_texture = 0;
	s_bytes_in_use = 0;

	if (m_chunks[0].m_split) {
		m_chunks[0].clear();
	}
	m_chunks[0].update(this, viewpoint);

	if (m_texture_quadtree) {
		m_chunks[0].update_texture(this, viewpoint);
	}

	m_loader->sync_loader_thread();
}


int	lod_chunk_tree::render(const view_state& v, render_options opt)
// Displays our model, using the LOD state computed during the last
// call to update().
//
// Returns the number of triangles rendered.
{
#ifdef MAP_ONTO_CUBEMAP
	s_max_horizontal_dimension = m_base_chunk_dimension * (1 << (m_tree_depth - 1));
#endif // MAP_ONTO_CUBEMAP
#ifdef MAP_ONTO_MERCATOR
	s_max_horizontal_dimension = m_base_chunk_dimension * (1 << (m_tree_depth - 1));
#endif // MAP_ONTO_MERCATOR

	int	triangle_count = 0;

	s_vertical_scale = m_vertical_scale;

	s_textures_bound = 0;	// stats

	if (m_chunks[0].m_data == NULL
	    || (m_texture_quadtree != NULL && m_chunks[0].m_texture_id == 0))
	{
		// No data in the root node; we can't really do
		// anything.  This should only happen briefly at the
		// very start of the program, until the loader thread
		// kicks in.
	}
	else
	{
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);

		// Render the chunked LOD tree.
		triangle_count += m_chunks[0].render(*this, v, cull::result_info(), opt, m_texture_quadtree == NULL ? true : false);
	}

	// xxx every so often, print our data usage stats.
	if (0) {
		static int	counter = 0;
		if (counter++ & 0x20)
		{
			int	estimated_texture_bytes = 0;
			if (m_texture_quadtree) {
				// guess 4 bytes per texel.
				estimated_texture_bytes += s_chunks_with_texture * 4 * m_texture_quadtree->get_tile_size() * m_texture_quadtree->get_tile_size();
			}

			printf("c_en = %d, c_dat = %d, cB = %d, c_tx = %d, tex bound = %d, tB = %d\n",
			       s_chunks_in_use,
			       s_chunks_with_data,
			       s_bytes_in_use,
			       s_chunks_with_texture,
			       s_textures_bound,
			       estimated_texture_bytes);
//			dlmalloc_stats();
		}
	}

	return triangle_count;
}


void	lod_chunk_tree::get_bounding_box(vec3* box_center, vec3* box_extent)
// Returns the bounding box of the data in this chunk tree.
{
	assert(m_chunks_allocated > 0);

	m_chunks[0].compute_bounding_box(*this, box_center, box_extent);
}


void	lod_chunk_tree::set_use_loader_thread(bool use)
// Enables or disables loading of chunk data in the background.
{
	m_loader->set_use_loader_thread(use);
}


void	lod_chunk_tree::set_parameters(float max_pixel_error, float max_texel_size, float screen_width_pixels, float horizontal_FOV_degrees)
// Initializes some internal parameters which are used to compute
// which chunks are split during update().
//
// Given a screen width and horizontal field of view, the
// lod_chunk_tree when properly updated guarantees a screen-space
// vertex error of no more than max_pixel_error (at the center of the
// viewport) when rendered.
{
	assert(max_pixel_error > 0);
	assert(max_texel_size > 0);
	assert(screen_width_pixels > 0);
	assert(horizontal_FOV_degrees > 0 && horizontal_FOV_degrees < 180.0f);

	const float	tan_half_FOV = tanf(0.5f * horizontal_FOV_degrees * float(M_PI) / 180.0f);
	const float	K = screen_width_pixels / tan_half_FOV;

	// distance_LODmax is the distance below which we need to be
	// at the maximum LOD.  It's used in compute_lod(), which is
	// called by the chunks during update().
	m_distance_LODmax = (m_error_LODmax / max_pixel_error) * K;

	// m_texture_distance_LODmax is the distance below which we
	// need to be at the leaf texture resolution.  It's used in
	// compute_texture_lod(), which is called by the chunks during
	// update_texture() to decide when to load textures.
	m_texture_distance_LODmax = 1.0f;	// default doesn't matter; it doesn't get used if we don't have a texture quadtree.
	if (m_texture_quadtree)
	{
		// Compute the geometric size of a texel at the
		// highest LOD in our chunk tree.
		assert(m_texture_quadtree->get_tile_size() > 1);
		float	texel_size_LODmax = m_base_chunk_dimension / (m_texture_quadtree->get_tile_size() - 1);	// 1 texel used up by the border.

		m_texture_distance_LODmax = (texel_size_LODmax / max_texel_size) * K;
	}
}


Uint16	lod_chunk_tree::compute_lod(const vec3& center, const vec3& extent, const vec3& viewpoint) const
// Given an AABB and the viewpoint, this function computes a desired
// LOD level, based on the distance from the viewpoint to the nearest
// point on the box.  So, desired LOD is purely a function of
// distance and the chunk tree parameters.
{
	vec3	disp = viewpoint - center;
	disp.set(0, fmax(0, fabsf(disp.get(0)) - extent.get(0)));
	disp.set(1, fmax(0, fabsf(disp.get(1)) - extent.get(1)));
	disp.set(2, fmax(0, fabsf(disp.get(2)) - extent.get(2)));

//	disp.set(1, 0);	//xxxxxxx just do calc in 2D, for debugging

	float	d = 0;
	d = sqrtf(disp * disp);

	return iclamp(((m_tree_depth << 8) - 1) - int(log2(fmax(1, d / m_distance_LODmax)) * 256), 0, 0x0FFFF);
}


int	lod_chunk_tree::compute_texture_lod(const vec3& center, const vec3& extent, const vec3& viewpoint) const
// Given an AABB and the viewpoint, this function computes a desired
// texture LOD level, based on the distance from the viewpoint to the
// nearest point on the box.  So, desired LOD is purely a function of
// distance and the texture tree & chunk tree parameters.
{
	vec3	disp = viewpoint - center;
	disp.set(0, fmax(0, fabsf(disp.get(0)) - extent.get(0)));
	disp.set(1, fmax(0, fabsf(disp.get(1)) - extent.get(1)));
	disp.set(2, fmax(0, fabsf(disp.get(2)) - extent.get(2)));

//	disp.set(1, 0);	//xxxxxxx just do calc in 2D, for debugging

	float	d = 0;
	d = sqrtf(disp * disp);

	return (m_tree_depth - 1 - int(log2(fmax(1, d / m_texture_distance_LODmax))));
}



// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
