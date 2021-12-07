#!/usr/bin/env python3
# Copyright (C) 2016-2018 Laboratoire de Recherche et Développement de
# l'Epita (LRDE).
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

import argparse
import csv
import re
from collections import namedtuple

# Pylatex Imports
from pylatex import Document, Package, Tabular, MultiColumn
from pylatex.utils import bold, NoEscape
from pylatex.base_classes import CommandBase, Arguments
from pylatex.base_classes.command import Parameters


# ---------------------------------------------------------PARSE_CONFIG
Bench = namedtuple('Bench', ['name', 'code', 'xoptions'])


class BenchConfig(object):
    """
        A class used to parse the 'bench.config' file.
    """
    def __init__(self, filename):
        """
            Any BenchConfig object file will have the following arguments:

            -self.l which represents the list of all bench in the config file.
             Each bench having three variables: name, code & xoptions.

            -self.sh which contains shell codes that must be executed before
             each bench.
        """
        self.l = []
        self.sh = []
        with open(filename, 'r') as inputfile:
            lines = inputfile.readlines()
            for line in lines:
                if line[0] == '#' or line.isspace():
                    continue
                elif line[0:2] == "sh":
                    sh = re.search('sh (.+?)$', line).group(1)
                    continue
                else:
                    name = re.search('(.+?):', line).group(1)
                    code = re.search(':(.+?)>', line).group(1)
                    xoptions = re.search('>(.+?)$', line).group(1)
                    b = Bench(name=name, code=code, xoptions=xoptions)
                    self.l.append(b)
                    self.sh.append(sh)
                    sh = ""
# ---------------------------------------------------------PARSE_CONFIG


# --------------------------------------------------------------PYLATEX
class ColorArgument(Parameters):
    """
        A class implementing custom arguments formating for LaTeX command.
    """
    def dumps(self):
        params = self._list_args_kwargs()
        if len(params) <= 0 or len(params) > 2:
            return ''
        string = '{' + params[0] + '} ' + params[1]
        return string


class SetColor(CommandBase):
    """
        A class representing a LaTeX command used to colorize table's cells.
    """
    _latex_name = 'cellcolor'

    def dumps(self):
        arguments = self.arguments.dumps()
        res = '\\' + self._latex_name + arguments[1:-1]
        return res


class DefineColor(CommandBase):
    """
        A class representing a LaTeX command used to define a color.
    """
    _latex_name = 'definecolor'


def ne(string):
    """
        A wrapper around pylatex class NoEscape. It helps to tell pylatex to
        not escape a string.

        Be careful:
                ne(string) + ne(string) = ne(string)
            but
                ne(string) + simple_string = simple_string
    """
    return NoEscape(string)
# --------------------------------------------------------------PYLATEX


# --------------------------------------------------------------SUMMARY
def add_winner(res, winner, loser):
    """
        Each time this function is called, it increments the scrore
        of one method against another one.
    """
    res_length = len(res)
    header_length = len(res[0])
    for i in range(1, res_length):  # except the first row (header)
        if winner in res[i]:
            for j in range(1, header_length):
                if loser in res[0][j]:
                    if type(res[i][j]) is str:
                        res[i][j] = 1
                    else:
                        res[i][j] = res[i][j] + 1
                    break


def is_better(to_be_compared, val):
    """
        Check that two to_be_compared is higher than val (5% tolerance).
    """
    if val == 0:
        return to_be_compared == 0
    else:
        return to_be_compared / val < 0.95


def cmp_to_others(id_cmp, instance_cmp, what_cmp, line, config, res):
    """
        Function used to compare a time of one method to others method.
    """
    len_l = len(config.l)
    for i in range(0, len_l):
        index = get_st_index_of('min' + what_cmp + '.' + config.l[i].code,
                                line)
        if index == id_cmp:
            continue
        if '-' not in line[index] and '!' not in line[index]:
            val_cmp = float(line[id_cmp + 7])
            other = float(line[index + 7])
            if is_better(val_cmp, other):
                add_winner(res, instance_cmp, config.l[i].code)
        else:
            add_winner(res, instance_cmp, config.l[i].code)


