#pragma once

#include <boost/optional.hpp>

#include "storm/logic/Formula.h"
#include "storm/logic/Bound.h"
#include "storm/logic/TimeBound.h"
#include "storm/logic/TimeBoundType.h"
#include "storm/solver/OptimizationDirection.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            template <typename ValueType>
            struct Objective {
                // the original input formula
                std::shared_ptr<storm::logic::Formula const> originalFormula;
                
                // the name of the considered reward model in the preprocessedModel
                boost::optional<std::string> rewardModelName;
                
                // True iff the complementary event is considered.
                // E.g. if we consider P<1-t [F !"safe"] instead of P>=t [ G "safe"]
                bool considersComplementaryEvent;
                
                // The probability/reward threshold for the preprocessed model (if originalFormula specifies one).
                boost::optional<storm::logic::Bound> bound;
                // The optimization direction for the preprocessed model
                // if originalFormula does ot specifies one, the direction is derived from the bound.
                storm::solver::OptimizationDirection optimizationDirection;
                
                // Lower and upper time/step/reward bouds
                boost::optional<storm::logic::TimeBound> lowerTimeBound, upperTimeBound;
                boost::optional<storm::logic::TimeBoundReference> timeBoundReference;
                
                boost::optional<ValueType> lowerResultBound, upperResultBound;
                
                void printToStream(std::ostream& out) const {
                    out  << originalFormula->toString();
                    out << " \t";
                    out << "direction: ";
                    out << optimizationDirection;
                    out << " \t";
                    out << "intern bound: ";
                    if (bound){
                        out << *bound;
                    } else {
                        out << " -none-    ";
                    }
                    out << " \t";
                    out << "time bounds: ";
                    if (lowerTimeBound && upperTimeBound) {
                        out << (lowerTimeBound->isStrict() ? "(" : "[") << lowerTimeBound->getBound() << "," << upperTimeBound->getBound() << (upperTimeBound->isStrict() ? ")" : "]");
                    } else if (lowerTimeBound) {
                        out << (lowerTimeBound->isStrict() ? ">" : ">=") << lowerTimeBound->getBound();
                    } else if (upperTimeBound) {
                        out << (upperTimeBound->isStrict() ? "<" : "<=") << upperTimeBound->getBound();
                    } else {
                        out << " -none-    ";
                    }
                    out << " \t";
                    out << "intern reward model: ";
                    if (rewardModelName) {
                        out << *rewardModelName;
                    } else {
                        out << " -none-    ";
                    }
                    out << " \t";
                    out << "result bounds: ";
                    if (lowerResultBound && upperResultBound) {
                        out << "[" << *lowerResultBound << ", " << *upperResultBound << "]";
                    } else if (lowerResultBound) {
                        out << ">=" << *lowerResultBound;
                    } else if (upperResultBound) {
                        out << "<=" << *upperResultBound;
                    } else {
                        out << " -none-    ";
                    }
                 }
            };
        }
    }
}

