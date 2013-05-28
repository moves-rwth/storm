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
#include <iostream>


/** A plugin calculating reachability in the DRA */
class DRAReachabilityPlugin : public PluginManager::Plugin {
public:
  DRAReachabilityPlugin() : 
    PluginManager::Plugin("DRAReach"), // register name as DRAReach
    filename("DRAReach.txt") // set default filename
  {}

  virtual result_t afterDRAGeneration(DRA_t& dra) {
    // Calculate SCCs in the DRA
    SCCs sccs;
    GraphAlgorithms<DRA_t>::calculateSCCs(dra, sccs);
    
    std::ofstream outfile(filename.c_str());

    for (unsigned int i=0;i<dra.size();i++) {
      for (unsigned int j=0;j<dra.size();j++) {
	if (sccs.stateIsReachable(i,j)) {
	  outfile << "+";
	} else {
	  outfile << "-";
	}
      }
      outfile << std::endl;
    }    

    outfile.close();

    return PLUGIN_CONTINUE;
  }

  /** Activate the plugin, get's empty string if there were no options */
  virtual void activate(const std::string& options) {
    if (!(options=="")) {
      // Set filename
      filename=options;
    }
  }

private:
  std::string filename;
};

// Construct one instance
DRAReachabilityPlugin reachability_plugin;