def get_summary(config, test):
    """
        Function used to get the summary of 'test' that can be either DBA,
        or DTGBA.
    """
    res = []
    len_l = len(config.l)

    # Header
    header = ['']
    for i in range(0, len_l):
        header.append(config.l[i].code)
    header.append('total')
    res.append(header)

    # Prepare Body (fill with '-')
    for i in range(0, len_l):
        row = [config.l[i].code]
        for j in range(0, len_l):
            row.append('-')
        row.append('')
        res.append(row)

    # Body
    ifile = open('all.csv', 'r')
    lines = ifile.readlines()
    length = len(lines)
    for i in range(0, length):
        lines[i] = lines[i].split(',')

    for i in range(0, length):
        for j in range(0, len_l):
            pattern = 'min' + test + '.' + config.l[j].code

            st_id = get_st_index_of(pattern, lines[i])
            if '-' not in lines[i][st_id] and \
                    '!' not in lines[i][st_id]:
                        cmp_to_others(st_id, config.l[j].code, test,
                                      lines[i], config, res)

    for row in res[1:]:
        row[-1] = sum(x for x in row[1:-1] if type(x) is not str)
    return res


def write_summary(table2, config):
    """
        Function that writes all the bench's summary.
    """
    dba_summary = get_summary(config, 'DBA')
    dtgba_summary = get_summary(config, 'DTGBA')
    len_l = len(config.l)
    table2.add_hline()
    table2.add_row(
            (MultiColumn(len_l + 2, align='|c|', data='DBA'),))
    table2.add_hline()
    for i in range(0, len(dba_summary)):
        table2.add_row(tuple(dba_summary[i]))
        table2.add_hline()
    table2.add_row((MultiColumn(len_l + 2),))
    table2.add_hline()
    table2.add_row((MultiColumn(len_l + 2, align='|c|', data='DTGBA'),))
    table2.add_hline()
    for line in dtgba_summary:
        table2.add_row(tuple(line))
        table2.add_hline()
# --------------------------------------------------------------SUMMARY


# --------------------------------------------------------------RESULTS
def get_st_index_of(pattern, line):
    """
        A function used to get the first column of each benchmarked tool.
    """
    size = len(line)
    for i in range(0, size):
        if pattern in line[i]:
            return i + 1
    raise ValueError('\'' + pattern + '\' is not found in log files')


def dba_st_cmp_dtgba_st(pattern, line):
    """
        Function that checks if minDTGBA.st * (minDTGBA.acc + 1) < minDBA.st.
        It should not be the case normally!
    """
    dba_st_id = get_st_index_of(pattern, line)
    dba_st = line[dba_st_id]
    dtgba_v = 0
    dtgba_acc = 0
    dba_v = 0
    if '-' not in dba_st and '!' not in dba_st:
        dba_v = int(dba_st)
        if '-' not in line[dba_st_id + 10] \
                and '!' not in line[dba_st_id + 10]:
            dtgba_v = int(line[dba_st_id + 10])
            dtgba_acc = int(line[dba_st_id + 13])
            return dtgba_v * (dtgba_acc + 1) < dba_v
    return False


def get_last_successful(n, category, pattern):
    """
        A function used to get the last automaton size from satlog file.
    """
    try:
        log = open(str(n) + '.' + category + '.' + pattern
                   + '.satlog', 'r')
        log_csv = csv.reader(log)
        for line in log_csv:
            min_val = line[2]
        return '$\\le$' + min_val
    except Exception:
        return ''


def check_all_st_are_eq(line, pattern):
    """
        A function that:
        - retrieve all column values that are just after any column containing
        the pattern.
        - check that all those values are exactly the same.
    """
    size = len(line)
    l = []
    for i in range(0, size):
        if pattern in line[i]:
            if '-' not in line[i + 1] and '!' not in line[i + 1] \
                    and 'n/a' not in line[i + 1]:
                try:
                    int(i + 1)
                except Exception as e:
                    print(e)
                    exit()
                l.append(line[i + 1])

    return all(x == l[0] for x in l)


