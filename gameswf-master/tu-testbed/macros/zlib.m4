AC_DEFUN([AM_PATH_ZLIB],
[
  dnl Lool for the header
  AC_ARG_WITH(zlib_incl, [  --with-zlib-incl        directory where zlib header is], with_zlib_incl=${withval})
  AC_CACHE_VAL(ac_cv_path_zlib_incl,[
  if test x"${with_zlib_incl}" != x ; then
    if test -f ${with_zlib_incl}/zlib.h ; then
      ac_cv_path_zlib_incl=`(cd ${with_zlib_incl}; pwd)`
    elif test -f ${with_zlib_incl}/zlib.h ; then
      ac_cv_path_zlib_incl=`(cd ${with_zlib_incl}; pwd)`
    else
      AC_MSG_ERROR([${with_zlib_incl} directory doesn't contain zliblib.h])
    fi
  fi
  ])

  if test x"${ac_cv_path_zlib_incl}" = x ; then
    AC_MSG_CHECKING([for zlib header])
    incllist="/sw/include /usr/local/include /home/latest/include /opt/include /usr/include .. ../.."

    for i in $incllist; do
      if test -f $i/zlib.h; then
        ac_cv_path_zlib_incl=$i
        break
      fi
    done

    ZLIB_CFLAGS=""
    if test x"${ac_cv_path_zlib_incl}" = x ; then
      AC_MSG_RESULT(none)
      AC_CHECK_HEADERS(zlib.h, [ac_cv_path_zlib_incl=""])
    else
      AC_MSG_RESULT(${ac_cv_path_zlib_incl})
      if test x"${ac_cv_path_zlib_incl}" != x"/usr/include"; then
        ac_cv_path_zlib_incl="-I${ac_cv_path_zlib_incl}"
      else
        ac_cv_path_zlib_incl=""
      fi
    fi
  fi

  if test x"${ac_cv_path_zlib_incl}" != x ; then
    ZLIB_CFLAGS="${ac_cv_path_zlib_incl}"
  fi

  dnl Look for the library
  AC_ARG_WITH(zlib_lib, [  --with-zlib-lib         directory where zlib library is], with_zlib_lib=${withval})
  AC_MSG_CHECKING([for zlib library])
  AC_CACHE_VAL(ac_cv_path_zlib_lib,[
  if test x"${with_zlib_lib}" != x ; then
    if test -f ${with_zlib_lib}/libz.a ; then
      ac_cv_path_zlib_lib=`(cd ${with_zlib_lib}; pwd)`
    elif test -f ${with_zlib_lib}/libz.a -o -f ${with_zlib_lib}/libz.so -o -f ${with_zlib_lib}/libz.dylib; then
      ac_cv_path_zlib_lib=`(cd ${with_zlib_incl}; pwd)`
    else
      AC_MSG_ERROR([${with_zlib_lib} directory doesn't contain libz.a])
    fi
  fi
  ])

  if test x"${ac_cv_path_zlib_lib}" = x ; then
    liblist="/sw/lib /usr/local/lib /home/latest/lib /opt/lib /usr/lib .. ../.."

    for i in $liblist; do
    if test -f $i/libz.a -o -f $i/libz.so -o -f $i/libz.dylib; then
       ac_cv_path_zlib_lib=$i
       break
    fi
    done

    ZLIB_LIBS=""
    if test x"${ac_cv_path_zlib_lib}" = x ; then
      AC_MSG_RESULT(none)
      dnl if we can't find libzlib via the path, see if it's in the compiler path
      AC_CHECK_LIB(zlib, compress2, ${ac_cv_path_zlib_lib}="-lz")
    else
      AC_MSG_RESULT(${ac_cv_path_zlib_lib})
      if test x"${ac_cv_path_zlib_lib}" != x"/usr/lib"; then
        ac_cv_path_zlib_lib="-L${ac_cv_path_zlib_lib} -lz"
      else
        ac_cv_path_zlib_lib="-lz"
      fi
    fi
  fi

  if test x"${ac_cv_path_zlib_lib}" != x ; then
    ZLIB_LIBS="${ac_cv_path_zlib_lib}"
  fi

  AC_SUBST(ZLIB_CFLAGS)
  AC_SUBST(ZLIB_LIBS)
])
