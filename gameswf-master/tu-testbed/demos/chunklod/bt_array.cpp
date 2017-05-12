// bt_array.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2002

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Wrapper for accessing a .bt format disk file.  Uses memory-mapped
// file access to a (potentially giant) data file directly without
// loading it all into RAM.

#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
	#include <io.h>
#else	// not _WIN32
	#include <unistd.h>
#endif	// not _WIN32
#include <fcntl.h>
#include <string.h>

#include "bt_array.h"
#include "mmap_util.h"
#include "base/utility.h"
#include "base/tu_file.h"
#include <string.h>


#ifdef _WIN32
#define lseek64 _lseeki64
#endif // _WIN32

#ifdef __MACH__
#define lseek64 lseek
#endif // __MACH__


const int	BT_HEADER_SIZE = 256;	// offset from the start of the .bt file, before the data starts.


const int TOTAL_CACHE_BYTES = 16 << 20; // a number of bytes that we're pretty sure will fit in physical RAM.


bt_array::bt_array()
// Constructor.  Invalidate everything.  Client use bt_array::create()
// to make an instance.
	: m_cache_height(0),
	  m_sizeof_element(2),
	  m_float_data(false),
	  m_width(0),
	  m_height(0),
	  m_utm_flag(false),
	  m_utm_zone(0),
	  m_datum(0),
	  m_left(0),
	  m_right(0),
	  m_bottom(0),
	  m_top(0),
	  m_data(0),
	  m_data_size(0),
	  m_file_handle(0)
{
}


bt_array::~bt_array()
// Destructor.  Free buffers, close files, etc.
{
	m_cache.resize(0);
	if (m_data) {
		assert(m_data_size);
		mmap_util::unmap(m_data, m_data_size);

		m_data = 0;
		m_data_size = 0;
	}

	// *** MJH modification: have to close the file if it was open w/ low level I/O
	if (m_file_handle)
	{
		close(m_file_handle);
	}
}


/* static */ bt_array*	bt_array::create(const char* filename)
// Makes a bt_array accessor for the specified .bt data file.  Returns
// NULL if the file is not a valid .bt file, or if there are other
// errors.
{
	assert(filename);

	tu_file*	in = new tu_file(filename, "rb");
	if (in->get_error())
	{
		// Can't open the file.
		// warning ...
		printf("can't open file %s\n", filename);
		return NULL;
	}
	
	//
	// Read .BT header.
	//
	
	// File-type marker.
	char	buf[11];
	in->read_bytes(buf, 10);
	buf[10] = 0;
	if (strcmp(buf, "binterr1.1") != 0) {
		// Bad input file format.  Must not be BT 1.1.
		printf("input file %s is not .BT version 1.1 format\n", filename);
		delete in;
		return NULL;
	}

	// Create a bt_array instance, and load its members.
	bt_array*	bt = new bt_array();

	bt->m_width = in->read_le32();
	bt->m_height = in->read_le32();
	if (bt->m_width <= 0 || bt->m_height <= 0) {
		// invalid data size.
		printf("invalid data size: width = %d, height = %d\n", bt->m_width, bt->m_height);

		delete bt;
		delete in;
		return NULL;
	}

	int	sample_size = in->read_le16();
	bt->m_float_data = in->read_le16() == 1 ? true : false;
	if (bt->m_float_data && sample_size != 4) {
		// can't deal with floats that aren't 4 bytes.
		printf("invalid data format: float, but size = %d\n", sample_size);
		delete bt;
		delete in;
		return NULL;
	}
	if (bt->m_float_data == false && sample_size != 2) {
		// can't deal with ints that aren't 2 bytes.
		printf("invalid data format: int, but size = %d\n", sample_size);
		delete bt;
		delete in;
		return NULL;
	}
	
	bt->m_sizeof_element = bt->m_float_data ? 4 : 2;
	bt->m_utm_flag = in->read_le16() ? true : false;
	bt->m_utm_zone = in->read_le16();
	bt->m_datum = in->read_le16();
	bt->m_left = in->read_double64();
	bt->m_right = in->read_double64();
	bt->m_bottom = in->read_double64();
	bt->m_top = in->read_double64();

	// Close the file.
	delete in;
	in = NULL;

	// *** MJH modification: only if the BT file is < 32768 samples wide/high do we
	//     do the memory map (else it requires 4GB address space which is impossible)

	if (bt->m_width < 32768 && bt->m_height < 32768)
	{
		// Reopen the data using memory-mapping.
		bt->m_data_size = sample_size * bt->m_width * bt->m_height;
		bt->m_data_size += BT_HEADER_SIZE;
		bt->m_data = mmap_util::map(bt->m_data_size, false, filename);
		if (bt->m_data == 0) {
			// Failed to open a memory-mapped view to the data.
			printf("mmap_util::map() failed on %s, size = %d\n", filename, bt->m_data_size);
			delete bt;
			return NULL;
		}
	}
	else
	{
		// *** MJH modification: else we leave the data / data_size member variables null
		bt->m_data = NULL;
		bt->m_data_size = 0;

// WK OSX uses large files by default, so it doesn't require O_LARGEFILE, and has no lseek64

                int flags = 0;
#ifdef WIN32
                flags = O_BINARY | O_RDWR | O_RANDOM;
#elif defined(__MACH__)
                flags = O_RDWR;
#else
                flags = O_LARGEFILE | O_RDWR;
#endif

                // ... and open the file using low level I/O
		if ((bt->m_file_handle = open(filename, flags)) == -1)
                {
			// Failed to re-open file w/ low level I/O
			printf("_open failed on %s\n", filename);
			delete bt;
			return NULL;
		}
	}

	// Initialize the (empty) cache.
	if (bt->m_width * 4096 < TOTAL_CACHE_BYTES) {
		// No point in caching -- the dataset isn't big enough
		// to stress mmap.
		bt->m_cache_height = 0;
	} else {
		// Figure out how big our cache lines should be.
		bt->m_cache_height = TOTAL_CACHE_BYTES / bt->m_width / bt->m_sizeof_element;
	}
	if (bt->m_cache_height) {
		bt->m_cache.resize(bt->m_width);
	}

	return bt;
}


