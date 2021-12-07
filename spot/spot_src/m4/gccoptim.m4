dnl  Adapted from Akim Demaille <akim@epita.fr> ad_GCC_WARNINGS.
AC_DEFUN([ad_GCC_OPTIM],
[AC_ARG_ENABLE([optimizations],
 [AS_HELP_STRING([--disable-optimizations],
  [turn off aggressive optimizations])])
 if test -n "$GCC" && test "${enable_optimizations-yes}" = "yes"; then
  AC_CACHE_CHECK([for gcc optimization options], ac_cv_prog_gcc_opt_flags,
  [changequote(,)dnl
  cat > conftest.$ac_ext <<EOF
#line __oline__ "configure"
int main(int argc, char *argv[]) { return argv[argc-1] == 0; }
EOF
  changequote([,])dnl
  AC_LANG_PUSH([C])
  cf_save_CFLAGS="$CFLAGS"
  ac_cv_prog_gcc_opt_flags="-O3"
  for cf_opt in \
    ffast-math \
    fstrict-aliasing \
    fomit-frame-pointer
  do
    CFLAGS="$cf_save_CFLAGS $ac_cv_prog_gcc_opt_flags -$cf_opt"
    if AC_TRY_EVAL([ac_compile]); then
      ac_cv_prog_gcc_opt_flags="$ac_cv_prog_gcc_opt_flags -$cf_opt"
    fi
  done
  rm -f conftest*
  AC_LANG_POP([C])
  CFLAGS="$cf_save_CFLAGS $ac_cv_prog_gcc_opt_flags"])
  AC_CACHE_CHECK([for g++ optimization options], ac_cv_prog_gxx_opt_flags,
  [changequote(,)dnl
  cat > conftest.$ac_ext <<EOF
#line __oline__ "configure"
int main(int argc, char *argv[]) { return argv[argc-1] == 0; }
EOF
  changequote([,])dnl
  AC_LANG_PUSH([C++])
  cf_save_CXXFLAGS="$CXXFLAGS"
  ac_cv_prog_gxx_opt_flags="-O3"
  for cf_opt in \
    ffast-math \
    fstrict-aliasing \
    fomit-frame-pointer
  do
    CXXFLAGS="$cf_save_CXXFLAGS $ac_cv_prog_gxx_opt_flags -$cf_opt"
    if AC_TRY_EVAL([ac_compile]); then
      ac_cv_prog_gxx_opt_flags="$ac_cv_prog_gxx_opt_flags -$cf_opt"
    fi
  done
  rm -f conftest*
  AC_LANG_POP([C++])
  CXXFLAGS="$cf_save_CXXFLAGS $ac_cv_prog_gxx_opt_flags"])
else
  case $enable_optimizations in
    no) ;;
    *) CXXFLAGS="$CXXFLAGS $enable_optimizations"
       CFLAGS="$CFLAGS $enable_optimizations" ;;
  esac
fi])
