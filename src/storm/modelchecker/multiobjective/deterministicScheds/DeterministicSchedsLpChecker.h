#pragma once

#include <optional>
#include <vector>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsObjectiveHelper.h"
#include "storm/solver/LpSolver.h"
#include "storm/storage/geometry/Polytope.h"
#include "storm/storage/geometry/PolytopeTree.h"
#include "storm/utility/Stopwatch.h"

namespace storm {

class Environment;

namespace modelchecker {
namespace multiobjective {

/*!
 * Represents the LP Encoding for achievability under simple strategies
 * @see http://doi.org/10.18154/RWTH-2023-09669 Chapter 8.3 and 8.4
 */
template<typename ModelType, typename GeometryValueType>
class DeterministicSchedsLpChecker {
   public:
    typedef typename ModelType::ValueType ValueType;
    typedef typename std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> Polytope;
    typedef typename std::vector<GeometryValueType> Point;

    DeterministicSchedsLpChecker(ModelType const& model, std::vector<DeterministicSchedsObjectiveHelper<ModelType>> const& objectiveHelper);

    /*!
     * Specifies the current direction.
     */
    void setCurrentWeightVector(Environment const& env, std::vector<GeometryValueType> const& weightVector);

    /*!
     * Optimizes in the currently given direction
     *
     * If there is a point in the given area, an achievable point p and a value v are returned such that
     * p*w≈q*w≈v, where w is the weight vector and q is an (unknown) optimal point in the overapproximation.
     * - if eps is present, the requirement is p*w ≤ q*w ≤ v and |p*w-v| ≤ |eps*w|, and
     * - if eps is not present (i.e. empty), the default precision of the underlying LP solver is used.
     * If there is no achievable point in the given area, the returned object remains uninitialized.
     *
     * @param eps if not empty, defines the precision requirement
     *
     */
    std::optional<std::pair<Point, GeometryValueType>> check(storm::Environment const& env, Polytope overapproximation, Point const& eps = {});

    /*!
     * Optimizes in the currently given direction, recursively checks for points in the given area.
     * @return all pareto optimal points in the area given by polytopeTree as well as a set of area in which no solution lies (the points might be achievable
     * via some point outside of this area, though)
     */
    std::pair<std::vector<Point>, std::vector<Polytope>> check(storm::Environment const& env,
                                                               storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, Point const& eps);

    /*!
     * Returns usage statistics in a human readable format.
     * @param prefix will be prepended to each line
     */
    std::string getStatistics(std::string const& prefix = "") const;

   private:
    void initialize(Environment const& env);

    void initializeLpModel(Environment const& env);

    // Builds the induced markov chain of the current model and checks whether the resulting value coincide with the result of the lp solver.
    Point validateCurrentModel(Environment const& env) const;

    void checkRecursive(storm::Environment const& env, storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, Point const& eps,
                        std::vector<Point>& foundPoints, std::vector<Polytope>& infeasableAreas, uint64_t const& depth);

    ModelType const& model;
    std::vector<DeterministicSchedsObjectiveHelper<ModelType>> const& objectiveHelper;

    std::unique_ptr<storm::solver::LpSolver<ValueType>> lpModel;
    std::vector<storm::expressions::Expression> choiceVariables;
    std::vector<storm::expressions::Expression> initialStateResults;
    std::vector<storm::expressions::Variable> currentObjectiveVariables;
    std::vector<GeometryValueType> currentWeightVector;
    bool flowEncoding;

    storm::utility::Stopwatch swAll;
    storm::utility::Stopwatch swInit;
    storm::utility::Stopwatch swCheckWeightVectors;
    storm::utility::Stopwatch swCheckAreas;
    storm::utility::Stopwatch swValidate;
    uint64_t numLpQueries;
};

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm