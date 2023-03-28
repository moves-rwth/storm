#pragma once

#include <type_traits>

#include "storm/environment/Environment.h"

#include "storm/modelchecker/abstraction/BisimulationAbstractionRefinementModelChecker.h"
#include "storm/modelchecker/abstraction/GameBasedMdpModelChecker.h"
#include "storm/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/HybridMarkovAutomatonCslModelChecker.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "storm/modelchecker/exploration/SparseExplorationModelChecker.h"
#include "storm/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "storm/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "storm/modelchecker/rpatl/SparseSmgRpatlModelChecker.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/MarkovAutomaton.h"
#include "storm/models/symbolic/Mdp.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Smg.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/EliminationSettings.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace api {

template<typename ValueType>
storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> createTask(std::shared_ptr<const storm::logic::Formula> const& formula,
                                                                            bool onlyInitialStatesRelevant = false) {
    return storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>(*formula, onlyInitialStatesRelevant);
}

//
// Verifying with Abstraction Refinement engine
//
struct AbstractionRefinementOptions {
    AbstractionRefinementOptions() = default;
    AbstractionRefinementOptions(std::vector<storm::expressions::Expression>&& constraints,
                                 std::vector<std::vector<storm::expressions::Expression>>&& injectedRefinementPredicates)
        : constraints(std::move(constraints)), injectedRefinementPredicates(std::move(injectedRefinementPredicates)) {
        // Intentionally left empty.
    }

    std::vector<storm::expressions::Expression> constraints;
    std::vector<std::vector<storm::expressions::Expression>> injectedRefinementPredicates;
};

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithAbstractionRefinementEngine(storm::Environment const& env, storm::storage::SymbolicModelDescription const& model,
                                      storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task,
                                      AbstractionRefinementOptions const& options = AbstractionRefinementOptions()) {
    storm::modelchecker::GameBasedMdpModelCheckerOptions modelCheckerOptions(options.constraints, options.injectedRefinementPredicates);

    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::DTMC) {
        storm::modelchecker::GameBasedMdpModelChecker<DdType, storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(model, modelCheckerOptions);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    } else if (model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::MDP) {
        storm::modelchecker::GameBasedMdpModelChecker<DdType, storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(model, modelCheckerOptions);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "The model type " << model.getModelType() << " is not supported by the abstraction refinement engine.");
    }
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithAbstractionRefinementEngine(storm::Environment const& env, storm::storage::SymbolicModelDescription const&,
                                      storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&,
                                      AbstractionRefinementOptions const& = AbstractionRefinementOptions()) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Abstraction-refinement engine does not support data type.");
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithAbstractionRefinementEngine(
    storm::storage::SymbolicModelDescription const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task,
    AbstractionRefinementOptions const& options = AbstractionRefinementOptions()) {
    Environment env;
    return verifyWithAbstractionRefinementEngine<DdType, ValueType>(env, model, task, options);
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithAbstractionRefinementEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    model->getManager().execute([&]() {
        if (model->getType() == storm::models::ModelType::Dtmc) {
            storm::modelchecker::BisimulationAbstractionRefinementModelChecker<storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(
                *model->template as<storm::models::symbolic::Dtmc<DdType, double>>());
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(env, task);
            }
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            storm::modelchecker::BisimulationAbstractionRefinementModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(
                *model->template as<storm::models::symbolic::Mdp<DdType, double>>());
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(env, task);
            }
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                            "The model type " << model->getType() << " is not supported by the abstraction refinement engine.");
        }
    });
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<!std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithAbstractionRefinementEngine(
    storm::Environment const&, std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const&,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Abstraction-refinement engine does not support data type.");
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithAbstractionRefinementEngine(
    std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithAbstractionRefinementEngine<DdType, ValueType>(env, model, task);
}

//
// Verifying with Exploration engine
//
template<typename ValueType>
typename std::enable_if<std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithExplorationEngine(
    storm::Environment const& env, storm::storage::SymbolicModelDescription const& model,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    STORM_LOG_THROW(model.isPrismProgram(), storm::exceptions::NotSupportedException, "Exploration engine is currently only applicable to PRISM models.");
    storm::prism::Program const& program = model.asPrismProgram();

    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (program.getModelType() == storm::prism::Program::ModelType::DTMC) {
        storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Dtmc<ValueType>> checker(program);
        if (checker.canHandle(task)) {
            result = checker.check(env, task);
        }
    } else if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
        storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Mdp<ValueType>> checker(program);
        if (checker.canHandle(task)) {
            result = checker.check(env, task);
        }
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "The model type " << program.getModelType() << " is not supported by the exploration engine.");
    }

    return result;
}

