lib_LTLIBRARIES = libsylvan.la

libsylvan_la_CFLAGS = $(AM_CFLAGS) -fno-strict-aliasing -std=gnu11

libsylvan_la_SOURCES = \
    avl.h \
    lace.c \
    lace.h \
    llmsset.c \
    llmsset.h \
    refs.h \
    refs.c \
    sha2.c \
    sha2.h \
    stats.h \
    stats.c \
    sylvan.h \
    sylvan_config.h \
    sylvan_bdd.h \
    sylvan_bdd.c \
    sylvan_ldd.h \
    sylvan_ldd.c \
    sylvan_cache.h \
    sylvan_cache.c \
    sylvan_common.c \
    sylvan_common.h \
    sylvan_mtbdd.h \
    sylvan_mtbdd.c \
    sylvan_mtbdd_int.h \
    sylvan_obj.hpp \
    sylvan_obj.cpp \
    tls.h

libsylvan_la_LIBADD = -lm

if HAVE_LIBHWLOC
libsylvan_la_LIBADD += -lhwloc
libsylvan_la_CFLAGS += -DUSE_HWLOC=1
endif
