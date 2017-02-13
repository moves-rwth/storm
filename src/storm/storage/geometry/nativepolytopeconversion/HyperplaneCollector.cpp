/* 
 * File:   HyperplaneCollector.cpp
 * Author: tim quatmann
 * 
 * Created on December 10, 2015, 6:06 PM
 */

#include "HyperplaneCollector.h"
namespace hypro {
    namespace pterm{
        
        template< typename Number>
        bool HyperplaneCollector<Number>::insert(hypro::Hyperplane<Number>const & hyperplane, std::vector<std::size_t>const* indexList) {
            return this->insert(hyperplane.normal(), hyperplane.offset(), indexList);
        }
        
        template< typename Number>
        bool HyperplaneCollector<Number>::insert(hypro::vector_t<Number> const& normal, Number const& offset, std::vector<std::size_t>const* indexList) {
            hypro::vector_t<Number> copyNormal(normal);
            Number copyOffset(offset);
            return this->insert(std::move(copyNormal), std::move(copyOffset), indexList);
        }
        
        template< typename Number>
        bool HyperplaneCollector<Number>::insert(hypro::vector_t<Number> && normal, Number && offset, std::vector<std::size_t>const* indexList) {
            //Normalize
            Number infinityNorm = normal.template lpNorm<Eigen::Infinity>();
            if(infinityNorm != (Number)0 ){
                normal /= infinityNorm;
                offset /= infinityNorm;
            }
            
            if(indexList == nullptr){
                //insert with empty list
                return this->mMap.insert(ValueType(KeyType(normal, offset), std::vector<std::size_t>())).second;
            } else {
                auto inserted = this->mMap.insert(ValueType(KeyType(normal, offset), *indexList));
                if(!inserted.second){
                    //Append vertex list
                    inserted.first->second.insert(inserted.first->second.end(), indexList->begin(), indexList->end());
                }
                return inserted.second;
            }
        }

        template< typename Number>
        std::pair<hypro::matrix_t<Number>, hypro::vector_t<Number>> HyperplaneCollector<Number>::getCollectedHyperplanesAsMatrixVector() const{
            assert(!this->mMap.empty());
            hypro::matrix_t<Number> A(this->mMap.size(), this->mMap.begin()->first.first.rows());
            hypro::vector_t<Number> b(this->mMap.size());
            
            std::size_t row = 0;
            for(auto const& mapEntry : this->mMap){
                A.row(row) = mapEntry.first.first;
                b(row) = mapEntry.first.second;
                ++row;
            }
            return std::pair<hypro::matrix_t<Number>, hypro::vector_t<Number>>(std::move(A), std::move(b)); 
        }

        template< typename Number>
        std::vector<std::vector<std::size_t>> HyperplaneCollector<Number>::getIndexLists() const{
            assert(!this->mMap.empty());
            std::vector<std::vector<std::size_t>> result(this->mMap.size());
            
            auto resultIt = result.begin();
            for(auto const& mapEntry : this->mMap){
                *resultIt = mapEntry.second;
                ++resultIt;
            }
            return result; 
        }
        
        template< typename Number>
        std::size_t HyperplaneCollector<Number>::numOfCollectedHyperplanes() const {
            return this->mMap.size();
        }
    }
}

