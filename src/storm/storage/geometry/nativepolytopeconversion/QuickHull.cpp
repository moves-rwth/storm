#include "storm/storage/geometry/nativepolytopeconversion/QuickHull.h"

#include <algorithm>

#include "storm/utility/macros.h"

#include "storm/storage/geometry/nativepolytopeconversion/SubsetEnumerator.h"
#include "storm/storage/geometry/nativepolytopeconversion/HyperplaneCollector.h"

namespace hypro {
    namespace pterm{
        
        template<typename ValueType>
        void QuickHull<Number>::generateHalfspacesFromVertices(std::vector<EigenVector> const& points, bool generateRelevantVerticesAndVertexSets){
            STORM_LOG_ASSERT(!points.empty(), "Invoked QuickHull with empty set of points.");
            STORM_LOG_DEBUG("Invoked QuickHull on " << points.size() << " points");
            const uint_fast64_t dimension = points.front().rows();

            // Generate initial set of d+1 affine independent points (if such a set exists)
            std::vector<uint_fast64_t> vertexIndices;
            uint_fast64_t minMaxVertexNumber;
            if(!this->findInitialVertices(points, vertexIndices, minMaxVertexNumber)) {
                // todo deal with this
                std::cout << "QuickHull: Could not find d+1 affine independend points. TODO: implement this case (we get some degenerated thing here)" << std::endl;
            }

            // compute point inside initial facet
            EigenVector insidePoint(EigenVector::Zero(dimension));
            for(uint_fast64_t vertexIndex : vertexIndices){
                insidePoint += points[vertexIndex];
            }
            insidePoint /= storm::utility::convertNumber<ValueType>(vertexIndices.size());

            // Create the initial facets from the found vertices.
            std::vector<Facet> facets = computeInitialFacets(vertexIndices, insidePoint);

            // Enlarge the mesh by adding by first considering all points that are min (or max) in at least one dimension
            storm::storage::BitVector currentFacets(facets.size(), true);
            storm::storage::BitVector consideredPoints(points.size(), false);
            uint_fast64_t currentNumOfVertices = vertexIndices.size();
            for(uint_fast64_t i = 0; i < minMaxVertexNumber; ++i) {
                    consideredPoints.set(i);
            }
            this->extendMesh(points,  consideredPoints, facets, currentFacets, insidePoint, currentNumOfVertices);
            for(auto & facet : facets){
                facet.maxOutsidePointIndex = 0;
                facet.outsideSet.clear();
            }

            consideredPoints = storm::storage::BitVector(points.size(), true);
            this->extendMesh(points, consideredPoints, facets, currentFacets, insidePoint, currentNumOfVertices);
            this->getPolytopeFromMesh(points, facets, currentFacets, success && generateRelevantVerticesAndVertexSets);
            
            return true;
        }
        
        
        template<typename ValueType>
        void QuickHull<Number>::extendMesh(std::vector<EigenVector>& points,
                storm::storage::BitVector& consideredPoints,
                std::vector<Facet>& facets,
                storm::storage::BitVector& currentFacets,
                vector_t<Number>& insidePoint,
                uint_fast64_t& currentNumOfVertices) const {
            
            storm::storage::BitVector currentOutsidePoints = consideredPoints;
            // Compute initial outside Sets
            for(uint_fast64_t facetIndex : currentFacets){
                computeOutsideSetOfFacet(facets[facetIndex], currentOutsidePoints, points);
            }

            for(uint_fast64_t facetCount = currentFacets.getNextSetIndex(0); facetCount != currentFacets.size(); facetCount = currentFacets.getNextSetIndex(facetCount+1)) {
                // set all points to false to get rid of points that lie within the polytope after each iteration
                currentOutsidePoints.reset();
                // Find a facet with a non-empty outside set 
                if(!facets[facetCount].outsideSet.empty()) {
                    uint_fast64_t numberOfNewFacets = 0;
                    // Now we compute the enlarged mesh
                    uint_fast64_t farAwayPointIndex = facets[facetCount].maxOutsidePointIndex;
                    assert(consideredPoints.get(farAwayPointIndex));
                    // get Visible set from maxOutsidePoint of the current facet
                    std::set<uint_fast64_t> visibleSet = getVisibleSet(facets, facetCount, points[farAwayPointIndex]);
                    std::set<uint_fast64_t> invisibleSet = getInvisibleNeighbors(facets, visibleSet);
                    for(auto invisFacetIt = invisibleSet.begin(); invisFacetIt != invisibleSet.end(); ++invisFacetIt) {
                        for(auto visFacetIt = visibleSet.begin(); visFacetIt != visibleSet.end(); ++visFacetIt) {
                            if (facetHasNeighborWithIndex(facets[*invisFacetIt], *visFacetIt)) {
                                Facet newFacet;
                                // Set points of Facet
                                newFacet.points = getCommonPoints(facets[*invisFacetIt], facets[*visFacetIt]);
                                 // replace old facet index by new facet index in the current neighbor
                                newFacet.points.push_back(farAwayPointIndex);
                                replaceFacetNeighbor(facets, *visFacetIt, facets.size(), *invisFacetIt);
                                newFacet.neighbors.push_back(*invisFacetIt);
                                // get new hyperplane from common points and new point
                                std::vector<EigenVector> vectorSet;
                                vectorSet.reserve(points[0].dimension());
                                EigenVector refPoint(points[farAwayPointIndex].rawCoordinates());
                                for (uint_fast64_t pointCount = 0; pointCount + 1 < points[0].dimension(); ++pointCount){
                                    vectorSet.emplace_back(points[newFacet.points[pointCount]].rawCoordinates() - refPoint);
                                }
                                assert(linearDependenciesFilter(vectorSet));
                                newFacet.hyperplane = Hyperplane<Number>(std::move(refPoint), std::move(vectorSet));
                                // check orientation of hyperplane
                                // To avoid multiple matrix multiplications we need a point that is known to lie well within the polytope
                                if (!newFacet.hyperplane.holds(insidePoint)){
                                    newFacet.hyperplane.invert();
                                }
                                facets.push_back(newFacet);
                                // avoid using replaced facets and add new ones
                                currentFacets.push_back(true);
                                // Set Points in outsideSet free
                                // increase Number Of new Facets
                                ++numberOfNewFacets;
                            }
                        }
                    }
                    
                    for(auto visFacetIt = visibleSet.begin(); visFacetIt != visibleSet.end(); ++visFacetIt){
                        for(uint_fast64_t m = 0; m < facets[*visFacetIt].outsideSet.size(); ++m){
                            currentOutsidePoints.set(facets[*visFacetIt].outsideSet[m], true);
                        }
                    }
                    for(auto visFacetIt = visibleSet.begin(); visFacetIt != visibleSet.end(); ++visFacetIt){
                        currentFacets.set(*visFacetIt, false);
                    }
                    // compute new outside sets
                    for(uint_fast64_t fIndex = facets.size()-numberOfNewFacets; fIndex != facets.size(); ++fIndex){
                        computeOutsideSetOfFacet(facets[fIndex], currentOutsidePoints, points);
                    }
                    
                    ++currentNumOfVertices;
                    
#ifdef PTERM_DEBUG_OUTPUT
                    numOfIncludedPoints += currentOutsidePoints.count();
                    PTERM_DEBUG("Mesh currently contains " << numOfIncludedPoints << " of " << consideredPoints.count() << "points");
#endif
                    
                    // find neighbors in new facets
                    setNeighborhoodOfNewFacets(facets, facets.size() - numberOfNewFacets, points[0].dimension());
                    
                }
            }
            return true;
        }
    
