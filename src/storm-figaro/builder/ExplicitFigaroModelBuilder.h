#pragma once

#include <boost/optional/optional.hpp>
#include <stack>
#include <unordered_set>
#include <limits>

#include "storm/models/sparse/StateLabeling.h"
#include "storm/models/sparse/ChoiceLabeling.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Model.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/sparse/StateStorage.h"

#include "storm-dft/builder/DftExplorationHeuristic.h"
#include "storm-figaro/generator/FigaroNextStateGenerator.h"
#include "storm-figaro/model/FigaroModel.h"
#include "storm-dft/storage/dft/SymmetricUnits.h" ///We donot need the explortiion heurisitc
#include "storm-dft/storage/BucketPriorityQueue.h"

namespace storm{
    namespace figaro{
        namespace builder{
            template<typename ValueType, typename StateType = uint32_t>
            class ExplicitFIGAROModelBuilder {
                
            public:
                /*!
                 * Constructor.
                 *
                 * @param figaro Figaro.
                 * @param symmetries Symmetries in the figaro model.
                 */
                ExplicitFIGAROModelBuilder(storm::figaro::FigaroProgram const& fogaromodel, storm::storage::DFTIndependentSymmetries const& symmetries);
                
            };
                
        }//builder
    }//figaro
}//storm
