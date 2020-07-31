#pragma once

#include <ostream>
#include <map>
#include <memory>

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

            /*!
             * Constructs a new MonotonicityResult object.
             */
            MonotonicityResult();

            /*!
             * Adds a new variable with a given Monotonicity to the map.
             *
             * @param var The variable that is to be added.
             * @param mon The Monotonicity of the variable.
             */
            void addMonotonicityResult(VariableType var, Monotonicity mon);


            /*!
             * Updates the Monotonicity of a variable based on its value so far and a new value.
             *
             * @param var The variable.
             * @param mon The new Monotonicity to be considered.
             */
            void updateMonotonicityResult(VariableType var, Monotonicity mon);

            /*!
             * Returns the  current monotonicity of a given parameter.
             *
             * @param var The parameter.
             * @return Incr, Decr, Constant, Not or Unknown.
             */
            Monotonicity getMonotonicity(VariableType var) const;

            /*!
             * Returns the results so far.
             *
             * @return The parameter / Monotonicity map
             */
            std::map<VariableType, Monotonicity> getMonotonicityResult() const;

            /*!
             * Constructs a string output of all variables and their corresponding Monotonicity
             *
             * @return Results so far
             */
            std::string toString() const;

            /*!
             * Checks if the result is complete
             */
            bool isDone() const;

            /*!
             * Checks if there is any variable that is monotone
             */
            bool isSomewhereMonotonicity();

            /*!
             * Returns if all Variables are monotone
             */
            bool isAllMonotonicity() const;

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
             *
             * @return Pointer to the copy
             */
            std::shared_ptr<MonotonicityResult<VariableType>> copy() const;

        private:
            std::map<VariableType, Monotonicity> monotonicityResult;
            bool done;
            bool somewhereMonotonicity;
            bool allMonotonicity;

        };

    }
}

