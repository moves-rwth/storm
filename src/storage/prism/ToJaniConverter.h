#pragma once

namespace storm {
    namespace jani {
        class Model;
    }
    
    namespace prism {
        
        class Program;
        
        class ToJaniConverter {
        public:
            storm::jani::Model convert(storm::prism::Program const& program, bool allVariablesGlobal = false) const;
        };
        
    }
}