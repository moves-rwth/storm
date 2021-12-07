.\" -*- coding: utf-8 -*-
[NAME]
autcross \- cross-compare tools that process automata

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
The following columns are output in the CSV files.
.TP
\fBinput.source\fR
Location of the input automaton fed to the tool.
.TP
\fBinput.name\fR
Name of the input automaton, if any.  This is supported
by the HOA format.
.TP
\fBinput.ap\fR,\fBoutput.ap\fR,
Number of atomic proposition in the input and output automata.
.TP
\fBinput.states\fR,\fBoutput.states\fR
Number of states in the input and output automata.
.TP
\fBinput.edges\fR,\fBoutput.edges\fR
Number of edges in the input and output automata.
.TP
\fBinput.transitions\fR,\fBoutput.transitions\fR
Number of transitions in the input and output automata.
.TP
\fBinput.acc_sets\fR,\fBoutput.acc_sets\fR
Number of acceptance sets in the input and output automata.
.TP
\fBinput.scc\fR,\fBoutput.scc\fR
Number of strongly connected components in the input and output automata.
.TP
\fBinput.nondetstates\fR,\fBoutput.nondetstates\fR
Number of nondeterministic states in the input and output automata.
.TP
\fBinput.nondeterministic\fR,\fBoutput.nondetstates\fR
1 if the automaton is nondeterministic, 0 if it is deterministic.
.TP
\fBinput.alternating\fR,\fBoutput.alternating\fR
1 if the automaton has some universal branching, 0 otherwise.

\fBexit_status\fR, \fBexit_code\fR
Information about how the execution of the tool went.
\fBexit_status\fR is a string that can take the following
values:
.RS
.TP
\f(CW"ok"\fR
The tool ran succesfully (this does not imply that the produced
automaton is correct) and autcross could parse the resulting
automaton.  In this case \fBexit_code\fR is always 0.
.TP
\f(CW"timeout"\fR
The tool ran for more than the number of seconds
specified with the \fB\-\-timeout\fR option.  In this
case \fBexit_code\fR is always -1.
.TP
\f(CW"exit code"\fR
The tool terminated with a non-zero exit code.
\fBexit_code\fR contains that value.
.TP
\f(CW"signal"\fR
The tool terminated with a signal.
\fBexit_code\fR contains that signal's number.
.TP
\f(CW"parse error"\fR
The tool terminated normally, but autcross could not
parse its output.  In this case \fBexit_code\fR is always -1.
.TP
\f(CW"no output"\fR
The tool terminated normally, but without creating the specified
output file.  In this case \fBexit_code\fR is always -1.
.RE
.TP
\fBtime\fR
A floating point number giving the run time of the tool in seconds.
This is reported for all executions, even failling ones.

[SEE ALSO]
.BR randaut (1),
.BR genaut (1),
.BR autfilt (1),
.BR ltlcross (1)
