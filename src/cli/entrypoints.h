#ifndef STORM_ENTRYPOINTS_H_H
#define STORM_ENTRYPOINTS_H_H

#include "src/utility/storm.h"

namespace storm {
    namespace cli {

        template<typename ValueType>
        void verifySparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
            for (auto const& formula : formulas) {
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula));
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
        inline void verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
            if (storm::settings::generalSettings().isParametricRegionSet()){
                auto regions=storm::modelchecker::region::ParameterRegion<storm::RationalFunction>::getRegionsFromSettings();
                if(model->getType() == storm::models::ModelType::Dtmc){
                    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();
                    storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double> modelchecker(*dtmc);
                    for (auto const& formula : formulas) {
                        std::cout << std::endl << "Model checking property: " << *formula << " for all parameters in the given regions." << std::endl;
                        STORM_LOG_THROW(modelchecker.canHandle(*formula.get()), storm::exceptions::InvalidSettingsException, "The parametric region check engine does not support this property.");
                        modelchecker.specifyFormula(formula);
                        modelchecker.checkRegions(regions);
                        modelchecker.printStatisticsToStream(std::cout);
                    }
                } else if (model->getType() == storm::models::ModelType::Mdp){
                    std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> mdp = model->template as<storm::models::sparse::Mdp<storm::RationalFunction>>();
                    storm::modelchecker::region::SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double> modelchecker(*mdp);
                    for (auto const& formula : formulas) {
                        std::cout << std::endl << "Model checking property: " << *formula << " for all parameters in the given regions." << std::endl;
                        STORM_LOG_THROW(modelchecker.canHandle(*formula.get()), storm::exceptions::InvalidSettingsException, "The parametric region check engine does not support this property.");
                        modelchecker.specifyFormula(formula);
                        modelchecker.checkRegions(regions);
                        modelchecker.printStatisticsToStream(std::cout);
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Currently parametric region verification is only available for DTMCs and Mdps.");
                }
               // for(auto const& reg : regions){
               //     std::cout << reg.toString() << "      Result: " << reg.getCheckResult() << std::endl;
               // }
            } else {
                for (auto const& formula : formulas) {
                    STORM_LOG_THROW(model->getType() == storm::models::ModelType::Dtmc, storm::exceptions::InvalidSettingsException, "Currently parametric verification is only available for DTMCs.");

                    std::cout << std::endl << "Model checking property: " << *formula << " ...";

                    std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula));


                    if (result) {
                        std::cout << " done." << std::endl;
                        std::cout << "Result (initial states): ";
                        result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                        std::cout << *result << std::endl;
                    } else {
                        std::cout << " skipped, because the modelling formalism is currently unsupported." << std::endl;
                    }

                    storm::settings::modules::ParametricSettings const& parametricSettings = storm::settings::parametricSettings();
                    if (parametricSettings.exportResultToFile()) {
                        exportParametricResultToFile(result->asExplicitQuantitativeCheckResult<storm::RationalFunction>()[*model->getInitialStates().begin()], storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector(*(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>())), parametricSettings.exportResultPath());
                    }
                }
            }
        }
#endif

        template<storm::dd::DdType DdType>
        void verifySymbolicModelWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
            for (auto const& formula : formulas) {
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySymbolicModelWithHybridEngine(model, formula));

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
        void verifySymbolicModelWithSymbolicEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
            for (auto const& formula : formulas) {
                std::cout << std::endl << "Model checking property: " << *formula << " ...";
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySymbolicModelWithDdEngine(model, formula));
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
    
        template<typename ValueType>
        void buildAndCheckSymbolicModel(storm::prism::Program const& program, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
            std::shared_ptr<storm::models::ModelBase> model = buildSymbolicModel<ValueType>(program, formulas);
            STORM_LOG_THROW(model != nullptr, storm::exceptions::InvalidStateException, "Model could not be constructed for an unknown reason.");

            // Preprocess the model if needed.
            BRANCH_ON_MODELTYPE(model, model, ValueType, storm::dd::DdType::CUDD, preprocessModel, formulas);

            // Print some information about the model.
            model->printModelInformationToStream(std::cout);

            // Verify the model, if a formula was given.
            if (!formulas.empty()) {
                if (model->isSparseModel()) {
                    if(storm::settings::generalSettings().isCounterexampleSet()) {
                        // If we were requested to generate a counterexample, we now do so for each formula.
                        for(auto const& formula : formulas) {
                            generateCounterexample<ValueType>(program, model->as<storm::models::sparse::Model<ValueType>>(), formula);
                        }
                    } else {
                        verifySparseModel<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), formulas);
                    }
                } else if (model->isSymbolicModel()) {
                    if (storm::settings::generalSettings().getEngine() == storm::settings::modules::GeneralSettings::Engine::Hybrid) {
                        verifySymbolicModelWithHybridEngine(model->as<storm::models::symbolic::Model<storm::dd::DdType::CUDD>>(), formulas);
                    } else {
                        verifySymbolicModelWithSymbolicEngine(model->as<storm::models::symbolic::Model<storm::dd::DdType::CUDD>>(), formulas);
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
            std::shared_ptr<storm::models::ModelBase> model = buildExplicitModel<ValueType>(settings.getTransitionFilename(), settings.getLabelingFilename(), settings.isStateRewardsSet() ? settings.getStateRewardsFilename() : boost::optional<std::string>(), settings.isTransitionRewardsSet() ? settings.getTransitionRewardsFilename() : boost::optional<std::string>(), settings.isChoiceLabelingSet() ? settings.getChoiceLabelingFilename() : boost::optional<std::string>());
            
            // Preprocess the model if needed.
            BRANCH_ON_MODELTYPE(model, model, ValueType, storm::dd::DdType::CUDD, preprocessModel, formulas);

            // Print some information about the model.
            model->printModelInformationToStream(std::cout);

            // Verify the model, if a formula was given.
            if (!formulas.empty()) {
                STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidStateException, "Expected sparse model.");
                verifySparseModel<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), formulas);
            }
        }
    }
}

#endif //STORM_ENTRYPOINTS_H_H
