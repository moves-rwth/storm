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

            template <typename T>
            struct path {
                boost::optional<state_t> tail;
                unsigned int tail_k;
                T distance;
            };

            template <typename T>
            class ShortestPathsGenerator {
            public:
                // FIXME: this shared_ptr-passing business is probably a bad idea
                ShortestPathsGenerator(std::shared_ptr<models::sparse::Model<T>> model);
                ~ShortestPathsGenerator();

            private:
                //storm::models::sparse::Model<T>* model;
                std::shared_ptr<storm::models::sparse::Model<T>> model;
                storm::storage::SparseMatrix<T>  transitionMatrix;

                std::vector<state_list_t> graphPredecessors;
                std::vector<state_t>      shortestPathPredecessors;
                std::vector<state_list_t> shortestPathSuccessors;
                std::vector<T>            shortestPathDistances;

                std::vector<std::vector<path<T>>> shortestPaths;
                std::vector<std::set<path<T>>>    shortestPathCandidates;

                /*!
                 * Computes list of predecessors for all nodes.
                 * Reachability is not considered; a predecessor is simply any node that has an edge leading to the
                 * node in question.
                 *
                 * @param model The model whose transitions will be examined
                 * @return A vector of predecessors for each node
                 */
                void computePredecessors();

                void performDijkstra();
                void computeSPSuccessors();
                void initializeShortestPaths();

            };
        }
    }
}


#endif //STORM_UTIL_SHORTESTPATHS_H_
