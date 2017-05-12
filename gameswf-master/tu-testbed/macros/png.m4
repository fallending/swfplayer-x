AC_DEFUN([AM_PATH_PNG],
[
  dnl Lool for the header
  AC_ARG_WITH(png_incl, [  --with-png-incl         directory where png header is], with_png_incl=${withval})
  AC_CACHE_VAL(ac_cv_path_png_incl,[
  if test x"${with_png_incl}" != x ; then
    if test -f ${with_png_incl}/png.h ; then
      ac_cv_path_png_incl=`(cd ${with_png_incl}; pwd)`
    elif test -f ${with_png_incl}/png.h ; then
      ac_cv_path_png_incl=`(cd ${with_png_incl}; pwd)`
    else
      AC_MSG_ERROR([${with_png_incl} directory doesn't contain pnglib.h])
    fi
  fi
  ])

  if test x"${ac_cv_path_png_incl}" = x ; then
    AC_MSG_CHECKING([for png header])
    incllist="/sw/include /usr/local/include /home/latest/include /opt/include /usr/include .. ../.."

    for i in $incllist; do
      if test -f $i/png.h; then
        ac_cv_path_png_incl=$i
        break
      fi
    done

    PNG_CFLAGS=""
    if test x"${ac_cv_path_png_incl}" = x ; then
      AC_MSG_RESULT(none)
      AC_CHECK_HEADERS(png.h, [ac_cv_path_png_incl=""])
    else
      AC_MSG_RESULT(${ac_cv_path_png_incl})
      if test x"${ac_cv_path_png_incl}" != x"/usr/include"; then
        ac_cv_path_png_incl="-I${ac_cv_path_png_incl}"
      else
        ac_cv_path_png_incl=""
      fi
    fi
  fi

  if test x"${ac_cv_path_png_incl}" != x ; then
    PNG_CFLAGS="${ac_cv_path_png_incl}"
  fi

  dnl Look for the library
  AC_ARG_WITH(png_lib, [  --with-png-lib          directory where png library is], with_png_lib=${withval})
  AC_MSG_CHECKING([for png library])
  AC_CACHE_VAL(ac_cv_path_png_lib,[
  if test x"${with_png_lib}" != x ; then
    if test -f ${with_png_lib}/libpng.a ; then
      ac_cv_path_png_lib=`(cd ${with_png_lib}; pwd)`
    elif test -f ${with_png_lib}/libpng.a -o -f ${with_png_lib}/libpng.so; then
      ac_cv_path_png_lib=`(cd ${with_png_incl}; pwd)`
    else
      AC_MSG_ERROR([${with_png_lib} directory doesn't contain libpng.a])
    fi
  fi
  ])

  if test x"${ac_cv_path_png_lib}" = x ; then
    liblist="/sw/lib /usr/local/lib /home/latest/lib /opt/lib /usr/lib .. ../.."

    for i in $liblist; do
    if test -f $i/libpng.a -o -f $i/libpng.so; then
       ac_cv_path_png_lib=$i
       break
    fi
    done

    PNG_LIBS=""
    if test x"${ac_cv_path_png_lib}" = x ; then
      AC_MSG_RESULT(none)
      dnl if we can't find libpng via the path, see if it's in the compiler path
      AC_CHECK_LIB(png, png_check_sig, PNG_LIBS="-lpng")
    else
      AC_MSG_RESULT(${ac_cv_path_png_lib})
      if test x"${ac_cv_path_png_lib}" != x"/usr/lib"; then
        ac_cv_path_png_lib="-L${ac_cv_path_png_lib} -lpng"
      else
        ac_cv_path_png_lib="-lpng"
      fi
    fi
  fi

  if test x"${ac_cv_path_png_lib}" != x ; then
    PNG_LIBS="${ac_cv_path_png_lib}"
  fi

  AC_SUBST(PNG_CFLAGS)
  AC_SUBST(PNG_LIBS)
])
