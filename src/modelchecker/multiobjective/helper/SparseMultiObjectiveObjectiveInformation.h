#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEOBJECTIVEINFORMATION_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEOBJECTIVEINFORMATION_H_

#include <iomanip>
#include <boost/optional.hpp>

#include "src/logic/Formulas.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            template <typename ValueType>
            struct SparseMultiObjectiveObjectiveInformation {
                // the original input formula
                std::shared_ptr<storm::logic::Formula const> originalFormula;
                
                // the name of the considered reward model in the preprocessedModel
                std::string rewardModelName;
                
                // true if all rewards for this objective are positive, false if all rewards are negative.
                bool rewardsArePositive;
                
                // transformation from the values of the preprocessed model to the ones for the actual input model, i.e.,
                // x is achievable in the preprocessed model iff factor*x + offset is achievable in the original model
                ValueType toOriginalValueTransformationFactor;
                ValueType toOriginalValueTransformationOffset;
                
                // The probability/reward threshold for the preprocessed model (if originalFormula specifies one).
                // This is always a lower bound.
                boost::optional<ValueType> threshold;
                // True iff the specified threshold is strict, i.e., >
                bool thresholdIsStrict = false;
                
                // The time bound(s) for the formula (if given by the originalFormula)
                // If only a discrete bound is given, it is interpreted as an upper bound
                boost::optional<boost::variant<uint_fast64_t, std::pair<double, double>>> timeBounds;
                
                // Stores whether reward finiteness has been checked for this objective.
                bool rewardFinitenessChecked;
                
                void printToStream(std::ostream& out) const {
                    out << std::setw(30) << originalFormula->toString();
                    out << " \t(toOrigVal:" << std::setw(3) << toOriginalValueTransformationFactor << "*x +" << std::setw(3) << toOriginalValueTransformationOffset << ", \t";
                    out << "intern threshold:";
                    if(threshold){
                        out << (thresholdIsStrict ? " >" : ">=");
                        out << std::setw(5) << (*threshold) << ",";
                    } else {
                        out << "   none,";
                    }
                    out << " \t";
                    out << "intern reward model: " << std::setw(10) << rewardModelName;
                    out << (rewardsArePositive ? " (positive)" : " (negative)") << ", \t";
                    out << "time bound:";
                    if(timeBounds) {
                        if(timeBounds->which() == 0 ) {
                            out << "<=" << std::setw(5) << (boost::get<uint_fast64_t>(timeBounds.get()));
                        } else {
                            out << "[" << boost::get<std::pair<double, double>>(timeBounds.get()).first << ", " <<  boost::get<std::pair<double, double>>(timeBounds.get()).second << "]";
                        }
                    } else {
                        out << " none";
                    }
                    out << ")" << std::endl;
                 }
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEOBJECTIVEINFORMATION_H_ */
