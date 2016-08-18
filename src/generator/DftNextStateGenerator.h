#ifndef STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_

#include "src/generator/NextStateGenerator.h"
#include "src/storage/dft/DFT.h"

#include "src/utility/ConstantsComparator.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType = uint_fast64_t>
        class DftNextStateGenerator : public NextStateGenerator<ValueType, std::shared_ptr<storm::storage::DFTState<ValueType>>, StateType> {

            using DFTStatePointer = std::shared_ptr<storm::storage::DFTState<ValueType>>;

        public:
            typedef typename NextStateGenerator<ValueType, DFTStatePointer, StateType>::StateToIdCallback StateToIdCallback;
            
            DftNextStateGenerator(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTStateGenerationInfo const& stateGenerationInfo);
                        
            virtual bool isDeterministicModel() const override;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) override;

            virtual void load(DFTStatePointer const& state) override;
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) override;
            virtual bool satisfies(storm::expressions::Expression const& expression) const override;

        private:
            
            // The program used for the generation of next states.
            storm::storage::DFT<ValueType> const& mDft;
            
            storm::storage::DFTStateGenerationInfo const& mStateGenerationInfo;
            
            DFTStatePointer const state;
            
            // A comparator used to compare constants.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
        
    }
}

#endif /* STORM_GENERATOR_DFTNEXTSTATEGENERATOR_H_ */