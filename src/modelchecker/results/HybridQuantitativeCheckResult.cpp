#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/storage/dd/CuddDdManager.h"

#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type>
        HybridQuantitativeCheckResult<Type>::HybridQuantitativeCheckResult(storm::dd::Bdd<Type> const& reachableStates, storm::dd::Bdd<Type> const& symbolicStates, storm::dd::Add<Type> const& symbolicValues, storm::dd::Bdd<Type> const& explicitStates, storm::dd::Odd<Type> const& odd, std::vector<double> const& explicitValues) : reachableStates(reachableStates), symbolicStates(symbolicStates), symbolicValues(symbolicValues), explicitStates(explicitStates), odd(odd), explicitValues(explicitValues) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        std::unique_ptr<CheckResult> HybridQuantitativeCheckResult<Type>::compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const {
            storm::dd::Bdd<Type> symbolicResult;

            // First compute the symbolic part of the result.
            if (comparisonType == storm::logic::ComparisonType::Less) {
                symbolicResult = symbolicValues.less(bound);
            } else if (comparisonType == storm::logic::ComparisonType::LessEqual) {
                symbolicResult = symbolicValues.lessOrEqual(bound);
            } else if (comparisonType == storm::logic::ComparisonType::Greater) {
                symbolicResult = symbolicValues.greater(bound);
            } else if (comparisonType == storm::logic::ComparisonType::GreaterEqual) {
                symbolicResult = symbolicValues.greaterOrEqual(bound);
            }
            
            // Then translate the explicit part to a symbolic format and simultaneously to a qualitative result.
            symbolicResult |= storm::dd::Bdd<Type>(this->reachableStates.getDdManager(), this->explicitValues, this->odd, this->symbolicValues.getContainedMetaVariables(), comparisonType, bound);
            
            return std::unique_ptr<SymbolicQualitativeCheckResult<Type>>(new SymbolicQualitativeCheckResult<Type>(reachableStates, symbolicResult));
        }
        
        template<storm::dd::DdType Type>
        std::unique_ptr<CheckResult> HybridQuantitativeCheckResult<Type>::toExplicitQuantitativeCheckResult() const {
            storm::dd::Bdd<Type> allStates = symbolicStates || explicitStates;
            storm::dd::Odd<Type> allStatesOdd(allStates);
            
            std::vector<double> fullExplicitValues = symbolicValues.template toVector<double>(allStatesOdd);
            this->odd.expandExplicitVector(allStatesOdd, this->explicitValues, fullExplicitValues);
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<double>(std::move(fullExplicitValues)));
        }
        
        template<storm::dd::DdType Type>
        bool HybridQuantitativeCheckResult<Type>::isHybrid() const {
            return true;
        }
        
        template<storm::dd::DdType Type>
        bool HybridQuantitativeCheckResult<Type>::isResultForAllStates() const {
            return true;
        }
        
        template<storm::dd::DdType Type>
        bool HybridQuantitativeCheckResult<Type>::isHybridQuantitativeCheckResult() const {
            return true;
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Bdd<Type> const& HybridQuantitativeCheckResult<Type>::getSymbolicStates() const {
            return symbolicStates;
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Add<Type> const& HybridQuantitativeCheckResult<Type>::getSymbolicValueVector() const {
            return symbolicValues;
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Bdd<Type> const& HybridQuantitativeCheckResult<Type>::getExplicitStates() const {
            return explicitStates;
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Odd<Type> const& HybridQuantitativeCheckResult<Type>::getOdd() const {
            return odd;
        }
        
        template<storm::dd::DdType Type>
        std::vector<double> const& HybridQuantitativeCheckResult<Type>::getExplicitValueVector() const {
            return explicitValues;
        }
        
        template<storm::dd::DdType Type>
        std::ostream& HybridQuantitativeCheckResult<Type>::writeToStream(std::ostream& out) const {
            out << "[";
            bool first = true;
            if (!this->symbolicStates.isZero()) {
                if (this->symbolicValues.isZero()) {
                    out << "0";
                } else {
                    for (auto valuationValuePair : this->symbolicValues) {
                        if (!first) {
                            out << ", ";
                        } else {
                            first = false;
                        }
                        out << valuationValuePair.second;
                    }
                }
            }
            if (!this->explicitStates.isZero()) {
                for (auto const& element : this->explicitValues) {
                    if (!first) {
                        out << ", ";
                    } else {
                        first = false;
                    }
                    out << element;
                }
            }
            out << "]";
            return out;
        }
        
        template<storm::dd::DdType Type>
        void HybridQuantitativeCheckResult<Type>::filter(QualitativeCheckResult const& filter) {
            STORM_LOG_THROW(filter.isSymbolicQualitativeCheckResult(), storm::exceptions::InvalidOperationException, "Cannot filter hybrid check result with non-symbolic filter.");
            
            // First, we filter the symbolic values.
            this->symbolicStates = this->symbolicStates && filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector();
            this->symbolicValues *= symbolicStates.toAdd();
            
            // Next, we filter the explicit values.

            // Start by computing the new set of states that is stored explictly and the corresponding ODD.
            this->explicitStates = this->explicitStates && filter.asSymbolicQualitativeCheckResult<Type>().getTruthValuesVector();
            storm::dd::Odd<Type> newOdd(explicitStates);
            
            // Then compute the new vector of explicit values and set the new data fields.
            this->explicitValues = this->odd.filterExplicitVector(explicitStates, this->explicitValues);
            this->odd = newOdd;
        }
        
        template<storm::dd::DdType Type>
        double HybridQuantitativeCheckResult<Type>::getMin() const {
            // In order to not get false zeros, we need to set the values of all states whose values is not stored
            // symbolically to infinity.
            storm::dd::Add<Type> tmp = symbolicStates.toAdd().ite(this->symbolicValues, reachableStates.getDdManager()->getConstant(storm::utility::infinity<double>()));
            double min = tmp.getMin();
            if (!explicitStates.isZero()) {
                for (auto const& element : explicitValues) {
                    min = std::min(min, element);
                }
            }
            return min;
        }
        
        template<storm::dd::DdType Type>
        double HybridQuantitativeCheckResult<Type>::getMax() const {
            double max = this->symbolicValues.getMax();
            if (!explicitStates.isZero()) {
                for (auto const& element : explicitValues) {
                    max = std::max(max, element);
                }
            }
            return max;
        }
        
        // Explicitly instantiate the class.
        template class HybridQuantitativeCheckResult<storm::dd::DdType::CUDD>;
    }
}