def add_other_cols(row, line, config):
    """
        A function used to add all columns that dynamically depend on the
        config.bench file.
    """
    n = int(line[-1])
    category = ['DBA', 'DTGBA']

    dba_t = []
    dtgba_t = []
    dba_st = int(line[13])

    all_dba_st_eq = check_all_st_are_eq(line, 'minDBA')
    all_dtgba_st_eq = check_all_st_are_eq(line, 'minDTGBA')

    len_l = len(config.l)
    for i in range(0, len_l):
        for elt in category:
            if 'DBA' in elt:
                width = 3
            elif 'DTGBA' in elt:
                width = 4
            st_id = get_st_index_of('min' + elt + '.' + config.l[i].code, line)
            if '-' in line[st_id]:
                s = ne(get_last_successful(n, elt, config.l[i].code))
                row.append(MultiColumn(width, align='c|',
                           data=ne('(killed') + s + ne(')')))
            elif '!' in line[st_id]:
                s = ne(get_last_successful(n, elt, config.l[i].code))
                row.append(MultiColumn(width, align='c|',
                           data=ne('(intmax') + s + ne(')')))
            else:
                cur_st = int(line[st_id])

                if 'DBA' in elt and \
                        dba_st_cmp_dtgba_st('min' + elt + '.'
                                            + config.l[i].code, line):
                    row.append(SetColor(
                        arguments=ColorArgument('Purpl', str(cur_st))))
                elif ((not all_dba_st_eq) and 'DBA' in elt) \
                        or ((not all_dtgba_st_eq) and 'DTGBA' in elt) \
                        or cur_st > dba_st:
                    row.append(SetColor(
                        arguments=ColorArgument('Red', str(cur_st))))
                elif cur_st < dba_st:
                    row.append(SetColor(
                        arguments=ColorArgument('Gray', str(cur_st))))
                else:
                    row.append(str(cur_st))

                row.append(line[st_id + 2])  # st + 2 = trans
                time = '%.2f' % round(float(line[st_id + 7]), 2)
                if width > 3:
                    row.append(line[st_id + 3])
                    dtgba_t.append(time)
                else:
                    dba_t.append(time)
                row.append(time)

    try:
        dba = min(float(x) for x in dba_t)
    except ValueError:
        dba = -1
    try:
        dtgba = min(float(x) for x in dtgba_t)
    except ValueError:
        dtgba = -1
    return dba, dtgba


def get_dra_st(line, c):
    """
        Get state of DRA.
    """
    for i in range(0, len(line)):
        if 'DRA' in line[i]:
            if 'n/a' in line[i + 1]:
                return ''
            else:
                return str(int(line[i + 1]) - 1 + int(c))


def get_type(type_f):
    """
        A function used to categorized each formula.
    """
    tmp = ''
    if 'trad' in type_f:
        tmp = 'T'
    elif 'TCONG' in type_f:
        tmp = 'P'
    elif 'DRA' in type_f:
        tmp = 'R'
    elif 'WDBA' in type_f:
        tmp = 'W'
    else:
        tmp = type_f
    return tmp


def val(line, v):
    """
        A function used to retrieve any integer located at line[v].
    """
    try:
        res = int(line[v])
    except Exception as e:
        if '-' in line[v]:
            return 1
        else:
            print(e)
            exit()
    return res


def all_aut_are_det(line, config):
    """
        A function that check that all automaton produced are determinist.
    """
    if val(line, 8) == 0 or val(line, 18) == 0:
        return False
    size = len(config.l)
    for i in range(1, size + 1):
        if val(line, 18 + 10 * i) == 0:
            return False
    return True


def clean_formula(f):
    """
        A function used to clean any formula.
    """
    res = '$'
    f_iter = iter(f)
    for it in f_iter:
        if it == '&':
            res += '\\land '
        elif it == '|':
            res += '\\lor '
        elif it == '!':
            res += '\\bar '
        elif it == '-' and next(f_iter, it) == '>':
            res += '\\rightarrow '
        elif it == '<' and next(f_iter, it) == '-' and next(f_iter, it) == '>':
            res += '\\leftrightarrow '
        else:
            res += it
    return ne(res + '$')


