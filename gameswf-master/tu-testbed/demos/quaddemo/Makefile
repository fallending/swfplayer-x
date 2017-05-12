# Makefile -- Thatcher Ulrich <tu@tulrich.com>

# This code has been donated to the Public Domain.

# Makefile for quadtree heightfield sample code.


all: quadtest.exe


.SUFFIXES : .obj .cpp

cpp = cl /c
link = link


!if "$(debug)" == "1"

cflags = -Zi
lflags = -map -debug:full -debugtype:cv

!else

cflags = -Ox
lflags = 

!endif


.cpp.obj :
	$(cpp) $(cflags) $<


OBJS = quadtest.obj quadtree.obj geometry.obj

quadtest.exe: $(OBJS)
	$(link) $(lflags) /NODEFAULTLIB:libc -out:$@ $(OBJS) glut32.lib glu32.lib opengl32.lib libcmt.lib libcpmt.lib

clean:
	rm $(OBJS)
	rm quadtest.exe
