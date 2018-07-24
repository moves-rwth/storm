#pragma once

#include <string>
#include <vector>

namespace storm {
    namespace converter {

        struct JaniConversionOptions {
            
            JaniConversionOptions();
            
            /// (Automaton,Variable)-pairs that will be transformed to location variables of the respective automaton.
            std::vector<std::pair<std::string, std::string>> locationVariables;
            
            /// If set, the model will be made standard compliant (e.g. no state rewards for discrete time models)
            bool standardCompliant;
            
            /// If set, the model is transformed into a single automaton
            bool exportFlattened;
            
        };
    }
}

