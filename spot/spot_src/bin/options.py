#!/usr/bin/python
# -*- coding: utf-8 -*-
# Copyright (C) 2014 Laboratoire de Recherche et
# Développement de l'Epita (LRDE).
#
# This file is part of Spot, a model checking library.
#
# Spot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Spot is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# Run all binaries, and collect the long option associated to each
# short option for easy comparison.
# This script should work with both Python 2 and 3.

from sys import stdout as out
import re
import subprocess

with open('Makefile.am', 'r') as mf:
    lines = mf.read()

lines = re.sub('\s*\\\\\s*', ' ', lines)
bin_programs = re.search('bin_PROGRAMS\s*=([\w \t]*)', lines).group(1).split()

optre = re.compile('(-\w), (--[\w=-]+)')

d = {}

for tool in bin_programs:
    args = ('./' + tool, '--help')
    try:
        popen = subprocess.Popen(args, stdout=subprocess.PIPE)
    except OSError:
        print("Cannot execute " + tool + ", is it compiled?")
        exit(1)
    popen.wait()
    output = popen.communicate()[0].decode('utf-8')

    for match in optre.finditer(output):
        shortname, longname = match.group(1), match.group(2)
        if not shortname in d:
            d[shortname] = { longname: tool }
        elif not longname in d[shortname]:
            d[shortname][longname] = tool
        else:
            w = ('%29s' % '') + d[shortname][longname]
            w = w[w.rfind('\n') + 1 : -1]
            if len(w + ' ' + tool) < 80:
                d[shortname][longname] += ' ' + tool
            else:
                d[shortname][longname] += '\n%29s%s' % ('', tool)

# The lambda function works around the fact that x might be an str or
# a unicode object depending on the Python implementation.
for shortname in sorted(d, key=lambda x: x.lower()):
    out.write(shortname)
    first=''
    for longname in sorted(d[shortname]):
        out.write('%s  %-24s %s\n' % (first, longname, d[shortname][longname]))
        first='  '
