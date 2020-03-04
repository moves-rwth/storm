#pragma once
#include "storm/storage/jani/ModelType.h"

namespace storm {
    namespace jani {

        class Model;

        struct InformationObject {
            storm::jani::ModelType modelType;
            uint64_t nrVariables;
            uint64_t nrAutomata;
            uint64_t nrEdges;
            uint64_t nrLocations;
        };

        InformationObject collect(Model const& model);
    }
}

