#include "storm-pars/api/synthesis.h"

#include <set>
#include <utility>

#include "storm-pars/analysis/RegionConstraintChecker.h"
#include "storm-pars/api/region.h"
#include "storm-pars/derivative/GradientDescentInstantiationSearcher.h"
#include "storm-pars/modelchecker/instantiation/SparseDtmcInstantiationModelChecker.h"
#include "storm-pars/modelchecker/instantiation/SparseMdpInstantiationModelChecker.h"
#include "storm-pars/scp/SparseScpInstantiationSearcher.h"
#include "storm/api/verification.h"
#include "storm/exceptions/AbortException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/utility/constants.h"

namespace storm::api {
namespace {

using Model = storm::models::sparse::Model<storm::RationalFunction>;
using Dtmc = storm::models::sparse::Dtmc<storm::RationalFunction>;
using Mdp = storm::models::sparse::Mdp<storm::RationalFunction>;
using Region = storm::storage::ParameterRegion<storm::RationalFunction>;
using Coefficient = Region::CoefficientType;
using Engine = storm::modelchecker::RegionCheckEngine;
using Status = storm::modelchecker::RegionResult;
template<typename ConstantType>
using Result = storm::pars::FeasibilitySynthesisResult<ConstantType>;
using GraphStatus = storm::pars::analysis::GraphPreservationStatus;

void validateInput(std::shared_ptr<Model> const& model, std::shared_ptr<storm::logic::Formula const> const& formula,
                   std::optional<Engine> const& regionEngine = std::nullopt) {
    STORM_LOG_THROW(model && formula, storm::exceptions::InvalidArgumentException, "A model and formula are required.");
    STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc) || model->isOfType(storm::models::ModelType::Mdp), storm::exceptions::NotSupportedException,
                    "Parameter synthesis supports sparse DTMCs and MDPs only.");
    STORM_LOG_THROW(model->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException,
                    "Parameter synthesis requires exactly one initial state.");
    STORM_LOG_THROW((formula->isProbabilityOperatorFormula() || formula->isRewardOperatorFormula()) && formula->asOperatorFormula().hasBound(),
                    storm::exceptions::NotSupportedException, "Parameter synthesis requires a bounded probability or reward operator formula.");
    if (regionEngine == Engine::RobustParameterLifting) {
        STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc), storm::exceptions::NotSupportedException, "Robust PLA supports DTMCs only.");
    }
}

std::pair<Region, GraphStatus> resolveRegion(Model const& model, std::optional<Region> const& region, std::optional<Coefficient> const& regionBound,
                                             bool requireGraphPreservation) {
    STORM_LOG_THROW(!region || !regionBound, storm::exceptions::InvalidArgumentException, "Provide either a region or a region bound, not both.");
    auto const parameters = getModelParameters<storm::RationalFunction>(model);
    Region resolved;
    if (region) {
        resolved = *region;
    } else if (regionBound) {
        Coefficient const one = storm::utility::one<Coefficient>();
        STORM_LOG_THROW(*regionBound >= storm::utility::zero<Coefficient>() && *regionBound <= one / 2, storm::exceptions::InvalidArgumentException,
                        "The region bound must be between 0 and 1/2, inclusive.");
        Region::Valuation lower, upper;
        for (auto const& parameter : parameters) {
            lower.emplace(parameter, *regionBound);
            upper.emplace(parameter, one - *regionBound);
        }
        resolved = Region(std::move(lower), std::move(upper));
    } else {
        STORM_LOG_THROW(parameters.empty(), storm::exceptions::InvalidArgumentException, "Provide a region or a region bound for a model with parameters.");
    }
    for (auto const& variable : parameters) {
        STORM_LOG_THROW(resolved.getVariables().count(variable), storm::exceptions::InvalidArgumentException,
                        "The region does not cover model parameter '" << variable.name() << "'.");
    }
    if (!requireGraphPreservation) {
        return {std::move(resolved), GraphStatus::None};
    }
    storm::analysis::ConstraintCollector collector(model);
    storm::pars::analysis::RegionConstraintChecker checker(collector);
    STORM_LOG_THROW(checker.isGraphPreserving(resolved), storm::exceptions::NotSupportedException,
                    "This engine requires a well-defined, graph-preserving region. Provide an explicit graph-preserving region or use robust PLA for a DTMC. "
                    "The region's endpoints are not removed automatically.");
    return {std::move(resolved), GraphStatus::GraphPreserving};
}

