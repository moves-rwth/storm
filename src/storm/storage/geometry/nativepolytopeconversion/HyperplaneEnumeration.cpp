/* 
 * File:   HyperplaneEnumeration.tpp
 * Author: tim quatmann
 * Author: phillip florian
 *
 * Created on December 28, 2015, 1:06 PM
 */

#include "HyperplaneEnumeration.h"

#include "../SubsetEnumerator.h"
#include "../HyperplaneCollector.h"

namespace hypro {
    namespace pterm{
     
        template<typename Number>
        bool HyperplaneEnumeration<Number>::generateVerticesFromHalfspaces(PTermHPolytope<Number> const& hPoly, bool generateRelevantHyperplanesAndVertexSets){
            PTERM_DEBUG("Invoked generateVerticesFromHalfspaces with " << hPoly.getMatrix().rows() << " hyperplanes. Dimension is " << hPoly.dimension());
            std::unordered_map<Point<Number>, std::set<std::size_t>> vertexCollector;
            hypro::pterm::SubsetEnumerator<hypro::matrix_t<Number>> subsetEnum(hPoly.getMatrix().rows(), hPoly.dimension(), hPoly.getMatrix(), this->linearDependenciesFilter);
            if(subsetEnum.setToFirstSubset()){
                do{
                    std::vector<std::size_t> const& subset = subsetEnum.getCurrentSubset();
                    std::pair<hypro::matrix_t<Number>, hypro::vector_t<Number>> subHPolytope(hPoly.getSubHPolytope(subset));
                    Point<Number> point(subHPolytope.first.fullPivLu().solve(subHPolytope.second));
                    //Point<Number> point(hypro::gauss(subHPolytope.first, subHPolytope.second));
                    if(hPoly.contains(point)){
                        //Note that the map avoids duplicates.
                        auto hyperplaneIndices = vertexCollector.insert(typename std::unordered_map<Point<Number>, std::set<std::size_t>>::value_type(std::move(point), std::set<std::size_t>())).first;
                        if(generateRelevantHyperplanesAndVertexSets){
                            hyperplaneIndices->second.insert(subset.begin(), subset.end());
                        }
                    }
                } while (subsetEnum.incrementSubset());
            } else {
                std::cout << "Could not generate any vertex while converting from H Polytope. TODO: implement this case (we get some unbounded thing here)" << std::endl;
                return false;
            }
            if(generateRelevantHyperplanesAndVertexSets){
                //For each hyperplane, get the number of (unique) vertices that lie on it.
                std::vector<std::size_t> verticesOnHyperplaneCounter(hPoly.getMatrix().rows(), 0);
                for(auto const& mapEntry : vertexCollector){
                    for(auto const& hyperplaneIndex : mapEntry.second){
                        ++verticesOnHyperplaneCounter[hyperplaneIndex];
                    }
                }
                
                //Only keep the hyperplanes on which at least dimension() vertices lie.
                //Note that this will change the indices of the Hyperplanes.
                //Therefore, we additionally store the old indices for every hyperplane to be able to translate from old to new indices
                hypro::pterm::HyperplaneCollector<Number> hyperplaneCollector;
                for(std::size_t hyperplaneIndex = 0; hyperplaneIndex < verticesOnHyperplaneCounter.size(); ++hyperplaneIndex){
                    if(verticesOnHyperplaneCounter[hyperplaneIndex] >= hPoly.dimension()){
                        std::vector<std::size_t> oldIndex;
                        oldIndex.push_back(hyperplaneIndex);
                        hyperplaneCollector.insert(hPoly.getMatrix().row(hyperplaneIndex), hPoly.getVector()(hyperplaneIndex), &oldIndex);
                    }
                }
                auto matrixVector = hyperplaneCollector.getCollectedHyperplanesAsMatrixVector();
                this->mRelevantMatrix = std::move(matrixVector.first);
                this->mRelevantVector = std::move(matrixVector.second);
                
                //Get the mapping from old to new indices
                std::vector<std::size_t> oldToNewIndexMapping (hPoly.getMatrix().rows(), hPoly.getMatrix().rows()); //Initialize with some illegal value
                std::vector<std::vector<std::size_t>> newToOldIndexMapping(hyperplaneCollector.getIndexLists());
                for(std::size_t newIndex = 0; newIndex < newToOldIndexMapping.size(); ++newIndex){
                    for(auto const& oldIndex : newToOldIndexMapping[newIndex]){
                        oldToNewIndexMapping[oldIndex] = newIndex;
                    }
                }
                
                //Insert the resulting vertices and get the set of vertices that lie on each hyperplane
                std::vector<std::set<std::size_t>> vertexSets(this->mRelevantMatrix.rows());
                this->mResultVertices.clear();
                this->mResultVertices.reserve(vertexCollector.size());
                for(auto const& mapEntry : vertexCollector){
                    for(auto const& oldHyperplaneIndex : mapEntry.second){
                        //ignore the hyperplanes which are redundant, i.e. for which there is no new index
                        if(oldToNewIndexMapping[oldHyperplaneIndex] < this->mRelevantVector.rows()){
                            vertexSets[oldToNewIndexMapping[oldHyperplaneIndex]].insert(this->mResultVertices.size());
                        }
                    }
                    this->mResultVertices.push_back(mapEntry.first);
                }
                this->mVertexSets.clear();
                this->mVertexSets.reserve(vertexSets.size());
                for(auto const& vertexSet : vertexSets){
                    this->mVertexSets.emplace_back(vertexSet.begin(), vertexSet.end());
                }
                
            } else {
                this->mResultVertices.clear();
                this->mResultVertices.reserve(vertexCollector.size());
                for(auto const& mapEntry : vertexCollector){
                    this->mResultVertices.push_back(mapEntry.first);
                }
                this->mVertexSets.clear();
                this->mRelevantMatrix = hypro::matrix_t<Number>();
                this->mRelevantVector = hypro::vector_t<Number>();
            }
            PTERM_DEBUG("Invoked generateVerticesFromHalfspaces with " << hPoly.getMatrix().rows() << " hyperplanes and " << this->mResultVertices.size() << " vertices and " << this->mRelevantMatrix.rows() << " relevant hyperplanes. Dimension is " << hPoly.dimension());
            return true;
        }
            