        template<typename ValueType>
        void QuickHull<Number>::getPolytopeFromMesh(std::vector<EigenVector> const& points, std::vector<Facet> const& facets, storm::storage::BitVector const& currentFacets, bool generateRelevantVerticesAndVertexSets){
            
            hypro::pterm::HyperplaneCollector<Number> hyperplaneCollector;
            for(uint_fast64_t facetCount = 0; facetCount < facets.size(); ++facetCount){
                if(currentFacets[facetCount]){
                    hyperplaneCollector.insert(std::move(facets[facetCount].hyperplane), generateRelevantVerticesAndVertexSets ? &facets[facetCount].points : nullptr);
                }
            }
            
            if(generateRelevantVerticesAndVertexSets){
                //Get the mapping from a hyperplane to the set of vertices that lie on that plane, erase the duplicates, and count for each vertex the number of hyperplanes on which that vertex lies
                this->mVertexSets = hyperplaneCollector.getIndexLists();
                std::vector<uint_fast64_t> hyperplanesOnVertexCounter(points.size(), 0);
                for(auto& vertexVector : this->mVertexSets){
                    std::set<uint_fast64_t> vertexSet;
                    for(auto const& i : vertexVector){
                        if(vertexSet.insert(i).second){
                            ++hyperplanesOnVertexCounter[i];
                        }
                    }
                    vertexVector.assign( vertexSet.begin(), vertexSet.end());
                }
                //Now, we can erase all vertices which do not lie on at least dimension() hyperplanes.
                //Note that the indices of the HyperplaneToVerticesMapping needs to be refreshed according to the new set of vertices
                //Therefore, we additionally store the old indices for every vertex to be able to translate from old to new indices
                std::unordered_map<hypro::EigenVector, std::vector<uint_fast64_t>> relevantVerticesMap;
                relevantVerticesMap.reserve(points.size());
                for(uint_fast64_t vertexIndex = 0; vertexIndex < hyperplanesOnVertexCounter.size(); ++vertexIndex){
                    if(hyperplanesOnVertexCounter[vertexIndex] >= points[0].dimension()){
                        auto mapEntry = relevantVerticesMap.insert(typename std::unordered_map<hypro::EigenVector, std::vector<uint_fast64_t>>::value_type(points[vertexIndex], std::vector<uint_fast64_t>())).first;
                        mapEntry->second.push_back(vertexIndex);
                    }
                }
                //Fill in the relevant vertices and create a translation map from old to new indices
                std::vector<uint_fast64_t> oldToNewIndexMapping (points.size(), points.size()); //Initialize with some illegal value
                this->mRelevantVertices.clear();
                this->mRelevantVertices.reserve(relevantVerticesMap.size());
                for(auto const& mapEntry : relevantVerticesMap){
                    for(auto const& oldIndex : mapEntry.second){
                        oldToNewIndexMapping[oldIndex] = this->mRelevantVertices.size();
                    }
                    this->mRelevantVertices.push_back(mapEntry.first);
                }
                //Actually translate and erase duplicates
                for(auto& vertexVector : this->mVertexSets){
                    std::set<uint_fast64_t> vertexSet;
                    for(auto const& oldIndex : vertexVector){
                        if(hyperplanesOnVertexCounter[oldIndex] >= points[0].dimension()){
                            vertexSet.insert(oldToNewIndexMapping[oldIndex]);
                        }
                    }
                    vertexVector.assign( vertexSet.begin(), vertexSet.end());
                }
            } else {
                this->mRelevantVertices.clear();
                this->mVertexSets.clear();
            }
            auto matrixVector = hyperplaneCollector.getCollectedHyperplanesAsMatrixVector();
            this->mResultMatrix = std::move(matrixVector.first);
            this->mResultVector = std::move(matrixVector.second);
            
            PTERM_DEBUG("Computed H representation from  " << points.size() << " vertices and " << this->mResultVector.rows() << " hyperplanes and " << this->mRelevantVertices.size() << " relevant vertices. Dimension is " << points[0].dimension());
            PTERM_DEBUG("Total number of considered facets: " << facets.size() << " where " << currentFacets.count() << " are enabled.");
        }

