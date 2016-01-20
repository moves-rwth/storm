
#ifndef STORM_H
#define	STORM_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <memory>

#include "initialize.h"

#include "storm-config.h"



// Headers that provide auxiliary functionality.
#include "src/settings/SettingsManager.h"


#include "src/settings/modules/BisimulationSettings.h"
#include "src/settings/modules/ParametricSettings.h"
#include "src/settings/modules/RegionSettings.h"

// Formula headers.
#include "src/logic/Formulas.h"

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

// Headers of builders.
#include "src/builder/ExplicitPrismModelBuilder.h"
#include "src/builder/DdPrismModelBuilder.h"

// Headers for model processing.
#include "src/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "src/storage/bisimulation/NondeterministicModelBisimulationDecomposition.h"
#include "src/storage/ModelProgramPair.h"

// Headers for model checking.
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "src/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "src/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "src/modelchecker/region/SparseDtmcRegionModelChecker.h"
#include "src/modelchecker/region/SparseMdpRegionModelChecker.h"
#include "src/modelchecker/region/ParameterRegion.h"
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

    template<typename ValueType>
    std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile = boost::optional<std::string>(), boost::optional<std::string> const& transitionRewardsFile = boost::optional<std::string>(), boost::optional<std::string> const& choiceLabelingFile = boost::optional<std::string>()) {
        return storm::parser::AutoParser<>::parseModel(transitionsFile, labelingFile, stateRewardsFile ? stateRewardsFile.get() : "", transitionRewardsFile ? transitionRewardsFile.get() : "", choiceLabelingFile ? choiceLabelingFile.get() : "" );
    }
            
    storm::prism::Program parseProgram(std::string const& path);
    std::vector<std::shared_ptr<storm::logic::Formula>> parseFormulasForExplicit(std::string const& inputString);
    std::vector<std::shared_ptr<storm::logic::Formula>> parseFormulasForProgram(std::string const& inputString, storm::prism::Program const& program);
            
    template<typename ValueType, storm::dd::DdType LibraryType = storm::dd::DdType::CUDD>
    storm::storage::ModelProgramPair buildSymbolicModel(storm::prism::Program const& program, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
        storm::storage::ModelProgramPair result;

        storm::settings::modules::GeneralSettings settings = storm::settings::generalSettings();

        // Get the string that assigns values to the unknown currently undefined constants in the model.
        std::string constants = settings.getConstantDefinitionString();

        // Customize and perform model-building.
        if (settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Sparse) {
            typename storm::builder::ExplicitPrismModelBuilder<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>::Options options;
            options = typename storm::builder::ExplicitPrismModelBuilder<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>::Options(formulas);
            options.addConstantDefinitionsFromString(program, constants);

            // Generate command labels if we are going to build a counterexample later.
            if (storm::settings::counterexampleGeneratorSettings().isMinimalCommandSetGenerationSet()) {
                options.buildCommandLabels = true;
            }

            storm::builder::ExplicitPrismModelBuilder<ValueType> builder;
            result.model = builder.translateProgram(program, options);
            result.program = builder.getTranslatedProgram();
        } else if (settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Dd || settings.getEngine() == storm::settings::modules::GeneralSettings::Engine::Hybrid) {
            typename storm::builder::DdPrismModelBuilder<LibraryType>::Options options;
            options = typename storm::builder::DdPrismModelBuilder<LibraryType>::Options(formulas);
            options.addConstantDefinitionsFromString(program, constants);

            storm::builder::DdPrismModelBuilder<LibraryType> builder;
            result.model = builder.translateProgram(program, options);
            result.program = builder.getTranslatedProgram();
        }

        return result;
    }
    
    template<typename ModelType>
    std::shared_ptr<ModelType> performDeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
        std::cout << "Performing bisimulation minimization... ";
        typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
        if (!formulas.empty()) {
            options = typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
        }
        if (storm::settings::bisimulationSettings().isWeakBisimulationSet()) {
            options.type = storm::storage::BisimulationType::Weak;
            options.bounded = false;
        }
        
        storm::storage::DeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
        bisimulationDecomposition.computeBisimulationDecomposition();
        model = bisimulationDecomposition.getQuotient();
        std::cout << "done." << std::endl << std::endl;
        return model;
    }
    
    template<typename ModelType>
    std::shared_ptr<ModelType> performNondeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
        std::cout << "Performing bisimulation minimization... ";
        typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
        if (!formulas.empty()) {
            options = typename storm::storage::NondeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
        }
        if (storm::settings::bisimulationSettings().isWeakBisimulationSet()) {
            options.type = storm::storage::BisimulationType::Weak;
            options.bounded = false;
        }
        
        storm::storage::NondeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
        bisimulationDecomposition.computeBisimulationDecomposition();
        model = bisimulationDecomposition.getQuotient();
        std::cout << "done." << std::endl << std::endl;
        return model;
    }
    
    template<typename ModelType, typename ValueType = typename ModelType::ValueType, typename std::enable_if<std::is_base_of<storm::models::sparse::Model<ValueType>, ModelType>::value, bool>::type = 0>
    std::shared_ptr<storm::models::ModelBase> preprocessModel(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
        if (storm::settings::generalSettings().isBisimulationSet()) {
            std::cout << "Model before preprocessing: " << std::endl;
            model->printModelInformationToStream(std::cout);
            STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for sparse models.");
            STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc) || model->isOfType(storm::models::ModelType::Ctmc) || model->isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for DTMCs, CTMCs and MDPs.");

            model->reduceToStateBasedRewards();

            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Dtmc<ValueType>>(model->template as<storm::models::sparse::Dtmc<ValueType>>(), formulas);
            } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
                return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), formulas);
            } else {
                return performNondeterministicSparseBisimulationMinimization<storm::models::sparse::Mdp<ValueType>>(model->template as<storm::models::sparse::Mdp<ValueType>>(), formulas);
            }
            
        }
        return model;
    }
    
    template<typename ModelType, storm::dd::DdType DdType = ModelType::DdType, typename std::enable_if< std::is_base_of< storm::models::symbolic::Model<DdType>, ModelType >::value, bool>::type = 0>
    std::shared_ptr<storm::models::ModelBase> preprocessModel(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula>> const& formulas) {
        // No preprocessing available yet.
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

    template<typename ValueType, storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifyModel(std::shared_ptr<storm::models::ModelBase> model, std::shared_ptr<storm::logic::Formula> const& formula) {
        storm::settings::modules::GeneralSettings const& settings = storm::settings::generalSettings();
        switch(settings.getEngine()) {
            case storm::settings::modules::GeneralSettings::Engine::Sparse: {
                std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->template as<storm::models::sparse::Model<ValueType>>();
                STORM_LOG_THROW(sparseModel != nullptr, storm::exceptions::InvalidArgumentException, "Sparse engine requires a sparse input model");
                return verifySparseModel(sparseModel, formula);
            }
            case storm::settings::modules::GeneralSettings::Engine::Hybrid: {
                std::shared_ptr<storm::models::symbolic::Model<DdType>> ddModel = model->template as<storm::models::symbolic::Model<DdType>>();
                STORM_LOG_THROW(ddModel != nullptr, storm::exceptions::InvalidArgumentException, "Hybrid engine requires a dd input model");
                return verifySymbolicModelWithHybridEngine(ddModel, formula);
            }
            case storm::settings::modules::GeneralSettings::Engine::Dd: {
                std::shared_ptr<storm::models::symbolic::Model<DdType>> ddModel = model->template as<storm::models::symbolic::Model<DdType>>();
                STORM_LOG_THROW(ddModel != nullptr, storm::exceptions::InvalidArgumentException, "Dd engine requires a dd input model");
                return verifySymbolicModelWithDdEngine(ddModel, formula);
            }
        }
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::shared_ptr<storm::logic::Formula> const& formula) {

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
        return result;

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
    inline std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> dtmc = model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

        storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>> modelchecker(*dtmc);
        if (modelchecker.canHandle(*formula)) {
            result = modelchecker.check(*formula);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The parametric engine currently does not support this property.");
        }
        return result;
    }
    
    /*!
     * Initializes a region model checker.
     * 
     * @param regionModelChecker the resulting model checker object
     * @param programFilePath a path to the prism program file
     * @param formulaString The considered formula (as path to the file or directly as string.) Should be exactly one formula.
     * @param constantsString can be used to specify constants for certain parameters, e.g., "p=0.9,R=42"
     * @return true when initialization was successful
     */
    inline bool initializeRegionModelChecker(std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::models::sparse::Model<storm::RationalFunction>, double>>& regionModelChecker,
                                      std::string const& programFilePath,
                                      std::string const& formulaString,
                                      std::string const& constantsString=""){
        regionModelChecker.reset();
        // Program and formula
        storm::prism::Program program = parseProgram(programFilePath);
        program.checkValidity();
        std::vector<std::shared_ptr<storm::logic::Formula>> formulas = parseFormulasForProgram(formulaString, program);
        if(formulas.size()!=1){
            STORM_LOG_ERROR("The given formulaString does not specify exactly one formula");
            return false;
        }
        // Parametric model
        typename storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options options = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>::Options(*formulas[0]);
        options.addConstantDefinitionsFromString(program, constantsString); 
        options.preserveFormula(*formulas[0]);
        std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = storm::builder::ExplicitPrismModelBuilder<storm::RationalFunction>().translateProgram(program, options)->as<storm::models::sparse::Model<storm::RationalFunction>>();
        preprocessModel(model,formulas);
        // ModelChecker
        if(model->isOfType(storm::models::ModelType::Dtmc)){
            regionModelChecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Model<storm::RationalFunction>, double>>(model);
        } else if (model->isOfType(storm::models::ModelType::Mdp)){
            regionModelChecker = std::make_shared<storm::modelchecker::region::SparseMdpRegionModelChecker<storm::models::sparse::Model<storm::RationalFunction>, double>>(model);
        } else {
            STORM_LOG_ERROR("The type of the given model is not supported (only Dtmcs or Mdps are supported");
            return false;
        }
        // Specify the formula
        if(!regionModelChecker->canHandle(*formulas[0])){
            STORM_LOG_ERROR("The given formula is not supported.");
            return false;
        }
        regionModelChecker->specifyFormula(formulas[0]);
        return true;
    }
    
    /*!
     * Computes the reachability value at the given point by instantiating the model.
     * 
     * @param regionModelChecker the model checker object that is to be used
     * @param point the valuation of the different variables
     * @return true iff the specified formula is satisfied (i.e., iff the reachability value is within the bound of the formula)
     */
    inline bool checkSamplingPoint(std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::models::sparse::Model<storm::RationalFunction>, double>> regionModelChecker,
                                   std::map<storm::Variable, storm::RationalNumber> const& point){
        return regionModelChecker->valueIsInBoundOfFormula(regionModelChecker->getReachabilityValue(point));
    }
    
    /*!
     * Does an approximation of the reachability value for all parameters in the given region.
     * @param regionModelChecker the model checker object that is to be used
     * @param lowerBoundaries maps every variable to its lowest possible value within the region. (corresponds to the bottom left corner point in the 2D case)
     * @param upperBoundaries maps every variable to its highest possible value within the region. (corresponds to the top right corner point in the 2D case)
     * @param proveAllSat if set to true, it is checked whether the property is satisfied for all parameters in the given region. Otherwise, it is checked
     *                    whether the property is violated for all parameters.
     * @return true iff the objective (given by the proveAllSat flag) was accomplished.
     * 
     * So there are the following cases:
     * proveAllSat=true,  return=true  ==> the property is SATISFIED for all parameters in the given region
     * proveAllSat=true,  return=false ==> the approximative value is NOT within the bound of the formula (either the approximation is too bad or there are points in the region that violate the property)
     * proveAllSat=false, return=true  ==> the property is VIOLATED for all parameters in the given region
     * proveAllSat=false, return=false ==> the approximative value IS within the bound of the formula (either the approximation is too bad or there are points in the region that satisfy the property)
     */
    inline bool checkRegionApproximation(std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::models::sparse::Model<storm::RationalFunction>, double>> regionModelChecker,
                                         std::map<storm::Variable, storm::RationalNumber> const& lowerBoundaries,
                                         std::map<storm::Variable, storm::RationalNumber> const& upperBoundaries,
                                         bool proveAllSat){
        storm::modelchecker::region::ParameterRegion<storm::RationalFunction> region(lowerBoundaries, upperBoundaries);
        return regionModelChecker->checkRegionWithApproximation(region, proveAllSat);
    }
    
            
#endif

    template<storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySymbolicModelWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
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
        return result;
    }


    template<storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySymbolicModelWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::shared_ptr<storm::logic::Formula> const& formula) {
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
