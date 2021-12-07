[NAME]
spot-x \- Common fine-tuning options and environment variables.

[SYNOPSIS]
.B \-\-extra-options STRING
.br
.B \-x STRING

[DESCRIPTION]
.\" Add any additional description here

[SAT\-MINIMIZE VALUES]
.TP
\fB1\fR
Used by default, \fB1\fR performs a binary search, checking N/2, etc.
The upper bound being N (the size of the starting automaton), the lower bound
is always 1 except when \fBsat-langmap\fR option is used.

.TP
\fB2\fR
Use PicoSAT assumptions. Each iteration encodes the search of an (N\-1) state
equivalent automaton, and additionally assumes that the last
\fBsat\-incr\-steps\fR states are unnecessary. On failure, relax the assumptions
to do a binary search between N\-1 and N\-1\-\fBsat\-incr\-steps\fR.
\fBsat\-incr\-steps\fR defaults to 6.

.TP
\fB3\fR
After an (N\-1) state automaton has been found, use incremental solving for
the next \fBsat\-incr\-steps\fR iterations by forbidding the usage of an
additional state without reencoding the problem again. A full encoding will
occur after \fBsat\-incr\-steps\fR iterations unless \fBsat\-incr\-steps=-1\fR
(see \fBSPOT_XCNF\fR environment variable). \fBsat\-incr\-steps\fR defaults to
2.

.TP
\fB4\fR
This naive method tries to reduce the size of the automaton one state at a
time. Note that it restarts all the encoding each time.

[ENVIRONMENT VARIABLES]
.TP
\fBSPOT_BDD_TRACE\fR
If this variable is set to any value, statistics about BDD garbage
collection and resizing will be output on standard error.

.TP
\fBSPOT_DEFAULT_FORMAT\fR
Set to a value of \fBdot\fR or \fBhoa\fR to override the default
format used to output automata.  Up to Spot 1.9.6 the default output
format for automata used to be \fBdot\fR.  Starting with Spot 1.9.7,
the default output format switched to \fBhoa\fR as it is more
convenient when chaining tools in a pipe.  Set this variable to
\fBdot\fR to get the old behavior.  Additional options may be
passed to the printer by suffixing the output format with
\fB=\fR and the options.  For instance running
.in +4n
.EX
% SPOT_DEFAULT_FORMAT=dot=bar autfilt ...
.EN
.in -4n
is the same as running
.in +4n
.EX
% autfilt --dot=bar ...
.EE
.in -4n
but the use of the environment variable makes more sense if you set
it up once for many commands.

.TP
\fBSPOT_DEBUG_PARSER\fR
If this variable is set to any value, the automaton parser of Spot is
executed in debug mode, showing how the input is processed.

