.\" -*- coding: utf-8 -*-
[NAME]
ltl2tgba \- translate LTL/PSL formulas into Büchi automata
[NOTE ON TGBA]
TGBA stands for Transition-based Generalized Büchi Automaton.  The
name was coined by Dimitra Giannakopoulou and Flavio Lerda in their
FORTE'02 paper (From States to Transitions: Improving Translation of
LTL Formulae to Büchi Automata), although similar automata have been
used under different names long before that.
.PP
As its name implies a TGBA uses a generalized Büchi acceptance
condition, meanings that a run of the automaton is accepted iff it
visits ininitely often multiple acceptance sets, and it also uses
transition-based acceptance, i.e., those acceptance sets are sets of
transitions.  TGBA are often more consise than traditional Büchi
automata.  For instance the LTL formula \f(CWGFa & GFb\fR can be
translated into a single-state TGBA while a traditional Büchi
automaton would need 3 states.  Compare
.PP
.in +4n
.nf
.ft C
% ltl2tgba 'GFa & GFb'
.fi
.PP
with
.PP
.in +4n
.ft C
.nf
% ltl2tgba --ba 'GFa & GFb'
.fi
.PP
In the dot output produced by the above commands, the membership of
the transitions to the various acceptance sets is denoted using names
in braces.  The actuall names do not really matter as they may be
produced by the translation algorithm or altered by any latter
postprocessing.
.PP
When the \fB\-\-ba\fR option is used to request a Büchi automaton, Spot
builds a TGBA with a single acceptance set, and in which for any state
either all outgoing transitions are accepting (this is equivalent to
the state being accepting) or none of them are.  Double circles are
used to highlight accepting states in the output, but the braces
denoting the accepting transitions are still shown because the
underling structure really is a TGBA.

[NOTE ON LBTT'S FORMAT]
.UR http://www.tcs.hut.fi/Software/lbtt/doc/html/Format-for-automata.html
LBTT's format
.UE
has support for both transition-based and state based generalized acceptance.
.PP
Because Spot uses transition-based generalized Büchi automata
internally, it will normally use the transition-based flavor of that
format, indicated with a 't' flag after the number of acceptance sets.
For instance:
.PP
.in +4n
.ft C
.nf
% ltl2tgba --lbtt 'GFp0 & GFp1 & FGp2'
2 2t                   // 2 states, 2 transition-based acceptance sets
0 1                    // state 0: initial
0 -1 t                 //   trans. to state 0, no acc., label: true
1 -1 | & p0 p2 & p1 p2 //   trans. to state 1, no acc., label: (p0&p2)|(p1&p2)
-1                     // end of state 0
1 0                    // state 1: not initial
1 0 1 -1 & & p0 p1 p2  //   trans. to state 1, acc. 0 and 1, label: p0&p1&p2
1 0 -1 & & p1 p2 ! p0  //   trans. to state 1, acc. 0, label: !p0&p1&p2
1 1 -1 & & p0 p2 ! p1  //   trans. to state 1, acc. 1, label: p0&!p1&p2
1 -1 & & p2 ! p0 ! p1  //   trans. to state 1, no acc., label: !p0&!p1&p2
-1                     // end if state 1
.fi
.PP
Here, the two acceptance sets are represented by the numbers 0 and 1,
and they each contain two transitions (the first transition of state 1
belongs to both sets).
.PP
When both \fB\-\-ba\fR and \fB\-\-lbtt\fR options are used,
the state-based flavor of
the format is used instead.  Note that the LBTT format supports
generalized acceptance conditions on states, but Spot only use this
format for Büchi automata, where there is always only one acceptance
set.  Unlike in the LBTT documentation, we do not use the
optional '\fBs\fR' flag to indicate the state-based acceptance, this way our
output is also compatible with that of
.UR http://www.tcs.hut.fi/Software/maria/tools/lbt/
LBT
.UE .
.PP
.in +4n
.ft C
.nf
% ltl2tgba --ba --lbtt FGp0
2 1                 // 2 states, 1 (state-based) accepance set
0 1 -1              // state 0: initial, non-accepting
0 t                 //   trans. to state 0, label: true
1 p0                //   trans. to state 1, label: p0
-1                  // end of state 0
1 0 0 -1            // state 1: not initial, in acceptance set 0
1 p0                //   trans. to state 0, label: p0
-1                  // end if state 1
.fi
.PP
You can force ltl2tgba to use the transition-based flavor of the
format even for Büchi automaton using \fB\-\-lbtt=t\fR.
.PP
.in +4n
.ft C
.nf
% ltl2tgba --ba --lbtt=t FGp0
2 1t                // 2 states, 1 transition-based accepance set.
0 1                 // state 0: initial
0 -1 t              //   trans. to state 0, no acc., label: true
1 -1 p0             //   trans. to state 1, no acc., label: p0
-1                  // end of state 0
1 0                 // state 1: not initial
1 0 -1 p0           //   trans. to state 1, acc. 0, label: p0
-1                  // end if state 1
.fi
.PP
When representing a Büchi automaton using transition-based acceptance,
all transitions leaving accepting states are put into the acceptance set.
.PP
A final note concerns the name of the atomic propositions.  The
original LBTT and LBT formats require these atomic propositions to
have names such as '\fBp0\fR', '\fBp32\fR', ...  We extend the format to accept
atomic proposition with arbitrary names that do not conflict with
LBT's operators (e.g. '\fBi\fR' is the symbol of the implication operator so
it may not be used as an atomic proposition), or as double-quoted
strings.  Spot will always output atomic-proposition that do not match
\fBp[0-9]+\fR as double-quoted strings.
.PP
.in +4n
.ft C
.nf
% ltl2tgba --lbtt 'GFa & GFb'
1 2t
0 1
0 0 1 -1 & "a" "b"
0 0 -1 & "b" ! "a"
0 1 -1 & "a" ! "b"
0 -1 & ! "b" ! "a"
-1
.fi

[NOTE ON GENERATING MONITORS]
The monitors generated with option \fB\-M\fR are finite state automata
used to reject finite words that cannot be extended to infinite words
compatible with the supplied formula.  The idea is that the monitor
should progress alongside the system, and can only make decisions
based on the finite prefix read so far.
.PP
Monitors can be seen as Büchi automata in which all recognized runs are
accepting.  As such, the only infinite words they can reject are those
are not recognized, i.e., infinite words that start with a bad prefix.
.PP
Because of this limited expressiveness, a monitor for some given LTL
or PSL formula may accept a larger language than the one specified by
the formula.  For instance a monitor for the LTL formula \f(CWa U b\fR
will reject (for instance) any word starting with \f(CW!a&!b\fR as
there is no way such a word can validate the formula, but it will not
reject a finite prefix repeating only \f(CWa&!b\fR as such a prefix
could be extented in a way that is comptible with \f(CWa U b\fR.
.PP
For more information about monitors, we refer the readers to the
following two papers (the first paper describes the construction of
the second paper in a more concise way):
.TP
\(bu
Deian Tabakov and Moshe Y. Vardi: Optimized Temporal Monitors for SystemC.
Proceedings of RV'10.  LNCS 6418.
.TP
\(bu
Marcelo d'Amorim and Grigoire Roşu: Efficient monitoring of
ω-languages.  Proceedings of CAV'05.  LNCS 3576.

[BIBLIOGRAPHY]
If you would like to give a reference to this tool in an article,
we suggest you cite one of the following papers:
.TP
\(bu
Alexandre Duret-Lutz: LTL translation improvements in Spot 1.0.
Int. J. on Critical Computer-Based Systems, 5(1/2):31--54, March 2014.
.TP
\(bu
Alexandre Duret-Lutz: Manipulating LTL formulas using Spot 1.0.
Proceedings of ATVA'13.  LNCS 8172.
.TP
\(bu
Tomáš Babiak, Thomas Badie, Alexandre Duret-Lutz, Mojmír Křetínský,
and Jan Strejček: Compositional approach to suspension and other
improvements to LTL translation.  Proceedings of SPIN'13.  LNCS 7976.
.TP
\(bu
Souheib Baarir and Alexandre Duret-Lutz: Mechanizing the minimization
of deterministic generalized Büchi automata.  Proceedings of FORTE'14.
LNCS 8461.

[SEE ALSO]
.BR spot-x (7)
