include ../Makefile.def

# For parser.c
AM_CPPFLAGS += -I.

EXTRA_DIST = \
 readme \
 example.cal \
 examples/c432.cal \
 examples/c499.cal \
 examples/c1355.cal \
 examples/c1908.cal \
 examples/c2670.cal \
 examples/c3540.cal \
 examples/readme

AM_YFLAGS = -d

BUILT_SOURCES = parser.h

check_PROGRAMS = bddcalc
bddcalc_SOURCES = \
 hashtbl.h \
 hashtbl.cxx \
 lexer.lxx \
 parser_.cxx \
 parser_.h \
 slist.h

EXTRA_bddcalc_SOURCES = parser.y
