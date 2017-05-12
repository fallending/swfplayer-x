# Makefile	-- Thatcher Ulrich <tu@tulrich.com> 2004

# This source code has been donated to the Public Domain.  Do
# whatever you want with it.

TOP = ../..
include $(TOP)/config

INCLUDE_PATHS := $(ENGINE_INCLUDE) $(ZLIB_INCLUDE) # $(LIBPNG_INCLUDE)

CFLAGS := $(CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS) $(LDFLAGS)

LIBS := $(GEOMETRY_LIB) $(BASE_LIBS) $(LIBS) $(JPEGLIB) $(ZLIB)


all: posterize$(EXE_EXT)


posterize$(EXE_EXT): posterize.$(OBJ_EXT) base_lib
	$(CC) -o $@ $< $(LIBS) $(BASE_LIB) $(LIBPNG) $(LDFLAGS)


clean: base_clean
	-rm posterize.$(OBJ_EXT) posterize$(EXE_EXT)

fresh: clean all