        template <typename ValueType>
        bool QuickHull<ValueType>::affineFilter(std::vector<uint_fast64_t> const& subset, uint_fast64_t const& item, std::vector<EigenVector<ValueType>> const& points){
            EigenMatrix vectorMatrix(vertices[item].dimension()+1, subset.size() + 1);
            for (uint_fast64_t i = 0; i < subset.size(); ++i){
                vectorMatrix.col(i) << vertices[subset[i]], storm::utility::one<ValueType>();
            }
            vectorMatrix.col(subset.size()) << vertices[item], storm::utility::one<ValueType>();
            return (vectorMatrix.fullPivLu().rank() > subset.size());
        }
            
        
        template<typename ValueType>
        bool QuickHull<Number>::findInitialVertices(std::vector<hypro::EigenVector>& points, std::vector<uint_fast64_t>& verticesOfInitialPolytope, uint_fast64_t& minMaxVertices) const{
            const uint_fast64_t dimension = points[0].dimension();
            if(points.size() < dimension + 1){
                //not enough points to obtain a (non-degenerated) polytope
                return false;
            }
            const uint_fast64_t candidatesToFind = std::min(2*dimension, points.size());
            uint_fast64_t candidatesFound = 0;
            storm::storage::BitVector consideredPoints(points.size(), true);
            while(candidatesFound < candidatesToFind && !consideredPoints.empty()) {
                for(uint_fast64_t currDim=0; currDim<dimension; ++currDim) {
                    uint_fast64_t minIndex = *consideredPoints.begin();
                    uint_fast64_t maxIndex = minIndex;
                    for(uint_fast64_t pointIndex : consideredPoints){
                        //Check if the current point is a new minimum or maximum at the current dimension
                        if(points[minIndex](currDim) > points[pointIndex](currDim)){
                            minIndex = pointIndex;
                        }
                        if(points[maxIndex](currDim) < points[pointIndex](currDim)){
                            maxIndex = pointIndex;
                        }
                    }
                    consideredPoints.set(minIndex, false);
                    consideredPoints.set(maxIndex, false);
                }
                //Found candidates. Now swap them to the front.
                consideredPoints = ~consideredPoints;
                const uint_fast64_t newNumberOfCandidates = consideredPoints.getNumberOfSetBits();
                assert(newNumberOfCandidates > 0);
                if(newNumberOfCandidates < points.size()){
                    uint_fast64_t nextPointToMove = consideredPoints.getNextSetIndex(newNumberOfCandidates);
                    for(uint_fast64_t indexAtFront = candidatesFound; indexAtFront < newNumberOfCandidates; ++indexAtFront){
                        if(!consideredPoints.get(indexAtFront)) {
                            assert(nextPointToMove != consideredPoints.size());
                            std::swap(points[indexAtFront], points[nextPointToMove]);
                            nextPointToMove = consideredPoints.getNextSetIndex(nextPointToMove+1);
                            consideredPoints.set(indexAtFront);
                        }
                    }
                    assert(nextPointToMove == consideredPoints.size());
                }
                if(candidatesFound==0){
                    //We are in the first iteration. It holds that the first newNumberOfCandidates points will be vertices of the final polytope
                    minMaxVertices = newNumberOfCandidates;
                }
                candidatesFound = newNumberOfCandidates;
                consideredPoints = ~consideredPoints;
            }
            
            storm::storage::geometry::SubsetEnumerator<std::vector<EigenVector<ValueType>>> subsetEnum(points.size(), dimension+1, points, affineFilter);
            if(subsetEnum.setToFirstSubset()){
                verticesOfInitialPolytope = subsetEnum.getCurrentSubset();
                return true;
            } else {
                return false;
            }
        }

        template<typename ValueType>
        std::vector<typename QuickHull<ValueType>::Facet> computeInitialFacets(std::vector<EigenVector> const& points, std::vector<uint_fast64_t> const& verticesOfInitialPolytope, EigenVector const& insidePoint) const {
            const uint_fast64_t dimension = points.front().rows();
            assert(verticesOfInitialPolytope.size() == dimension + 1);
            std::vector<Facet> result;
            result.reserve(dimension + 1);
            storm::storage::geometry::SubsetEnumerator<> subsetEnum(verticesOfInitialPolytope.size(), dimension);
            if(!subsetEnum.setToFirstSubset()){
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not find an initial subset.");
            }
            do{
                Facet newFacet;
                // set the points that lie on the new facet
                std::vector<uint_fast64_t> const& subset(subsetEnum.getCurrentSubset());
                newFacet.points.reserve(subset.size());
                for(uint_fast64_t i : subset){
                    newFacet.points.push_back(verticesOfInitialPolytope[i]);
                }
                //neighbors: these are always the remaining facets
                newFacet.neighbors.reserve(dimension);
                for(uint_fast64_t i = 0; i < dimension+1; ++i){
                    if(i != facets.size()){  //initFacets.size() will be the index of this new facet!
                        newFacet.neighbors.push_back(i);
                    }
                }
                // normal and offset:
                computeNormalAndOffsetOfFacet(points, insidePoint, newFacet);

                facets.push_back(std::move(newFacet));
            } while(subsetEnum.incrementSubset());
            assert(facets.size()==dimension+1);
        }

        template<typename ValueType>
        void computeNormalAndOffsetOfFacet(std::vector<EigenVector> const& points, EigenVector const& insidePoint, Facet& facet) const {
            const uint_fast64_t dimension = points.front().rows()
            assert(facet.points.size() == dimension);
            EigenVector refPoint(facet.points.back());
            EigenMatrix constraints(dimension-1, dimension);
            for(unsigned row = 0; row < dimension-1; ++row) {
                constraints.row(row) = points[facet.points[row]] - refPoint;
            }
            facet.normal = constraints.fullPivLu().kernel();
            facet.offset = facet.normal.dot(refPoint);

            // invert the plane if the insidePoint is not contained in it
            if(facet.normal.dot(insidePoint) > facet.offset) {
                facet.normal = -facet.normal;
                facet.offset = -facet.offset;
            }
        }


