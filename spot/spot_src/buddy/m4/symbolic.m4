dnl This was addapted from a patch submitted written by H.J. Lu for GCC.
dnl   https://gcc.gnu.org/ml/gcc/2007-01/msg00363.html
dnl The -Bsymbolic-functions is no-longer part of the --help of ld, but
dnl we can call ld -Bsymbolic-functions --help and it will choke if the
dnl option is not supported.
dnl Substitute SYMBOLIC_LDFLAGS with -Bsymbolic-functions for GNU linker
dnl if it is supported.
AC_DEFUN([AC_LIB_PROG_LD_GNU_SYMBOLIC],
[AC_CACHE_CHECK([if the GNU linker ($LD) supports -Bsymbolic-functions],
acl_cv_prog_gnu_ld_symbolic, [
acl_cv_prog_gnu_ld_symbolic=no
if test x"$with_gnu_ld" = x"yes"; then
  if $LD -Bsymbolic-functions --help>/dev/null 2>&1 </dev/null; then
    acl_cv_prog_gnu_ld_symbolic=yes
  fi
fi])
if test x"$acl_cv_prog_gnu_ld_symbolic" = x"yes"; then
  SYMBOLIC_LDFLAGS="-Wl,-Bsymbolic-functions"
else
  SYMBOLIC_LDFLAGS=''
fi
AC_SUBST([SYMBOLIC_LDFLAGS])])

AC_DEFUN([AX_BSYMBOLIC],
[AC_ARG_ENABLE([Bsymbolic],
   [AS_HELP_STRING([--disable-Bsymbolic],
                   [disable linking with -Bsymbolic])],
   [], [enable_Bsymbolic=yes])
if test "$enable_Bsymbolic" != "no"; then
  AC_LIB_PROG_LD_GNU_SYMBOLIC
fi])
