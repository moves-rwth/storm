#include "MonotonicityResult.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

namespace storm {
    namespace analysis {

        template <typename VariableType>
        MonotonicityResult<VariableType>::MonotonicityResult(){
            this->done = false;
            this->somewhereMonotonicity = false;
            this->allMonotonicity = true;
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::addMonotonicityResult(VariableType var,
                                                                     MonotonicityResult<VariableType>::Monotonicity mon) {
            monotonicityResult.insert(std::pair<VariableType, MonotonicityResult<VariableType>::Monotonicity>(std::move(var), std::move(mon)));
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::setMonotonicityResult(VariableType var,
                                                                     MonotonicityResult<VariableType>::Monotonicity mon) {
            monotonicityResult[var] = mon;
        }

        template <typename VariableType>
        void MonotonicityResult<VariableType>::updateMonotonicityResult(VariableType var,
                                                                        MonotonicityResult<VariableType>::Monotonicity mon) {

            if(mon == MonotonicityResult<VariableType>::Monotonicity::Not){
                mon = MonotonicityResult<VariableType>::Monotonicity::Unknown;
            }

            if (monotonicityResult.find(var) == monotonicityResult.end()){
                addMonotonicityResult(std::move(var), std::move(mon));
            }
            else{
                auto monRes = monotonicityResult[var];
                if (monRes == MonotonicityResult<VariableType>::Monotonicity::Unknown || monRes == mon || mon == MonotonicityResult<VariableType>::Monotonicity::Constant){
                    return;
                }
                else if (mon == MonotonicityResult<VariableType>::Monotonicity::Unknown || monRes == MonotonicityResult<VariableType>::Monotonicity::Constant ){
                    monotonicityResult[var] = mon;
                }
                else{
                    monotonicityResult[var] = MonotonicityResult<VariableType>::Monotonicity::Unknown;
                }
            }
            if(monotonicityResult[var] == MonotonicityResult<VariableType>::Monotonicity::Unknown){
                setAllMonotonicity(false);
                setSomewhereMonotonicity(false);
            }
            else{
                setSomewhereMonotonicity(true);
            }

        }

        template <typename VariableType>
        std::map<VariableType, typename MonotonicityResult<VariableType>::Monotonicity> MonotonicityResult<VariableType>::getMonotonicityResult() {
            return monotonicityResult;
        }

        template <typename VariableType>
        std::string MonotonicityResult<VariableType>::toString() {
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
        bool MonotonicityResult<VariableType>::isDone() {
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
        bool MonotonicityResult<VariableType>::isAllMonotonicity() {
            return allMonotonicity;
        }

        template <typename VariableType>
        std::ostream& operator<<(std::ostream& os, MonotonicityResult<VariableType> const& regionMonotonicityResult) {
            os << regionMonotonicityResult.toString();
            return os;
        }

        template <typename VariableType>
        MonotonicityResult<VariableType>* MonotonicityResult<VariableType>::copy(){
            MonotonicityResult<VariableType>* copy = new MonotonicityResult<VariableType>();
            copy->monotonicityResult = *(new std::map<VariableType, Monotonicity>(monotonicityResult));
            return copy;
        }


        template class MonotonicityResult<storm::RationalFunctionVariable>;
    }
}