def add_static_cols(row, line, config):
    """
        A function used to add the 14 first static columns. Those columns don't
        depend on the config.bench file.
    """
    f = clean_formula(line[0])  # TODO: define math operators for formula
    m = line[1]
    f_type = line[2]
    c = line[9]
    if all_aut_are_det(line, config):
        c_str = line[9]
    else:
        c_str = SetColor(arguments=ColorArgument('Red', line[9]))
    dtgba_st = line[3]
    dtgba_tr = line[5]
    dtgba_acc = line[6]
    dtgba_time = '%.2f' % round(float(line[10]), 2)
    dba_st = line[13]
    dba_tr = line[15]
    dba_time = line[20]

    row.append(f)  # formula
    row.append(m)
    row.append(get_type(f_type))  # trad -> T, TCONG -> P, DRA -> R, WDBA -> W
    row.append(c_str)  # is complete or not
    row.append(get_dra_st(line, c))
    row.append(dtgba_st)
    row.append(dtgba_tr)
    row.append(dtgba_acc)
    row.append(dtgba_time)
    row.append(dba_st)
    row.append(dba_tr)
    if '-' in dba_time or '!' in dba_time:
        row.append(dba_time)
    else:
        row.append('%.2f' % round(float(dba_time), 2))

    # DBAminimizer
    length = len(line)
    for i in range(0, length):
        if 'dbaminimizer' in line[i]:
            if '-' in line[i + 1]:
                row.append(MultiColumn(2, align='|c', data='(killed)'))
            elif 'n/a' in line[i + 1]:
                row.append('')
                row.append('')
            else:
                minimizer_st = int(line[i + 1]) - 1 + int(c)
                if minimizer_st < int(dba_st):
                    row.append(SetColor(
                        arguments=ColorArgument('Gray', str(minimizer_st))))
                elif minimizer_st > int(dba_st):
                    row.append(SetColor(
                        arguments=ColorArgument('Red', str(minimizer_st))))
                else:
                    row.append(minimizer_st)
                row.append('%.2f' % round(float(line[i + 2]), 2))


def next_bench_considering_all(line, index):
    """
        A function used to get the index of the next benchmark. It takes into
        account '(killed)' MultiColumns...
    """
    try:
        line[index + 7]
    except:
        return index + 7

    if is_not_MultiColumn(line, index):
        if is_not_MultiColumn(line, index + 3):
            return index + 7
        else:
            return index + 4
    else:
        if is_not_MultiColumn(line, index + 1):
            return index + 5
        else:
            return index + 2


def is_eq(to_be_compared, val):
    """
        Check that two values are almost equal (5% tolerance).
    """
    try:
        return to_be_compared / val <= 1.05     # to_... is always >= val
    except ZeroDivisionError:
        return is_eq(to_be_compared + 1, val + 1)


def is_not_MultiColumn(line, index):
    """
        Check that the type(line[index]) is not MultiColumn.
    """
    try:
        return type(line[index]) is not MultiColumn
    except IndexError as e:
        print(e)
        exit()


def get_first_mindba(line):
    """
        A function used to get the index of the first benchmark (just after
        the static columns).
    """
    if type(line[12]) is MultiColumn:
        return 13
    return 14


