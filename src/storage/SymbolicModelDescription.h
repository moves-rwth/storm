#pragma once

#include <boost/variant.hpp>

#include "src/storage/jani/Model.h"
#include "src/storage/prism/Program.h"

namespace storm {
    namespace storage {
        
        class SymbolicModelDescription {
        public:
            SymbolicModelDescription() = default;
            SymbolicModelDescription(storm::jani::Model const& model);
            SymbolicModelDescription(storm::prism::Program const& program);

            SymbolicModelDescription& operator=(storm::jani::Model const& model);
            SymbolicModelDescription& operator=(storm::prism::Program const& program);
            
            bool hasModel() const;
            bool isJaniModel() const;
            bool isPrismProgram() const;

            void setModel(storm::jani::Model const& model);
            void setModel(storm::prism::Program const& program);
            
            storm::jani::Model const& asJaniModel() const;
            storm::prism::Program const& asPrismProgram() const;
            
            SymbolicModelDescription toJani(bool makeVariablesGlobal = true) const;
            
            SymbolicModelDescription preprocess(std::string const& constantDefinitionString = "") const;
            
        private:
            boost::optional<boost::variant<storm::jani::Model, storm::prism::Program>> modelDescription;
        };
        
    }
}