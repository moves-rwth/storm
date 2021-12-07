// -*- coding: utf-8 -*-
// Copyright (C) 2017, 2018, 2019 Laboratoire de Recherche et Developpement de
// l'EPITA (LRDE).
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

#pragma once

#include <spot/tl/formula.hh>

namespace spot
{
  namespace gen
  {
    /// \ingroup gen
    /// \defgroup genltl Hard-coded families of formulas.
    /// @{

    /// \brief Identifiers for formula patterns
    enum ltl_pattern_id {
      LTL_BEGIN = 256,
      /// `F(p1)&F(p2)&...&F(pn)`
      /// \cite geldenhuys.06.spin
      LTL_AND_F = LTL_BEGIN,
      /// `FG(p1)&FG(p2)&...&FG(pn)`
      LTL_AND_FG,
      /// `GF(p1)&GF(p2)&...&GF(pn)`
      /// \cite cichon.09.depcos ,
      /// \cite geldenhuys.06.spin .
      LTL_AND_GF,
      /// `F(p1&F(p2&F(p3&...F(pn)))) & F(q1&F(q2&F(q3&...F(qn))))`
      /// \cite cichon.09.depcos
      LTL_CCJ_ALPHA,
      /// `F(p&X(p&X(p&...X(p)))) & F(q&X(q&X(q&...X(q))))`
      /// \cite cichon.09.depcos
      LTL_CCJ_BETA,
      /// `F(p&(Xp)&(XXp)&...(X...X(p))) & F(q&(Xq)&(XXq)&...(X...X(q)))`
      /// \cite cichon.09.depcos
      LTL_CCJ_BETA_PRIME,
      /// 55 specification patterns from Dwyer et al.
      /// \cite dwyer.98.fmsp
      LTL_DAC_PATTERNS,
      /// 12 formulas from Etessami and Holzmann.
      /// \cite etessami.00.concur
      LTL_EH_PATTERNS,
      /// `F(p0 | XG(p1 | XG(p2 | ... XG(pn))))`
      LTL_FXG_OR,
      /// `(GFa1 & GFa2 & ... & GFan) <-> GFz`
      LTL_GF_EQUIV,
      /// `GF(a <-> X[n](a))`
      LTL_GF_EQUIV_XN,
      /// `(GFa1 & GFa2 & ... & GFan) -> GFz`
      LTL_GF_IMPLIES,
      /// `GF(a -> X[n](a))`
      LTL_GF_IMPLIES_XN,
      /// `(F(p1)|G(p2))&(F(p2)|G(p3))&...&(F(pn)|G(p{n+1}))`
      /// \cite geldenhuys.06.spin
      LTL_GH_Q,
      /// `(GF(p1)|FG(p2))&(GF(p2)|FG(p3))&...  &(GF(pn)|FG(p{n+1}))`
      /// \cite geldenhuys.06.spin
      LTL_GH_R,
      /// `!((GF(p1)&GF(p2)&...&GF(pn)) -> G(q->F(r)))`
      /// \cite gastin.01.cav
      LTL_GO_THETA,
      /// `G(p0 & XF(p1 & XF(p2 & ... XF(pn))))`
      LTL_GXF_AND,
      /// 55 patterns from the Liberouter project.
      /// \cite holevek.04.tr
      LTL_HKRSS_PATTERNS,
      /// Linear formula with doubly exponential DBA.
      /// \cite kupferman.10.mochart
      LTL_KR_N,
      /// Quasilinear formula with doubly exponential DBA.
      /// \cite kupferman.10.mochart
      LTL_KR_NLOGN,
      /// Quadratic formula with doubly exponential DBA.
      /// \cite kupferman.10.mochart ,
      /// \cite kupferman.05.tcl .
      LTL_KV_PSI,
      /// `GF(a1&X(a2&X(a3&...Xan)))&F(b1&F(b2&F(b3&...&Xbm)))`
      /// \cite muller.17.gandalf
      LTL_MS_EXAMPLE,
      /// `FG(a|b)|FG(!a|Xb)|FG(a|XXb)|FG(!a|XXXb)|...`
      /// \cite muller.17.gandalf
      LTL_MS_PHI_H,
      /// `(FGa{n}&GFb{n})|((FGa{n-1}|GFb{n-1})&(...))`
      /// \cite muller.17.gandalf
      LTL_MS_PHI_R,
      /// `(FGa{n}|GFb{n})&((FGa{n-1}&GFb{n-1})|(...))`
      /// \cite muller.17.gandalf
      LTL_MS_PHI_S,
      /// `FG(p1)|FG(p2)|...|FG(pn)`
      /// \cite cichon.09.depcos
      LTL_OR_FG,
      /// `G(p1)|G(p2)|...|G(pn)`
      /// \cite geldenhuys.06.spin
      LTL_OR_G,
      /// `GF(p1)|GF(p2)|...|GF(pn)`
      /// \cite geldenhuys.06.spin
      LTL_OR_GF,
      /// 20 formulas from BEEM.
      /// \cite pelanek.07.spin
      LTL_P_PATTERNS,
      /// Arbiter for n clients sending requests, and receiving
      /// grants. \cite piterman.06.vmcai using standard
      /// semantics from \cite jacobs.16.synt .
      LTL_PPS_ARBITER_STANDARD,
      /// Arbiter for n clients sending requests, and receiving
      /// grants. \cite piterman.06.vmcai using strict
      /// semantics from \cite jacobs.16.synt .
      LTL_PPS_ARBITER_STRICT,
      /// `(((p1 R p2) R p3) ... R pn)`
      LTL_R_LEFT,
      /// `(p1 R (p2 R (... R pn)))`
      LTL_R_RIGHT,
      /// n-bit counter
      /// \cite rozier.07.spin
      LTL_RV_COUNTER,
      /// n-bit counter with carry
      /// \cite rozier.07.spin
      LTL_RV_COUNTER_CARRY,
      /// linear-size formular for an n-bit counter with carry
      /// \cite rozier.07.spin
      LTL_RV_COUNTER_CARRY_LINEAR,
      /// linear-size formular for an n-bit counter
      /// \cite rozier.07.spin
      LTL_RV_COUNTER_LINEAR,
      /// 27 formulas from Somenzi and Bloem
      /// \cite somenzi.00.cav
      LTL_SB_PATTERNS,
      /// `f(0,j)=(GFa0 U X^j(b))`, `f(i,j)=(GFai U G(f(i-1,j)))`
      /// \cite sickert.16.cav
      LTL_SEJK_F,
      /// `(GFa1&...&GFan) -> (GFb1&...&GFbn)`
      /// \cite sickert.16.cav
      LTL_SEJK_J,
      /// `(GFa1|FGb1)&...&(GFan|FGbn)`
      /// \cite sickert.16.cav
      LTL_SEJK_K,
      /// 3 formulas from Sikert et al.
      /// \cite sickert.16.cav
      LTL_SEJK_PATTERNS,
      /// `G(p -> (q | Xq | ... | XX...Xq)`
      /// \cite tabakov.10.rv
      LTL_TV_F1,
      /// `G(p -> (q | X(q | X(... | Xq)))`
      /// \cite tabakov.10.rv
      LTL_TV_F2,
      /// `G(p -> (q & Xq & ... & XX...Xq)`
      /// \cite tabakov.10.rv
      LTL_TV_G1,
      /// `G(p -> (q & X(q & X(... & Xq)))`
      /// \cite tabakov.10.rv
      LTL_TV_G2,
      /// `G(p1 -> (p1 U (p2 & (p2 U (p3 & (p3 U ...))))))`
      /// \cite tabakov.10.rv
      LTL_TV_UU,
      /// `(((p1 U p2) U p3) ... U pn)`
      /// \cite geldenhuys.06.spin
      LTL_U_LEFT,
      /// `(p1 U (p2 U (... U pn)))`
      /// \cite geldenhuys.06.spin ,
      /// \cite gastin.01.cav .
      LTL_U_RIGHT,
      LTL_END
    };

    /// \brief generate an LTL from a known pattern
    ///
    /// The pattern is specified using one value from the ltl_pattern_id
    /// enum.  See the man page of the `genltl` binary for a
    /// description of those patterns, and bibliographic references.
    SPOT_API formula ltl_pattern(ltl_pattern_id pattern, int n, int m = -1);

    /// \brief convert an ltl_pattern_id value into a name
    ///
    /// The returned name is suitable to be used as an option
    /// key for the genltl binary.
    SPOT_API const char* ltl_pattern_name(ltl_pattern_id pattern);

    /// \brief upper bound for LTL patterns
    ///
    /// If an LTL pattern has an upper bound, this returns it.
    /// Otherwise, this returns 0.
    SPOT_API int ltl_pattern_max(ltl_pattern_id pattern);

    /// \brief argument count for LTL patterns
    ///
    /// Return the number of arguments expected by an LTL pattern.
    SPOT_API int ltl_pattern_argc(ltl_pattern_id pattern);
    /// @}
  }
}