def get_lines(config):
    """
        Entry point for parsing the csv file. It returns all lines that will
        be displayed. After this function, no more treatment are done on datas.
    """
    all_l = []
    best_dba_l = []
    best_dtgba_l = []
    ifile = open('all.csv', 'r')
    reader = csv.reader(ifile)
    for line in reader:
        row = []
        add_static_cols(row, line, config)          # 14 first columns
        dba_t, dtgba_t = add_other_cols(row, line, config)     # All the rest
        all_l.append(row)
        best_dba_l.append(dba_t)
        best_dtgba_l.append(dtgba_t)

    all_lines_length = len(all_l)
    for i in range(0, all_lines_length):
        index = get_first_mindba(all_l[i])
        size = len(all_l[i])
        while index < size:
            if best_dba_l[i] != -1:
                if is_not_MultiColumn(all_l[i], index):
                    if is_eq(float(all_l[i][index + 2]), best_dba_l[i]):
                        all_l[i][index + 2] = SetColor(
                            arguments=ColorArgument('Green',
                                                    str(best_dba_l[i])))

            if best_dtgba_l[i] != -1:
                if is_not_MultiColumn(all_l[i], index):
                    if is_not_MultiColumn(all_l[i], index + 3)\
                            and is_eq(float(all_l[i][index + 6]),
                                      best_dtgba_l[i]):
                            all_l[i][index + 6] = SetColor(
                                arguments=ColorArgument('Yelw',
                                                        str(best_dtgba_l[i])))
                else:
                    if is_not_MultiColumn(all_l[i], index + 1)\
                            and is_eq(float(all_l[i][index + 4]),
                                      best_dtgba_l[i]):
                            all_l[i][index + 4] = SetColor(
                                arguments=ColorArgument('Yelw',
                                                        str(best_dtgba_l[i])))

            index = next_bench_considering_all(all_l[i], index)
    return all_l, best_dba_l, best_dtgba_l


def write_header(table, config):
    """
        Function that write the first lines of the document.
    """
    # Static datas
    data_row1 = ne('Column ') + bold('type') + \
        ne(' shows how the initial det. aut. was obtained: T = translation'
           ' produces DTGBA; W = WDBA minimization works; P = powerset '
           'construction transforms TBA to DTBA; R = DRA to DBA.')
    data_row2 = ne('Column ') + bold('C.') + \
        ne(' tells whether the output automaton is complete: rejecting '
           'sink states are always omitted (add 1 state when C=0 if you '
           'want the size of the complete automaton).')
    row3 = [MultiColumn(14)]
    row4 = ['', '', '', '', 'DRA',
            MultiColumn(4, align='|c', data='DTGBA'),
            MultiColumn(3, align='|c', data='DBA'),
            MultiColumn(2, align='|c',
                        data=ne('DBA\\footnotesize minimizer'))]
    row5 = ['formula', 'm', 'type', 'C.', 'st.', 'st.', 'tr.',
            'acc.', 'time', 'st.', 'tr.', 'time', 'st.', 'time']

    # Datas that depends on the configuration file
    len_l = len(config.l)
    for i in range(0, len_l):
        row3.append(MultiColumn(7, align='|c', data=config.l[i].name))
        row4.append(MultiColumn(3, align='|c', data='minDBA'))
        row4.append(MultiColumn(4, align='|c', data='minDTGBA'))
        row5.extend(['st.', 'tr.', 'time', 'st.', 'tr.', 'acc.', 'time'])

    # Add the first 5 lines of the document.
    n = 14 + len_l * 7
    table.add_row((MultiColumn(n, align='c', data=data_row1),))
    table.add_row((MultiColumn(n, align='c', data=data_row2),))
    table.add_row((MultiColumn(n, align='l', data=''),))  # add empty line
    table.add_row(tuple(row3))
    table.add_row(tuple(row4))
    table.add_row(tuple(row5))
    table.add_hline()


def write_results(table, config):
    """
        Function that writes all the bench's result.
    """
    # Write header (first 5 lines)
    write_header(table, config)

    # Write all results
    lines, best_dba_l, best_dtgba_l = get_lines(config)
    for line in lines:
        table.add_row(tuple(line))
# --------------------------------------------------------------RESULTS


def add_fmt(nfields, doc):
    """
        Function used to define the table's format depending on config.bench
        file.
    """
    if doc:
        tmp = '|lrcr|r|rrrr|rrr|rr|'
        for i in range(0, nfields):
            tmp += 'rrr|rrrr|'
        return tmp
    else:
        tmp = '|c|c|'
        for i in range(0, nfields):
            tmp += 'c|'
        return tmp


