#include "storm/modelchecker/multiobjective/MultiObjectivePostprocessing.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/Scheduler.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

template<typename ValueType, typename GeometryValueType>
std::vector<GeometryValueType> transformObjectiveValuesToOriginal(std::vector<Objective<ValueType>> objectives, std::vector<GeometryValueType> const& point) {
    std::vector<GeometryValueType> result;
    result.reserve(point.size());
    for (uint_fast64_t objIndex = 0; objIndex < objectives.size(); ++objIndex) {
        auto const& obj = objectives[objIndex];
        if (storm::solver::maximize(obj.formula->getOptimalityType())) {
            if (obj.considersComplementaryEvent) {
                result.push_back(storm::utility::one<GeometryValueType>() - point[objIndex]);
            } else {
                result.push_back(point[objIndex]);
            }
        } else {
            if (obj.considersComplementaryEvent) {
                result.push_back(storm::utility::one<GeometryValueType>() + point[objIndex]);
            } else {
                result.push_back(-point[objIndex]);
            }
        }
    }
    return result;
}

template<typename ValueType, typename GeometryValueType>
std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> transformObjectivePolytopeToOriginal(
    std::vector<Objective<ValueType>> objectives, std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> const& polytope) {
    if (polytope->isEmpty()) {
        return storm::storage::geometry::Polytope<GeometryValueType>::createEmptyPolytope();
    }
    if (polytope->isUniversal()) {
        return storm::storage::geometry::Polytope<GeometryValueType>::createUniversalPolytope();
    }
    uint_fast64_t numObjectives = objectives.size();
    std::vector<std::vector<GeometryValueType>> transformationMatrix(numObjectives,
                                                                     std::vector<GeometryValueType>(numObjectives, storm::utility::zero<GeometryValueType>()));
    std::vector<GeometryValueType> transformationVector;
    transformationVector.reserve(numObjectives);
    for (uint_fast64_t objIndex = 0; objIndex < numObjectives; ++objIndex) {
        auto const& obj = objectives[objIndex];
        if (storm::solver::maximize(obj.formula->getOptimalityType())) {
            if (obj.considersComplementaryEvent) {
                transformationMatrix[objIndex][objIndex] = -storm::utility::one<GeometryValueType>();
                transformationVector.push_back(storm::utility::one<GeometryValueType>());
            } else {
                transformationMatrix[objIndex][objIndex] = storm::utility::one<GeometryValueType>();
                transformationVector.push_back(storm::utility::zero<GeometryValueType>());
            }
        } else {
            if (obj.considersComplementaryEvent) {
                transformationMatrix[objIndex][objIndex] = storm::utility::one<GeometryValueType>();
                transformationVector.push_back(storm::utility::one<GeometryValueType>());
            } else {
                transformationMatrix[objIndex][objIndex] = -storm::utility::one<GeometryValueType>();
                transformationVector.push_back(storm::utility::zero<GeometryValueType>());
            }
        }
    }
    return polytope->affineTransformation(transformationMatrix, transformationVector);
}

/*
 * This function is only responsible to reverse changes to the model made in the preprocessor
 * (not the ones done by specific checkers)
 */
template<typename ValueType, typename SparseModelType>
std::shared_ptr<storm::storage::Scheduler<ValueType>> transformObjectiveSchedulerToOriginal(std::shared_ptr<SparseModelType> const& originalModel,
                                                                                            std::shared_ptr<storm::storage::Scheduler<ValueType>> scheduler) {
    storm::storage::Scheduler<ValueType> result(originalModel->getNumberOfStates());
    // set common states
    for (int state = 0; state < scheduler->getNumberOfModelStates(); state++) {
        for (int memory = 0; memory < scheduler->getNumberOfMemoryStates(); memory++) {
            result.setChoice(scheduler->getChoice(state, memory), state, memory);
        }
    }
    // add irrelevant states removed in preprocessing
    for (int state = scheduler->getNumberOfModelStates(); state < originalModel->getNumberOfStates(); state++) {
        for (int memory = 0; memory < scheduler->getNumberOfMemoryStates(); memory++) {
            result.setDontCare(state, memory, false);
        }
    }
    return std::make_shared<storm::storage::Scheduler<ValueType>>(result);
}

template std::vector<storm::RationalNumber> transformObjectiveValuesToOriginal(std::vector<Objective<double>> objectives,
                                                                               std::vector<storm::RationalNumber> const& point);
template std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> transformObjectivePolytopeToOriginal(
    std::vector<Objective<double>> objectives, std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> const& polytope);
template std::vector<storm::RationalNumber> transformObjectiveValuesToOriginal(std::vector<Objective<storm::RationalNumber>> objectives,
                                                                               std::vector<storm::RationalNumber> const& point);
template std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> transformObjectivePolytopeToOriginal(
    std::vector<Objective<storm::RationalNumber>> objectives, std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> const& polytope);
template std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>> transformObjectiveSchedulerToOriginal(
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalNumber>> const& originalModel,
    std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>> scheduler);
template std::shared_ptr<storm::storage::Scheduler<double>> transformObjectiveSchedulerToOriginal(
    std::shared_ptr<storm::models::sparse::Mdp<double>> const& originalModel, std::shared_ptr<storm::storage::Scheduler<double>> scheduler);
template std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>> transformObjectiveSchedulerToOriginal(
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>> const& originalModel,
    std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>> scheduler);
template std::shared_ptr<storm::storage::Scheduler<double>> transformObjectiveSchedulerToOriginal(
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> const& originalModel, std::shared_ptr<storm::storage::Scheduler<double>> scheduler);
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm