#ifndef STORM_ENTRYPOINTS_H_H
#define STORM_ENTRYPOINTS_H_H

#include "src/utility/storm.h"

#include "src/storage/SymbolicModelDescription.h"
#include "src/utility/ExplicitExporter.h"

#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace cli {

        template<typename ValueType>
        void verifySparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            for (auto const& formula : formulas) {
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula, onlyInitialStatesRelevant));
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

#ifdef STORM_HAVE_CARL
        template<>
        inline void verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant) {

            for (auto const& formula : formulas) {
                STORM_LOG_THROW(model->getType() == storm::models::ModelType::Dtmc || model->getType() == storm::models::ModelType::Ctmc, storm::exceptions::InvalidSettingsException, "Currently parametric verification is only available for DTMCs and CTMCs.");
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula, onlyInitialStatesRelevant));
                if (result) {
                    std::cout << " done." << std::endl;
                    std::cout << "Result (initial states): ";
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                    std::cout << *result << std::endl;
                } else {
                    std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                }

                if (storm::settings::getModule<storm::settings::modules::ParametricSettings>().exportResultToFile()) {
                    exportParametricResultToFile(result->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[*model->getInitialStates().begin()], storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector(*(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>())), storm::settings::getModule<storm::settings::modules::ParametricSettings>().exportResultPath());
                }
            }
        }
#endif

        template<storm::dd::DdType DdType>
        void verifySymbolicModelWithAbstractionRefinementEngine(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Abstraction Refinement is not yet implemented.");
        }
        
        template<typename ValueType>
        void verifySymbolicModelWithExplorationEngine(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            STORM_LOG_THROW(model.isPrismProgram(), storm::exceptions::InvalidSettingsException, "Exploration engine is currently only applicable to PRISM models.");
            storm::prism::Program const& program = model.asPrismProgram();
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::InvalidSettingsException, "Currently exploration-based verification is only available for DTMCs and MDPs.");

            for (auto const& formula : formulas) {
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                bool supportFormula;
                std::unique_ptr<storm::modelchecker::CheckResult> result;

                if(program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                    storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Dtmc<ValueType>> checker(program);
                    storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);

                    supportFormula = checker.canHandle(task);
                    if (supportFormula) {
                        result = checker.check(task);
                    }
                } else if(program.getModelType() == storm::prism::Program::ModelType::MDP) {
                    storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Mdp<ValueType>> checker(program);
                    storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);

                    supportFormula = checker.canHandle(task);
                    if (supportFormula) {
                        result = checker.check(task);
                    }
                } else {
                    // Should be catched before.
                    assert(false);
                }
                if(!supportFormula) {
                    std::cout << " skipped, because the formula cannot be handled by the selected engine/method." << std::endl;
                }

                if (result) {
                    std::cout << " done." << std::endl;
                    std::cout << "Result (initial states): ";
                    std::cout << *result << std::endl;
                } else {
                    std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                }
            }
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        void verifySymbolicModelWithExplorationEngine<storm::RationalFunction>(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Exploration-based verification does currently not support parametric models.");
        }
#endif

        template<storm::dd::DdType DdType>
        void verifySymbolicModelWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            for (auto const& formula : formulas) {
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySymbolicModelWithHybridEngine(model, formula, onlyInitialStatesRelevant));

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
        void verifySymbolicModelWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            for (auto const& formula : formulas) {
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySymbolicModelWithDdEngine(model, formula, onlyInitialStatesRelevant));
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
        
