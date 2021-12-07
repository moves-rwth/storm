// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2014, 2015, 2016, 2018, 2019 Laboratoire de Recherche
// et Developpement de l'Epita (LRDE).
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

#include "config.h"
#include <spot/tl/snf.hh>

#if defined __clang__
// See https://llvm.org/bugs/show_bug.cgi?id=30636
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif

namespace spot
{
  namespace
  {
    // E°  if bounded=false
    // E^□ if nounded=true
    template<bool bounded>
    class snf_visitor final
    {
    protected:
      snf_cache* cache_;
    public:
      snf_visitor(snf_cache* c)
        : cache_(c)
      {
      }

      formula visit(formula f)
      {
        if (!f.accepts_eword())
          return f;

        snf_cache::const_iterator i = cache_->find(f);
        if (i != cache_->end())
          return i->second;

        formula out;
        switch (f.kind())
          {
          case op::eword:
            out = formula::ff();
            break;
          case op::Star:
            if (!bounded)
              out = visit(f[0]); // Strip the star.
            else
              out = formula::Star(visit(f[0]),
                                  std::max(unsigned(f.min()), 1U), f.max());
            break;
          case op::Concat:
            if (bounded)
              {
                out = f;
                break;
              }
            SPOT_FALLTHROUGH;
          case op::OrRat:
          case op::AndNLM:
            // Let F designate expressions that accept [*0],
            // and G designate expressions that do not.

            // (G₁;G₂;G₃)° = G₁;G₂;G₃
            // (G₁;F₂;G₃)° = (G₁°);F₂;(G₃°) = G₁;F₂;G₃
            // because there is nothing to do recursively on a G.
            //
            // AndNLM can be dealt with similarly.
            //
            // The above cases are already handled by the
            // accepts_eword() tests at the top of this method.  So
            // we reach this switch, we only have to deal with...
            //
            // (F₁;F₂;F₃)° = (F₁°)|(F₂°)|(F₃°)
            // (F₁&F₂&F₃)° = (F₁°)|(F₂°)|(F₃°)
            // (F₁|G₂|F₃)° = (F₁°)|(G₂°)|(F₃°)
            {
              unsigned s = f.size();
              std::vector<formula> v;
              v.reserve(s);
              for (unsigned pos = 0; pos < s; ++pos)
                v.emplace_back(visit(f[pos]));
              out = formula::OrRat(v);
              break;
            }
          case op::ff:
          case op::tt:
          case op::ap:
          case op::Not:
          case op::X:
          case op::strong_X:
          case op::F:
          case op::G:
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
          case op::Xor:
          case op::Implies:
          case op::Equiv:
          case op::U:
          case op::R:
          case op::W:
          case op::M:
          case op::EConcat:
          case op::EConcatMarked:
          case op::UConcat:
          case op::Fusion:
          case op::Or:
          case op::And:
            SPOT_UNREACHABLE();
          case op::AndRat:        // Can AndRat be handled better?
          case op::FStar:        // Can FStar be handled better?
          case op::first_match:  // Can first_match be handled better?
            out = f;
            break;
          }

        return (*cache_)[f] = out;
      }
    };
  }


  formula
  star_normal_form(formula sere, snf_cache* cache)
  {
    snf_visitor<false> v(cache);
    return v.visit(sere);
  }

  formula
  star_normal_form_bounded(formula sere, snf_cache* cache)
  {
    snf_visitor<true> v(cache);
    return v.visit(sere);
  }
}