std::shared_ptr<storm::logic::Formula> normalizedFormula(Model const& model, storm::logic::Formula const& formula) {
    auto result = formula.clone();
    auto& op = result->asOperatorFormula();
    // Removing the bound must not lose the default universal scheduler quantification of a threshold property.
    if (model.isOfType(storm::models::ModelType::Mdp) && !op.hasOptimalityType()) {
        op.setOptimalityType(op.getBound().isLowerBound() ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
    }
    return result;
}

std::shared_ptr<storm::logic::Formula> quantitativeFormula(Model const& model, storm::logic::Formula const& formula) {
    auto result = normalizedFormula(model, formula);
    result->asOperatorFormula().removeBound();
    return result;
}

std::shared_ptr<storm::pars::FeasibilitySynthesisTask const> createFeasibilityTask(Model const& model, storm::logic::Formula const& formula,
                                                                                   Region const& region) {
    auto task = std::make_shared<storm::pars::FeasibilitySynthesisTask>(quantitativeFormula(model, formula));
    task->setBound(formula.asOperatorFormula().getBound());
    task->setRegion(region);
    return task;
}

template<typename ConstantType>
storm::utility::ExtendedValueType<ConstantType> evaluate(storm::Environment const& env, Model const& model, storm::logic::Formula const& formula,
                                                         Region::Valuation const& valuation) {
    auto quantitative = quantitativeFormula(model, formula);
    auto task = createTask<storm::RationalFunction>(quantitative, true);
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (model.isOfType(storm::models::ModelType::Dtmc)) {
        storm::modelchecker::SparseDtmcInstantiationModelChecker<Dtmc, ConstantType> checker(env, dynamic_cast<Dtmc const&>(model));
        checker.specifyFormula(task);
        result = checker.check(env, valuation);
    } else {
        storm::modelchecker::SparseMdpInstantiationModelChecker<Mdp, ConstantType> checker(env, dynamic_cast<Mdp const&>(model));
        checker.specifyFormula(task);
        result = checker.check(env, valuation);
    }
    return result->asExplicitQuantitativeCheckResult<ConstantType>()[model.getInitialStates().getNextSetIndex(0)];
}

template<typename ConstantType>
Result<ConstantType> validateWitness(storm::Environment const& env, Model const& model, storm::logic::Formula const& formula, Region const& region,
                                     Region::Valuation valuation) {
    // Complete omitted parameters and correct floating-point rounding at the exact region boundaries.
    for (auto const& variable : region.getVariables()) {
        auto [it, inserted] = valuation.emplace(variable, region.getCenter(variable));
        it->second = storm::utility::max(region.getLowerBoundary(variable), storm::utility::min(region.getUpperBoundary(variable), it->second));
    }
    auto value = evaluate<ConstantType>(env, model, formula, valuation);
    if (!formula.asOperatorFormula().getBound().isSatisfied(value)) {
        return {};
    }
    return {Status::ExistsSat, storm::pars::FeasibilityResult<ConstantType>{std::move(value), std::move(valuation)}};
}

}  // namespace

bool verify(storm::Environment const& env, std::shared_ptr<Model> const& model, std::shared_ptr<storm::logic::Formula const> const& formula,
            std::optional<Region> const& region, std::optional<Coefficient> const& regionBound, Engine engine) {
    validateInput(model, formula, engine);
    auto const [consideredRegion, graphStatus] = resolveRegion(*model, region, regionBound, engine != Engine::RobustParameterLifting);
    auto normalized = normalizedFormula(*model, *formula);
    std::set<Region::VariableType> discreteVariables;
    RefinementOptions<storm::RationalFunction> settings(model, *normalized, engine, storm::modelchecker::RegionSplittingStrategy(), MonotonicitySetting(),
                                                        discreteVariables, true, graphStatus);
    return verifyRegion(env, settings, consideredRegion);
}

