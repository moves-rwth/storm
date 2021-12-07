# -*- coding: utf-8 -*-
# Copyright (C) 2016, 2019-2020 Laboratoire de Recherche et
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

# This file is named "aux_.py" for compatibility with Windows'
# historical limitations, but should really be imported as "spot.aux".

"""
Auxiliary functions for Spot's Python bindings
"""

from functools import lru_cache
import subprocess
import sys
import os
import errno
import contextlib
import re


def extend(*classes):
    """
    Decorator that extends all the given classes with the contents
    of the class currently being defined.
    """
    def wrap(this):
        for cls in classes:
            for (name, val) in this.__dict__.items():
                if name not in ('__dict__', '__weakref__') \
                   and not (name == '__doc__' and val is None):
                    setattr(cls, name, val)
        return classes[0]
    return wrap


# Work around a bug introduced in GraphViz 2.42.x, where the scale
# parameter is inverted.  https://gitlab.com/graphviz/graphviz/issues/1605
# In our case, the scale parameters should both be <= 1, so we can
# detect when that is not the case.
svgscale_regex = re.compile('transform="scale\(([\d.]+) ([\d.]+)\) rotate')

def _gvfix(matchobj):
        xs = float(matchobj.group(1))
        ys = float(matchobj.group(2))
        if xs >= 1 and ys >= 1:
            xs = 1/xs
            ys = 1/ys
        return 'transform="scale({} {}) rotate'.format(xs, ys)


# Add a small LRU cache so that when we display automata into a
# interactive widget, we avoid some repeated calls to dot for
# identical inputs.
@lru_cache(maxsize=64)
def str_to_svg(str):
    """
    Send some text to dot for conversion to SVG.
    """
    try:
        dot = subprocess.Popen(['dot', '-Tsvg'],
                               stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    except FileNotFoundError:
        print("The command 'dot' seems to be missing on your system.\n"
              "Please install the GraphViz package "
              "and make sure 'dot' is in your PATH.", file=sys.stderr)
        raise

    stdout, stderr = dot.communicate(str)
    if stderr:
        print("Calling 'dot' for the conversion to SVG produced the message:\n"
              + stderr.decode('utf-8'), file=sys.stderr)
    ret = dot.wait()
    if ret:
        raise subprocess.CalledProcessError(ret, 'dot')
    out = stdout.decode('utf-8')
    return svgscale_regex.sub(_gvfix, out)


def ostream_to_svg(ostr):
    """
    Encode an ostringstream as utf-8 and send it to dot for cocnversion to SVG.
    """
    return str_to_svg(ostr.str().encode('utf-8'))


def rm_f(filename):
    """
    Remove filename if it exists.
    """
    try:
        os.remove(filename)
    except OSError as e:
        if e.errno != errno.ENOENT:
            raise


@contextlib.contextmanager
def tmpdir():
    cwd = os.getcwd()
    tmpdir = os.environ.get('SPOT_TMPDIR') or os.environ.get('TMPDIR') or '.'
    try:
        os.chdir(tmpdir)
        yield
    finally:
        os.chdir(cwd)
