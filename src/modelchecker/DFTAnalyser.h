#pragma once

#include "logic/Formula.h"
#include "parser/DFTGalileoParser.h"
#include "builder/ExplicitDFTModelBuilder.h"
#include "modelchecker/results/CheckResult.h"
#include "utility/storm.h"
#include "storage/dft/DFTIsomorphism.h"


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
    void check(storm::storage::DFT<ValueType> dft , std::shared_ptr<const storm::logic::Formula> const& formula, bool symred = true, bool allowModularisation = true, bool enableDC = true) {
        
        // Building DFT
        std::chrono::high_resolution_clock::time_point totalStart = std::chrono::high_resolution_clock::now();
        
        
        // Optimizing DFT
        dft = dft.optimize();
        std::vector<storm::storage::DFT<ValueType>> dfts;
        checkResult = checkHelper(dft, formula, symred, allowModularisation, enableDC);
        totalTime = std::chrono::high_resolution_clock::now() - totalStart;
            
        
            
    }
    
private:
    ValueType checkHelper(storm::storage::DFT<ValueType> const& dft , std::shared_ptr<const storm::logic::Formula> const& formula, bool symred = true, bool allowModularisation = true, bool enableDC = true)  {
        std::cout << "check helper called" << std::endl;
        bool invResults = false;
        std::vector<storm::storage::DFT<ValueType>> dfts = {dft};
        std::vector<ValueType> res;
        
        if(allowModularisation) {
            std::cout << dft.topLevelType() << std::endl;
            if(dft.topLevelType() == storm::storage::DFTElementType::AND) {
                std::cout << "top modularisation called AND" << std::endl;
                dfts = dft.topModularisation();
                if(dfts.size() > 1) {
                    for(auto const ft : dfts) {
                        res.push_back(checkHelper(ft, formula, symred, allowModularisation));
                    }
                }
            }
            if(dft.topLevelType() == storm::storage::DFTElementType::OR) {
                std::cout << "top modularisation called OR" << std::endl;
                dfts = dft.topModularisation();
                if(dfts.size() > 1) {
                    for(auto const ft : dfts) {
                        res.push_back(checkHelper(ft, formula, symred, allowModularisation));
                    }
                }
                invResults = true;
            }
        } 
        if(res.empty()) {
            // Model based modularisation.
            auto const& models = buildMarkovModels(dfts, formula, symred, enableDC);


            for (auto const& model : models) {
                // Model checking
                std::cout << "Model checking..." << std::endl;
                std::chrono::high_resolution_clock::time_point modelCheckingStart = std::chrono::high_resolution_clock::now();
                std::unique_ptr<storm::modelchecker::CheckResult> result(storm::verifySparseModel(model, formula));
                std::cout << "done." << std::endl;
                assert(result);
                result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                modelCheckingTime += std::chrono::high_resolution_clock::now() -  modelCheckingStart;
                res.push_back(result->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second);
            }
        }
        
        ValueType result = storm::utility::one<ValueType>();
        for(auto const& r : res) {
            if(invResults) {
                result *= storm::utility::one<ValueType>() - r;
            } else {
                result *= r;
            }
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
                std::cout << "Found " << symmetries.groups.size() << " symmetries." << std::endl;
                STORM_LOG_TRACE("Symmetries: " << std::endl << symmetries);
            }
            std::chrono::high_resolution_clock::time_point buildingEnd = std::chrono::high_resolution_clock::now();
            buildingTime += buildingEnd - buildingStart;
            
            // Building Markov Automaton
            std::cout << "Building Model..." << std::endl;
            storm::builder::ExplicitDFTModelBuilder<ValueType> builder(dft, symmetries, enableDC);
            typename storm::builder::ExplicitDFTModelBuilder<ValueType>::LabelOptions labeloptions; // TODO initialize this with the formula
            std::shared_ptr<storm::models::sparse::Model<ValueType>> model = builder.buildModel(labeloptions);
            //model->printModelInformationToStream(std::cout);
            std::cout << "No. states (Explored): " << model->getNumberOfStates() << std::endl;
            std::cout << "No. transitions (Explored): " << model->getNumberOfTransitions() << std::endl;
            std::chrono::high_resolution_clock::time_point explorationEnd = std::chrono::high_resolution_clock::now();
            explorationTime += explorationEnd -buildingEnd;

            // Bisimulation
            if (model->isOfType(storm::models::ModelType::Ctmc)) {
                std::cout << "Bisimulation..." << std::endl;
                model =  storm::performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), {formula}, storm::storage::BisimulationType::Weak)->template as<storm::models::sparse::Ctmc<ValueType>>();
                //model->printModelInformationToStream(std::cout);
            }
            std::cout << "No. states (Bisimulation): " << model->getNumberOfStates() << std::endl;
            std::cout << "No. transitions (Bisimulation): " << model->getNumberOfTransitions() << std::endl;
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
