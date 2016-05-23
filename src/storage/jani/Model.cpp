#include <src/exceptions/InvalidJaniException.h>
#include "src/storage/jani/Model.h"

namespace storm {
    namespace jani {
        
        Model::Model(std::string const& name, ModelType const& modelType, uint64_t version) : name(name), modelType(modelType), version(version) {
            // Intentionally left empty.
        }

        uint64_t Model::addAction(Action const& act) {
            STORM_LOG_THROW(actionToIndex.count(act.getName()) == 0, storm::exceptions::InvalidJaniException, "Action with name " + act.getName() + " already exists");
            actionToIndex.emplace(act.getName(), actions.size());
            actions.push_back(act);
            return actions.size() - 1;
        }

        bool Model::hasAction(std::string const &name) const {
            return actionToIndex.count(name) != 0;
        }

        uint64_t Model::getActionIndex(std::string const& name) const {
            assert(this->hasAction(name));
            return actionToIndex.at(name);
        }


        void Model::checkSupported() const {
            //TODO
        }

        bool Model::checkValidity(bool logdbg) const {
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