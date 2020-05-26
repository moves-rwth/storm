#ifndef STORM_GENERATOR_NEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_NEXTSTATEGENERATOR_H_

#include <vector>
#include <cstdint>

#include <boost/variant.hpp>

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/sparse/StateStorage.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/sparse/ChoiceOrigins.h"
#include "storm/storage/sparse/StateValuations.h"

#include "storm/builder/BuilderOptions.h"
#include "storm/builder/RewardModelInformation.h"

#include "storm/generator/VariableInformation.h"
#include "storm/generator/CompressedState.h"
#include "storm/generator/StateBehavior.h"

#include "storm/utility/ConstantsComparator.h"

namespace storm {
    namespace generator {
        typedef storm::builder::BuilderOptions NextStateGeneratorOptions;
        
        enum class ModelType {
            DTMC,
            CTMC,
            MDP,
            MA,
            POMDP
        };
        
        template<typename ValueType, typename StateType = uint32_t>
        class NextStateGenerator {
        public:
            typedef std::function<StateType (CompressedState const&)> StateToIdCallback;

            NextStateGenerator(storm::expressions::ExpressionManager const& expressionManager, VariableInformation const& variableInformation, NextStateGeneratorOptions const& options);
            
            /*!
             * Creates a new next state generator. This version of the constructor default-constructs the variable information.
             * Hence, the subclass is responsible for suitably initializing it in its constructor.
             */
            NextStateGenerator(storm::expressions::ExpressionManager const& expressionManager, NextStateGeneratorOptions const& options);
            
            virtual ~NextStateGenerator() = default;
            
            uint64_t getStateSize() const;
            virtual ModelType getModelType() const = 0;
            virtual bool isDeterministicModel() const = 0;
            virtual bool isDiscreteTimeModel() const = 0;
            virtual bool isPartiallyObservable() const = 0;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) = 0;
            
            /// Initializes a builder for state valuations by adding the appropriate variables.
            virtual storm::storage::sparse::StateValuationsBuilder initializeStateValuationsBuilder() const;
            
            void load(CompressedState const& state);
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) = 0;
            bool satisfies(storm::expressions::Expression const& expression) const;
            
            /// Adds the valuation for the currently loaded state to the given builder
            virtual void addStateValuation(storm::storage::sparse::state_type const& currentStateIndex, storm::storage::sparse::StateValuationsBuilder& valuationsBuilder) const;
            
            virtual std::size_t getNumberOfRewardModels() const = 0;
            virtual storm::builder::RewardModelInformation getRewardModelInformation(uint64_t const& index) const = 0;
            
            std::string stateToString(CompressedState const& state) const;

            uint32_t observabilityClass(CompressedState const& state) const;

            virtual storm::models::sparse::StateLabeling label(storm::storage::sparse::StateStorage<StateType> const& stateStorage, std::vector<StateType> const& initialStateIndices = {}, std::vector<StateType> const& deadlockStateIndices = {}) = 0;

            NextStateGeneratorOptions const& getOptions() const;


            
            virtual std::shared_ptr<storm::storage::sparse::ChoiceOrigins> generateChoiceOrigins(std::vector<boost::any>& dataForChoiceOrigins) const;

            /*!
             * Performs a remapping of all values stored by applying the given remapping.
             *
             * @param remapping The remapping to apply.
             */
            void remapStateIds(std::function<StateType(StateType const&)> const& remapping);
            
        protected:
            /*!
             * Creates the state labeling for the given states using the provided labels and expressions.
             */
            storm::models::sparse::StateLabeling label(storm::storage::sparse::StateStorage<StateType> const& stateStorage, std::vector<StateType> const& initialStateIndices, std::vector<StateType> const& deadlockStateIndices, std::vector<std::pair<std::string, storm::expressions::Expression>> labelsAndExpressions);

            virtual storm::storage::BitVector evaluateObservationLabels(CompressedState const& state) const =0;

            void postprocess(StateBehavior<ValueType, StateType>& result);
            
            /// The options to be used for next-state generation.
            NextStateGeneratorOptions options;
            
            /// The expression manager used for evaluating expressions.
            std::shared_ptr<storm::expressions::ExpressionManager const> expressionManager;
            
            /// The expressions that define terminal states.
            std::vector<std::pair<storm::expressions::Expression, bool>> terminalStates;
            
            /// Information about how the variables are packed.
            VariableInformation variableInformation;
            
            /// An evaluator used to evaluate expressions.
            std::unique_ptr<storm::expressions::ExpressionEvaluator<ValueType>> evaluator;
            
            /// The currently loaded state.
            CompressedState const* state;
            
            /// A comparator used to compare constants.
            storm::utility::ConstantsComparator<ValueType> comparator;

            /// The mask to compute the observability class (Constructed upon first use)
            mutable storm::storage::BitVector mask;

            /// The observability classes handed out so far.
            // TODO consider using a BitVectorHashMap for this?
            mutable std::unordered_map<storm::storage::BitVector, uint32_t> observabilityMap;
            /// A state that encodes the outOfBoundsState
            CompressedState outOfBoundsState;

            /// A map that stores the indices of states with overlapping guards.
            boost::optional<std::vector<uint64_t>> overlappingGuardStates;

        };
    }
}

#endif /* STORM_GENERATOR_NEXTSTATEGENERATOR_H_ */