        template<typename ValueType>
        std::set<uint_fast64_t> QuickHull<Number>::getVisibleSet(std::vector<Facet> const& facets, uint_fast64_t const& startIndex, EigenVector const& point) const {
            std::set<uint_fast64_t> facetsToCheck;
            std::set<uint_fast64_t> facetsChecked;
            std::set<uint_fast64_t> visibleSet;
            facetsChecked.insert(startIndex);
            visibleSet.insert(startIndex);
            for(uint_fast64_t i = 0; i < facets[startIndex].neighbors.size(); ++i){
                facetsToCheck.insert(facets[startIndex].neighbors[i]);
            }
            while (!facetsToCheck.empty()){
                auto elementIt = facetsToCheck.begin();
                if ( point.dot(facets[*elementIt].normal) > facets[*elementIt].offset) {
                    visibleSet.insert(*elementIt);
                    for(uint_fast64_t i = 0; i < facets[*elementIt].neighbors.size(); ++i){
                        if (facetsChecked.find(facets[*elementIt].neighbors[i]) == facetsChecked.end()){
                            facetsToCheck.insert(facets[*elementIt].neighbors[i]);
                        }
                    }
                }
                facetsChecked.insert(*elementIt);
                facetsToCheck.erase(elementIt);
            }
            return visibleSet;
        }
         
        template<typename ValueType>
        void QuickHull<Number>::setNeighborhoodOfNewFacets(std::vector<Facet>& facets, uint_fast64_t firstNewFacet, uint_fast64_t dimension) const{
            for(uint_fast64_t currentFacet = firstNewFacet; currentFacet < facets.size(); ++currentFacet){
                for(uint_fast64_t otherFacet = currentFacet + 1; otherFacet < facets.size(); ++otherFacet){
                    std::vector<uint_fast64_t> commonPoints = getCommonPoints(facets[currentFacet], facets[otherFacet]); 
                    if (commonPoints.size() >= dimension-1){
                        facets[currentFacet].neighbors.push_back(otherFacet);
                        facets[otherFacet].neighbors.push_back(currentFacet);
                    }
                }
            }
        }
        
        template<typename ValueType>
        void QuickHull<Number>::replaceFacetNeighbor(std::vector<Facet>& facets, uint_fast64_t oldFacetIndex, uint_fast64_t newFacetIndex, uint_fast64_t neighborIndex) const {
            uint_fast64_t index = 0; 
            while(facets[neighborIndex].neighbors[index] != oldFacetIndex && index < facets[neighborIndex].neighbors.size()){ 
                ++index;
            }
            if (index < facets[neighborIndex].neighbors.size()){
                facets[neighborIndex].neighbors[index] = newFacetIndex;
            }
        }
        
        template<typename ValueType>
        void QuickHull<Number>::computeOutsideSetOfFacet(Facet& facet, storm::storage::BitVector& currentOutsidePoints, std::vector<EigenVector> const& points) const {
            Number maxMultiplicationResult = facet.hyperplane.offset();
            for(uint_fast64_t pointIndex = currentOutsidePoints) {
                ValueType multiplicationResult = points[pointIndex].dot(facet.normal);
                if( multiplicationResult > facet.hyperplane.offset() ) {
                    currentOutsidePoints.set(pointIndex, false); // we already know that the point lies outside so it can be ignored for future facets
                    facet.outsideSet.push_back(pointIndex);
                    if (multiplicationResult > maxMultiplicationResult) {
                        maxMultiplicationResult = multiplicationResult;
                        facet.maxOutsidePointIndex = pointIndex;
                    }
                }
            }
        }

        template<typename ValueType>
        std::vector<uint_fast64_t> QuickHull<Number>::getCommonPoints(Facet const& lhs, Facet const& rhs) const{
            std::vector<uint_fast64_t> commonPoints;
            for(uint_fast64_t refPoint = 0; refPoint < lhs.points.size(); ++refPoint){
                for(uint_fast64_t currentPoint = 0; currentPoint < rhs.points.size(); ++currentPoint){
                    if (lhs.points[refPoint] == rhs.points[currentPoint]){
                        commonPoints.push_back(lhs.points[refPoint]);
                    }
                }
            }
            return commonPoints;
        }
 
