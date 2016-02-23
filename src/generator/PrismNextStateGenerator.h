#ifndef STORM_GENERATOR_PRISMNEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_PRISMNEXTSTATEGENERATOR_H_

#include "src/generator/NextStateGenerator.h"

#include "src/storage/prism/Program.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType = uint32_t>
        class PrismNextStateGenerator : public NextStateGenerator<ValueType, StateType> {
        public:
            typedef typename NextStateGenerator<ValueType, StateType>::StateToIdCallback StateToIdCallback;
            
            PrismNextStateGenerator(storm::prism::Program const& program);
            
            virtual std::vector<StateType> getInitialStates(StateToIdCallback stateToIdCallback) = 0;
            virtual std::vector<Choice<ValueType>> expand(StateType const& state, StateToIdCallback stateToIdCallback) override;
            virtual ValueType getStateReward(StateType const& state) override;
            
        private:
            // The program used for the generation of next states.
            storm::prism::Program program;
            
            
        };
        
    }
}

#endif /* STORM_GENERATOR_PRISMNEXTSTATEGENERATOR_H_ */