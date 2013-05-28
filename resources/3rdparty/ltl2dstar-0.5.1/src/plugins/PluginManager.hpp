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

#ifndef PLUGINMANAGER_HPP
#define PLUGINMANAGER_HPP


#include "LTL2DRA.hpp"
#include <list>
#include <map>
#include <iostream>

/**
 * Class PluginManager, managing plugins.
 * There is a singleton instance of PluginManager.
 */
class PluginManager {
public:
  /** Constructor */
  PluginManager() : _output_plugin(NULL) {}
  /** Destructor */
  ~PluginManager() {}

  /** Abstract Plugin class. */
  class Plugin {
  public:
    /** The diffent result codes */
    enum result_t {PLUGIN_CONTINUE=0, PLUGIN_EXIT=1, PLUGIN_ERROR=2}; 

    /** Constructor
     * @param plugin_name the name under which the plugin is to be registered
     */
    Plugin(std::string plugin_name) : _plugin_name(plugin_name) {
      registerPlugin(plugin_name);
    }
    
    /** Destructor */
    virtual ~Plugin() {}
    
    /** Plugin entry point: afterNBAGeneration */
    virtual result_t afterNBAGeneration(NBA_t& nba) {return PLUGIN_CONTINUE;}

    /** Plugin entry point: afterDRAGeneration */
    virtual result_t afterDRAGeneration(DRA_t& dra) {return PLUGIN_CONTINUE;}

    /** Plugin entry point: outputDRA (for output plugin) */
    virtual result_t outputDRA(DRA_t& dra, std::ostream& out) {return PLUGIN_CONTINUE;}

    /** Plugin entry point: activation
     * @param options command line options, empty string if none were given
     */
    virtual void activate(const std::string& options) {}
    
  private:
    /** The name */
    std::string _plugin_name;
    
    /** Register this plugin with the PluginManager */
    void registerPlugin(const std::string& name) {
      PluginManager& manager=PluginManager::getManager();
      manager.registerPlugin(this, name);
    }
  };

  /** Singleton access method to the PluginManager
   * @return the PluginManager singleton
   */
  static PluginManager& getManager() {
    if (!_manager) {
      _manager=new PluginManager();
    }
    return *_manager;
  }

  /** Register the plugin with the manager */
  void registerPlugin(Plugin* plugin,
		      const std::string& name) {
    _registered_plugins[name]=plugin;
  }

  /** Set the plugin responsible for output of the automaton */
  void setOutputPlugin(const std::string& name) {
    registered_plugin_map_t::iterator it=_registered_plugins.find(name);
    if (it!=_registered_plugins.end()) {
      _output_plugin=(*it).second;
    } else {
      THROW_EXCEPTION(Exception, "There is no registered plugin with name '"+name+"'");
    }

  }

  /** Activate a given plugin */
  void activatePlugin(const std::string& name, const std::string& options) {
    registered_plugin_map_t::iterator it=_registered_plugins.find(name);
    if (it!=_registered_plugins.end()) {
      _active_plugins.push_back((*it).second);
      (*it).second->activate(options);
    } else {
      THROW_EXCEPTION(Exception, "There is no registered plugin with name '"+name+"'");
    }
  }
  
  /** Call the Plugins afterNBAGeneration entry points */
  Plugin::result_t afterNBAGeneration(NBA_t &nba) {
    for (active_plugin_list_t::iterator it=_active_plugins.begin();
	 it!=_active_plugins.end();
	 ++it) {
      Plugin::result_t rv=(*it)->afterNBAGeneration(nba);
      switch (rv) {
      case Plugin::PLUGIN_CONTINUE:
	continue;
      case Plugin::PLUGIN_EXIT:
      case Plugin::PLUGIN_ERROR:
	return rv;
      }
    }
    return Plugin::PLUGIN_CONTINUE;
  }

  /** Call the Plugins afterDRAGeneration entry points */
  Plugin::result_t afterDRAGeneration(DRA_t& dra) {
    for (active_plugin_list_t::iterator it=_active_plugins.begin();
	 it!=_active_plugins.end();
	 ++it) {
      Plugin::result_t rv=(*it)->afterDRAGeneration(dra);
      switch (rv) {
      case Plugin::PLUGIN_CONTINUE:
	continue;
      case Plugin::PLUGIN_EXIT:
      case Plugin::PLUGIN_ERROR:
	return rv;
      }
    }
    return Plugin::PLUGIN_CONTINUE;
  }

  /** Call the output plugins outputDRA entry point */
  void outputDRA(DRA_t& dra, std::ostream& out) {
    if (_output_plugin) {
      _output_plugin->outputDRA(dra, out);
    }
  }

private:
  /** The singleton PluginManager instance */
  static PluginManager* _manager;

  typedef std::list<Plugin*> active_plugin_list_t;
  typedef std::map<std::string, Plugin*> registered_plugin_map_t;
  
  /** The active plugins */
  active_plugin_list_t _active_plugins;
  /** The registered plugins */
  registered_plugin_map_t _registered_plugins;

  /** The plugin responsible for automata output */
  Plugin* _output_plugin;
};

#endif
