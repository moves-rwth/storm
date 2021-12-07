// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2014, 2016 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
// Copyright (C) 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
// département Systèmes Répartis Coopératifs (SRC), Université Pierre
// et Marie Curie.
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

#include <spot/twaalgos/gtec/status.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/emptiness_stats.hh>

namespace spot
{
  /// Compute a counter example from a spot::couvreur99_check_status
  class SPOT_API couvreur99_check_result final:
    public emptiness_check_result,
    public acss_statistics
  {
  public:
    couvreur99_check_result(const
                            std::shared_ptr<const couvreur99_check_status>& ecs,
                            option_map o = option_map());

    virtual twa_run_ptr accepting_run() override;

    void print_stats(std::ostream& os) const;

    virtual unsigned acss_states() const override;

  protected:
    /// Called by accepting_run() to find a cycle which traverses all
    /// acceptance conditions in the accepted SCC.
    void accepting_cycle();

  private:
    std::shared_ptr<const couvreur99_check_status> ecs_;
    twa_run_ptr run_;
  };
}
