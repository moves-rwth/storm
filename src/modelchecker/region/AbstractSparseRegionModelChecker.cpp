/* 
 * File:   AbstractSparseRegionModelChecker.cpp
 * Author: tim
 * 
 * Created on September 9, 2015, 12:34 PM
 */

#include "src/modelchecker/region/AbstractSparseRegionModelChecker.h"

#include "src/adapters/CarlAdapter.h"
#include "src/modelchecker/region/RegionCheckResult.h"
#include "src/logic/Formulas.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/RegionSettings.h"
#include "src/utility/constants.h"
#include "src/utility/graph.h"
#include "src/utility/macros.h"

#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/UnexpectedException.h"
#include "utility/ConversionHelper.h"

namespace storm {
    namespace modelchecker {
        namespace region {
                                
            template<typename ParametricSparseModelType, typename ConstantType>
            AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::AbstractSparseRegionModelChecker(ParametricSparseModelType const& model) : 
                    model(model),
                    specifiedFormula(nullptr){
                STORM_LOG_THROW(model.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Model is required to have exactly one initial state.");
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::~AbstractSparseRegionModelChecker() {
                //Intentionally left empty
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            ParametricSparseModelType const& AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getModel() const {
                return this->model;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<storm::logic::OperatorFormula> const& AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSpecifiedFormula() const {
                return specifiedFormula;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSpecifiedFormulaBound() const {
                return storm::utility::region::convertNumber<ConstantType>(this->getSpecifiedFormula()->getBound());
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::specifiedFormulaHasUpperBound() const {
                return !storm::logic::isLowerBound(this->getSpecifiedFormula()->getComparisonType());
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool const& AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::isComputeRewards() const {
                return computeRewards;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool const AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::isResultConstant() const {
                return this->constantResult.operator bool();
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<ParametricSparseModelType> const& AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSimpleModel() const {
                return this->simpleModel;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<storm::logic::OperatorFormula> const& AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSimpleFormula() const {
                return this->simpleFormula;
            }

            

            
            template<typename ParametricSparseModelType, typename ConstantType>
            void AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::specifyFormula(std::shared_ptr<storm::logic::Formula> formula) {
                std::chrono::high_resolution_clock::time_point timeSpecifyFormulaStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_THROW(this->canHandle(*formula), storm::exceptions::InvalidArgumentException, "Tried to specify a formula that can not be handled.");
                //Initialize the context for this formula
                if (formula->isProbabilityOperatorFormula()) {
                    this->specifiedFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(formula->asProbabilityOperatorFormula());
                    this->computeRewards = false;
                }
                else if (formula->isRewardOperatorFormula()) {
                    this->specifiedFormula =  std::make_shared<storm::logic::RewardOperatorFormula>(formula->asRewardOperatorFormula());
                    this->computeRewards=true;
                }
                else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "The specified property " << this->getSpecifiedFormula() << "is not supported");
                }
                this->constantResult = boost::none;
                this->simpleFormula = nullptr;
                this->isApproximationApplicable = false;
                this->approximationModel = nullptr;
                this->samplingModel = nullptr;
                //stuff for statistics:
                this->numOfCheckedRegions=0;
                this->numOfRegionsSolvedThroughSampling=0;
                this->numOfRegionsSolvedThroughApproximation=0;
                this->numOfRegionsSolvedThroughSmt=0;
                this->numOfRegionsExistsBoth=0;
                this->numOfRegionsAllSat=0;
                this->numOfRegionsAllViolated=0;
                this->timeCheckRegion=std::chrono::high_resolution_clock::duration::zero();
                this->timeSampling=std::chrono::high_resolution_clock::duration::zero();
                this->timeApproximation=std::chrono::high_resolution_clock::duration::zero();
                this->timeSmt=std::chrono::high_resolution_clock::duration::zero();
                this->timeApproxModelInstantiation=std::chrono::high_resolution_clock::duration::zero();
                this->timeComputeReachabilityFunction=std::chrono::high_resolution_clock::duration::zero();
                this->timeApproxModelInstantiation=std::chrono::high_resolution_clock::duration::zero();

                
                std::chrono::high_resolution_clock::time_point timePreprocessingStart = std::chrono::high_resolution_clock::now();
                this->preprocess(this->simpleModel, this->simpleFormula, isApproximationApplicable, constantResult);
                std::chrono::high_resolution_clock::time_point timePreprocessingEnd = std::chrono::high_resolution_clock::now();
                //Check if the approximation and the sampling model needs to be computed
                if(!this->isResultConstant()){
                    if(this->isApproximationApplicable && storm::settings::regionSettings().doApprox()){
                        initializeApproximationModel(*this->getSimpleModel(), this->getSimpleFormula());
                    }
                    if(storm::settings::regionSettings().getSampleMode()==storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE ||
                            (!storm::settings::regionSettings().doSample() && storm::settings::regionSettings().getApproxMode()==storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST)){
                        initializeSamplingModel(*this->getSimpleModel(), this->getSimpleFormula());
                    }
                } else if (this->isResultConstant() && this->constantResult.get() == storm::utility::region::convertNumber<ConstantType>(-1.0)){
                    //In this case, the result is constant but has not been computed yet. so do it now!
                    initializeSamplingModel(*this->getSimpleModel(), this->getSimpleFormula());
                    std::map<VariableType, CoefficientType> emptySubstitution;
                    this->getSamplingModel()->instantiate(emptySubstitution);
                    this->constantResult = this->getSamplingModel()->computeValues()[*this->getSamplingModel()->getModel()->getInitialStates().begin()];
                }

                //some more information for statistics...
                std::chrono::high_resolution_clock::time_point timeSpecifyFormulaEnd = std::chrono::high_resolution_clock::now();
                this->timeSpecifyFormula= timeSpecifyFormulaEnd - timeSpecifyFormulaStart;
                this->timePreprocessing = timePreprocessingEnd - timePreprocessingStart;  
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::initializeApproximationModel(ParametricSparseModelType const& model, std::shared_ptr<storm::logic::OperatorFormula> formula) {
                std::chrono::high_resolution_clock::time_point timeInitApproxModelStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_THROW(this->isApproximationApplicable, storm::exceptions::UnexpectedException, "Approximation model requested but approximation is not applicable");
                this->approximationModel=std::make_shared<ApproximationModel<ParametricSparseModelType, ConstantType>>(model, formula);
                std::chrono::high_resolution_clock::time_point timeInitApproxModelEnd = std::chrono::high_resolution_clock::now();
                this->timeInitApproxModel=timeInitApproxModelEnd - timeInitApproxModelStart;
                STORM_LOG_DEBUG("Initialized Approximation Model");
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::initializeSamplingModel(ParametricSparseModelType const& model, std::shared_ptr<storm::logic::OperatorFormula> formula) {
                std::chrono::high_resolution_clock::time_point timeInitSamplingModelStart = std::chrono::high_resolution_clock::now();
                this->samplingModel=std::make_shared<SamplingModel<ParametricSparseModelType, ConstantType>>(model, formula);
                std::chrono::high_resolution_clock::time_point timeInitSamplingModelEnd = std::chrono::high_resolution_clock::now();
                this->timeInitSamplingModel = timeInitSamplingModelEnd - timeInitSamplingModelStart;
                STORM_LOG_DEBUG("Initialized Sampling Model");
            }
            
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkRegions(std::vector<ParameterRegion<ParametricType>>& regions) {
                STORM_LOG_DEBUG("Checking " << regions.size() << "regions.");
                std::cout << "Checking " << regions.size() << " regions. Progress: ";
                std::cout.flush();

                uint_fast64_t progress=0;
                uint_fast64_t checkedRegions=0;
                for(auto& region : regions){
                    this->checkRegion(region);
                    if((checkedRegions++)*10/regions.size()==progress){
                        std::cout << progress++;
                        std::cout.flush();
                    }
                }
                std::cout << " done!" << std::endl;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkRegion(ParameterRegion<ParametricType>& region) {
                std::chrono::high_resolution_clock::time_point timeCheckRegionStart = std::chrono::high_resolution_clock::now();
                ++this->numOfCheckedRegions;

                STORM_LOG_THROW(this->getSpecifiedFormula()!=nullptr, storm::exceptions::InvalidStateException, "Tried to analyze a region although no property has been specified" );
                STORM_LOG_DEBUG("Analyzing the region " << region.toString());
                //std::cout << "Analyzing the region " << region.toString() << std::endl;

                //switches for the different steps.
                bool done=false;
                STORM_LOG_WARN_COND( (!storm::settings::regionSettings().doApprox() || this->isApproximationApplicable), "the approximation is only correct if the model has only linear functions. As this is not the case, approximation is deactivated");
                bool doApproximation=storm::settings::regionSettings().doApprox() && this->isApproximationApplicable;
                bool doSampling=storm::settings::regionSettings().doSample();
                bool doSmt=storm::settings::regionSettings().doSmt();

                if(!done && this->isResultConstant()){
                    STORM_LOG_DEBUG("Checking a region although the result is constant, i.e., independent of the region. This makes sense none.");
                    if(this->valueIsInBoundOfFormula(this->getReachabilityValue(region.getSomePoint()))){
                        region.setCheckResult(RegionCheckResult::ALLSAT);
                    }
                    else{
                        region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                    }
                    done=true;
                }

                std::chrono::high_resolution_clock::time_point timeApproximationStart = std::chrono::high_resolution_clock::now();
                std::vector<ConstantType> lowerBounds;
                std::vector<ConstantType> upperBounds;
                if(!done && doApproximation){
                    STORM_LOG_DEBUG("Checking approximative values...");
                    if(this->checkApproximativeValues(region, lowerBounds, upperBounds)){
                        ++this->numOfRegionsSolvedThroughApproximation;
                        STORM_LOG_DEBUG("Result '" << region.getCheckResult() <<"' obtained through approximation.");
                        done=true;
                    }
                }
                std::chrono::high_resolution_clock::time_point timeApproximationEnd = std::chrono::high_resolution_clock::now();

                std::chrono::high_resolution_clock::time_point timeSamplingStart = std::chrono::high_resolution_clock::now();
                if(!done && doSampling){
                    STORM_LOG_DEBUG("Checking sample points...");
                    if(this->checkSamplePoints(region)){
                        ++this->numOfRegionsSolvedThroughSampling;
                        STORM_LOG_DEBUG("Result '" << region.getCheckResult() <<"' obtained through sampling.");
                        done=true;
                    }
                }
                std::chrono::high_resolution_clock::time_point timeSamplingEnd = std::chrono::high_resolution_clock::now();

                std::chrono::high_resolution_clock::time_point timeSmtStart = std::chrono::high_resolution_clock::now();
                if(!done && doSmt){
                    STORM_LOG_DEBUG("Checking with Smt Solving...");
                    if(this->checkSmt(region)){
                        ++this->numOfRegionsSolvedThroughSmt;
                        STORM_LOG_DEBUG("Result '" << region.getCheckResult() <<"' obtained through Smt Solving.");
                        done=true;
                    }
                }
                std::chrono::high_resolution_clock::time_point timeSmtEnd = std::chrono::high_resolution_clock::now();

                //some information for statistics...
                std::chrono::high_resolution_clock::time_point timeCheckRegionEnd = std::chrono::high_resolution_clock::now();
                this->timeCheckRegion += timeCheckRegionEnd-timeCheckRegionStart;
                this->timeSampling += timeSamplingEnd - timeSamplingStart;
                this->timeApproximation += timeApproximationEnd - timeApproximationStart;
                this->timeSmt += timeSmtEnd - timeSmtStart;
                switch(region.getCheckResult()){
                    case RegionCheckResult::EXISTSBOTH:
                        ++this->numOfRegionsExistsBoth;
                        break;
                    case RegionCheckResult::ALLSAT:
                        ++this->numOfRegionsAllSat;
                        break;
                    case RegionCheckResult::ALLVIOLATED:
                        ++this->numOfRegionsAllViolated;
                        break;
                    default:
                        break;
                }
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<ApproximationModel<ParametricSparseModelType, ConstantType>> const& AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getApproximationModel() {
                if(this->approximationModel==nullptr){
                    STORM_LOG_WARN("Approximation model requested but it has not been initialized when specifying the formula. Will initialize it now.");
                    initializeApproximationModel(*this->getSimpleModel(), this->getSimpleFormula());
                }
                return this->approximationModel;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkSamplePoints(ParameterRegion<ParametricType>& region) {
                auto samplingPoints = region.getVerticesOfRegion(region.getVariables()); //test the 4 corner points
                for (auto const& point : samplingPoints){
                    if(checkPoint(region, point)){
                        return true;
                    }            
                }
                return false;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getReachabilityValue(std::map<VariableType, CoefficientType> const& point) {
                if(this->isResultConstant()){
                    return this->constantResult.get();
                }
                this->getSamplingModel()->instantiate(point);
                return this->getSamplingModel()->computeValues()[*this->getSamplingModel()->getModel()->getInitialStates().begin()];
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<SamplingModel<ParametricSparseModelType, ConstantType>> const& AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSamplingModel() {
                if(this->samplingModel==nullptr){
                    STORM_LOG_WARN("Sampling model requested but it has not been initialized when specifying the formula. Will initialize it now.");
                    initializeSamplingModel(*this->getSimpleModel(), this->getSimpleFormula());
                }
                return this->samplingModel;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::valueIsInBoundOfFormula(ConstantType const& value){
                STORM_LOG_THROW(this->getSpecifiedFormula()!=nullptr, storm::exceptions::InvalidStateException, "Tried to compare a value to the bound of a formula, but no formula specified.");
                switch (this->getSpecifiedFormula()->getComparisonType()) {
                    case storm::logic::ComparisonType::Greater:
                        return (value > this->getSpecifiedFormulaBound());
                    case storm::logic::ComparisonType::GreaterEqual:
                        return (value >= this->getSpecifiedFormulaBound());
                    case storm::logic::ComparisonType::Less:
                        return (value < this->getSpecifiedFormulaBound());
                    case storm::logic::ComparisonType::LessEqual:
                        return (value <= this->getSpecifiedFormulaBound());
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "the comparison relation of the formula is not supported");
                }
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::valueIsInBoundOfFormula(CoefficientType const& value){
                return valueIsInBoundOfFormula(storm::utility::region::convertNumber<ConstantType>(value));
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void AbstractSparseRegionModelChecker<ParametricSparseModelType, ConstantType>::printStatisticsToStream(std::ostream& outstream) {

                if(this->getSpecifiedFormula()==nullptr){
                    outstream << "Region Model Checker Statistics Error: No formula specified." << std::endl; 
                    return;
                }

                std::chrono::milliseconds timeSpecifyFormulaInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSpecifyFormula);
                std::chrono::milliseconds timePreprocessingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timePreprocessing);
                std::chrono::milliseconds timeInitSamplingModelInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeInitSamplingModel);
                std::chrono::milliseconds timeInitApproxModelInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeInitApproxModel);
                std::chrono::milliseconds timeComputeReachabilityFunctionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeComputeReachabilityFunction);
                std::chrono::milliseconds timeCheckRegionInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeCheckRegion);
                std::chrono::milliseconds timeSammplingInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSampling);
                std::chrono::milliseconds timeApproximationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeApproximation);
                std::chrono::milliseconds timeApproxModelInstantiationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeApproxModelInstantiation);
                std::chrono::milliseconds timeSmtInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSmt);

                std::chrono::high_resolution_clock::duration timeOverall = timeSpecifyFormula + timeCheckRegion; // + ...
                std::chrono::milliseconds timeOverallInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeOverall);

                uint_fast64_t numOfSolvedRegions= this->numOfRegionsExistsBoth + this->numOfRegionsAllSat + this->numOfRegionsAllViolated;

                outstream << std::endl << "Region Model Checker Statistics:" << std::endl;
                outstream << "-----------------------------------------------" << std::endl;
                outstream << "Model: " << this->model.getNumberOfStates() << " states, " << this->model.getNumberOfTransitions() << " transitions." << std::endl;
                outstream << "Formula: " << *this->getSpecifiedFormula() << std::endl;
                if(this->isResultConstant()){
                    outstream << "The requested value is constant (i.e. independent of any parameters)" << std::endl;
                }
                else{
                    outstream << "Simple model: " << this->getSimpleModel()->getNumberOfStates() << " states, " << this->getSimpleModel()->getNumberOfTransitions() << " transitions" << std::endl;
                }
                outstream << "Approximation is " << (this->isApproximationApplicable ? "" : "not ") << "applicable" << std::endl;
                outstream << "Number of checked regions: " << this->numOfCheckedRegions << std::endl;
                if(this->numOfCheckedRegions>0){
                    outstream << "  Number of solved regions:  " <<  numOfSolvedRegions << "(" << numOfSolvedRegions*100/this->numOfCheckedRegions << "%)" <<  std::endl;
                    outstream << "    AllSat:      " <<  this->numOfRegionsAllSat << "(" << this->numOfRegionsAllSat*100/this->numOfCheckedRegions << "%)" <<  std::endl;
                    outstream << "    AllViolated: " <<  this->numOfRegionsAllViolated << "(" << this->numOfRegionsAllViolated*100/this->numOfCheckedRegions << "%)" <<  std::endl;
                    outstream << "    ExistsBoth:  " <<  this->numOfRegionsExistsBoth << "(" << this->numOfRegionsExistsBoth*100/this->numOfCheckedRegions << "%)" <<  std::endl;
                    outstream << "    Unsolved:    " <<  this->numOfCheckedRegions - numOfSolvedRegions << "(" << (this->numOfCheckedRegions - numOfSolvedRegions)*100/this->numOfCheckedRegions << "%)" <<  std::endl;
                    outstream << "  --  Note: %-numbers are relative to the NUMBER of regions, not the size of their area --" <<  std::endl;
                    outstream << "  " << this->numOfRegionsSolvedThroughApproximation << " regions solved through Approximation" << std::endl;
                    outstream << "  " << this->numOfRegionsSolvedThroughSampling << " regions solved through Sampling" << std::endl;
                    outstream << "  " << this->numOfRegionsSolvedThroughSmt << " regions solved through Smt" << std::endl;
                    outstream << std::endl;
                }
                outstream << "Running times:" << std::endl;
                outstream << "  " << timeOverallInMilliseconds.count() << "ms overall (excluding model parsing, bisimulation (if applied))" << std::endl;
                outstream << "  " << timeSpecifyFormulaInMilliseconds.count() << "ms Initialization for the specified formula, including... " << std::endl;
                outstream << "    " << timePreprocessingInMilliseconds.count() << "ms for Preprocessing (mainly: state elimination of const transitions), including" << std::endl;
                outstream << "      " << timeComputeReachabilityFunctionInMilliseconds.count() << "ms to compute the reachability function" << std::endl;
                outstream << "    " << timeInitApproxModelInMilliseconds.count() << "ms to initialize the Approximation Model" << std::endl;
                outstream << "    " << timeInitSamplingModelInMilliseconds.count() << "ms to initialize the Sampling Model" << std::endl;
                outstream << "  " << timeCheckRegionInMilliseconds.count() << "ms Region Check including... " << std::endl;
                outstream << "    " << timeApproximationInMilliseconds.count() << "ms Approximation including... " << std::endl;
                outstream << "      " << timeApproxModelInstantiationInMilliseconds.count() << "ms for instantiation of the approximation model" << std::endl;
                outstream << "    " << timeSammplingInMilliseconds.count() << "ms Sampling " << std::endl;
                outstream << "    " << timeSmtInMilliseconds.count() << "ms Smt solving" << std::endl;
                outstream << "-----------------------------------------------" << std::endl;

            }
        
        
        //note: for other template instantiations, add rules for the typedefs of VariableType and CoefficientType in utility/regions.h
#ifdef STORM_HAVE_CARL
        template class AbstractSparseRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
        template class AbstractSparseRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
#endif
        } // namespace region
    } //namespace modelchecker
} //namespace storm

