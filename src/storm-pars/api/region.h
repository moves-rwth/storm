#pragma once

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "storm-pars/modelchecker/region/AnnotatedRegion.h"
#include "storm-pars/modelchecker/region/RegionCheckEngine.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/modelchecker/region/RegionRefinementChecker.h"
#include "storm-pars/modelchecker/region/RegionResultHypothesis.h"
#include "storm-pars/modelchecker/region/RegionSplitEstimateKind.h"
#include "storm-pars/modelchecker/region/RegionSplittingStrategy.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseMdpParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/ValidatingSparseParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/monotonicity/MonotonicityBackend.h"
#include "storm-pars/modelchecker/region/monotonicity/OrderBasedMonotonicityBackend.h"
#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/results/RegionRefinementCheckResult.h"
#include "storm-pars/parser/MonotonicityParser.h"
#include "storm-pars/parser/ParameterRegionParser.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/utility/parameterlifting.h"

#include "storm/environment/Environment.h"

#include "storm/api/properties.h"
#include "storm/api/transformation.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/io/file.h"
#include "storm/models/sparse/Model.h"

namespace storm {

namespace api {
struct MonotonicitySetting {
    bool useMonotonicity;
    bool useOnlyGlobalMonotonicity;
    bool useBoundsFromPLA;

