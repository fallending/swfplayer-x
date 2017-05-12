dnl Configure library prefix and suffix
dnl FIXME: This should really do tests for this data, and not
dnl just hardcode it based on the OS. This currently depends on
dnl AC_EXEEXT being called first.

AC_DEFUN([AM_COMPILER_LIB],
[dnl 
  AC_MSG_CHECKING(for library file name specifics)
  dnl These are the same for most platforms
  LIBEXT="a"
  LIBPRE="lib"

  if test "x$LIBPRE" != "x" ; then
	if test x"$EXEEXT"	== "exe"; then
	   LIBPRE="lib"
	fi
  fi

  if test "x$LIBEXT" != "x" ; then
	if test x"$EXEEXT"	== "exe"; then
	   LIBEXT="dll"
	fi
  fi

  if test "x$LIBPRE" != "x" -a  "x$LIBEXE" != "x"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi

AC_SUBST(LIBPRE)
AC_SUBST(LIBEXT)
])
