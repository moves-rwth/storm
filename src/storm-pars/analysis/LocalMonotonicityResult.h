#ifndef STORM_LOCALMONOTONICITYRESULT_H
#define STORM_LOCALMONOTONICITYRESULT_H

#include <vector>
#include "storm-pars/analysis/MonotonicityResult.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/storage/BitVector.h"

namespace storm {
    namespace analysis {
        template <typename VariableType>
        class LocalMonotonicityResult {

        public:
            // TODO: @Svenja, could you update the documentation of the public methods?
            typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;

            LocalMonotonicityResult(uint_fast64_t numberOfStates);

            Monotonicity getMonotonicity(uint_fast64_t state, VariableType var) const;

            void setMonotonicity(uint_fast64_t state, VariableType var, Monotonicity mon);

            std::shared_ptr<MonotonicityResult<VariableType>> getGlobalMonotonicityResult() const;

            /*!
             * Constructs a new LocalMonotonicityResult object that is a copy of the current one
             * @return Pointer to the copy
             */
            std::shared_ptr<LocalMonotonicityResult<VariableType>> copy();

            bool isDone() const;

            bool isNoMonotonicity() const;

            void setConstant(uint_fast64_t state);

            void setIndexMinimize(int index);
            void setIndexMaximize(int index);
            int getIndexMinimize() const;
            int getIndexMaximize() const;

            /*!
             * Constructs a string output of all variables and their corresponding Monotonicity
             * @return Results so far
             */
            std::string toString() const;


        private:
            std::vector<std::shared_ptr<MonotonicityResult<VariableType>>> stateMonRes;

            std::shared_ptr<MonotonicityResult<VariableType>> globalMonotonicityResult;

            void setMonotonicityResult(uint_fast64_t state, std::shared_ptr<MonotonicityResult<VariableType>> monRes);

            void setGlobalMonotonicityResult(std::shared_ptr<MonotonicityResult<VariableType>> monRes);

            void setStatesMonotone(storm::storage::BitVector statesMonotone);

            storm::storage::BitVector statesMonotone;
            bool done = false;

            int indexMinimize = -1;
            int indexMaximize = -1;
            std::shared_ptr<MonotonicityResult<VariableType>> dummyPointer;

        };
    }
}

#endif //STORM_LOCALMONOTONICITYRESULT_H
