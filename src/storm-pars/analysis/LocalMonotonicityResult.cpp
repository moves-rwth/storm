#include "LocalMonotonicityResult.h"

namespace storm {
    namespace analysis {

        template <typename VariableType>
        LocalMonotonicityResult<VariableType>::LocalMonotonicityResult(uint_fast64_t numberOfStates) {
            stateMonRes = std::vector<std::shared_ptr<MonotonicityResult<VariableType>>>(numberOfStates, nullptr);
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
                stateMonRes[state] = std::make_shared<MonotonicityResult<VariableType>>();
            }
            stateMonRes[state]->addMonotonicityResult(var, mon);
        }

        template <typename VariableType>
        void LocalMonotonicityResult<VariableType>::setMonotonicityResult(uint_fast64_t state, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
            this->stateMonRes[state] = monRes;
        }

        template <typename VariableType>
        std::shared_ptr<LocalMonotonicityResult<VariableType>> LocalMonotonicityResult<VariableType>::copy() {
            std::shared_ptr<LocalMonotonicityResult<VariableType>> copy = std::make_shared<LocalMonotonicityResult<VariableType>>(stateMonRes.size());
            for (auto state = 0; state < stateMonRes.size(); state++) {
                if (stateMonRes[state] != nullptr) {
                    copy->setMonotonicityResult(state, stateMonRes[state]->copy());
                }
            }
            return copy;
        }

        template class LocalMonotonicityResult<storm::RationalFunctionVariable>;
    }
}
