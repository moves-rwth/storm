#pragma once

#include <boost/optional.hpp>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/Dimension.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/EpochManager.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/EpochModel.h"
#include "storm/modelchecker/prctl/helper/rewardbounded/ProductModel.h"
#include "storm/models/sparse/Model.h"
#include "storm/solver/LinearEquationSolverProblemFormat.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/vector.h"

namespace storm {
namespace modelchecker {
namespace helper {
namespace rewardbounded {

template<typename ValueType, bool SingleObjectiveMode>
class MultiDimensionalRewardUnfolding {
   public:
    typedef typename EpochManager::Epoch Epoch;  // The number of reward steps that are "left" for each dimension
    typedef typename EpochManager::EpochClass EpochClass;

    typedef typename std::conditional<SingleObjectiveMode, ValueType, std::vector<ValueType>>::type SolutionType;

    /*
     *
     * @param model The (preprocessed) model
     * @param objectives The (preprocessed) objectives
     *
     */
    MultiDimensionalRewardUnfolding(storm::models::sparse::Model<ValueType> const& model,
                                    std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> const& objectives);

    /*!
     * Initializes the reward unfolding with just a single objective.
     *
     * @param model The input model
     * @param objectiveFormula the formula
     * @param infinityBoundVariables if non-empty, reward bounds with these variables are assumed to approach infinity
     */
    MultiDimensionalRewardUnfolding(storm::models::sparse::Model<ValueType> const& model, std::shared_ptr<storm::logic::OperatorFormula const> objectiveFormula,
                                    std::set<storm::expressions::Variable> const& infinityBoundVariables = {});

    ~MultiDimensionalRewardUnfolding() = default;

    /*!
     * Retrieves the desired epoch that needs to be analyzed to compute the reward bounded values.
     * @param setUnknownDimsToBottom if true, dimensions for which no maxValue is known are set to the bottom dimension. If false, an exception is thrown if
     * there are unknown maxValues.
     */
    Epoch getStartEpoch(bool setUnknownDimsToBottom = false);

    /*!
     * Computes a sequence of epochs that need to be analyzed to get a result at the start epoch.
     * @param stopAtComputedEpochs if set, the search for epochs that need to be computed is stopped at epochs that already have been computed earlier.
     */
    std::vector<Epoch> getEpochComputationOrder(Epoch const& startEpoch, bool stopAtComputedEpochs = false);

    EpochModel<ValueType, SingleObjectiveMode>& setCurrentEpoch(Epoch const& epoch);

    void setEquationSystemFormatForEpochModel(storm::solver::LinearEquationSolverProblemFormat eqSysFormat);

    /*!
     * Returns the precision required for the analyzis of each epoch model in order to achieve the given overall precision
     */
    ValueType getRequiredEpochModelPrecision(Epoch const& startEpoch, ValueType const& precision);

    /*!
     * Returns an upper/lower bound for the objective result in every state (if this bound could be computed)
     */
    boost::optional<ValueType> getUpperObjectiveBound(uint64_t objectiveIndex = 0);
    boost::optional<ValueType> getLowerObjectiveBound(uint64_t objectiveIndex = 0);

    void setSolutionForCurrentEpoch(std::vector<SolutionType>&& inStateSolutions);
    SolutionType getInitialStateResult(Epoch const& epoch);  // Assumes that the initial state is unique
    SolutionType getInitialStateResult(Epoch const& epoch, uint64_t initialStateIndex);

    EpochManager const& getEpochManager() const;
    Dimension<ValueType> const& getDimension(uint64_t dim) const;

   private:
    void setCurrentEpochClass(Epoch const& epoch);
    void initialize(std::set<storm::expressions::Variable> const& infinityBoundVariables = {});

    void initializeObjectives(std::vector<Epoch>& epochSteps, std::set<storm::expressions::Variable> const& infinityBoundVariables);
    void computeMaxDimensionValues();
    void translateLowerBoundInfinityDimensions(std::vector<Epoch>& epochSteps);

    void initializeMemoryProduct(std::vector<Epoch> const& epochSteps);

    template<bool SO = SingleObjectiveMode, typename std::enable_if<SO, int>::type = 0>
    SolutionType getScaledSolution(SolutionType const& solution, ValueType const& scalingFactor) const;
    template<bool SO = SingleObjectiveMode, typename std::enable_if<!SO, int>::type = 0>
    SolutionType getScaledSolution(SolutionType const& solution, ValueType const& scalingFactor) const;

    template<bool SO = SingleObjectiveMode, typename std::enable_if<SO, int>::type = 0>
    void addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const;
    template<bool SO = SingleObjectiveMode, typename std::enable_if<!SO, int>::type = 0>
    void addScaledSolution(SolutionType& solution, SolutionType const& solutionToAdd, ValueType const& scalingFactor) const;

    template<bool SO = SingleObjectiveMode, typename std::enable_if<SO, int>::type = 0>
    void setSolutionEntry(SolutionType& solution, uint64_t objIndex, ValueType const& value) const;
    template<bool SO = SingleObjectiveMode, typename std::enable_if<!SO, int>::type = 0>
    void setSolutionEntry(SolutionType& solution, uint64_t objIndex, ValueType const& value) const;

    template<bool SO = SingleObjectiveMode, typename std::enable_if<SO, int>::type = 0>
    std::string solutionToString(SolutionType const& solution) const;
    template<bool SO = SingleObjectiveMode, typename std::enable_if<!SO, int>::type = 0>
    std::string solutionToString(SolutionType const& solution) const;

    SolutionType const& getStateSolution(Epoch const& epoch, uint64_t const& productState);
    struct EpochSolution {
        uint64_t count;
        std::shared_ptr<std::vector<uint64_t> const> productStateToSolutionVectorMap;
        std::vector<SolutionType> solutions;
    };
    std::map<Epoch, EpochSolution> epochSolutions;
    EpochSolution const& getEpochSolution(std::map<Epoch, EpochSolution const*> const& solutions, Epoch const& epoch);
    SolutionType const& getStateSolution(EpochSolution const& epochSolution, uint64_t const& productState);

    storm::models::sparse::Model<ValueType> const& model;
    std::vector<storm::modelchecker::multiobjective::Objective<ValueType>> objectives;

    std::unique_ptr<ProductModel<ValueType>> productModel;

    std::vector<uint64_t> epochModelToProductChoiceMap;
    std::shared_ptr<std::vector<uint64_t> const> productStateToEpochModelInStateMap;
    std::set<Epoch> possibleEpochSteps;

    EpochModel<ValueType, SingleObjectiveMode> epochModel;
    boost::optional<Epoch> currentEpoch;

    EpochManager epochManager;

    std::vector<Dimension<ValueType>> dimensions;
    std::vector<storm::storage::BitVector> objectiveDimensions;
};
}  // namespace rewardbounded
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm