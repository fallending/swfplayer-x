# Makefile for luaSDL -Thatcher Ulrich

# Public Domain


include config.mak

LUA := ../../lua-4.0.1-loadmodule
LUA_INCLUDE := $(LUA)/include
LUA_LIBS := -L$(LUA)/lib -llua-4.0

TOLUA := ../../tolua
TOLUA_INCLUDE := $(TOLUA)/include

CFLAGS := $(CFLAGS) -I$(LUA_INCLUDE) $(CC_DEBUG_FLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(LDFLAGS)

TOLUA_LIBS := $(TOLUA)/lib/libtolua.$(LIB_EXT)

LUA_SDL_LIBS := $(TOLUA_LIBS) $(LUA_LIBS) $(SDL_LDFLAGS)

all: $(LIB_PRE)luaSDL.$(DLL_EXT)

SDL_tolua.c: SDL.pkg
	tolua -n SDL -o $@ $^

SDL_tolua.$(OBJ_EXT): SDL_tolua.c
	$(CC) -c $^ -I../../../tolua/include -I$(LUA_INCLUDE) -I$(TOLUA_INCLUDE)

$(LIB_PRE)luaSDL.$(DLL_EXT): SDL_tolua.$(OBJ_EXT)
	$(MAKE_DLL) -o $@ $^ $(LUA_SDL_LIBS)


clean:
	-rm *~
	-rm SDL_tolua.$(OBJ_EXT)
	-rm SDL_tolua.c
	-rm $(LIB_PRE)luaSDL.$(DLL_EXT)


# Some manual mingw32 command-lines that worked for me.  TODO
# integrate them here in some sane way.

#$ gcc -mno-cygwin -c SDL_tolua.c -I../../tolua/include -I../../lua-4.0.1-loadmodule/include -I../../SDL-1.2.2/include -D_WIN32

#$ dllwrap --mno-cygwin --implib luaSDL.a SDL_tolua.o --no-export-all-symbols --add-stdcall-alias -o luaSDL.dll ../../tolua/lib/libtolua.lib ../../lua-4.0.1-loadmodule/lib/liblua.a ../../SDL-1.2.2/VisualC/SDL/Release/SDL.lib /usr/lib/mingw/libcrtdll.a
