AC_PREREQ([2.69])
AC_INIT([sylvan], [1.0])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([tools])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_CXX
LT_INIT

AC_CHECKING([for any suitable hwloc installation])
AC_CHECK_LIB([hwloc], [hwloc_topology_init], [AC_CHECK_HEADER([hwloc.h], [hwloc=yes])])
AM_CONDITIONAL([HAVE_LIBHWLOC], [test "$hwloc" = "yes"])

AC_CANONICAL_HOST
AM_CONDITIONAL([DARWIN], [case $host_os in darwin*) true;; *) false;; esac])
# test x$(uname) == "xDarwin"])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