#define BRANCH_ON_MODELTYPE(result, model, value_type, dd_type, function, ...) \
    if (model->isSymbolicModel()) { \
        if (model->isOfType(storm::models::ModelType::Dtmc)) { \
            result = function<storm::models::symbolic::Dtmc<dd_type>>(model->as<storm::models::symbolic::Dtmc<dd_type>>(), __VA_ARGS__); \
        } else if (model->isOfType(storm::models::ModelType::Ctmc)) { \
            result = function<storm::models::symbolic::Ctmc<dd_type>>(model->as<storm::models::symbolic::Ctmc<dd_type>>(), __VA_ARGS__); \
        } else if (model->isOfType(storm::models::ModelType::Mdp)) { \
            result = function<storm::models::symbolic::Mdp<dd_type>>(model->as<storm::models::symbolic::Mdp<dd_type>>(), __VA_ARGS__); \
        } else { \
            STORM_LOG_ASSERT(false, "Unknown model type."); \
        } \
    } else { \
        STORM_LOG_ASSERT(model->isSparseModel(), "Unknown model type."); \
        if (model->isOfType(storm::models::ModelType::Dtmc)) { \
            result = function<storm::models::sparse::Dtmc<value_type>>(model->as<storm::models::sparse::Dtmc<value_type>>(), __VA_ARGS__); \
        } else if (model->isOfType(storm::models::ModelType::Ctmc)) { \
            result = function<storm::models::sparse::Ctmc<value_type>>(model->as<storm::models::sparse::Ctmc<value_type>>(), __VA_ARGS__); \
        } else if (model->isOfType(storm::models::ModelType::Mdp)) { \
            result = function<storm::models::sparse::Mdp<value_type>>(model->as<storm::models::sparse::Mdp<value_type>>(), __VA_ARGS__); \
        } else if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) { \
            result = function<storm::models::sparse::MarkovAutomaton<value_type>>(model->as<storm::models::sparse::MarkovAutomaton<value_type>>(), __VA_ARGS__); \
        } else { \
            STORM_LOG_ASSERT(false, "Unknown model type."); \
        } \
    }
    
#define BRANCH_ON_SPARSE_MODELTYPE(result, model, value_type, function, ...) \
    STORM_LOG_ASSERT(model->isSparseModel(), "Illegal model type."); \
    if (model->isOfType(storm::models::ModelType::Dtmc)) { \
        result = function<storm::models::sparse::Dtmc<value_type>>(model->template as<storm::models::sparse::Dtmc<value_type>>(), __VA_ARGS__); \
    } else if (model->isOfType(storm::models::ModelType::Ctmc)) { \
        result = function<storm::models::sparse::Ctmc<value_type>>(model->template as<storm::models::sparse::Ctmc<value_type>>(), __VA_ARGS__); \
    } else if (model->isOfType(storm::models::ModelType::Mdp)) { \
        result = function<storm::models::sparse::Mdp<value_type>>(model->template as<storm::models::sparse::Mdp<value_type>>(), __VA_ARGS__); \
    } else if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) { \
        result = function<storm::models::sparse::MarkovAutomaton<value_type>>(model->template as<storm::models::sparse::MarkovAutomaton<value_type>>(), __VA_ARGS__); \
    } else { \
        STORM_LOG_ASSERT(false, "Unknown model type."); \
    }
        
        template<storm::dd::DdType LibraryType>
        void buildAndCheckSymbolicModelWithSymbolicEngine(bool hybrid, storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            // Start by building the model.
            auto markovModel = buildSymbolicModel<double, LibraryType>(model, formulas);
            
            // Print some information about the model.
            markovModel->printModelInformationToStream(std::cout);
            
            // Then select the correct engine.
            if (hybrid) {
                verifySymbolicModelWithHybridEngine(markovModel, formulas, onlyInitialStatesRelevant);
            } else {
                verifySymbolicModelWithDdEngine(markovModel, formulas, onlyInitialStatesRelevant);
            }
        }
        
        template<typename ValueType>
        void buildAndCheckSymbolicModelWithSparseEngine(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            // Start by building the model.
            std::shared_ptr<storm::models::ModelBase> markovModel = buildSparseModel<ValueType>(model, formulas);
            
            // Print some information about the model.
            markovModel->printModelInformationToStream(std::cout);
            
            // Preprocess the model.
            BRANCH_ON_SPARSE_MODELTYPE(markovModel, markovModel, ValueType, preprocessModel, formulas);
            
            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = markovModel->template as<storm::models::sparse::Model<ValueType>>();
            
            // Finally, treat the formulas.
            if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isCounterexampleSet()) {
                generateCounterexamples<ValueType>(model, sparseModel, formulas);
            } else {
                verifySparseModel<ValueType>(sparseModel, formulas, onlyInitialStatesRelevant);
            }
            
            // And export if required.
            if(storm::settings::getModule<storm::settings::modules::IOSettings>().isExportExplicitSet()) {
                std::ofstream ofs;
                ofs.open(storm::settings::getModule<storm::settings::modules::IOSettings>().getExportExplicitFilename(), std::ofstream::out);
                storm::exporter::explicitExportSparseModel(ofs, sparseModel, model.getParameterNames());
                ofs.close();
            }
        }
        
        template<typename ValueType>
        void buildAndCheckSymbolicModel(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            if (storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::AbstractionRefinement) {
                auto ddlib = storm::settings::getModule<storm::settings::modules::CoreSettings>().getDdLibraryType();
                if (ddlib == storm::dd::DdType::CUDD) {
                    verifySymbolicModelWithAbstractionRefinementEngine<storm::dd::DdType::CUDD>(model, formulas, onlyInitialStatesRelevant);
                } else {
                    verifySymbolicModelWithAbstractionRefinementEngine<storm::dd::DdType::Sylvan>(model, formulas, onlyInitialStatesRelevant);
                }
            } else if (storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::Exploration) {
                verifySymbolicModelWithExplorationEngine<ValueType>(model, formulas, onlyInitialStatesRelevant);
            } else {
                auto engine = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine();
                if (engine == storm::settings::modules::CoreSettings::Engine::Dd || engine == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                    auto ddlib = storm::settings::getModule<storm::settings::modules::CoreSettings>().getDdLibraryType();
                    if (ddlib == storm::dd::DdType::CUDD) {
                        buildAndCheckSymbolicModelWithSymbolicEngine<storm::dd::DdType::CUDD>(engine == storm::settings::modules::CoreSettings::Engine::Hybrid, model, formulas, onlyInitialStatesRelevant);
                    } else {
                        buildAndCheckSymbolicModelWithSymbolicEngine<storm::dd::DdType::Sylvan>(engine == storm::settings::modules::CoreSettings::Engine::Hybrid, model, formulas, onlyInitialStatesRelevant);
                    }
                } else {
                    STORM_LOG_THROW(engine == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Illegal engine.");
                    buildAndCheckSymbolicModelWithSparseEngine<ValueType>(model, formulas, onlyInitialStatesRelevant);
                }
            }
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        void buildAndCheckSymbolicModel<storm::RationalNumber>(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant) {
            STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Cannot use this data type with an engine different than the sparse one.");
            buildAndCheckSymbolicModelWithSparseEngine<storm::RationalNumber>(model, formulas, onlyInitialStatesRelevant);
        }
        
        template<>
        void buildAndCheckSymbolicModel<storm::RationalFunction>(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant) {
            STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Cannot use this data type with an engine different than the sparse one.");
            buildAndCheckSymbolicModelWithSparseEngine<storm::RationalFunction>(model, formulas, onlyInitialStatesRelevant);
        }
#endif
        
        template<typename ValueType>
        void buildAndCheckExplicitModel(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool onlyInitialStatesRelevant = false) {
            storm::settings::modules::IOSettings const& settings = storm::settings::getModule<storm::settings::modules::IOSettings>();

            STORM_LOG_THROW(settings.isExplicitSet(), storm::exceptions::InvalidStateException, "Unable to build explicit model without model files.");
            std::shared_ptr<storm::models::ModelBase> model = buildExplicitModel<ValueType>(settings.getTransitionFilename(), settings.getLabelingFilename(), settings.isStateRewardsSet() ? boost::optional<std::string>(settings.getStateRewardsFilename()) : boost::none, settings.isTransitionRewardsSet() ? boost::optional<std::string>(settings.getTransitionRewardsFilename()) : boost::none, settings.isChoiceLabelingSet() ? boost::optional<std::string>(settings.getChoiceLabelingFilename()) : boost::none);
            
            // Preprocess the model if needed.
            BRANCH_ON_MODELTYPE(model, model, ValueType, storm::dd::DdType::CUDD, preprocessModel, formulas);

            // Print some information about the model.
            model->printModelInformationToStream(std::cout);

            // Verify the model, if a formula was given.
            if (!formulas.empty()) {
                STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidStateException, "Expected sparse model.");
                verifySparseModel<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), formulas, onlyInitialStatesRelevant);
            }
        }
    }
}

#endif //STORM_ENTRYPOINTS_H_H
