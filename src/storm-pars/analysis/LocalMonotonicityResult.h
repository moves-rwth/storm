#ifndef STORM_LOCALMONOTONICITYRESULT_H
#define STORM_LOCALMONOTONICITYRESULT_H

#include <vector>
#include "storm-pars/analysis/MonotonicityResult.h"
#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace analysis {
        template <typename VariableType>
        class LocalMonotonicityResult {

        public:
            typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;

            LocalMonotonicityResult(uint_fast64_t numberOfStates);

            Monotonicity getMonotonicity(uint_fast64_t state, VariableType var);

            void setMonotonicity(uint_fast64_t state, VariableType var, Monotonicity mon);

        private:
            std::vector<MonotonicityResult<VariableType>*> stateMonRes;
        };
    }
}

#endif //STORM_LOCALMONOTONICITYRESULT_H
