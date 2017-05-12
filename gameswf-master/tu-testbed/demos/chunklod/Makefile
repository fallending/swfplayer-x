# Makefile for chunklod programs


TOP = ../..
include $(TOP)/config

INCLUDE_PATHS := $(LUA_INCLUDE) $(ENGINE_INCLUDE) $(LIBPNG_INCLUDE) $(ZLIB_INCLUDE) $(JPEGLIB_INCLUDE)

CFLAGS := $(CFLAGS) $(SDL_CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS) $(LDFLAGS)

%.$(OBJ_EXT) : %.cpp
	$(CC) -c $^ $(CFLAGS)


all: \
	make_libs	\
	heightfield_chunker$(EXE_EXT)	\
	heightfield_shader$(EXE_EXT)	\
	chunkdemo$(EXE_EXT)	\
	makebt$(EXE_EXT)	\
	maketqt$(EXE_EXT)	\
	decimate_texture$(EXE_EXT)

heightfield_chunker$(EXE_EXT): heightfield_chunker.$(OBJ_EXT) mmap_util.$(OBJ_EXT) bt_array.$(OBJ_EXT) $(GEOMETRY_LIB) $(BASE_LIB)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS) $(BASE_LIB)

heightfield_shader$(EXE_EXT): heightfield_shader.$(OBJ_EXT) mmap_util.$(OBJ_EXT) bt_array.$(OBJ_EXT) $(BASE_LIB) $(GEOMETRY_LIB)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

makebt$(EXE_EXT): makebt.$(OBJ_EXT) $(BASE_LIB) $(GEOMETRY_LIB)
	$(CC) -o $@ $^ $(LIBPNG) $(ZLIB) $(LIBS) $(LDFLAGS)

maketqt$(EXE_EXT): maketqt.$(OBJ_EXT) $(BASE_LIB) $(GEOMETRY_LIB)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

decimate_texture$(EXE_EXT): decimate_texture.$(OBJ_EXT) $(BASE_LIB) $(GEOMETRY_LIB)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

chunkdemo$(EXE_EXT): chunkdemo.$(OBJ_EXT) chunklod.$(OBJ_EXT) $(BASE_LIB) $(GEOMETRY_LIB)
	$(CC) -o $@ $^ $(JPEGLIB) $(LIBS) $(LDFLAGS)

make_libs:
	make -C ../../base
	make -C ../../geometry

clean:
	make -C ../../base clean
	make -C ../../geometry clean
	-@rm -f *.$(OBJ_EXT)
	-@rm -f chunkdemo$(EXE_EXT) heightfield_chunker$(EXE_EXT) heightfield_shader$(EXE_EXT) makebt$(EXE_EXT)