float	bt_array::get_sample(int x, int z) const
// Return the altitude from this .bt dataset, at the specified
// coordinates.  x runs west-to-east, and z runs north-to-south,
// unlike UTM.  @@ Fix this to make it match UTM???
//
// Out-of-bounds coordinates are clamped.
{
	// clamp coordinates.
	x = iclamp(x, 0, m_width - 1);
	z = iclamp(z, 0, m_height - 1);

	int	index = 0;
	char*	data = NULL;
	if (m_cache_height) {
		// Cache is active.
		
		// 'v' coordinate is (m_height - 1 - z) -- the issue
		// is that BT scans south-to-north, which our z
		// coordinates run north-to-south.  It's easier to
		// compute cache info using the natural .bt data
		// order.
		int	v = (m_height - 1 - z);

		cache_line*	cl = &m_cache[x];
		if (cl->m_data == NULL || v < cl->m_v0 || v >= cl->m_v0 + m_cache_height) {
			// Cache line must be refreshed.
			cl->m_v0 = (v / m_cache_height) * m_cache_height;
			if (cl->m_data == NULL) {
				cl->m_data = new unsigned char[m_cache_height * m_sizeof_element];
			}
			int	fillsize = imin(m_cache_height, m_height - cl->m_v0) * m_sizeof_element;

			// *** MJH modification: only read from memory mapped file if its around
			if (m_data != NULL)
			{
				memcpy(cl->m_data,
					   ((unsigned char*)m_data) + BT_HEADER_SIZE + (cl->m_v0 + m_height * x) * m_sizeof_element,
					   fillsize);
			}
			else
			{
				// *** MJH modification: else seek and load from the file proper, 64bit addressing
				Uint64 offset_i64 =
					(Uint64) BT_HEADER_SIZE
					+ ((Uint64) cl->m_v0 + (Uint64) m_height * (Uint64) x)
					* (Uint64) m_sizeof_element;
                                lseek64(m_file_handle, offset_i64, SEEK_SET);
				read(m_file_handle, cl->m_data, fillsize);
			}
		}

		index = v - cl->m_v0;
		data = (char*) cl->m_data;
	} else {
		// Cache is inactive.  Index straight into the raw data.
		data = (char*) m_data;
		data += BT_HEADER_SIZE;
		index = (m_height - 1 - z) + m_height * x;
	}

	if (m_float_data) {
		// raw data is floats.
		data += index * 4;
		union {
			float	f;
			Uint32	u;
		} raw;
		raw.u = swap_le32(*(Uint32*) data);

		return raw.f;

	} else {
		// Raw data is 16-bit integer.
		data += index * 2;
		Uint16	y = swap_le16(*(Uint16*) data);

		return y;
	}
}



// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
