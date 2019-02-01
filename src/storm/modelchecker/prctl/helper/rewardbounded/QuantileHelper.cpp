#include "storm/modelchecker/prctl/helper/rewardbounded/QuantileHelper.h"

#include <set>
#include <vector>
#include <memory>
#include <boost/optional.hpp>
#include <storm/exceptions/NotSupportedException.h>

#include "storm/models/sparse/Mdp.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/MultiDimensionalRewardUnfolding.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
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
                    /* Todo: Current assumption:
                     * Subformula is always prob operator with bounded until
                     * Each bound variable occurs at most once (change this?)
                     * cost bounds can either be <= or >  (the epochs returned by the reward unfolding assume this. One could translate them, though)
                     * 'reasonable' quantile (e.g. not quantile(max B, Pmax>0.5 [F <=B G]) (we could also filter out these things early on)
                     */
                }

                enum class BoundTransformation {
                    None,
                    GreaterZero,
                    GreaterEqualZero,
                    LessEqualZero
                };
                std::shared_ptr<storm::logic::ProbabilityOperatorFormula> transformBoundedUntilOperator(storm::logic::ProbabilityOperatorFormula const& boundedUntilOperator, std::vector<BoundTransformation> const& transformations) {
                    auto const& origBoundedUntil = boundedUntilOperator.getSubformula().asBoundedUntilFormula();
                    STORM_LOG_ASSERT(transformations.size() == origBoundedUntil.getDimension(), "Tried to replace the bound of a dimension that is higher than the number of dimensions of the formula.");
                    std::vector<std::shared_ptr<storm::logic::Formula const>> leftSubformulas, rightSubformulas;
                    std::vector<boost::optional<storm::logic::TimeBound>> lowerBounds, upperBounds;
                    std::vector<storm::logic::TimeBoundReference> timeBoundReferences;

                    for (uint64_t dim = 0; dim < origBoundedUntil.getDimension(); ++dim) {
                        leftSubformulas.push_back(origBoundedUntil.getLeftSubformula(dim).asSharedPointer());
                        rightSubformulas.push_back(origBoundedUntil.getRightSubformula(dim).asSharedPointer());
                        timeBoundReferences.push_back(origBoundedUntil.getTimeBoundReference(dim));
                        if (transformations[dim] == BoundTransformation::None) {
                            if (origBoundedUntil.hasLowerBound()) {
                                lowerBounds.push_back(storm::logic::TimeBound(origBoundedUntil.isLowerBoundStrict(dim), origBoundedUntil.getLowerBound(dim)));
                            } else {
                                lowerBounds.push_back(boost::none);
                            }
                            if (origBoundedUntil.hasUpperBound()) {
                                upperBounds.push_back(storm::logic::TimeBound(origBoundedUntil.isUpperBoundStrict(dim), origBoundedUntil.getUpperBound(dim)));
                            } else {
                                upperBounds.push_back(boost::none);
                            }
                        } else {
                            // We need a zero expression in all other cases
                            storm::expressions::Expression zero;
                            if (origBoundedUntil.hasLowerBound(dim)) {
                                zero = origBoundedUntil.getLowerBound(dim).getManager().rational(0.0);
                            } else {
                                STORM_LOG_THROW(origBoundedUntil.hasUpperBound(dim), storm::exceptions::InvalidOperationException, "The given bounded until formula has no cost-bound for one dimension.");
                                zero = origBoundedUntil.getUpperBound(dim).getManager().rational(0.0);
                            }
                            if (transformations[dim] == BoundTransformation::LessEqualZero) {
                                lowerBounds.push_back(boost::none);
                                upperBounds.push_back(storm::logic::TimeBound(false, zero));
                            } else {
                                STORM_LOG_ASSERT(transformations[dim] == BoundTransformation::GreaterZero || transformations[dim] == BoundTransformation::GreaterEqualZero, "Unhandled bound transformation.");
                                lowerBounds.push_back(storm::logic::TimeBound(transformations[dim] == BoundTransformation::GreaterZero, zero));
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
                storm::storage::BitVector QuantileHelper<ModelType>::getOpenDimensions() const {
                    auto const& boundedUntil = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula();
                    storm::storage::BitVector res(getDimension());
                    for (uint64_t dim = 0; dim < getDimension(); ++dim) {
                        res.set(dim, boundedUntil.hasLowerBound(dim) ? boundedUntil.getLowerBound(dim).containsVariables() : boundedUntil.getUpperBound(dim).containsVariables());
                    }
                    return res;
                }

                template<typename ModelType>
                storm::solver::OptimizationDirection const& QuantileHelper<ModelType>::getOptimizationDirForDimension(uint64_t const& dim) const {
                    auto const& boundedUntil = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula();
                    storm::expressions::Variable const& dimVar = (boundedUntil.hasLowerBound() ? boundedUntil.getLowerBound(dim) : boundedUntil.getUpperBound(dim)).getBaseExpression().asVariableExpression().getVariable();
                    for (auto const& boundVar : quantileFormula.getBoundVariables()) {
                        if (boundVar.second == dimVar) {
                            return boundVar.first;
                        }
                    }
                    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "The bound variable '" << dimVar.getName() << "' is not specified within the quantile formula '" << quantileFormula << "'.");
                    return quantileFormula.getOptimizationDirection();
                }
                
                template<typename ModelType>
                storm::storage::BitVector QuantileHelper<ModelType>::getDimensionsForVariable(storm::expressions::Variable const& var) const {
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
                std::vector<std::vector<typename ModelType::ValueType>> QuantileHelper<ModelType>::computeQuantile(Environment const& env) {
                    std::vector<std::vector<ValueType>> result;
                    if (getOpenDimensions().getNumberOfSetBits() == 1) {
                        uint64_t dimension = *getOpenDimensions().begin();
                        auto const& boundedUntilOperator = quantileFormula.getSubformula().asProbabilityOperatorFormula();
                        
                        bool maxProbSatisfiesFormula = boundedUntilOperator.getBound().isSatisfied(computeExtremalValue(env, storm::storage::BitVector(getDimension(), false)));
                        bool minProbSatisfiesFormula = boundedUntilOperator.getBound().isSatisfied(computeExtremalValue(env, getOpenDimensions()));
                        if (maxProbSatisfiesFormula != minProbSatisfiesFormula) {
                            auto quantileRes = computeQuantileForDimension(env, dimension);
                            result = {{storm::utility::convertNumber<ValueType>(quantileRes.first) * quantileRes.second}};
                        } else if (maxProbSatisfiesFormula) {
                            // i.e., all bound values satisfy the formula
                            bool minimizingRewardBound = storm::solver::minimize(getOptimizationDirForDimension(dimension));
                            bool upperCostBound = boundedUntilOperator.getSubformula().asBoundedUntilFormula().hasUpperBound(dimension);
                            if (minimizingRewardBound) {
                                result = {{ (upperCostBound ? storm::utility::zero<ValueType>() : -storm::utility::one<ValueType>())}};
                            } else {
                                result = {{ storm::utility::infinity<ValueType>()}};
                            }
                        } else {
                            // i.e., no bound value satisfies the formula
                            result = {{}};
                        }
                    } else if (getOpenDimensions().getNumberOfSetBits() == 2) {
                    
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The quantile formula considers " << getOpenDimensions().getNumberOfSetBits() << " open dimensions. Only one- or two-dimensional quantiles are supported.");
                    }
                    return result;
                    /*
                                        return unboundedFormula->getBound().isSatisfied(numericResult);
                    
                    if (!checkUnboundedValue(env)) {
                        STORM_LOG_INFO("Bound is not satisfiable.");
                        result.emplace_back();
                        for (auto const& bv : quantileFormula.getBoundVariables()) {
                            result.front().push_back(storm::solver::minimize(bv.first) ? storm::utility::infinity<ValueType>() : -storm::utility::infinity<ValueType>());
                        }
                    } else {
                        std::vector<bool> limitProbCheckResults;
                        for (uint64_t dim = 0; dim < getDimension(); ++dim) {
                            storm::storage::BitVector dimAsBitVector(getDimension(), false);
                            dimAsBitVector.set(dim, true);
                            limitProbCheckResults.push_back(checkLimitProbability(env, dimAsBitVector));
                            auto quantileRes = computeQuantileForDimension(env, dim);
                            std::cout << "Quantile for dim " << dim << " is " << (storm::utility::convertNumber<ValueType>(quantileRes.first) * quantileRes.second) << std::endl;
                        }
                        
                        result = {{27}};
                    }
                    return result;*/
                }

                template<typename ModelType>
                std::pair<uint64_t, typename ModelType::ValueType> QuantileHelper<ModelType>::computeQuantileForDimension(Environment const& env, uint64_t dimension) const {
                    // We assume that their is one bound value that violates the quantile and one bound value that satisfies it.
                    
                    // 'Drop' all other open bounds
                    std::vector<BoundTransformation> bts(getDimension(), BoundTransformation::None);
                    for (auto const& d : getOpenDimensions()) {
                        if (d != dimension) {
                            bts[d] = BoundTransformation::GreaterEqualZero;
                        }
                    }
                    auto transformedFormula = transformBoundedUntilOperator(quantileFormula.getSubformula().asProbabilityOperatorFormula(), bts);
                    MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, transformedFormula);

                    bool upperCostBound = transformedFormula->getSubformula().asBoundedUntilFormula().hasUpperBound(dimension);
                   // bool lowerProbabilityBound = storm::logic::isLowerBound(transformedFormula->getBound().comparisonType);
                    bool minimizingRewardBound = storm::solver::minimize(getOptimizationDirForDimension(dimension));
                    
                    // initialize data that will be needed for each epoch
                    auto lowerBound = rewardUnfolding.getLowerObjectiveBound();
                    auto upperBound = rewardUnfolding.getUpperObjectiveBound();
                    std::vector<ValueType> x, b;
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> minMaxSolver;
                    
                    int64_t epochValue = 0; // -1 is actally allowed since we express >=0 as >-1
                    std::set<typename EpochManager::Epoch> exploredEpochs;
                    while (true) {
                        auto currEpoch = rewardUnfolding.getStartEpoch(true);
                        rewardUnfolding.getEpochManager().setDimensionOfEpoch(currEpoch, dimension, (uint64_t) epochValue);
                        auto epochOrder = rewardUnfolding.getEpochComputationOrder(currEpoch);
                        for (auto const& epoch : epochOrder) {
                            if (exploredEpochs.count(epoch) == 0) {
                                auto& epochModel = rewardUnfolding.setCurrentEpoch(epoch);
                                rewardUnfolding.setSolutionForCurrentEpoch(epochModel.analyzeSingleObjective(env, quantileFormula.getSubformula().asProbabilityOperatorFormula().getOptimalityType(), x, b, minMaxSolver, lowerBound, upperBound));
                                exploredEpochs.insert(epoch);
                            }
                        }
                        ValueType currValue = rewardUnfolding.getInitialStateResult(currEpoch);
                        std::cout << "Numeric result for epoch " << rewardUnfolding.getEpochManager().toString(currEpoch) << " is " << currValue << std::endl;
                        bool propertySatisfied = transformedFormula->getBound().isSatisfied(currValue);
                        // If the reward bound should be as small as possible, we should stop as soon as the property is satisfied.
                        // If the reward bound should be as large as possible, we should stop as soon as sthe property is violated and then go a step backwards
                        if (minimizingRewardBound && propertySatisfied) {
                            break;
                        } else if (!minimizingRewardBound && !propertySatisfied) {
                            STORM_LOG_ASSERT(epochValue > 0 || !upperCostBound, "The property does not seem to be satisfiable. This case should have been treated earlier.");
                            --epochValue;
                            break;
                        }
                        ++epochValue;
                    }
                    return {epochValue, rewardUnfolding.getDimension(dimension).scalingFactor};
                }
                
                template<typename ModelType>
                typename ModelType::ValueType QuantileHelper<ModelType>::computeExtremalValue(Environment const& env, storm::storage::BitVector const& minimizingDimensions) const {
                    // For maximizing in a dimension, we can simply 'drop' the bound by replacing it with >=0
                    // For minimizing an upper-bounded dimension, we can replace it with <=0
                    // For minimizing a lower-bounded dimension, the lower bound needs to approach infinity.
                    // To compute this limit probability, we only consider epoch steps that lie in an end component and check for the bound >0 instead.
                    // Notice, however, that this approach fails if we try to minimize for a lower and an upper bounded dimension
                    
                    std::vector<BoundTransformation> bts(getDimension(), BoundTransformation::None);
                    storm::storage::BitVector minimizingLowerBoundedDimensions(getDimension(), false);

                    for (auto const& d : getOpenDimensions()) {
                        if (minimizingDimensions.get(d)) {
                            bool upperCostBound = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula().hasUpperBound(d);
                            if (upperCostBound) {
                                bts[d] = BoundTransformation::LessEqualZero;
                                STORM_LOG_ASSERT(std::find(bts.begin(), bts.end(), BoundTransformation::GreaterZero) == bts.end(), "Unable to compute extremal value for minimizing lower and upper bounded dimensions.");
                            } else {
                                bts[d] = BoundTransformation::GreaterZero;
                                minimizingLowerBoundedDimensions.set(d, true);
                                STORM_LOG_ASSERT(std::find(bts.begin(), bts.end(), BoundTransformation::LessEqualZero) == bts.end(), "Unable to compute extremal value for minimizing lower and upper bounded dimensions.");
                            }
                        } else {
                            bts[d] = BoundTransformation::GreaterEqualZero;
                        }
                    }

                    auto transformedFormula = transformBoundedUntilOperator(quantileFormula.getSubformula().asProbabilityOperatorFormula(), bts);
                    
                    storm::storage::BitVector nonMecChoices;
                    if (!minimizingLowerBoundedDimensions.empty()) {
                        // Get the choices that do not lie on a mec
                        nonMecChoices.resize(model.getNumberOfChoices(), true);
                        auto mecDecomposition = storm::storage::MaximalEndComponentDecomposition<ValueType>(model);
                        for (auto const& mec : mecDecomposition) {
                            for (auto const& stateChoicesPair : mec) {
                                for (auto const& choice : stateChoicesPair.second) {
                                    nonMecChoices.set(choice, false);
                                }
                            }
                        }
                    }

                    MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, transformedFormula, [&](std::vector<EpochManager::Epoch>& epochSteps, EpochManager const& epochManager) {
                        if (!minimizingLowerBoundedDimensions.empty()) {
                            for (auto const& choice : nonMecChoices) {
                                for (auto const& dim : minimizingLowerBoundedDimensions) {
                                    epochManager.setDimensionOfEpoch(epochSteps[choice], dim, 0);
                                }
                            }
                        }
                    });

                    // Get lower and upper bounds for the solution.
                    auto lowerBound = rewardUnfolding.getLowerObjectiveBound();
                    auto upperBound = rewardUnfolding.getUpperObjectiveBound();

                    // Initialize epoch models
                    auto initEpoch = rewardUnfolding.getStartEpoch();
                    auto epochOrder = rewardUnfolding.getEpochComputationOrder(initEpoch);
                    // initialize data that will be needed for each epoch
                    std::vector<ValueType> x, b;
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> minMaxSolver;

                    for (auto const& epoch : epochOrder) {
                        auto& epochModel = rewardUnfolding.setCurrentEpoch(epoch);
                        rewardUnfolding.setSolutionForCurrentEpoch(epochModel.analyzeSingleObjective(env, quantileFormula.getSubformula().asProbabilityOperatorFormula().getOptimalityType(), x, b, minMaxSolver, lowerBound, upperBound));
                    }

                    ValueType numericResult = rewardUnfolding.getInitialStateResult(initEpoch);
                    STORM_LOG_TRACE("Extremal probability for minimizing dimensions " << minimizingDimensions << " is " << numericResult << ".");
                    return transformedFormula->getBound().isSatisfied(numericResult);
                }

                template class QuantileHelper<storm::models::sparse::Mdp<double>>;
                template class QuantileHelper<storm::models::sparse::Mdp<storm::RationalNumber>>;

            }
        }
    }
}
