#include "storm-pars/transformer/BinaryDtmcTransformer.h"
#include <carl/formula/Constraint.h>
#include <cstdint>
#include <queue>

#include "adapters/RationalFunctionAdapter.h"
#include "storage/SparseMatrix.h"
#include "storm-pars/utility/parametric.h"
#include "storm/storage/sparse/ModelComponents.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "utility/constants.h"
#include "utility/logging.h"

namespace storm {
    namespace transformer {

        BinaryDtmcTransformer::BinaryDtmcTransformer() {
            // Intentionally left empty
        }
        
        std::shared_ptr<storm::models::sparse::Dtmc<RationalFunction>> BinaryDtmcTransformer::transform(storm::models::sparse::Dtmc<RationalFunction> const& pomdp, bool transformSimple, bool keepStateValuations) const {
            auto data = transformTransitions(pomdp, transformSimple);
            storm::storage::sparse::ModelComponents<RationalFunction> components;
            components.stateLabeling = transformStateLabeling(pomdp, data);
            for (auto const& rewModel : pomdp.getRewardModels()) {
                components.rewardModels.emplace(rewModel.first, transformRewardModel(pomdp, rewModel.second, data));
            }
            components.transitionMatrix = std::move(data.simpleMatrix);
            if (keepStateValuations && pomdp.hasStateValuations()) {
                components.stateValuations = pomdp.getStateValuations().blowup(data.simpleStateToOriginalState);
            }
            
            return std::make_shared<storm::models::sparse::Dtmc<RationalFunction>>(std::move(components.transitionMatrix), std::move(components.stateLabeling), std::move(components.rewardModels));
        }
        
        struct StateWithRow {
            uint_fast64_t state;
            std::vector<storage::MatrixEntry<uint_fast64_t, RationalFunction>> row;
        };
        
        typename BinaryDtmcTransformer::TransformationData BinaryDtmcTransformer::transformTransitions(storm::models::sparse::Dtmc<RationalFunction> const& pomdp, bool transformSimple) const {
            auto const& matrix = pomdp.getTransitionMatrix();
           
            
            // Initialize a FIFO Queue that stores the start and the end of each row
            std::queue<StateWithRow> queue;
            for (uint64_t state = 0; state < matrix.getRowCount(); ++state) {
                std::vector<storage::MatrixEntry<uint_fast64_t, RationalFunction>> diyRow;
                for (auto const& entry : matrix.getRow(state)) {
                    diyRow.push_back(entry);
                }
                queue.emplace(StateWithRow{state, diyRow});
            }
            
            storm::storage::SparseMatrixBuilder<RationalFunction> builder;
            uint64_t currRow = 0;
            uint64_t currAuxState = queue.size();
            std::vector<uint64_t> origStates;

            while (!queue.empty()) {
                auto stateWithRow = std::move(queue.front());
                queue.pop();

                std::set<RationalFunctionVariable> variablesInRow;
                
                for (auto const& entry : stateWithRow.row) {
                    for (auto const& variable : entry.getValue().gatherVariables()) {
                        variablesInRow.emplace(variable);
                    }
                }
                
                if (variablesInRow.size() == 0) {
                    // Insert the row directly
                    for (auto const& entry : stateWithRow.row) {
                        builder.addNextValue(currRow, entry.getColumn(), entry.getValue());
                    }
                    ++currRow;
                } else if (variablesInRow.size() == 1) {
                    auto parameter = *variablesInRow.begin();
                    auto parameterPol = RawPolynomial(parameter);
                    auto oneMinusParameter = RawPolynomial(1) - parameterPol;
                    
                    std::vector<storage::MatrixEntry<uint_fast64_t, RationalFunction>> outgoing;
                    // p * .. state
                    std::vector<storage::MatrixEntry<uint_fast64_t, RationalFunction>> newStateLeft;
                    // (1-p) * .. state
                    std::vector<storage::MatrixEntry<uint_fast64_t, RationalFunction>> newStateRight;

                    RationalFunction sumOfLeftBranch;
                    RationalFunction sumOfRightBranch;
                    
                    for (auto const& entry : stateWithRow.row) {
                        if (entry.getValue().isConstant()) {
                            outgoing.push_back(entry);
                        }
                        auto nominator = entry.getValue().nominator();
                        auto denominator = entry.getValue().denominator();
                        auto byP = RawPolynomial(nominator).divideBy(parameterPol);
                        if (byP.remainder.isZero()) {
                            auto probability = RationalFunction(carl::makePolynomial<Polynomial>(byP.quotient), denominator);
                            newStateLeft.push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(entry.getColumn(), probability));
                            sumOfLeftBranch += probability;
                            continue;
                        }
                        auto byOneMinusP = RawPolynomial(nominator).divideBy(oneMinusParameter);
                        if (byOneMinusP.remainder.isZero()) {
                            auto probability = RationalFunction(carl::makePolynomial<Polynomial>(byOneMinusP.quotient), denominator);
                            newStateRight.push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(entry.getColumn(), probability));
                            sumOfRightBranch += probability;
                            continue;
                        }
                        STORM_LOG_ERROR("Invalid transition!");
                    }
                    for (auto& entry : newStateLeft) {
                        entry.setValue(entry.getValue() / sumOfLeftBranch);
                    }
                    for (auto& entry : newStateRight) {
                        entry.setValue(entry.getValue() / sumOfRightBranch);
                    }
                    


                    queue.push(StateWithRow{currAuxState, newStateLeft});
                    outgoing.push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(currAuxState, (sumOfLeftBranch) * RationalFunction(carl::makePolynomial<Polynomial>(parameter))));
                    ++currAuxState;
                    queue.push(StateWithRow{currAuxState, newStateRight});
                    outgoing.push_back(storage::MatrixEntry<uint_fast64_t, RationalFunction>(currAuxState, (sumOfRightBranch) * (utility::one<RationalFunction>() - RationalFunction(carl::makePolynomial<Polynomial>(parameter)))));
                    ++currAuxState;

