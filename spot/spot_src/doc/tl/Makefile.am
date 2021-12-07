## -*- coding: utf-8 -*-
## Copyright (C) 2011, 2012, 2013, 2015, 2019 Laboratoire de Recherche et
## Développement de l'Epita (LRDE).
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


all: $(srcdir)/tl.pdf

# The second -e '$$pdflatex... ' line can be replaced by -pdflatex on
# recent version of latexmk; but version 4.13a, installed on some
# of our hosts, does not support it.  Add -pvc- because some people
# turn pvc on in their config file.
LATEXMK = BIBINPUTS='$(srcdir)/..' latexmk -pdf -ps- -dvi- -pvc- \
  -e '$$bibtex_use=2' \
  -e '$$pdflatex="pdflatex %O \"\\def\\SpotVersion{$(VERSION)}\\input{%S}\""'

dist_pdf_DATA = $(srcdir)/tl.pdf
EXTRA_DIST = tl.tex spotltl.sty

# latexmk produces its output in the current directory, and may not
# update its timestamp when no rebuild was necessary.  This can cause
# latexmk to be called again and again even when there is nothing to
# do, and this is unacceptable (we don't want to require latexmk for
# people that do not change the documentation).  Hence the "touch"
# below.  In case of a VPATH build, we always copy the file, so the
# timestamp is updated anyway.
$(srcdir)/tl.pdf: $(srcdir)/tl.tex $(srcdir)/../spot.bib
	$(LATEXMK) $(srcdir)/tl.tex
	if test '$(srcdir)' = '.'; then touch $@; else cp tl.pdf $@; fi

pvc:
	$(LATEXMK) -pvc $(srcdir)/tl.tex

.PHONY: mainainer-clean-local pvc
mainainer-clean-local:
	$(LATEXMK) -c $(srcdir)/tl.tex
