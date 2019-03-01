#include "storm/modelchecker/prctl/helper/rewardbounded/QuantileHelper.h"

#include <set>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/models/sparse/Mdp.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/MultiDimensionalRewardUnfolding.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"
#include "storm/utility/vector.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

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
                    // Bounds are either constants or variables that are declared in the quantile formula.
                    // Prop op has optimality type
                    // No Prmin with lower cost bounds: Ec construction fails. In single obj we would do 1-Prmax[F "nonRewOrNonGoalEC"] but this invalidates other lower/upper cost bounds.
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
                        if (origBoundedUntil.hasMultiDimensionalSubformulas()) {
                            leftSubformulas.push_back(origBoundedUntil.getLeftSubformula(dim).asSharedPointer());
                            rightSubformulas.push_back(origBoundedUntil.getRightSubformula(dim).asSharedPointer());
                        }
                        timeBoundReferences.push_back(origBoundedUntil.getTimeBoundReference(dim));
                        if (transformations[dim] == BoundTransformation::None) {
                            if (origBoundedUntil.hasLowerBound(dim)) {
                                lowerBounds.push_back(storm::logic::TimeBound(origBoundedUntil.isLowerBoundStrict(dim), origBoundedUntil.getLowerBound(dim)));
                            } else {
                                lowerBounds.push_back(boost::none);
                            }
                            if (origBoundedUntil.hasUpperBound(dim)) {
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
                    std::shared_ptr<storm::logic::Formula> newBoundedUntil;
                    if (origBoundedUntil.hasMultiDimensionalSubformulas()) {
                        newBoundedUntil = std::make_shared<storm::logic::BoundedUntilFormula>(leftSubformulas, rightSubformulas, lowerBounds, upperBounds, timeBoundReferences);
                    } else {
                        newBoundedUntil = std::make_shared<storm::logic::BoundedUntilFormula>(origBoundedUntil.getLeftSubformula().asSharedPointer(), origBoundedUntil.getRightSubformula().asSharedPointer(), lowerBounds, upperBounds, timeBoundReferences);
                    }
                    return std::make_shared<storm::logic::ProbabilityOperatorFormula>(newBoundedUntil, boundedUntilOperator.getOperatorInformation());
                }

                /// Increases the precision of solver results
                void increasePrecision(storm::Environment& env) {
                    STORM_LOG_DEBUG("Increasing precision of underlying solver.");
                    auto factor = storm::utility::convertNumber<storm::RationalNumber, std::string>("0.1");
                    env.solver().setLinearEquationSolverPrecision(static_cast<storm::RationalNumber>(env.solver().getPrecisionOfLinearEquationSolver(env.solver().getLinearEquationSolverType()).first.get() * factor));
                    env.solver().minMax().setPrecision(env.solver().minMax().getPrecision() * factor);
                }
                
                /// Computes a lower/ upper bound on the actual result of a minmax or linear equation solver
                template<typename ValueType>
                std::pair<ValueType, ValueType> getLowerUpperBound(storm::Environment const& env, ValueType const& value, bool minMax = true) {
                    ValueType prec;
                    bool relative;
                    if (minMax) {
                        prec = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision());
                        relative = env.solver().minMax().getRelativeTerminationCriterion();
                    } else {
                        prec = storm::utility::convertNumber<ValueType>(env.solver().getPrecisionOfLinearEquationSolver(env.solver().getLinearEquationSolverType()).first.get());
                        relative = env.solver().getPrecisionOfLinearEquationSolver(env.solver().getLinearEquationSolverType()).second.get();
                    }
                    if (relative) {
                        return std::make_pair<ValueType, ValueType>(value * (1/(prec + 1)), value * (1 + prec/(prec +1)));
                    } else {
                        return std::make_pair<ValueType, ValueType>(value - prec, value + prec);
                    }
                }
                
                template<typename ModelType>
                uint64_t QuantileHelper<ModelType>::getDimension() const {
                    return quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula().getDimension();
                }

                template<typename ModelType>
                storm::storage::BitVector QuantileHelper<ModelType>::getOpenDimensions() const {
                    auto const& boundedUntil = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula();
                    storm::storage::BitVector res(getDimension(), false);
                    for (uint64_t dim = 0; dim < getDimension(); ++dim) {
                        auto const& bound = boundedUntil.hasLowerBound(dim) ? boundedUntil.getLowerBound(dim) : boundedUntil.getUpperBound(dim);
                        if (bound.containsVariables()) {
                            res.set(dim, true);
                        }
                    }
                    return res;
                }

                template<typename ModelType>
                storm::solver::OptimizationDirection const& QuantileHelper<ModelType>::getOptimizationDirForDimension(uint64_t const& dim) const {
                    storm::expressions::Variable const& dimVar = getVariableForDimension(dim);
                    for (auto const& boundVar : quantileFormula.getBoundVariables()) {
                        if (boundVar.second == dimVar) {
                            return boundVar.first;
                        }
                    }
                    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "The bound variable '" << dimVar.getName() << "' is not specified within the quantile formula '" << quantileFormula << "'.");
                    return quantileFormula.getOptimizationDirection();
                }

                template<typename ModelType>
                storm::expressions::Variable const& QuantileHelper<ModelType>::getVariableForDimension(uint64_t const& dim) const {
                    auto const& boundedUntil = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula();
                    return (boundedUntil.hasLowerBound(dim) ? boundedUntil.getLowerBound(dim) : boundedUntil.getUpperBound(dim)).getBaseExpression().asVariableExpression().getVariable();
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
                std::vector<std::vector<typename ModelType::ValueType>> QuantileHelper<ModelType>::computeQuantile(Environment const& env) const {
                    std::vector<std::vector<ValueType>> result;
                    Environment envCpy = env; // It might be necessary to increase the precision during the computation
                    numCheckedEpochs = 0; numPrecisionRefinements = 0;
                    if (getOpenDimensions().getNumberOfSetBits() == 1) {
                        uint64_t dimension = *getOpenDimensions().begin();
                        
                        bool zeroSatisfiesFormula = checkLimitValue(envCpy, storm::storage::BitVector(getDimension(), false));
                        bool infSatisfiesFormula = checkLimitValue(envCpy, getOpenDimensions());
                        if (zeroSatisfiesFormula != infSatisfiesFormula) {
                            while (true) {
                                auto quantileRes = computeQuantileForDimension(envCpy, dimension);
                                if (quantileRes) {
                                    result = {{storm::utility::convertNumber<ValueType>(quantileRes->first) * quantileRes->second}};
                                    break;
                                }
                                increasePrecision(envCpy);
                                ++numPrecisionRefinements;
                            }
                        } else if (zeroSatisfiesFormula) {
                            // thus also infSatisfiesFormula is true, i.e., all bound values satisfy the formula
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
                        result = computeTwoDimensionalQuantile(envCpy);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The quantile formula considers " << getOpenDimensions().getNumberOfSetBits() << " open dimensions. Only one- or two-dimensional quantiles are supported.");
                    }
                    if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                        std::cout << "Number of checked epochs: " << numCheckedEpochs << std::endl;
                        std::cout << "Number of required precision refinements: " << numPrecisionRefinements << std::endl;
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
                std::vector<std::vector<typename ModelType::ValueType>> QuantileHelper<ModelType>::computeTwoDimensionalQuantile(Environment& env) const {
                    std::vector<std::vector<ValueType>> result;

                    auto const& probOpFormula = quantileFormula.getSubformula().asProbabilityOperatorFormula();
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
                            
                            bool infInfProbSatisfiesFormula = checkLimitValue(env, storm::storage::BitVector(getDimension(), false));
                            bool zeroZeroProbSatisfiesFormula = checkLimitValue(env, dimensionsAsBitVector[0] | dimensionsAsBitVector[1]);
                            if (infInfProbSatisfiesFormula != zeroZeroProbSatisfiesFormula) {
                                std::vector<std::pair<int64_t, typename ModelType::ValueType>> singleQuantileValues;
                                for (uint64_t i = 0; i < 2; ++i) {
                                    // find out whether the bounds B_i = 0 and B_1-i = infinity satisfy the formula
                                    bool zeroInfSatisfiesFormula = checkLimitValue(env, dimensionsAsBitVector[1-i]);
                                    if (zeroInfSatisfiesFormula == storm::solver::minimize(optimizationDirections[0])) {
                                        // There is bound value b such that the point B_i=0 and B_1-i = b is part of the result
                                        singleQuantileValues.emplace_back(0, storm::utility::zero<ValueType>());
                                    } else {
                                        // Compute quantile where 1-i is set to infinity
                                        singleQuantileValues.push_back(computeQuantileForDimension(env, dimensions[i]).get());
                                        if (!storm::solver::minimize(optimizationDirections[i])) {
                                            // When maximizing bounds, the computed single dimensional quantile value is sat for all values of the other bound.
                                            // Increase the computed value so that there is at least one unsat assignment of bounds with B[i] = singleQuantileValues[i]
                                            ++singleQuantileValues.back().first;
                                        }
                                    }
                                    // Decrease the value for lower cost bounds to convert >= to >
                                    if (lowerCostBounds[i]) {
                                        --singleQuantileValues[i].first;
                                    }
                                }
                                std::vector<int64_t> currentEpochValues = {(int64_t) singleQuantileValues[0].first, (int64_t) singleQuantileValues[1].first}; // signed int is needed to allow lower bounds >-1 (aka >=0).
                                while (!exploreTwoDimensionalQuantile(env, singleQuantileValues, currentEpochValues, result)) {
                                    increasePrecision(env);
                                    ++numPrecisionRefinements;
                                }
                            } else if (infInfProbSatisfiesFormula) {
                                // then also zeroZeroProb satisfies the formula, i.e., all bound values satisfy the formula
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
                bool QuantileHelper<ModelType>::exploreTwoDimensionalQuantile(Environment const& env, std::vector<std::pair<int64_t, typename ModelType::ValueType>> const& startEpochValues, std::vector<int64_t>& currentEpochValues, std::vector<std::vector<ValueType>>& resultPoints) const {
                    // Initialize some data for easy access
                    auto const& probOpFormula = quantileFormula.getSubformula().asProbabilityOperatorFormula();
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
                    
                    
                    MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, transformBoundedUntilOperator(quantileFormula.getSubformula().asProbabilityOperatorFormula(), std::vector<BoundTransformation>(getDimension(), BoundTransformation::None)));
                        
                    // initialize data that will be needed for each epoch
                    auto lowerBound = rewardUnfolding.getLowerObjectiveBound();
                    auto upperBound = rewardUnfolding.getUpperObjectiveBound();
                    std::vector<ValueType> x, b;
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> minMaxSolver;
                    
                    if (currentEpochValues[0] < 0 && currentEpochValues[1] < 0) {
                        // This case can only happen in these cases:
                        assert(lowerCostBounds[0]);
                        assert(currentEpochValues[0] == -1);
                        assert(currentEpochValues[1] == -1);
                        // This case has been checked already, so we can skip it.
                        // Skipping this is actually necessary, since the rewardUnfolding will handle formulas like [F{"a"}>A,{"b"}>B "init"] incorrectly if A=B=-1.
                        ++currentEpochValues[1];
                    }
                    std::set<typename EpochManager::Epoch> exploredEpochs;
                    while (true) {
                        auto currEpoch = rewardUnfolding.getStartEpoch(true);
                        for (uint64_t i = 0; i < 2; ++i) {
                            if (currentEpochValues[i] >= 0) {
                                rewardUnfolding.getEpochManager().setDimensionOfEpoch(currEpoch, dimensions[i], (uint64_t) currentEpochValues[i]);
                            } else {
                                rewardUnfolding.getEpochManager().setBottomDimension(currEpoch, dimensions[i]);
                            }
                        }
                        auto epochOrder = rewardUnfolding.getEpochComputationOrder(currEpoch);
                        for (auto const& epoch : epochOrder) {
                            if (exploredEpochs.count(epoch) == 0) {
                                auto& epochModel = rewardUnfolding.setCurrentEpoch(epoch);
                                rewardUnfolding.setSolutionForCurrentEpoch(epochModel.analyzeSingleObjective(env, quantileFormula.getSubformula().asProbabilityOperatorFormula().getOptimalityType(), x, b, minMaxSolver, lowerBound, upperBound));
                                ++numCheckedEpochs;
                                exploredEpochs.insert(epoch);
                            }
                        }
                        ValueType currValue = rewardUnfolding.getInitialStateResult(currEpoch);
                        bool propertySatisfied;
                        if (env.solver().isForceSoundness()) {
                            auto lowerUpperValue = getLowerUpperBound(env, currValue);
                            propertySatisfied = probOpFormula.getBound().isSatisfied(lowerUpperValue.first);
                            if (propertySatisfied != probOpFormula.getBound().isSatisfied(lowerUpperValue.second)) {
                                // unclear result due to insufficient precision.
                                return false;
                            }
                        } else {
                            propertySatisfied = probOpFormula.getBound().isSatisfied(currValue);
                        }
                        
                        // If the reward bounds should be as small as possible, we should stop as soon as the property is satisfied.
                        // If the reward bounds should be as large as possible, we should stop as soon as the property is violated and then go a step backwards
                        if (storm::solver::minimize(optimizationDirections[0]) == propertySatisfied) {
                            // We found another point for the result! Translate it to the original domain
                            auto point = currentEpochValues;
                            std::vector<ValueType> convertedPoint;
                            for (uint64_t i = 0; i < 2; ++i) {
                                if (lowerCostBounds[i]) {
                                    // Translate > to >=
                                    ++point[i];
                                }
                                if (i == 1 && storm::solver::maximize(optimizationDirections[i])) {
                                    // When maximizing, we actually searched for each x-value the smallest y-value that leads to a property violation. Hence, decreasing y by one means property satisfaction
                                    --point[i];
                                }
                                if (point[i] < 0) {
                                    // Skip this point
                                    convertedPoint.clear();
                                    continue;
                                }
                                convertedPoint.push_back(storm::utility::convertNumber<ValueType>(point[i]) * rewardUnfolding.getDimension(dimensions[i]).scalingFactor);
                            }
                            if (!convertedPoint.empty()) {
                                resultPoints.push_back(std::move(convertedPoint));
                            }
                            
                            if (currentEpochValues[1] == startEpochValues[1].first) {
                                break;
                            } else {
                                ++currentEpochValues[0];
                                currentEpochValues[1] = startEpochValues[1].first;
                            }
                        } else {
                            ++currentEpochValues[1];
                        }
                    }
                    
    
                    // When maximizing, there are border cases where one dimension can be arbitrarily large
                    for (uint64_t i = 0; i < 2; ++i) {
                        if (storm::solver::maximize(optimizationDirections[i]) && startEpochValues[i].first > 0) {
                            std::vector<ValueType> newResultPoint(2);
                            newResultPoint[i] = storm::utility::convertNumber<ValueType>(startEpochValues[i].first - 1) * startEpochValues[i].second;
                            newResultPoint[1-i] = storm::utility::infinity<ValueType>();
                            resultPoints.push_back(newResultPoint);
                        }
                    }
                    
                    filterDominatedPoints(resultPoints, optimizationDirections);
                    return true;
                }
                
                template<typename ModelType>
                boost::optional<std::pair<uint64_t, typename ModelType::ValueType>> QuantileHelper<ModelType>::computeQuantileForDimension(Environment const& env, uint64_t dimension) const {
                    // We assume that there is one bound value that violates the quantile and one bound value that satisfies it.
                    
                    // Let all other open bounds approach infinity
                    std::set<storm::expressions::Variable> infinityVariables;
                    for (auto const& d : getOpenDimensions()) {
                        if (d != dimension) {
                            infinityVariables.insert(getVariableForDimension(d));
                        }
                    }
                    auto transformedFormula = transformBoundedUntilOperator(quantileFormula.getSubformula().asProbabilityOperatorFormula(), std::vector<BoundTransformation>(getDimension(), BoundTransformation::None));
                    MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, transformedFormula, infinityVariables);
                    
                    bool upperCostBound = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula().hasUpperBound(dimension);
                    bool minimizingRewardBound = storm::solver::minimize(getOptimizationDirForDimension(dimension));
                    
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
                                ++numCheckedEpochs;
                                exploredEpochs.insert(epoch);
                            }
                        }
                        ValueType currValue = rewardUnfolding.getInitialStateResult(currEpoch);
                        
                        bool propertySatisfied;
                        if (env.solver().isForceSoundness()) {
                            auto lowerUpperValue = getLowerUpperBound(env, currValue);
                            propertySatisfied = transformedFormula->getBound().isSatisfied(lowerUpperValue.first);
                            if (propertySatisfied != transformedFormula->getBound().isSatisfied(lowerUpperValue.second)) {
                                // unclear result due to insufficient precision.
                                return boost::none;
                            }
                        } else {
                            propertySatisfied = transformedFormula->getBound().isSatisfied(currValue);
                        }
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
                    return std::make_pair(epochValue, rewardUnfolding.getDimension(dimension).scalingFactor);
                }
                
                template<typename ModelType>
                bool QuantileHelper<ModelType>::checkLimitValue(Environment& env, storm::storage::BitVector const& infDimensions) const {
                    auto const& probabilityBound = quantileFormula.getSubformula().asProbabilityOperatorFormula().getBound();
                    // Increase the precision until we get a conclusive result
                    while (true) {
                        ValueType numericResult = computeLimitValue(env, infDimensions);
                        if (env.solver().isForceSoundness()) {
                            auto lowerUpper = getLowerUpperBound(env, numericResult);
                            bool lowerSat = probabilityBound.isSatisfied(lowerUpper.first);
                            bool upperSat = probabilityBound.isSatisfied(lowerUpper.second);
                            if (lowerSat == upperSat) {
                                return lowerSat;
                            } else {
                                increasePrecision(env);
                                ++numPrecisionRefinements;
                            }
                        } else {
                            return probabilityBound.isSatisfied(numericResult);
                        }
                    }
                }
                
                template<typename ModelType>
                typename ModelType::ValueType QuantileHelper<ModelType>::computeLimitValue(Environment const& env, storm::storage::BitVector const& infDimensions) const {
                    // To compute the limit for an upper bounded dimension, we can simply drop the bound
                    // To compute the limit for a lower bounded dimension, we only consider epoch steps that lie in an end component and check for the bound >0 instead.
                    // Notice, however, that this approach becomes problematic if, at the same time, we consider an upper bounded dimension with bound value zero.
                    std::vector<BoundTransformation> bts(getDimension(), BoundTransformation::None);
                    std::set<storm::expressions::Variable> infinityVariables;
                    for (auto const& d : getOpenDimensions()) {
                        bool upperCostBound = quantileFormula.getSubformula().asProbabilityOperatorFormula().getSubformula().asBoundedUntilFormula().hasUpperBound(d);
                        if (infDimensions.get(d)) {
                            infinityVariables.insert(getVariableForDimension(d));
                        } else {
                            if (upperCostBound) {
                                bts[d] = BoundTransformation::LessEqualZero;
                            } else {
                                bts[d] = BoundTransformation::GreaterEqualZero;
                            }
                        }
                    }
                    auto transformedFormula = transformBoundedUntilOperator(quantileFormula.getSubformula().asProbabilityOperatorFormula(), bts);
                    
                    MultiDimensionalRewardUnfolding<ValueType, true> rewardUnfolding(model, transformedFormula, infinityVariables);
                    if (!rewardUnfolding.getProb1Objectives().empty()) {
                        assert(rewardUnfolding.getProb1Objectives().size() == 1);
                        // The probability is one.
                        return storm::utility::one<ValueType>();
                    }
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
                        ++numCheckedEpochs;
                    }

                    ValueType numericResult = rewardUnfolding.getInitialStateResult(initEpoch);
                    STORM_LOG_TRACE("Limit probability for infinity dimensions " << infDimensions << " is " << numericResult << ".");
                    return numericResult;
                }

                template class QuantileHelper<storm::models::sparse::Mdp<double>>;
                template class QuantileHelper<storm::models::sparse::Mdp<storm::RationalNumber>>;

            }
        }
    }
}
