#include <sstream>
#include <storm/models/sparse/MarkovAutomaton.h>

#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicParetoExplorer.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"


#include "storm/utility/export.h"

#include "storm/exceptions/InvalidOperationException.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseModelType, typename GeometryValueType>
            DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::Point(std::vector<GeometryValueType> const& coordinates) : coordinates(coordinates), paretoOptimal(false) {
                STORM_LOG_ASSERT(!this->coordinates.empty(), "Points with dimension 0 are not supported");
            }
            
            template <class SparseModelType, typename GeometryValueType>
            DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::Point(std::vector<GeometryValueType>&& coordinates) : coordinates(std::move(coordinates)), paretoOptimal(false) {
                STORM_LOG_ASSERT(!this->coordinates.empty(), "Points with dimension 0 are not supported");
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::vector<GeometryValueType>& DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::get() {
                return coordinates;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::vector<GeometryValueType> const& DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::get() const {
                return coordinates;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            uint64_t DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::dimension() const {
                STORM_LOG_ASSERT(!coordinates.empty(), "Points with dimension 0 are not supported");
                return coordinates.size();
            }
            
            template <class SparseModelType, typename GeometryValueType>
            typename DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::DominanceResult DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::getDominance(Point const& other) const {
                STORM_LOG_ASSERT(this->dimension() == other.dimension(), "Non-Equal dimensions of points: [" << this->toString() << "] vs. [" << other.toString() << "]");
                auto thisIt = this->get().begin();
                auto otherIt = other.get().begin();
                auto thisItE = this->get().end();
                
                // Find the first entry where the points differ
                while (*thisIt == *otherIt) {
                    ++thisIt;
                    ++otherIt;
                    if (thisIt == thisItE) {
                        return DominanceResult::Equal;
                    }
                }
                    
                if (*thisIt > *otherIt) {
                    // *this might dominate other
                    for (++thisIt, ++otherIt; thisIt != thisItE; ++thisIt, ++otherIt) {
                        if (*thisIt < *otherIt) {
                            return DominanceResult::Incomparable;
                        }
                    }
                    return DominanceResult::Dominates;
                } else {
                    assert(*thisIt < *otherIt);
                    // *this might be dominated by other
                    for (++thisIt, ++otherIt; thisIt != thisItE; ++thisIt, ++otherIt) {
                        if (*thisIt > *otherIt) {
                            return DominanceResult::Incomparable;
                        }
                    }
                    return DominanceResult::Dominated;
                }
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::setParetoOptimal(bool value) {
                paretoOptimal = value;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::isParetoOptimal() const {
                return paretoOptimal;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::string DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point::toString(bool convertToDouble) const {
                std::stringstream out;
                bool first = true;
                for (auto const& pi : this->get()) {
                    if (first) {
                        first = false;
                    } else {
                        out << ", ";
                    }
                    if (convertToDouble) {
                        out << pi;
                    } else {
                        out << pi;
                    }
                }
                return out.str();
            }
            
      //      template <class SparseModelType, typename GeometryValueType>
      //      bool operator<(typename DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point const& lhs, typename DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point const& rhs) {
      //          STORM_LOG_ASSERT(lhs.dimension() == rhs.dimension(), "Non-Equal dimensions of points: " << lhs << " vs. " << rhs);
      //          for (uint64_t i = 0; i < lhs.dimension(); ++i) {
      //              if (lhs.get()[i] < rhs.get()[i]) {
      //                  return true;
      //              } else if (lhs.get()[i] != rhs.get()[i]) {
      //                  return false;
      //              }
      //          }
      //          return false;
      //      }
            
            template <class SparseModelType, typename GeometryValueType>
            DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Pointset::Pointset() : currId(1) {
                // Intentionally left empty
            }
        
            template <class SparseModelType, typename GeometryValueType>
            boost::optional<typename DeterministicParetoExplorer<SparseModelType, GeometryValueType>::PointId> DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Pointset::addPoint(Point const& point) {
            
                // Find dominated and dominating points
                auto pointsIt = points.begin();
                auto pointsItE = points.end();
                while (pointsIt != points.end()) {
                    switch (point.getDominance(pointsIt->second)) {
                        case Point::DominanceResult::Incomparable:
                            // Nothing to be done for this point
                            ++pointsIt;
                        case Point::DominanceResult::Dominates:
                            // Found a point in the set that is dominated by the new point, so we erase it
                            pointsIt = points.erase(pointsIt);
                        case Point::DominanceResult::Dominated:
                            // The new point is dominated by another point.
                            return boost::none;
                        case Point::DominanceResult::Equal:
                            if (point.isParetoOptimal()) {
                                pointsIt->second.setParetoOptimal();
                            }
                            return pointsIt->first;
                    }
                }
                
                points.emplace_hint(points.end(), currId, point);
                return currId++;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            typename DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Point const& DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Pointset::getPoint(PointId const& id) const {
                return points.at(id);
            }
            
            template <class SparseModelType, typename GeometryValueType>
            uint64_t DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Pointset::size() const {
                return points.size();
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Pointset::collectPointsInPolytope(std::set<PointId>& collectedPoints, Polytope const& polytope) {
                for (auto const& p : points) {
                    if (polytope->contains(p.second.get())) {
                        collectedPoints.insert(p.first);
                    }
                }
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Pointset::printToStream(std::ostream& out, bool includeIDs, bool convertToDouble) {
                for (auto const& p : this->points) {
                    if (includeIDs) {
                        out << p.first << ": [" << p.second.toString(convertToDouble) << "]" << std::endl;
                    } else {
                        out << p.second.toString(convertToDouble) << std::endl;
                    }
                }
            }
            
            template <class SparseModelType, typename GeometryValueType>
            DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Facet::Facet(storm::storage::geometry::Halfspace<GeometryValueType> const& halfspace) : halfspace(halfspace) {
                // Intentionally left empty
            }
            
            template <class SparseModelType, typename GeometryValueType>
            DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Facet::Facet(storm::storage::geometry::Halfspace<GeometryValueType>&& halfspace) : halfspace(std::move(halfspace)) {
                // Intentionally left empty
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Facet::addPoint(PointId const& pointId) {
                paretoPointsOnFacet.push_back(pointId);
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::vector<typename DeterministicParetoExplorer<SparseModelType, GeometryValueType>::PointId> const& DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Facet::getPoints() const {
                return paretoPointsOnFacet;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            uint64_t DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Facet::getNumberOfPoints() const {
                return paretoPointsOnFacet.size();
            }
            
            
            template <class SparseModelType, typename GeometryValueType>
            typename DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Polytope DeterministicParetoExplorer<SparseModelType, GeometryValueType>::Facet::getInducedSimplex(Pointset const& pointset) const {
                std::vector<std::vector<GeometryValueType>> vertices;
                STORM_LOG_ASSERT(getNumberOfPoints() > 0, "Tried to compute the induced simplex, but not enough points are given.");
                auto pointIdIt = paretoPointsOnFacet.begin();
                auto pointIdItE = paretoPointsOnFacet.end();
                vertices.push_back(pointset.getPoint(*pointIdIt).get());
                std::vector<GeometryValueType> minPoint = vertices.back();
                
                for (++pointIdIt; pointIdIt != pointIdItE; ++pointIdIt) {
                    vertices.push_back(pointset.getPoint(*pointIdIt).get());
                    auto pIt = vertices.back().begin();
                    for (auto& mi : minPoint) {
                        mi = std::min(mi, *pIt);
                        ++pIt;
                    }
                }
                vertices.push_back(std::move(minPoint));
                
                // This facet might lie at the 'border', which means that the downward closure has to be taken in some directions
                storm::storage::BitVector dimensionsForDownwardClosure = storm::utility::vector::filterZero(this->halfspace.normalVector());
                STORM_LOG_ASSERT(dimensionsForDownwardClosure.getNumberOfSetBits() + vertices.size() >= halfspace.normalVector().size() + 1, "The number of points on the facet is insufficient");
                if (dimensionsForDownwardClosure.empty()) {
                    return storm::storage::geometry::Polytope<GeometryValueType>::create(vertices);
                } else {
                    return storm::storage::geometry::Polytope<GeometryValueType>::createSelectiveDownwardClosure(vertices, dimensionsForDownwardClosure);
                }
            }
            
            template <class SparseModelType, typename GeometryValueType>
            DeterministicParetoExplorer<SparseModelType, GeometryValueType>::DeterministicParetoExplorer(preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult) {
                // Intentionally left empty
            }

            template <class SparseModelType, typename GeometryValueType>
            std::unique_ptr<CheckResult> DeterministicParetoExplorer<SparseModelType, GeometryValueType>::check(Environment const& env) {
                
                clean();
                initializeFacets(env);
                while (!unprocessedFacets.empty()) {
                    Facet f = std::move(unprocessedFacets.front());
                    unprocessedFacets.pop();
                    processFacet(env, f);
                }
    
                // Todo: Prepare check result
                return nullptr;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::clean() {
                pointset = Pointset();
                unprocessedFacets = std::queue<Facet>();
                overApproximation = storm::storage::geometry::Polytope<GeometryValueType>::createUniversalPolytope();
                unachievableAreas.clear();
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::initializeFacets(Environment const& env) {
                // TODO
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool DeterministicParetoExplorer<SparseModelType, GeometryValueType>::checkFacetPrecision(Environment const& env, Facet const& f) {
                // TODO
                return false;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::processFacet(Environment const& env, Facet const& f) {
                // TODO
            
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool DeterministicParetoExplorer<SparseModelType, GeometryValueType>::optimizeAndSplitFacet(Environment const& env, Facet const& f) {
                // TODO
                return false;
            
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool DeterministicParetoExplorer<SparseModelType, GeometryValueType>::findAndCheckCachedPoints(Environment const& env, Facet const& f, Polytope const& inducedSimplex, std::set<PointId>& collectedPoints) {
                // TODO
                return false;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool DeterministicParetoExplorer<SparseModelType, GeometryValueType>::analyzePointsOnFacet(Environment const& env, Facet const& f, Polytope const& inducedSimplex, std::set<PointId>& collectedPoints) {
            
                // TODO
                return false;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool DeterministicParetoExplorer<SparseModelType, GeometryValueType>::analyzePointsInSimplex(Environment const& env, Facet const& f, Polytope const& inducedSimplex, std::set<PointId>& collectedPoints) {
            
                // TODO
                return false;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void DeterministicParetoExplorer<SparseModelType, GeometryValueType>::exportPlotOfCurrentApproximation(Environment const& env) {
                /*
                STORM_LOG_ERROR_COND(objectives.size()==2, "Exporting plot requested but this is only implemented for the two-dimensional case.");
                
                auto transformedUnderApprox = transformPolytopeToOriginalModel(underApproximation);
                auto transformedOverApprox = transformPolytopeToOriginalModel(overApproximation);
                
                // Get pareto points as well as a hyperrectangle that is used to guarantee that the resulting polytopes are bounded.
                storm::storage::geometry::Hyperrectangle<GeometryValueType> boundaries(std::vector<GeometryValueType>(objectives.size(), storm::utility::zero<GeometryValueType>()), std::vector<GeometryValueType>(objectives.size(), storm::utility::zero<GeometryValueType>()));
                std::vector<std::vector<GeometryValueType>> paretoPoints;
                paretoPoints.reserve(refinementSteps.size());
                for(auto const& step : refinementSteps) {
                    paretoPoints.push_back(transformPointToOriginalModel(step.lowerBoundPoint));
                    boundaries.enlarge(paretoPoints.back());
                }
                auto underApproxVertices = transformedUnderApprox->getVertices();
                for(auto const& v : underApproxVertices) {
                    boundaries.enlarge(v);
                }
                auto overApproxVertices = transformedOverApprox->getVertices();
                for(auto const& v : overApproxVertices) {
                    boundaries.enlarge(v);
                }
                
                //Further enlarge the boundaries a little
                storm::utility::vector::scaleVectorInPlace(boundaries.lowerBounds(), GeometryValueType(15) / GeometryValueType(10));
                storm::utility::vector::scaleVectorInPlace(boundaries.upperBounds(), GeometryValueType(15) / GeometryValueType(10));
                
                auto boundariesAsPolytope = boundaries.asPolytope();
                std::vector<std::string> columnHeaders = {"x", "y"};
                
                std::vector<std::vector<double>> pointsForPlotting;
                if (env.modelchecker().multi().getPlotPathUnderApproximation()) {
                    underApproxVertices = transformedUnderApprox->intersection(boundariesAsPolytope)->getVerticesInClockwiseOrder();
                    pointsForPlotting.reserve(underApproxVertices.size());
                    for(auto const& v : underApproxVertices) {
                        pointsForPlotting.push_back(storm::utility::vector::convertNumericVector<double>(v));
                    }
                    storm::utility::exportDataToCSVFile<double, std::string>(env.modelchecker().multi().getPlotPathUnderApproximation().get(), pointsForPlotting, columnHeaders);
                }
                
                if (env.modelchecker().multi().getPlotPathOverApproximation()) {
                    pointsForPlotting.clear();
                    overApproxVertices = transformedOverApprox->intersection(boundariesAsPolytope)->getVerticesInClockwiseOrder();
                    pointsForPlotting.reserve(overApproxVertices.size());
                    for(auto const& v : overApproxVertices) {
                        pointsForPlotting.push_back(storm::utility::vector::convertNumericVector<double>(v));
                    }
                    storm::utility::exportDataToCSVFile<double, std::string>(env.modelchecker().multi().getPlotPathOverApproximation().get(), pointsForPlotting, columnHeaders);
                }
                
                if (env.modelchecker().multi().getPlotPathParetoPoints()) {
                    pointsForPlotting.clear();
                    pointsForPlotting.reserve(paretoPoints.size());
                    for(auto const& v : paretoPoints) {
                        pointsForPlotting.push_back(storm::utility::vector::convertNumericVector<double>(v));
                    }
                    storm::utility::exportDataToCSVFile<double, std::string>(env.modelchecker().multi().getPlotPathParetoPoints().get(), pointsForPlotting, columnHeaders);
                }
            };
                 */
            }
            
            template class DeterministicParetoExplorer<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class DeterministicParetoExplorer<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
            template class DeterministicParetoExplorer<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            template class DeterministicParetoExplorer<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
        }
    }
}