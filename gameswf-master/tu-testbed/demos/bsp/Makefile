# Makefile for bspdemo program


TOP = ../..
include $(TOP)/config

all: engine_libs bspdemo$(EXE_EXT)

INCLUDE_PATHS := $(SDL_INCLUDE) $(ENGINE_INCLUDE)

CFLAGS := $(CFLAGS) $(SDL_CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS)

%.obj : %.cpp
	$(CC) -c $^ $(CFLAGS)


BSPDEMO_OBJS = bspdemo.$(OBJ_EXT)

bspdemo$(EXE_EXT): $(BSPDEMO_OBJS) $(BASE_LIB) $(GEOMETRY_LIB)
	$(CC) -o $@ $(BSPDEMO_OBJS) $(BASE_LIB) $(GEOMETRY_LIB) $(LDFLAGS) $(LIBS) $(ENGINE)

engine_libs:
	$(MAKE) -C ../../base
	$(MAKE) -C ../../geometry
