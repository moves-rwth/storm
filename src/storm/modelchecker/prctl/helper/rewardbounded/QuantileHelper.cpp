#include "storm/modelchecker/prctl/helper/rewardbounded/QuantileHelper.h"

#include <set>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm/models/sparse/Mdp.h"
#include "storm/storage/expressions/ExpressionManager.h"

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

                std::shared_ptr<storm::logic::ProbabilityOperatorFormula> replaceBoundsByGreaterEqZero(std::shared_ptr<storm::logic::ProbabilityOperatorFormula> const& boundedUntilOperator, std::set<uint64_t> dimensionsToReplace) {
                    auto const& origBoundedUntil = boundedUntilOperator->getSubformula().asBoundedUntilFormula();
                    STORM_LOG_ASSERT(*(--dimensionsToReplace.end()) < origBoundedUntil.getDimension(), "Tried to replace the bound of a dimension that is higher than the number of dimensions of the formula.");
                    std::vector<std::shared_ptr<storm::logic::Formula const>> leftSubformulas, rightSubformulas;
                    std::vector<boost::optional<storm::logic::TimeBound>> lowerBounds, upperBounds;
                    std::vector<storm::logic::TimeBoundReference> timeBoundReferences;

                    for (uint64_t dim = 0; dim < origBoundedUntil.getDimension(); ++dim) {
                        leftSubformulas.push_back(origBoundedUntil.getLeftSubformula(dim).asSharedPointer());
                        rightSubformulas.push_back(origBoundedUntil.getRightSubformula(dim).asSharedPointer());
                        timeBoundReferences.push_back(origBoundedUntil.getTimeBoundReference(dim));
                        if (dimensionsToReplace.count(dim) == 0) {
                            if (origBoundedUntil.hasLowerBound()) {
                                lowerBounds.push_back(storm::logic::TimeBound(origBoundedUntil.isLowerBoundStrict(dim), origBoundedUntil.getLowerBound(dim)));
                            } else {
                                lowerBounds.push_back(boost::none);
                            }
                            if (origBoundedUntil.hasUpperBound()) {
                                upperBounds.emplace_back(origBoundedUntil.isUpperBoundStrict(dim), origBoundedUntil.getUpperBound(dim));
                            } else {
                                upperBounds.push_back(boost::none);
                            }
                        } else {
                            storm::expressions::Expression zero;
                            if (origBoundedUntil.hasLowerBound(dim)) {
                                zero = origBoundedUntil.getLowerBound(dim).getManager().rational(0.0);
                            } else {
                                STORM_LOG_THROW(origBoundedUntil.hasUpperBound(dim), storm::exceptions::InvalidOperationException, "The given bounded until formula has no cost-bound for one dimension.");
                                zero = origBoundedUntil.getUpperBound(dim).getManager().rational(0.0);
                            }
                            lowerBounds.emplace_back(false, zero);
                            upperBounds.push_back(boost::none);
                        }
                    }
                    auto newBoundedUntil = std::make_shared<storm::logic::BoundedUntilFormula>(leftSubformulas, rightSubformulas, lowerBounds, upperBounds, timeBoundReferences);
                    return std::make_shared<storm::logic::ProbabilityOperatorFormula>(newBoundedUntil, boundedUntilOperator->getOperatorInformation());
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
                    return false;
                }


                template class QuantileHelper<storm::models::sparse::Mdp<double>>;
                template class QuantileHelper<storm::models::sparse::Mdp<storm::RationalNumber>>;

            }
        }
    }
}
