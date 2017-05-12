# Makefile for config demo

include ../../config

SDL_INCLUDE := SDL
LUA := c:/thatcher/lua-4.0
LUA_INCLUDE := $(LUA)/include

INCLUDE_PATHS := -I$(SDL_INCLUDE) -I$(LUA_INCLUDE) -I$(ENGINE_INCLUDE)

#SDL_CFLAGS := $(shell sdl-config --cflags)
#SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS := $(CFLAGS) $(SDL_CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS) -DNO_GLIB
LDFLAGS := $(SDL_LDFLAGS) $(LDFLAGS)

LUA_LIBS := $(LUA)/lib/liblua.$(LIB_EXT) $(LUA)/lib/liblualib.$(LIB_EXT)

LIBS := $(ENGINE) $(LUA_LIBS) $(LIBS) ../../../luaswig/luaswig40.lib

#CFLAGS := $(CFLAGS) $(SDL_CFLAGS) $(INCLUDE) -g
#LDFLAGS := -lGL -lGLU -lSDL_image $(SDL_LDFLAGS) -g

OBJS := test_config.$(OBJ_EXT) SDL_wrap.$(OBJ_EXT)

all: test_config$(EXE_EXT)

SDL_wrap.c : SDL.i
	luaswig -shadow SDL.i

test_config$(EXE_EXT): $(OBJS) $(ENGINE)
	$(CC) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

