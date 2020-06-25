#include "MonotonicityResult.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

namespace storm {
    namespace analysis {

        template <typename VariableType>
        MonotonicityResult<VariableType>::MonotonicityResult() {
            this->done = false;
            this->somewhereMonotonicity = true;
            this->allMonotonicity = true;
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::addMonotonicityResult(VariableType var,  MonotonicityResult<VariableType>::Monotonicity mon) {
            monotonicityResult.insert(std::pair<VariableType, MonotonicityResult<VariableType>::Monotonicity>(std::move(var), std::move(mon)));
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::updateMonotonicityResult(VariableType var, MonotonicityResult<VariableType>::Monotonicity mon) {

            if (mon == MonotonicityResult<VariableType>::Monotonicity::Not) {
                mon = MonotonicityResult<VariableType>::Monotonicity::Unknown;
            }

            if (monotonicityResult.find(var) == monotonicityResult.end()) {
                addMonotonicityResult(std::move(var), std::move(mon));
            } else {
                auto monRes = monotonicityResult[var];
                if (monRes == MonotonicityResult<VariableType>::Monotonicity::Unknown || monRes == mon || mon == MonotonicityResult<VariableType>::Monotonicity::Constant) {
                    return;
                } else if (mon == MonotonicityResult<VariableType>::Monotonicity::Unknown || monRes == MonotonicityResult<VariableType>::Monotonicity::Constant ) {
                    monotonicityResult[var] = mon;
                } else {
                    monotonicityResult[var] = MonotonicityResult<VariableType>::Monotonicity::Unknown;
                }
            }
            if (monotonicityResult[var] == MonotonicityResult<VariableType>::Monotonicity::Unknown) {
                setAllMonotonicity(false);
                setSomewhereMonotonicity(false);
            } else {
                setSomewhereMonotonicity(true);
            }
        }

        template <typename VariableType>
        typename MonotonicityResult<VariableType>::Monotonicity MonotonicityResult<VariableType>::getMonotonicity(VariableType var) const {
            auto itr = monotonicityResult.find(var);
            if (itr != monotonicityResult.end()) {
                return itr->second;
            }
            return Monotonicity::Unknown;
        }

        template <typename VariableType>
        std::map<VariableType, typename MonotonicityResult<VariableType>::Monotonicity> MonotonicityResult<VariableType>::getMonotonicityResult() const {
            return monotonicityResult;
        }

        template <typename VariableType>
        std::string MonotonicityResult<VariableType>::toString() const {
            std::string result;
            for (auto res : getMonotonicityResult()) {
                result += res.first.name();
                switch (res.second) {
                    case MonotonicityResult<VariableType>::Monotonicity::Incr:
                        result += " MonIncr; ";
                        break;
                    case MonotonicityResult<VariableType>::Monotonicity::Decr:
                        result += " MonDecr; ";
                        break;
                    case MonotonicityResult<VariableType>::Monotonicity::Constant:
                        result += " Constant; ";
                        break;
                    case MonotonicityResult<VariableType>::Monotonicity::Not:
                        result += " NotMon; ";
                        break;
                    case MonotonicityResult<VariableType>::Monotonicity::Unknown:
                        result += " Unknown; ";
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Could not get a string from the region monotonicity check result. The case has not been implemented");
                }
            }
            return result;
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::setDone(bool done) {
            this->done = done;
        }

        template <typename VariableType>
        bool MonotonicityResult<VariableType>::isDone() const {
            return done;
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::setSomewhereMonotonicity(bool somewhereMonotonicity) {
            this->somewhereMonotonicity = somewhereMonotonicity;
        }

        template <typename VariableType>
        bool MonotonicityResult<VariableType>::isSomewhereMonotonicity() {
            if(somewhereMonotonicity == false){
                for(auto itr : monotonicityResult){
                    if(itr.second != MonotonicityResult<VariableType>::Monotonicity::Unknown){
                        setSomewhereMonotonicity(true);
                        break;
                    }
                }
            }
            return somewhereMonotonicity;
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::setAllMonotonicity(bool allMonotonicity) {
            this->allMonotonicity = allMonotonicity;
        }

        template <typename VariableType>
        bool MonotonicityResult<VariableType>::isAllMonotonicity() const {
            return allMonotonicity;
        }

        template <typename VariableType>
        std::shared_ptr<MonotonicityResult<VariableType>> MonotonicityResult<VariableType>::copy() const {
            std::shared_ptr<MonotonicityResult<VariableType>> copy = std::make_shared<MonotonicityResult<VariableType>>();
            copy->monotonicityResult = std::map<VariableType, Monotonicity>(monotonicityResult);
            copy->setAllMonotonicity(allMonotonicity);
            copy->setSomewhereMonotonicity(somewhereMonotonicity);
            copy->setDone(done);
            return copy;
        }

        template class MonotonicityResult<storm::RationalFunctionVariable>;
    }
}
