#pragma once

#include <set>
#include <map>

#include "src/storage/jani/CompositionVisitor.h"

namespace storm {
    namespace jani {
        
        class Model;
        
        class ActionInformation {
        public:
            ActionInformation(std::set<uint64_t> const& nonsilentActionIndices, std::map<uint64_t, std::string> const& indexToNameMap, std::map<std::string, uint64_t> const& nameToIndexMap, uint64_t silentActionIndex = 0);
            
            std::string const& getActionName(uint64_t index) const;
            uint64_t getActionIndex(std::string const& name) const;
            std::set<uint64_t> const& getNonSilentActionIndices() const;
            uint64_t getSilentActionIndex() const;
            
        private:
            uint64_t silentActionIndex;
            std::set<uint64_t> nonsilentActionIndices;
            std::map<uint64_t, std::string> indexToNameMap;
            std::map<std::string, uint64_t> nameToIndexMap;
        };
        
        class CompositionActionInformationVisitor : public CompositionVisitor {
        public:
            CompositionActionInformationVisitor(storm::jani::Model const& model);
            
            ActionInformation getActionInformation(storm::jani::Composition const& composition);
            
            virtual boost::any visit(AutomatonComposition const& composition, boost::any const& data) override;
            virtual boost::any visit(RenameComposition const& composition, boost::any const& data) override;
            virtual boost::any visit(ParallelComposition const& composition, boost::any const& data) override;
            
        private:
            uint64_t addOrGetActionIndex(std::string const& name);
            
            storm::jani::Model const& model;
            uint64_t nextFreeActionIndex;
            std::map<std::string, uint64_t> nameToIndexMap;
            std::map<uint64_t, std::string> indexToNameMap;
        };

        
    }
}
