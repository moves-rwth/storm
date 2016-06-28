
#ifndef STORM_H
#define	STORM_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <memory>
#include <src/storage/ModelFormulasPair.h>

#include "initialize.h"

#include "storm-config.h"

// Headers that provide auxiliary functionality.
#include "src/settings/SettingsManager.h"

#include "src/settings/modules/CoreSettings.h"
#include "src/settings/modules/IOSettings.h"
#include "src/settings/modules/BisimulationSettings.h"
#include "src/settings/modules/ParametricSettings.h"
#include "src/settings/modules/EliminationSettings.h"
#include "src/settings/modules/CoreSettings.h"

// Formula headers.
#include "src/logic/Formulas.h"
#include "src/logic/FragmentSpecification.h"

// Model headers.
#include "src/models/ModelBase.h"
#include "src/models/sparse/Model.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/symbolic/Model.h"
#include "src/models/symbolic/StandardRewardModel.h"

#include "src/storage/dd/Add.h"
#include "src/storage/dd/Bdd.h"

#include "src/parser/AutoParser.h"

#include "src/storage/jani/Model.h"

// Headers of builders.
#include "src/builder/ExplicitModelBuilder.h"
#include "src/builder/DdPrismModelBuilder.h"

// Headers for model processing.
#include "src/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "src/storage/bisimulation/NondeterministicModelBisimulationDecomposition.h"
#include "src/storage/ModelFormulasPair.h"

// Headers for model checking.
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/modelchecker/exploration/SparseExplorationModelChecker.h"
#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "src/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "src/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "src/modelchecker/multiobjective/SparseMdpMultiObjectiveModelChecker.h"
#include "src/modelchecker/multiobjective/SparseMaMultiObjectiveModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

// Headers for counterexample generation.
#include "src/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "src/counterexamples/SMTMinimalCommandSetGenerator.h"

// Headers related to PRISM model building.
#include "src/generator/PrismNextStateGenerator.h"
#include "src/utility/prism.h"

// Headers related to exception handling.
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {

    template<typename ValueType>
    std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile = boost::none, boost::optional<std::string> const& transitionRewardsFile = boost::none, boost::optional<std::string> const& choiceLabelingFile = boost::none) {
        return storm::parser::AutoParser<>::parseModel(transitionsFile, labelingFile, stateRewardsFile ? stateRewardsFile.get() : "", transitionRewardsFile ? transitionRewardsFile.get() : "", choiceLabelingFile ? choiceLabelingFile.get() : "" );
    }

    storm::jani::Model parseModel(std::string const& path);
    storm::prism::Program parseProgram(std::string const& path);
    std::vector<std::shared_ptr<storm::logic::Formula const>> parseFormulasForExplicit(std::string const& inputString);
    std::vector<std::shared_ptr<storm::logic::Formula const>> parseFormulasForProgram(std::string const& inputString, storm::prism::Program const& program);

    template<typename ValueType>
    std::shared_ptr<storm::models::sparse::Model<ValueType>> buildSparseModel(storm::prism::Program const& program, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
        storm::generator::NextStateGeneratorOptions options(formulas);
        
        // Generate command labels if we are going to build a counterexample later.
        if (storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>().isMinimalCommandSetGenerationSet()) {
            options.setBuildChoiceLabels(true);
        }
        
        std::shared_ptr<storm::generator::NextStateGenerator<ValueType, uint32_t>> generator = std::make_shared<storm::generator::PrismNextStateGenerator<ValueType, uint32_t>>(program, options);
        storm::builder::ExplicitModelBuilder<ValueType> builder(generator);
        return builder.build();
    }
    
    template<typename ValueType, storm::dd::DdType LibraryType = storm::dd::DdType::CUDD>
    std::shared_ptr<storm::models::symbolic::Model<LibraryType, ValueType>> buildSymbolicModel(storm::prism::Program const& program, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
        typename storm::builder::DdPrismModelBuilder<LibraryType>::Options options;
        options = typename storm::builder::DdPrismModelBuilder<LibraryType>::Options(formulas);

        storm::builder::DdPrismModelBuilder<LibraryType> builder;
        return builder.build(program, options);
    }
    
    template<typename ModelType>
    std::shared_ptr<ModelType> performDeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
        STORM_LOG_INFO("Performing bisimulation minimization... ");
        typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
        if (!formulas.empty()) {
            options = typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
        }
        options.setType(type);
        
        storm::storage::DeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
        bisimulationDecomposition.computeBisimulationDecomposition();
        model = bisimulationDecomposition.getQuotient();
        STORM_LOG_INFO("Bisimulation done. ");
        return model;
    }
    
    template<typename ModelType>
    std::shared_ptr<ModelType> performNondeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
        STORM_LOG_INFO("Performing bisimulation minimization... ");
        typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
        if (!formulas.empty()) {
            options = typename storm::storage::NondeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
        }
        options.setType(type);
        
        storm::storage::NondeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
        bisimulationDecomposition.computeBisimulationDecomposition();
        model = bisimulationDecomposition.getQuotient();
        STORM_LOG_INFO("Bisimulation done.");
        return model;
    }
    
    template<typename ModelType>
    std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> performBisimulationMinimization(std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
        using ValueType = typename ModelType::ValueType;
        
        STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc) || model->isOfType(storm::models::ModelType::Ctmc) || model->isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for DTMCs, CTMCs and MDPs.");
        model->reduceToStateBasedRewards();

        if (model->isOfType(storm::models::ModelType::Dtmc)) {
            return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Dtmc<ValueType>>(model->template as<storm::models::sparse::Dtmc<ValueType>>(), formulas, type);
        } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
            return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), formulas, type);
        } else {
            return performNondeterministicSparseBisimulationMinimization<storm::models::sparse::Mdp<ValueType>>(model->template as<storm::models::sparse::Mdp<ValueType>>(), formulas, type);
        }       
    }
        
    template<typename ModelType>
    std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> performBisimulationMinimization(std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> const& model, std::shared_ptr<storm::logic::Formula const> const& formula, storm::storage::BisimulationType type) {
        std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = { formula };
        return performBisimulationMinimization<ModelType>(model, formulas , type);
    }
    
    
    template<typename ModelType>
    std::shared_ptr<storm::models::ModelBase> preprocessModel(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
        if(model->getType() == storm::models::ModelType::MarkovAutomaton && model->isSparseModel()) {
            std::shared_ptr<storm::models::sparse::MarkovAutomaton<typename ModelType::ValueType>> ma = model->template as<storm::models::sparse::MarkovAutomaton<typename ModelType::ValueType>>();
            if (ma->hasOnlyTrivialNondeterminism()) {
                // Markov automaton can be converted into CTMC
                model = ma->convertToCTMC();
            } 
        }
        
        if (model->isSparseModel() && storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet()) {
            storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
            if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
                bisimType = storm::storage::BisimulationType::Weak;
            }
            
            STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for sparse models.");
            return performBisimulationMinimization<ModelType>(model->template as<storm::models::sparse::Model<typename ModelType::ValueType>>(), formulas, bisimType);
        }
        
        return model;
    }
    
    template<typename ValueType>
    void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::shared_ptr<storm::logic::Formula const> const& formula) {
        if (storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>().isMinimalCommandSetGenerationSet()) {
            STORM_LOG_THROW(model->getType() == storm::models::ModelType::Mdp, storm::exceptions::InvalidTypeException, "Minimal command set generation is only available for MDPs.");
            STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::IOSettings>().isSymbolicSet(), storm::exceptions::InvalidSettingsException, "Minimal command set generation is only available for symbolic models.");

            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = model->template as<storm::models::sparse::Mdp<ValueType>>();

            // Determine whether we are required to use the MILP-version or the SAT-version.
            bool useMILP = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>().isUseMilpBasedMinimalCommandSetGenerationSet();

            if (useMILP) {
                storm::counterexamples::MILPMinimalLabelSetGenerator<ValueType>::computeCounterexample(program, *mdp, formula);
            } else {
                storm::counterexamples::SMTMinimalCommandSetGenerator<ValueType>::computeCounterexample(program, storm::settings::getModule<storm::settings::modules::IOSettings>().getConstantDefinitionString(), *mdp, formula);
            }

        } else {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No suitable counterexample representation selected.");
        }
    }

    template<>
    inline void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> model, std::shared_ptr<storm::logic::Formula const> const& formula) {
        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for parametric model.");
    }

    template<>
    inline void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula const> const& formula) {
        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for parametric model.");
    }
    
    template<typename ValueType>
    void generateCounterexamples(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
        for (auto const& formula : formulas) {
            generateCounterexample(program, model, formula);
        }
    }

    template<typename ValueType, storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifyModel(std::shared_ptr<storm::models::ModelBase> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        switch(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine()) {
            case storm::settings::modules::CoreSettings::Engine::Sparse: {
                std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->template as<storm::models::sparse::Model<ValueType>>();
                STORM_LOG_THROW(sparseModel != nullptr, storm::exceptions::InvalidArgumentException, "Sparse engine requires a sparse input model");
                return verifySparseModel(sparseModel, formula, onlyInitialStatesRelevant);
            }
            case storm::settings::modules::CoreSettings::Engine::Hybrid: {
                std::shared_ptr<storm::models::symbolic::Model<DdType>> ddModel = model->template as<storm::models::symbolic::Model<DdType>>();
                STORM_LOG_THROW(ddModel != nullptr, storm::exceptions::InvalidArgumentException, "Hybrid engine requires a dd input model");
                return verifySymbolicModelWithHybridEngine(ddModel, formula, onlyInitialStatesRelevant);
            }
            case storm::settings::modules::CoreSettings::Engine::Dd: {
                std::shared_ptr<storm::models::symbolic::Model<DdType>> ddModel = model->template as<storm::models::symbolic::Model<DdType>>();
                STORM_LOG_THROW(ddModel != nullptr, storm::exceptions::InvalidArgumentException, "Dd engine requires a dd input model");
                return verifySymbolicModelWithDdEngine(ddModel, formula, onlyInitialStatesRelevant);
            }
            default: {
                STORM_LOG_ASSERT(false, "This position should not be reached, as at this point no model has been built.");
            }
        }
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseDtmc(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc, storm::modelchecker::CheckTask<storm::logic::Formula> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver() == storm::solver::EquationSolverType::Elimination && storm::settings::getModule<storm::settings::modules::EliminationSettings>().isUseDedicatedModelCheckerSet()) {
            storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported by the dedicated elimination model checker.");
            }
        } else {
            storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
            }
        }
        return result;
    }
    
    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseCtmc(std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> ctmc, storm::modelchecker::CheckTask<storm::logic::Formula> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>> modelchecker(*ctmc);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
        }
        return result;
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseMdp(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp, storm::modelchecker::CheckTask<storm::logic::Formula> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if(task.getFormula().isMultiObjectiveFormula()) {
            storm::modelchecker::SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
            }
        } else {
            storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
            }
        }
        return result;
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseMarkovAutomaton(std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> ma, storm::modelchecker::CheckTask<storm::logic::Formula> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if(task.getFormula().isMultiObjectiveFormula()) {
            // Close the MA, if it is not already closed.
            if (!ma->isClosed()) {
                ma->close();
            }
            storm::modelchecker::SparseMaMultiObjectiveModelChecker<storm::models::sparse::MarkovAutomaton<ValueType>> modelchecker(*ma);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
            }
        } else {
            storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<ValueType>> modelchecker(*ma);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
            }
        }
        return result;
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant = false) {
        storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);

        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (model->getType() == storm::models::ModelType::Dtmc) {
            result = verifySparseDtmc(model->template as<storm::models::sparse::Dtmc<ValueType>>(), task);
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            result = verifySparseMdp(model->template as<storm::models::sparse::Mdp<ValueType>>(), task);
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            result = verifySparseCtmc(model->template as<storm::models::sparse::Ctmc<ValueType>>(), task);
        } else if (model->getType() == storm::models::ModelType::MarkovAutomaton) {
            result = verifySparseMarkovAutomaton(model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(), task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type " << model->getType() << " is not supported.");
        }
        return result;
    }
    
    template<>
    inline std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);

        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (model->getType() == storm::models::ModelType::Dtmc) {
            result = verifySparseDtmc(model->template as<storm::models::sparse::Dtmc<storm::RationalNumber>>(), task);
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            result = verifySparseCtmc(model->template as<storm::models::sparse::Ctmc<storm::RationalNumber>>(), task);
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            result = verifySparseMdp(model->template as<storm::models::sparse::Mdp<storm::RationalNumber>>(), task);
        } else {
            STORM_LOG_ASSERT(false, "Illegal model type.");
        }
        return result;
    }
    
