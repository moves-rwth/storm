#pragma once

#include <type_traits>

#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "storm/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "storm/modelchecker/abstraction/GameBasedMdpModelChecker.h"
#include "storm/modelchecker/exploration/SparseExplorationModelChecker.h"
#include "storm/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/EliminationSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace api {

        template<typename ValueType>
        storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> createTask(std::shared_ptr<const storm::logic::Formula> const& formula, bool onlyInitialStatesRelevant = false) {
            return storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>(*formula, onlyInitialStatesRelevant);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithAbstractionRefinementEngine(storm::storage::SymbolicModelDescription const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            STORM_LOG_THROW(model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::DTMC || model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::MDP, storm::exceptions::NotSupportedException, "Can only treat DTMCs/MDPs using the abstraction refinement engine.");
            
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            if (model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::DTMC) {
                storm::modelchecker::GameBasedMdpModelChecker<DdType, storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(model);
                if (modelchecker.canHandle(task)) {
                    result = modelchecker.check(task);
                }
            } else {
                storm::modelchecker::GameBasedMdpModelChecker<DdType, storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(model);
                if (modelchecker.canHandle(task)) {
                    result = modelchecker.check(task);
                }
            }
            return result;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithAbstractionRefinementEngine(storm::storage::SymbolicModelDescription const&, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Abstraction-refinement engine does not support data type.");
        }
        
        template<typename ValueType>
        typename std::enable_if<std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithExplorationEngine(storm::storage::SymbolicModelDescription const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            STORM_LOG_THROW(model.isPrismProgram(), storm::exceptions::NotSupportedException, "Exploration engine is currently only applicable to PRISM models.");
            storm::prism::Program const& program = model.asPrismProgram();
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently exploration-based verification is only available for DTMCs and MDPs.");

            std::unique_ptr<storm::modelchecker::CheckResult> result;
            if (program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Dtmc<ValueType>> checker(program);
                if (checker.canHandle(task)) {
                    result = checker.check(task);
                }
            } else {
                storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Mdp<ValueType>> checker(program);
                if (checker.canHandle(task)) {
                    result = checker.check(task);
                }
            }
            return result;
        }

        template<typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, double>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithExplorationEngine(storm::storage::SymbolicModelDescription const&, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exploration engine does not support data type.");
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> const& dtmc, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            if (storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver() == storm::solver::EquationSolverType::Elimination && storm::settings::getModule<storm::settings::modules::EliminationSettings>().isUseDedicatedModelCheckerSet()) {
                storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
                if (modelchecker.canHandle(task)) {
                    result = modelchecker.check(task);
                }
            } else {
                storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
                if (modelchecker.canHandle(task)) {
                    result = modelchecker.check(task);
                }
            }
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> const& ctmc, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>> modelchecker(*ctmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }
        
        template<typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const& mdp, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }

        template<typename ValueType>
        typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const&, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Sparse engine cannot verify MDPs with this data type.");
        }

        template<typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const& ma, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;

            // Close the MA, if it is not already closed.
            if (!ma->isClosed()) {
                STORM_LOG_WARN("Closing Markov automaton. Consider closing the MA before verification.");
                ma->close();
            }
            
            storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<ValueType>> modelchecker(*ma);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }

        template<typename ValueType>
        typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const& ma, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Sparse engine cannot verify MAs with this data type.");
        }
                                                                                                                                                                          
        template<typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            if (model->getType() == storm::models::ModelType::Dtmc) {
                result = verifyWithSparseEngine(model->template as<storm::models::sparse::Dtmc<ValueType>>(), task);
            } else if (model->getType() == storm::models::ModelType::Mdp) {
                result = verifyWithSparseEngine(model->template as<storm::models::sparse::Mdp<ValueType>>(), task);
            } else if (model->getType() == storm::models::ModelType::Ctmc) {
                result = verifyWithSparseEngine(model->template as<storm::models::sparse::Ctmc<ValueType>>(), task);
            } else if (model->getType() == storm::models::ModelType::MarkovAutomaton) {
                result = verifyWithSparseEngine(model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(), task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type " << model->getType() << " is not supported.");
            }
            return result;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>> const& dtmc, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Ctmc<DdType, ValueType>> const& ctmc, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<DdType, ValueType>> modelchecker(*ctmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const& mdp, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            storm::modelchecker::HybridMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const&, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Hybrid engine cannot verify MDPs with this data type.");
        }

        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            if (model->getType() == storm::models::ModelType::Dtmc) {
                result = verifyWithHybridEngine(model->template as<storm::models::symbolic::Dtmc<DdType, ValueType>>(), task);
            } else if (model->getType() == storm::models::ModelType::Ctmc) {
                result = verifyWithHybridEngine(model->template as<storm::models::symbolic::Ctmc<DdType, ValueType>>(), task);
            } else if (model->getType() == storm::models::ModelType::Mdp) {
                result = verifyWithHybridEngine(model->template as<storm::models::symbolic::Mdp<DdType, ValueType>>(), task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type is not supported by the hybrid engine.");
            }
            return result;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Dtmc<DdType, ValueType>> const& dtmc, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<!std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const& mdp, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
            return result;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<std::is_same<ValueType, storm::RationalFunction>::value, std::unique_ptr<storm::modelchecker::CheckResult>>::type verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Mdp<DdType, ValueType>> const&, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Dd engine cannot verify MDPs with this data type.");
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
            std::unique_ptr<storm::modelchecker::CheckResult> result;
            if (model->getType() == storm::models::ModelType::Dtmc) {
                result = verifyWithDdEngine(model->template as<storm::models::symbolic::Dtmc<DdType, ValueType>>(), task);
            } else if (model->getType() == storm::models::ModelType::Mdp) {
                result = verifyWithDdEngine(model->template as<storm::models::symbolic::Mdp<DdType, ValueType>>(), task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type is not supported by the dd engine.");
            }
            return result;
        }
        
    }
}
