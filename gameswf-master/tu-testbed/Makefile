# Makefile for tu-testbed project.

# Very miscellaneous at the moment.
all:
	make -C base
	make -C geometry
	make -C gameswf
#	make -C demos/chunklod

clean:
	make -C base clean
	make -C geometry clean
	make -C gameswf clean
#	make -C engine clean
#	make -C demos/chunklod clean

TAGS: .PHONY
	-rm -rf TAGS
	touch TAGS
	find . -name '*.h' -o -name '*.cpp' -o -name '*.c' | xargs etags -a -f TAGS -e

.PHONY:

basic_files =					\
	tu-testbed/README			\
	tu-testbed/Makefile			\
	tu-testbed/config			\
	tu-testbed/docs/AUTHORS			\
	tu-testbed/docs/LICENSE			\
	tu-testbed/docs/MANIFESTO		\
	tu-testbed/docs/README			\
	tu-testbed/docs/STYLE			\
	tu-testbed/compatibility_include.h

base_files =					\
	tu-testbed/base/Makefile		\
	tu-testbed/base/*.cpp			\
	tu-testbed/base/*.c			\
	tu-testbed/base/*.h			\
	tu-testbed/base/TODO

net_files =					\
	tu-testbed/net/Makefile			\
	tu-testbed/net/*.cpp			\
	tu-testbed/net/*.h			\
	tu-testbed/net/static/*			\
	tu-testbed/net/static/images/*		\
	tu-testbed/net/win32/VC8/*

geometry_files =				\
	tu-testbed/geometry/Makefile		\
	tu-testbed/geometry/*.cpp		\
	tu-testbed/geometry/*.c			\
	tu-testbed/geometry/*.h			\
	tu-testbed/geometry/TODO

engine_files = $(base_files) $(geometry_files)

chunklod_files =						\
	tu-testbed/go_chunklod.bat				\
	tu-testbed/go_chunklod_puget.bat				\
	tu-testbed/demos/chunklod/chunkdemo.exe			\
	tu-testbed/demos/chunklod/heightfield_chunker.exe	\
	tu-testbed/demos/chunklod/heightfield_shader.exe	\
	tu-testbed/demos/chunklod/maketqt.exe			\
	tu-testbed/demos/chunklod/makebt.exe			\
	tu-testbed/demos/chunklod/decimate_texture.exe		\
	tu-testbed/demos/chunklod/dem2bt.pl			\
	tu-testbed/demos/chunklod/Makefile			\
	tu-testbed/demos/chunklod/*.cpp				\
	tu-testbed/demos/chunklod/*.h				\
	tu-testbed/demos/chunklod/README			\
	tu-testbed/demos/chunklod/TODO				\
	tu-testbed/demos/chunklod/README-SDL.txt		\
	tu-testbed/demos/chunklod/*.dll				\
	tu-testbed/demos/chunklod/lua.exe			\
	tu-testbed/demos/chunklod/*.lua

psm_vistool_files =					\
	tu-testbed/go_psm_vistool.bat			\
	tu-testbed/demos/psm_vistool/psm_vistool.exe	\
	tu-testbed/demos/psm_vistool/Makefile		\
	tu-testbed/demos/psm_vistool/*.cpp		\
	tu-testbed/demos/psm_vistool/*.h		\
	tu-testbed/demos/psm_vistool/*.dll

test_triangulate_files =				\
	tu-testbed/go_build_generic_test_triangulate.sh	\
	tu-testbed/demos/test_triangulate/Makefile	\
	tu-testbed/demos/test_triangulate/*.cpp		\
	tu-testbed/demos/test_triangulate/*.h		\
	tu-testbed/demos/test_triangulate/testdata/*	\
	tu-testbed/demos/test_triangulate/*.txt		\
	tu-testbed/demos/test_triangulate/*.dll

gameswf_source =				\
	tu-testbed/gameswf/Makefile		\
	tu-testbed/gameswf/*.cpp		\
	tu-testbed/gameswf/*.c			\
	tu-testbed/gameswf/*.h			\
	tu-testbed/gameswf/gameswf_as_classes/*.cpp	\
	tu-testbed/gameswf/gameswf_as_classes/*.h	\
	tu-testbed/gameswf/TODO

gameswf_exe =					\
	tu-testbed/gameswf/*.dll		\
	tu-testbed/gameswf/gameswf_parser.exe	\
	tu-testbed/gameswf/gameswf_processor.exe \
	tu-testbed/gameswf/gameswf_test_ogl.exe

gameswf_files = $(gameswf_source) $(gameswf_exe)	\
	tu-testbed/go_gameswf.bat

crater_data_files =					\
	tu-testbed/demos/chunklod/crater/crater.chu	\
	tu-testbed/demos/chunklod/crater/crater.jpg

zlib_files =					\
	zlib-1.2.2/zlib.h			\
	zlib-1.2.2/zconf.h			\
	zlib-1.2.2/zlib.lib

libjpeg_files =					\
	jpeg-6b/jpeglib.h			\
	jpeg-6b/jconfig.h			\
	jpeg-6b/jmorecfg.h			\
	jpeg-6b/libjpeg.lib

libpng_files = 					\
	libpng-1.2.8/png.h			\
	libpng-1.2.8/pngconf.h			\
	libpng-1.2.8/libpng.lib


chunkdemo.zip:
	cd ..; zip tu-testbed/chunkdemo.zip						\
		$(basic_files) $(engine_files) $(chunklod_files) $(crater_data_files)

gameswf.zip:
	cd ..; zip tu-testbed/gameswf.zip			\
		$(basic_files) $(base_files) $(net_files) $(gameswf_files)

bspdemo.zip:
	cd ..; zip tu-testbed/bspdemo.zip		\
		tu-testbed/Makefile			\
		tu-testbed/config			\
		tu-testbed/demos/bsp/bspdemo.exe	\
		tu-testbed/demos/bsp/Makefile		\
		tu-testbed/demos/bsp/*.cpp		\
		tu-testbed/demos/bsp/*.h		\
		tu-testbed/demos/bsp/README		\
		tu-testbed/demos/bsp/TODO		\
		tu-testbed/demos/bsp/*.ply		\
		tu-testbed/demos/bsp/*.pl		\
		$(engine_files)

psm_vistool.zip:
	cd ..; zip tu-testbed/psm_vistool.zip				\
		$(basic_files) $(engine_files) $(psm_vistool_files)


luaSDL.src.tar.gz:
	make -C luaSDL clean
	tar czvf $@ luaSDL

tu_triangulate.zip:
	cd ..; zip tu-testbed/tu_triangulate.zip	\
		$(basic_files) $(base_files) $(net_files) $(test_triangulate_files)

win32_dependencies.zip:
	cd ../../..; zip tu-testbed/trunk/tu-testbed/win32_dependencies.zip \
		$(zlib_files) $(libjpeg_files) $(libpng_files)
