#pragma once

#include <memory>
#include <queue>

#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectivePreprocessorResult.h"
#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsLpChecker.h"

#include "storm/storage/geometry/Polytope.h"
#include "storm/storage/geometry/Halfspace.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/solver/SmtSolver.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseModelType, typename GeometryValueType>
            class DeterministicSchedsParetoExplorer {
            public:
                typedef uint64_t PointId;
                typedef typename std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> Polytope;
                typedef typename SparseModelType::ValueType ModelValueType;
                
                class Point {
                public:
                    Point(std::vector<GeometryValueType> const& coordinates);
                    Point(std::vector<GeometryValueType>&& coordinates);
                    
                    std::vector<GeometryValueType> const& get() const;
                    std::vector<GeometryValueType>& get();
                    
                    uint64_t dimension() const;
                    
                    enum class DominanceResult {
                        Incomparable,
                        Dominates,
                        Dominated,
                        Equal
                    };
                    DominanceResult getDominance(Point const& other) const;
                    
                    void setParetoOptimal(bool value = true);
                    bool isParetoOptimal() const;
                    void setOnFacet(bool value = true);
                    bool liesOnFacet() const;
                    
                    std::string toString(bool convertToDouble = false) const;

                private:
                    std::vector<GeometryValueType> coordinates;
                    bool paretoOptimal;
                    bool onFacet;
                };
                
                
                class Pointset {
                public:
                    
                    typedef typename std::map<PointId, Point>::const_iterator iterator_type;

                    
                    Pointset();
                    
                    /*!
                     * If the given point is not dominated by another point in the set, it is added
                     * to the set and its ID is returned.
                     * If the point is dominated by another point, boost::none is returned.
                     * Erases all points in the set, that are dominated by the given point.
                     * If the same point is already contained in the set, its id is returned
                     */
                    boost::optional<PointId> addPoint(Environment const& env, Point&& point);
                    
                    /*!
                     * Returns the point with the given ID
                     */
                    Point const& getPoint(PointId const& id) const;
                    
                    iterator_type begin() const;
                    iterator_type end() const;
                    
                    /*!
                     * Returns the number of points currently contained in the set
                     */
                    uint64_t size() const;
                    
                    /*!
                     * Returns the downward closure of the contained points.
                     */
                    Polytope downwardClosure() const;
                    
                    void collectPointsInPolytope(std::set<PointId>& collectedPoints, Polytope const& polytope);
                    
                    void printToStream(std::ostream& out, bool includeIDs = true, bool convertToDouble = false);
                    
                private:
                    std::map<PointId, Point> points;
                    PointId currId;
                };
                
                class Facet {
                public:
                    Facet(storm::storage::geometry::Halfspace<GeometryValueType> const& halfspace);
                    Facet(storm::storage::geometry::Halfspace<GeometryValueType>&& halfspace);
                    storm::storage::geometry::Halfspace<GeometryValueType> const& getHalfspace() const;
                    void addPoint(PointId const& pointId, Point const& point);
                    std::vector<PointId> const& getPoints() const;
                    uint64_t getNumberOfPoints() const;
                    
                    /*!
                     * Creates a polytope that captures all points that lie 'under' the facet
                     */
                    Polytope const& getInducedSimplex(Pointset const& pointset, std::vector<GeometryValueType> const& referenceCoordinates);
                    
                    

                private:
                    storm::storage::geometry::Halfspace<GeometryValueType> halfspace;
                    std::vector<PointId> paretoPointsOnFacet;
                    Polytope inducedSimplex;
                };
                
                struct FacetAnalysisContext {
                    FacetAnalysisContext(Facet& f);
                    
                    Facet& facet;
                    std::set<PointId> collectedPoints;
                    std::unique_ptr<storm::solver::SmtSolver> smtSolver;
                    std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
                    
                    // Variables that encode two points that lie in the induced simplex of the analyzed facet
                    // xMinusEps = (x_1-eps, x_m-eps)
                    std::vector<storm::expressions::Variable> x, xMinusEps;
                };
                
                
                DeterministicSchedsParetoExplorer(preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult);

                virtual std::unique_ptr<CheckResult> check(Environment const& env);

                void exportPlotOfCurrentApproximation(Environment const& env);
                
            private:
                
                /*!
                 * Cleans up all cached results from a previous check call
                 */
                void clean();
                
                /*!
                 * Intersects the overapproximation with the given halfspace
                 */
                void addHalfspaceToOverApproximation(Environment const& env, std::vector<GeometryValueType> const& normalVector, Point const& pointOnHalfspace);
                
                /*!
                 * Adds a polytope which consists of unachievable points
                 */
                void addUnachievableArea(Environment const& env, Polytope const& area);
                
                /*!
                 *   Builds the initial facets by optimizing the objectives individually.
                 *   Adds the facets that need further processing to unprocessedFacets
                 */
                void initializeFacets(Environment const& env);
                
                /*!
                 *  Gets reference coordinates used to subdividing the downwardclosure
                 */
                std::vector<GeometryValueType> getReferenceCoordinates() const;
                
                /*!
                 * Checks the precision of the given Facet and returns true, if no further processing of the facet is necessary
                 */
                bool checkFacetPrecision(Environment const& env, Facet& f);
                
                /*!
                 * Checks the precision of the given Facet and returns true, if no further processing of the facet is necessary.
                 * Also takes the given points within the simplex of the facet into account
                 */
                bool checkFacetPrecision(Environment const& env, Facet& f, std::set<PointId> const& collectedSimplexPoints);
                
                /*! Processes the given facet as follows:
                 * 1. Optimize in the facet direction. Potentially, this adds new, unprocessed facets
                 * 2. Find points that have already been collected so far such that  they lie in the induced simplex of the facet.
                 * 3. Find more points that lie on the facet
                 * 4. Find all points that lie in the induced simplex or prove that there are none
                 */
                void processFacet(Environment const& env, Facet& f);
                
                FacetAnalysisContext createAnalysisContext(Environment const& env, Facet& f);
                
                /*!
                 * Optimizes in the facet direction. If this results in a point that does not lie on the facet,
                 * 1. The new Pareto optimal point is added
                 * 2. New facets are generated and (if not already precise enough) added to unprocessedFacets
                 * 3. true is returned
                 */
                bool optimizeAndSplitFacet(Environment const& env, Facet& f);
                
                /*!
                 * Adds a new point that lies within the induced simplex of the given facet to the analysis context.
                 * @param context the analysis context
                 * @param pointId the id of the given point.
                 * @param performCheck if true, it is checked whether the facet is sufficiently precise now. If false, no check is performed.
                 * @return true iff performCheck is true and the facet is sufficiently precise.
                 */
                bool addNewSimplexPoint(FacetAnalysisContext& context, PointId const& pointId, bool performCheck);
                
                Pointset pointset;
                std::queue<Facet> unprocessedFacets;
                Polytope overApproximation;
                std::vector<Polytope> unachievableAreas;
                
                std::shared_ptr<DeterministicSchedsLpChecker<SparseModelType, GeometryValueType>> lpChecker;
                std::shared_ptr<SparseModelType> const& model;
                uint64_t originalModelInitialState;
                std::vector<Objective<ModelValueType>> const& objectives;
            };
            
        }
    }
}