#include "storm/storage/jani/traverser/InformationCollector.h"
#include "storm/storage/jani/traverser/JaniTraverser.h"
#include "storm/storage/jani/Model.h"

namespace storm {
    namespace jani {
        namespace detail {

        }

        InformationObject collect(Model const& model) {
            InformationObject result;
            result.modelType = model.getModelType();
            result.nrAutomata = model.getNumberOfAutomata();
            result.nrEdges = model.getNumberOfEdges();
            result.nrVariables = model.getTotalNumberOfNonTransientVariables();
            return result;
        }
    }
}