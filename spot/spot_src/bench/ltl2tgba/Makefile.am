## -*- coding: utf-8 -*-
## Copyright (C) 2013, 2016 Laboratoire de Recherche et Développement
## de l'Epita (LRDE).
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

EXTRA_DIST = \
  big \
  known \
  small \
  tools \
  tools.sim \
  sum.py

OUTPUTS = known small big
OUTCSV = $(OUTPUTS:=.csv)
OUTJSON = $(OUTPUTS:=.json)
OUTLOG = $(OUTPUTS:=.log)

CLEANFILES = $(OUTCSV) $(OUTJSON) $(OUTLOG) \
             results.pdf results.aux results.log results.tex

.PHONY = run json

run: results.pdf

json: $(OUTJSON)

deps = $(srcdir)/tools \
       $(top_srcdir)/configure.ac \
       $(top_builddir)/bin/ltl2tgba

small.json: $(srcdir)/small $(deps)
	$(srcdir)/small
big.json: $(srcdir)/big $(deps)
	$(srcdir)/big
known.json: $(srcdir)/known $(deps)
	$(srcdir)/known

results.tex: $(srcdir)/sum.py $(OUTJSON)
	v=`git describe --always --dirty 2>/dev/null || \
	  echo $(PACKAGE_STRING)`; \
	$(srcdir)/sum.py $(OUTJSON) >$(@:.tex=.tmp) \
	  --intro "Results assembled on `LANG=C date` with $$v."; \
	mv $(@:.tex=.tmp) $@

results.pdf: results.tex
	pdflatex results.tex
