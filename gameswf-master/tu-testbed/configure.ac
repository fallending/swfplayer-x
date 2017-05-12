dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.50)
AC_INIT(gameswf/gameswf.h)
AM_CONFIG_HEADER(config.h)

AC_CANONICAL_TARGET

dnl These are required by automake
AM_INIT_AUTOMAKE(gameswf, 0.2)
AM_MAINTAINER_MODE
AC_PROG_MAKE_SET

dnl Use the POSIX Threads library, if specified.
AC_ARG_ENABLE(pthreads, [  --enable-pthreads       Enable support for pthreads],
[case "${enableval}" in
  yes) pthreads=yes ;;
  no)  pthreads=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-pthreads option]) ;;
esac])

if test x$pthreads = xyes; then
    AC_DEFINE([HAVE_PTHREADS], [], [Use POSIX Threads])
fi

dnl Add XML support, if specified.
AC_ARG_ENABLE(libxml, [  --disable-xml           Disable support for xml and xmlsocket],
[case "${enableval}" in
  yes) disablexml=yes ;;
  no)  disablexml=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for disable-xml option]) ;;
esac])

if test x$disable_xml != xno; then
    AC_DEFINE([HAVE_LIBXML], [], [Use XML])
    AM_PATH_LIBXML2
fi

dnl Fix the Intel 810 LOD bias problem
AC_ARG_ENABLE(i810-lod-bias, [  --enable-i810-lod-bias  Enable fix for Intel 810 LOD bias problem],
[case "${enableval}" in
  yes) i810lodbias=yes ;;
  no)  i810lodbias=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-i810-lod-bias option]) ;;
esac])

if test x$i810lodbias = xyes; then
    AC_DEFINE([FIX_I810_LOD_BIAS], [], [Fix i810 LOD bias problem])
fi

dnl Use the DMalloc library, if specified.
AC_ARG_ENABLE(dmalloc, [  --enable-dmalloc        Enable support for DMalloc],
[case "${enableval}" in
  yes) dmalloc=yes ;;
  no)  dmalloc=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])

if test x$dmalloc = xyes; then
    AC_DEFINE([USE_DMALLOC], [], [Use DMalloc])
    DMALLOC_LIB="-ldmallocxx"
else
    DMALLOC_LIB=""
fi
AC_SUBST(DMALLOC_LIB)


dnl Use the Dom parser from libxml2, not the xmlReader one
dnl The default is to use the Dom parser
dom=yes
AC_ARG_ENABLE(dom, [  --enable-dom         Enable support for Dom parser],
[case "${enableval}" in
  yes) dom=yes ;;
  no)  dom=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])

if test x$dom = xyes; then
    AC_DEFINE([USE_DOM], [], [Use Dom parser])
fi

dnl Use an xmlReader parser, instead of the DOM one
AC_ARG_ENABLE(reader, [  --enable-xmlreader      Enable support for xmlReader parser],
[case "${enableval}" in
  yes) reader=yes ;;
  no)  reader=no ;;
  *)   AC_MSG_ERROR([bad value ${enableval} for enable-malloc option]) ;;
esac])

if test x$reader = xyes; then
    AC_DEFINE([USE_XMLREADER], [], [Use xmlReader parser])
fi

if test x$reader = xyes -a x$dom = xyes; then
   AC_ERROR([Can't specify a DOM and an xmlReader parser])
fi


AC_PROG_CC
AC_PROG_CXX
AC_EXEEXT
AM_COMPILER_LIB
AC_PROG_INSTALL

AM_PATH_SDL
AM_PATH_SDL_MIXER
AM_PATH_OPENGL
AM_PATH_ZLIB
AM_PATH_JPEG
AM_PATH_PNG

dnl Define winsock if we're on widows. We could do something complicated,
dnl but since AC_EXEEXT does it for us, we'll do this the easy way.
if test x"$EXEEXT" == "exe"; then
  AC_DEFINE(HAVE_WINSOCK,1,[This is defined is we are on Win32])
fi

dnl Look for pthreads
if test x"$pthreads" = x"yes"; then
  AC_CHECK_LIB(pthread, main, PTHREAD_LIB=-lpthread,PTHREAD_LIB="")
else
  PTHREAD_LIB=""
fi
AC_SUBST(PTHREAD_LIB)

AC_OUTPUT(Makefile
base/Makefile
geometry/Makefile
gameswf/Makefile
config.mk
)

echo ""
echo "Configurable options are:"
if test x"$pthreads" = x"yes"; then
  echo "        Using POSIX Threads"
else
  echo "        Not using POSIX Threads"
fi
if test x"$disablexml" = x"yes"; then
  echo "        Not using XML"
else
  echo "        Using XML"
fi
if test x"$i810lodbias" = x"yes"; then
  echo "        Using the Intel 810 LOD bias hack"
else
  echo "        Not using the Intel 810 LOD bias hack"
fi
if test x"$dmalloc" = x"yes"; then
  echo "        Using DMalloc, For a list of the command-line options enter: dmalloc --usage"
  dnl dmalloc
else
  echo "        Not using DMalloc"
fi

