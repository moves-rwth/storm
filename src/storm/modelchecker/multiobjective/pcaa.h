#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_H_

#include <memory>

#include "src/storm/modelchecker/results/CheckResult.h"
#include "src/storm/logic/Formulas.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template<typename SparseModelType>
            std::unique_ptr<CheckResult> performPcaa(SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula);
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_H_ */