def generate_docs(config):
    """
        Function used to generate two pdf:

        -results.pdf: which shows all statistics about each formula with each
         benchmarked method.

        -summary.pdf: which count the number of times that each method is
         better than another.
    """
    # Let's create the documents (result & summary)
    doc = Document(documentclass='standalone')
    doc.packages.append(Package('amsmath'))
    doc.packages.append(Package('color'))
    doc.packages.append(Package('colortbl'))
    doc2 = Document(documentclass='standalone')

    # Declare colors in result document
    doc.append(DefineColor(
        arguments=Arguments('Gray', 'rgb', '0.7, 0.7, 0.7')))
    doc.append(DefineColor(arguments=Arguments('Green', 'rgb', '0.4, 1, 0.4')))
    doc.append(DefineColor(arguments=Arguments('Red', 'rgb', '0.8, 0, 0')))
    doc.append(DefineColor(arguments=Arguments('Yelw', 'rgb', '1, 0.98, 0.4')))
    doc.append(DefineColor(arguments=Arguments('Purpl', 'rgb', '1, 0.6, 1')))

    # Create Table with format : True is result format, False is summary format
    table = Tabular(add_fmt(len(config.l), True))
    table2 = Tabular(add_fmt(len(config.l), False))

    # Write everything
    write_results(table, config)
    write_summary(table2, config)

    # Output PDF
    doc.append(table)
    doc2.append(table2)
    doc.generate_pdf('results')
    doc2.generate_pdf('summary')


