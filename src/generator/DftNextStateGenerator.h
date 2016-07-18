#ifndef STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_

#include "src/generator/NextStateGenerator.h"
#include "src/storage/dft/DFT.h"

#include "src/utility/ConstantsComparator.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType = uint32_t>
        class DftNextStateGenerator : public NextStateGenerator<ValueType, std::shared_ptr<storm::storage::DFTState<ValueType>>, StateType> {
        public:
            typedef typename NextStateGenerator<ValueType, std::shared_ptr<storm::storage::DFTState<ValueType>>, StateType>::StateToIdCallback StateToIdCallback;
            
            DftNextStateGenerator(storm::storage::DFT<ValueType> const& dft);
                        
            virtual bool isDeterministicModel() const override;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) override;

            virtual void load(std::shared_ptr<storm::storage::DFTState<ValueType>> const& state) override;
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) override;
            virtual bool satisfies(storm::expressions::Expression const& expression) const override;

        private:
            
            // The program used for the generation of next states.
            storm::storage::DFT<ValueType> const& mDft;
            
            CompressedState const* state;
            
            // A comparator used to compare constants.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
        
    }
}

#endif /* STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_ */