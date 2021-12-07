AC_DEFUN([adl_ENABLE_DEBUG],
 [AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],[enable debugging symbols])])
  case "${enable_debug}" in
    yes)
      AC_DEFINE([DEBUG],1,[Define if you want debugging code.])
      # We used to use -ggdb3 when supported, but not all tools
      # are able to grok the resulting debug infos.
      if test "${ac_cv_prog_cc_g}" = yes; then
        CFLAGS="$CFLAGS -g"
        CXXFLAGS="$CXXFLAGS -g"
      fi
      ;;
    no)
      ;;
    *)
      if test "${ac_cv_prog_cc_g}" = yes; then
        CFLAGS="$CFLAGS -g"
        CXXFLAGS="$CXXFLAGS -g"
      fi
      ;;
  esac])
