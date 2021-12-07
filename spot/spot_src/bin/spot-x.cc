// -*- coding: utf-8 -*-
// Copyright (C) 2013-2020 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "common_sys.hh"
#include <string>
#include <iostream>
#include <cstdlib>
#include <argp.h>
#include "common_setup.hh"

const char argp_program_doc[] ="\
Common fine-tuning options for binaries built with Spot.\n\
\n\
The argument of -x or --extra-options is a comma-separated list of KEY=INT \
assignments that are passed to the post-processing routines (they may \
be passed to other algorithms in the future). These options are \
mostly used for benchmarking and debugging purpose. KEYR (without any \
value) is a shorthand for KEY=1, while !KEY is a shorthand for KEY=0.";

#define DOC(NAME, TXT) NAME, 0, nullptr, OPTION_DOC | OPTION_NO_USAGE, TXT, 0

static const argp_option options[] =
  {
    { nullptr, 0, nullptr, 0, "Temporal logic simplification options:", 0 },
    { DOC("tls-impl",
          "Control usage of implication-based rewriting. \
(0) disables it, (1) enables rules based on syntactic implications, \
(2) additionally allows automata-based implication checks, (3) enables \
more rules based on automata-based implication checks.  The default value \
depends on the --low, --medium, or --high settings.") },
    { nullptr, 0, nullptr, 0, "Translation options:", 0 },
    { DOC("ltl-split", "Set to 0 to disable the translation of automata \
as product or sum of subformulas.") },
    { DOC("comp-susp", "Set to 1 to enable compositional suspension, \
as described in our SPIN'13 paper (see Bibliography below).  Set to 2, \
to build only the skeleton TGBA without composing it.  Set to 0 (the \
default) to disable.") },
    { DOC("early-susp", "When set to 1, start compositional suspension on \
the transitions that enter accepting SCCs, and not only on the transitions \
inside accepting SCCs.  This option defaults to 0, and is only used when \
comp-susp=1.") },
    { DOC("skel-simul", "Default to 1.  Set to 0 to disable simulation \
on the skeleton automaton during compositional suspension. Only used when \
comp-susp=1.") },
    { DOC("skel-wdba", "Set to 0 to disable WDBA \
minimization on the skeleton automaton during compositional suspension. \
Set to 1 always WDBA-minimize the skeleton .  Set to 2 to keep the WDBA \
only if it is smaller than the original skeleton.  This option is only \
used when comp-susp=1 and default to 1 or 2 depending on whether --small \
or --deterministic is specified.") },
    { nullptr, 0, nullptr, 0, "Postprocessing options:", 0 },
    { DOC("scc-filter", "Set to 1 (the default) to enable \
SCC-pruning and acceptance simplification at the beginning of \
post-processing. Transitions that are outside of accepting SCC are \
removed from accepting sets, except those that enter into an accepting \
SCC. Set to 2 to remove even these entering transition from the \
accepting sets. Set to 0 to disable this SCC-pruning and acceptance \
simpification pass.") },
    { DOC("degen-reset", "If non-zero (the default), the \
degeneralization algorithm will reset its level any time it exits \
an SCC.") },
    { DOC("degen-lcache", "If non-zero (the default is 1), whenever the \
degeneralization algorithm enters an SCC on a state that has already \
been associated to a level elsewhere, it should reuse that level. \
Different values can be used to select which level to reuse: 1 always \
uses the first level created, 2 uses the minimum level seen so far, and \
3 uses the maximum level seen so far. The \"lcache\" stands for \
\"level cache\".") },
    { DOC("degen-order", "If non-zero, the degeneralization algorithm \
will compute an independent degeneralization order for each SCC it \
processes.  This is currently disabled by default.") },
    { DOC("degen-lskip", "If non-zero (the default), the degeneralization \
algorithm will skip as much levels as possible for each transition.  This \
is enabled by default as it very often reduce the number of resulting \
states.  A consequence of skipping levels is that the degeneralized \
automaton tends to have smaller cycles around the accepting states.  \
Disabling skipping will produce automata with large cycles, and often \
with more states.") },
    { DOC("degen-lowinit", "Whenever the degeneralization algorihm enters \
a new SCC (or starts from the initial state), it starts on some level L that \
is compatible with all outgoing transitions.  If degen-lowinit is zero \
(the default) and the corresponding state (in the generalized automaton) \
has an accepting self-loop, then level L is replaced by the accepting \
level, as it might favor finding accepting cycles earlier.  If \
degen-lowinit is non-zero, then level L is always used without looking \
for the presence of an accepting self-loop.") },
    { DOC("degen-remscc", "If non-zero (the default), make sure the output \
of the degenalization has as many SCCs as the input, by removing superfluous \
ones.") },
    { DOC("det-max-states", "When defined to a positive integer N, \
determinizations will be aborted whenever the number of generated \
states would exceed N.  In this case a non-deterministic automaton \
will be returned.")},
    { DOC("det-max-edges", "When defined to a positive integer N, \
determinizations will be aborted whenever the number of generated \
edges would exceed N.  In this case a non-deterministic automaton \
will be returned.")},
    { DOC("det-scc", "Set to 0 to disable scc-based optimizations in \
the determinization algorithm.") },
    { DOC("det-simul", "Set to 0 to disable simulation-based optimizations in \
the determinization algorithm.") },
    { DOC("det-stutter", "Set to 0 to disable optimizations based on \
the stutter-invariance in the determinization algorithm.") },
    { DOC("gen-reduce-parity", "When the postprocessor routines are \
configured to output automata with any kind of acceptance condition, \
but they happen to process an automaton with parity acceptance, they \
call a function to minimize the number of colors needed.  This option \
controls what happen when this reduction does not reduce the number of \
colors: when set (the default) the output of the reduction is returned, \
this means the colors in the automaton may have changed slightly, and in \
particular, there is no transition with more than one color; when unset, \
the original automaton is returned.") },
    { DOC("gf-guarantee", "Set to 0 to disable alternate constructions \
for GF(guarantee)->[D]BA and FG(safety)->DCA.  Those constructions \
are from an LICS'18 paper by J. Esparza, J. Křentínský, and S. Sickert.  \
This is enabled by default for medium and high optimization \
levels.  Unless we are building deterministic automata, the \
resulting automata are compared to the automata built using the \
more traditional pipeline, and only kept if they are better.") },
    { DOC("simul", "Set to 0 to disable simulation-based reductions. \
Set to 1 to use only direct simulation. Set to 2 to use only reverse \
simulation. Set to 3 to iterate both direct and reverse simulations. \
The default is 3, except when option --low is specified, in which case \
the default is 1.") },
    { DOC("ba-simul", "Set to 0 to disable simulation-based reductions \
on automata where state-based acceptance must be preserved (e.g., \
after degeneralization has been performed).   The name suggests this applies \
only to Büchi automata for historical reasons; it really applies to any \
state-based acceptance nowadays. \
Set to 1 to use only direct simulation.  Set to 2 to use only reverse \
simulation.  Set to 3 to iterate both direct and reverse simulations.   \
The default is 3 in --high mode, and 0 otherwise.") },
    { DOC("relabel-bool", "If set to a positive integer N, a formula \
with N atomic propositions or more will have its Boolean subformulas \
abstracted as atomic propositions during the translation to automaton. \
This relabeling can speeds the translation if a few Boolean subformulas \
use a large number of atomic propositions.  By default N=4.  Setting \
this value to 0 will disable the rewriting.") },
    { DOC("wdba-minimize", "Set to 0 to disable WDBA-minimization, to 1 to \
always try it, or 2 to attempt it only on syntactic obligations or on automata \
that are weak and deterministic.  The default is 1 in --high mode, else 2 in \
--medium or --deterministic modes, else 0 in --low mode.") },
    { DOC("tba-det", "Set to 1 to attempt a powerset determinization \
if the TGBA is not already deterministic.  Doing so will degeneralize \
the automaton.  This is disabled by default, unless sat-minimize is set.") },
    { DOC("sat-minimize",
          "Set it to enable SAT-based minimization of deterministic \
TGBA. Depending on its value (from 1 to 4) it changes the algorithm \
to perform. The default value is (1) and it proves to be the most effective \
method. SAT-based minimization uses PicoSAT (distributed with Spot), but \
another installed SAT-solver can be set thanks to the SPOT_SATSOLVER \
environment variable. Enabling SAT-based minimization will also enable \
tba-det.") },
    { DOC("sat-incr-steps", "Set the value of sat-incr-steps. This variable \
is used by two SAT-based minimization algorithms: (2) and (3). They are both \
described below.") },
    { DOC("sat-langmap", "Find the lower bound of default sat-minimize \
procedure (1). This relies on the fact that the size of the minimal automaton \
is at least equal to the total number of different languages recognized by \
the automaton's states.") },
    { DOC("sat-states",
          "When this is set to some positive integer, the SAT-based \
minimization will attempt to construct a TGBA with the given number of \
states.  It may however return an automaton with less states if some of \
these are unreachable or useless.  Setting sat-states automatically \
enables sat-minimize, but no iteration is performed.  If no equivalent \
automaton could be constructed with the given number of states, the original \
automaton is returned.") },
    { DOC("sat-acc",
          "When this is set to some positive integer, the SAT-based will \
attempt to construct a TGBA with the given number of acceptance sets. \
states.  It may however return an automaton with less acceptance sets if \
some of these are useless.  Setting sat-acc automatically \
sets sat-minimize to 1 if not set differently.") },
    { DOC("state-based",
          "Set to 1 to instruct the SAT-minimization procedure to produce \
a TGBA where all outgoing transition of a state have the same acceptance \
sets.  By default this is only enabled when option -B is used.") },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp_child children[] =
  {
    { &misc_argp_hidden, 0, nullptr, -1 },
    { nullptr, 0, nullptr, 0 }
  };

int
main(int argc, char** argv)
{
  setup(argv);

  const argp ap = { options, nullptr, nullptr, argp_program_doc, children,
                    nullptr, nullptr };

  if (int err = argp_parse(&ap, argc, argv, ARGP_NO_HELP, nullptr, nullptr))
    exit(err);

  std::cerr << "This binary serves no purpose other than generating"
            << " the spot-x.7 manpage.\n";

  return 1;
}
