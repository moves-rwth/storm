#pragma once

#include <map>
#include <set>
#include <cstdint>

#include <boost/optional.hpp>

#include "src/storage/jani/CompositionVisitor.h"

namespace storm {
    namespace jani {
        
        class Model;
        
        class CompositionInformation {
        public:
            CompositionInformation() = default;
            CompositionInformation(std::map<std::string, uint64_t> const& automatonNameToMultiplicity, std::set<std::string> const& nonsilentActions, bool containsRenaming, bool containsRestrictedParallelComposition);

            void increaseAutomatonMultiplicity(std::string const& automatonName, uint64_t count = 1);
            
            void addNonsilentAction(std::string const& actionName);
            std::set<std::string> const& getNonsilentActions() const;
            static std::set<std::string> renameNonsilentActions(std::set<std::string> const& nonsilentActions, std::map<std::string, boost::optional<std::string>> const& renaming);
            
            void setContainsRenameComposition();
            bool containsRenameComposition() const;
            
            void setContainsRestrictedParallelComposition();
            bool containsRestrictedParallelComposition() const;
            
            static std::map<std::string, uint64_t> joinMultiplicityMaps(std::map<std::string, uint64_t> const& first, std::map<std::string, uint64_t> const& second);
            std::map<std::string, uint64_t> const& getAutomatonToMultiplicityMap() const;
            
        private:
            /// A mapping from the automata's names to the amount of times they occur in the composition.
            std::map<std::string, uint64_t> automatonNameToMultiplicity;
            
            /// The set of non-silent actions contained in the composition.
            std::set<std::string> nonsilentActions;
            
            /// A flag indicating whether the composition contains a renaming composition.
            bool renameComposition;
            
            /// A flag indicating whether the composition contains
            bool restrictedParallelComposition;
        };
        
        class CompositionInformationVisitor : public CompositionVisitor {
        public:
            CompositionInformationVisitor() = default;
            
            CompositionInformation getInformation(Composition const& composition, Model const& model);
            
            virtual boost::any visit(AutomatonComposition const& composition, boost::any const& data) override;
            virtual boost::any visit(RenameComposition const& composition, boost::any const& data) override;
            virtual boost::any visit(ParallelComposition const& composition, boost::any const& data) override;
        };
        
    }
}