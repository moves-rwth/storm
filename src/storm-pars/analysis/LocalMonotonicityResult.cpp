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

        template <typename VariableType>
        void LocalMonotonicityResult<VariableType>::setMonotonicityResult(uint_fast64_t state, MonotonicityResult<VariableType>* monRes) {
            this->stateMonRes[state] = monRes;
        }

        template <typename VariableType>
        LocalMonotonicityResult<VariableType>* LocalMonotonicityResult<VariableType>::copy() {
            auto copy = new LocalMonotonicityResult<VariableType>(stateMonRes.size());
            for (auto state = 0; state < stateMonRes.size(); state++) {
                copy->setMonotonicityResult(state, stateMonRes[state]->copy());
            }
            return copy;
        }

        template class LocalMonotonicityResult<storm::RationalFunctionVariable>;
    }
}
