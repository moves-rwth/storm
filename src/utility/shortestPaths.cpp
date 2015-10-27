#include <queue>
#include "shortestPaths.h"
#include "graph.h"
#include "constants.h"

namespace storm {
    namespace utility {
        namespace shortestPaths {
            template <typename T>
            ShortestPathsGenerator<T>::ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model) : model(model) {
                // FIXME: does this create a copy? I don't need one, so I should avoid that
                transitionMatrix = model->getTransitionMatrix();
                numStates = model->getNumberOfStates();

                computePredecessors();

                // gives us SP-predecessors, SP-distances
                performDijkstra();

                computeSPSuccessors();

                // constructs the recursive shortest path representations
                initializeShortestPaths();
            }

            template <typename T>
            ShortestPathsGenerator<T>::~ShortestPathsGenerator() {
            }

            template <typename T>
            void ShortestPathsGenerator<T>::computePredecessors() {
                graphPredecessors.resize(numStates);

                assert(numStates == transitionMatrix.getRowCount());
                for (state_t i = 0; i < numStates; i++) {
                    for (auto transition : transitionMatrix.getRowGroup(i)) {
                        graphPredecessors[transition.getColumn()].push_back(i);
                    }
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::performDijkstra() {
                // the existing Dijkstra isn't working anyway AND
                // doesn't fully meet our requirements, so let's roll our own

                T inftyDistance = storm::utility::zero<T>();
                T zeroDistance = storm::utility::one<T>();
                shortestPathDistances.resize(numStates, inftyDistance);
                shortestPathPredecessors.resize(numStates, boost::optional<state_t>());

                // set serves as priority queue with unique membership
                // default comparison on pair actually works fine if distance is the first entry
                std::set<std::pair<T, state_t>> dijkstraQueue;

                for (state_t initialState : model->getInitialStates()) {
                    shortestPathDistances[initialState] = zeroDistance;
                    dijkstraQueue.emplace(zeroDistance, initialState);
                }

                while (!dijkstraQueue.empty()) {
                    state_t currentNode = (*dijkstraQueue.begin()).second;
                    dijkstraQueue.erase(dijkstraQueue.begin());

                    for (auto const& transition : transitionMatrix.getRowGroup(currentNode)) {
                        state_t otherNode = transition.getColumn();

                        // note that distances are probabilities, thus they are multiplied and larger is better
                        T alternateDistance = shortestPathDistances[currentNode] * transition.getValue();
                        if (alternateDistance > shortestPathDistances[otherNode]) {
                            shortestPathDistances[otherNode] = alternateDistance;
                            shortestPathPredecessors[otherNode] = boost::optional<state_t>(currentNode);
                            dijkstraQueue.emplace(alternateDistance, otherNode);
                        }
                    }
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::computeSPSuccessors() {
                shortestPathSuccessors.resize(numStates);

                for (state_t i = 0; i < numStates; i++) {
                    if (shortestPathPredecessors[i]) {
                        state_t predecessor = shortestPathPredecessors[i].get();
                        shortestPathSuccessors[predecessor].push_back(i);
                    }
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::initializeShortestPaths() {
                kShortestPaths.resize(numStates);
                candidatePaths.resize(numStates);

                // BFS in Dijkstra-SP order
                std::queue<state_t> bfsQueue;
                for (state_t initialState : model->getInitialStates()) {
                    bfsQueue.push(initialState);
                }

                while (!bfsQueue.empty()) {
                    state_t currentNode = bfsQueue.front();
                    bfsQueue.pop();

                    if (!kShortestPaths[currentNode].empty()) {
                        continue; // already visited
                    }

                    for (state_t successorNode : shortestPathSuccessors[currentNode]) {
                        bfsQueue.push(successorNode);
                    }

                    // note that `shortestPathPredecessor` may not be present
                    // if current node is an initial state
                    // in this case, the boost::optional copy of an uninitialized optional is hopefully also uninitialized
                    kShortestPaths[currentNode].push_back(Path<T> {
                            shortestPathPredecessors[currentNode],
                            1,
                            shortestPathDistances[currentNode]
                    });
                }
            }
        }
    }
}
