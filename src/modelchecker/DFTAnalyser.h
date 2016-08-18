#pragma once

#include "src/logic/Formula.h"
#include "src/parser/DFTGalileoParser.h"
#include "src/builder/ExplicitDFTModelBuilder.h"
#include "src/builder/ExplicitDFTModelBuilderApprox.h"
#include "src/modelchecker/results/CheckResult.h"
#include "src/utility/storm.h"
#include "src/storage/dft/DFTIsomorphism.h"
#include "src/settings/modules/DFTSettings.h"
#include "src/utility/bitoperations.h"


#include <chrono>

template<typename ValueType>
class DFTAnalyser {
    
    std::chrono::duration<double> buildingTime = std::chrono::duration<double>::zero();
    std::chrono::duration<double> explorationTime = std::chrono::duration<double>::zero();
    std::chrono::duration<double> bisimulationTime = std::chrono::duration<double>::zero();
    std::chrono::duration<double> modelCheckingTime = std::chrono::duration<double>::zero();
    std::chrono::duration<double> totalTime = std::chrono::duration<double>::zero();
    ValueType checkResult = storm::utility::zero<ValueType>();
public:
    void check(storm::storage::DFT<ValueType> const& origDft , std::shared_ptr<const storm::logic::Formula> const& formula, bool symred = true, bool allowModularisation = true, bool enableDC = true) {
        
        // Building DFT
        std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();
        
        
        // Optimizing DFT
        storm::storage::DFT<ValueType> dft = origDft.optimize();
        checkResult = checkHelper(dft, formula, symred, allowModularisation, enableDC);
        totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            
        
            
    }
    
private:
    ValueType checkHelper(storm::storage::DFT<ValueType> const& dft , std::shared_ptr<const storm::logic::Formula> const& formula, bool symred = true, bool allowModularisation = true, bool enableDC = true)  {
        STORM_LOG_TRACE("Check helper called");
        bool invResults = false;
        std::vector<storm::storage::DFT<ValueType>> dfts = {dft};
        std::vector<ValueType> res;
        size_t nrK = 0; // K out of M 
        size_t nrM = 0; // K out of M
        
        if(allowModularisation) {
            if(dft.topLevelType() == storm::storage::DFTElementType::AND) {
                STORM_LOG_TRACE("top modularisation called AND");
                dfts = dft.topModularisation();
                STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                nrK = dfts.size();
                nrM = dfts.size();
            }
            if(dft.topLevelType() == storm::storage::DFTElementType::OR) {
                STORM_LOG_TRACE("top modularisation called OR");
                dfts = dft.topModularisation();
                STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                nrK = 0;
                nrM = dfts.size();
                invResults = true;
            }
            if(dft.topLevelType() == storm::storage::DFTElementType::VOT) {
                STORM_LOG_TRACE("top modularisation called VOT");
                dfts = dft.topModularisation();
                STORM_LOG_TRACE("Modularsation into " << dfts.size() << " submodules.");
                nrK = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(dft.getTopLevelGate())->threshold();
                nrM = dfts.size();
                if(nrK <= nrM/2) {
                    nrK -= 1;
                    invResults = true;
                }
            }
            if(dfts.size() > 1) {
                STORM_LOG_TRACE("Recursive CHECK Call");
                for(auto const ft : dfts) {
                    res.push_back(checkHelper(ft, formula, symred, allowModularisation));
                }
            }
        } 
        if(res.empty()) {
            // Model based modularisation.
            auto const& models = buildMarkovModels(dfts, formula, symred, enableDC);


            for (auto const& model : models) {
                // Model checking
                STORM_LOG_INFO("Model checking...");
                std::chrono::high_resolution_clock::time_point modelCheckingStart = std::chrono::high_resolution_clock::now();
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula));
                STORM_LOG_INFO("Model checking done.");
                STORM_LOG_ASSERT(result, "Result does not exist.");
                result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                modelCheckingTime += std::chrono::high_resolution_clock::now() -  modelCheckingStart;
                res.push_back(result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second);
            }
        }
        if(nrM <= 1) {
            // No modularisation done.
            STORM_LOG_ASSERT(res.size()==1, "Result size not 1.");
            return res[0];
        }
        
        STORM_LOG_TRACE("Combining all results... K=" << nrK << "; M=" << nrM << "; invResults=" << (invResults?"On":"Off"));
        ValueType result = storm::utility::zero<ValueType>();
        int limK = invResults ? -1 : nrM+1;
        int chK = invResults ? -1 : 1;
        for(int cK = nrK; cK != limK; cK += chK ) {
            STORM_LOG_ASSERT(cK >= 0, "ck negative.");
            size_t permutation = smallestIntWithNBitsSet(static_cast<size_t>(cK));
            do {
                STORM_LOG_TRACE("Permutation="<<permutation);
                ValueType permResult = storm::utility::one<ValueType>();
                for(size_t i = 0; i < res.size(); ++i) {
                    if(permutation & (1 << i)) {
                        permResult *= res[i];
                    } else {
                        permResult *= storm::utility::one<ValueType>() - res[i];
                    }
                }
                STORM_LOG_TRACE("Result for permutation:"<<permResult);
                permutation = nextBitPermutation(permutation);
                result += permResult;
            } while(permutation < (1 << nrM) && permutation != 0);
        }
        if(invResults) {
            return storm::utility::one<ValueType>() - result;
        }
        return result;
    }
    
    
    std::vector<std::shared_ptr<storm::models::sparse::Model<ValueType>>> buildMarkovModels(std::vector<storm::storage::DFT<ValueType>> const&  dfts,   std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool enableDC) {
        std::vector<std::shared_ptr<storm::models::sparse::Model<ValueType>>> models;
        for(auto& dft : dfts) {
            std::chrono::high_resolution_clock::time_point buildingStart = std::chrono::high_resolution_clock::now();

            std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
            storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);
            if(symred) {
                auto colouring = dft.colourDFT();
                symmetries = dft.findSymmetries(colouring);
                STORM_LOG_INFO("Found " << symmetries.groups.size() << " symmetries.");
                STORM_LOG_TRACE("Symmetries: " << std::endl << symmetries);
            }
            std::chrono::high_resolution_clock::time_point buildingEnd = std::chrono::high_resolution_clock::now();
            buildingTime += buildingEnd - buildingStart;
            
            // Building Markov Automaton
            STORM_LOG_INFO("Building Model...");
            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;
            // TODO Matthias: use only one builder if everything works again
            if (storm::settings::getModule<storm::settings::modules::DFTSettings>().computeApproximation()) {
                storm::builder::ExplicitDFTModelBuilderApprox<ValueType> builder(dft, symmetries, enableDC);
                typename storm::builder::ExplicitDFTModelBuilderApprox<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
                model = builder.buildModel(labeloptions);
            } else {
                storm::builder::ExplicitDFTModelBuilder<ValueType> builder(dft, symmetries, enableDC);
                typename storm::builder::ExplicitDFTModelBuilder<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
                model = builder.buildModel(labeloptions);
            }
            //model->printModelInformationToStream(std::cout);
            STORM_LOG_INFO("No. states (Explored): " << model->getNumberOfStates());
            STORM_LOG_INFO("No. transitions (Explored): " << model->getNumberOfTransitions());
            std::chrono::high_resolution_clock::time_point explorationEnd = std::chrono::high_resolution_clock::now();
            explorationTime += explorationEnd -buildingEnd;

            // Bisimulation
            if (model->isOfType(storm::models::ModelType::Ctmc) && storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet()) {
                STORM_LOG_INFO("Bisimulation...");
                model =  storm::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), {formula}, storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();
                //model->printModelInformationToStream(std::cout);
            }
            STORM_LOG_INFO("No. states (Bisimulation): " << model->getNumberOfStates());
            STORM_LOG_INFO("No. transitions (Bisimulation): " << model->getNumberOfTransitions());
            bisimulationTime += std::chrono::high_resolution_clock::now() - explorationEnd;
            models.push_back(model);
        }
        return models;
    }
    
public:
    void printTimings(std::ostream& os = std::cout) {
        os << "Times:" << std::endl;
        os << "Building:\t" << buildingTime.count() << std::endl;
        os << "Exploration:\t" << explorationTime.count() << std::endl;
        os << "Bisimulation:\t" << bisimulationTime.count() << std::endl;
        os << "Modelchecking:\t" << modelCheckingTime.count() << std::endl;
        os << "Total:\t\t" << totalTime.count() << std::endl;
    }
    
    void printResult(std::ostream& os = std::cout) {
        
        os << "Result: [";
        os << checkResult << "]" << std::endl;
    }
    
};
