#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/utility/graph.h"

namespace storm {
    namespace abstraction {

        template<storm::dd::DdType Type>
        struct QualitativeGameResultMinMax {
        public:
            QualitativeGameResultMinMax() = default;
            
            storm::utility::graph::GameProb01Result<Type> prob0Min;
            storm::utility::graph::GameProb01Result<Type> prob1Min;
            storm::utility::graph::GameProb01Result<Type> prob0Max;
            storm::utility::graph::GameProb01Result<Type> prob1Max;
        };

    }
}