.TP
\fBSPOT_DOTDEFAULT\fR
Whenever the \f(CW--dot\fR option is used without argument (even
implicitely via \fBSPOT_DEFAULT_FORMAT\fR), the contents of this
variable is used as default argument.  If you have some default
settings in \fBSPOT_DOTDEFAULT\fR and want to append to options
\f(CWxyz\fR temporarily for one call, use \f(CW--dot=.xyz\fR:
the dot character will be replaced by the contents of the
\f(CWSPOT_DOTDEFAULT\fR environment variable.

.TP
\fBSPOT_DOTEXTRA\fR
The contents of this variable is added to any dot output, immediately
before the first state is output.  This makes it easy to override
global attributes of the graph.

.TP
\fBSPOT_HOA_TOLERANT\fR
If this variable is set, a few sanity checks performed by the HOA
parser are skipped.  The tests in questions correspond to issues
in third-party tools that output incorrect HOA (e.g., declaring
the automaton with property "univ-branch" when no universal branching
is actually used)

.TP
\fBSPOT_O_CHECK\fR
Specifies the default algorithm that should be used
by the \f(CWis_obligation()\fR function.  The value should
be one of the following:
.RS
.RS
.IP 1
Make sure that the formula and its negation are
realizable by non-deterministic co-Büchi automata.
.IP 2
Make sure that the formula and its negation are
realizable by deterministic Büchi automata.
.IP 3
Make sure that the formula is realizable
by a weak and deterministic Büchi automata.
.RE
.RE

.TP
\fBSPOT_OOM_ABORT\fR
If this variable is set, Out-Of-Memory errors will \f(CWabort()\fR the
program (potentially generating a coredump) instead of raising an
exception.  This is useful to debug a program and to obtain a stack
trace pointing to the function doing the allocation.  When this
variable is unset (the default), \f(CWstd::bad_alloc\fR are thrown on
memory allocation failures, and the stack is usually unwinded up to
top-level, losing the original context of the error.  Note that at
least \f(CWltlcross\fR has some custom handling of
\f(CWstd::bad_alloc\fR to recover from products that are too large (by
ignoring them), and setting this variable will interfer with that.

.TP
\fBSPOT_PR_CHECK\fR
Select the default algorithm that must be used to check the persistence
or recurrence property of a formula f. The values it can take are between
1 and 3. All  methods work either on f or !f thanks to the duality of
persistence and recurrence classes.  See
.UR https://spot.lrde.epita.fr/hierarchy.html
this page
.UE
for more details. If it is set to:
.RS
.RS
.IP 1
It will try to check if f (or !f) is co-Büchi realizable in order to
tell if f belongs to the persistence (or the recurrence) class.
.IP 2
It checks if f (or !f) is det-Büchi realizable via a reduction
to deterministic-Rabin in order to tell if f belongs to the
recurrence (or the persistance) class.
.IP 3
It checks if f (or !f) is det-Büchi realizable via a reduction
to deterministic-parity in order to tell if f belongs to the
recurrence (or the persistance) class.
.RE
.RE

.TP
\fBSPOT_SATLOG\fR
If set to a filename, the SAT-based minimization routines will append
statistics about each iteration to the named file.  Each line lists
the following comma-separated values: input number of states, target
number of states, number of reachable states in the output, number of
edges in the output, number of transitions in the output, number of
variables in the SAT problem, number of clauses in the SAT problem,
user time for encoding the SAT problem, system time for encoding the
SAT problem, user time for solving the SAT problem, system time for
solving the SAT problem, automaton produced at this step in HOA
format.

.TP
\fBSPOT_SATSOLVER\fR
If set, this variable should indicate how to call an external
SAT\-solver \- by default, Spot uses PicoSAT, which is distributed
with. This is used by the sat\-minimize option described above.
The format to follow is the following: \f(CW"<sat_solver> [options] %I >%O"\fR.
The escape sequences \f(CW%I\fR and \f(CW%O\fR respectively
denote the names of the input and output files.  These temporary files
are created in the directory specified by \fBSPOT_TMPDIR\fR or
\fBTMPDIR\fR (see below). The SAT\-solver should follow the convention
of the SAT Competition for its input and output format.

.TP
\fBSPOT_STREETT_CONV_MIN\fR
The number of Streett pairs above which conversion from Streett
acceptance to generalized-Büchi acceptance should be made with a
dedicated algorithm.  By default this is 3, i.e., if a Streett
automaton with 3 acceptance pairs or more has to be converted into
generalized-Büchi, the dedicated algorithm is used.  This algorithm is
close to the classical conversion from Streett to Büchi, but with
several tweaks.  When this algorithm is not used, the standard
"Fin-removal" approach is used instead: first the acceptance condition
is converted into disjunctive normal form (DNF), then Fin acceptance
is removed like for Rabin automata, yielding a disjuction of
generalized Büchi acceptance, and the result is finally converted into
conjunctive normal form (CNF) to obtain a generalized Büchi
acceptance.  Both algorithms have a worst-case size that is
exponential in the number of Streett pairs, but in practice the
dedicated algorithm works better for most Streett automata with 3 or
more pairs (and many 2-pair Streett automata as well, but the
difference here is less clear).  Setting this variable to 0 will
disable the dedicated algorithm.  Setting it to 1 will enable it for
all Streett automata, however we do not recommand setting it to less
than 2, because the "Fin-removal" approach is better for single-pair
Streett automata.

.TP
\fBSPOT_STUTTER_CHECK\fR
Select the default check used to decide stutter invariance.  The
variable should hold a value between 1 and 8, corresponding to the
following tests described in our Spin'15 paper (see the BIBLIOGRAPHY
section).  The default is 8.
.RS
.RS
.IP 1
sl(a) x sl(!a)
.IP 2
sl(cl(a)) x !a
.IP 3
cl(sl(a)) x !a
.IP 4
sl2(a) x sl2(!a)
.IP 5
sl2(cl(a)) x !a
.IP 6
cl(sl2(a)) x !a
.IP 7
sl(a) x sl(!a), performed on-the-fly
.IP 8
cl(a) x cl(!a)
.RE
.RE
This variable is used by the \fB--check=stutter-invariance\fR and
\fB--stutter-invariant\fR options, but it is ignored by
\fB--check=stutter-sensitive-example\fR.

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

.TP
\fBSPOT_XCNF\fR
Assign a folder path to this variable to generate XCNF files whenever
SAT\-based minimization is used \- the file is outputed as "incr.xcnf"
in the specified directory. This feature works only with an external
SAT\-solver. See \fBSPOT_SATSOLVER\fR to know how to provide one. Also note
that this needs an incremental approach without restarting the encoding i.e
"sat\-minimize=3,param=-1" for ltl2tgba and ltl2tgta or "incr,param=-1" for
autfilt (see sat\-minimize options described above or autfilt man page).
The XCNF format is the one used by the SAT incremental competition.

[BIBLIOGRAPHY]
.TP
1.
Christian Dax, Jochen Eisinger, Felix Klaedtke: Mechanizing the
Powerset Construction for Restricted Classes of
ω-Automata. Proceedings of ATVA'07.  LNCS 4762.

Describes the WDBA-minimization algorithm implemented in Spot.  The
algorithm used for the tba-det options is also a generalization (to
TBA instead of BA) of what they describe in sections 3.2 and 3.3.

.TP
2.
Tomáš Babiak, Thomas Badie, Alexandre Duret-Lutz, Mojmír Křetínský,
Jan Strejček: Compositional Approach to Suspension and Other
Improvements to LTL Translation.  Proceedings of SPIN'13.  LNCS 7976.

Describes the compositional suspension, the simulation-based
reductions, and the SCC-based simplifications.

.TP
3.
Rüdiger Ehlers: Minimising Deterministic Büchi Automata Precisely using
SAT Solving.  Proceedings of SAT'10.  LNCS 6175.

Our SAT-based minimization procedures are generalizations of this
paper to deal with TBA or TGBA.

.TP
4.
Thibaud Michaud and Alexandre Duret-Lutz: Practical stutter-invariance
checks for ω-regular languages, Proceedings of SPIN'15.  LNCS 9232.

Describes the stutter-invariance checks that can be selected through
\fBSPOT_STUTTER_CHECK\fR.

.TP
5.
Javier Esparza, Jan Křetínský and Salomon Sickert: One Theorem to Rule
Them All: A Unified Translation of LTL into ω-Automata.  Proceedings
of LICS'18.  To appear.

Describes (among other things) the constructions used for translating
formulas of the form GF(guarantee) or FG(safety), that can be
disabled with \fB-x gf-guarantee=0\fR.


[SEE ALSO]
.BR ltl2tgba (1)
.BR ltl2tgta (1)
.BR dstar2tgba (1)
.BR autfilt (1)
