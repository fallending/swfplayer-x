dnl gameswf_xml.h      -- Rob Savoye <rob@senecass.com> 2005

dnl This source code has been donated to the Public Domain.  Do
dnl whatever you want with it.

# Configure paths for libxml2
AC_DEFUN([AM_PATH_LIBXML2],
[dnl 
dnl Get the cflags and libraries
dnl
AC_ARG_WITH(libxml,[  --with-libxml=PFX   Prefix where libxml++ is installed (optional)], libxml_prefix="$withval", libxml_prefix="")
AC_ARG_WITH(libxml-libraries,[  --with-libxml-libraries=DIR   Directory where libxml++ library is installed (optional)], libxml_libraries="$withval", libxml_libraries="")
AC_ARG_WITH(libxml-includes,[  --with-libxml-includes=DIR   Directory where libxmlc++ header files are installed (optional)], libxml_includes="$withval", libxml_includes="")
dnl AC_ARG_ENABLE(libxmltest, [  --disable-libxmltest       Do not try to compile and run a test libxml program],, enable_libxmltest=yes)

  if test "x$libxml_libraries" != "x" ; then
    LIBXML_LIBS="-L$libxml_libraries"
  elif test "x$libxml_prefix" != "x" ; then
    LIBXML_LIBS="-L$libxml_prefix/lib"
  elif test "x$prefix" != "xNONE" ; then
    LIBXML_LIBS="-L$libdir"
  fi

  if test "x$libxml_includes" != "x" ; then
    LIBXML_CFLAGS="-I$libxml_includes"
  elif test "x$libxml_prefix" != "x" ; then
    LIBXML_CFLAGS="-I$libxml_prefix/include"
  elif test "$prefix" != "NONE"; then
    LIBXML_CFLAGS="-I$prefix/include"
  fi

  AC_MSG_CHECKING(for libxml)
  no_libxml=""

  if test "x$LIBXML_CFLAGS" = "x" ; then
    LIBXML_CFLAGS=`pkg-config --cflags libxml-2.0`
  fi

  if test "x$LIBXML_LIBS" = "x" ; then
    LIBXML_LIBS=`pkg-config --libs libxml-2.0`
  fi

  if test "x$LIBXML_CFLAGS" != "x" -a  "x$LIBXML_LIBS" != "x"; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_LIBXML,1,[Define this if you have libxml2 support available])
  else
    AC_MSG_RESULT(no)
  fi

  AC_SUBST(LIBXML_CFLAGS)
  AC_SUBST(LIBXML_LIBS)
])
