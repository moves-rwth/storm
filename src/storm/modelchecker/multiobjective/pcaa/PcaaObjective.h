#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_PCAAOBJECTIVE_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_PCAAOBJECTIVE_H_

#include <iomanip>
#include <boost/optional.hpp>

#include "storm/logic/Formulas.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            template <typename ValueType>
            struct PcaaObjective {
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
                boost::optional<storm::expressions::Expression> lowerTimeBound;
                boost::optional<storm::expressions::Expression> upperTimeBound;
                bool lowerTimeBoundStrict = false;
                bool upperTimeBoundStrict = false;
                
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
                    out << "time bounds:";
                    if(lowerTimeBound) {
                        if(upperTimeBound) {
                            out << (lowerTimeBoundStrict ? "(" : "[") << *lowerTimeBound << ", " <<  *upperTimeBound << (upperTimeBoundStrict ? ")" : "]");
                        } else {
                            out << (lowerTimeBoundStrict ? " >" : ">=") << std::setw(5) << *lowerTimeBound;
                        }
                    } else if (upperTimeBound) {
                        out << (upperTimeBoundStrict ? " <" : "<=") << std::setw(5) << *upperTimeBound;
                    } else {
                        out << " none";
                    }
                    out << ")" << std::endl;
                 }
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_OBJECTIVE_H_ */