Result<double> checkFeasibility(storm::Environment const& env, std::shared_ptr<Model> const& model, std::shared_ptr<storm::logic::Formula const> const& formula,
                                std::optional<Region> const& region, std::optional<Coefficient> const& regionBound, storm::pars::FeasibilityMethod method,
                                Engine regionEngine) {
    try {
        switch (method) {
            case storm::pars::FeasibilityMethod::PLA: {
                validateInput(model, formula, regionEngine);
                auto const [consideredRegion, graphStatus] = resolveRegion(*model, region, regionBound, regionEngine != Engine::RobustParameterLifting);
                auto normalized = normalizedFormula(*model, *formula);
                std::set<Region::VariableType> discreteVariables;
                RefinementOptions<storm::RationalFunction> settings(model, *normalized, regionEngine, storm::modelchecker::RegionSplittingStrategy(),
                                                                    MonotonicitySetting(), discreteVariables, true, graphStatus);
                auto checker = initializeRegionRefinementChecker(env, settings);
                auto const& bound = normalized->asOperatorFormula().getBound();
                auto acceptGlobalBound = [&](auto const&, auto const& globalBound) { return !bound.isSatisfied(globalBound); };
                auto rejectInstance = [&](auto const& value) { return bound.isSatisfied(value); };
                auto direction = bound.isLowerBound() ? storm::solver::OptimizationDirection::Maximize : storm::solver::OptimizationDirection::Minimize;
                auto result = checker->computeExtremalValueHelper(env, consideredRegion, direction, acceptGlobalBound, rejectInstance);
                if (bound.isSatisfied(result.first)) {
                    return validateWitness<double>(env, *model, *formula, consideredRegion, std::move(result.second));
                }
                return {Status::AllViolated, std::nullopt};
            }
            case storm::pars::FeasibilityMethod::GD: {
                validateInput(model, formula);
                STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc), storm::exceptions::NotSupportedException,
                                "Gradient descent supports DTMCs only.");
                auto const [consideredRegion, graphStatus] = resolveRegion(*model, region, regionBound, true);
                auto task = createFeasibilityTask(*model, *formula, consideredRegion);
                storm::derivative::GradientDescentInstantiationSearcher<storm::RationalFunction, double> searcher(
                    env, *model->as<Dtmc>(), storm::derivative::GradientDescentMethod::ADAM, 0.1, 0.9, 0.999, 32, 1e-6, consideredRegion.getCenterPoint(),
                    storm::derivative::GradientDescentConstraintMethod::PROJECT_WITH_GRADIENT, consideredRegion);
                searcher.setup(env, task);
                auto result = searcher.gradientDescent();
                return validateWitness<double>(env, *model, *formula, consideredRegion, std::move(result.valuation));
            }
            case storm::pars::FeasibilityMethod::SCP: {
                validateInput(model, formula);
                auto const& op = formula->asOperatorFormula();
                if (model->isOfType(storm::models::ModelType::Mdp) && op.hasOptimalityType()) {
                    auto schedulerDirection =
                        op.getBound().isLowerBound() ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;
                    STORM_LOG_THROW(op.getOptimalityType() == schedulerDirection, storm::exceptions::NotSupportedException,
                                    "SCP requires universal scheduler quantification on MDPs: lower bounds use min and upper bounds use max.");
                }
                auto const [consideredRegion, graphStatus] = resolveRegion(*model, region, regionBound, true);
                auto task = createFeasibilityTask(*model, *formula, consideredRegion);
                storm::pars::scp::ScpResult<double> result;
                if (model->isOfType(storm::models::ModelType::Dtmc)) {
                    result = storm::pars::scp::SparseScpInstantiationSearcher<Dtmc, double>(*model->as<Dtmc>()).run(env, task);
                } else {
                    STORM_LOG_ASSERT(model->isOfType(storm::models::ModelType::Mdp), "Expected an MDP.");
                    result = storm::pars::scp::SparseScpInstantiationSearcher<Mdp, double>(*model->as<Mdp>()).run(env, task);
                }
                if (result.status != storm::pars::scp::ScpStatus::Feasible) {
                    return {};
                }
                return validateWitness<double>(env, *model, *formula, consideredRegion, std::move(result.valuation));
            }
        }
        STORM_LOG_THROW_UNCONDITIONALLY(storm::exceptions::InvalidArgumentException, "Unknown feasibility method.");
    } catch (storm::exceptions::AbortException const&) {
        return {};
    }
}

}  // namespace storm::api
