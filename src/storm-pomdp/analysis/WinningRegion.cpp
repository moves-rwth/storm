#include <iostream>
#include "storm-pomdp/analysis/WinningRegion.h"

namespace storm {
namespace pomdp {
    WinningRegion::WinningRegion(std::vector<uint64_t> const& observationSizes) : observationSizes(observationSizes)
    {
        for (uint64_t i = 0; i < observationSizes.size(); ++i) {
            winningRegion.push_back(std::vector<storm::storage::BitVector>());
        }
    }

    void WinningRegion::update(uint64_t observation, storm::storage::BitVector const& winning) {
        std::vector<storm::storage::BitVector> newWinningSupport = std::vector<storm::storage::BitVector>();
        bool changed = false;
        for (auto const& support : winningRegion[observation]) {
            if (winning.isSubsetOf(support)) {
                // This new winning support is already covered.
                return;
            }
            if(support.isSubsetOf(winning)) {
                // This new winning support extends the previous support, thus the previous support is now spurious
                changed = true;
            } else {
                newWinningSupport.push_back(support);
            }
        }

        // only if changed.
        if (changed) {
            newWinningSupport.push_back(winning);
            winningRegion[observation] = newWinningSupport;
        } else {
            winningRegion[observation].push_back(winning);
        }

    }

    bool WinningRegion::query(uint64_t observation, storm::storage::BitVector const& currently) const {
        for(storm::storage::BitVector winning : winningRegion[observation]) {
            if(currently.isSubsetOf(winning)) {
                return true;
            }
        }
        return false;
    }

    /**
     * If we observe this observation, do we surely win?
     * @param observation
     * @return yes, if all supports for this observation are winning.
     */
    bool WinningRegion::observationIsWinning(uint64_t observation) const {
        return winningRegion[observation].size() == 1 && winningRegion[observation].front().full();
    }

    void WinningRegion::print() const {
        uint64_t observation = 0;
        for (auto const& winningSupport : winningRegion) {
            std::cout << "***** observation" << observation << std::endl;
            for (auto const& support : winningSupport) {
                std::cout << " " << support;
            }
            std::cout << std::endl;
        }
    }

    /**
     * How many different observations are there?
     * @return
     */
    uint64_t WinningRegion::getNumberOfObservations() const {
        return observationSizes.size();
    }

    uint64_t WinningRegion::getStorageSize() const {
        uint64_t result = 0;
        for (uint64_t i = 0; i < getNumberOfObservations(); ++i) {
            result += winningRegion[i].size() * observationSizes[i];
        }
        return result;
    }


}
}