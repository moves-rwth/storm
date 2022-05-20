#include "storm-pars/transformer/ParametricTransformer.h"
#include "storm-pars/utility/parametric.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/sparse/ModelComponents.h"

namespace storm {
namespace transformer {
/**
 * We want to transform a state s with parametric reward r to 3 states, s0, s1 and s2.
 * s1 and s2 have as successor states the successor states of s
 * the reward of s2 is always 0 (rew2 = 0)
 * f1 and f2 represent the prob from s0 to s1/s2
 * val0 is the valuation of the reward at p=0 (same for val1, p=1)
 *  rew s     | val0 | val1      | constant | rew0  | rew1 | f1    | f2 |
 *  p * a + b | b    | a + b     | b        | b     | a    | p     | 1 - p
 *  b - p * a | b    | b - a     | b        | b - a | a    | 1 - p | p
 */
std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> makeRewardsConstant(storm::models::sparse::Dtmc<storm::RationalFunction> const& pMC) {
    STORM_LOG_THROW(pMC.hasUniqueRewardModel(), storm::exceptions::IllegalArgumentException, "pMC needs to have an unique reward model");
    storm::storage::sparse::ModelComponents<storm::RationalFunction> modelComponents;

    uint64_t nrStates = pMC.getTransitionMatrix().getColumnCount();
    uint_fast64_t nrOfNewStates = 0;
    auto rewardModel = pMC.getUniqueRewardModel();
    STORM_LOG_THROW(rewardModel.hasStateActionRewards(), storm::exceptions::NotImplementedException,
                    "Making rewards constant not implemented for state action rewards");
    STORM_LOG_ASSERT(rewardModel.hasStateRewards(), "Expecting model to have either state rewards");

    for (uint64_t state = 0; state < nrStates; ++state) {
        if (rewardModel.hasStateRewards() && !rewardModel.getStateReward(state).isConstant()) {
            nrOfNewStates = nrOfNewStates + 2;
        }
    }

    storm::storage::SparseMatrixBuilder<storm::RationalFunction> smb(nrStates + nrOfNewStates, nrStates + nrOfNewStates, 0, true);

    uint64_t offset = 0;
    std::vector<storm::RationalFunction> stateRewards(nrStates + nrOfNewStates, storm::RationalFunction(0));
    storm::models::sparse::StateLabeling stateLabeling(nrStates + nrOfNewStates);
    STORM_LOG_THROW(!pMC.getOptionalStateValuations(), storm::exceptions::NotImplementedException,
                    "Keeping rewards constant while having state valuations is not implemented");
    for (uint64_t state = 0; state < nrStates; ++state) {
        storm::RationalFunction reward = storm::RationalFunction(0);
        reward = rewardModel.getStateReward(state);
        for (auto& label : pMC.getStateLabeling().getLabelsOfState(state)) {
            if (!stateLabeling.containsLabel(label)) {
                stateLabeling.addLabel(label);
            }
            stateLabeling.addLabelToState(label, state + offset);
        }

        if (!reward.isConstant()) {
            auto vars = reward.gatherVariables();
            storm::RationalFunction::CoeffType b = reward.constantPart();
            STORM_LOG_THROW(vars.size() == 1, storm::exceptions::NotImplementedException,
                            "Making rewards constant for rewards with more than 1 parameter not implemented");
            STORM_LOG_THROW(storm::utility::parametric::isLinear(reward), storm::exceptions::NotImplementedException,
                            "Expecting rewards to be constant or linear");
            std::map<RationalFunctionVariable, RationalFunctionCoefficient> val0, val1;
            val0[*vars.begin()] = 0;
            val1[*vars.begin()] = 1;
            storm::RationalFunction::CoeffType value0 = reward.evaluate(val0);
            storm::RationalFunction::CoeffType value1 = reward.evaluate(val1);
            if (value1 - b >= 0) {
                storm::RationalFunction::CoeffType a = value1 - b;
                // Reward is b + p * a
                stateRewards[state + offset] = storm::utility::convertNumber<storm::RationalFunction>(b);
                stateRewards[state + offset + 1] = storm::utility::convertNumber<storm::RationalFunction>(a);
                stateRewards[state + offset + 2] = storm::utility::zero<storm::RationalFunction>();
                // probs are p and 1-p
                storm::RationalFunction funcP = (reward - b) / a;
                smb.addNextValue(state + offset, state + 1, funcP);
                smb.addNextValue(state + offset, state + 2, storm::utility::one<storm::RationalFunction>() - funcP);
            } else {
                // Reward is b - p * a
                storm::RationalFunction::CoeffType a = b - value1;
                stateRewards[state + offset] = storm::utility::convertNumber<storm::RationalFunction>(value1);
                stateRewards[state + offset + 1] = storm::utility::convertNumber<storm::RationalFunction>(a);
                stateRewards[state + offset + 2] = storm::utility::zero<storm::RationalFunction>();
                storm::RationalFunction funcP = (-(reward - b)) / a;
                smb.addNextValue(state + offset, state + 1, storm::utility::one<storm::RationalFunction>() - funcP);
                smb.addNextValue(state + offset, state + 2, funcP);
            }
            auto row = pMC.getTransitionMatrix().getRow(state);

            // To reduce the number of transitions, we let s1 have only one outgoing transition to s2.
            // The reward at s2 is 0, and it goes to the successors of our original state.
            for (auto const& entry : row) {
                smb.addNextValue(state + offset + 1, state + offset + 2, storm::utility::one<storm::RationalFunction>());
            }
            for (auto const& entry : row) {
                smb.addNextValue(state + offset + 2, entry.getColumn(), entry.getValue());
            }
            offset += 2;
        } else {
            stateRewards[state + offset] = reward;
            for (auto const& entry : pMC.getTransitionMatrix().getRow(state)) {
                smb.addNextValue(state + offset, entry.getColumn(), entry.getValue());
            }
        }
    }
    modelComponents.transitionMatrix = smb.build();
    modelComponents.rewardModels.emplace(pMC.getUniqueRewardModelName(), std::move(stateRewards));

    modelComponents.stateLabeling = std::move(stateLabeling);
    modelComponents.stateValuations = pMC.getOptionalStateValuations();
    return std::make_shared<storm::models::sparse::Dtmc<storm::RationalFunction>>(modelComponents);
}
}  // namespace transformer
}  // namespace storm