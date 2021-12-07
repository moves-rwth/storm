dnl Adapted from the predefined _AC_LANG_COMPILER_GNU.
m4_define([_AC_LANG_COMPILER_INTEL],
[AC_CACHE_CHECK([whether we are using the INTEL _AC_LANG compiler],
		[ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel],
[
_AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[#ifndef __INTEL_COMPILER
       choke me
#endif
]])],
		   [ac_compiler_intel=yes],
		   [ac_compiler_intel=no])
ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel=$ac_compiler_intel
])])# _AC_LANG_COMPILER_INTEL
dnl The list of warnings that must be disabled.
m4_define([_INTEL_IGNORE_WARNINGS],
[ 522   dnl Remark ``redeclared "inline" after being called''
  981   dnl Remark ``operands are evaluated in unspecified order''
  383   dnl Remark ``value copied to temporary, reference to temporary used''
  279   dnl Remark ``controlling expression is constant''
]) # _INTEL_IGNORE_WARNINGS

dnl Add extra flags when icc is used.
AC_DEFUN([buddy_INTEL],
[_AC_LANG_COMPILER_INTEL
AC_CACHE_CHECK([to add extra CFLAGS for the INTEL C compiler],
		[ac_cv_intel_cflags],
[ dnl
  if test x"$ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel" = x"yes"; then
    # -W does not exist for icc in CFLAGS.
    CFLAGS=`echo "$CFLAGS" | sed 's/-W[[[:space:]]]//g;s/-W$//'`

    disabled_warnings=''
    for warning in _INTEL_IGNORE_WARNINGS; do
      disabled_warnings="$disabled_warnings,$warning"
    done

    # Remove the extra "," and extra whitespaces.
    disabled_warnings=`echo "$disabled_warnings" | sed "s/^,//;s/[[[:space:]]]//g"`

    INTEL_CFLAGS="-w1 -Werror -wd${disabled_warnings}"

    [ac_cv_intel_cflags="$INTEL_CFLAGS"]
  else
    [ac_cv_intel_cflags=""]
  fi])
  AC_SUBST([INTEL_CFLAGS])

  if test x"$ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel" = x"yes"; then
    # -W does not exist for icc in CFLAGS.
    CFLAGS=`echo "$CFLAGS" | sed 's/-W[[[:space:]]]//g;s/-W$//'`
    CFLAGS="$CFLAGS $ac_cv_intel_cflags"
    CXXFLAGS="$CFLAGS $ac_cv_intel_cflags"
  fi
]) # buddy_INTEL
