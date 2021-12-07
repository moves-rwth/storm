#!/usr/bin/env python3
## -*- coding: utf-8 -*-
## Copyright (C) 2013 Laboratoire de Recherche et Développement de
## l'Epita (LRDE).
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

import json
import argparse

def latex_escape_char(ch):
  if ch in '#$%&_{}':
    return '\\' + ch
  elif ch in '~^':
    return '\\' + ch + '{}'
  elif ch == '\\':
    return '\\textbackslash'
  else:
    return ch

def latex_escape(x):
  if type(x) == str:
    return ''.join(latex_escape_char(ch) for ch in x)
  return map(latex_escape, x)

def rot(x):
  if type(x) == str:
    return '\\rot{' + x + '}'
  return map(rot, x)


def process_file(filename):
  data = json.load(open(filename))

  ncols = len(data['fields'])
  ntools = len(data['tool'])
  datacols = range(4, ncols)
  fields = { name:index for index,name in enumerate(data["fields"]) }
  toolcol = fields['tool']
  inputcol = fields['formula']
  statescol = fields['states']

  inputs = data["inputs"]

  # Index results by tool, then input
  results = { t:{} for t in range(0, ntools) }
  for l in data["results"]:
    if l[statescol] == None:
      continue
    results[l[toolcol]][l[inputcol]] = l

  for i in range(0, ntools):
    # Remove any leading directory, and trailing %...
    name = data["tool"][i]
    name = name[name.rfind('/', 0, name.find(' ')) + 1:]
    data["tool"][i] = latex_escape(name[0:(name+'%').find('%')])

  timecol = fields['time']

  print(r'''
\section*{\texttt{%s}}
\subsection*{Cumulative summary}''' % latex_escape(filename))

  print('\\noindent\\begin{tabular}{l' + ('r' * (ncols - 1)) + '}\n',
        " & ".join(rot(latex_escape(["tool", "count"] + data["fields"][4:]))),
        "\\\\")
  for i in range(0, ntools):
    # Compute sums over each column.
    sums = [("%6.2f" if j == timecol else "%6d") %
            (sum([x[j] for x in results[i].values()]))
            for j in datacols]
    print("\\texttt{%-18s} & %3d & "
          % (data["tool"][i], len(results[i])), " & ".join(sums), "\\\\")
  print(r'\end{tabular}')

  print(r'''\subsection*{Cross comparison}

How many times did the left tool produce an automaton strictly bigger
than the top tool?  Bigger means more states, or equal number of
states and more transitions.

''')

  header = '\\noindent{\\small\\begin{tabular}{l'
  for i in data["tool"]:
    header += 'c'
  header += '}'

  transcol = fields['transitions']

  print(header)
  for left in data["tool"]:
    print("&", rot("\\texttt{%s}" % left), end=' ')
  print(r'\\')
  for left in range(0, ntools):
    print("\\texttt{%-18s}" % data["tool"][left], end=' ')
    for top in range(0, ntools):
      x = 0
      for k,ct in results[top].items():
        if k in results[left]:
          cl = results[left][k]
          if (cl[statescol] > ct[statescol]
              or (cl[statescol] == ct[statescol]
                  and cl[transcol] > ct[transcol])):
            x += 1
      print("&", x, end=' ')
    print(r"\\")
  print(r'\end{tabular}}')


def main():
  p = argparse.ArgumentParser(description="Process ltlcross' output",
                              epilog="Report bugs to <spot@lrde.epita.fr>.")
  p.add_argument('filenames',
                 help="name of the JSON file to read",
                 nargs='+')
  p.add_argument('--intro',
                 help="introductory text for the LaTeX output",
                 default='')
  args = p.parse_args()

  print(r'''\documentclass{article}
\usepackage[a4paper,landscape,margin=1cm]{geometry}
\usepackage{adjustbox}
\usepackage{array}

\newcolumntype{R}[2]{%
    >{\adjustbox{angle=#1,lap=\width-(#2)}\bgroup}%
    l%
    <{\egroup}%
}
\newcommand*\rot{\multicolumn{1}{R{90}{0em}}}% no optional argument here, please!

\begin{document}
''')
  print(args.intro)
  for filename in args.filenames:
    process_file(filename)
  print(r'\end{document}')


main()
