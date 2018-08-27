

#include "storm-cli-utilities/cli.h"
#include "storm-cli-utilities/model-handling.h"

#include "storm-pars/analysis/Lattice.h"

#include "storm-pars/api/storm-pars.h"
#include "storm-pars/api/region.h"

#include "storm-pars/modelchecker/instantiation/SparseCtmcInstantiationModelChecker.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"

#include "storm-pars/settings/ParsSettings.h"
#include "storm-pars/settings/modules/ParametricSettings.h"
#include "storm-pars/settings/modules/RegionSettings.h"

#include "storm-pars/transformer/SparseParametricMdpSimplifier.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm/api/storm.h"

#include "storm/exceptions/BaseException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/models/ModelBase.h"

#include "storm/settings/SettingsManager.h"

#include "storm/solver/stateelimination/PrioritizedStateEliminator.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/utility/file.h"
#include "storm/utility/initialize.h"
#include "storm/utility/Stopwatch.h"
#include "storm/utility/macros.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/BisimulationSettings.h"


namespace storm {
    namespace pars {
    
        typedef typename storm::cli::SymbolicInput SymbolicInput;

        template <typename ValueType>
        struct SampleInformation {
            SampleInformation(bool graphPreserving = false, bool exact = false) : graphPreserving(graphPreserving), exact(exact) {
                // Intentionally left empty.
            }
            
            bool empty() const {
                return cartesianProducts.empty();
            }
            
            std::vector<std::map<typename utility::parametric::VariableType<ValueType>::type, std::vector<typename utility::parametric::CoefficientType<ValueType>::type>>> cartesianProducts;
            bool graphPreserving;
            bool exact;
        };

        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::shared_ptr<storm::models::ModelBase> const& model) {
            std::vector<storm::storage::ParameterRegion<ValueType>> result;
            auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
            if (regionSettings.isRegionSet()) {
                result = storm::api::parseRegions<ValueType>(regionSettings.getRegionString(), *model);
            }
            return result;
        }
        
        template <typename ValueType>
        SampleInformation<ValueType> parseSamples(std::shared_ptr<storm::models::ModelBase> const& model, std::string const& sampleString, bool graphPreserving) {
            STORM_LOG_THROW(!model || model->isSparseModel(), storm::exceptions::NotSupportedException, "Sampling is only supported for sparse models.");

            SampleInformation<ValueType> sampleInfo(graphPreserving);
            if (sampleString.empty()) {
                return sampleInfo;
            }
            
            // Get all parameters from the model.
            std::set<typename utility::parametric::VariableType<ValueType>::type> modelParameters;
            auto const& sparseModel = *model->as<storm::models::sparse::Model<ValueType>>();
            modelParameters = storm::models::sparse::getProbabilityParameters(sparseModel);
            auto rewParameters = storm::models::sparse::getRewardParameters(sparseModel);
            modelParameters.insert(rewParameters.begin(), rewParameters.end());

            std::vector<std::string> cartesianProducts;
            boost::split(cartesianProducts, sampleString, boost::is_any_of(";"));
            for (auto& product : cartesianProducts) {
                boost::trim(product);
                
                // Get the values string for each variable.
                std::vector<std::string> valuesForVariables;
                boost::split(valuesForVariables, product, boost::is_any_of(","));
                for (auto& values : valuesForVariables) {
                    boost::trim(values);
                }
                
                std::set<typename utility::parametric::VariableType<ValueType>::type> encounteredParameters;
                sampleInfo.cartesianProducts.emplace_back();
                auto& newCartesianProduct = sampleInfo.cartesianProducts.back();
                for (auto const& varValues : valuesForVariables) {
                    auto equalsPosition = varValues.find("=");
                    STORM_LOG_THROW(equalsPosition != varValues.npos, storm::exceptions::WrongFormatException, "Incorrect format of samples.");
                    std::string variableName = varValues.substr(0, equalsPosition);
                    boost::trim(variableName);
                    std::string values = varValues.substr(equalsPosition + 1);
                    boost::trim(values);
                    
                    bool foundParameter = false;
                    typename utility::parametric::VariableType<ValueType>::type theParameter;
                    for (auto const& parameter : modelParameters) {
                        std::stringstream parameterStream;
                        parameterStream << parameter;
                        if (parameterStream.str() == variableName) {
                            foundParameter = true;
                            theParameter = parameter;
                            encounteredParameters.insert(parameter);
                        }
                    }
                    STORM_LOG_THROW(foundParameter, storm::exceptions::WrongFormatException, "Unknown parameter '" << variableName << "'.");
                    
                    std::vector<std::string> splitValues;
                    boost::split(splitValues, values, boost::is_any_of(":"));
                    STORM_LOG_THROW(!splitValues.empty(), storm::exceptions::WrongFormatException, "Expecting at least one value per parameter.");
                    
                    auto& list = newCartesianProduct[theParameter];
                    
                    for (auto& value : splitValues) {
                        boost::trim(value);
                        list.push_back(storm::utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(value));
                    }
                }
                
                STORM_LOG_THROW(encounteredParameters == modelParameters, storm::exceptions::WrongFormatException, "Variables for all parameters are required when providing samples.");
            }
            
            return sampleInfo;
        }
        
