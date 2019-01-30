#include "storm/modelchecker/prctl/helper/rewardbounded/QuantileHelper.h"

#include <set>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm/models/sparse/Mdp.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/MultiDimensionalRewardUnfolding.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/BitVector.h"
#include "storm/utility/vector.h"

#include "storm/logic/ProbabilityOperatorFormula.h"
#include "storm/logic/BoundedUntilFormula.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            namespace rewardbounded {

                template<typename ModelType>
                QuantileHelper<ModelType>::QuantileHelper(ModelType const& model, storm::logic::QuantileFormula const& quantileFormula) : model(model), quantileFormula(quantileFormula) {
                    // Intentionally left empty
                }

                std::shared_ptr<storm::logic::ProbabilityOperatorFormula> replaceBoundsByGreaterEqZero(storm::logic::ProbabilityOperatorFormula const& boundedUntilOperator, storm::storage::BitVector const& dimensionsToReplace) {
                    auto const& origBoundedUntil = boundedUntilOperator.getSubformula().asBoundedUntilFormula();
                    STORM_LOG_ASSERT(dimensionsToReplace.size() == origBoundedUntil.getDimension(), "Tried to replace the bound of a dimension that is higher than the number of dimensions of the formula.");
                    std::vector<std::shared_ptr<storm::logic::Formula const>> leftSubformulas, rightSubformulas;
                    std::vector<boost::optional<storm::logic::TimeBound>> lowerBounds, upperBounds;
                    std::vector<storm::logic::TimeBoundReference> timeBoundReferences;

                    for (uint64_t dim = 0; dim < origBoundedUntil.getDimension(); ++dim) {
                        leftSubformulas.push_back(origBoundedUntil.getLeftSubformula(dim).asSharedPointer());
                        rightSubformulas.push_back(origBoundedUntil.getRightSubformula(dim).asSharedPointer());
                        timeBoundReferences.push_back(origBoundedUntil.getTimeBoundReference(dim));
                        if (dimensionsToReplace.get(dim)) {
                            storm::expressions::Expression zero;
                            if (origBoundedUntil.hasLowerBound(dim)) {
                                zero = origBoundedUntil.getLowerBound(dim).getManager().rational(0.0);
                            } else {
                                STORM_LOG_THROW(origBoundedUntil.hasUpperBound(dim), storm::exceptions::InvalidOperationException, "The given bounded until formula has no cost-bound for one dimension.");
                                zero = origBoundedUntil.getUpperBound(dim).getManager().rational(0.0);
                            }
                            lowerBounds.push_back(storm::logic::TimeBound(false, zero));
                            upperBounds.push_back(boost::none);
                        } else {
                            if (origBoundedUntil.hasLowerBound()) {
                                lowerBounds.push_back(storm::logic::TimeBound(origBoundedUntil.isLowerBoundStrict(dim), origBoundedUntil.getLowerBound(dim)));
                            } else {
                                lowerBounds.push_back(boost::none);
                            }
                            if (origBoundedUntil.hasUpperBound()) {
                                lowerBounds.push_back(storm::logic::TimeBound(origBoundedUntil.isUpperBoundStrict(dim), origBoundedUntil.getUpperBound(dim)));
                            } else {
                                upperBounds.push_back(boost::none);
                            }
                        }
                    }
                    auto newBoundedUntil = std::make_shared<storm::logic::BoundedUntilFormula>(leftSubformulas, rightSubformulas, lowerBounds, upperBounds, timeBoundReferences);
                    return std::make_shared<storm::logic::ProbabilityOperatorFormula>(newBoundedUntil, boundedUntilOperator.getOperatorInformation());
                }



                template<typename ModelType>
                uint64_t QuantileHelper<ModelType>::getDimension() const {
                    return quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula().getDimension();
                }

                template<typename ModelType>
                storm::storage::BitVector QuantileHelper<ModelType>::getDimensionsForVariable(storm::expressions::Variable const& var) {
                    auto const& boundedUntil = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula();
                    storm::storage::BitVector result(boundedUntil.getDimension(), false);
                    for (uint64_t dim = 0; dim < boundedUntil.getDimension(); ++dim) {
                        if (boundedUntil.hasLowerBound(dim) && boundedUntil.getLowerBound(dim).isVariable() && boundedUntil.getLowerBound(dim).getBaseExpression().asVariableExpression().getVariable() == var) {
                            result.set(dim, true);
                        }
                        if (boundedUntil.hasUpperBound(dim) && boundedUntil.getUpperBound(dim).isVariable() && boundedUntil.getUpperBound(dim).getBaseExpression().asVariableExpression().getVariable() == var) {
                            result.set(dim, true);
                        }
                    }
                    return result;
                }

                template<typename ModelType>
                std::vector<std::vector<typename ModelType::ValueType>> QuantileHelper<ModelType>::computeMultiDimensionalQuantile(Environment const& env) {
                    std::vector<std::vector<ValueType>> result;
                    if (!computeUnboundedValue(env)) {
                        STORM_LOG_INFO("Bound is not satisfiable.");
                        result.emplace_back();
                        for (auto const& bv : quantileFormula.getBoundVariables()) {
                            result.front().push_back(storm::solver::minimize(bv.first) ? storm::utility::infinity<ValueType>() : -storm::utility::infinity<ValueType>());
                        }
                    } else {
                        result = {{27}};
                    }
                    return result;
                }

                template<typename ModelType>
                bool QuantileHelper<ModelType>::computeUnboundedValue(Environment const& env) {
                    auto unboundedFormula = replaceBoundsByGreaterEqZero(quantileFormula.getSubformula().asProbabilityOperatorFormula(), storm::storage::BitVector(getDimension(), true));
                    MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, unboundedFormula);

                    // Get lower and upper bounds for the solution.
                    auto lowerBound = rewardUnfolding.getLowerObjectiveBound();
                    auto upperBound = rewardUnfolding.getUpperObjectiveBound();

                    // Initialize epoch models
                    auto initEpoch = rewardUnfolding.getStartEpoch();
                    auto epochOrder = rewardUnfolding.getEpochComputationOrder(initEpoch);
                    STORM_LOG_ASSERT(epochOrder.size() == 1, "unexpected epoch order size.");
                    // initialize data that will be needed for each epoch
                    std::vector<ValueType> x, b;
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> minMaxSolver;

                    auto& epochModel = rewardUnfolding.setCurrentEpoch(initEpoch);
                    rewardUnfolding.setSolutionForCurrentEpoch(epochModel.analyzeSingleObjective(env, quantileFormula.getSubformula().asProbabilityOperatorFormula().getOptimalityType(), x, b, minMaxSolver, lowerBound, upperBound));

                    ValueType numericResult = rewardUnfolding.getInitialStateResult(initEpoch);
                    std::cout << "Numeric result is " << numericResult;
                    return unboundedFormula->getBound().isSatisfied(numericResult);
                }


                template class QuantileHelper<storm::models::sparse::Mdp<double>>;
                template class QuantileHelper<storm::models::sparse::Mdp<storm::RationalNumber>>;

            }
        }
    }
}
