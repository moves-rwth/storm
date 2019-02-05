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
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/utility/vector.h"

#include "storm/logic/ProbabilityOperatorFormula.h"
#include "storm/logic/BoundedUntilFormula.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            namespace rewardbounded {

                template<typename ModelType>
                QuantileHelper<ModelType>::QuantileHelper(ModelType const& model, storm::logic::QuantileFormula const& quantileFormula) : model(model), quantileFormula(quantileFormula) {
                    // Intentionally left empty
                    STORM_LOG_THROW(quantileFormula.getSubformula().isProbabilityOperatorFormula(), storm::exceptions::NotSupportedException, "Quantile formula needs probability operator inside. The formula " << quantileFormula << " is not supported.");
                    auto const& probOpFormula = quantileFormula.getSubformula().asProbabilityOperatorFormula();
                    STORM_LOG_THROW(probOpFormula.getBound().comparisonType == storm::logic::ComparisonType::Greater || probOpFormula.getBound().comparisonType == storm::logic::ComparisonType::LessEqual, storm::exceptions::NotSupportedException, "Probability operator inside quantile formula needs to have bound > or <=.");
                    STORM_LOG_THROW(probOpFormula.getSubformula().isBoundedUntilFormula(), storm::exceptions::NotSupportedException, "Quantile formula needs bounded until probability operator formula as subformula. The formula " << quantileFormula << " is not supported.");
                    auto const& boundedUntilFormula = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula();
                    
                    // Only > and <= are supported for upper bounds. This is to make sure that Pr>0.7 [F "goal"] holds iff Pr>0.7 [F<=B "goal"] holds for some B.
                    // Only >= and < are supported for lower bounds. (EC construction..)
                    // TODO
                    
                    /* Todo: Current assumptions:
                     * Subformula is always prob operator with bounded until
                     * Each bound variable occurs at most once (change this?)
                     * cost bounds are assumed to be always non-strict (the epochs returned by the reward unfolding assume strict lower and non-strict upper cost bounds, though...)
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
                            if (storm::solver::minimize(getOptimizationDirForDimension(dimension))) {
                                result = {{ storm::utility::zero<ValueType>() }};
                            } else {
                                result = {{ storm::utility::infinity<ValueType>()}};
                            }
                        } else {
                            // i.e., no bound value satisfies the formula
                            result = {{}};
                        }
                    } else if (getOpenDimensions().getNumberOfSetBits() == 2) {
                        result = computeTwoDimensionalQuantile(env);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The quantile formula considers " << getOpenDimensions().getNumberOfSetBits() << " open dimensions. Only one- or two-dimensional quantiles are supported.");
                    }
                    return result;
                }

                template<typename ValueType>
                void filterDominatedPoints(std::vector<std::vector<ValueType>>& points, std::vector<storm::solver::OptimizationDirection> const& dirs) {
                    std::vector<std::vector<ValueType>> result;
                    // Note: this is slow and not inplace but also most likely not performance critical
                    for (auto const& p1 : points) {
                        bool p1Dominated = false;
                        for (auto const& p2 : points) {
                            assert(p1.size() == p2.size());
                            bool p2DominatesP1 = false;
                            for (uint64_t i = 0; i < dirs.size(); ++i) {
                                if (storm::solver::minimize(dirs[i]) ? p2[i] <= p1[i] : p2[i] >= p1[i]) {
                                    if (p2[i] != p1[i]) {
                                        p2DominatesP1 = true;
                                    }
                                } else {
                                    p2DominatesP1 = false;
                                    break;
                                }
                            }
                            if (p2DominatesP1) {
                                p1Dominated = true;
                                break;
                            }
                        }
                        if (!p1Dominated) {
                            result.push_back(p1);
                        }
                    }
                    points = std::move(result);
                }
                
                template<typename ModelType>
                std::vector<std::vector<typename ModelType::ValueType>> QuantileHelper<ModelType>::computeTwoDimensionalQuantile(Environment const& env) {
                    std::vector<std::vector<ValueType>> result;

                    auto const& probOpFormula = quantileFormula.getSubformula().asProbabilityOperatorFormula();
                    storm::logic::ComparisonType probabilityBound = probOpFormula.getBound().comparisonType;
                    auto const& boundedUntilFormula = probOpFormula.getSubformula().asBoundedUntilFormula();
                    std::vector<storm::storage::BitVector> dimensionsAsBitVector;
                    std::vector<uint64_t> dimensions;
                    std::vector<storm::solver::OptimizationDirection> optimizationDirections;
                    std::vector<bool> lowerCostBounds;
                    for (auto const& dirVar : quantileFormula.getBoundVariables()) {
                        dimensionsAsBitVector.push_back(getDimensionsForVariable(dirVar.second));
                        STORM_LOG_THROW(dimensionsAsBitVector.back().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "There is not exactly one reward bound referring to quantile variable '" << dirVar.second.getName() << "'.");
                        dimensions.push_back(*dimensionsAsBitVector.back().begin());
                        lowerCostBounds.push_back(boundedUntilFormula.hasLowerBound(dimensions.back()));
                        optimizationDirections.push_back(dirVar.first);
                    }
                    STORM_LOG_ASSERT(dimensions.size() == 2, "Expected to have exactly two open dimensions.");
                    if (optimizationDirections[0] == optimizationDirections[1]) {
                        if (lowerCostBounds[0] == lowerCostBounds[1]) {
                            // TODO: Assert that we have a reasonable probability bound. STORM_LOG_THROW(storm::solver::minimize(optimizationDirections[0])
                            
                            bool maxmaxProbSatisfiesFormula = probOpFormula.getBound().isSatisfied(computeExtremalValue(env, storm::storage::BitVector(getDimension(), false)));
                            bool minminProbSatisfiesFormula = probOpFormula.getBound().isSatisfied(computeExtremalValue(env, dimensionsAsBitVector[0] | dimensionsAsBitVector[1]));
                            if (maxmaxProbSatisfiesFormula != minminProbSatisfiesFormula) {
                                std::vector<std::pair<int64_t, typename ModelType::ValueType>> singleQuantileValues;
                                std::vector<std::vector<int64_t>> resultPoints;
                                const int64_t infinity = std::numeric_limits<int64_t>().max(); // use this value to represent infinity in a result point
                                for (uint64_t i = 0; i < 2; ++i) {
                                    // find out whether the bounds B_i = 0 and B_1-i = infinity satisfy the formula
                                    uint64_t indexToMinimizeProb = lowerCostBounds[i] ? (1-i) : i;
                                    bool zeroInfSatisfiesFormula = probOpFormula.getBound().isSatisfied(computeExtremalValue(env, dimensionsAsBitVector[indexToMinimizeProb]));
                                    std::cout << "Formula sat is " << zeroInfSatisfiesFormula << " and lower bound is " << storm::logic::isLowerBound(probabilityBound) << std::endl;
                                    if (zeroInfSatisfiesFormula == storm::solver::minimize(optimizationDirections[0])) {
                                        // There is bound value b such that the point B_i=0 and B_1-i = b is part of the result
                                        singleQuantileValues.emplace_back(0, storm::utility::zero<ValueType>());
                                    } else {
                                        // Compute quantile where 1-i approaches infinity
                                        std::cout << "Computing quantile for single dimension " << dimensions[i] << std::endl;
                                        singleQuantileValues.push_back(computeQuantileForDimension(env, dimensions[i]));
                                        std::cout << ".. Result is " << singleQuantileValues.back().first << std::endl;
                                        // When maximizing bounds, the computed quantile value is sat for all values of the other bound.
                                        if (!storm::solver::minimize(optimizationDirections[i])) {
                                            std::vector<int64_t> newResultPoint(2);
                                            newResultPoint[i] = singleQuantileValues.back().first;
                                            newResultPoint[1-i] = infinity;
                                            resultPoints.push_back(newResultPoint);
                                            // Increase the computed value so that there is at least one unsat assignment of bounds with B[i] = singleQuantileValues[i]
                                            ++singleQuantileValues.back().first;
                                        }
                                    }
                                    // Decrease the value for lower cost bounds to convert >= to >
                                    if (lowerCostBounds[i]) {
                                        --singleQuantileValues[i].first;
                                    }
                                }
                                std::cout << "Found starting point. Beginning 2D exploration" << std::endl;
                                
                                MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, transformBoundedUntilOperator(quantileFormula.getSubformula().asProbabilityOperatorFormula(), std::vector<BoundTransformation>(getDimension(), BoundTransformation::None)));
                        
                                // initialize data that will be needed for each epoch
                                auto lowerBound = rewardUnfolding.getLowerObjectiveBound();
                                auto upperBound = rewardUnfolding.getUpperObjectiveBound();
                                std::vector<ValueType> x, b;
                                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> minMaxSolver;
                    
                                std::vector<int64_t> epochValues = {(int64_t) singleQuantileValues[0].first, (int64_t) singleQuantileValues[1].first}; // signed int is needed to allow lower bounds >-1 (aka >=0).
                                std::cout << "Starting quantile exploration with: (" << epochValues[0] << ", " << epochValues[1] << ")." << std::endl;
                                std::set<typename EpochManager::Epoch> exploredEpochs;
                                while (true) {
                                    auto currEpoch = rewardUnfolding.getStartEpoch(true);
                                    for (uint64_t i = 0; i < 2; ++i) {
                                        if (epochValues[i] >= 0) {
                                            rewardUnfolding.getEpochManager().setDimensionOfEpoch(currEpoch, dimensions[i], (uint64_t) epochValues[i]);
                                        } else {
                                            rewardUnfolding.getEpochManager().setBottomDimension(currEpoch, dimensions[i]);
                                        }
                                    }
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
                                    bool propertySatisfied = probOpFormula.getBound().isSatisfied(currValue);
                                    // If the reward bounds should be as small as possible, we should stop as soon as the property is satisfied.
                                    // If the reward bounds should be as large as possible, we should stop as soon as the property is violated and then go a step backwards
                                    if (storm::solver::minimize(optimizationDirections[0]) == propertySatisfied) {
                                        // We found another point for the result!
                                        resultPoints.push_back(epochValues);
                                        std::cout << "Found another result point: (" << epochValues[0] << ", " << epochValues[1] << ")." << std::endl;
                                        if (epochValues[1] == singleQuantileValues[1].first) {
                                            break;
                                        } else {
                                            ++epochValues[0];
                                            epochValues[1] = singleQuantileValues[1].first;
                                        }
                                    } else {
                                        ++epochValues[1];
                                    }
                                }
                                
                                // Translate the result points to the 'original' domain
                                for (auto& p : resultPoints) {
                                    std::vector<ValueType> convertedP;
                                    for (uint64_t i = 0; i < 2; ++i) {
                                        if (p[i] == infinity) {
                                            convertedP.push_back(storm::utility::infinity<ValueType>());
                                        } else {
                                            if (lowerCostBounds[i]) {
                                                // Translate > to >=
                                                ++p[i];
                                            }
                                            if (i == 1 && storm::solver::maximize(optimizationDirections[i])) {
                                                // When maximizing, we actually searched for each x-value the smallest y-value that leads to a property violation. Hence, decreasing y by one means property satisfaction
                                                --p[i];
                                            }
                                            if (p[i] < 0) {
                                                // Skip this point
                                                convertedP.clear();
                                                continue;
                                            }
                                            convertedP.push_back(storm::utility::convertNumber<ValueType>(p[i]) * rewardUnfolding.getDimension(dimensions[i]).scalingFactor);
                                        }
                                    }
                                    if (!convertedP.empty()) {
                                        result.push_back(convertedP);
                                    }
                                }
                                filterDominatedPoints(result, optimizationDirections);
                            } else if (maxmaxProbSatisfiesFormula) {
                                // i.e., all bound values satisfy the formula
                                if (storm::solver::minimize(optimizationDirections[0])) {
                                    result = {{storm::utility::zero<ValueType>(), storm::utility::zero<ValueType>()}};
                                } else {
                                    result = {{ storm::utility::infinity<ValueType>(),  storm::utility::infinity<ValueType>()}};
                                }
                            } else {
                                // i.e., no bound value satisfies the formula
                                result = {{}};
                            }
                        } else {
                            // TODO: this is an "unreasonable" case
                        }
                    } else {
                        // TODO: find reasonable min/max cases
                    }
                    return result;
                }
                
                template<typename ModelType>
                std::pair<uint64_t, typename ModelType::ValueType> QuantileHelper<ModelType>::computeQuantileForDimension(Environment const& env, uint64_t dimension) const {
                    // We assume that there is one bound value that violates the quantile and one bound value that satisfies it.
                    
                    // Let all other open bounds approach infinity
                    std::vector<BoundTransformation> bts(getDimension(), BoundTransformation::None);
                    storm::storage::BitVector otherLowerBoundedDimensions(getDimension(), false);
                    for (auto const& d : getOpenDimensions()) {
                        if (d != dimension) {
                            if (quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula().hasLowerBound(d)) {
                                bts[d] = BoundTransformation::GreaterZero;
                                otherLowerBoundedDimensions.set(d, true);
                            } else {
                                bts[d] = BoundTransformation::GreaterEqualZero;
                            }
                        }
                    }
                    
                    bool upperCostBound = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula().hasUpperBound(dimension);
                    bool minimizingRewardBound = storm::solver::minimize(getOptimizationDirForDimension(dimension));
                    
                    storm::storage::BitVector nonMecChoices;
                    if (!otherLowerBoundedDimensions.empty()) {
                        STORM_LOG_ASSERT(!upperCostBound, "It is not possible to let other open dimensions approach infinity if this is an upper cost bound and others are lower cost bounds.");
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
                    
                    auto transformedFormula = transformBoundedUntilOperator(quantileFormula.getSubformula().asProbabilityOperatorFormula(), bts);
                    MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, transformedFormula, [&](std::vector<EpochManager::Epoch>& epochSteps, EpochManager const& epochManager) {
                        if (!otherLowerBoundedDimensions.empty()) {
                            for (auto const& choice : nonMecChoices) {
                                for (auto const& dim : otherLowerBoundedDimensions) {
                                    epochManager.setDimensionOfEpoch(epochSteps[choice], dim, 0);
                                }
                            }
                        }
                    });

                    
                    // initialize data that will be needed for each epoch
                    auto lowerBound = rewardUnfolding.getLowerObjectiveBound();
                    auto upperBound = rewardUnfolding.getUpperObjectiveBound();
                    std::vector<ValueType> x, b;
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> minMaxSolver;
                    
                    uint64_t epochValue = 0;
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
                            // We found a satisfying value!
                            if (!upperCostBound) {
                                // The rewardunfolding assumes strict lower cost bounds while we assume non-strict ones. Hence, >B becomes >=(B+1).
                                ++epochValue;
                            }
                            break;
                        } else if (!minimizingRewardBound && !propertySatisfied) {
                            // We found a non-satisfying value. Go one step back to get the largest satisfying value.
                            // ... however, lower cost bounds need to be converted from strict  bounds >B to non strict bounds >=(B+1).
                            // Hence, no operation is necessary in this case.
                            if (upperCostBound) {
                                STORM_LOG_ASSERT(epochValue > 0, "The property does not seem to be satisfiable. This case should have been excluded earlier.");
                                --epochValue;
                            }
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
                    std::cout << "nonmec choices are " << nonMecChoices << std::endl;

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
