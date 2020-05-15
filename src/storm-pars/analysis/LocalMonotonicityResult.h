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

            void setMonotonicityResult(uint_fast64_t state, MonotonicityResult<VariableType>* monRes);

            /*!
             * Constructs a new LocalMonotonicityResult object that is a copy of the current one
             * @return Pointer to the copy
             */
            LocalMonotonicityResult<VariableType>* copy();

        private:
            std::vector<MonotonicityResult<VariableType>*> stateMonRes;
        };
    }
}

#endif //STORM_LOCALMONOTONICITYRESULT_H
