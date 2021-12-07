AC_DEFUN([adl_CHECK_BISON],
[AC_ARG_VAR([BISON], [Bison parser generator])
AC_CHECK_PROGS([BISON], [bison])
if test -n "$BISON"; then
   opt='-Wno-deprecated'
   if AM_RUN_LOG([$BISON $opt --version]); then
      BISON_EXTRA_FLAGS=$opt
   fi
fi
AC_SUBST([BISON_EXTRA_FLAGS])])
