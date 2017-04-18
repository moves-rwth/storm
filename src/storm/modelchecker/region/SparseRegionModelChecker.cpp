#include "storm/modelchecker/region/SparseRegionModelChecker.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/modelchecker/region/RegionCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/logic/Formulas.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/RegionSettings.h"
#include "storm/utility/constants.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "modelchecker/results/CheckResult.h"
#include "modelchecker/results/ExplicitQuantitativeCheckResult.h"

namespace storm {
    namespace modelchecker {
        namespace region {

            SparseRegionModelCheckerSettings::SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode const& sampleM,
                                                storm::settings::modules::RegionSettings::ApproxMode const& appM,
                                                storm::settings::modules::RegionSettings::SmtMode    const& smtM) : sampleMode(sampleM), approxMode(appM), smtMode(smtM) {
                // Intentionally left empty
            }

            storm::settings::modules::RegionSettings::ApproxMode SparseRegionModelCheckerSettings::getApproxMode() const {
                return this->approxMode;
            }

            storm::settings::modules::RegionSettings::SampleMode SparseRegionModelCheckerSettings::getSampleMode() const {
                return this->sampleMode;
            }

            storm::settings::modules::RegionSettings::SmtMode SparseRegionModelCheckerSettings::getSmtMode() const {
                return this->smtMode;
            }

            bool SparseRegionModelCheckerSettings::doApprox() const {
                return getApproxMode() != storm::settings::modules::RegionSettings::ApproxMode::OFF;
            }

            bool SparseRegionModelCheckerSettings::doSample() const {
                return getSampleMode() != storm::settings::modules::RegionSettings::SampleMode::OFF;
            }

