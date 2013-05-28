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

/** A sample plugin demonstrating the interface,
 * activate with --plugin=SamplePlugin
 */
class SamplePlugin : public PluginManager::Plugin {
public:
  /** Constructor */
  SamplePlugin() : 
    PluginManager::Plugin("SamplePlugin") // register name as SamplePlugin
  {}
  
  virtual result_t afterNBAGeneration(NBA_t& nba) {
    std::cerr << "SamplePlugin was called for NBA" << std::endl;
    return PLUGIN_CONTINUE;
  }

  virtual result_t afterDRAGeneration(DRA_t& dra) {
    std::cerr << "SamplePlugin was called for DRA" << std::endl;
    return PLUGIN_CONTINUE;
  }
  
  virtual void activate(const std::string& options) {
    std::cerr << "SamplePlugin was activated with options '" << options << "'" << std::endl;
  }
  
};

// Construct one instance
SamplePlugin sample_plugin;
