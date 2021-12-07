AC_DEFUN([adl_ENABLE_DEVEL],
[AC_ARG_ENABLE([devel],
	       [AS_HELP_STRING([--enable-devel],
			       [turn on useful developer options])])

 # Turn on devel options for development version, unless
 # explicitely turned off.
 case $VERSION in
   *[[abcdefghijklmnopqrstuvwxyz]])
     if test -z "${enable_devel}"; then
       enable_devel=yes
       # Pass this flag to sub-libraries
       as_fn_append ac_configure_args " --enable-devel"
     fi;;
   *)
     if test -z "${enable_devel}"; then
       enable_devel=no
       # Pass this flag to sub-libraries
       as_fn_append ac_configure_args " --disable-devel"
     fi;;
 esac

 if test x"$enable_devel" = xyes; then
   enable_debug=${enable_debug-yes}
   enable_warnings=${enable_warnings-yes}
   enable_assert=${enable_assert-yes}
   enable_optimizations=${enable_optimizations--O}
 fi
])


AC_DEFUN([adl_ENABLE_GLIBCXX_DEBUG],
[AC_ARG_ENABLE([glibcxx-debug],
  [AS_HELP_STRING([--enable-glibcxx-debug],
    [turn on use the libstdc++ debug mode (see README)])])
if test x$enable_glibcxx_debug = xyes; then
  CPPFLAGS="$CPPFLAGS -D_GLIBCXX_DEBUG"
fi
])
