/* 
 * File:   HyperplaneCollector.h
 * Author: tim quatmann
 *
 * Created on December 10, 2015, 6:06 PM
 */

#pragma once
#include <unordered_map>
#include "../../config.h"
#include "../../datastructures/Hyperplane.h"

namespace hypro{
    namespace pterm{
        /*!
         * This class can be used to collect a set of hyperplanes (without duplicates).
         * The inserted hyperplanes are normalized, i.e. devided by the infinity norm of the normal vector
         */
        template< typename Number>
        class HyperplaneCollector {
        public:
            HyperplaneCollector() = default;
            HyperplaneCollector(const HyperplaneCollector& orig) = default;
            virtual ~HyperplaneCollector() = default;
            
            /*
             * inserts the given hyperplane.
             * For every (unique) hyperplane, there is a list of indices which can be used e.g. to obtain the set of vertices that lie on each hyperplane.
             * If indexList is given (i.e. not nullptr), the given indices are appended to that list.
             * Returns true iff the hyperplane was inserted (i.e. the hyperplane was not already contained in this)
             */
            bool insert(hypro::Hyperplane<Number> const& hyperplane, std::vector<std::size_t>const* indexList = nullptr);
            bool insert(hypro::vector_t<Number> const& normal, Number const& offset, std::vector<std::size_t>const* indexList = nullptr);
            bool insert(hypro::vector_t<Number>&& normal, Number && offset, std::vector<std::size_t>const* indexList = nullptr);
            
            std::pair<hypro::matrix_t<Number>, hypro::vector_t<Number>> getCollectedHyperplanesAsMatrixVector() const;
            //Note that the returned lists might contain dublicates.
            std::vector<std::vector<std::size_t>> getIndexLists() const;
            
            std::size_t numOfCollectedHyperplanes() const;
            
        private:
            typedef std::pair<hypro::vector_t<Number>, Number> NormalOffset; 
            class NormalOffsetHash{
                public:
                    std::size_t operator()(NormalOffset const& ns) const {
                        std::size_t seed = std::hash<hypro::vector_t<Number>>()(ns.first);
                        carl::hash_add(seed, std::hash<Number>()(ns.second));
                        return seed;
                    }
            };
            typedef typename std::unordered_map<NormalOffset, std::vector<std::size_t>, NormalOffsetHash>::key_type KeyType;
            typedef typename std::unordered_map<NormalOffset, std::vector<std::size_t>, NormalOffsetHash>::value_type ValueType;
            
            std::unordered_map<NormalOffset, std::vector<std::size_t>, NormalOffsetHash> mMap;
            
            
        };
    }
}


#include "HyperplaneCollector.tpp"