#include "storm/modelchecker/exploration/Statistics.h"

#include "storm/modelchecker/exploration/ExplorationInformation.h"

namespace storm {
    namespace modelchecker {
        namespace exploration_detail {
            
            template<typename StateType, typename ValueType>
            Statistics<StateType, ValueType>::Statistics() : pathsSampled(0), pathsSampledSinceLastPrecomputation(0), explorationSteps(0), explorationStepsSinceLastPrecomputation(0), maxPathLength(0), numberOfTargetStates(0), numberOfExploredStates(0), numberOfPrecomputations(0), ecDetections(0), failedEcDetections(0), totalNumberOfEcDetected(0) {
                // Intentionally left empty.
            }
            
            template<typename StateType, typename ValueType>
            void Statistics<StateType, ValueType>::explorationStep() {
                ++explorationSteps;
                ++explorationStepsSinceLastPrecomputation;
            }
            
            template<typename StateType, typename ValueType>
            void Statistics<StateType, ValueType>::sampledPath() {
                ++pathsSampled;
                ++pathsSampledSinceLastPrecomputation;
            }
            
            template<typename StateType, typename ValueType>
            void Statistics<StateType, ValueType>::updateMaxPathLength(std::size_t const& currentPathLength) {
                maxPathLength = std::max(maxPathLength, currentPathLength);
            }
            
            template<typename StateType, typename ValueType>
            void Statistics<StateType, ValueType>::printToStream(std::ostream& out, ExplorationInformation<StateType, ValueType> const& explorationInformation) const {
                out << std::endl << "Exploration statistics:" << std::endl;
                out << "Discovered states: " << explorationInformation.getNumberOfDiscoveredStates() << " (" << numberOfExploredStates << " explored, " << explorationInformation.getNumberOfUnexploredStates() << " unexplored, " << numberOfTargetStates << " target)" << std::endl;
                out << "Exploration steps: " << explorationSteps << std::endl;
                out << "Sampled paths: " << pathsSampled << std::endl;
                out << "Maximal path length: " << maxPathLength << std::endl;
                out << "Precomputations: " << numberOfPrecomputations << std::endl;
                out << "EC detections: " << ecDetections << " (" << failedEcDetections << " failed, " << totalNumberOfEcDetected << " EC(s) detected)" << std::endl;
            }
         
            template struct Statistics<uint32_t, double>;
            
        }
    }
}
