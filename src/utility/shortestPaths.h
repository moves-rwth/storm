#ifndef STORM_UTIL_SHORTESTPATHS_H_
#define STORM_UTIL_SHORTESTPATHS_H_

#include <vector>
#include <boost/optional/optional.hpp>
#include "src/models/sparse/Model.h"
#include "src/storage/sparse/StateType.h"
#include "constants.h"

namespace storm {
    namespace utility {
        namespace shortestPaths {
            typedef storm::storage::sparse::state_type state_t;
            typedef std::vector<state_t> state_list_t;

            /*
             * Implicit shortest path representation
             *
             * All shortest paths (from s to t) can be described as some
             * k-shortest path to some node u plus the edge to t:
             *
             *     s ~~k-shortest path~~> u --> t
             *
             * This struct stores u (`predecessorNode`) and k (`predecessorK`).
             *
             * t is implied by this struct's location: It is stored in the
             * k-shortest paths list associated with t.
             *
             * Thus we can reconstruct the entire path by recursively looking
             * up the path's tail stored as the k-th entry [1] of the
             * predecessor's shortest paths list.
             *
             * [1] oh, actually, the `k-1`th entry due to 0-based indexing!
             */
            template <typename T>
            struct Path {
                boost::optional<state_t> predecessorNode;
                unsigned long predecessorK;
                T distance;

                // FIXME: uhh.. is this okay for set? just some arbitrary order
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
                // FIXME: this shared_ptr-passing business is probably a bad idea
                ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model);

                ~ShortestPathsGenerator();

                // TODO: think about suitable output format
                Path<T> getKShortest(state_t node, unsigned long k);


            private:
                std::shared_ptr<storm::models::sparse::Model<T>> model;
                storm::storage::SparseMatrix<T> transitionMatrix;
                state_t numStates;

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
                 * Recurses over the path and prints the nodes. Intended for debugging.
                 */
                void printKShortestPath(state_t targetNode, unsigned long k, bool head=true);


                // --- tiny helper fcts ---
                bool isInitialState(state_t node) {
                    auto initialStates = model->getInitialStates();
                    return find(initialStates.begin(), initialStates.end(), node) != initialStates.end();
                }

                T getEdgeDistance(state_t tailNode, state_t headNode) {
                    for (auto const& transition : transitionMatrix.getRowGroup(tailNode)) {
                        if (transition.getColumn() == headNode) {
                            return transition.getValue();
                        }
                    }
                    return storm::utility::zero<T>();
                }
                // -----------------------
            };
        }
    }
}


#endif //STORM_UTIL_SHORTESTPATHS_H_