template<typename ValueType>
typename std::enable_if<!std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithExplorationEngine(
    storm::Environment const&, storm::storage::SymbolicModelDescription const&, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exploration engine does not support data type.");
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithExplorationEngine(storm::storage::SymbolicModelDescription const& model,
                                                                              storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithExplorationEngine(env, model, task);
}

//
// Verifying with Sparse engine
//
template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(storm::Environment const& env,
                                                                         std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> const& dtmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver() == storm::solver::EquationSolverType::Elimination &&
        storm::settings::getModule<storm::settings::modules::EliminationSettings>().isUseDedicatedModelCheckerSet()) {
        storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    } else {
        storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    }
    return result;
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> const& dtmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithSparseEngine(env, dtmc, task);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(storm::Environment const& env,
                                                                         std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> const& ctmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>> modelchecker(*ctmc);
    if (modelchecker.canHandle(task)) {
        result = modelchecker.check(env, task);
    }
    return result;
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> const& ctmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithSparseEngine(env, ctmc, task);
}

template<typename ValueType>
typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithSparseEngine(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const& mdp,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
    if (modelchecker.canHandle(task)) {
        result = modelchecker.check(env, task);
    }
    return result;
}

template<typename ValueType>
typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithSparseEngine(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const& mdp,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
    if (modelchecker.canHandle(task)) {
        result = modelchecker.check(env, task);
    }
    return result;
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const& mdp,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithSparseEngine(env, mdp, task);
}

template<typename ValueType>
typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithSparseEngine(storm::Environment const& env, std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const& ma,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;

    // Close the MA, if it is not already closed.
    if (!ma->isClosed()) {
        STORM_LOG_WARN("Closing Markov automaton. Consider closing the MA before verification.");
        ma->close();
    }

    storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<ValueType>> modelchecker(*ma);
    if (modelchecker.canHandle(task)) {
        result = modelchecker.check(env, task);
    }
    return result;
}

template<typename ValueType>
typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithSparseEngine(storm::Environment const&, std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const&,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Sparse engine cannot verify MAs with this data type.");
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const& ma,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithSparseEngine(env, ma, task);
}

template<typename ValueType>
typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithSparseEngine(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Smg<ValueType>> const& smg,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparseSmgRpatlModelChecker<storm::models::sparse::Smg<ValueType>> modelchecker(*smg);
    if (modelchecker.canHandle(task)) {
        result = modelchecker.check(env, task);
    }
    return result;
}

template<typename ValueType>
typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithSparseEngine(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Smg<ValueType>> const& mdp,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Sparse engine cannot verify SMGs with this data type.");
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Smg<ValueType>> const& smg,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithSparseEngine(env, smg, task);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(storm::Environment const& env,
                                                                         std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (model->getType() == storm::models::ModelType::Dtmc) {
        result = verifyWithSparseEngine(env, model->template as<storm::models::sparse::Dtmc<ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::Mdp) {
        result = verifyWithSparseEngine(env, model->template as<storm::models::sparse::Mdp<ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::Ctmc) {
        result = verifyWithSparseEngine(env, model->template as<storm::models::sparse::Ctmc<ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::MarkovAutomaton) {
        result = verifyWithSparseEngine(env, model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::Smg) {
        result = verifyWithSparseEngine(env, model->template as<storm::models::sparse::Smg<ValueType>>(), task);
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type " << model->getType() << " is not supported by the sparse engine.");
    }
    return result;
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithSparseEngine(env, model, task);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> computeSteadyStateDistributionWithSparseEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> const& dtmc) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
    return modelchecker.computeSteadyStateDistribution(env);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> computeSteadyStateDistributionWithSparseEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> const& ctmc) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>> modelchecker(*ctmc);
    return modelchecker.computeSteadyStateDistribution(env);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> computeSteadyStateDistributionWithSparseEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (model->getType() == storm::models::ModelType::Dtmc) {
        result = computeSteadyStateDistributionWithSparseEngine(env, model->template as<storm::models::sparse::Dtmc<ValueType>>());
    } else if (model->getType() == storm::models::ModelType::Ctmc) {
        result = computeSteadyStateDistributionWithSparseEngine(env, model->template as<storm::models::sparse::Ctmc<ValueType>>());
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Computing the long run average distribution for the model type " << model->getType() << " is not supported.");
    }
    return result;
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> computeExpectedVisitingTimesWithSparseEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> const& dtmc) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
    return modelchecker.computeExpectedVisitingTimes(env);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> computeExpectedVisitingTimesWithSparseEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> const& ctmc) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>> modelchecker(*ctmc);
    return modelchecker.computeExpectedVisitingTimes(env);
}

template<typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> computeExpectedVisitingTimesWithSparseEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (model->getType() == storm::models::ModelType::Dtmc) {
        result = computeExpectedVisitingTimesWithSparseEngine(env, model->template as<storm::models::sparse::Dtmc<ValueType>>());
    } else if (model->getType() == storm::models::ModelType::Ctmc) {
        result = computeExpectedVisitingTimesWithSparseEngine(env, model->template as<storm::models::sparse::Ctmc<ValueType>>());
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Computing expected visiting times for the model type " << model->getType() << " is not supported.");
    }
    return result;
}

//
// Verifying with Hybrid engine
//
template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(storm::Environment const& env,
                                                                         std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>> const& dtmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    dtmc->getManager().execute([&]() {
        storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(*dtmc);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    });
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>> const& dtmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithHybridEngine(env, dtmc, task);
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(storm::Environment const& env,
                                                                         std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>> const& ctmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    ctmc->getManager().execute([&]() {
        storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<DdType, ValueType>> modelchecker(*ctmc);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    });
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>> const& ctmc,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithHybridEngine(env, ctmc, task);
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithHybridEngine(storm::Environment const& env, std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const& mdp,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    mdp->getManager().execute([&]() {
        storm::modelchecker::HybridMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(*mdp);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    });
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithHybridEngine(storm::Environment const&, std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const&,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Hybrid engine cannot verify MDPs with this data type.");
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const& mdp,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithHybridEngine(env, mdp, task);
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithHybridEngine(storm::Environment const& env, std::shared_ptr<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>> const& ma,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    ma->getManager().execute([&]() {
        storm::modelchecker::HybridMarkovAutomatonCslModelChecker<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>> modelchecker(*ma);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    });
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type
verifyWithHybridEngine(storm::Environment const&, std::shared_ptr<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>> const&,
                       storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Hybrid engine cannot verify MDPs with this data type.");
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>> const& ma,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithHybridEngine(env, ma, task);
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(storm::Environment const& env,
                                                                         std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (model->getType() == storm::models::ModelType::Dtmc) {
        result = verifyWithHybridEngine(env, model->template as<storm::models::symbolic::Dtmc<DdType, ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::Ctmc) {
        result = verifyWithHybridEngine(env, model->template as<storm::models::symbolic::Ctmc<DdType, ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::Mdp) {
        result = verifyWithHybridEngine(env, model->template as<storm::models::symbolic::Mdp<DdType, ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::MarkovAutomaton) {
        result = verifyWithHybridEngine(env, model->template as<storm::models::symbolic::MarkovAutomaton<DdType, ValueType>>(), task);
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type " << model->getType() << " is not supported by the hybrid engine.");
    }
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
                                                                         storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithHybridEngine(env, model, task);
}

//
// Verifying with DD engine
//
template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithDdEngine(storm::Environment const& env,
                                                                     std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>> const& dtmc,
                                                                     storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    dtmc->getManager().execute([&]() {
        storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(*dtmc);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    });
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>> const& dtmc,
                                                                     storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithDdEngine(env, dtmc, task);
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithDdEngine(
    storm::Environment const& env, std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const& mdp,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    mdp->getManager().execute([&]() {
        storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(*mdp);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(env, task);
        }
    });
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithDdEngine(
    storm::Environment const&, std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const&,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Dd engine cannot verify MDPs with this data type.");
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const& mdp,
                                                                     storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithDdEngine(env, mdp, task);
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithDdEngine(storm::Environment const& env,
                                                                     std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
                                                                     storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    std::unique_ptr<storm::modelchecker::CheckResult> result;
    if (model->getType() == storm::models::ModelType::Dtmc) {
        result = verifyWithDdEngine(env, model->template as<storm::models::symbolic::Dtmc<DdType, ValueType>>(), task);
    } else if (model->getType() == storm::models::ModelType::Mdp) {
        result = verifyWithDdEngine(env, model->template as<storm::models::symbolic::Mdp<DdType, ValueType>>(), task);
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type " << model->getType() << " is not supported by the dd engine.");
    }
    return result;
}

template<storm::dd::DdType DdType, typename ValueType>
std::unique_ptr<storm::modelchecker::CheckResult> verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
                                                                     storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
    Environment env;
    return verifyWithDdEngine(env, model, task);
}

}  // namespace api
}  // namespace storm