    explicit MonotonicitySetting(bool useMonotonicity = false, bool useOnlyGlobalMonotonicity = false, bool useBoundsFromPLA = false) {
        this->useMonotonicity = useMonotonicity;
        this->useOnlyGlobalMonotonicity = useOnlyGlobalMonotonicity;
        this->useBoundsFromPLA = useBoundsFromPLA;
    }
};

template<typename ValueType>
std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(
    std::string const& inputString, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> const& consideredVariables) {
    // If the given input string looks like a file (containing a dot and there exists a file with that name),
    // we try to parse it as a file, otherwise we assume it's a region string.
    if (inputString.find(".") != std::string::npos && std::ifstream(inputString).good()) {
        return storm::parser::ParameterRegionParser<ValueType>().parseMultipleRegionsFromFile(inputString, consideredVariables);
    } else {
        return storm::parser::ParameterRegionParser<ValueType>().parseMultipleRegions(inputString, consideredVariables);
    }
}

template<typename ValueType>
storm::storage::ParameterRegion<ValueType> createRegion(
    std::string const& inputString, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> const& consideredVariables) {
    return storm::parser::ParameterRegionParser<ValueType>().createRegion(inputString, consideredVariables);
}

template<typename ValueType>
std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::string const& inputString, storm::models::ModelBase const& model) {
    std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> modelParameters;
    if (model.isSparseModel()) {
        auto const& sparseModel = dynamic_cast<storm::models::sparse::Model<ValueType> const&>(model);
        modelParameters = storm::models::sparse::getProbabilityParameters(sparseModel);
        auto rewParameters = storm::models::sparse::getRewardParameters(sparseModel);
        modelParameters.insert(rewParameters.begin(), rewParameters.end());
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Retrieving model parameters is not supported for the given model type.");
    }
    return parseRegions<ValueType>(inputString, modelParameters);
}

template<typename ValueType>
std::vector<storm::storage::ParameterRegion<ValueType>> createRegion(std::string const& inputString, storm::models::ModelBase const& model) {
    std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> modelParameters;
    if (model.isSparseModel()) {
        auto const& sparseModel = dynamic_cast<storm::models::sparse::Model<ValueType> const&>(model);
        modelParameters = storm::models::sparse::getProbabilityParameters(sparseModel);
        auto rewParameters = storm::models::sparse::getRewardParameters(sparseModel);
        modelParameters.insert(rewParameters.begin(), rewParameters.end());
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Retrieving model parameters is not supported for the given model type.");
    }
    return std::vector<storm::storage::ParameterRegion<ValueType>>({createRegion<ValueType>(inputString, modelParameters)});
}

template<typename ValueType>
storm::storage::ParameterRegion<ValueType> parseRegion(std::string const& inputString,
                                                       std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> const& consideredVariables) {
    // Handle the "empty region" case
    if (inputString == "" && consideredVariables.empty()) {
        return storm::storage::ParameterRegion<ValueType>();
    }

    auto res = parseRegions<ValueType>(inputString, consideredVariables);
    STORM_LOG_THROW(res.size() == 1, storm::exceptions::InvalidOperationException, "Parsed " << res.size() << " regions but exactly one was expected.");
    return res.front();
}

template<typename ValueType>
storm::storage::ParameterRegion<ValueType> parseRegion(std::string const& inputString, storm::models::ModelBase const& model) {
    // Handle the "empty region" case
    if (inputString == "" && !model.hasParameters()) {
        return storm::storage::ParameterRegion<ValueType>();
    }

    auto res = parseRegions<ValueType>(inputString, model);
    STORM_LOG_THROW(res.size() == 1, storm::exceptions::InvalidOperationException, "Parsed " << res.size() << " regions but exactly one was expected.");
    return res.front();
}

template<typename ValueType>
std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>,
          std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>
parseMonotoneParameters(std::string const& fileName, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model) {
    std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> modelParameters;
    modelParameters = storm::models::sparse::getProbabilityParameters(*model);
    auto rewParameters = storm::models::sparse::getRewardParameters(*model);
    modelParameters.insert(rewParameters.begin(), rewParameters.end());
    return std::move(storm::parser::MonotonicityParser<typename storm::storage::ParameterRegion<ValueType>::VariableType>().parseMonotoneVariablesFromFile(
        fileName, modelParameters));
}

template<typename ParametricType>
std::shared_ptr<storm::models::sparse::Model<ParametricType>> preprocessSparseModelForParameterLifting(
    std::shared_ptr<storm::models::sparse::Model<ParametricType>> const& model,
    storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> const& task, bool preconditionsValidatedManually = false) {
    STORM_LOG_WARN_COND(preconditionsValidatedManually || storm::utility::parameterlifting::validateParameterLiftingSound(*model, task.getFormula()),
                        "Could not validate whether parameter lifting is applicable. Please validate manually...");
    std::shared_ptr<storm::models::sparse::Model<ParametricType>> consideredModel = model;

    // Treat continuous time models
    if (consideredModel->isOfType(storm::models::ModelType::Ctmc) || consideredModel->isOfType(storm::models::ModelType::MarkovAutomaton)) {
        STORM_LOG_WARN("Parameter lifting not supported for continuous time models. Transforming continuous model to discrete model...");
        std::vector<std::shared_ptr<storm::logic::Formula const>> taskFormulaAsVector{task.getFormula().asSharedPointer()};
        consideredModel = storm::api::transformContinuousToDiscreteTimeSparseModel(consideredModel, taskFormulaAsVector).first;
        STORM_LOG_THROW(consideredModel->isOfType(storm::models::ModelType::Dtmc) || consideredModel->isOfType(storm::models::ModelType::Mdp),
                        storm::exceptions::UnexpectedException, "Transformation to discrete time model has failed.");
    }
    return consideredModel;
}

template<typename ParametricType, typename ImpreciseType = double, typename PreciseType = storm::RationalNumber>
std::unique_ptr<storm::modelchecker::RegionModelChecker<ParametricType>> createRegionModelChecker(storm::modelchecker::RegionCheckEngine engine,
                                                                                                  storm::models::ModelType modelType) {
    STORM_LOG_THROW(modelType == storm::models::ModelType::Dtmc || modelType == storm::models::ModelType::Mdp, storm::exceptions::NotSupportedException,
                    "Unable to create a region checker for the provided model type.");

    switch (engine) {
        case storm::modelchecker::RegionCheckEngine::ParameterLifting:
            if (modelType == storm::models::ModelType::Dtmc) {
                return std::make_unique<
                    storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<ParametricType>, ImpreciseType>>();
            } else {
                return std::make_unique<
                    storm::modelchecker::SparseMdpParameterLiftingModelChecker<storm::models::sparse::Mdp<ParametricType>, ImpreciseType>>();
            }
        case storm::modelchecker::RegionCheckEngine::ExactParameterLifting:
            if (modelType == storm::models::ModelType::Dtmc) {
                return std::make_unique<
                    storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<ParametricType>, PreciseType>>();
            } else {
                return std::make_unique<storm::modelchecker::SparseMdpParameterLiftingModelChecker<storm::models::sparse::Mdp<ParametricType>, PreciseType>>();
            }
        case storm::modelchecker::RegionCheckEngine::RobustParameterLifting:
            return std::make_unique<storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<ParametricType>, ImpreciseType, true>>();
        case storm::modelchecker::RegionCheckEngine::ValidatingParameterLifting:
            if (modelType == storm::models::ModelType::Dtmc) {
                return std::make_unique<storm::modelchecker::ValidatingSparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<ParametricType>,
                                                                                                          ImpreciseType, PreciseType>>();
            } else {
                return std::make_unique<storm::modelchecker::ValidatingSparseParameterLiftingModelChecker<storm::models::sparse::Mdp<ParametricType>,
                                                                                                          ImpreciseType, PreciseType>>();
            }
        default:
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected region model checker type.");
    }
    return nullptr;
}

template<typename ParametricType, typename ImpreciseType = double, typename PreciseType = storm::RationalNumber>
std::unique_ptr<storm::modelchecker::MonotonicityBackend<ParametricType>> initializeMonotonicityBackend(
    storm::modelchecker::RegionModelChecker<ParametricType> const& regionChecker, storm::modelchecker::RegionCheckEngine engine,
    storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> const& task, MonotonicitySetting const& monotonicitySetting,
    std::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>,
                            std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>>>
        monotoneParameters = std::nullopt) {
    // Initialize default backend
    auto monotonicityBackend = std::make_unique<storm::modelchecker::MonotonicityBackend<ParametricType>>();

    // Potentially replace default by order-based monotonicity
    if (monotonicitySetting.useMonotonicity) {
        std::unique_ptr<storm::modelchecker::MonotonicityBackend<ParametricType>> orderBasedBackend;
        if (engine == storm::modelchecker::RegionCheckEngine::ExactParameterLifting) {
            orderBasedBackend = std::make_unique<storm::modelchecker::OrderBasedMonotonicityBackend<ParametricType, PreciseType>>(
                monotonicitySetting.useOnlyGlobalMonotonicity, monotonicitySetting.useBoundsFromPLA);
        } else {
            orderBasedBackend = std::make_unique<storm::modelchecker::OrderBasedMonotonicityBackend<ParametricType, ImpreciseType>>(
                monotonicitySetting.useOnlyGlobalMonotonicity, monotonicitySetting.useBoundsFromPLA);
        }
        if (regionChecker.isMonotonicitySupported(*orderBasedBackend, task)) {
            monotonicityBackend = std::move(orderBasedBackend);
        } else {
            STORM_LOG_WARN("Order-based Monotonicity enabled for region checking engine " << engine << " but not supported in this configuration.");
        }
    }

    // Insert monotone parameters if available
    if (monotoneParameters) {
        for (auto const& incrPar : monotoneParameters->first) {
            monotonicityBackend->setMonotoneParameter(incrPar, storm::analysis::MonotonicityKind::Incr);
        }
        for (auto const& decrPar : monotoneParameters->second) {
            monotonicityBackend->setMonotoneParameter(decrPar, storm::analysis::MonotonicityKind::Decr);
        }
    }
    return monotonicityBackend;
}

template<typename ParametricType>
storm::modelchecker::RegionSplittingStrategy initializeSplittingStrategy(storm::modelchecker::RegionModelChecker<ParametricType> const& regionChecker,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> const& task,
                                                                         storm::modelchecker::RegionSplittingStrategy::Heuristic heuristic,
                                                                         std::optional<storm::modelchecker::RegionSplitEstimateKind> estimateKind,
                                                                         std::optional<uint64_t> maxSplitsPerStepThreshold = std::nullopt) {
    storm::modelchecker::RegionSplittingStrategy strat;
    if (maxSplitsPerStepThreshold) {
        strat.maxSplitDimensions = *maxSplitsPerStepThreshold;
    }
    strat.heuristic = heuristic;
    strat.estimateKind = estimateKind;

    if (strat.heuristic == storm::modelchecker::RegionSplittingStrategy::Heuristic::EstimateBased) {
        if (!strat.estimateKind) {
            strat.estimateKind = regionChecker.getDefaultRegionSplitEstimateKind(task);
        }
        STORM_LOG_THROW(regionChecker.isRegionSplitEstimateKindSupported(*strat.estimateKind, task), storm::exceptions::NotSupportedException,
                        "The chosen region split estimate kind is not supported by the region model checker.");
    }

    return strat;
}

template<typename ValueType, typename ImpreciseType = double, typename PreciseType = storm::RationalNumber>
std::unique_ptr<storm::modelchecker::RegionModelChecker<ValueType>> initializeRegionModelChecker(
    Environment const& env, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::modelchecker::RegionCheckEngine engine,
    bool allowModelSimplification = true, bool preconditionsValidated = false, MonotonicitySetting monotonicitySetting = MonotonicitySetting(),
    std::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>,
                            std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>>
        monotoneParameters = std::nullopt) {
    auto consideredModel = preprocessSparseModelForParameterLifting(model, task, preconditionsValidated);
    auto regionChecker = createRegionModelChecker<ValueType, ImpreciseType, PreciseType>(engine, model->getType());
    auto monotonicityBackend =
        initializeMonotonicityBackend<ValueType, ImpreciseType, PreciseType>(*regionChecker, engine, task, monotonicitySetting, monotoneParameters);
    if (allowModelSimplification) {
        allowModelSimplification = monotonicityBackend->recommendModelSimplifications();
        STORM_LOG_WARN_COND(allowModelSimplification, "Model simplification is disabled because the monotonicity algorithm does not recommend it.");
    }
    regionChecker->specify(env, consideredModel, task, std::nullopt, std::move(monotonicityBackend), allowModelSimplification);
    return regionChecker;
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::RegionModelChecker<ValueType>> initializeRegionModelChecker(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task,
    storm::modelchecker::RegionCheckEngine engine) {
    Environment env;
    return initializeRegionModelChecker(env, model, task, engine);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::RegionCheckResult<ValueType>> checkRegionsWithSparseEngine(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task,
    std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, storm::modelchecker::RegionCheckEngine engine,
    std::vector<storm::modelchecker::RegionResultHypothesis> const& hypotheses, bool sampleVerticesOfRegions) {
    Environment env;
    auto regionChecker = initializeRegionModelChecker(env, model, task, engine);
    return regionChecker->analyzeRegions(env, regions, hypotheses, sampleVerticesOfRegions);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::RegionCheckResult<ValueType>> checkRegionsWithSparseEngine(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task,
    std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, storm::modelchecker::RegionCheckEngine engine,
    storm::modelchecker::RegionResultHypothesis const& hypothesis = storm::modelchecker::RegionResultHypothesis::Unknown,
    bool sampleVerticesOfRegions = false) {
    std::vector<storm::modelchecker::RegionResultHypothesis> hypotheses(regions.size(), hypothesis);
    return checkRegionsWithSparseEngine(model, task, regions, engine, hypotheses, sampleVerticesOfRegions);
}

template<typename ValueType, typename ImpreciseType = double, typename PreciseType = storm::RationalNumber>
std::unique_ptr<storm::modelchecker::RegionRefinementChecker<ValueType>> initializeRegionRefinementChecker(
    Environment const& env, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::modelchecker::RegionCheckEngine engine,
    storm::modelchecker::RegionSplittingStrategy::Heuristic heuristic = storm::modelchecker::RegionSplittingStrategy::Heuristic::EstimateBased,
    std::optional<storm::modelchecker::RegionSplitEstimateKind> estimateKind = storm::modelchecker::RegionSplitEstimateKind::Distance,
    std::optional<uint64_t> maxSplitsPerStepThreshold = std::nullopt, bool allowModelSimplification = true,
    bool preconditionsValidated = false, MonotonicitySetting monotonicitySetting = MonotonicitySetting(),
    std::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>,
                            std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>>
        monotoneParameters = std::nullopt) {
    auto consideredModel = preprocessSparseModelForParameterLifting(model, task, preconditionsValidated);
    auto regionChecker = createRegionModelChecker<ValueType, ImpreciseType, PreciseType>(engine, model->getType());
    auto monotonicityBackend =
        initializeMonotonicityBackend<ValueType, ImpreciseType, PreciseType>(*regionChecker, engine, task, monotonicitySetting, monotoneParameters);
    auto splitStrat = initializeSplittingStrategy(*regionChecker, task, heuristic, estimateKind, maxSplitsPerStepThreshold);
    allowModelSimplification = allowModelSimplification && monotonicityBackend->recommendModelSimplifications();
    auto refinementChecker = std::make_unique<storm::modelchecker::RegionRefinementChecker<ValueType>>(std::move(regionChecker));
    refinementChecker->specify(env, consideredModel, task, splitStrat, std::move(monotonicityBackend), allowModelSimplification);
    return refinementChecker;
}

/*!
 * Checks and iteratively refines the given region with the sparse engine
 * @param engine The considered region checking engine
 * @param coverageThreshold if given, the refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then this
 * threshold
 * @param refinementDepthThreshold if given, the refinement stops at the given depth. depth=0 means no refinement.
 * @param hypothesis if not 'unknown', it is only checked whether the hypothesis holds (and NOT the complementary result).
 * @param allowModelSimplification
 * @param useMonotonicity
 * @param monThresh if given, determines at which depth to start using monotonicity
 */
template<typename ValueType>
std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ValueType>> checkAndRefineRegionWithSparseEngine(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task,
    storm::storage::ParameterRegion<ValueType> const& region, storm::modelchecker::RegionCheckEngine engine, std::optional<ValueType> const& coverageThreshold,
    std::optional<uint64_t> const& refinementDepthThreshold = std::nullopt,
    storm::modelchecker::RegionResultHypothesis hypothesis = storm::modelchecker::RegionResultHypothesis::Unknown, bool allowModelSimplification = true,
    storm::modelchecker::RegionSplittingStrategy::Heuristic splittingStrategy = storm::modelchecker::RegionSplittingStrategy::Heuristic::EstimateBased,
    std::optional<storm::modelchecker::RegionSplitEstimateKind> estimateKind = storm::modelchecker::RegionSplitEstimateKind::Distance,
    std::optional<uint64_t> const& maxSplitsPerStepThreshold = std::nullopt,
    MonotonicitySetting monotonicitySetting = MonotonicitySetting(), uint64_t monThresh = 0) {
    Environment env;
    // TODO: allow passing these settings? Maybe also pass monotone parameters?
    bool const preconditionsValidated = false;
    auto refinementChecker = initializeRegionRefinementChecker(env, model, task, engine, splittingStrategy, estimateKind, maxSplitsPerStepThreshold,
                                                               allowModelSimplification, preconditionsValidated, monotonicitySetting);
    return refinementChecker->performRegionPartitioning(env, region, coverageThreshold, refinementDepthThreshold, hypothesis, monThresh);
}

// TODO: update documentation
/*!
 * Finds the extremal value in the given region
 */
template<typename ValueType>
std::pair<storm::RationalNumber, typename storm::storage::ParameterRegion<ValueType>::Valuation> computeExtremalValue(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task,
    storm::storage::ParameterRegion<ValueType> const& region, storm::modelchecker::RegionCheckEngine engine, storm::solver::OptimizationDirection const& dir,
    std::optional<ValueType> const& precision, bool absolutePrecision, MonotonicitySetting const& monotonicitySetting,
    std::optional<storm::logic::Bound> const& boundInvariant,
    storm::modelchecker::RegionSplittingStrategy::Heuristic splittingStrategy = storm::modelchecker::RegionSplittingStrategy::Heuristic::RoundRobin,
    std::optional<storm::modelchecker::RegionSplitEstimateKind> estimateKind = storm::modelchecker::RegionSplitEstimateKind::Distance,
    std::optional<uint64_t> maxSplitsPerStepThreshold = std::nullopt) {
    Environment env;
    // TODO: allow passing these settings? Maybe also pass monotone parameters?
    bool const preconditionsValidated = false;
    bool const allowModelSimplification = true;
    auto refinementChecker = initializeRegionRefinementChecker(env, model, task, engine, splittingStrategy, estimateKind, maxSplitsPerStepThreshold,
                                                               allowModelSimplification, preconditionsValidated, monotonicitySetting);
    auto res =
        refinementChecker->computeExtremalValue(env, region, dir, precision.value_or(storm::utility::zero<ValueType>()), absolutePrecision, boundInvariant);
    return {storm::utility::convertNumber<storm::RationalNumber>(res.first), std::move(res.second)};
}

/*!
 * Verifies whether a region satisfies a property.
 */
template<typename ValueType>
bool verifyRegion(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::logic::Formula const& formula,
                  storm::storage::ParameterRegion<ValueType> const& region, storm::modelchecker::RegionCheckEngine engine,
                  MonotonicitySetting const& monotonicitySetting,
                  storm::modelchecker::RegionSplittingStrategy::Heuristic heuristic = storm::modelchecker::RegionSplittingStrategy::Heuristic::RoundRobin,
                  std::optional<storm::modelchecker::RegionSplitEstimateKind> estimateKind = storm::modelchecker::RegionSplitEstimateKind::Distance,
                  std::optional<uint64_t> maxSplitsPerStepThreshold = std::numeric_limits<uint64_t>::max()) {
    Environment env;
    STORM_LOG_THROW(formula.isProbabilityOperatorFormula() || formula.isRewardOperatorFormula(), storm::exceptions::NotSupportedException,
                    "Only probability and reward operators supported");
    STORM_LOG_THROW(formula.asOperatorFormula().hasBound(), storm::exceptions::NotSupportedException, "Verification requires a bounded operator formula.");

    storm::logic::Bound const& bound = formula.asOperatorFormula().getBound();
    std::shared_ptr<storm::logic::Formula> formulaWithoutBounds = formula.clone();
    formulaWithoutBounds->asOperatorFormula().removeBound();
    // TODO: allow passing these settings? Maybe also pass monotone parameters?
    bool preconditionsValidated = false;
    bool const allowModelSimplification = true;
    auto refinementChecker = initializeRegionRefinementChecker(
        env, model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>(*formulaWithoutBounds, true), engine, heuristic, estimateKind,
        maxSplitsPerStepThreshold, allowModelSimplification, preconditionsValidated, monotonicitySetting);
    return refinementChecker->verifyRegion(env, region, bound);
}

template<typename ValueType>
void exportRegionCheckResultToFile(std::unique_ptr<storm::modelchecker::CheckResult> const& checkResult, std::string const& filename,
                                   bool onlyConclusiveResults = false) {
    auto const* regionCheckResult = dynamic_cast<storm::modelchecker::RegionCheckResult<ValueType> const*>(checkResult.get());
    STORM_LOG_THROW(regionCheckResult != nullptr, storm::exceptions::UnexpectedException,
                    "Can not export region check result: The given checkresult does not have the expected type.");

    std::ofstream filestream;
    storm::utility::openFile(filename, filestream);
    for (auto const& res : regionCheckResult->getRegionResults()) {
        if (!onlyConclusiveResults || res.second == storm::modelchecker::RegionResult::AllViolated || res.second == storm::modelchecker::RegionResult::AllSat ||
            res.second == storm::modelchecker::RegionResult::AllIllDefined) {
            filestream << res.second << ": " << res.first << '\n';
        }
    }
}

}  // namespace api
}  // namespace storm
