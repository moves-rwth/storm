.\" -*- coding: utf-8 -*-
[NAME]
autfilt \- filter, convert, and transform omega-automata
[DESCRIPTION]
.\" Add any additional description here
[OPTIONS FOR SAT\-MINIMIZATION]
.TP
\fB\fP
By default, SAT\-based minimization executes a binary search, checking N/2 etc.
The upper bound being N (the size of the starting automaton), the lower bound
is always 1 except when \fBsat-langmap\fR option is used.

.TP
\fBacc=DOUBLEQUOTEDSTRING\fP
DOUBLEQUOTEDSTRING is an acceptance formula in the HOA syntax, or a
parametrized acceptance name (the different acc\-name: options from HOA).

.TP
\fBcolored\fP
force all transitions (or all states if \fB\-S\fR is used) to belong to exactly
one acceptance condition.

.TP
\fBmax\-states=M\fP
M is an upper-bound on the maximum number of states of the constructed
automaton.

.TP
\fBsat\-incr=M\fP
use an incremental approach for SAT-based minimization algorithm. M can be
either \fB1\fR or \fB2\fR. They correspond respectively to
\fB\-x sat\-minimize=2\fR and \fB\-x sat\-minimize=3\fR options. They restart
the encoding only after (N\-1)\-\fBsat\-incr\-steps\fR states have been won.
Each iterations of both starts by encoding the research of an N\-1 automaton,
N being the size of the starting automaton. \fB1\fR uses Picosat assumptions.
It additionally assumes that the last \fBsat-incr-steps\fR states are
unnecessary. On failure, it relax the assumptions to do a binary search
between N\-1 and (N\-1)\-\fBsat-incr-steps\fR. \fBsat-incr-steps\fR defaults
to 6. \fB2\fR, as for it, after an N-1 state automaton has been found, uses
incremental solving for the next \fBsat\-incr\-steps\fR iterations by forbidding
the usage of an additional state without reencoding the problem again. A full
encoding occurs after \fBsat\-incr\-steps\fR iterations unless
\fBsat\-incr\-steps=\-1\fR (see SPOT_XCNF environment variable described in
spot\-x). It defaults to 2.

.TP
\fBsat\-incr\-steps=M\fP
set the value of \fBsat\-incr\-steps\fR to M. This is used by \fBsat\-incr\fR
option.

.TP
\fBsat-naive\fP
use the naive algorithm to find a smaller automaton. It starts from N (N being
the size of the starting automaton) and then checks N\-1, N\-2, etc. until the
last successful check.

.TP
\fBsat-langmap\fP
Find the lower bound of default sat\-minimize procedure (1). This relies on the
fact that the size of the minimal automaton is at least equal to the total
number of different languages recognized by the automaton's states.

.TP
\fBstates=M\fP
M is a fixed number of states to use in the result (all the states needs
not be accessible in the result. Therefore, the output might be smaller
nonetheless). The SAT\-based procedure is just used once to synthesize
one automaton, and no further minimization is attempted.

[BIBLIOGRAPHY]
The following papers are related to some of the transformations implemented
in autfilt.

.TP
\(bu
Etienne Renault, Alexandre Duret-Lutz, Fabrice Kordon, and Denis Poitrenaud:
Strength-based decomposition of the property Büchi automaton for faster
model checking. Proceedings of TACAS'13. LNCS 7795.

The \fB\-\-strength\-decompose\fR option implements the definitions
given in the above paper.
.TP
\(bu
František Blahoudek, Alexandre Duret-Lutz, Vojtčech Rujbr, and Jan Strejček:
On refinement of Büchi automata for explicit model checking.
Proceedings of SPIN'15.  LNCS 9232.

That paper gives the motivation for options \fB\-\-exclusive\-ap\fR
and \fB\-\-simplify\-exclusive\-ap\fR.
.TP
\(bu
Thibaud Michaud and Alexandre Duret-Lutz:
Practical stutter-invariance checks for ω-regular languages.
Proceedings of SPIN'15.  LNCS 9232.

Describes the algorithms used by the \fB\-\-destut\fR and
\fB\-\-instut\fR options.  These options correpond respectively to
cl() and sl() in the paper.
.TP
\(bu
Souheib Baarir and Alexandre Duret-Lutz: SAT-based minimization of
deterministic ω-automata.  Proceedings of LPAR'15 (a.k.a LPAR-20).
LNCS 9450.

Describes the \fB\-\-sat\-minimize\fR option.
[SEE ALSO]
.BR spot-x (7)
.BR dstar2tgba (1)
