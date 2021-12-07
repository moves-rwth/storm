[NAME]
dstar2tgba \- convert automata into Büchi automata
[HISTORY]
.B dstar2tgba
was introduced in Spot 1.2 as a command that reads automata
in
.BR ltl2dstar 's
format, and converts them into TGBA.  At this time it was
the only command-line tool being able to read automata.
.PP
In Spot 1.99.1 the
.B autfilt
command was introduced, but could only read automata
in the HOA format, or in
.BR lbtt 's
format, or as never claims.  So
.B dstar2tgba
was still the only way to process automata
in
.BR ltl2dstar 's
format.
.PP
In Spot 1.99.4 the parser for
.BR ltl2dstar 's
format was finally merged with the parser
used by
.B autfilt
for reading the other format.  This implies not only
that
.B autfilt
can now read
.BR ltl2dstar's
format, but also that
.B dstar2tgba
can read the other formats as well.

Nowadays, the command
.PP
.in +4n
.nf
.ft C
% dstar2tgba some files
.fi
.PP
can be used as a shorthand for
.PP
.in +4n
.nf
.ft C
% autfilt \-\-tgba \-\-high \-\-small some files
.fi
.PP
The name
.B dstar2tgba
is kept for backward compatibility and because it is used
in at least one published paper, but naming this tool
.B aut2tgba
would make more sense.

[BIBLIOGRAPHY]
.TP
1.
.UR http://www.ltl2dstar.de/docs/ltl2dstar.html
The
.BR ltl2dstar manual
.UE .

Documents the output format of
.BR ltl2dstar .

.TP
2.
Chistof Löding: Mehods for the Transformation of ω-Automata:
Complexity and Connection to Second Order Logic.  Diploma Thesis.
University of Kiel. 1998.

Describes various tranformations from non-deterministic Rabin and
Streett automata to Büchi automata.  Slightly optimized variants of
these transformations are used by dstar2tgba for the general cases.

.TP
3.
Sriram C. Krishnan, Anuj Puri, and Robert K. Brayton: Deterministic
ω-automata vis-a-vis Deterministic Büchi Automata.  ISAAC'94.

Explains how to preserve the determinism of Rabin and Streett automata
when the property can be repreted by a Deterministic automaton.
dstar2tgba implements this for the Rabin case only.  In other words,
translating a deterministic Rabin automaton with dstar2tgba will
produce a deterministic TGBA or BA if such a automaton exists.

.TP
4.
Souheib Baarir and Alexandre Duret-Lutz: Mechanizing the minimization
of deterministic generalized Büchi automata.  Proceedings of FORTE'14.
LNCS 8461.

Explains the SAT-based minimization techniques that can be used (on
request only) by dstar2tgba to minimize deterministic Büchi automata.

.TP
5.
Souheib Baarir and Alexandre Duret-Lutz: SAT-based minimization of
deterministic ω-automata.  Proceedings of LPAR'15 (a.k.a LPAR-20).
LNCS 9450.

Extends the previous paper by allowing arbitrary acceptance
conditions.
[SEE ALSO]
.BR spot-x (7),
.BR autfilt (1)
