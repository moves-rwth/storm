#pragma once

#include <cstdint>
#include <set>
#include "adapters/RationalFunctionAdapter.h"
#include "modelchecker/CheckTask.h"
#include "models/sparse/Dtmc.h"
#include "models/sparse/StateLabeling.h"
#include "storage/FlexibleSparseMatrix.h"
#include "storm-pars/utility/parametric.h"

namespace storm {
namespace transformer {

class TimeTravelling {
   public:
    /**
     * This class re-orders parameteric transitions of a pMC so bounds computed by Parameter Lifting will eventually be better.
     * The parametric reachability probability for the given check task will be the same in the time-travelled and in the original model.
     */
    TimeTravelling() = default;

    /**
     * Perform big-step on the given model and the given checkTask.
     *
     * @param model A pMC.
     * @param checkTask A property (probability or reward) on the pMC.
     * @param horizon How big to step: maximal degree of the resulting polynomials.
     * @param timeTravel Whether to time-travel, i.e., add new states to consolidate similar transitions.
     * @return models::sparse::Dtmc<RationalFunction> The time-travelled pMC.
     */
    models::sparse::Dtmc<RationalFunction> bigStep(models::sparse::Dtmc<RationalFunction> const& model,
                                                   modelchecker::CheckTask<logic::Formula, RationalFunction> const& checkTask, uint64_t horizon = 4,
                                                   bool timeTravel = true);

   private:
    /**
     * updateTreeStates updates the `treeStates` map on the given states.
     * The `treeStates` map keeps track of the parametric transitions reachable with constants from any given state: for some parameter, for some state, this
     * set of parametric transitions is reachable by constant transitions. This function creates or updates this map by searching from the transitions in the
     * working sets upwards.
     *
     * @param treeStates The tree states map to update.
     * @param workingSets Where to start the search. When creating the tree states map: set this to all states with parametric transitions.
     * @param flexibleMatrix The flexible matrix of the pMC.
     * @param flexibleMatrix The transposed flexibleMatrix.
     * @param allParameters The set of all parameters of the pMC.
     * @param stateRewardVector The state reward vector of the pMC.
     * @param stateLabelling The state labelling of the pMC.
     * @param labelsInFormula The labels that occur in the property.
     */
    void updateTreeStates(std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
                          std::map<RationalFunctionVariable, std::set<uint64_t>>& workingSets,
                          const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                          const storage::FlexibleSparseMatrix<RationalFunction>& backwardsTransitions, const std::set<carl::Variable>& allParameters,
                          const boost::optional<std::vector<RationalFunction>>& stateRewardVector, const models::sparse::StateLabeling stateLabelling,
                          const std::set<std::string> labelsInFormula);

    /**
     * extendStateLabeling extends the given state labeling to newly created states. It will set the new labels to the labels on the given state.
     *
     * @param oldLabeling The old labeling.
     * @param oldSize The size of the old labeling.
     * @param newSize The size of the new labeling (>= oldSize).
     * @param stateWithLabels The new states will have the labels that this state has.
     * @param labelsInFormula The labels that occur in the property.
     * @return models::sparse::StateLabeling
     */
    models::sparse::StateLabeling extendStateLabeling(models::sparse::StateLabeling const& oldLabeling, uint64_t oldSize, uint64_t newSize,
                                                      uint64_t stateWithLabels, const std::set<std::string> labelsInFormula);
    /**
     * Sums duplicate transitions in a vector of MatrixEntries into one MatrixEntry.
     *
     * @param entries
     * @return std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>>
     */
    std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>> joinDuplicateTransitions(
        std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>> const& entries);
};

}  // namespace transformer
}  // namespace storm
