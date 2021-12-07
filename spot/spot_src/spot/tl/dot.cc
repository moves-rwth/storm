// -*- coding: utf-8 -*-
// Copyright (C) 2009-2015, 2018-2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6
// (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
// Pierre et Marie Curie.
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
#include <spot/tl/dot.hh>
#include <spot/tl/formula.hh>
#include <unordered_map>
#include <ostream>
#include <sstream>

namespace spot
{
  namespace
  {
    struct dot_printer final
    {
      std::ostream& os_;
      std::unordered_map<formula, int> node_;
      std::ostringstream* sinks_;

      dot_printer(std::ostream& os, formula f)
        : os_(os), sinks_(new std::ostringstream)
        {
          os_ << "digraph G {\n";
          rec(f);
          os_ << "  subgraph atoms {\n     rank=sink;\n"
              << sinks_->str() << "  }\n}\n";
        }

      ~dot_printer()
        {
          delete sinks_;
        }

      int rec(formula f)
      {
        auto i = node_.emplace(f, node_.size());
        int src = i.first->second;
        if (!i.second)
          return src;

        op o = f.kind();
        std::string str = (o == op::ap) ? f.ap_name() : f.kindstr();

        if (o == op::ap || f.is_constant())
          *sinks_ << "    " << src << " [label=\""
                  << str << "\", shape=box];\n";
        else
          os_ << "  " << src << " [label=\"" << str << "\"];\n";

        int childnum = 0;
        switch (o)
          {
          case op::ff:
          case op::tt:
          case op::eword:
          case op::ap:
          case op::Not:
          case op::X:
          case op::strong_X:
          case op::F:
          case op::G:
          case op::Closure:
          case op::NegClosure:
          case op::NegClosureMarked:
          case op::Or:
          case op::OrRat:
          case op::And:
          case op::AndRat:
          case op::AndNLM:
          case op::Star:
          case op::FStar:
          case op::first_match:
            childnum = 0;                // No number for children
            break;
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
            childnum = -2;                // L and R markers
            break;
          case op::Concat:
          case op::Fusion:
            childnum = 1;                // Numbered children
            break;
          }

        for (auto c: f)
          {
            // Do not merge the next two lines, as there is no
            // guarantee that rec will be called before we start
            // printing the transition.
            int dst = rec(c);
            os_ << "  " << src << " -> " << dst;
            if (childnum > 0)
              os_ << " [taillabel=\"" << childnum << "\"]";
            if (childnum == -2)
              os_ << " [taillabel=\"L\"]";
            else if (childnum == -1)
              os_ << " [taillabel=\"R\"]";
            os_ << ";\n";
            if (childnum)
              ++childnum;
          }

        return src;
      }
    };
  }

  std::ostream&
  print_dot_psl(std::ostream& os, formula f)
  {
    dot_printer p(os, f);
    return os;
  }
}