    template <typename Number>
    bool HyperplaneEnumeration<Number>::linearDependenciesFilter(std::vector<std::size_t> const& subset, std::size_t const& item, hypro::matrix_t<Number> const& A) {
        hypro::matrix_t<Number> subMatrix(subset.size() +1, A.cols());
        for (std::size_t i = 0; i < subset.size(); ++i){
            subMatrix.row(i) = A.row(subset[i]);
        }
        subMatrix.row(subset.size()) = A.row(item);
        Eigen::FullPivLU<matrix_t<Number>> lUMatrix( subMatrix );
       // std::cout << "The rank is " << lUMatrix.rank() << " and the matrix has " << subMatrix.rows() << " rows" << std::endl;
        if (lUMatrix.rank() < subMatrix.rows()){
            //Linear dependent!
            return false;
        } else {
            return true;
        }
    }
    
        
        template<typename Number>
        std::vector<Point<Number>>& HyperplaneEnumeration<Number>::getResultVertices() {
            return this->mResultVertices;
        }
        
        template<typename Number>
        hypro::matrix_t<Number>& HyperplaneEnumeration<Number>::getRelevantMatrix() {
            return this->mRelevantMatrix;
        }
        
        template<typename Number>
        hypro::vector_t<Number>& HyperplaneEnumeration<Number>::getRelevantVector() {
            return this->mRelevantVector;
        }
        
        template<typename Number>
        std::vector<std::vector<std::size_t>>& HyperplaneEnumeration<Number>::getVertexSets() {
            return this->mVertexSets;
        }
        
    }
}

