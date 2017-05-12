# Makefile	-- Thatcher Ulrich <tu@tulrich.com> 2005

# This source code has been donated to the Public Domain.  Do
# whatever you want with it.

# Makefile for word association test programs

TOP = ../..
include $(TOP)/config

INCLUDE_PATHS := $(ENGINE_INCLUDE) $(ZLIB_INCLUDE)

CFLAGS := $(CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS) $(LDFLAGS)

LIBS := $(LIB_OUT) $(GEOMETRY_LIB) $(BASE_LIB) $(LIBS) $(JPEGLIB) $(ZLIB)


all: engine_libs word_associations$(EXE_EXT)


word_associations$(EXE_EXT): word_associations.$(OBJ_EXT) $(GEOMETRY_LIB) $(BASE_LIB)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

engine_libs:
	$(MAKE) -C $(TOP)/base
	$(MAKE) -C $(TOP)/geometry


clean:
	$(MAKE) -C $(TOP)/base clean
	$(MAKE) -C $(TOP)/geometry clean
	rm word_associations.$(OBJ_EXT) word_associations$(EXE_EXT)
