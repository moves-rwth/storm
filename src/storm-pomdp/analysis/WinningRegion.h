#pragma once

#include <vector>
#include "storm/storage/BitVector.h"

namespace storm {
    namespace pomdp {
        class WinningRegion {
        public:
            WinningRegion(std::vector<uint64_t> const& observationSizes = {});

            bool update(uint64_t observation, storm::storage::BitVector const& winning);
            bool query(uint64_t observation, storm::storage::BitVector const& currently) const;

            bool observationIsWinning(uint64_t observation) const;
            storm::expressions::Expression extensionExpression(uint64_t observation, std::vector<storm::expressions::Expression>& varsForStates) const;


                uint64_t getStorageSize() const;
            uint64_t getNumberOfObservations() const;
            void print() const;
        private:
            std::vector<std::vector<storm::storage::BitVector>> winningRegion;
            std::vector<uint64_t> observationSizes;
        };
    }
}