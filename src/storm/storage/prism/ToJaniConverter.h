#pragma once

#include <map>
#include <string>

namespace storm {
    namespace jani {
        class Model;
    }
    
    namespace prism {
        
        class Program;
        
        class ToJaniConverter {
        public:
            storm::jani::Model convert(storm::prism::Program const& program, bool allVariablesGlobal = false, std::string suffix = "");
            
            bool labelsWereRenamed() const;
            std::map<std::string, std::string> const& getLabelRenaming() const;
            
        private:
            std::map<std::string, std::string> labelRenaming;
        };
        
    }
}
