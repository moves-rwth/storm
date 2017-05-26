#pragma once

#include <type_traits>

#include "storm/utility/storm.h"

#include "storm/analysis/GraphConditions.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/utility/DirectEncodingExporter.h"
#include "storm/utility/Stopwatch.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace cli {

        template<typename ValueType>
        void applyFilterFunctionAndOutput(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::modelchecker::FilterType ft) {
            if (result->isQuantitative()) {
                switch (ft) {
                    case storm::modelchecker::FilterType::VALUES:
                        STORM_PRINT_AND_LOG(*result);
                        break;
                    case storm::modelchecker::FilterType::SUM:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().sum());
                        break;
                    case storm::modelchecker::FilterType::AVG:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().average());
                        break;
                    case storm::modelchecker::FilterType::MIN:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().getMin());
                        break;
                    case storm::modelchecker::FilterType::MAX:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().getMax());
                        break;
                    case storm::modelchecker::FilterType::ARGMIN:
                    case storm::modelchecker::FilterType::ARGMAX:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Outputting states is not supported");
                    case storm::modelchecker::FilterType::EXISTS:
                    case storm::modelchecker::FilterType::FORALL:
                    case storm::modelchecker::FilterType::COUNT:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "FilterType only defined for qualitative results");
                }
            } else {
                switch (ft) {
                    case storm::modelchecker::FilterType::VALUES:
                        STORM_PRINT_AND_LOG(*result << std::endl);
                        break;
                    case storm::modelchecker::FilterType::EXISTS:
                        STORM_PRINT_AND_LOG(result->asQualitativeCheckResult().existsTrue());
                        break;
                    case storm::modelchecker::FilterType::FORALL:
                        STORM_PRINT_AND_LOG(result->asQualitativeCheckResult().forallTrue());
                        break;
                    case storm::modelchecker::FilterType::COUNT:
                        STORM_PRINT_AND_LOG(result->asQualitativeCheckResult().count());
                        break;
                        
                    case storm::modelchecker::FilterType::ARGMIN:
                    case storm::modelchecker::FilterType::ARGMAX:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Outputting states is not supported");
                    case storm::modelchecker::FilterType::SUM:
                    case storm::modelchecker::FilterType::AVG:
                    case storm::modelchecker::FilterType::MIN:
                    case storm::modelchecker::FilterType::MAX:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "FilterType only defined for quantitative results");
                }
            }
            STORM_PRINT_AND_LOG(std::endl);
        }
        
        template<typename ValueType>
        void verifySparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::vector<storm::jani::Property> const& properties, bool onlyInitialStatesRelevant = false) {
            for (auto const& property : properties) {
                STORM_PRINT_AND_LOG(std::endl << "Model checking property " << *property.getRawFormula() << " ..." << std::endl);
                std::cout.flush();
                storm::utility::Stopwatch modelCheckingWatch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, property.getFilter().getFormula(), onlyInitialStatesRelevant));
                modelCheckingWatch.stop();
                if (result) {
                    STORM_PRINT_AND_LOG("Result (initial states): ");
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                    applyFilterFunctionAndOutput<ValueType>(result, property.getFilter().getFilterType());
                    STORM_PRINT_AND_LOG("Time for model checking: " << modelCheckingWatch << "." << std::endl);
                } else {
                    STORM_PRINT_AND_LOG(" skipped, because the modelling formalism is currently unsupported." << std::endl);
                }
            }
        }

#ifdef STORM_HAVE_CARL
        template<>
        inline void verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::vector<storm::jani::Property> const& properties, bool onlyInitialStatesRelevant) {

            for (auto const& property : properties) {
                STORM_LOG_THROW(model->getType() == storm::models::ModelType::Dtmc || model->getType() == storm::models::ModelType::Ctmc, storm::exceptions::InvalidSettingsException, "Currently parametric verification is only available for DTMCs and CTMCs.");
                STORM_PRINT_AND_LOG(std::endl << "Model checking property " << *property.getRawFormula() << " ..." << std::endl);
                std::cout.flush();
                storm::utility::Stopwatch modelCheckingWatch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, property.getFilter().getFormula(), onlyInitialStatesRelevant));
                modelCheckingWatch.stop();
                if (result) {
                    STORM_PRINT_AND_LOG("Result (initial states): ");
                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                    applyFilterFunctionAndOutput<storm::RationalFunction>(result, property.getFilter().getFilterType());
                    STORM_PRINT_AND_LOG("Time for model checking: " << modelCheckingWatch << "." << std::endl);
                } else {
                    STORM_PRINT_AND_LOG(" skipped, because the modelling formalism is currently unsupported." << std::endl);
                }

                if (storm::settings::getModule<storm::settings::modules::ParametricSettings>().exportResultToFile()) {
                    exportParametricResultToFile(result->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[*model->getInitialStates().begin()], storm::analysis::ConstraintCollector<storm::RationalFunction>(*(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>())), storm::settings::getModule<storm::settings::modules::ParametricSettings>().exportResultPath());
                }
            }
        }
#endif

        template<storm::dd::DdType DdType>
        void verifySymbolicModelWithAbstractionRefinementEngine(storm::storage::SymbolicModelDescription const& model, std::vector<storm::jani::Property> const& properties, bool onlyInitialStatesRelevant = false) {
            typedef double ValueType;
            for (auto const& property : properties) {
                STORM_PRINT_AND_LOG(std::endl << "Model checking property " << *property.getRawFormula() << " ..." << std::endl);
                std::cout.flush();
                storm::utility::Stopwatch modelCheckingWatch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySymbolicModelWithAbstractionRefinementEngine<DdType, ValueType>(model, property.getFilter().getFormula(), onlyInitialStatesRelevant));
                modelCheckingWatch.stop();
                if (result) {
                    STORM_PRINT_AND_LOG("Result (initial states): ");
                    STORM_PRINT_AND_LOG(*result << std::endl);
                    STORM_PRINT_AND_LOG("Time for model checking: " << modelCheckingWatch << "." << std::endl);
                } else {
                    STORM_PRINT_AND_LOG(" skipped, because the modelling formalism is currently unsupported." << std::endl);
                }
            }
        }
        
        template<typename ValueType>
        void verifySymbolicModelWithExplorationEngine(storm::storage::SymbolicModelDescription const& model, std::vector<storm::jani::Property> const& formulas, bool onlyInitialStatesRelevant = false) {
            STORM_LOG_THROW(model.isPrismProgram(), storm::exceptions::InvalidSettingsException, "Exploration engine is currently only applicable to PRISM models.");
            storm::prism::Program const& program = model.asPrismProgram();
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::InvalidSettingsException, "Currently exploration-based verification is only available for DTMCs and MDPs.");

            for (auto const& property : formulas) {
                STORM_PRINT_AND_LOG(std::endl << "Model checking property " << *property.getRawFormula() << " ..." << std::endl);
                std::cout.flush();
                bool formulaSupported = false;
                std::unique_ptr<storm::modelchecker::CheckResult> result;

                storm::utility::Stopwatch modelCheckingWatch(false);
                
                if (program.getModelType() == storm::prism::Program::ModelType::DTMC) {
                    storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Dtmc<ValueType>> checker(program);
                    storm::modelchecker::CheckTask<storm::logic::Formula> task(*property.getFilter().getFormula(), onlyInitialStatesRelevant);

                    formulaSupported = checker.canHandle(task);
                    if (formulaSupported) {
                        modelCheckingWatch.start();
                        result = checker.check(task);
                        modelCheckingWatch.stop();
                    }
                } else if (program.getModelType() == storm::prism::Program::ModelType::MDP) {
                    storm::modelchecker::SparseExplorationModelChecker<storm::models::sparse::Mdp<ValueType>> checker(program);
                    storm::modelchecker::CheckTask<storm::logic::Formula> task(*property.getFilter().getFormula(), onlyInitialStatesRelevant);

                    formulaSupported = checker.canHandle(task);
                    if (formulaSupported) {
                        modelCheckingWatch.start();
                        result = checker.check(task);
                        modelCheckingWatch.stop();
                    }
                } else {
                    // Should be catched before.
                    assert(false);
                }
                if (!formulaSupported) {
                    STORM_PRINT_AND_LOG(" skipped, because the formula cannot be handled by the selected engine/method." << std::endl);
                }

                if (result) {
                    STORM_PRINT_AND_LOG("Result (initial states): ");
                    applyFilterFunctionAndOutput<ValueType>(result, property.getFilter().getFilterType());
                    STORM_PRINT_AND_LOG("Time for model checking: " << modelCheckingWatch << "." << std::endl);
                } else {
                    STORM_PRINT_AND_LOG(" skipped, because the modelling formalism is currently unsupported." << std::endl);
                }
            }
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        void verifySymbolicModelWithExplorationEngine<storm::RationalFunction>(storm::storage::SymbolicModelDescription const& , std::vector<storm::jani::Property> const& , bool ) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Exploration-based verification does currently not support parametric models.");
        }
