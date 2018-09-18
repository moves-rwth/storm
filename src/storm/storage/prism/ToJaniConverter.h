#pragma once

#include <map>
#include <string>

namespace storm {
    namespace jani {
        class Model;
        class Property;
    }
    
    namespace prism {
        
        class Program;
        
        class ToJaniConverter {
        public:
            storm::jani::Model convert(storm::prism::Program const& program, bool allVariablesGlobal = true, std::string suffix = "", bool standardCompliant = false);
            
            bool labelsWereRenamed() const;
            bool rewardModelsWereRenamed() const;
            std::map<std::string, std::string> const& getLabelRenaming() const;
            std::map<std::string, std::string> const& getRewardModelRenaming() const;
            
            storm::jani::Property applyRenaming(storm::jani::Property const& property) const;
            std::vector<storm::jani::Property> applyRenaming(std::vector<storm::jani::Property> const& property) const;
            
        private:
            std::map<std::string, std::string> labelRenaming;
            std::map<std::string, std::string> rewardModelRenaming;
        };
        
    }
}
