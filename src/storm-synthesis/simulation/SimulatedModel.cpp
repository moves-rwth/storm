#include "storm-synthesis/simulation/SimulatedModel.h"


namespace storm {
    namespace synthesis {

        template<typename ValueType>
        SimulatedModel<ValueType>::SimulatedModel(storm::models::sparse::Pomdp<ValueType> const& pomdp) {
            // TODO
        }

        template<typename ValueType>
        uint_fast64_t SimulatedModel<ValueType>::sampleAction(uint_fast64_t state) {
            // TODO
            return 0;
        }
            
        template<typename ValueType>
        uint_fast64_t SimulatedModel<ValueType>::sampleSuccessor(uint_fast64_t state, uint_fast64_t action) {
            // TODO
            return 0;
        }
        
        template<typename ValueType>
        double SimulatedModel<ValueType>::stateActionRollout(
            uint_fast64_t state, uint_fast64_t action, uint_fast64_t length,
            std::string const& reward_name, double discount_factor
        ) {
            // TODO
            return 0;
        }

        template class SimulatedModel<double>;

    } 
}
