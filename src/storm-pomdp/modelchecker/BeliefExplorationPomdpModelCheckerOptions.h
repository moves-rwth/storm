#pragma once

#include "storm-pomdp/builder/BeliefMdpExplorer.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/constants.h"

namespace storm {
namespace builder {
template<typename PomdpType, typename BeliefValueType>
class BeliefMdpExplorer;
}
namespace pomdp {
namespace modelchecker {
template<typename ValueType>
struct BeliefExplorationPomdpModelCheckerOptions {
    BeliefExplorationPomdpModelCheckerOptions(bool discretize, bool unfold) : discretize(discretize), unfold(unfold) {
        // Intentionally left empty
    }

    // TODO documentation?
    bool discretize;
    bool unfold;

    bool useClipping = false;
    bool interactiveUnfolding = false;
    bool refine = false;
    bool cutZeroGap = false;
    bool useStateEliminationCutoff = false;
    uint64_t refineStepLimit = 0;
    ValueType refinePrecision = storm::utility::convertNumber<ValueType>(1e-4);
    uint64_t explorationTimeLimit = 0;

    // Control parameters for the refinement heuristic
    // Discretization Resolution
    uint64_t resolutionInit = 2;
    ValueType resolutionFactor = storm::utility::convertNumber<ValueType, uint64_t>(2);
    // The maximal number of newly expanded MDP states in a refinement step
    uint64_t sizeThresholdInit = 0;
    ValueType sizeThresholdFactor = storm::utility::convertNumber<ValueType, uint64_t>(4);
    // Controls how large the gap between known lower- and upper bounds at a belief state needs to be in order to explore
    ValueType gapThresholdInit = storm::utility::convertNumber<ValueType>(0.1);
    ValueType gapThresholdFactor = storm::utility::convertNumber<ValueType>(0.25);
    // Controls whether "almost optimal" choices will be considered optimal
    ValueType optimalChoiceValueThresholdInit = storm::utility::convertNumber<ValueType>(1e-3);
    ValueType optimalChoiceValueThresholdFactor = storm::utility::one<ValueType>();
    // Controls which observations are refined.
    ValueType obsThresholdInit = storm::utility::convertNumber<ValueType>(0.1);
    ValueType obsThresholdIncrementFactor = storm::utility::convertNumber<ValueType>(0.1);

    uint64_t clippingGridRes = 2;

    bool skipHeuristicSchedulers = false;

    ValueType numericPrecision = storm::NumberTraits<ValueType>::IsExact
                                     ? storm::utility::zero<ValueType>()
                                     : storm::utility::convertNumber<ValueType>(1e-9);  /// Used to decide whether two beliefs are equal
    bool dynamicTriangulation = true;  // Sets whether the triangulation is done in a dynamic way (yielding more precise triangulations)

    storm::builder::ExplorationHeuristic explorationHeuristic = storm::builder::ExplorationHeuristic::BreadthFirst;
};
}  // namespace modelchecker
}  // namespace pomdp
}  // namespace storm
