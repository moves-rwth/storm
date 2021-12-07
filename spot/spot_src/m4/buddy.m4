AC_DEFUN([AX_CHECK_BUDDY], [
  AC_SUBST([BUDDY_LDFLAGS], ['$(top_builddir)/buddy/src/libbddx.la'])
  AC_SUBST([BUDDY_CPPFLAGS], ['-I$(top_srcdir)/buddy/src'])
  AC_CONFIG_SUBDIRS([buddy])
])
