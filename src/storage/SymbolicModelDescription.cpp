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
        
        SymbolicModelDescription& SymbolicModelDescription::operator=(storm::jani::Model const& model) {
            this->modelDescription = model;
            return *this;
        }
        
        SymbolicModelDescription& SymbolicModelDescription::operator=(storm::prism::Program const& program) {
            this->modelDescription = program;
            return *this;
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
        
        SymbolicModelDescription SymbolicModelDescription::toJani(bool makeVariablesGlobal) const {
            if (this->isJaniModel()) {
                return *this;
            }
            if (this->isPrismProgram()) {
                return SymbolicModelDescription(this->asPrismProgram().toJani(makeVariablesGlobal));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Cannot transform model description to the JANI format.");
            }
        }
        
        SymbolicModelDescription SymbolicModelDescription::preprocess(std::string const& constantDefinitionString) const {
            if (this->isJaniModel()) {
                std::map<storm::expressions::Variable, storm::expressions::Expression> substitution = storm::utility::jani::parseConstantDefinitionString(this->asJaniModel(), constantDefinitionString);
                storm::jani::Model preparedModel = this->asJaniModel().defineUndefinedConstants(substitution).substituteConstants();
                if (preparedModel.hasTransientEdgeDestinationAssignments()) {
                    preparedModel.liftTransientEdgeDestinationAssignments();
                    if (preparedModel.hasTransientEdgeDestinationAssignments()) {
                        STORM_LOG_WARN("JANI model has non-liftable transient edge-destinations assignments, which are currently not supported. Trying to lift these assignments to edges rather than destinations failed.");
                    }
                }
                return SymbolicModelDescription(preparedModel);
            } else if (this->isPrismProgram()) {
                std::map<storm::expressions::Variable, storm::expressions::Expression> substitution = storm::utility::prism::parseConstantDefinitionString(this->asPrismProgram(), constantDefinitionString);
                return SymbolicModelDescription(this->asPrismProgram().defineUndefinedConstants(substitution).substituteConstants());
            }
            return *this;
        }
        
    }
}