def generate_bench(config, args):
    """
        A function used to generate a complete benchmark. It outputs a shell
        script.
    """
    echo = ""
    with open('stat-gen.sh', 'w') as script:
        # Header.
        script.write('''#!/bin/sh

ltlfilt=../../bin/ltlfilt
ltl2tgba=../../bin/ltl2tgba
dstar2tgba=../../bin/dstar2tgba
timeout='timeout -sKILL {}'
stats=--stats=\"%s, %e, %t, %a, %c, %d, %p, %r, %R\"
empty='-, -, -, -, -, -, -, -, -'
tomany='!, !, !, !, !, !, !, !, !'
dbamin=${}

'''.format(str(args.timeout) + args.unit, '{DBA_MINIMIZER}'))

        script.write('''get_stats()
{
  type=$1
  shift
  SPOT_SATLOG=$n.$type.satlog $timeout \"$@\" \"$stats\"> stdin.$$ 2>stderr.$$
  if grep -q 'INT_MAX' stderr.$$; then
    # Too many SAT-clause?
    echo \"tomany\"
  else
    tmp=`cat stdin.$$`
    echo ${tmp:-$empty}
  fi
  rm -f stdin.$$ stderr.$$
}

get_dbamin_stats()
{
  tmp=`./rundbamin.pl $timeout $dbamin \"$@\"`
  mye='-, -'
  echo ${tmp:-$mye}
}

n=$1
f=$2
type=$3
accmax=$4

case $type in
    *WDBA*)
  exit 0
  ;;
    *TCONG*|*trad*)  # Not in WDBA
  echo \"$f, $accmax, $type...\" 1>&2
  input=`get_stats TBA $ltl2tgba \"$f\" -D -x '!wdba-minimize,tba-det'`
  echo \"$f, $accmax, $type, $input, DBA, ...\" 1>&2\n
  dba=`get_stats BA $ltl2tgba \"$f\" -BD -x '!wdba-minimize,tba-det'`

''')

        # Body.
        echo = "$f, $accmax, $type, $input, DBA, $dba"
        for i in range(0, len(config.l)):
            script.write("  # " + config.l[i].name + "\n")
            if config.sh[i]:
                script.write("  " + config.sh[i] + "\n")
            script.write("  echo \"" + echo + ", minDBA." + config.l[i].code
                         + "...\" 1>&2\n")
            script.write("  mindba_" + config.l[i].code + "=`get_stats DBA."
                         + config.l[i].code + " $ltl2tgba \"$f\" -BD -x"
                         " '!wdba-minimize," + config.l[i].xoptions + "'`\n")
            echo += ", minDBA." + config.l[i].code + ", $mindba_" \
                    + config.l[i].code
            script.write("  echo \"" + echo + ", minDTGBA." + config.l[i].code
                         + "...\" 1>&2\n")
            script.write("  mindtgba_" + config.l[i].code +
                         "=`get_stats DTGBA." + config.l[i].code +
                         " $ltl2tgba \"$f\" -D -x '!wdba-minimize," +
                         config.l[i].xoptions + "'`\n\n")
            echo += ", minDTGBA." + config.l[i].code + ", $mindtgba_" \
                    + config.l[i].code
        script.write('''  # Dbaminimizer
  echo \"{}, dbaminimizer...\" 1>&2
  case $type in
    *TCONG*) dbamin=\"n/a, n/a\" dra=\"n/a\";;
        *trad*)
      $ltlfilt --remove-wm -f \"$f\" -l |
      ltl2dstar --ltl2nba=spin:$ltl2tgba@-Ds - dra.$$
      dbamin=`get_dbamin_stats dra.$$`
      dra=`sed -n 's/States: \\(.*\\)/\\1/p' dra.$$`
      rm dra.$$
      ;;
  esac
  ;;
    *DRA*)
  echo \"$f, $accmax, $type...\" 1>&2
  $ltlfilt --remove-wm -f \"$f\" -l |
  ltl2dstar --ltl2nba=spin:$ltl2tgba@-Ds - dra.$$
  input=`get_stats TBA $dstar2tgba dra.$$ -D -x '!wdba-minimize'`
  echo \"$f, $accmax, $type, $input, DBA, ...\" 1>&2
  dba=`get_stats BA $dstar2tgba dra.$$ -BD -x '!wdba-minimize'`

'''.format(echo))

        echo = "$f, $accmax, $type, $input, DBA, $dba"
        for i in range(0, len(config.l)):
            script.write("  # " + config.l[i].name + "\n")
            if config.sh[i]:
                script.write("  " + config.sh[i] + "\n")
            script.write("  echo \"" + echo + ", minDBA." + config.l[i].code
                         + "...\" 1>&2\n")
            script.write("  mindba_" + config.l[i].code + "=`get_stats DBA."
                         + config.l[i].code + " $dstar2tgba dra.$$ -BD -x"
                         " '!wdba-minimize," + config.l[i].xoptions + "'`\n")
            echo += ", minDBA." + config.l[i].code + ", $mindba_" \
                    + config.l[i].code
            script.write("  echo \"" + echo + ", minDTGBA." + config.l[i].code
                         + "...\" 1>&2\n")
            script.write("  mindtgba_" + config.l[i].code +
                         "=`get_stats DTGBA." + config.l[i].code +
                         " $dstar2tgba dra.$$ -D -x '!wdba-minimize," +
                         config.l[i].xoptions + "'`\n\n")
            echo += ", minDTGBA." + config.l[i].code + ", $mindtgba_" \
                    + config.l[i].code

        script.write('''  # Dbaminimizer
  echo \"{}, dbaminimizer...\" 1>&2
  dbamin=`get_dbamin_stats dra.$$`
  dra=`sed -n 's/States: \\(.*\\)/\\1/p' dra.$$`
  rm -f dra.$$
  ;;
    *not*)
  exit 0
  ;;
    *)
  echo \"SHOULD NOT HAPPEND\"
  exit 2
  ;;
esac

'''.format(echo))
        echo += ", dbaminimizer, $dbamin, DRA, $dra, $n"
        script.write("echo \"{}\" 1>&2\n".format(echo))
        script.write("echo \"{}\"\n".format(echo))
    print("Now run chmod +x stat-gen.sh")


def parse_args():
    """
        Function that parse the command-line arguments and options.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=['script', 'results'])
    parser.add_argument('--timeout', type=int, help="Timeout (0-9)+")
    parser.add_argument('--unit', type=str, help="Time unit (h|m|s)",
                        choices=['h', 'm', 's'])
    args = parser.parse_args()

    config = BenchConfig('config.bench')
    if args.action == 'script':
        if not args.timeout:
            parser.error('bench requires --timeout')
        if not args.unit:
            parser.error('bench requires --unit')
        generate_bench(config, args)
    elif args.action == 'results':
        generate_docs(config)


if __name__ == '__main__':
    parse_args()
