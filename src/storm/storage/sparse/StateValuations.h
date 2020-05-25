#pragma once

#include <cstdint>
#include <string>

#include "storm/storage/sparse/StateType.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/models/sparse/StateAnnotation.h"
#include "storm/adapters/JsonAdapter.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            class StateValuations;
            class StateValuationsBuilder;
            
            class StateValuation {
            public:
                typedef storm::json<storm::RationalNumber> Json;
                StateValuation() = default;
                StateValuation(std::vector<bool>&& booleanValues, std::vector<int64_t>&& integerValues, std::vector<storm::RationalNumber>&& rationalValues);

                bool getBooleanValue(StateValuations const& valuations, storm::expressions::Variable const& booleanVariable) const;
                int64_t const& getIntegerValue(StateValuations const& valuations, storm::expressions::Variable const& integerVariable) const;
                storm::RationalNumber const& getRationalValue(StateValuations const& valuations, storm::expressions::Variable const& rationalVariable) const;
                
                // Returns true, if this valuation does not contain any value.
                bool isEmpty() const;
                
                /*!
                 * Returns a string representation of the valuation.
                 *
                 * @param selectedVariables If given, only the informations for the variables in this set are processed.
                 * @return The string representation.
                 */
                std::string toString(StateValuations const& valuations, bool pretty = true, boost::optional<std::set<storm::expressions::Variable>> const& selectedVariables = boost::none) const;
                
                /*!
                 * Returns a JSON representation of this valuation
                 * @param selectedVariables If given, only the informations for the variables in this set are processed.
                 * @return
                 */
                Json toJson(StateValuations const& valuations, boost::optional<std::set<storm::expressions::Variable>> const& selectedVariables = boost::none) const;
                
            private:
                // Asserts whether the variable and value counts for each type match.
                bool assertValuations(StateValuations const& valuations) const;
                
                std::vector<bool> booleanValues;
                std::vector<int64_t> integerValues;
                std::vector<storm::RationalNumber> rationalValues;
            };
            
            // A structure holding information about the reachable state space that can be retrieved from the outside.
            class StateValuations : public storm::models::sparse::StateAnnotation {
            public:
                friend class StateValuation;
                friend class StateValuationsBuilder;

                StateValuations() = default;
                
                virtual ~StateValuations() = default;
                virtual std::string getStateInfo(storm::storage::sparse::state_type const& state) const override;
                
                /*!
                 * Returns the valuation at the specific state
                 */
                StateValuation& operator[](storm::storage::sparse::state_type const& state);
                StateValuation const& operator[](storm::storage::sparse::state_type const& state) const;
                
                
                // Returns the (current) number of states that this object describes.
                uint_fast64_t getNumberOfStates() const;
                
                /*
                 * Derive new state valuations from this by selecting the given states.
                 */
                StateValuations selectStates(storm::storage::BitVector const& selectedStates) const;
                
                /*
                 * Derive new state valuations from this by selecting the given states.
                 * If an invalid state index is selected, the corresponding valuation will be empty.
                 */
                StateValuations selectStates(std::vector<storm::storage::sparse::state_type> const& selectedStates) const;
                
                
            private:
                StateValuations(std::map<storm::expressions::Variable, uint64_t> const& variableToIndexMap, std::vector<StateValuation>&& valuations);
                std::map<storm::expressions::Variable, uint64_t> variableToIndexMap;
                // A mapping from state indices to their variable valuations.
                std::vector<StateValuation> valuations;
                
            };
            
            class StateValuationsBuilder {
            public:
                StateValuationsBuilder();
                
                /*! Adds a new variable to keep track of for the state valuations.
                 *! All variables need to be added before adding new states.
                 */
                void addVariable(storm::expressions::Variable const& variable);
                
                /*!
                 * Adds a new state.
                 * The variable values have to be given in the same order as the variables have been added.
                 * The number of given variable values for each type needs to match the number of added variables.
                 * After calling this method, no more variables should be added.
                 */
                 void addState(storm::storage::sparse::state_type const& state, std::vector<bool>&& booleanValues = {}, std::vector<int64_t>&& integerValues = {}, std::vector<storm::RationalNumber>&& rationalValues = {});
                 
                 /*!
                  * Creates the finalized state valuations object.
                  */
                 StateValuations build(std::size_t totalStateCount);

            private:
                StateValuations currentStateValuations;
                uint64_t booleanVarCount;
                uint64_t integerVarCount;
                uint64_t rationalVarCount;
            };
        }
    }
}
