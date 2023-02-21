#include "storm/modelchecker/multiobjective/MultiObjectivePostprocessing.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/memorystructure/SparseModelMemoryProductReverseData.h"

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
void transformObjectiveSchedulersToOriginal(storm::storage::SparseModelMemoryProductReverseData const& reverseData,
                                            std::shared_ptr<SparseModelType> const& originalModel,
                                            std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> schedulers) {
    auto memoryStateCount = reverseData.memory.getNumberOfStates();
    auto originalModelStates = originalModel->getNumberOfStates();
    for (auto it = schedulers.begin(); it != schedulers.end(); ++it) {
        std::shared_ptr<storm::storage::Scheduler<ValueType>> scheduler = it->second;
        auto point = it->first;
        storm::storage::Scheduler<ValueType> result(originalModelStates, reverseData.memory);
        auto currSchedulerModelStates = scheduler->getNumberOfModelStates();
        for (int state = 0; state < originalModelStates; state++) {
            for (int memState = 0; memState < memoryStateCount; memState++) {
                auto const& productState = reverseData.toResultStateMapping[state * memoryStateCount + memState];
                if (productState != -1) {
                    // if it's -1 it's unreachable
                    if (productState >= currSchedulerModelStates) {
                        // means it's been deleted as irrelevant state
                        result.setDontCare(productState, memState, true);
                    } else {
                        auto choice = scheduler->getChoice(productState);
                        result.setChoice(choice, state, memState);
                    }
                }
            }
        }
        scheduler.reset();
        schedulers[point] = std::move(std::make_shared<storm::storage::Scheduler<ValueType>>(result));
    }
}

template std::vector<storm::RationalNumber> transformObjectiveValuesToOriginal(std::vector<Objective<double>> objectives,
                                                                               std::vector<storm::RationalNumber> const& point);
template std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> transformObjectivePolytopeToOriginal(
    std::vector<Objective<double>> objectives, std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> const& polytope);
template std::vector<storm::RationalNumber> transformObjectiveValuesToOriginal(std::vector<Objective<storm::RationalNumber>> objectives,
                                                                               std::vector<storm::RationalNumber> const& point);
template std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> transformObjectivePolytopeToOriginal(
    std::vector<Objective<storm::RationalNumber>> objectives, std::shared_ptr<storm::storage::geometry::Polytope<storm::RationalNumber>> const& polytope);

template void transformObjectiveSchedulersToOriginal(
    storm::storage::SparseModelMemoryProductReverseData const& reverseData,
    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalNumber>> const& originalModel,
    // std::shared_ptr<std::map<std::vector<storm::RationalNumber>, std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>>>> schedulers);
    std::map<std::vector<storm::RationalNumber>, std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>>> schedulers);

template void transformObjectiveSchedulersToOriginal(
    storm::storage::SparseModelMemoryProductReverseData const& reverseData, std::shared_ptr<storm::models::sparse::Mdp<double>> const& originalModel,
    // std::shared_ptr<std::map<std::vector<double>, std::shared_ptr<storm::storage::Scheduler<double>>>> schedulers);
    std::map<std::vector<double>, std::shared_ptr<storm::storage::Scheduler<double>>> schedulers);

template void transformObjectiveSchedulersToOriginal(
    storm::storage::SparseModelMemoryProductReverseData const& reverseData,
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>> const& originalModel,
    // std::shared_ptr<std::map<std::vector<storm::RationalNumber>, std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>>>> schedulers);
    std::map<std::vector<storm::RationalNumber>, std::shared_ptr<storm::storage::Scheduler<storm::RationalNumber>>> schedulers);

template void transformObjectiveSchedulersToOriginal(
    storm::storage::SparseModelMemoryProductReverseData const& reverseData,
    std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> const& originalModel,
    // std::shared_ptr<std::map<std::vector<double>, std::shared_ptr<storm::storage::Scheduler<double>>>> schedulers);
    std::map<std::vector<double>, std::shared_ptr<storm::storage::Scheduler<double>>> schedulers);

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm