# Makefile for tu-testbed/geometry library

# Library containing mostly 3d and geometric utility code


TOP = ..
include $(TOP)/config

INCLUDE_PATHS := $(SDL_INCLUDE) $(ENGINE_INCLUDE) $(LIBPNG_INCLUDE) $(ZLIB_INCLUDE) $(JPEGLIB_INCLUDE)

CFLAGS := $(CFLAGS) $(SDL_CFLAGS) $(INCLUDE_PATHS) $(CC_DEBUG_FLAGS)
LDFLAGS := -lGL -lGLU $(SDL_LDFLAGS) $(LIB_DEBUG_FLAGS)

# Source files.
SRCS := \
	axial_box.cpp		\
	bsp.cpp			\
	collision.cpp		\
	cull.cpp		\
	geometry.cpp		\
	kd_tree_dynamic.cpp	\
	kd_tree_packed.cpp	\
	tqt.cpp
OBJS := $(SRCS:.cpp=.$(OBJ_EXT))
OBJS := $(OBJS:.c=.$(OBJ_EXT))


GEOMETRY_LIB = $(LIB_PRE)geometry.$(LIB_EXT)

all: $(GEOMETRY_LIB)

$(GEOMETRY_LIB): $(OBJS)
	$(AR) $(LIB_OUT_FLAG)$(GEOMETRY_LIB) $(OBJS)

clean:
	-@rm -f $(GEOMETRY_LIB) $(OBJS)
