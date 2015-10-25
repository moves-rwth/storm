#include "shortestPaths.h"
#include "graph.h"

namespace storm {
    namespace utility {
        namespace shortestPaths {
            template <typename T>
            ShortestPathsGenerator<T>::ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model) : model(model) {
                // FIXME: does this create a copy? I don't need one, so I should avoid that
                transitionMatrix = model->getTransitionMatrix();

                // TODO: init various things we'll need later
                // - predecessors
                computePredecessors();
                // - Dijkstra (giving us SP-predecessors, SP-distances)
                performDijkstra();
                // - SP-successors
                computeSPSuccessors();
                // - shortest paths
                initializeShortestPaths();
            }

            template <typename T>
            ShortestPathsGenerator<T>::~ShortestPathsGenerator() {
            }

            template <typename T>
            void ShortestPathsGenerator<T>::computePredecessors() {
                graphPredecessors.resize(model->getNumberOfStates());

                for (int i = 0; i < transitionMatrix.getRowCount(); i++) {
                    // what's the difference? TODO
                    //auto foo = transitionMatrix.getRowGroupEntryCount(i);
                    //auto bar = transitionMatrix.getRowGroupSize(i);

                    for (auto transition : transitionMatrix.getRowGroup(i)) {
                        graphPredecessors[transition.getColumn()].push_back(i);
                    }
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::performDijkstra() {
                auto result = storm::utility::graph::performDijkstra(*model, transitionMatrix, model->getInitialStates());
                shortestPathDistances = result.first;
                shortestPathPredecessors = result.second;
                // FIXME: fix bad predecessor result for initial states (either here, or by fixing the Dijkstra)
            }

            template <typename T>
            void ShortestPathsGenerator<T>::computeSPSuccessors() {
                shortestPathSuccessors.resize(model->getNumberOfStates());

                for (int i = 0; i < model->getNumberOfStates(); i++) {
                    state_t predecessor = shortestPathPredecessors[i];
                    shortestPathSuccessors[predecessor].push_back(i);
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::initializeShortestPaths() {}
        }
    }
}