                    for (auto const& entry : outgoing) {
                        builder.addNextValue(currRow, entry.getColumn(), entry.getValue());
                    }
                    ++currRow;
                } else {
                    STORM_LOG_ERROR("More than one variable in row " << currRow << "!");
                }
                origStates.push_back(stateWithRow.state);
            }
            TransformationData result;
            result.simpleMatrix = builder.build(currRow, currAuxState, currAuxState);
            result.simpleStateToOriginalState = std::move(origStates);
            return result;
        }
        
        
        storm::models::sparse::StateLabeling BinaryDtmcTransformer::transformStateLabeling(storm::models::sparse::Dtmc<RationalFunction> const& pomdp, TransformationData const& data) const {
            storm::models::sparse::StateLabeling labeling(data.simpleMatrix.getRowCount());
            for (auto const& labelName : pomdp.getStateLabeling().getLabels()) {
                storm::storage::BitVector newStates = pomdp.getStateLabeling().getStates(labelName);
                newStates.resize(data.simpleMatrix.getRowCount(), false);
                if (labelName != "init") {
                    for (uint64_t newState = pomdp.getNumberOfStates();
                         newState < data.simpleMatrix.getRowCount(); ++newState) {
                        newStates.set(newState, newStates[data.simpleStateToOriginalState[newState]]);
                    }
                }
                labeling.addLabel(labelName, std::move(newStates));

            }
            return labeling;
        }
        
        storm::models::sparse::StandardRewardModel<RationalFunction> BinaryDtmcTransformer::transformRewardModel(storm::models::sparse::Dtmc<RationalFunction> const& pomdp, storm::models::sparse::StandardRewardModel<RationalFunction> const& rewardModel, TransformationData const& data) const {
            boost::optional<std::vector<RationalFunction>> stateRewards, actionRewards;
            STORM_LOG_THROW(rewardModel.hasStateActionRewards(), storm::exceptions::NotSupportedException, "Only state rewards supported.");
            if (rewardModel.hasStateRewards()) {
                stateRewards = rewardModel.getStateRewardVector();
                stateRewards.get().resize(data.simpleMatrix.getRowCount(), storm::utility::zero<RationalFunction>());
            }
            return storm::models::sparse::StandardRewardModel<RationalFunction>(std::move(stateRewards), std::move(actionRewards));
        }
        
        
    //    template<typename RationalFunction>
    //     boost::optional<std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::pair<typename storm::utility::parametric::CoefficientType<RationalFunction>::type, typename storm::utility::parametric::CoefficientType<RationalFunction>::type>>> BinaryDtmcTransformer<RationalFunction>::BinaryDtmcTransformer::tryDecomposing(RawPolynomial polynomial, bool firstIteration) {
    //         auto parameterPol = RawPolynomial(parameter);
    //         auto oneMinusParameter = RawPolynomial(1) - parameterPol;
    //         if (polynomial.isConstant()) {
    //             return std::make_pair(std::make_pair((uint_fast64_t) 0, (uint_fast64_t) 0), std::make_pair(utility::convertNumber<CoefficientType>(polynomial.constantPart()), utility::zero<CoefficientType>()));
    //         }
    //         auto byOneMinusP = polynomial.divideBy(oneMinusParameter);
    //         if (byOneMinusP.remainder.isZero()) {
    //             auto recursiveResult = tryDecomposing(byOneMinusP.quotient, false);
    //             if (recursiveResult) {
    //                 return std::make_pair(std::make_pair(recursiveResult->first.first, recursiveResult->first.second + 1), recursiveResult->second);
    //             }
    //         }
    //         auto byP = polynomial.divideBy(parameterPol);
    //         if (byP.remainder.isZero()) {
    //             auto recursiveResult = tryDecomposing(byP.quotient, false);
    //             if (recursiveResult) {
    //                 return std::make_pair(std::make_pair(recursiveResult->first.first + 1, recursiveResult->first.second), recursiveResult->second);
    //             }
    //         }
    //         if (!firstIteration) {
    //             return boost::none;
    //         }
    //         if (byOneMinusP.remainder.isConstant()) {
    //             auto rem1 = utility::convertNumber<CoefficientType>(byOneMinusP.remainder.constantPart());
    //             auto recursiveResult = tryDecomposing(byOneMinusP.quotient, false);
    //             if (recursiveResult) {
    //                 STORM_LOG_ASSERT(recursiveResult->second.second == 0, "");
    //                 return std::make_pair(std::make_pair(recursiveResult->first.first, recursiveResult->first.second + 1), 
    //                     std::pair<CoefficientType, CoefficientType>(recursiveResult->second.first, rem1));
    //             }
    //         }
    //         if (byP.remainder.isConstant()) {
    //             auto rem2 = utility::convertNumber<CoefficientType>(byP.remainder.constantPart());
    //             auto recursiveResult = tryDecomposing(byP.quotient, false);
    //             if (recursiveResult) {
    //                 STORM_LOG_ASSERT(recursiveResult->second.second == 0, "");
    //                 return std::make_pair(std::make_pair(recursiveResult->first.first + 1, recursiveResult->first.second), 
    //                     std::pair<CoefficientType, CoefficientType>(recursiveResult->second.first, rem2));
    //             }
    //         }
    //         return boost::none;
    //     }
    }
}