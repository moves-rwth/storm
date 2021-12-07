## -*- coding: utf-8 -*-
## Copyright (C) 2010-2011, 2013-2020 Laboratoire de Recherche et
## Développement de l'Epita (LRDE).
## Copyright (C) 2003-2005 Laboratoire d'Informatique de Paris 6
## (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
## Pierre et Marie Curie.
##
## This file is part of Spot, a model checking library.
##
## Spot is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## Spot is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
## License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

SUBDIRS = tl

DOXYGEN = doxygen
if ENABLE_DOXYGEN
  DOXY_STAMP = stamp
endif

all-local: $(DOXY_STAMP) $(srcdir)/org-stamp

.PHONY: doc svgo
doc:
	-rm -f stamp
	$(MAKE) stamp

# We use SVGO only if it is available.
SVGO = if ! (svgo -v >/dev/null); then :; else svgo --disable=removeViewBox
SVGOEND = ; fi

dist-hook: svgo
svgo:
	$(SVGO) -f $(distdir)/userdoc $(SVGOEND)


stamp: $(srcdir)/Doxyfile.in $(top_srcdir)/configure.ac
	$(MAKE) Doxyfile
	-rm -rf spot.html spot.latex
	$(DOXYGEN)
	$(SVGO) -f spot.html $(SVGOEND)
	touch $@

spot.html spot.tag: stamp

DISTCLEANFILES = spot.tag spot.html stamp

.PHONY: org org-man
org: $(PICTURES_EXTRA)
	cd $(top_builddir) && $(MAKE) doc/org/init.el
	rm -rf $(srcdir)/userdoc $(builddir)/org/org.errors
	$(MAKE) org-man org/plantuml.jar
	$(EMACS) --batch -q -l org/init.el
	if test -s $(builddir)/org/org.errors; then \
	  cat $(builddir)/org/org.errors; exit 1; \
        else :; fi

# We mirror the SourceForge URL here, because we have too many issues
# with these automated download otherwise (mainly SSL issues and down
# time), and failing to download those breaks our automated builds.
PLANTUML_URL = https://www.lrde.epita.fr/dload/spot/deps/plantuml.jar
org/plantuml.jar:
	wget $(PLANTUML_URL) -O org/plantuml.jar || curl -L $(PLANTUML_URL) -o org/plantuml.jar

org-man:
	mkdir -p $(srcdir)/userdoc/man
	$(top_srcdir)/tools/man2html.pl $(top_srcdir)/bin/man $(srcdir)/userdoc/man

ORG_FILES = \
  org/.dir-locals.el.in \
  org/g++wrap.in \
  org/init.el.in \
  org/spot.css \
  org/arch.tex \
  org/autcross.org \
  org/autfilt.org \
  org/csv.org \
  org/citing.org \
  org/compile.org \
  org/concepts.org \
  org/dstar2tgba.org \
  org/genaut.org \
  org/genltl.org \
  org/hoa.org \
  org/hierarchy.org \
  org/hierarchy.tex \
  org/index.org \
  org/install.org \
  org/ioltl.org \
  org/ltl2tgba.org \
  org/ltl2tgta.org \
  org/ltlcross.org \
  org/ltldo.org \
  org/ltlfilt.org \
  org/ltlgrind.org \
  org/ltlsynt.org \
  org/oaut.org \
  org/randaut.org \
  org/randltl.org \
  org/tools.org \
  org/tut.org \
  org/tut01.org \
  org/tut02.org \
  org/tut03.org \
  org/tut04.org \
  org/tut10.org \
  org/tut11.org \
  org/tut12.org \
  org/tut20.org \
  org/tut21.org \
  org/tut22.org \
  org/tut23.org \
  org/tut24.org \
  org/tut30.org \
  org/tut31.org \
  org/tut50.org \
  org/tut51.org \
  org/tut52.org \
  org/tut90.org \
  org/upgrade2.org \
  org/satmin.org \
  org/satmin.tex \
  org/setup.org \
  org/spot2.svg

PICTURES_EXTRA = \
  $(srcdir)/org/arch.svg \
  $(srcdir)/org/hierarchy.svg \
  $(srcdir)/org/satmin.svg

$(srcdir)/org/satmin.svg: org/satmin.tex
	cd $(srcdir)/org && \
	pdflatex satmin.tex && \
	pdf2svg satmin.pdf satmin.svg && \
	rm -f satmin.pdf satmin.aux satmin.log

$(srcdir)/org/arch.svg: org/arch.tex
	cd $(srcdir)/org && \
	pdflatex arch.tex && \
	pdf2svg arch.pdf arch.svg && \
	rm -f arch.pdf arch.aux arch.log

$(srcdir)/org/hierarchy.svg: org/hierarchy.tex
	cd $(srcdir)/org && \
	pdflatex hierarchy.tex && \
	pdf2svg hierarchy.pdf hierarchy.svg && \
	rm -f hierarchy.pdf hierarchy.aux hierarchy.log

$(srcdir)/org-stamp: $(ORG_FILES) $(configure_ac)
	$(MAKE) org && touch $@

$(srcdir)/userdoc: $(srcdir)/org-stamp

EXTRA_DIST = \
  footer.html \
  mainpage.dox \
  spot.bib \
  $(ORG_FILES) \
  $(PICTURES_EXTRA) \
  $(srcdir)/org-stamp \
  $(srcdir)/userdoc

DISTCLEANFILES += org/plantuml.jar
