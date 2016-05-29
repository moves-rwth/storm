#pragma once

#include "src/storage/jani/Edge.h"

namespace storm {
    namespace jani {
        
        class EdgeSet {
        public:
            typedef std::vector<Edge> container_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            
            /*!
             * Creates a new set of edges.
             */
            EdgeSet(std::vector<Edge> const& edges = std::vector<Edge>());

            /*!
             * Adds the edge to the edges.
             */
            void addEdge(Edge const& edge);
            
            // Methods to get an iterator to the edges.
            iterator begin();
            iterator end();
            const_iterator begin() const;
            const_iterator end() const;
            
        private:
            // The set of edges.
            container_type edges;
        };
        
    }
}