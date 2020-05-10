#pragma once

#include <ostream>
#include <map>

namespace storm {
    namespace analysis {
        template <typename VariableType>
        class MonotonicityResult {
        public:

            /*!
             * The results of monotonicity checking for a single Parameter Region
             */
            enum class Monotonicity {
                Incr, /*!< the result is monotonically increasing */
                Decr, /*!< the result is monotonically decreasing */
                Constant, /*!< the result is constant */
                Not, /*!< the result is not monotonic */
                Unknown /*!< the monotonicity result is unknown */
            };

            MonotonicityResult();

            /*!
             * Adds a new variable with a given Monotonicity to the map
             * @param var the variable that is to be added
             * @param mon the Monotonicity of the variable
             */
            void addMonotonicityResult(VariableType var, Monotonicity mon);


            /*!
             * Updates the Monotonicity of a variable based on its value so far and a new value
             * @param var the variable
             * @param mon the new Monotonicity to be considered
             */
            void updateMonotonicityResult(VariableType var, Monotonicity mon);

            Monotonicity getMonotonicity(VariableType var);

            /*!
             * Returns the results so far
             * @return the variable / Monotonicity map
             */
            std::map<VariableType, Monotonicity> getMonotonicityResult();

            /*!
             * Constructs a string output of all variables and their corresponding Monotonicity
             * @return Results so far
             */
            std::string toString();

            /*!
             * Checks if the result is complete
             */
            bool isDone();

            /*!
             * Checks if there is any variable that is monotone
             */
            bool isSomewhereMonotonicity();

            /*!
             * Checks if all Variables are monotone
             */
            bool isAllMonotonicity();

            /*!
             * Sets the done bool to the given truth value
             */
            void setDone(bool done = true);

            /*!
             * Sets the somewhereMonotonicity bool to the given truth value
             */
            void setSomewhereMonotonicity(bool done = true);

            /*!
             * Sets the allMonotonicity bool to the given truth value
             */
            void setAllMonotonicity(bool done = true);

            /*!
             * Constructs a new MonotonicityResult object that is a copy of the current one
             * @return Pointer to the copy
             */
            MonotonicityResult<VariableType>* copy();

        private:
            std::map<VariableType, Monotonicity> monotonicityResult;
            bool done;
            bool somewhereMonotonicity;
            bool allMonotonicity;

        };

    }
}