        template <typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
            auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
            
            if (result.first->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                result.first = storm::cli::preprocessSparseMarkovAutomaton(result.first->template as<storm::models::sparse::MarkovAutomaton<ValueType>>());
                result.second = true;
            }
            
            if (generalSettings.isBisimulationSet()) {
                result.first = storm::cli::preprocessSparseModelBisimulation(result.first->template as<storm::models::sparse::Model<ValueType>>(), input, bisimulationSettings);
                result.second = true;
            }
            
            if (parametricSettings.transformContinuousModel() && (result.first->isOfType(storm::models::ModelType::Ctmc) || result.first->isOfType(storm::models::ModelType::MarkovAutomaton))) {
                result.first = storm::api::transformContinuousToDiscreteTimeSparseModel(std::move(*result.first->template as<storm::models::sparse::Model<ValueType>>()), storm::api::extractFormulasFromProperties(input.properties));
                result.second = true;
            }
            
            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input) {
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
        
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            if (coreSettings.getEngine() == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                // Currently, hybrid engine for parametric models just referrs to building the model symbolically.
                STORM_LOG_INFO("Translating symbolic model to sparse model...");
                result.first = storm::api::transformSymbolicToSparseModel(model);
                result.second = true;
                // Invoke preprocessing on the sparse model
                auto sparsePreprocessingResult = storm::pars::preprocessSparseModel<ValueType>(result.first->as<storm::models::sparse::Model<ValueType>>(), input);
                if (sparsePreprocessingResult.second) {
                    result.first = sparsePreprocessingResult.first;
                }
            }
            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            storm::utility::Stopwatch preprocessingWatch(true);
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
            if (model->isSparseModel()) {
                result = storm::pars::preprocessSparseModel<ValueType>(result.first->as<storm::models::sparse::Model<ValueType>>(), input);
            } else {
                STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
                result = storm::pars::preprocessDdModel<DdType, ValueType>(result.first->as<storm::models::symbolic::Model<DdType, ValueType>>(), input);
            }
            
            if (result.second) {
                STORM_PRINT_AND_LOG(std::endl << "Time for model preprocessing: " << preprocessingWatch << "." << std::endl << std::endl);
            }
            return result;
        }
        
        template<typename ValueType>
        void printInitialStatesResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::jani::Property const& property, storm::utility::Stopwatch* watch = nullptr, storm::utility::parametric::Valuation<ValueType> const* valuation = nullptr) {
            if (result) {
                STORM_PRINT_AND_LOG("Result (initial states)");
                if (valuation) {
                    bool first = true;
                    std::stringstream ss;
                    for (auto const& entry : *valuation) {
                        if (!first) {
                            ss << ", ";
                        } else {
                            first = false;
                        }
                        ss << entry.first << "=" << entry.second;
                    }
                    
                    STORM_PRINT_AND_LOG(" for instance [" << ss.str() << "]");
                }
                STORM_PRINT_AND_LOG(": ")
                
                auto const* regionCheckResult = dynamic_cast<storm::modelchecker::RegionCheckResult<ValueType> const*>(result.get());
                if (regionCheckResult != nullptr) {
                    auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
                    std::stringstream outStream;
                    if (regionSettings.isPrintFullResultSet()) {
                        regionCheckResult->writeToStream(outStream);
                    } else {
                        regionCheckResult->writeCondensedToStream(outStream);
                    }
                    outStream << std::endl;
                    if (!regionSettings.isPrintNoIllustrationSet()) {
                        auto const* regionRefinementCheckResult = dynamic_cast<storm::modelchecker::RegionRefinementCheckResult<ValueType> const*>(regionCheckResult);
                        if (regionRefinementCheckResult != nullptr) {
                            regionRefinementCheckResult->writeIllustrationToStream(outStream);
                        }
                    }
                    outStream << std::endl;
                    STORM_PRINT_AND_LOG(outStream.str());
                } else {
                    STORM_PRINT_AND_LOG(*result << std::endl);
                }
                if (watch) {
                    STORM_PRINT_AND_LOG("Time for model checking: " << *watch << "." << std::endl << std::endl);
                }
            } else {
                STORM_PRINT_AND_LOG(" failed, property is unsupported by selected engine/settings." << std::endl);
            }
        }
        
        template<typename ValueType>
        void verifyProperties(std::vector<storm::jani::Property> const& properties, std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> const& verificationCallback, std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> const& postprocessingCallback) {
            for (auto const& property : properties) {
                storm::cli::printModelCheckingProperty(property);
                storm::utility::Stopwatch watch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result = verificationCallback(property.getRawFormula());
                watch.stop();
                printInitialStatesResult<ValueType>(result, property, &watch);
                postprocessingCallback(result);
            }
        }
        
        template<template<typename, typename> class ModelCheckerType, typename ModelType, typename ValueType, typename SolveValueType = double>
        void verifyPropertiesAtSamplePoints(ModelType const& model, SymbolicInput const& input, SampleInformation<ValueType> const& samples) {
            
            // When samples are provided, we create an instantiation model checker.
            ModelCheckerType<ModelType, SolveValueType> modelchecker(model);
            
            for (auto const& property : input.properties) {
                storm::cli::printModelCheckingProperty(property);
                
                modelchecker.specifyFormula(storm::api::createTask<ValueType>(property.getRawFormula(), true));
                modelchecker.setInstantiationsAreGraphPreserving(samples.graphPreserving);
                
                storm::utility::parametric::Valuation<ValueType> valuation;
                
                std::vector<typename utility::parametric::VariableType<ValueType>::type> parameters;
                std::vector<typename std::vector<typename utility::parametric::CoefficientType<ValueType>::type>::const_iterator> iterators;
                std::vector<typename std::vector<typename utility::parametric::CoefficientType<ValueType>::type>::const_iterator> iteratorEnds;
                
                storm::utility::Stopwatch watch(true);
                for (auto const& product : samples.cartesianProducts) {
                    parameters.clear();
                    iterators.clear();
                    iteratorEnds.clear();
                    
                    for (auto const& entry : product) {
                        parameters.push_back(entry.first);
                        iterators.push_back(entry.second.cbegin());
                        iteratorEnds.push_back(entry.second.cend());
                    }
                    
                    bool done = false;
                    while (!done) {
                        // Read off valuation.
                        for (uint64_t i = 0; i < parameters.size(); ++i) {
                            valuation[parameters[i]] = *iterators[i];
                        }
                        
                        storm::utility::Stopwatch valuationWatch(true);
                        std::unique_ptr<storm::modelchecker::CheckResult> result = modelchecker.check(Environment(), valuation);
                        valuationWatch.stop();
                        
                        if (result) {
                            result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model.getInitialStates()));
                        }
                        printInitialStatesResult<ValueType>(result, property, &valuationWatch, &valuation);
                        
                        for (uint64_t i = 0; i < parameters.size(); ++i) {
                            ++iterators[i];
                            if (iterators[i] == iteratorEnds[i]) {
                                // Reset iterator and proceed to move next iterator.
                                iterators[i] = product.at(parameters[i]).cbegin();
                                
                                // If the last iterator was removed, we are done.
                                if (i == parameters.size() - 1) {
                                    done = true;
                                }
                            } else {
                                // If an iterator was moved but not reset, we have another valuation to check.
                                break;
                            }
                        }
                        
                    }
                }
                
                watch.stop();
                STORM_PRINT_AND_LOG("Overall time for sampling all instances: " << watch << std::endl << std::endl);
            }
        }
        
        template <typename ValueType, typename SolveValueType = double>
        void verifyPropertiesAtSamplePoints(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, SampleInformation<ValueType> const& samples) {
            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                verifyPropertiesAtSamplePoints<storm::modelchecker::SparseDtmcInstantiationModelChecker, storm::models::sparse::Dtmc<ValueType>, ValueType, SolveValueType>(*model->template as<storm::models::sparse::Dtmc<ValueType>>(), input, samples);
            } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
                verifyPropertiesAtSamplePoints<storm::modelchecker::SparseCtmcInstantiationModelChecker, storm::models::sparse::Ctmc<ValueType>, ValueType, SolveValueType>(*model->template as<storm::models::sparse::Ctmc<ValueType>>(), input, samples);
            } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
                verifyPropertiesAtSamplePoints<storm::modelchecker::SparseMdpInstantiationModelChecker, storm::models::sparse::Mdp<ValueType>, ValueType, SolveValueType>(*model->template as<storm::models::sparse::Mdp<ValueType>>(), input, samples);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Sampling is currently only supported for DTMCs, CTMCs and MDPs.");
            }
        }
        
        template <typename ValueType>
        void verifyPropertiesWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, SampleInformation<ValueType> const& samples) {
            
            if (samples.empty()) {
                verifyProperties<ValueType>(input.properties,
                                            [&model] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                                                std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true));
                                                if (result) {
                                                    result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                                                }
                                                return result;
                                            },
                                            [&model] (std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
                                                auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
                                                if (parametricSettings.exportResultToFile() && model->isOfType(storm::models::ModelType::Dtmc)) {
                                                    auto dtmc = model->template as<storm::models::sparse::Dtmc<ValueType>>();
                                                    boost::optional<ValueType> rationalFunction = result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()];
                                                    storm::api::exportParametricResultToFile(rationalFunction, storm::analysis::ConstraintCollector<ValueType>(*dtmc), parametricSettings.exportResultPath());
                                                }
                                            });
            } else {
                STORM_LOG_TRACE("Sampling the model at given points.");
                
                if (samples.exact) {
                    verifyPropertiesAtSamplePoints<ValueType, storm::RationalNumber>(model, input, samples);
                } else {
                    verifyPropertiesAtSamplePoints<ValueType, double>(model, input, samples);
                }
            }
        }
        
        template <typename ValueType>
        void verifyRegionsWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions) {
            STORM_LOG_ASSERT(!regions.empty(), "Can not analyze an empty set of regions.");
            
            auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            auto regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
            
            std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> verificationCallback;
            std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> postprocessingCallback;
            
            STORM_PRINT_AND_LOG(std::endl);
            if (regionSettings.isHypothesisSet()) {
                STORM_PRINT_AND_LOG("Checking hypothesis " << regionSettings.getHypothesis() << " on ");
            } else {
                STORM_PRINT_AND_LOG("Analyzing ");
            }
            if (regions.size() == 1) {
                STORM_PRINT_AND_LOG("parameter region " << regions.front());
            } else {
                STORM_PRINT_AND_LOG(regions.size() << " parameter regions");
            }
            auto engine = regionSettings.getRegionCheckEngine();
            STORM_PRINT_AND_LOG(" using " << engine);
        
            // Check the given set of regions with or without refinement
            if (regionSettings.isRefineSet()) {
                STORM_LOG_THROW(regions.size() == 1, storm::exceptions::NotSupportedException, "Region refinement is not supported for multiple initial regions.");
                STORM_PRINT_AND_LOG(" with iterative refinement until " << (1.0 - regionSettings.getCoverageThreshold()) * 100.0 << "% is covered." << (regionSettings.isDepthLimitSet() ? " Depth limit is " + std::to_string(regionSettings.getDepthLimit()) + "." : "") << std::endl);
                verificationCallback = [&] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                                        ValueType refinementThreshold = storm::utility::convertNumber<ValueType>(regionSettings.getCoverageThreshold());
                                        boost::optional<uint64_t> optionalDepthLimit;
                                        if (regionSettings.isDepthLimitSet()) {
                                            optionalDepthLimit = regionSettings.getDepthLimit();
                                        }
                                        std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ValueType>> result = storm::api::checkAndRefineRegionWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true), regions.front(), engine, refinementThreshold, optionalDepthLimit, regionSettings.getHypothesis());
                                        return result;
                                    };
            } else {
                STORM_PRINT_AND_LOG("." << std::endl);
                verificationCallback = [&] (std::shared_ptr<storm::logic::Formula const> const& formula) {
                                        std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::checkRegionsWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true), regions, engine, regionSettings.getHypothesis());
                                        return result;
                                    };
            }
            
            postprocessingCallback = [&] (std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
                                        if (parametricSettings.exportResultToFile()) {
                                            storm::api::exportRegionCheckResultToFile<ValueType>(result, parametricSettings.exportResultPath());
                                        }
                                    };
            
            verifyProperties<ValueType>(input.properties, verificationCallback, postprocessingCallback);
        }
        
        template <typename ValueType>
        void verifyWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, SampleInformation<ValueType> const& samples) {
            if (regions.empty()) {
                storm::pars::verifyPropertiesWithSparseEngine(model, input, samples);
            } else {
                storm::pars::verifyRegionsWithSparseEngine(model, input, regions);
            }
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void verifyParametricModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, SampleInformation<ValueType> const& samples) {
            STORM_LOG_ASSERT(model->isSparseModel(), "Unexpected model type.");
            storm::pars::verifyWithSparseEngine<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), input, regions, samples);
        }

        template <typename ValueType>
        std::map<carl::Variable, std::pair<bool, bool>> analyseMonotonicity(storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) {
            //TODO: Seperate cpp file with this and criticalstatefinding/handling
            std::map<carl::Variable, std::pair<bool, bool>> varsMonotone;
            ofstream myfile;
            myfile.open ("mc.dot");
            myfile << "digraph \"MC\" {" << std::endl;
            myfile << "\t" << "node [shape=ellipse]" << std::endl;
            // print all nodes
            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                myfile << "\t\"" << i << "\" [label = \"" << i << "\"]" << std::endl;
            }


            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                // go over all rows
                auto row = matrix.getRow(i);

                auto first = (*row.begin());
                if (first.getValue() != ValueType(1)) {
                    auto second = (*(++row.begin()));
                    string color = "";
                    auto val = first.getValue();
                    auto vars = val.gatherVariables();
                    for (auto itr = vars.begin(); itr != vars.end(); ++itr) {
                        auto derivative = val.derivative(*itr);
                        STORM_LOG_THROW(derivative.isConstant(), storm::exceptions::NotSupportedException, "Expecting derivative to be constant");

                        if (varsMonotone.find(*itr) == varsMonotone.end()) {
                            varsMonotone[*itr].first = true;
                            varsMonotone[*itr].second = true;
                        }

                        auto compare = lattice->compare(first.getColumn(), second.getColumn());
                        std::pair<bool, bool>* value = &varsMonotone.find(*itr)->second;
                        std::pair<bool, bool> old = *value;
                        if (compare == 1) {
                            value->first &=derivative.constantPart() >= 0;
                            value->second &=derivative.constantPart() <= 0;
                        } else if (compare == 2) {
                            value->first &=derivative.constantPart() <= 0;
                            value->second &=derivative.constantPart() >= 0;
                        } else if (compare == 0) {
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Don't know what is happening, something in monotonicity checking went wrong");
                        } else {
                            value->first = false;
                            value->second = false;
                        }
                        if ((value->first != old.first) && (value->second != old.second)) {
                            color = "color = red, ";
                        } else if ((value->first != old.first)) {
                            myfile << "\t edge[style=dashed];" << std::endl;
                            color = "color = blue, ";
                        } else if ((value->second != old.second)) {
                            myfile << "\t edge[style=dotted];" << std::endl;
                            color = "color = blue, ";
                        }
                    }
                    myfile << "\t" << i << " -> " << first.getColumn() << "[" << color << "label=\"" << first.getValue() << "\"];"
                           << std::endl;
                    myfile << "\t" << i << " -> " << second.getColumn() << "[" << color << "label=\"" << second.getValue() << "\"];"
                           << std::endl;
                    myfile << "\t edge[style=\"\"];" << std::endl;
                } else {
                    myfile << "\t" << i << " -> " << first.getColumn() << "[label=\"" << first.getValue() << "\"];"
                           << std::endl;
                }


            }

            myfile << "\tsubgraph legend {" << std::endl;
            myfile << "\t\tnode [color=white];" << std::endl;
            myfile << "\t\tedge [style=invis];" << std::endl;
            myfile << "\t\tt0 [label=\"incr? and decr?\", fontcolor=red];" << std::endl;
            myfile << "\t\tt1 [label=\"incr? (dashed)\", fontcolor=blue];" << std::endl;
            myfile << "\t\tt2 [label=\"decr? (dotted)\", fontcolor=blue];" << std::endl;

            myfile << "\t}" << std::endl;
            myfile << "}" << std::endl;
            myfile.close();
            return varsMonotone;
        };

        template <storm::dd::DdType DdType, typename ValueType>
        void processInputWithValueTypeAndDdlib(SymbolicInput& input) {
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();

            auto buildSettings = storm::settings::getModule<storm::settings::modules::BuildSettings>();
            auto parSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            
            auto engine = coreSettings.getEngine();
            STORM_LOG_THROW(engine == storm::settings::modules::CoreSettings::Engine::Sparse || engine == storm::settings::modules::CoreSettings::Engine::Hybrid || engine == storm::settings::modules::CoreSettings::Engine::Dd, storm::exceptions::InvalidSettingsException, "The selected engine is not supported for parametric models.");
            
            std::shared_ptr<storm::models::ModelBase> model;
            if (!buildSettings.isNoBuildModelSet()) {
                model = storm::cli::buildModel<DdType, ValueType>(engine, input, ioSettings);
            }
            
            if (model) {
                model->printModelInformationToStream(std::cout);
            }
            
            STORM_LOG_THROW(model || input.properties.empty(), storm::exceptions::InvalidSettingsException, "No input model.");

            if (parSettings.isMonotonicityAnalysisSet()) {
                std::cout << "Hello, Jip1" << std::endl;
                // Simplify the model
                storm::utility::Stopwatch simplifyingWatch(true);
                if (model->isOfType(storm::models::ModelType::Dtmc)) {
                    auto consideredModel = (model->as<storm::models::sparse::Dtmc<ValueType>>());
                    auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<ValueType>>(*consideredModel);

                    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(input.properties);
                    STORM_LOG_THROW(formulas.begin()!=formulas.end(), storm::exceptions::NotSupportedException, "Only one formula at the time supported");

                    if (!simplifier.simplify(*(formulas[0]))) {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
                    }
                    model = simplifier.getSimplifiedModel();
                } else if (model->isOfType(storm::models::ModelType::Mdp)) {
                    auto consideredModel = (model->as<storm::models::sparse::Mdp<ValueType>>());
                    auto simplifier = storm::transformer::SparseParametricMdpSimplifier<storm::models::sparse::Mdp<ValueType>>(*consideredModel);

                    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(input.properties);
                    STORM_LOG_THROW(formulas.begin()!=formulas.end(), storm::exceptions::NotSupportedException, "Only one formula at the time supported");

                    if (!simplifier.simplify(*(formulas[0]))) {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
                    }
                    model = simplifier.getSimplifiedModel();
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform monotonicity analysis on the provided model type.");
                }

                simplifyingWatch.stop();
                STORM_PRINT(std::endl << "Time for model simplification: " << simplifyingWatch << "." << std::endl << std::endl);
                model->printModelInformationToStream(std::cout);
                std::cout << "Bye, Jip1" << std::endl;
            }

            if (model) {
                auto preprocessingResult = storm::pars::preprocessModel<DdType, ValueType>(model, input);
                if (preprocessingResult.second) {
                    model = preprocessingResult.first;
                    model->printModelInformationToStream(std::cout);
                }
            }

            if (parSettings.isMonotonicityAnalysisSet()) {
                std::cout << "Hello, Jip2" << std::endl;
                std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = storm::api::extractFormulasFromProperties(input.properties);
                std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->as<storm::models::sparse::Model<ValueType>>();

                // Transform to Lattice
                storm::utility::Stopwatch latticeWatch(true);
                storm::analysis::Lattice* lattice = storm::analysis::Lattice::toLattice<ValueType>(sparseModel, formulas);
                latticeWatch.stop();
                STORM_PRINT(std::endl << "Time for lattice creation: " << latticeWatch << "." << std::endl << std::endl);
                ofstream myfile;
                myfile.open ("lattice.dot");
                lattice->toDotFile(myfile);
                myfile.close();


                // CriticalStates
                //TODO: what if states not found in lattice/cyclic thingie
                storm::storage::SparseMatrix<ValueType> matrix = sparseModel.get()->getTransitionMatrix();
                //TODO: separate class
                storm::utility::Stopwatch criticalStatesWatch(true);
                storm::storage::BitVector criticalStates = storm::storage::BitVector(sparseModel.get()->getNumberOfStates());
                for (uint_fast64_t i = 0; i < sparseModel.get()->getNumberOfStates(); ++i) {

                    auto row = matrix.getRow(i);
                    auto first = (*row.begin());
                    if (first.getValue() != ValueType(1)) {
                        auto second = (*(++row.begin()));
                        string color = "";
                        auto val = first.getValue();
                        auto vars = val.gatherVariables();
                        for (auto itr = vars.begin(); itr != vars.end(); ++itr) {
                            auto compare = lattice->compare(first.getColumn(), second.getColumn());
                            if (compare != 1 && compare != 2 && compare !=0) {

                                if (storm::utility::parameterlifting::validateParameterLiftingSound(*(sparseModel.get()), *((formulas[0]).get()))) {
                                    //TODO: parameterlifting gebruiken om te kijken op kans op =) vanuit first.getCOlumn() en second.getColumn()
                                    // als vanuit first.getColumn() >= second.getColumn() voor =) dan  lattice->addRelation(lattice->getNode(first.getColumn()),
                                    //                                                                     lattice->getNode(i),
                                    //                                                                     lattice->getNode(second.getColumn()));
                                    // als vanuit second.getColumn() >= first.getColumn() voor =) dan  lattice->addRelation(lattice->getNode(second.getColumn()),
                                    //                                                                     lattice->getNode(i),
                                    //                                                                     lattice->getNode(first.getColumn()));


                                    auto modelParameters = storm::models::sparse::getProbabilityParameters(*sparseModel);
                                    auto rewParameters = storm::models::sparse::getRewardParameters(*sparseModel);
                                    modelParameters.insert(rewParameters.begin(), rewParameters.end());

//                                    auto regionChecker = storm::api::initializeParameterLiftingRegionModelChecker<ValueType, storm::RationalNumber>(Environment(), sparseModel, storm::api::createTask<storm::RationalFunction>(formulas[0], true));
//
//                                    //start testing
//                                    auto allSatRegion=storm::api::parseRegion<storm::RationalFunction>("0.7<=pL<=0.9,0.75<=pK<=0.95", modelParameters);
//                                    auto exBothRegion=storm::api::parseRegion<storm::RationalFunction>("0.4<=pL<=0.65,0.75<=pK<=0.95", modelParameters);
//                                    auto allVioRegion=storm::api::parseRegion<storm::RationalFunction>("0.1<=pL<=0.73,0.2<=pK<=0.715", modelParameters);
//                                    if (storm::modelchecker::RegionResult::AllSat == regionChecker->analyzeRegion(Environment(), allSatRegion, storm::modelchecker::RegionResultHypothesis::Unknown, storm::modelchecker::RegionResult::Unknown, true)) {
//                                        std::cout << "Tralalala" << std::endl;
//                                    }
                                }


                                auto rowFirst = matrix.getRow(first.getColumn());
                                while ((*rowFirst.begin()).getValue() == ValueType(1)) {
                                    rowFirst = matrix.getRow((*rowFirst.begin()).getColumn());
                                }

                                auto rowSecond = matrix.getRow(second.getColumn());
                                while ((*rowSecond.begin()).getValue() == ValueType(1)) {
                                    rowSecond = matrix.getRow((*rowSecond.begin()).getColumn());
                                }

                                auto succF1 = *rowFirst.begin();
                                auto succF2 = *(++rowFirst.begin());
                                auto compareF = lattice->compare(succF1.getColumn(), succF2.getColumn());
                                ValueType valF;
                                ValueType valS;
                                if (compareF == 1) {
                                    valF = succF1.getValue();
                                } else if (compareF == 2) {
                                    valF = succF2.getValue();
                                } else {
                                    continue;
                                }

                                auto succS1 = *rowSecond.begin();
                                auto succS2 = *(++rowSecond.begin());
                                auto compareS = lattice->compare(succS1.getColumn(), succS2.getColumn());
                                if (compareS == 1) {
                                    valS = succS1.getValue();
                                } else if (compareS == 2) {
                                    valS = succS2.getValue();
                                } else {
                                    continue;
                                }

                                storm::RationalFunction diff = valF-valS;
                                auto vars = diff.gatherVariables();
                                if (vars.size() == 1) {
                                    for (auto varsItr = vars.begin(); varsItr != vars.end(); ++varsItr) {
                                        ValueType derivative = diff.derivative(*varsItr);
                                        if (derivative.isConstant()) {
                                            std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> sub0;
                                            sub0.emplace(*varsItr,
                                                         storm::utility::convertNumber<storm::RationalFunctionCoefficient>(
                                                                 std::string("0")));
                                            std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> sub1;
                                            sub1.emplace(*varsItr,
                                                         storm::utility::convertNumber<storm::RationalFunctionCoefficient>(
                                                                 std::string("1")));
                                            if (diff.evaluate(sub0) >= 0 && diff.evaluate(sub1) >= 0) {
                                                lattice->addRelation(lattice->getNode(first.getColumn()),
                                                                     lattice->getNode(i),
                                                                     lattice->getNode(second.getColumn()));
                                            } else if (diff.evaluate(sub0) <= 0 && diff.evaluate(sub1) <= 0) {
                                                lattice->addRelation(lattice->getNode(second.getColumn()),
                                                                     lattice->getNode(i),
                                                                     lattice->getNode(first.getColumn()));
                                            }
                                        }
                                    }
                                }
                            }
                        }

                    }
                }

                criticalStatesWatch.stop();
                STORM_PRINT(std::endl << "Time for critical states checking: " << criticalStatesWatch << "." << std::endl << std::endl);
                myfile.open ("lattice.dot");
                lattice->toDotFile(myfile);
                myfile.close();


                // Monotonicity?
                storm::utility::Stopwatch monotonicityWatch(true);
                std::map<carl::Variable, std::pair<bool, bool>> varsMonotone = analyseMonotonicity<ValueType>(lattice, matrix);
                monotonicityWatch.stop();
                STORM_PRINT(std::endl << "Time for monotonicity: " << monotonicityWatch << "." << std::endl << std::endl);

                for (auto itr = varsMonotone.begin(); itr != varsMonotone.end(); ++itr) {
                    if (itr->second.first) {
                        std::cout << "Monotone increasing in: " << itr->first << std::endl;
                    } else {
                        std::cout << "Do not know if monotone increasing in: " << itr->first << std::endl;
                    }
                    if (itr->second.second) {
                        std::cout << "Monotone decreasing in: " << itr->first << std::endl;
                    } else {
                        std::cout << "Do not know if monotone decreasing in: " << itr->first << std::endl;
                    }
                }

                std::cout << "Bye, Jip2" << std::endl;
                return;
            }

            std::vector<storm::storage::ParameterRegion<ValueType>> regions = parseRegions<ValueType>(model);
            std::string samplesAsString = parSettings.getSamples();
            SampleInformation<ValueType> samples;
            if (!samplesAsString.empty()) {
                samples = parseSamples<ValueType>(model, samplesAsString, parSettings.isSamplesAreGraphPreservingSet());
                samples.exact = parSettings.isSampleExactSet();
            }
            
            if (model) {
                storm::cli::exportModel<DdType, ValueType>(model, input);
            }

            if (parSettings.onlyObtainConstraints()) {
                STORM_LOG_THROW(parSettings.exportResultToFile(), storm::exceptions::InvalidSettingsException, "When computing constraints, export path has to be specified.");
                storm::api::exportParametricResultToFile<ValueType>(boost::none, storm::analysis::ConstraintCollector<ValueType>(*(model->as<storm::models::sparse::Model<ValueType>>())), parSettings.exportResultPath());
                return;
            }

            if (model) {
                verifyParametricModel<DdType, ValueType>(model, input, regions, samples);
            }
        }
        
        void processOptions() {
            // Start by setting some urgent options (log levels, resources, etc.)
            storm::cli::setUrgentOptions();
            
            // Parse and preprocess symbolic input (PRISM, JANI, properties, etc.)
            SymbolicInput symbolicInput = storm::cli::parseAndPreprocessSymbolicInput();
            
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            auto engine = coreSettings.getEngine();
            STORM_LOG_WARN_COND(engine != storm::settings::modules::CoreSettings::Engine::Dd || engine != storm::settings::modules::CoreSettings::Engine::Hybrid || coreSettings.getDdLibraryType() == storm::dd::DdType::Sylvan, "The selected DD library does not support parametric models. Switching to Sylvan...");
        
            processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalFunction>(symbolicInput);
        }

    }
}


/*!
 * Main entry point of the executable storm-pars.
 */
int main(const int argc, const char** argv) {

    try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-pars", argc, argv);
        storm::settings::initializeParsSettings("Storm-pars", "storm-pars");

        storm::utility::Stopwatch totalTimer(true);
        if (!storm::cli::parseOptions(argc, argv)) {
            return -1;
        }

        storm::pars::processOptions();

        totalTimer.stop();
        if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
            storm::cli::printTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
        }

        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const& exception) {
        STORM_LOG_ERROR("An exception caused Storm-pars to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const& exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-pars to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
