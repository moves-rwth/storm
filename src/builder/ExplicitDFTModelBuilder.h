#ifndef EXPLICITDFTMODELBUILDER_H
#define	EXPLICITDFTMODELBUILDER_H

#include "../storage/dft/DFT.h"

#include <unordered_set>

namespace storm {
    namespace builder {
        class ExplicitDFTModelBuilder {
            storm::storage::DFT const &mDft;
            std::unordered_set<storm::storage::DFTState> mStates;
            size_t newIndex = 0;
            //std::stack<std::shared_ptr<storm::storage::DFTState>> mStack;

        public:
            ExplicitDFTModelBuilder(storm::storage::DFT const &dft) : mDft(dft) {

            }

            void buildCTMC() {
                // Construct starting start
                storm::storage::DFTState state(mDft, newIndex++);
                mStates.insert(state);
                // Begin model generation
                exploreStateSuccessors(state);
                std::cout << "Generated " << mStates.size() << " states" << std::endl;
            }

        private:
            void exploreStateSuccessors(storm::storage::DFTState const &state);
        };
    }
}

#endif	/* EXPLICITDFTMODELBUILDER_H */