#endif

        template<storm::dd::DdType DdType, typename ValueType>
        void verifySymbolicModelWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> model, std::vector<storm::jani::Property> const& formulas, bool onlyInitialStatesRelevant = false) {
            for (auto const& property : formulas) {
                STORM_PRINT_AND_LOG(std::endl << "Model checking property " << *property.getRawFormula() << " ..." << std::endl);
                std::cout.flush();
                
                storm::utility::Stopwatch modelCheckingWatch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySymbolicModelWithHybridEngine(model, property.getFilter().getFormula(), onlyInitialStatesRelevant));
                modelCheckingWatch.stop();

                if (result) {
                    STORM_PRINT_AND_LOG("Result (initial states): ");
                    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(model->getReachableStates(), model->getInitialStates()));
                    applyFilterFunctionAndOutput<double>(result, property.getFilter().getFilterType());
                    STORM_PRINT_AND_LOG("Time for model checking: " << modelCheckingWatch << "." << std::endl);
                } else {
                    STORM_PRINT_AND_LOG(" skipped, because the modelling formalism is currently unsupported." << std::endl);
                }
            }
        }

        template<storm::dd::DdType DdType, typename ValueType>
        void verifySymbolicModelWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> model, std::vector<storm::jani::Property> const& formulas, bool onlyInitialStatesRelevant = false) {
            for (auto const& property : formulas) {
                STORM_PRINT_AND_LOG(std::endl << "Model checking property " << *property.getRawFormula() << " ..." << std::endl);
                std::cout.flush();
                
                storm::utility::Stopwatch modelCheckingWatch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySymbolicModelWithDdEngine(model, property.getFilter().getFormula(), onlyInitialStatesRelevant));
                modelCheckingWatch.stop();
                if (result) {
                    STORM_PRINT_AND_LOG("Result (initial states): ");
                    result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(model->getReachableStates(), model->getInitialStates()));
                    applyFilterFunctionAndOutput<double>(result, property.getFilter().getFilterType());
                    STORM_PRINT_AND_LOG("Time for model checking: " << modelCheckingWatch << "." << std::endl);
                } else {
                    STORM_PRINT_AND_LOG(" skipped, because the modelling formalism is currently unsupported." << std::endl);
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
        
        template<storm::dd::DdType LibraryType, typename ValueType = double>
        void buildAndCheckSymbolicModelWithSymbolicEngine(bool hybrid, storm::storage::SymbolicModelDescription const& model, std::vector<storm::jani::Property> const& properties, bool onlyInitialStatesRelevant = false) {
            // Start by building the model.
            storm::utility::Stopwatch modelBuildingWatch(true);
            auto markovModel = buildSymbolicModel<ValueType, LibraryType>(model, extractFormulasFromProperties(properties));
            modelBuildingWatch.stop();
            STORM_PRINT_AND_LOG("Time for model construction: " << modelBuildingWatch << "." << std::endl << std::endl);
            
            // Print some information about the model.
            markovModel->printModelInformationToStream(std::cout);
            
            // Then select the correct engine.
            if (hybrid) {
                if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isParameterLiftingSet()) {
                    STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::GeneralSettings>().isParametricSet(), storm::exceptions::InvalidSettingsException, "Invoked parameter lifting without enabling the parametric engine.");
                    STORM_PRINT_AND_LOG("Transforming symbolic model to sparse model ...");
                    auto sparseModel = storm::transformer::transformSymbolicToSparseModel(markovModel);
                    STORM_PRINT_AND_LOG(" done" << std::endl);
                    sparseModel->printModelInformationToStream(std::cout);
                    auto formulas = extractFormulasFromProperties(properties);
                    storm::performParameterLifting<ValueType>(sparseModel, formulas);
                } else {
                    verifySymbolicModelWithHybridEngine<LibraryType, ValueType>(markovModel, properties, onlyInitialStatesRelevant);
                }
            } else {
                verifySymbolicModelWithDdEngine<LibraryType, ValueType>(markovModel, properties, onlyInitialStatesRelevant);
            }
        }
        
        template<typename ValueType>
        void buildAndCheckSymbolicModelWithSparseEngine(storm::storage::SymbolicModelDescription const& model, std::vector<storm::jani::Property> const& properties, bool onlyInitialStatesRelevant = false) {
            auto formulas = extractFormulasFromProperties(properties);
            // Start by building the model.
            storm::utility::Stopwatch modelBuildingWatch(true);
            std::shared_ptr<storm::models::ModelBase> markovModel = buildSparseModel<ValueType>(model, formulas);
            STORM_LOG_THROW(markovModel, storm::exceptions::UnexpectedException, "The model was not successfully built.");
            modelBuildingWatch.stop();
            STORM_PRINT_AND_LOG("Time for model construction: " << modelBuildingWatch << "." << std::endl << std::endl);

            // Print some information about the model.
            markovModel->printModelInformationToStream(std::cout);
            
            // Preprocess the model.
            BRANCH_ON_SPARSE_MODELTYPE(markovModel, markovModel, ValueType, preprocessModel, formulas);
            std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = markovModel->template as<storm::models::sparse::Model<ValueType>>();
            
            // Finally, treat the formulas.
            if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isCounterexampleSet()) {
                generateCounterexamples<ValueType>(model, sparseModel, formulas);
            } else if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isParameterLiftingSet()) {
                STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::GeneralSettings>().isParametricSet(), storm::exceptions::InvalidSettingsException, "Invoked parameter lifting without enabling the parametric engine.");
                storm::performParameterLifting<ValueType>(sparseModel, formulas);
            } else {
                verifySparseModel<ValueType>(sparseModel, properties, onlyInitialStatesRelevant);
            }
            
            // And export if required.
            if(storm::settings::getModule<storm::settings::modules::IOSettings>().isExportExplicitSet()) {
                std::ofstream stream;
                storm::utility::openFile(storm::settings::getModule<storm::settings::modules::IOSettings>().getExportExplicitFilename(), stream);
                storm::exporter::explicitExportSparseModel(stream, sparseModel, model.getParameterNames());
                storm::utility::closeFile(stream);
            }

            // And export DOT if required.
            if(storm::settings::getModule<storm::settings::modules::IOSettings>().isExportDotSet()) {
                std::ofstream stream;
                storm::utility::openFile(storm::settings::getModule<storm::settings::modules::IOSettings>().getExportDotFilename(), stream);
                sparseModel->writeDotToStream(stream);
                storm::utility::closeFile(stream);
            }
        }
        
        template<typename ValueType>
        void buildAndCheckSymbolicModel(storm::storage::SymbolicModelDescription const& model, std::vector<storm::jani::Property> const& formulas, bool onlyInitialStatesRelevant = false) {
            if (storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine() == storm::settings::modules::CoreSettings::Engine::AbstractionRefinement) {
                STORM_LOG_THROW((std::is_same<double, ValueType>::value), storm::exceptions::InvalidTypeException, "The value type is not supported by abstraction refinement.");
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
        void buildAndCheckSymbolicModel<storm::RationalNumber>(storm::storage::SymbolicModelDescription const& model, std::vector<storm::jani::Property> const& formulas, bool onlyInitialStatesRelevant) {
            auto engine = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine();
            if (engine == storm::settings::modules::CoreSettings::Engine::Dd || engine == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                auto ddlib = storm::settings::getModule<storm::settings::modules::CoreSettings>().getDdLibraryType();
                STORM_LOG_THROW(ddlib == storm::dd::DdType::Sylvan, storm::exceptions::InvalidSettingsException, "This data-type is only available when selecting sylvan.");
                buildAndCheckSymbolicModelWithSymbolicEngine<storm::dd::DdType::Sylvan, storm::RationalNumber>(engine == storm::settings::modules::CoreSettings::Engine::Hybrid, model, formulas, onlyInitialStatesRelevant);
            } else if (engine == storm::settings::modules::CoreSettings::Engine::Sparse) {
                buildAndCheckSymbolicModelWithSparseEngine<storm::RationalNumber>(model, formulas, onlyInitialStatesRelevant);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Cannot use this data type with this engine.");
            }
        }
        
        template<>
        void buildAndCheckSymbolicModel<storm::RationalFunction>(storm::storage::SymbolicModelDescription const& model, std::vector<storm::jani::Property> const& formulas, bool onlyInitialStatesRelevant) {
            auto engine = storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine();
            if (engine == storm::settings::modules::CoreSettings::Engine::Dd || engine == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                auto ddlib = storm::settings::getModule<storm::settings::modules::CoreSettings>().getDdLibraryType();
                STORM_LOG_THROW(ddlib == storm::dd::DdType::Sylvan, storm::exceptions::InvalidSettingsException, "This data-type is only available when selecting sylvan.");
                buildAndCheckSymbolicModelWithSymbolicEngine<storm::dd::DdType::Sylvan, storm::RationalFunction>(engine == storm::settings::modules::CoreSettings::Engine::Hybrid, model, formulas, onlyInitialStatesRelevant);
            } else if (engine == storm::settings::modules::CoreSettings::Engine::Sparse) {
                buildAndCheckSymbolicModelWithSparseEngine<storm::RationalFunction>(model, formulas, onlyInitialStatesRelevant);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Cannot use this data type with this engine.");
            }
        }
#endif
        
        template<typename ValueType>
        void buildAndCheckExplicitModel(std::vector<storm::jani::Property> const& properties, bool onlyInitialStatesRelevant = false) {
            storm::settings::modules::IOSettings const& settings = storm::settings::getModule<storm::settings::modules::IOSettings>();

            STORM_LOG_THROW(settings.isExplicitSet() || settings.isExplicitDRNSet(), storm::exceptions::InvalidStateException, "Unable to build explicit model without model files.");

            storm::utility::Stopwatch modelBuildingWatch(true);
            std::shared_ptr<storm::models::ModelBase> model;
            if (settings.isExplicitSet()) {
                model = buildExplicitModel<ValueType>(settings.getTransitionFilename(), settings.getLabelingFilename(), settings.isStateRewardsSet() ? boost::optional<std::string>(settings.getStateRewardsFilename()) : boost::none, settings.isTransitionRewardsSet() ? boost::optional<std::string>(settings.getTransitionRewardsFilename()) : boost::none, settings.isChoiceLabelingSet() ? boost::optional<std::string>(settings.getChoiceLabelingFilename()) : boost::none);
            } else {
                model = buildExplicitDRNModel<ValueType>(settings.getExplicitDRNFilename());
            }
            modelBuildingWatch.stop();
            STORM_PRINT_AND_LOG("Time for model construction: " << modelBuildingWatch << "." << std::endl);
            
            // Preprocess the model if needed.
            BRANCH_ON_MODELTYPE(model, model, ValueType, storm::dd::DdType::CUDD, preprocessModel, extractFormulasFromProperties(properties));

            // Print some information about the model.
            model->printModelInformationToStream(std::cout);

            // Verify the model, if a formula was given.
            if (!properties.empty()) {
                STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidStateException, "Expected sparse model.");
                verifySparseModel<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), properties, onlyInitialStatesRelevant);
            }
            
            // Export DOT if required.
            if(storm::settings::getModule<storm::settings::modules::IOSettings>().isExportDotSet()) {
                std::ofstream stream;
                storm::utility::openFile(storm::settings::getModule<storm::settings::modules::IOSettings>().getExportDotFilename(), stream);
                model->as<storm::models::sparse::Model<ValueType>>()->writeDotToStream(stream);
                storm::utility::closeFile(stream);
            }

        }
    }
}
