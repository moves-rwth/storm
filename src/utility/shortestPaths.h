#ifndef STORM_UTIL_SHORTESTPATHS_H_
#define STORM_UTIL_SHORTESTPATHS_H_

#include <vector>
#include <boost/optional/optional.hpp>
#include <unordered_set>
#include "src/models/sparse/Model.h"
#include "src/storage/sparse/StateType.h"
#include "constants.h"

// NOTE: You'll (eventually) find the usual API documentation below;
//       for more information about the purpose, design decisions,
//       etc., you may consult `shortestPath.md`. - Tom

/*
 * TODO:
 *  - take care of self-loops of target states
 *  - implement target group
 *  - think about how to get paths with new nodes, rather than different
 *    paths over the same set of states (which happens often)
 */


namespace storm {
    namespace utility {
        namespace ksp {
            typedef storage::sparse::state_type state_t;
            typedef std::vector<state_t> state_list_t;
            using BitVector = storage::BitVector;

            template <typename T>
            struct Path {
                boost::optional<state_t> predecessorNode;
                unsigned long predecessorK;
                T distance;

                // arbitrary order for std::set
                bool operator<(const Path<T>& rhs) const {
                    if (predecessorNode != rhs.predecessorNode) {
                        return predecessorNode < rhs.predecessorNode;
                    }
                    return predecessorK < predecessorK;
                }

                bool operator==(const Path<T>& rhs) const {
                    return (predecessorNode == rhs.predecessorNode) && (predecessorK == rhs.predecessorK);
                }
            };

            // -------------------------------------------------------------------------------------------------------

            template <typename T>
            class ShortestPathsGenerator {
            public:
                /*!
                 * Performs precomputations (including meta-target insertion and Dijkstra).
                 * Modifications are done locally, `model` remains unchanged.
                 * Target (group) cannot be changed.
                 */
                // FIXME: this shared_ptr-passing business might be a bad idea
                ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model, state_list_t const& targets);

                // allow alternative ways of specifying the target,
                // all of which will be converted to list and delegated to constructor above
                ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model, state_t singleTarget);
                ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model, storage::BitVector const& targetBV);
                ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model, std::string const& targetLabel = "target");

                // a further alternative: use transition matrix of maybe-states
                // combined with target vector (e.g., the instantiated matrix/
                // vector from SamplingModel);
                // in this case separately specifying a target makes no sense
                //ShortestPathsGenerator(storm::storage::SparseMatrix<T> maybeTransitionMatrix, std::vector<T> targetProbVector);

                inline ~ShortestPathsGenerator(){}

                /*!
                 * Returns distance (i.e., probability) of the KSP.
                 * Computes KSP if not yet computed.
                 * @throws std::invalid_argument if no such k-shortest path exists
                 */
                T getDistance(unsigned long k);

                /*!
                 * Returns the states that occur in the KSP.
                 * For a path-traversal (in order and with duplicates), see `getKSP`.
                 * Computes KSP if not yet computed.
                 * @throws std::invalid_argument if no such k-shortest path exists
                 */
                storage::BitVector getStates(unsigned long k);

                /*!
                 * Returns the states of the KSP as back-to-front traversal.
                 * Computes KSP if not yet computed.
                 * @throws std::invalid_argument if no such k-shortest path exists
                 */
                state_list_t getPathAsList(unsigned long k);


            private:
                storage::SparseMatrix<T> transitionMatrix;
                state_t numStates; // includes meta-target, i.e. states in model + 1
                state_t metaTarget;
                state_list_t targets;
                BitVector initialStates;
                std::unordered_map<state_t, T> targetProbMap;

                std::vector<state_list_t>             graphPredecessors;
                std::vector<boost::optional<state_t>> shortestPathPredecessors;
                std::vector<state_list_t>             shortestPathSuccessors;
                std::vector<T>                        shortestPathDistances;

                std::vector<std::vector<Path<T>>> kShortestPaths;
                std::vector<std::set<Path<T>>>    candidatePaths;

                /*!
                 * Computes list of predecessors for all nodes.
                 * Reachability is not considered; a predecessor is simply any node that has an edge leading to the node in question.
                 * Requires `transitionMatrix`.
                 * Modifies `graphPredecessors`.
                 */
                void computePredecessors();

                /*!
                 * Computes shortest path distances and predecessors.
                 * Requires `model`, `numStates`, `transitionMatrix`.
                 * Modifies `shortestPathPredecessors` and `shortestPathDistances`.
                 */
                void performDijkstra();

                /*!
                 * Computes list of shortest path successor nodes from predecessor list.
                 * Requires `shortestPathPredecessors`, `numStates`.
                 * Modifies `shortestPathSuccessors`.
                 */
                void computeSPSuccessors();

                /*!
                 * Constructs and stores the implicit shortest path representations (see `Path`) for the (1-)shortest paths.
                 * Requires `shortestPathPredecessors`, `shortestPathDistances`, `model`, `numStates`.
                 * Modifies `kShortestPaths`.
                 */
                void initializeShortestPaths();

                /*!
                 * Main step of REA algorithm. TODO: Document further.
                 */
                void computeNextPath(state_t node, unsigned long k);

                /*!
                 * Computes k-shortest path if not yet computed.
                 * @throws std::invalid_argument if no such k-shortest path exists
                 */
                void computeKSP(unsigned long k);

                /*!
                 * Recurses over the path and prints the nodes. Intended for debugging.
                 */
                void printKShortestPath(state_t targetNode, unsigned long k, bool head=true) const;

                /*!
                 * Returns actual distance for real edges, 1 for edges to meta-target.
                 */
                T getEdgeDistance(state_t tailNode, state_t headNode) const;


                // --- tiny helper fcts ---
                inline bool isInitialState(state_t node) const {
                    return find(initialStates.begin(), initialStates.end(), node) != initialStates.end();
                }

                inline state_list_t bitvectorToList(storage::BitVector const& bv) const {
                    state_list_t list;
                    for (state_t state : bv) {
                        list.push_back(state);
                    }
                    return list;
                }
                // -----------------------
            };
        }
    }
}


#endif //STORM_UTIL_SHORTESTPATHS_H_
