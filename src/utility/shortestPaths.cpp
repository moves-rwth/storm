#include <queue>
#include <set>
#include "shortestPaths.h"
#include "graph.h"

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

                candidatePaths.resize(numStates);
            }

            template <typename T>
            ShortestPathsGenerator<T>::~ShortestPathsGenerator() {
            }

            template <typename T>
            void ShortestPathsGenerator<T>::computePredecessors() {
                auto rowCount = transitionMatrix.getRowCount();
                assert(numStates == rowCount);

                graphPredecessors.resize(rowCount);

                for (state_t i = 0; i < rowCount; i++) {
                    for (auto const& transition : transitionMatrix.getRowGroup(i)) {
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
                std::set<std::pair<T, state_t>, std::greater<std::pair<T, state_t>>> dijkstraQueue;

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

            template <typename T>
            void ShortestPathsGenerator<T>::computeNextPath(state_t node, unsigned long k) {
                assert(kShortestPaths[node].size() == k - 1); // if not, the previous SP must not exist

                if (k == 2) {
                    Path<T> shortestPathToNode = kShortestPaths[node][1 - 1]; // never forget index shift :-|

                    for (state_t predecessor : graphPredecessors[node]) {
                        // add shortest paths to predecessors plus edge to current node
                        Path<T> pathToPredecessorPlusEdge = {
                            boost::optional<state_t>(predecessor),
                            1,
                            shortestPathDistances[predecessor] * getEdgeDistance(predecessor, node)
                        };
                        candidatePaths[node].insert(pathToPredecessorPlusEdge);

                        // ... but not the actual shortest path
                        auto it = find(candidatePaths[node].begin(), candidatePaths[node].end(), shortestPathToNode);
                        if (it != candidatePaths[node].end()) {
                            candidatePaths[node].erase(it);
                        }
                    }
                }

                if (k > 2 || !isInitialState(node)) {
                    // the (k-1)th shortest path (i.e., one better than the one we want to compute)
                    Path<T> previousShortestPath = kShortestPaths[node][k - 1 - 1]; // oh god, I forgot index shift AGAIN

                    // the predecessor node on that path
                    state_t predecessor = previousShortestPath.predecessorNode.get();
                    // the path to that predecessor was the `tailK`-shortest
                    unsigned long tailK = previousShortestPath.predecessorK;

                    // i.e. source ~~tailK-shortest path~~> predecessor --> node

                    // compute one-worse-shortest path to the predecessor (if it hasn't yet been computed)
                    if (kShortestPaths[predecessor].size() < tailK + 1) {
                        // TODO: investigate recursion depth and possible iterative alternative
                        computeNextPath(predecessor, tailK + 1);
                    }

                    if (kShortestPaths[predecessor].size() >= tailK + 1) {
                        // take that path, add an edge to the current node; that's a candidate
                        Path<T> pathToPredecessorPlusEdge = {
                                boost::optional<state_t>(predecessor),
                                tailK + 1,
                                kShortestPaths[predecessor][tailK + 1 - 1].distance * getEdgeDistance(predecessor, node)
                        };
                        candidatePaths[node].insert(pathToPredecessorPlusEdge);
                    }
                    // else there was no path; TODO: does this need handling?
                }

                if (!candidatePaths[node].empty()) {
                    Path<T> minDistanceCandidate = *(candidatePaths[node].begin());
                    for (auto path : candidatePaths[node]) {
                        if (path.distance > minDistanceCandidate.distance) {
                            minDistanceCandidate = path;
                        }
                    }

                    candidatePaths[node].erase(find(candidatePaths[node].begin(), candidatePaths[node].end(), minDistanceCandidate));
                    kShortestPaths[node].push_back(minDistanceCandidate);
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::getKShortest(state_t node, unsigned long k) {
                unsigned long alreadyComputedK = kShortestPaths[node].size();

//                std::cout << std::endl << "--> DEBUG: Dijkstra SP to " << node << ":" << std::endl;
//                printKShortestPath(node, 1);
//                std::cout << "---------" << std::endl;

                for (unsigned long nextK = alreadyComputedK + 1; nextK <= k; nextK++) {
                    computeNextPath(node, nextK);
                    if (kShortestPaths[node].size() < nextK) {
                        std::cout << std::endl << "--> DEBUG: Last path: k=" << (nextK - 1) << ":" << std::endl;
                        printKShortestPath(node, nextK - 1);
                        std::cout << "---------" << "No other path exists!" << std::endl;
                        return;
                    }
                }

                std::cout << std::endl << "--> DEBUG: Finished. " << k << "-shortest path:" << std::endl;
                printKShortestPath(node, k);
                std::cout << "---------" << std::endl;
            }

            template <typename T>
            void ShortestPathsGenerator<T>::printKShortestPath(state_t targetNode, unsigned long k, bool head) {
                // note the index shift! risk of off-by-one
                Path<T> p = kShortestPaths[targetNode][k - 1];

                if (head) {
                    std::cout << "Path (reversed), dist (prob)=" << p.distance << ": [";
                }

                std::cout << " " << targetNode;

                if (p.predecessorNode) {
                    printKShortestPath(p.predecessorNode.get(), p.predecessorK, false);
                } else {
                    std::cout << " ]" << std::endl;
                }
            }
        }
    }
}
