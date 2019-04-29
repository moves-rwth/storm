#pragma once

#include <vector>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/multiobjective/deterministicScheds/DeterministicSchedsObjectiveHelper.h"
#include "storm/storage/geometry/Polytope.h"
#include "storm/storage/geometry/PolytopeTree.h"
#include "storm/solver/LpSolver.h"
#include "storm/utility/Stopwatch.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace multiobjective {
            
            template <typename ModelType, typename GeometryValueType>
            class DeterministicSchedsLpChecker {
            public:
                
                typedef typename ModelType::ValueType ValueType;
                typedef typename std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> Polytope;
                typedef typename std::vector<GeometryValueType> Point;
                
                DeterministicSchedsLpChecker(Environment const& env, ModelType const& model, std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives);
                ~DeterministicSchedsLpChecker();

                /*!
                 * Specifies the current direction.
                 */
                void setCurrentWeightVector(std::vector<GeometryValueType> const& weightVector);
                
                /*!
                 * Optimizes in the currently given direction
                 * @return some optimal point found in that direction.
                 */
                boost::optional<Point> check(storm::Environment const& env, Polytope area);
                
                /*!
                 * Optimizes in the currently given direction, recursively checks for points in the given area.
                 * @return all pareto optimal points in the area given by polytopeTree as well as a set of area in which no solution lies (the points might be achievable via some point outside of this area, though)
                 */
                std::pair<std::vector<Point>, std::vector<Polytope>> check(storm::Environment const& env, storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, GeometryValueType const& eps);

            private:
                void initializeObjectiveHelper(std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives);
                void initializeLpModel(Environment const& env);
                
                void checkRecursive(storm::storage::geometry::PolytopeTree<GeometryValueType>& polytopeTree, GeometryValueType const& eps, std::vector<Point>& foundPoints, std::vector<Polytope>& infeasableAreas);
                
                ModelType const& model;
                std::vector<DeterministicSchedsObjectiveHelper<ModelType>> objectiveHelper;

                std::unique_ptr<storm::solver::LpSolver<ValueType>> lpModel;
                std::vector<storm::expressions::Expression> initialStateResults;
                std::vector<storm::expressions::Variable> currentObjectiveVariables;
                std::vector<GeometryValueType> currentWeightVector;

                storm::utility::Stopwatch swInit;
                storm::utility::Stopwatch swCheck;
                storm::utility::Stopwatch swCheckVertices;
                storm::utility::Stopwatch swLpSolve;
                storm::utility::Stopwatch swLpBuild;
                storm::utility::Stopwatch swAux;
            };
            
        }
    }
}