#ifdef STORM_HAVE_CARL
    inline void exportParametricResultToFile(storm::RationalFunction const& result, storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector const& constraintCollector, std::string const& path) {
        std::ofstream filestream;
        filestream.open(path);
        // TODO: add checks.
        filestream << "!Parameters: ";
        std::set<storm::RationalFunctionVariable> vars = result.gatherVariables();
        std::copy(vars.begin(), vars.end(), std::ostream_iterator<storm::RationalFunctionVariable>(filestream, "; "));
        filestream << std::endl;
        filestream << "!Result: " << result << std::endl;
        filestream << "!Well-formed Constraints: " << std::endl;
        std::copy(constraintCollector.getWellformedConstraints().begin(), constraintCollector.getWellformedConstraints().end(), std::ostream_iterator<storm::ArithConstraint<storm::RationalFunction>>(filestream, "\n"));
        filestream << "!Graph-preserving Constraints: " << std::endl;
        std::copy(constraintCollector.getGraphPreservingConstraints().begin(), constraintCollector.getGraphPreservingConstraints().end(), std::ostream_iterator<storm::ArithConstraint<storm::RationalFunction>>(filestream, "\n"));
        filestream.close();
    }

    template<>
    inline std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);

        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (model->getType() == storm::models::ModelType::Dtmc) {
            result = verifySparseDtmc(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>(), task);
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> mdp = model->template as<storm::models::sparse::Mdp<storm::RationalFunction>>();
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The parametric engine currently does not support MDPs.");
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            verifySparseCtmc(model->template as<storm::models::sparse::Ctmc<storm::RationalFunction>>(), task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The parametric engine currently does not support " << model->getType());
        }
        return result;
    }
            
