AC_DEFUN([adl_NDEBUG],
 [AC_ARG_ENABLE([assert],
  [AS_HELP_STRING([--enable-assert], [turn on assertions])])
  if test "$enable_assert" != yes; then
    CPPFLAGS="$CPPFLAGS -DNDEBUG"
  fi])
