#include "src/storage/SymbolicModelDescription.h"

#include "src/utility/prism.h"
#include "src/utility/jani.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace storage {
        
        SymbolicModelDescription::SymbolicModelDescription(storm::jani::Model const& model) : modelDescription(model) {
            // Intentionally left empty.
        }
        
        SymbolicModelDescription::SymbolicModelDescription(storm::prism::Program const& program) : modelDescription(program) {
            // Intentionally left empty.
        }
        
        bool SymbolicModelDescription::hasModel() const {
            return static_cast<bool>(modelDescription);
        }
        
        bool SymbolicModelDescription::isJaniModel() const {
            return modelDescription.get().which() == 0;
        }
        
        bool SymbolicModelDescription::isPrismProgram() const {
            return modelDescription.get().which() == 1;
        }
        
        void SymbolicModelDescription::setModel(storm::jani::Model const& model) {
            modelDescription = model;
        }
        
        void SymbolicModelDescription::setModel(storm::prism::Program const& program) {
            modelDescription = program;
        }
        
        storm::jani::Model const& SymbolicModelDescription::asJaniModel() const {
            STORM_LOG_THROW(isJaniModel(), storm::exceptions::InvalidOperationException, "Cannot retrieve JANI model, because the symbolic description has a different type.");
            return boost::get<storm::jani::Model>(modelDescription.get());
        }
        
        storm::prism::Program const& SymbolicModelDescription::asPrismProgram() const {
            STORM_LOG_THROW(isPrismProgram(), storm::exceptions::InvalidOperationException, "Cannot retrieve JANI model, because the symbolic description has a different type.");
            return boost::get<storm::prism::Program>(modelDescription.get());
        }
        
        void SymbolicModelDescription::preprocess(std::string const& constantDefinitionString) {
            if (this->isJaniModel()) {
                std::map<storm::expressions::Variable, storm::expressions::Expression> substitution = storm::utility::jani::parseConstantDefinitionString(this->asJaniModel(), constantDefinitionString);
                this->modelDescription = this->asJaniModel().defineUndefinedConstants(substitution);
                this->modelDescription = this->asJaniModel().substituteConstants();
            } else if (this->isPrismProgram()) {
                std::map<storm::expressions::Variable, storm::expressions::Expression> substitution = storm::utility::prism::parseConstantDefinitionString(this->asPrismProgram(), constantDefinitionString);
                this->modelDescription = this->asPrismProgram().defineUndefinedConstants(substitution);
                this->modelDescription = this->asPrismProgram().substituteConstants();
            }
        }
        
    }
}