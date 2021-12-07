

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

dnl The list of warnings that must be disabled for Spot.
m4_define([_INTEL_IGNORE_WARNINGS],
[ 69   dnl Warn when an enum value is used as an int, without any
       dnl explicit cast.
  177  dnl Warm when a method was declared but never referenced.
  279  dnl Warn when a constant expression is used in a control statement.
  654  dnl Warn when a child does not overload all virtual members of his
       dnl parents.
  913  dnl Warn when multibyte character are used (for example "Büchi").
  1125 dnl Warn when a child member hides a parent member.
]) # _INTEL_IGNORE_WARNINGS

dnl Add extra flags when icpc is used.
AC_DEFUN([spot_INTEL],
[_AC_LANG_COMPILER_INTEL
AC_CACHE_CHECK([to add extra CXXFLAGS for the INTEL C++ compiler],
		[ac_cv_intel_cxxflags],
[ dnl
  if test x"$ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel" = x"yes"; then
    disabled_warnings=''
    for warning in _INTEL_IGNORE_WARNINGS; do
      disabled_warnings="$disabled_warnings,$warning"
    done

    # Remove the extra "," and extra whitespaces.
    disabled_warnings=`echo "$disabled_warnings" | sed "s/^,//;s/[[[:space:]]]//g"`

    INTEL_CXXFLAGS="-w1 -Werror -wd${disabled_warnings}"

    [ac_cv_intel_cxxflags="$INTEL_CXXFLAGS"]
  else
    [ac_cv_intel_cxxflags=""]
  fi])

  if test x"$ac_cv_[]_AC_LANG_ABBREV[]_compiler_intel" = x"yes"; then
    # -W does not exist for icpc in CXXFLAGS.
    CXXFLAGS=`echo "$CXXFLAGS" | sed 's/-W[[[:space:]]]//g;s/-W$//'`

    # Even if the icpc preprocessor defines __GNUC__, it is not a GNU compiler.
    GXX=
    CXXFLAGS="$CXXFLAGS $ac_cv_intel_cxxflags"
  fi
  AC_SUBST([INTEL_CXXFLAGS])
]) # spot_INTEL
