#pragma once

#include <boost/variant.hpp>

#include "src/storage/jani/Model.h"
#include "src/storage/prism/Program.h"

namespace storm {
    namespace storage {
        
        class SymbolicModelDescription {
        public:
            SymbolicModelDescription(storm::jani::Model const& model);
            SymbolicModelDescription(storm::prism::Program const& program);

            bool isJaniModel() const;
            bool isPrismProgram() const;
            
            storm::jani::Model const& asJaniModel() const;
            storm::prism::Program const& asPrismProgram() const;
            
        private:
            boost::variant<storm::jani::Model, storm::prism::Program> modelDescription;
        };
        
    }
}