# Makefile	-- Thatcher Ulrich <tu@tulrich.com> 2003

# This source code has been donated to the Public Domain.  Do
# whatever you want with it.

# Makefile for little test programs

TOP = ../..
include $(TOP)/config

INCLUDE_PATHS := $(ENGINE_INCLUDE) $(ZLIB_INCLUDE)

CFLAGS := $(CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS) $(LDFLAGS)

LIBS := $(LIB_OUT) $(GEOMETRY_LIB) $(BASE_LIB) $(LIBS) $(JPEGLIB) $(ZLIB)


all: engine_libs quad_distance_test$(EXE_EXT) calendar$(EXE_EXT)


quad_distance_test$(EXE_EXT): quad_distance_test.$(OBJ_EXT) $(GEOMETRY_LIB) $(BASE_LIB)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)


calendar$(EXE_EXT): calendar.$(OBJ_EXT) $(BASE_LIB)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

lattice_resistance$(EXE_EXT): lattice_resistance.$(OBJ_EXT) $(BASE_LIB)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

count_ones$(EXE_EXT): count_ones.$(OBJ_EXT) $(BASE_LIB)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

engine_libs:
	$(MAKE) -C $(TOP)/base
	$(MAKE) -C $(TOP)/geometry


clean:
	$(MAKE) -C $(TOP)/base clean
	$(MAKE) -C $(TOP)/geometry clean
	rm quad_distance_test.$(OBJ_EXT) quad_distance_test$(EXE_EXT)
