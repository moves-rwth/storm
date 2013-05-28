/*
 * This file is part of the program ltl2dstar (http://www.ltl2dstar.de/).
 * Copyright (C) 2005-2007 Joachim Klein <j.klein@ltl2dstar.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "plugins/PluginManager.hpp"

class WriteDRAPlugin : public PluginManager::Plugin {
public:
  WriteDRAPlugin() : PluginManager::Plugin("WriteDRA") {}

  virtual result_t outputDRA(DRA_t& dra, std::ostream& out) {
    typedef DRA_t::state_type state_type;

    for (unsigned int i=0;i<dra.size();i++) {
      out << "State " << i;
      if (dra.getStartState() == dra[i]) {
	out << " (start)";
      }
      out << ": ";

      RabinAcceptance::AcceptanceForState acceptance=
	dra[i]->acceptance();

      // for all acceptance pairs j
      for (unsigned int j=0;j<acceptance.size();j++) {
	if (acceptance.isIn_U(j)) {
	  out << "-" << j;
	} else if (acceptance.isIn_L(j)) {
	  out << "+" << j;
	} else {
	  out << " " << j;
	}
      }
      out << std::endl;

      const APSet& ap_set=dra.getAPSet();
      for (APSet::element_iterator el_it=ap_set.all_elements_begin();
	   el_it!=ap_set.all_elements_end();
	   ++el_it) {
	APElement label=*el_it;
	state_type *to_state=dra[i]->edges().get(label);
	unsigned int to_state_index=to_state->getName();
	out << to_state_index << " ";
      }
      out << std::endl;
    }    

    return PLUGIN_CONTINUE;
  }
};

// Construct one instance
WriteDRAPlugin write_dra_plugin;
