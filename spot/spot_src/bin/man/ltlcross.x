.\" -*- coding: utf-8 -*-
[NAME]
ltlcross \- cross-compare LTL/PSL translators to omega-automata
[EXAMPLES]
The following commands compare never claims produced by
.BR ltl2tgba (1),
.BR spin (1),
and
.B lbt
for 100 random formulas, using a timeout of 2 minutes.  Because
.B ltlcross
knows those tools, there is no need to specify their input and
output. A trace of the execution of the two tools, including any
potential issue detected, is reported on standard error, while
statistics are written to \f(CWresults.json\fR.
.PP
.in +4n
.nf
.ft C
% randltl \-n100 \-\-tree\-size=20..30 a b c | \e
ltlcross \-T120 ltl2tgba spin lbt \-\-json=results.json
.fi
.PP
The next command compares
.BR lbt ,
.BR ltl3ba ,
and
.BR ltl2tgba (1)
on a set of formulas saved in file \f(CWinput.ltl\fR.
Statistics are again writen
as CSV into \f(CWresults.csv\fR.  This examples specify the
input and output for each tool, to show how this can be done.
Note the use of \f(CW%L\fR to indicate that the formula passed t
for the formula in
.BR spin (1)'s
format, and \f(CW%f\fR for the
formula in Spot's format.  Each of these tool produces an
automaton in a different format (respectively, LBTT's format,
Spin's never claims, and HOA format), but Spot's parser can
distinguish and understand these three formats.
.PP
.in +4n
.nf
.ft C
% ltlcross \-F input.ltl \-\-csv=results.csv \e
           'lbt <%L >%O' \e
           'ltl3ba \-f %s >%O' \e
           'ltl2tgba \-H %f >%O'
.fi
.PP
Rabin or Streett automata output by
.B ltl2dstar
in its historical format can be read from a
file specified with \f(CW%D\fR instead of \f(CW%O\fR.  For instance:
.PP
.in +4n
.nf
.ft C
% ltlcross \-F input.ltl \e
  'ltl2dstar \-\-ltl2nba=spin:ltl2tgba@\-Ds %L %D' \e
  'ltl2dstar \-\-automata=streett \-\-ltl2nba=spin:ltl2tgba@\-Ds %L %D' \e
.fi
.PP
However, we now recommand to use the HOA output of
.BR ltl2dstar ,
as supported since version 0.5.2:
.PP
.in +4n
.nf
.ft C
% ltlcross \-F input.ltl \e
  'ltl2dstar \-\-output\-format=hoa \-\-ltl2nba=spin:ltl2tgba@\-Ds %L %O' \e
  'ltl2dstar \-\-output\-format=hoa \-\-automata=streett \-\-ltl2nba=spin:ltl2tgba@\-Ds %L %O' \e
.fi
.PP
In more recent versions of ltl2dstar, \fB\-\-output\-format=hoa\fR can
be abbreviated \fB-H\fR.

[ENVIRONMENT VARIABLES]
.TP
\fBSPOT_TMPDIR\fR, \fBTMPDIR\fR
These variables control in which directory temporary files (e.g.,
those who contain the input and output when interfacing with
translators) are created.  \fBTMPDIR\fR is only read if
\fBSPOT_TMPDIR\fR does not exist.  If none of these environment
variables exist, or if their value is empty, files are created in the
current directory.
.TP
\fBSPOT_TMPKEEP\fR
When this variable is defined, temporary files are not removed.
This is mostly useful for debugging.

[OUTPUT DATA]
The following columns are output in the CSV or JSON files.
.TP
\fBformula\fR
The formula translated.
.TP
\fBtool\fR
The tool used to translate this formula.  This is either the value of the
full \fICOMMANDFMT\fR string specified on the command-line, or,
if \fICOMMANDFMT\fR has the form \f(CW{\fISHORTNAME\fR\f(CW}\fR\fICMD\fR,
the value of \fISHORTNAME\fR.
.TP
\fBexit_status\fR, \fBexit_code\fR
Information about how the execution of the translator went.  If the
option \fB\-\-omit\-missing\fR is given, these two columns are omitted
and only the lines corresponding to successful translation are output.
Otherwise, \fBexit_status\fR is a string that can take the following
values:
.RS
.TP
\f(CW"ok"\fR
The translator ran succesfully (this does not imply that the produced
automaton is correct) and ltlcross could parse the resulting
automaton.  In this case \fBexit_code\fR is always 0.
.TP
\f(CW"timeout"\fR
The translator ran for more than the number of seconds
specified with the \fB\-\-timeout\fR option.  In this
case \fBexit_code\fR is always -1.
.TP
\f(CW"exit code"\fR
The translator terminated with a non-zero exit code.
\fBexit_code\fR contains that value.
.TP
\f(CW"signal"\fR
The translator terminated with a signal.
\fBexit_code\fR contains that signal's number.
.TP
\f(CW"parse error"\fR
The translator terminated normally, but ltlcross could not
parse its output.  In this case \fBexit_code\fR is always -1.
.TP
\f(CW"no output"\fR
The translator terminated normally, but without creating the specified
output file.  In this case \fBexit_code\fR is always -1.
.RE
.TP
\fBtime\fR
A floating point number giving the run time of the translator in seconds.
This is reported for all executions, even failling ones.
.PP
Unless the \fB\-\-omit\-missing\fR option is used, data for all the
following columns might be missing.
.TP
\fBstates\fR, \fBedges\fR, \fBtransitions\fR, \fBacc\fR
The number of states, edges, transitions, and acceptance sets in the
translated automaton.  Column \fBedges\fR counts the number of edges
(labeled by Boolean formulas) in the automaton seen as a graph, while
\fBtransitions\fR counts the number of assignment-labeled transitions
that might have been merged into a formula-labeled edge.  For instance
an edge labeled by \f(CWtrue\fR will be counted as 2^3=8 transitions if
the automaton uses 3 atomic propositions.
.TP
\fBscc\fR, \fBnonacc_scc\fR, \fBterminal_scc\fR, \fBweak_scc\fR, \fBstrong_scc\fR
The number of strongly connected components in the automaton.  The
\fBscc\fR column gives the total number, while the other columns only
count the SCCs that are non-accepting (a.k.a. transiant), terminal
(recognizes and accepts all words), weak (do not recognize all words,
but accepts all recognized words), or strong (accept some words, but
reject some recognized words).
.TP
\fBnondet_states\fR, \fBnondet_aut\fR
The number of nondeterministic states, and a Boolean indicating whether the
automaton is nondeterministic.
.TP
\fBterminal_aut\fR, \fBweak_aut\fR, \fBstrong_aut\fR
Three Boolean used to indicate whether the automaton is terminal (no
weak nor strong SCCs), weak (some weak SCCs but no strong SCCs), or strong
(some strong SCCs).
.TP
\fBproduct_states\fR, \fBproduct_transitions\fR, \fBproduct_scc\fR
Size of the product between the translated automaton and a randomly
generated state-space.  For a given formula, the same state-space is
of course used the result of each translator.  When the
\fB\-\-products\fR=\fIN\fR option is used, these values are averaged
over the \fIN\fR products performed.

[DEPRECATED OUTPUT SPECIFIERS]
Until version 1.2.6, the output of translators was specifed using the
following escape sequences.
.PP
.TP
%N
An output file containing a never claim.
.TP
%T
An output file in \fBlbtt\fR's format.
.TP
%D
An output file in \fBltl2dstar\fR's format.
.PP
Some development versions leading to 1.99.1 also featured
.PP
.TP
%H
An output file in the HOA format.
.PP
Different specifiers were needed because Spot implemented
different parsers for these formats.  Nowadays, these parsers
have been merged into a single parser that is able to
distinguish these four formats automatically.
.B ltlcross
officially supports only one output specifier:
.TP
%O
An output file in either \fBlbtt\fR's format, \fBltl2dstar\fR's format,
as a never claim, or in the HOA format
.P
For backward compatibility, the sequences %D, %H, %N, and %T, are
still supported (as aliases for %O), but are deprecated.

[SEE ALSO]
.BR randltl (1),
.BR genltl (1),
.BR ltlfilt (1),
.BR ltl2tgba (1),
.BR ltldo (1)

[BIBLIOGRAPHY]
If you would like to give a reference to this tool in an article,
we suggest you cite the following paper:
.PP
.TP
\(bu
Alexandre Duret-Lutz: Manipulating LTL formulas using Spot 1.0.
Proceedings of ATVA'13.  LNCS 8172.
.PP
.B ltlcross
is a Spot-based reimplementation of a tool called LBTT.  LBTT
was developped by Heikki Tauriainen at the Helsinki University of
Technology.  The main motivation for the reimplementation was to
support PSL, and output more statistics about the translations.
.PP
The sanity checks performed on the result of each translator (by
either LBTT or ltlcross) are described in the following paper:
.PP
.TP
\(bu
H. Tauriainen and K. Heljanko: Testing LTL formula translation into
Büchi automata.  Int. J. on Software Tools for Technology Transfer.
Volume 4, number 1, October 2002.
.PP
LBTT did not implement Test 2 described in this paper.  ltlcross
implements a slight variation: when an automaton produced by some
translator is deterministic, its complement is built and used for
additional cross-comparisons with other tools.  If the translation P1
of the positive formula and the translation N1 of the negative formula
both yield deterministic automata (this may only happen for obligation
properties) then the emptiness check of Comp(P1)*Comp(N1) is
equivalent to Test 2 of Tauriainen and Heljanko.  If only one
automaton is deterministic, say P1, it can still be used to check we
can be used to check the result of another translators, for instance
checking the emptiness of Comp(P1)*P2.
.PP
Our implementation will detect and reports problems (like
inconsistencies between two translations) but unlike LBTT it does not
offer an interactive mode to investigate such problems.
.PP
Another major difference with LBTT is that ltlcross is
not restricted to generalized Büchi acceptance.  It supports
Rabin and Streett automata via ltl2dstar's format, and automata
with arbitrary acceptance conditions via the HOA format.