#endif

    template<storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySymbolicModelWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant = false) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);
        if (model->getType() == storm::models::ModelType::Dtmc) {
            std::shared_ptr<storm::models::symbolic::Dtmc<DdType>> dtmc = model->template as<storm::models::symbolic::Dtmc<DdType>>();
            storm::modelchecker::HybridDtmcPrctlModelChecker<DdType, double> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            std::shared_ptr<storm::models::symbolic::Ctmc<DdType>> ctmc = model->template as<storm::models::symbolic::Ctmc<DdType>>();
            storm::modelchecker::HybridCtmcCslModelChecker<DdType, double> modelchecker(*ctmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            std::shared_ptr<storm::models::symbolic::Mdp<DdType>> mdp = model->template as<storm::models::symbolic::Mdp<DdType>>();
            storm::modelchecker::HybridMdpPrctlModelChecker<DdType, double> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
        }
        return result;
    }


    template<storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySymbolicModelWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);
        if (model->getType() == storm::models::ModelType::Dtmc) {
            std::shared_ptr<storm::models::symbolic::Dtmc<DdType>> dtmc = model->template as<storm::models::symbolic::Dtmc<DdType>>();
            storm::modelchecker::SymbolicDtmcPrctlModelChecker<DdType, double> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            std::shared_ptr<storm::models::symbolic::Mdp<DdType>> mdp = model->template as<storm::models::symbolic::Mdp<DdType>>();
            storm::modelchecker::SymbolicMdpPrctlModelChecker<DdType, double> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
        }
        return result;
    }

    template<typename ValueType>
    void exportMatrixToFile(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::string const& filepath) {
        STORM_LOG_THROW(model->getType() != storm::models::ModelType::Ctmc, storm::exceptions::NotImplementedException, "This functionality is not yet implemented." );
        std::ofstream ofs;
        ofs.open (filepath, std::ofstream::out);
        model->getTransitionMatrix().printAsMatlabMatrix(ofs);
        ofs.close();
    }
        
}

#endif	/* STORM_H */

