#include <iostream>
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm-pomdp/analysis/WinningRegion.h"

namespace storm {
namespace pomdp {
    WinningRegion::WinningRegion(std::vector<uint64_t> const& observationSizes) : observationSizes(observationSizes)
    {
        for (uint64_t i = 0; i < observationSizes.size(); ++i) {
            winningRegion.push_back(std::vector<storm::storage::BitVector>());
        }
    }

    void WinningRegion::setObservationIsWinning(uint64_t observation) {
        winningRegion[observation] = { storm::storage::BitVector(observationSizes[observation], true) };
    }

//    void WinningRegion::addTargetState(uint64_t observation, uint64_t offset) {
//        std::vector<storm::storage::BitVector> newWinningSupport = std::vector<storm::storage::BitVector>();
//        bool changed = true;
//        for (auto const& support : winningRegion[observation]) {
//            newWinningSupport.push_back(storm::storage::BitVector(support));
//            if(!support.get(offset)) {
//                changed = true;
//                newWinningSupport.back().set(offset);
//            }
//        }
//
//
//    }

    bool WinningRegion::update(uint64_t observation, storm::storage::BitVector const& winning) {
        std::vector<storm::storage::BitVector> newWinningSupport = std::vector<storm::storage::BitVector>();
        bool changed = false;
        for (auto const& support : winningRegion[observation]) {
            if (winning.isSubsetOf(support)) {
                // This new winning support is already covered.
                return false;
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
        return true;

    }

    bool WinningRegion::query(uint64_t observation, storm::storage::BitVector const& currently) const {
        for(storm::storage::BitVector winning : winningRegion[observation]) {
            if(currently.isSubsetOf(winning)) {
                return true;
            }
        }
        return false;
    }

    storm::expressions::Expression WinningRegion::extensionExpression(uint64_t observation, std::vector<storm::expressions::Expression>& varsForStates) const {
        std::vector<storm::expressions::Expression> expressionForEntry;

        for(auto const& winningForObservation : winningRegion[observation]) {
            if (winningForObservation.full()) {
                assert(winningRegion[observation].size() == 1);
                return varsForStates.front().getManager().boolean(false);
            }
            std::vector<storm::expressions::Expression> subexpr;
            std::vector<storm::expressions::Expression> leftHandSides;
            assert(varsForStates.size() == winningForObservation.size());
            for(uint64_t i = 0; i < varsForStates.size(); ++i) {
                if (winningForObservation.get(i)) {
                    leftHandSides.push_back(varsForStates[i]);
                } else {
                    subexpr.push_back(varsForStates[i]);
                }
            }
            storm::expressions::Expression rightHandSide = storm::expressions::disjunction(subexpr);
            for(auto const& lhs : leftHandSides) {
                expressionForEntry.push_back(storm::expressions::implies(lhs,rightHandSide));
            }
            expressionForEntry.push_back(storm::expressions::disjunction(varsForStates));

        }
        return storm::expressions::conjunction(expressionForEntry);
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
        std::vector<uint64_t> winningObservations;
        for (auto const& winningSupport : winningRegion) {
            if (observationIsWinning(observation)) {
                winningObservations.push_back(observation);
            } else {
                std::cout << "***** observation" << observation << std::endl;
                for (auto const& support : winningSupport) {
                    std::cout << " " << support;
                }
                std::cout << std::endl;
            }
            observation++;
        }
        std::cout << " and " << winningObservations.size() << " winning observations: (";
        for (auto const& obs : winningObservations) {
            std::cout << obs << " ";
        }
        std::cout << ")" << std::endl;
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