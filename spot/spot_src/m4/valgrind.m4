# _AX_CHECK_VALGRIND_SANITY(IF-SANE, IF-NOT)
# ------------------------------------------
# Some installations of valgrind emit spurious warnings in ld.so or
# other standard libraries.  We cannot rely on these during "make check".
# We check that by running valgrind on "ls".
AC_DEFUN([_AX_CHECK_VALGRIND_SANITY],
[
  if (exec 8>valgrind.err
      exitcode=0
      $VALGRIND --tool=memcheck --leak-check=yes --log-fd=8 -q ls >/dev/null ||
        exitcode=$?
      test -z "`sed 1q valgrind.err`" || exitcode=50
      rm -f valgrind.err
      exit $exitcode
     ); then
     $1;
  else
     $2;
  fi
])

AC_DEFUN([AX_CHECK_VALGRIND], [
  AC_CHECK_PROG([VALGRIND], [valgrind], [valgrind])
  if test -n "$VALGRIND"; then
    AC_CACHE_CHECK([whether valgrind is sane],
		   [ax_cv_valgrind_sanity],
		   [_AX_CHECK_VALGRIND_SANITY([ax_cv_valgrind_sanity=yes],
					      [ax_cv_valgrind_sanity=no])])
    if test x"$ax_cv_valgrind_sanity" = xno; then
      VALGRIND=
    fi
  fi
])