        template<typename ValueType>
        std::set<uint_fast64_t> QuickHull<Number>::getInvisibleNeighbors( std::vector<Facet>& facets, std::set<uint_fast64_t> const& visibleSet) const {
            std::set<uint_fast64_t> invisibleNeighbors;
            for(auto currentFacetIt = visibleSet.begin(); currentFacetIt != visibleSet.end(); ++currentFacetIt){
                for(uint_fast64_t currentNeighbor = 0; currentNeighbor < facets[*currentFacetIt].neighbors.size(); ++currentNeighbor){
                    if (visibleSet.find(facets[*currentFacetIt].neighbors[currentNeighbor]) == visibleSet.end()){
                        invisibleNeighbors.insert(facets[*currentFacetIt].neighbors[currentNeighbor]);
                    }
                }
            }
            return invisibleNeighbors;
        }

        template<typename ValueType>
        void QuickHull<Number>::enlargeIncompleteResult(std::vector<Facet> const& facets, storm::storage::BitVector const& currentFacets, std::vector<EigenVector> const& points,  bool generateRelevantVerticesAndVertexSets){
            PTERM_TRACE("Enlarging incomplete Result of QuickHull");
            //Obtain the set of outside points
            std::vector<uint_fast64_t> outsidePoints;
            for(uint_fast64_t facetIndex = currentFacets.find_first(); facetIndex != currentFacets.npos; facetIndex = currentFacets.find_next(facetIndex)){
                outsidePoints.insert(outsidePoints.end(), facets[facetIndex].outsideSet.begin(), facets[facetIndex].outsideSet.end());
            }
            
            //Now adjust the offsets of all the hyperplanes such that they contain each outside point
            for(uint_fast64_t planeIndex=0; planeIndex < this->mResultMatrix.rows(); ++planeIndex){
                Number maxValue = this->mResultVector(planeIndex);
                for (uint_fast64_t outsidePointIndex : outsidePoints){
                    Number currValue = this->mResultMatrix.row(planeIndex) * (points[outsidePointIndex].rawCoordinates());
                    maxValue = std::max(maxValue, currValue);
                }
                if (pterm::Settings::instance().getQuickHullRoundingMode() != pterm::Settings::QuickHullRoundingMode::NONE) {
                    maxValue = NumberReduction<Number>::instance().roundUp(maxValue);
                }
                this->mResultVector(planeIndex) = maxValue;
            }

            if(generateRelevantVerticesAndVertexSets){
                /* Note: The adjustment of the offset might introduce redundant halfspaces.
                 * It is also not clear if it suffices to consider intersection points of hyperplanes that intersected in the original polytope
                 * 
                 * Our solution is to convert the resulting h polytope back to a v poly
                 */

                PTermHPolytope<Number> enlargedPoly(std::move(this->mResultMatrix), std::move(this->mResultVector));
                HyperplaneEnumeration<Number> he;
                if(!he.generateVerticesFromHalfspaces(enlargedPoly, true)){
                    PTERM_ERROR("Could not find the vertices of the enlarged Polytope");
                }
                this->mResultMatrix = std::move(he.getRelevantMatrix());
                this->mResultVector = std::move(he.getRelevantVector());
                this->mRelevantVertices = std::move(he.getResultVertices());
                this->mVertexSets = std::move(he.getVertexSets());

            }
        }
    }
}

/* 
 * File:   AbstractVtoHConverter.tpp
 * Author: tim quatmann
 *
 * Created on Februrary 2, 2016, 1:06 PM
 */

#include "AbstractVtoHConverter.h"

namespace hypro {
    namespace pterm{
        
        template<typename ValueType>
        EigenMatrix& AbstractVtoHConverter<Number>::getResultMatrix() {
            return this->mResultMatrix;
        }
        
        template<typename ValueType>
        EigenVector& AbstractVtoHConverter<Number>::getResultVector() {
            return this->mResultVector;
        }
        
        template<typename ValueType>
        std::vector<EigenVector>& AbstractVtoHConverter<Number>::getRelevantVertices() {
            return this->mRelevantVertices;
        }
        
        template<typename ValueType>
        std::vector<std::vector<uint_fast64_t>>& AbstractVtoHConverter<Number>::getVertexSets() {
            return this->mVertexSets;
        }
        
    }
}

