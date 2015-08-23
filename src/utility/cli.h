#ifndef STORM_UTILITY_CLI_H_
#define STORM_UTILITY_CLI_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <memory>

#include "initialize.h"

#include "storm-config.h"



// Headers that provide auxiliary functionality.
#include "src/utility/storm-version.h"
#include "src/utility/OsDetection.h"
#include "src/settings/SettingsManager.h"


#include "src/settings/modules/BisimulationSettings.h"
#include "src/settings/modules/ParametricSettings.h"

// Headers related to parsing.
#include "src/parser/AutoParser.h"
#include "src/parser/PrismParser.h"
#include "src/parser/FormulaParser.h"

// Formula headers.
#include "src/logic/Formulas.h"

// Model headers.
#include "src/models/ModelBase.h"
#include "src/models/sparse/Model.h"
#include "src/models/symbolic/Model.h"

#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

// Headers of builders.
#include "src/builder/ExplicitPrismModelBuilder.h"
#include "src/builder/DdPrismModelBuilder.h"

// Headers for model processing.
#include "src/storage/DeterministicModelBisimulationDecomposition.h"

// Headers for model checking.
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "src/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"

// Headers for counterexample generation.
#include "src/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "src/counterexamples/SMTMinimalCommandSetGenerator.h"

// Headers related to exception handling.
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace utility {
        namespace cli {
            
            std::string getCurrentWorkingDirectory();
            
            void printHeader(const int argc, const char* argv[]);
            
            void printUsage();
            
            /*!
             * Parses the given command line arguments.
             *
             * @param argc The argc argument of main().
             * @param argv The argv argument of main().
             * @return True iff the program should continue to run after parsing the options.
             */
            bool parseOptions(const int argc, const char* argv[]);
            
            template<typename ValueType>
            std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile = boost::optional<std::string>(), boost::optional<std::string> const& transitionRewardsFile = boost::optional<std::string>()) {
                return storm::parser::AutoParser::parseModel(transitionsFile, labelingFile, stateRewardsFile ? stateRewardsFile.get() : "", transitionRewardsFile ? transitionRewardsFile.get() : "");
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::ModelBase> buildSymbolicModel(storm::prism::Program const& program, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
                std::shared_ptr<storm::models::ModelBase> result(nullptr);
                
                storm::settings::modules::GeneralSettings settings = storm::settings::generalSettings();
                
                // Get the string that assigns values to the unknown currently undefined constants in the model.
                std::string constants = settings.getConstantDefinitionString();
                                
                // Customize and perform model-building.
                if (settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Sparse) {
                    typename storm::builder::ExplicitPrismModelBuilder<ValueType>::Options options;
                    options = typename storm::builder::ExplicitPrismModelBuilder<ValueType>::Options(formulas);
                    options.addConstantDefinitionsFromString(program, settings.getConstantDefinitionString());

                    // Generate command labels if we are going to build a counterexample later.
                    if (storm::settings::counterexampleGeneratorSettings().isMinimalCommandSetGenerationSet()) {
                        options.buildCommandLabels = true;
                    }
                    
                    result = storm::builder::ExplicitPrismModelBuilder<ValueType>::translateProgram(program, options);
                } else if (settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Dd || settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Hybrid) {
                    typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options options;
                    options = typename storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::Options(formulas);
                    options.addConstantDefinitionsFromString(program, settings.getConstantDefinitionString());

                    result = storm::builder::DdPrismModelBuilder<storm::dd::DdType::CUDD>::translateProgram(program, options);
                }
                
                // Then, build the model from the symbolic description.
                return result;
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::models::ModelBase> preprocessModel(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
                if (storm::settings::generalSettings().isBisimulationSet()) {
                    STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for sparse models.");
                    std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->template as<storm::models::sparse::Model<ValueType>>();
                    STORM_LOG_THROW(model->getType() == storm::models::ModelType::Dtmc || model->getType() == storm::models::ModelType::Ctmc, storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for DTMCs.");
                    std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc = sparseModel->template as<storm::models::sparse::Dtmc<ValueType>>();
                    
                    dtmc->reduceToStateBasedRewards();
                    
                    std::cout << "Performing bisimulation minimization... ";
                    typename storm::storage::DeterministicModelBisimulationDecomposition<ValueType>::Options options;
                    if (!formulas.empty()) {
                        options = typename storm::storage::DeterministicModelBisimulationDecomposition<ValueType>::Options(*sparseModel, formulas);
                    }
                    if (storm::settings::bisimulationSettings().isWeakBisimulationSet()) {
                        options.weak = true;
                        options.bounded = false;
                    }
                    
                    storm::storage::DeterministicModelBisimulationDecomposition<ValueType> bisimulationDecomposition(*dtmc, options);
                    model = bisimulationDecomposition.getQuotient();
                    std::cout << "done." << std::endl << std::endl;
                }
                return model;
            }
            
            template<typename ValueType>
            void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
                if (storm::settings::counterexampleGeneratorSettings().isMinimalCommandSetGenerationSet()) {
                    STORM_LOG_THROW(model->getType() == storm::models::ModelType::Mdp, storm::exceptions::InvalidTypeException, "Minimal command set generation is only available for MDPs.");
                    STORM_LOG_THROW(storm::settings::generalSettings().isSymbolicSet(), storm::exceptions::InvalidSettingsException, "Minimal command set generation is only available for symbolic models.");
                    
                    std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = model->template as<storm::models::sparse::Mdp<ValueType>>();
                    
                    // Determine whether we are required to use the MILP-version or the SAT-version.
                    bool useMILP = storm::settings::counterexampleGeneratorSettings().isUseMilpBasedMinimalCommandSetGenerationSet();
                    
                    if (useMILP) {
                        storm::counterexamples::MILPMinimalLabelSetGenerator<ValueType>::computeCounterexample(program, *mdp, formula);
                    } else {
                        storm::counterexamples::SMTMinimalCommandSetGenerator<ValueType>::computeCounterexample(program, storm::settings::generalSettings().getConstantDefinitionString(), *mdp, formula);
                    }
                    
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No suitable counterexample representation selected.");
                }
            }
            
#ifdef STORM_HAVE_CARL
            template<>
            inline void generateCounterexample(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for parametric model.");
            }
#endif
            
            template<typename ValueType>
            void verifySparseModel(boost::optional<storm::prism::Program> const& program, std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
                storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
                
                for (auto const& formula : formulas) {
                    // If we were requested to generate a counterexample, we now do so.
                    if (settings.isCounterexampleSet()) {
                        STORM_LOG_THROW(program, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for non-symbolic model.");
                        generateCounterexample<ValueType>(program.get(), model, formula);
                    } else {
                        std::cout << std::endl << "Model checking property: " << *formula << " ...";
                        std::unique_ptr<storm::modelchecker::CheckResult> result;
                        if (model->getType() == storm::models::ModelType::Dtmc) {
                            std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc = model->template as<storm::models::sparse::Dtmc<ValueType>>();
                            storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
                            if (modelchecker.canHandle(*formula)) {
                                result = modelchecker.check(*formula);
                            } else {
                                storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker2(*dtmc);
                                if (modelchecker2.canHandle(*formula)) {
                                    result = modelchecker2.check(*formula);
                                }
                            }
                        } else if (model->getType() == storm::models::ModelType::Mdp) {
                            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = model->template as<storm::models::sparse::Mdp<ValueType>>();
#ifdef STORM_HAVE_CUDA
                            if (settings.isCudaSet()) {
                                storm::modelchecker::TopologicalValueIterationMdpPrctlModelChecker<ValueType> modelchecker(*mdp);
                                result = modelchecker.check(*formula);
                            } else {
                                storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
                                result = modelchecker.check(*formula);
                            }
#else
                            storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
                            result = modelchecker.check(*formula);
#endif
                        } else if (model->getType() == storm::models::ModelType::Ctmc) {
                            std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> ctmc = model->template as<storm::models::sparse::Ctmc<ValueType>>();
                            
                            storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>> modelchecker(*ctmc);
                            result = modelchecker.check(*formula);
                        }
                        
                        if (result) {
                            std::cout << " done." << std::endl;
                            std::cout << "Result (initial states): ";
                            result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                            std::cout << *result << std::endl;
                        } else {
                            std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                        }
                    }
                }
            }
            
#ifdef STORM_HAVE_CARL
            inline void exportParametricResultToFile(storm::RationalFunction const& result, storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector const& constraintCollector, std::string const& path) {
                std::ofstream filestream;
                filestream.open(path);
                // TODO: add checks.
                filestream << "!Parameters: ";
                std::set<storm::Variable> vars = result.gatherVariables();
                std::copy(vars.begin(), vars.end(), std::ostream_iterator<storm::Variable>(filestream, ", "));
                filestream << std::endl;
                filestream << "!Result: " << result << std::endl;
                filestream << "!Well-formed Constraints: " << std::endl;
                std::copy(constraintCollector.getWellformedConstraints().begin(), constraintCollector.getWellformedConstraints().end(), std::ostream_iterator<storm::ArithConstraint<storm::RationalFunction>>(filestream, "\n"));
                filestream << "!Graph-preserving Constraints: " << std::endl;
                std::copy(constraintCollector.getGraphPreservingConstraints().begin(), constraintCollector.getGraphPreservingConstraints().end(), std::ostream_iterator<storm::ArithConstraint<storm::RationalFunction>>(filestream, "\n"));
                filestream.close();
            }
            
            template<>
            inline void verifySparseModel(boost::optional<storm::prism::Program> const& program, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {

                for (auto const& formula : formulas) {
                    STORM_LOG_THROW(model->getType() == storm::models::ModelType::Dtmc, storm::exceptions::InvalidSettingsException, "Currently parametric verification is only available for DTMCs.");
                    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
                    
                    std::cout << std::endl << "Model checking property: " << *formula << " ...";
                    std::unique_ptr<storm::modelchecker::CheckResult> result;
                    
                    storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>> modelchecker(*dtmc);
                    if (modelchecker.canHandle(*formula)) {
                        result = modelchecker.check(*formula);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The parametric engine currently does not support this property.");
                    }
                    
                    if (result) {
                        std::cout << " done." << std::endl;
                        std::cout << "Result (initial states): ";
                        result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(dtmc->getInitialStates()));
                        std::cout << *result << std::endl;
                    } else {
                        std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                    }
                    
                    storm::settings::modules::ParametricSettings const& parametricSettings = storm::settings::parametricSettings();
                    if (parametricSettings.exportResultToFile()) {
                        exportParametricResultToFile(result->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[*dtmc->getInitialStates().begin()], storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector(*dtmc), parametricSettings.exportResultPath());
                    }
                }
            }
#endif
            
            template<storm::dd::DdType DdType>
            void verifySymbolicModelWithHybridEngine(boost::optional<storm::prism::Program> const& program, std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
                for (auto const& formula : formulas) {
                    std::cout << std::endl << "Model checking property: " << *formula << " ...";
                    std::unique_ptr<storm::modelchecker::CheckResult> result;
                    if (model->getType() == storm::models::ModelType::Dtmc) {
                        std::shared_ptr<storm::models::symbolic::Dtmc<DdType>> dtmc = model->template as<storm::models::symbolic::Dtmc<DdType>>();
                        storm::modelchecker::HybridDtmcPrctlModelChecker<DdType, double> modelchecker(*dtmc);
                        if (modelchecker.canHandle(*formula)) {
                            result = modelchecker.check(*formula);
                        }
                    } else if (model->getType() == storm::models::ModelType::Ctmc) {
                        std::shared_ptr<storm::models::symbolic::Ctmc<DdType>> ctmc = model->template as<storm::models::symbolic::Ctmc<DdType>>();
                        storm::modelchecker::HybridCtmcCslModelChecker<DdType, double> modelchecker(*ctmc);
                        if (modelchecker.canHandle(*formula)) {
                            result = modelchecker.check(*formula);
                        }
                    } else if (model->getType() == storm::models::ModelType::Mdp) {
                        std::shared_ptr<storm::models::symbolic::Mdp<DdType>> mdp = model->template as<storm::models::symbolic::Mdp<DdType>>();
                        storm::modelchecker::HybridMdpPrctlModelChecker<DdType, double> modelchecker(*mdp);
                        if (modelchecker.canHandle(*formula)) {
                            result = modelchecker.check(*formula);
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
                    }
                    
                    if (result) {
                        std::cout << " done." << std::endl;
                        std::cout << "Result (initial states): ";
                        result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(model->getReachableStates(), model->getInitialStates()));
                        std::cout << *result << std::endl;
                    } else {
                        std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                    }
                }
            }
            
            template<storm::dd::DdType DdType>
            void verifySymbolicModelWithSymbolicEngine(boost::optional<storm::prism::Program> const& program, std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
                for (auto const& formula : formulas) {
                    std::cout << std::endl << "Model checking property: " << *formula << " ...";
                    std::unique_ptr<storm::modelchecker::CheckResult> result;
                    if (model->getType() == storm::models::ModelType::Dtmc) {
                        std::shared_ptr<storm::models::symbolic::Dtmc<DdType>> dtmc = model->template as<storm::models::symbolic::Dtmc<DdType>>();
                        storm::modelchecker::SymbolicDtmcPrctlModelChecker<DdType, double> modelchecker(*dtmc);
                        if (modelchecker.canHandle(*formula)) {
                            result = modelchecker.check(*formula);
                        }
                    } else if (model->getType() == storm::models::ModelType::Mdp) {
                        std::shared_ptr<storm::models::symbolic::Mdp<DdType>> mdp = model->template as<storm::models::symbolic::Mdp<DdType>>();
                        storm::modelchecker::SymbolicMdpPrctlModelChecker<DdType, double> modelchecker(*mdp);
                        if (modelchecker.canHandle(*formula)) {
                            result = modelchecker.check(*formula);
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
                    }
                    
                    if (result) {
                        std::cout << " done." << std::endl;
                        std::cout << "Result (initial states): ";
                        result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(model->getReachableStates(), model->getInitialStates()));
                        std::cout << *result << std::endl;
                    } else {
                        std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                    }
                }
            }
            
            template<typename ValueType>
            void buildAndCheckSymbolicModel(boost::optional<storm::prism::Program> const& program, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
                // Now we are ready to actually build the model.
                STORM_LOG_THROW(program, storm::exceptions::InvalidStateException, "Program has not been successfully parsed.");
                std::shared_ptr<storm::models::ModelBase> model = buildSymbolicModel<ValueType>(program.get(), formulas);
                
                STORM_LOG_THROW(model != nullptr, storm::exceptions::InvalidStateException, "Model could not be constructed for an unknown reason.");
                
                // Preprocess the model if needed.
                model = preprocessModel<ValueType>(model, formulas);
                
                // Print some information about the model.
                model->printModelInformationToStream(std::cout);
                
                // Verify the model, if a formula was given.
                if (!formulas.empty()) {
                    if (model->isSparseModel()) {
                        verifySparseModel<ValueType>(program, model->as<storm::models::sparse::Model<ValueType>>(), formulas);
                    } else if (model->isSymbolicModel()) {
                        if (storm::settings::generalSettings().getEngine() == storm::settings::modules::GeneralSettings::Engine::Hybrid) {
                            verifySymbolicModelWithHybridEngine(program, model->as<storm::models::symbolic::Model<storm::dd::DdType::CUDD>>(), formulas);
                        } else {
                            verifySymbolicModelWithSymbolicEngine(program, model->as<storm::models::symbolic::Model<storm::dd::DdType::CUDD>>(), formulas);
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Invalid input model type.");
                    }
                }
            }
            
            template<typename ValueType>
            void buildAndCheckExplicitModel(std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
                storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
                
                STORM_LOG_THROW(settings.isExplicitSet(), storm::exceptions::InvalidStateException, "Unable to build explicit model without model files.");
                std::shared_ptr<storm::models::ModelBase> model = buildExplicitModel<ValueType>(settings.getTransitionFilename(), settings.getLabelingFilename(), settings.isStateRewardsSet() ? settings.getStateRewardsFilename() : boost::optional<std::string>(), settings.isTransitionRewardsSet() ? settings.getTransitionRewardsFilename() : boost::optional<std::string>());
                
                // Preprocess the model if needed.
                model = preprocessModel<ValueType>(model, formulas);
                
                // Print some information about the model.
                model->printModelInformationToStream(std::cout);
                
                // Verify the model, if a formula was given.
                if (!formulas.empty()) {
                    STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidStateException, "Expected sparse model.");
                    verifySparseModel<ValueType>(boost::optional<storm::prism::Program>(), model->as<storm::models::sparse::Model<ValueType>>(), formulas);
                }
            }
            
            void processOptions();
        }
    }
}

#endif
