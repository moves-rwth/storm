#pragma once

#include <type_traits>

#include "storm/modelchecker/abstraction/GameBasedMdpModelChecker.h"

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
            STORM_LOG_THROW(model.isPrismProgram(), storm::exceptions::InvalidSettingsException, "Exploration engine is currently only applicable to PRISM models.");
            storm::prism::Program const& program = model.asPrismProgram();
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::InvalidSettingsException, "Currently exploration-based verification is only available for DTMCs and MDPs.");

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
        
        template<typename ParametricType>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithParameterLifting(std::shared_ptr<storm::models::sparse::Model<ParametricType>>, std::shared_ptr<storm::logic::Formula const> const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter-lifting is unavailable for this data-type.");
        }
            
        template<>
        std::unique_ptr<storm::modelchecker::CheckResult> verifyWithParameterLifting(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> markovModel, std::shared_ptr<storm::logic::Formula const> const& formula) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Parameter-lifting is currently unavailable from the API.");
//                storm::utility::Stopwatch parameterLiftingStopWatch(true);
//                std::shared_ptr<storm::logic::Formula const> consideredFormula = formula;
//                
//                STORM_LOG_WARN_COND(storm::utility::parameterlifting::validateParameterLiftingSound(markovModel, formula), "Could not validate whether parameter lifting is sound on the input model and the formula " << *formula);
//                
//                if (markovModel->isOfType(storm::models::ModelType::Ctmc) || markovModel->isOfType(storm::models::ModelType::MarkovAutomaton)) {
//                    STORM_PRINT_AND_LOG("Transforming continuous model to discrete model...");
//                    storm::transformer::transformContinuousToDiscreteModelInPlace(markovModel, consideredFormula);
//                    STORM_PRINT_AND_LOG(" done!" << std::endl);
//                    markovModel->printModelInformationToStream(std::cout);
//                }
//                
//                auto modelParameters = storm::models::sparse::getProbabilityParameters(*markovModel);
//                auto rewParameters = storm::models::sparse::getRewardParameters(*markovModel);
//                modelParameters.insert(rewParameters.begin(), rewParameters.end());
//                
//                STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::ParametricSettings>().isParameterSpaceSet(), storm::exceptions::InvalidSettingsException, "Invoked Parameter lifting but no parameter space was defined.");
//                auto parameterSpaceAsString = storm::settings::getModule<storm::settings::modules::ParametricSettings>().getParameterSpace();
//                auto parameterSpace = storm::storage::ParameterRegion<storm::RationalFunction>::parseRegion(parameterSpaceAsString, modelParameters);
//                auto refinementThreshold = storm::utility::convertNumber<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>(storm::settings::getModule<storm::settings::modules::ParametricSettings>().getRefinementThreshold());
//                std::vector<std::pair<storm::storage::ParameterRegion<storm::RationalFunction>, storm::modelchecker::parametric::RegionCheckResult>> result;
//                
//                STORM_PRINT_AND_LOG("Performing parameter lifting for property " << *consideredFormula << " with parameter space " << parameterSpace.toString(true) << " and refinement threshold " << storm::utility::convertNumber<double>(refinementThreshold) << " ..." << std::endl);
//                
//                storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction> task(*consideredFormula, true);
//                std::string resultVisualization;
//                
//                if (markovModel->isOfType(storm::models::ModelType::Dtmc)) {
//                    if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isExactSet()) {
//                        storm::modelchecker::parametric::SparseDtmcRegionChecker <storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber> regionChecker(*markovModel->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>());
//                        regionChecker.specifyFormula(task);
//                        result = regionChecker.performRegionRefinement(parameterSpace, refinementThreshold);
//                        parameterLiftingStopWatch.stop();
//                        if (modelParameters.size() == 2) {
//                            resultVisualization = regionChecker.visualizeResult(result, parameterSpace, *modelParameters.begin(), *(modelParameters.rbegin()));
//                        }
//                    } else {
//                        storm::modelchecker::parametric::SparseDtmcRegionChecker <storm::models::sparse::Dtmc<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*markovModel->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>());
//                        regionChecker.specifyFormula(task);
//                        result = regionChecker.performRegionRefinement(parameterSpace, refinementThreshold);
//                        parameterLiftingStopWatch.stop();
//                        if (modelParameters.size() == 2) {
//                            resultVisualization = regionChecker.visualizeResult(result, parameterSpace, *modelParameters.begin(), *(modelParameters.rbegin()));
//                        }
//                    }
//                } else if (markovModel->isOfType(storm::models::ModelType::Mdp)) {
//                    if (storm::settings::getModule<storm::settings::modules::GeneralSettings>().isExactSet()) {
//                        storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber> regionChecker(*markovModel->template as<storm::models::sparse::Mdp<storm::RationalFunction>>());
//                        regionChecker.specifyFormula(task);
//                        result = regionChecker.performRegionRefinement(parameterSpace, refinementThreshold);
//                        parameterLiftingStopWatch.stop();
//                        if (modelParameters.size() == 2) {
//                            resultVisualization = regionChecker.visualizeResult(result, parameterSpace, *modelParameters.begin(), *(modelParameters.rbegin()));
//                        }
//                    } else {
//                        storm::modelchecker::parametric::SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber> regionChecker(*markovModel->template as<storm::models::sparse::Mdp<storm::RationalFunction>>());
//                        regionChecker.specifyFormula(task);
//                        result = regionChecker.performRegionRefinement(parameterSpace, refinementThreshold);
//                        parameterLiftingStopWatch.stop();
//                        if (modelParameters.size() == 2) {
//                            resultVisualization = regionChecker.visualizeResult(result, parameterSpace, *modelParameters.begin(), *(modelParameters.rbegin()));
//                        }
//                    }
//                } else {
//                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to perform parameterLifting on the provided model type.");
//                }
//                
//                
//                auto satArea = storm::utility::zero<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>();
//                auto unsatArea = storm::utility::zero<typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType>();
//                uint_fast64_t numOfSatRegions = 0;
//                uint_fast64_t numOfUnsatRegions = 0;
//                for (auto const& res : result) {
//                    switch (res.second) {
//                        case storm::modelchecker::parametric::RegionCheckResult::AllSat:
//                            satArea += res.first.area();
//                            ++numOfSatRegions;
//                            break;
//                        case storm::modelchecker::parametric::RegionCheckResult::AllViolated:
//                            unsatArea += res.first.area();
//                            ++numOfUnsatRegions;
//                            break;
//                        default:
//                            STORM_LOG_ERROR("Unexpected result for region " << res.first.toString(true) << " : " << res.second << ".");
//                            break;
//                    }
//                }
//                typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType satAreaFraction = satArea / parameterSpace.area();
//                typename storm::storage::ParameterRegion<storm::RationalFunction>::CoefficientType unsatAreaFraction = unsatArea / parameterSpace.area();
//                STORM_PRINT_AND_LOG("Done! Found " << numOfSatRegions << " safe regions and "
//                                    << numOfUnsatRegions << " unsafe regions." << std::endl);
//                STORM_PRINT_AND_LOG(storm::utility::convertNumber<double>(satAreaFraction) * 100 << "% of the parameter space is safe, and "
//                                    << storm::utility::convertNumber<double>(unsatAreaFraction) * 100 << "% of the parameter space is unsafe." << std::endl);
//                STORM_PRINT_AND_LOG("Model checking with parameter lifting took " << parameterLiftingStopWatch << " seconds." << std::endl);
//                STORM_PRINT_AND_LOG(resultVisualization);
//                
//                if (storm::settings::getModule<storm::settings::modules::ParametricSettings>().exportResultToFile()) {
//                    std::string path = storm::settings::getModule<storm::settings::modules::ParametricSettings>().exportResultPath();
//                    STORM_PRINT_AND_LOG("Exporting result to path " << path << "." << std::endl);
//                    std::ofstream filestream;
//                    storm::utility::openFile(path, filestream);
//                    
//                    for (auto const& res : result) {
//                        switch (res.second) {
//                            case storm::modelchecker::parametric::RegionCheckResult::AllSat:
//                                filestream << "safe: " << res.first.toString(true) << std::endl;
//                                break;
//                            case storm::modelchecker::parametric::RegionCheckResult::AllViolated:
//                                filestream << "unsafe: " << res.first.toString(true) << std::endl;
//                                break;
//                            default:
//                                break;
//                        }
//                    }
//                }
//            }
        }
        
    }
}
