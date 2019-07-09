#include "storm/models/sparse/MarkovAutomaton.h"

namespace storm {
    namespace transformer {
        /**
         * Transformer for eliminating chains of non-Markovian states (instantaneous path fragment leading to the same outcome) from Markov Automata
         */
        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class NonMarkovianChainTransformer {
        public:
            /**
             * Generates a model with the same basic behavior as the input, but eliminates non-Markovian chains.
             * If no non-determinism occurs, a CTMC is generated.
             *
             * @param ma the input Markov Automaton
             * @param preserveLabels if set, the procedure considers the labels of non-Markovian states when eliminating states
             * @return a reference to the new Mmodel after eliminating non-Markovian states
             */
            static std::shared_ptr<
                    models::sparse::Model < ValueType, RewardModelType>> eliminateNonmarkovianStates(std::shared_ptr<
                    models::sparse::MarkovAutomaton < ValueType, RewardModelType>> ma,
            bool preserveLabels = true
            );
        };
    }
}

