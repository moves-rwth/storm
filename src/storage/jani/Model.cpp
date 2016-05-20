#include "src/storage/jani/Model.h"

namespace storm {
    namespace jani {
        
        Model::Model(ModelType const& modelType, uint64_t version) : modelType(modelType), version(version) {
            // Intentionally left empty.
        }

        void Model::checkSupported() {
            //TODO
        }

        bool Model::checkValidity(bool logdbg) {
            // TODO switch to exception based return value.

            if (version == 0) {
                if(logdbg) STORM_LOG_DEBUG("Jani version is unspecified");
                return false;
            }

            if(modelType == ModelType::UNDEFINED) {
                if(logdbg) STORM_LOG_DEBUG("Model type is unspecified");
                return false;
            }

            if(automata.empty()) {
                if(logdbg) STORM_LOG_DEBUG("No automata specified");
                return false;
            }
            // All checks passed.
            return true;

        }
        
    }
}