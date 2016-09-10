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

            bool hasModel() const;
            bool isJaniModel() const;
            bool isPrismProgram() const;

            void setModel(storm::jani::Model const& model);
            void setModel(storm::prism::Program const& program);
            
            storm::jani::Model const& asJaniModel() const;
            storm::prism::Program const& asPrismProgram() const;
            
            void preprocess(std::string const& constantDefinitionString = "");
            
        private:
            boost::optional<boost::variant<storm::jani::Model, storm::prism::Program>> modelDescription;
        };
        
    }
}