            bool SparseRegionModelCheckerSettings::doSmt() const {
                return getSmtMode() != storm::settings::modules::RegionSettings::SmtMode::OFF;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::SparseRegionModelChecker(std::shared_ptr<ParametricSparseModelType> model, SparseRegionModelCheckerSettings const& settings) :
                    model(model),
                    specifiedFormula(nullptr),
                    settings(settings) {
                STORM_LOG_THROW(model->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::InvalidArgumentException, "Model is required to have exactly one initial state.");
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::~SparseRegionModelChecker() {
                //Intentionally left empty
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<ParametricSparseModelType> const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getModel() const {
                return this->model;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<storm::logic::OperatorFormula> const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSpecifiedFormula() const {
                return this->specifiedFormula;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSpecifiedFormulaBound() const {
                return this->getSpecifiedFormula()->template getThresholdAs<ConstantType>();
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::specifiedFormulaHasLowerBound() const {
                return storm::logic::isLowerBound(this->getSpecifiedFormula()->getComparisonType());
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            bool const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::isComputeRewards() const {
                return computeRewards;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::isResultConstant() const {
                return this->constantResult.operator bool();
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<ParametricSparseModelType> const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSimpleModel() const {
                return this->simpleModel;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<storm::logic::OperatorFormula> const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSimpleFormula() const {
                return this->simpleFormula;
            }

//            template<typename ParametricSparseModelType, typename ConstantType>
//            SparseRegionModelCheckerSettings& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSettings() {
//                return this->settings;
//            };

            template<typename ParametricSparseModelType, typename ConstantType>
            SparseRegionModelCheckerSettings const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSettings() const {
                return this->settings;
            }




            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::specifyFormula(std::shared_ptr<const storm::logic::Formula> formula) {
                std::chrono::high_resolution_clock::time_point timeSpecifyFormulaStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_DEBUG("Specifying the formula " << *formula.get());
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
                this->timeComputeReachabilityFunction=std::chrono::high_resolution_clock::duration::zero();

                
                std::chrono::high_resolution_clock::time_point timePreprocessingStart = std::chrono::high_resolution_clock::now();
                this->preprocess(this->simpleModel, this->simpleFormula, isApproximationApplicable, constantResult);
                std::chrono::high_resolution_clock::time_point timePreprocessingEnd = std::chrono::high_resolution_clock::now();
                
                //TODO: Currently we are not able to detect functions of the form p*q correctly as these functions are not linear but approximation is still applicable.
                //This is just a quick fix to work with such models anyway.
                if(!this->isApproximationApplicable){
                    STORM_LOG_ERROR("There are non-linear functions that occur in the given model. Approximation is still correct for functions that are linear w.r.t. a single parameter (assuming the remaining parameters are constants), e.g., p*q is okay. Currently, the implementation is not able to validate this..");
                    this->isApproximationApplicable=true;
                }
                
                //Check if the approximation and the sampling model needs to be computed
                if(!this->isResultConstant()){
                    if(this->isApproximationApplicable && settings.doApprox()){
                        initializeApproximationModel(*this->getSimpleModel(), this->getSimpleFormula());
                    }
                    if(settings.getSampleMode()==storm::settings::modules::RegionSettings::SampleMode::INSTANTIATE ||
                            (!settings.doSample() && settings.getApproxMode()==storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST)){
                        initializeSamplingModel(*this->getSimpleModel(), this->getSimpleFormula());
                    }
                } else if (this->isResultConstant() && this->constantResult.get() == storm::utility::convertNumber<ConstantType>(-1.0)){
                    //In this case, the result is constant but has not been computed yet. so do it now!
                    STORM_LOG_DEBUG("The Result is constant and will be computed now.");
                    initializeSamplingModel(*this->getSimpleModel(), this->getSimpleFormula());
                    std::map<VariableType, CoefficientType> emptySubstitution;
                    this->constantResult = this->getSamplingModel()->computeInitialStateValue(emptySubstitution);
                }

                //some more information for statistics...
                std::chrono::high_resolution_clock::time_point timeSpecifyFormulaEnd = std::chrono::high_resolution_clock::now();
                this->timeSpecifyFormula= timeSpecifyFormulaEnd - timeSpecifyFormulaStart;
                this->timePreprocessing = timePreprocessingEnd - timePreprocessingStart;  
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::initializeApproximationModel(ParametricSparseModelType const& model, std::shared_ptr<storm::logic::OperatorFormula> formula) {
                std::chrono::high_resolution_clock::time_point timeInitApproxModelStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_DEBUG("Initializing the Approximation Model...");
                STORM_LOG_THROW(this->isApproximationApplicable, storm::exceptions::UnexpectedException, "Approximation model requested but approximation is not applicable");
                this->approximationModel=std::make_shared<ApproximationModel<ParametricSparseModelType, ConstantType>>(model, formula);
                std::chrono::high_resolution_clock::time_point timeInitApproxModelEnd = std::chrono::high_resolution_clock::now();
                this->timeInitApproxModel=timeInitApproxModelEnd - timeInitApproxModelStart;
                STORM_LOG_DEBUG("Initialized Approximation Model");
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::initializeSamplingModel(ParametricSparseModelType const& model, std::shared_ptr<storm::logic::OperatorFormula> formula) {
                STORM_LOG_DEBUG("Initializing the Sampling Model....");
                std::chrono::high_resolution_clock::time_point timeInitSamplingModelStart = std::chrono::high_resolution_clock::now();
                this->samplingModel=std::make_shared<SamplingModel<ParametricSparseModelType, ConstantType>>(model, formula);
                std::chrono::high_resolution_clock::time_point timeInitSamplingModelEnd = std::chrono::high_resolution_clock::now();
                this->timeInitSamplingModel = timeInitSamplingModelEnd - timeInitSamplingModelStart;
                STORM_LOG_DEBUG("Initialized Sampling Model");
            }
            
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkRegions(std::vector<ParameterRegion<ParametricType>>& regions) {
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
            void SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::refineAndCheckRegion(std::vector<ParameterRegion<ParametricType>>& regions, double const& refinementThreshold) {
                STORM_LOG_DEBUG("Applying refinement on region: " << regions.front().toString() << ".");
                std::cout << "Applying refinement on region: " << regions.front().toString() << std::endl;
                std::cout.flush();
                CoefficientType areaOfParameterSpace = regions.front().area();
                uint_fast64_t indexOfCurrentRegion=0;
                CoefficientType fractionOfUndiscoveredArea = storm::utility::one<CoefficientType>();
                CoefficientType fractionOfAllSatArea = storm::utility::zero<CoefficientType>();
                CoefficientType fractionOfAllViolatedArea = storm::utility::zero<CoefficientType>();
                while(fractionOfUndiscoveredArea > storm::utility::convertNumber<CoefficientType>(refinementThreshold)){
                    STORM_LOG_THROW(indexOfCurrentRegion < regions.size(), storm::exceptions::InvalidStateException, "Threshold for undiscovered area not reached but no unprocessed regions left.");
                    ParameterRegion<ParametricType>& currentRegion = regions[indexOfCurrentRegion];
                    this->checkRegion(currentRegion);
                    switch(currentRegion.getCheckResult()){
                        case RegionCheckResult::ALLSAT:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllSatArea += currentRegion.area() / areaOfParameterSpace;
                            break;
                        case RegionCheckResult::ALLVIOLATED:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllViolatedArea += currentRegion.area() / areaOfParameterSpace;
                            break;
                        default:
                            std::vector<ParameterRegion<ParametricType>> newRegions;
                            currentRegion.split(currentRegion.getCenterPoint(), newRegions);
                            regions.insert(regions.end(), newRegions.begin(), newRegions.end());
                            break;
                    }
                    ++indexOfCurrentRegion;
                }
                std::cout << " done! " << std::endl << "Fraction of ALLSAT;ALLVIOLATED;UNDISCOVERED area:" << std::endl;
                std::cout << "REFINEMENTRESULT;" <<storm::utility::convertNumber<double>(fractionOfAllSatArea) << ";" << storm::utility::convertNumber<double>(fractionOfAllViolatedArea) << ";" << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) << std::endl;
                
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkRegion(ParameterRegion<ParametricType>& region) {
                std::chrono::high_resolution_clock::time_point timeCheckRegionStart = std::chrono::high_resolution_clock::now();
                ++this->numOfCheckedRegions;

                STORM_LOG_THROW(this->getSpecifiedFormula()!=nullptr, storm::exceptions::InvalidStateException, "Tried to analyze a region although no property has been specified" );
                STORM_LOG_DEBUG("Analyzing the region " << region.toString());

                //switches for the different steps.
                bool done=false;
                STORM_LOG_WARN_COND( (!settings.doApprox() || this->isApproximationApplicable), "the approximation is only correct if the model has only linear functions (more precisely: linear in a single parameter, i.e., functions like p*q are okay). As this is not the case, approximation is deactivated");
                bool doApproximation=settings.doApprox() && this->isApproximationApplicable;
                bool doSampling=settings.doSample();
                bool doSmt=settings.doSmt();

                if(this->isResultConstant()){
                    STORM_LOG_DEBUG("Checking a region although the result is constant, i.e., independent of the region. This makes sense none.");
                    if(this->checkFormulaOnSamplingPoint(region.getSomePoint())){
                        region.setCheckResult(RegionCheckResult::ALLSAT);
                    }
                    else{
                        region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                    }
                    done=true;
                }

                std::chrono::high_resolution_clock::time_point timeApproximationStart = std::chrono::high_resolution_clock::now();
                if(!done && doApproximation){
                    STORM_LOG_DEBUG("Checking approximative values...");
                    if(this->checkApproximativeValues(region)){
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
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkApproximativeValues(ParameterRegion<ParametricType>& region) {
                // Decide whether to prove allsat or allviolated. 
                bool proveAllSat;
                switch (region.getCheckResult()){
                    case RegionCheckResult::UNKNOWN: 
                        switch(this->settings.getApproxMode()){
                            case storm::settings::modules::RegionSettings::ApproxMode::TESTFIRST:
                                //Sample a single point to know whether we should try to prove ALLSAT or ALLVIOLATED
                                checkPoint(region,region.getSomePoint(), false);
                                proveAllSat= (region.getCheckResult()==RegionCheckResult::EXISTSSAT);
                                break;
                            case storm::settings::modules::RegionSettings::ApproxMode::GUESSALLSAT:
                                proveAllSat=true;
                                break;
                            case storm::settings::modules::RegionSettings::ApproxMode::GUESSALLVIOLATED:
                                proveAllSat=false;
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The specified approxmode is not supported");
                        }
                        break;
                    case RegionCheckResult::ALLSAT:
                         STORM_LOG_WARN("The checkresult of the current region should not be conclusive (ALLSAT)");
                         //Intentionally no break;
                    case RegionCheckResult::EXISTSSAT:
                        proveAllSat=true;
                        break;
                    case RegionCheckResult::ALLVIOLATED:
                         STORM_LOG_WARN("The checkresult of the current region should not be conclusive (ALLViolated)");
                         //Intentionally no break;
                    case RegionCheckResult::EXISTSVIOLATED:
                        proveAllSat=false;
                        break;
                    default:
                         STORM_LOG_WARN("The checkresult of the current region should not be conclusive, i.e. it should be either EXISTSSAT or EXISTSVIOLATED or UNKNOWN in order to apply approximative values");
                         proveAllSat=true;
                }

                if(this->checkRegionWithApproximation(region, proveAllSat)){
                    //approximation was conclusive
                    if(proveAllSat){
                        region.setCheckResult(RegionCheckResult::ALLSAT);
                    } else {
                        region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                    }
                    return true;
                }

                if(region.getCheckResult()==RegionCheckResult::UNKNOWN){
                    //In this case, it makes sense to try to prove the contrary statement
                    proveAllSat=!proveAllSat;
                    if(this->checkRegionWithApproximation(region, proveAllSat)){
                        //approximation was conclusive
                        if(proveAllSat){
                            region.setCheckResult(RegionCheckResult::ALLSAT);
                        } else {
                            region.setCheckResult(RegionCheckResult::ALLVIOLATED);
                        }
                        return true;
                    }
                }
                //if we reach this point than the result is still inconclusive.
                return false;            
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkRegionWithApproximation(ParameterRegion<ParametricType> const& region, bool proveAllSat){
                if(this->isResultConstant()){
                    return (proveAllSat==this->checkFormulaOnSamplingPoint(region.getSomePoint()));
                }
                bool computeLowerBounds = (this->specifiedFormulaHasLowerBound() && proveAllSat) || (!this->specifiedFormulaHasLowerBound() && !proveAllSat);
                bool formulaSatisfied = this->getApproximationModel()->checkFormulaOnRegion(region, computeLowerBounds);
                return (proveAllSat==formulaSatisfied);
            }
                
            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<ApproximationModel<ParametricSparseModelType, ConstantType>> const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getApproximationModel() {
                if(this->approximationModel==nullptr){
                    STORM_LOG_WARN("Approximation model requested but it has not been initialized when specifying the formula. Will initialize it now.");
                    initializeApproximationModel(*this->getSimpleModel(), this->getSimpleFormula());
                }
                return this->approximationModel;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkSamplePoints(ParameterRegion<ParametricType>& region) {
                auto samplingPoints = region.getVerticesOfRegion(region.getVariables()); //test the 4 corner points
                for (auto const& point : samplingPoints){
                    if(checkPoint(region, point)){
                        return true;
                    }            
                }
                return false;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getReachabilityValue(std::map<VariableType, CoefficientType> const& point) {
                if(this->isResultConstant()){
                    return this->constantResult.get();
                }
                return this->getSamplingModel()->computeInitialStateValue(point);
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::checkFormulaOnSamplingPoint(std::map<VariableType, CoefficientType> const& point) {
                if(this->isResultConstant()){
                    return this->valueIsInBoundOfFormula(this->constantResult.get());
                }
                return this->getSamplingModel()->checkFormulaOnSamplingPoint(point);
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            std::shared_ptr<SamplingModel<ParametricSparseModelType, ConstantType>> const& SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::getSamplingModel() {
                if(this->samplingModel==nullptr){
                    STORM_LOG_WARN("Sampling model requested but it has not been initialized when specifying the formula. Will initialize it now.");
                    initializeSamplingModel(*this->getSimpleModel(), this->getSimpleFormula());
                }
                return this->samplingModel;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::valueIsInBoundOfFormula(ConstantType const& value){
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
            bool SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::valueIsInBoundOfFormula(CoefficientType const& value){
                return valueIsInBoundOfFormula(storm::utility::convertNumber<ConstantType>(value));
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SparseRegionModelChecker<ParametricSparseModelType, ConstantType>::printStatisticsToStream(std::ostream& outstream) {
                STORM_LOG_DEBUG("Printing statistics");

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
                std::chrono::milliseconds timeSmtInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(this->timeSmt);

                std::chrono::high_resolution_clock::duration timeOverall = timeSpecifyFormula + timeCheckRegion; // + ...
                std::chrono::milliseconds timeOverallInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeOverall);

                uint_fast64_t numOfSolvedRegions= this->numOfRegionsExistsBoth + this->numOfRegionsAllSat + this->numOfRegionsAllViolated;

                outstream << std::endl << "Region Model Checker Statistics:" << std::endl;
                outstream << "-----------------------------------------------" << std::endl;
                outstream << "Model: " << this->model->getNumberOfStates() << " states, " << this->model->getNumberOfTransitions() << " transitions." << std::endl;
                outstream << "Formula: " << *this->getSpecifiedFormula() << std::endl;
                if(this->isResultConstant()){
                    outstream << "The requested value is constant (i.e. independent of any parameters)" << std::endl;
                }
                else{
                    outstream << "Simple model: " << this->getSimpleModel()->getNumberOfStates() << " states, " << this->getSimpleModel()->getNumberOfTransitions() << " transitions" << std::endl;
                    outstream << "Simple formula: " << *this->getSimpleFormula() << std::endl;
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
                outstream << "    " << timeApproximationInMilliseconds.count() << "ms Approximation" << std::endl;
                outstream << "    " << timeSammplingInMilliseconds.count() << "ms Sampling " << std::endl;
                outstream << "    " << timeSmtInMilliseconds.count() << "ms Smt solving" << std::endl;
                outstream << "-----------------------------------------------" << std::endl;

                outstream << "CSV format;" << timeOverallInMilliseconds.count() << ";" << this->numOfRegionsAllSat << ";" << this->numOfRegionsAllViolated << ";" << this->numOfRegionsExistsBoth << ";" << (this->numOfCheckedRegions-numOfSolvedRegions) << std::endl;
            }
        
        
        //note: for other template instantiations, add rules for the typedefs of VariableType and CoefficientType in utility/regions.h
#ifdef STORM_HAVE_CARL
        template class SparseRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
        template class SparseRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
#endif
        } // namespace region
    } //namespace modelchecker
} //namespace storm

