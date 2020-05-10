#include "LocalMonotonicityResult.h"

namespace storm {
    namespace analysis {

        template <typename VariableType>
        LocalMonotonicityResult<VariableType>::LocalMonotonicityResult(uint_fast64_t numberOfStates) {
            stateMonRes = std::vector<MonotonicityResult<VariableType>*>(numberOfStates, nullptr);
        }

        template <typename VariableType>
        typename LocalMonotonicityResult<VariableType>::Monotonicity LocalMonotonicityResult<VariableType>::getMonotonicity(uint_fast64_t state, VariableType var) {
            if (stateMonRes[state] != nullptr) {
                return stateMonRes[state]->getMonotonicity(var);
            } else {
                return Monotonicity::Unknown;
            }
        }

        template <typename VariableType>
        void LocalMonotonicityResult<VariableType>::setMonotonicity(uint_fast64_t state, VariableType var, typename LocalMonotonicityResult<VariableType>::Monotonicity mon) {
            if (stateMonRes[state] == nullptr) {
                stateMonRes[state] = new MonotonicityResult<VariableType>();
            }
            stateMonRes[state]->addMonotonicityResult(var, mon);
        }

        template class LocalMonotonicityResult<storm::RationalFunctionVariable>;
    }
}
