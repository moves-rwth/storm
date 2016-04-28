#include "core.h"

#include "common.h"

//#include <src/utility/storm.h>

// Thin wrapper for initializing
void setupStormLib(std::string const& args) {
//    storm::utility::setUp();
//    storm::settings::SettingsManager::manager().setFromString(args);
}

void define_core(py::module& m) {
    
    m.def("set_up", &setupStormLib);
}
