#ifndef STORM_UTIL_SHORTESTPATHS_H_
#define STORM_UTIL_SHORTESTPATHS_H_

#include <vector>
#include <boost/optional/optional.hpp>
#include "src/models/sparse/Model.h"
#include "src/storage/sparse/StateType.h"

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
             * up the path's tail stored as the k-th entry of the predecessor's
             * shortest paths list.
             */
            template <typename T>
            struct Path {
                boost::optional<state_t> predecessorNode;
                unsigned int predecessorK;
                T distance;
            };

            template <typename T>
            class ShortestPathsGenerator {
            public:
                // FIXME: this shared_ptr-passing business is probably a bad idea
                ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model);
                ~ShortestPathsGenerator();

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
                 * Recurses over the path and prints the nodes. Intended for debugging.
                 */
                void printKShortestPath(state_t targetNode, int k, bool head=true);
            };
        }
    }
}


#endif //STORM_UTIL_SHORTESTPATHS_H_
