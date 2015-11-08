#include <queue>
#include <set>
#include "shortestPaths.h"
#include "graph.h"

namespace storm {
    namespace utility {
        namespace ksp {
            template <typename T>
            ShortestPathsGenerator<T>::ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model,
                                                              state_list_t const& targets) : model(model), targets(targets) {
                // FIXME: does this create a copy? I don't need one, so I should avoid that
                transitionMatrix = model->getTransitionMatrix();
                numStates = model->getNumberOfStates() + 1; // one more for meta-target

                metaTarget = numStates - 1; // first unused state number
                targetSet = std::unordered_set<state_t>(targets.begin(), targets.end());

                computePredecessors();

                // gives us SP-predecessors, SP-distances
                performDijkstra();

                computeSPSuccessors();

                // constructs the recursive shortest path representations
                initializeShortestPaths();

                candidatePaths.resize(numStates);
            }

            // several alternative ways to specify the targets are provided,
            // each converts the targets and delegates to the ctor above
            // I admit it's kind of ugly, but actually pretty convenient (and I've wanted to try C++11 delegation)
            template <typename T>
            ShortestPathsGenerator<T>::ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model,
                    state_t singleTarget) : ShortestPathsGenerator<T>(model, std::vector<state_t>{singleTarget}) {}

            template <typename T>
            ShortestPathsGenerator<T>::ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model,
                    storage::BitVector const& targetBV) : ShortestPathsGenerator<T>(model, bitvectorToList(targetBV)) {}

            template <typename T>
            ShortestPathsGenerator<T>::ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model,
                    std::string const& targetLabel) : ShortestPathsGenerator<T>(model, bitvectorToList(model->getStates(targetLabel))) {}

            template <typename T>
            T ShortestPathsGenerator<T>::getDistance(unsigned long k) {
                computeKSP(k);
                return kShortestPaths[metaTarget][k - 1].distance;
            }

            template <typename T>
            storage::BitVector ShortestPathsGenerator<T>::getStates(unsigned long k) {
                computeKSP(k);
                storage::BitVector stateSet(numStates - 1, false); // no meta-target

                Path<T> currentPath = kShortestPaths[metaTarget][k - 1];
                boost::optional<state_t> maybePredecessor = currentPath.predecessorNode;
                // this omits the first node, which is actually convenient since that's the meta-target

                while (maybePredecessor) {
                    state_t predecessor = maybePredecessor.get();
                    stateSet.set(predecessor, true);

                    currentPath = kShortestPaths[predecessor][currentPath.predecessorK - 1]; // god damn you, index
                    maybePredecessor = currentPath.predecessorNode;
                }

                return stateSet;
            }

            template <typename T>
            state_list_t ShortestPathsGenerator<T>::getPathAsList(unsigned long k) {
                computeKSP(k);

                state_list_t backToFrontList;

                Path<T> currentPath = kShortestPaths[metaTarget][k - 1];
                boost::optional<state_t> maybePredecessor = currentPath.predecessorNode;
                // this omits the first node, which is actually convenient since that's the meta-target

                while (maybePredecessor) {
                    state_t predecessor = maybePredecessor.get();
                    backToFrontList.push_back(predecessor);

                    currentPath = kShortestPaths[predecessor][currentPath.predecessorK - 1];
                    maybePredecessor = currentPath.predecessorNode;
                }

                return backToFrontList;
            }

            template <typename T>
            void ShortestPathsGenerator<T>::computePredecessors() {
                assert(numStates - 1 == transitionMatrix.getRowCount());

                // one more for meta-target
                graphPredecessors.resize(numStates);

                for (state_t i = 0; i < numStates - 1; i++) {
                    for (auto const& transition : transitionMatrix.getRowGroup(i)) {
                        // to avoid non-minimal paths, the target states are
                        // *not* predecessors of any state but the meta-target
                        if (std::find(targets.begin(), targets.end(), i) == targets.end()) {
                            graphPredecessors[transition.getColumn()].push_back(i);
                        }
                    }
                }

                // meta-target has exactly the target states as predecessors
                graphPredecessors[metaTarget] = targets;
            }

            template <typename T>
            void ShortestPathsGenerator<T>::performDijkstra() {
                // the existing Dijkstra isn't working anyway AND
                // doesn't fully meet our requirements, so let's roll our own

                T inftyDistance = utility::zero<T>();
                T zeroDistance = utility::one<T>();
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

                    if (targetSet.count(currentNode) == 0) {
                        // non-target node, treated normally
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
                    } else {
                        // target node has only "virtual edge" (with prob 1) to meta-target
                        // no multiplication necessary
                        T alternateDistance = shortestPathDistances[currentNode];
                        if (alternateDistance > shortestPathDistances[metaTarget]) {
                            shortestPathDistances[metaTarget] = alternateDistance;
                            shortestPathPredecessors[metaTarget] = boost::optional<state_t>(currentNode);
                        }
                        // no need to enqueue meta-target
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
            T ShortestPathsGenerator<T>::getEdgeDistance(state_t tailNode, state_t headNode) const {
                // just to be clear, head is where the arrow points (obviously)
                if (headNode != metaTarget) {
                    // edge is "normal", not to meta-target

                    for (auto const& transition : transitionMatrix.getRowGroup(tailNode)) {
                        if (transition.getColumn() == headNode) {
                            return transition.getValue();
                        }
                    }

                    // there is no such edge
                    // let's disallow that for now, because I'm not expecting it to happen
                    assert(false);
                    return utility::zero<T>();
                } else {
                    // edge must be "virtual edge" from target state to meta-target
                    assert(targetSet.count(tailNode) == 1);
                    return utility::one<T>();
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
                        auto it = std::find(candidatePaths[node].begin(), candidatePaths[node].end(), shortestPathToNode);
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

                    candidatePaths[node].erase(std::find(candidatePaths[node].begin(), candidatePaths[node].end(), minDistanceCandidate));
                    kShortestPaths[node].push_back(minDistanceCandidate);
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::computeKSP(unsigned long k) {
                unsigned long alreadyComputedK = kShortestPaths[metaTarget].size();

                for (unsigned long nextK = alreadyComputedK + 1; nextK <= k; nextK++) {
                    computeNextPath(metaTarget, nextK);
                    if (kShortestPaths[metaTarget].size() < nextK) {
                        //std::cout << std::endl << "--> DEBUG: Last path: k=" << (nextK - 1) << ":" << std::endl;
                        //printKShortestPath(metaTarget, nextK - 1);
                        //std::cout << "---------" << "No other path exists!" << std::endl;
                        throw std::invalid_argument("k-SP does not exist for k=" + std::to_string(k));
                    }
                }
            }

            template <typename T>
            void ShortestPathsGenerator<T>::printKShortestPath(state_t targetNode, unsigned long k, bool head) const {
                // note the index shift! risk of off-by-one
                Path<T> p = kShortestPaths[targetNode][k - 1];

                if (head) {
                    std::cout << "Path (reversed";
                    if (targetNode == metaTarget) {
                        std::cout << ", w/ meta-target";
                    }
                    std::cout <<"), dist (prob)=" << p.distance << ": [";
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
