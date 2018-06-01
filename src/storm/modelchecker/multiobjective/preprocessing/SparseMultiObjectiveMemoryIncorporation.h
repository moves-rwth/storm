#pragma once

#include <memory>

#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/logic/MultiObjectiveFormula.h"
#include "storm/logic/Formula.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            namespace preprocessing {
                
                template <class SparseModelType>
                class SparseMultiObjectiveMemoryIncorporation {
                
                typedef typename SparseModelType::ValueType ValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                
                
    
                public:
                    static std::shared_ptr<SparseModelType> incorporateGoalMemory(SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula);
                    static std::shared_ptr<SparseModelType> incorporateFullMemory(SparseModelType const& model, uint64_t memoryStates);
    
                };
            }
        }
    }
}


