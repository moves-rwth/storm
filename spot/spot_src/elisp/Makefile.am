## -*- coding: utf-8 -*-
## Copyright (C) 2015, 2016, 2017, 2018 Laboratoire de Recherche et Développement
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

EXTRA_DIST = hoa-mode.el

GIT = https://gitlab.lrde.epita.fr/spot/emacs-modes/raw/master/

.PHONY: update-el
update-el:
	wget $(GIT)/hoa-mode.el -O $(srcdir)/hoa-mode.el || curl $(GIT)/hoa-mode.el -o $(srcdir)/hoa-mode.el
	chmod a-w $(srcdir)/hoa-mode.el
