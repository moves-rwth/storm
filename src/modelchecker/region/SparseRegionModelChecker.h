#pragma once

#include <ostream>
#include <boost/optional.hpp>

#include "src/utility/region.h"
#include "src/modelchecker/region/AbstractSparseRegionModelChecker.h"
#include "src/modelchecker/region/ParameterRegion.h"
#include "src/modelchecker/region/ApproximationModel.h"
#include "src/modelchecker/region/SamplingModel.h"

#include "src/models/sparse/StandardRewardModel.h"
#include "src/models/sparse/Model.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/logic/Formulas.h"

#include "src/settings/modules/RegionSettings.h"

namespace storm {
    namespace modelchecker{
        namespace region{

            class SparseRegionModelCheckerSettings {
            public:
                SparseRegionModelCheckerSettings(storm::settings::modules::RegionSettings::SampleMode const& sampleM,
                                                 storm::settings::modules::RegionSettings::ApproxMode const& appM,
                                                 storm::settings::modules::RegionSettings::SmtMode    const& smtM);

                storm::settings::modules::RegionSettings::SampleMode getSampleMode() const;
                storm::settings::modules::RegionSettings::ApproxMode getApproxMode() const;
                storm::settings::modules::RegionSettings::SmtMode    getSmtMode() const;

                bool doApprox() const;
                bool doSmt() const;
                bool doSample() const;
            private:
                storm::settings::modules::RegionSettings::SampleMode sampleMode;
                storm::settings::modules::RegionSettings::ApproxMode approxMode;
                storm::settings::modules::RegionSettings::SmtMode    smtMode;
            };

            template<typename ParametricSparseModelType, typename ConstantType>
            class SparseRegionModelChecker : public AbstractSparseRegionModelChecker<typename ParametricSparseModelType::ValueType, ConstantType> {
            public:

                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;

                SparseRegionModelChecker(std::shared_ptr<ParametricSparseModelType> model, SparseRegionModelCheckerSettings const& settings);

                virtual ~SparseRegionModelChecker();
                
                /*!
                 * Checks if the given formula can be handled by This region model checker
                 * @param formula the formula to be checked
                 */
                virtual bool canHandle(storm::logic::Formula const& formula) const = 0;
                
                /*!
                 * Specifies the considered formula.
                 * A few preprocessing steps are performed.
                 * If another formula has been specified before, all 'context' regarding the old formula is lost.
                 * 
                 * @param formula the formula to be considered.
                 */
                void specifyFormula(std::shared_ptr<const storm::logic::Formula> formula);
                
                /*!
                 * Checks for every given region whether the specified formula holds for all parameters that lie in that region.
                 * Sets the region checkresult accordingly.
                 * TODO: set region.satpoint and violated point correctly.
                 * 
                 * @note A formula has to be specified first.
                 * 
                 * @param region The considered region
                 */
                void checkRegions(std::vector<ParameterRegion<ParametricType>>& regions);
                
                /*!
                 * Refines a given region and checks whether the specified formula holds for all parameters in the subregion.
                 * The procedure stops as soon as the fraction of the area of regions where the result is neither "ALLSAT" nor "ALLVIOLATED" is less then the given threshold.
                 * 
                 * It is required that the given vector of regions contains exactly one region (the parameter space). All the analyzed regions are appended to that vector.
                 * 
                 * @note A formula has to be specified first.
                 * 
                 * @param regions The considered region
                 * @param refinementThreshold The considered threshold.
                 */
                void refineAndCheckRegion(std::vector<ParameterRegion<ParametricType>>& regions, double const& refinementThreshold);
                
                /*!
                 * Checks whether the given formula holds for all parameters that lie in the given region.
                 * Sets the region checkresult accordingly.
                 * 
                 * @note A formula has to be specified first.
                 * 
                 * @param region The considered region
                 * 
                 */
                void checkRegion(ParameterRegion<ParametricType>& region);
                
                /*!
                 * Returns the reachability Value at the specified point by instantiating and checking the sampling model. 
                 * 
                 * @param point The point (i.e. parameter evaluation) at which to compute the reachability value.
                 */
                ConstantType getReachabilityValue(std::map<VariableType, CoefficientType>const& point);
                
                /*!
                 * Computes the reachability Value at the specified point by instantiating and checking the sampling model. 
                 * @param point The point (i.e. parameter evaluation) at which to compute the reachability value.
                 * @return true iff the specified formula is satisfied
                 */
                bool checkFormulaOnSamplingPoint(std::map<VariableType, CoefficientType>const& point);
                
                /*!
                 * Computes the approximative Value for the given region by instantiating and checking the approximation model. 
                 * returns true iff the provided formula is satisfied w.r.t. the approximative value
                 * 
                 * @param region The region for which to compute the approximative value
                 * @param proveAllSat if set to true, it is checked whether the property is satisfied for all parameters in the given region. Otherwise, it is checked
                          whether the property is violated for all parameters.
                 * @return true iff the objective (given by the proveAllSat flag) was accomplished.
                 */
                bool checkRegionWithApproximation(ParameterRegion<ParametricType> const& region, bool proveAllSat);
                
                /*!
                 * Returns true iff the given value satisfies the bound given by the specified property
                 */
                bool valueIsInBoundOfFormula(ConstantType const& value);
                
                /*!
                 * Returns true iff the given value satisfies the bound given by the specified property
                 */
                bool valueIsInBoundOfFormula(CoefficientType const& value);
                
                /*!
                 * Prints statistical information to the given stream.
                 */
                void printStatisticsToStream(std::ostream& outstream);
                
                /*!
                 * Returns the model that has been specified upon initialization of this
                 */
                std::shared_ptr<ParametricSparseModelType> const& getModel() const;
                
                /*!
                 * Returns the formula that has been specified upon initialization of this
                 */
                std::shared_ptr<storm::logic::OperatorFormula> const& getSpecifiedFormula() const;

                //SparseRegionModelCheckerSettings& getSettings();
                SparseRegionModelCheckerSettings const& getSettings() const;

            protected:
                
                /*!
                 * some trivial getters
                 */
                ConstantType getSpecifiedFormulaBound() const;
                bool specifiedFormulaHasLowerBound() const;
                bool const& isComputeRewards() const;
                bool const isResultConstant() const;
                std::shared_ptr<ParametricSparseModelType> const& getSimpleModel() const;
                std::shared_ptr<storm::logic::OperatorFormula> const& getSimpleFormula() const;
                
                /*!
                 * Makes the required preprocessing steps for the specified model and formula
                 * Computes a simplified version of the model and formula that can be analyzed more efficiently.
                 * Also checks whether the approximation technique is applicable and whether the result is constant.
                 * In the latter case, the result is already computed and set to the given parameter. (otherwise the parameter is not touched).
                 * @note this->specifiedFormula and this->computeRewards has to be set accordingly, before calling this function
                 */
                virtual void preprocess(std::shared_ptr<ParametricSparseModelType>& simpleModel, std::shared_ptr<storm::logic::OperatorFormula>& simpleFormula, bool& isApproximationApplicable, boost::optional<ConstantType>& constantResult) = 0;
                
                /*!
                 * Instantiates the approximation model to compute bounds on the maximal/minimal reachability probability (or reachability reward).
                 * If the current region result is EXISTSSAT (or EXISTSVIOLATED), then this function tries to prove ALLSAT (or ALLVIOLATED).
                 * If this succeeded, then the region check result is changed accordingly.
                 * If the current region result is UNKNOWN, then this function first tries to prove ALLSAT and if that failed, it tries to prove ALLVIOLATED.
                 * In any case, the computed bounds are written to the given lowerBounds/upperBounds.
                 * However, if only the lowerBounds (or upperBounds) have been computed, the other vector is set to a vector of size 0.
                 * True is returned iff either ALLSAT or ALLVIOLATED could be proved.
                 */
                bool checkApproximativeValues(ParameterRegion<ParametricType>& region); 

                /*!
                 * Returns the approximation model.
                 * If it is not yet available, it is computed.
                 */
                std::shared_ptr<ApproximationModel<ParametricSparseModelType, ConstantType>> const& getApproximationModel();
                
                /*!
                 * Checks the value of the function at some sampling points within the given region.
                 * May set the satPoint and violatedPoint of the regions if they are not yet specified and such points are found
                 * Also changes the regioncheckresult of the region to EXISTSSAT, EXISTSVIOLATED, or EXISTSBOTH
                 * 
                 * @return true if an violated point as well as a sat point has been found during the process
                 */
                bool checkSamplePoints(ParameterRegion<ParametricType>& region);
                
                /*!
                 * Checks the value of the function at the given sampling point.
                 * May set the satPoint and violatedPoint of the regions if thy are not yet specified and such point is given.
                 * Also changes the regioncheckresult of the region to EXISTSSAT, EXISTSVIOLATED, or EXISTSBOTH
                 * 
                 * @param favorViaFunction If sampling has been turned off in the settings and a computation via evaluating
                 *                         the reachability function is possible, this flag decides whether to instantiate the
                 *                         sampling model or evaluate the function.
                 * @return true if an violated point as well as a sat point has been found, i.e., the check result is changed to EXISTSOTH
                 */
                virtual bool checkPoint(ParameterRegion<ParametricType>& region, std::map<VariableType, CoefficientType>const& point, bool favorViaFunction=false) = 0;
                
                /*!
                 * Returns the sampling model.
                 * If it is not yet available, it is computed.
                 */
                std::shared_ptr<SamplingModel<ParametricSparseModelType, ConstantType>> const& getSamplingModel();
                
                /*!
                 * Starts the SMTSolver to get the result.
                 * The current regioncheckresult of the region should be EXISTSSAT or EXISTVIOLATED.
                 * Otherwise, a sampingPoint will be computed.
                 * True is returned iff the solver was successful (i.e., it returned sat or unsat)
                 * A Sat- or Violated point is set, if the solver has found one (not yet implemented!).
                 * The region checkResult of the given region is changed accordingly.
                 */
                virtual bool checkSmt(ParameterRegion<ParametricType>& region)=0; 
              
            private:
                /*!
                 * initializes the Approximation Model
                 * 
                 * @note does not check whether approximation can be applied
                 */
                void initializeApproximationModel(ParametricSparseModelType const& model, std::shared_ptr<storm::logic::OperatorFormula> formula);

                /*!
                 * initializes the Sampling Model
                 */
                void initializeSamplingModel(ParametricSparseModelType const& model, std::shared_ptr<storm::logic::OperatorFormula> formula);
                
                // The model this model checker is supposed to analyze.
                std::shared_ptr<ParametricSparseModelType> model;
                //The currently specified formula
                std::shared_ptr<storm::logic::OperatorFormula> specifiedFormula;
                //A flag that is true iff we are interested in rewards
                bool computeRewards;
                // the original model after states with constant transitions have been eliminated
                std::shared_ptr<ParametricSparseModelType> simpleModel;
                // a formula that can be checked on the simplified model
                std::shared_ptr<storm::logic::OperatorFormula> simpleFormula;
                // a flag that is true if approximation is applicable, i.e., there are only linear functions at transitions of the model
                bool isApproximationApplicable;
                // the model that  is used to approximate the reachability values
                std::shared_ptr<ApproximationModel<ParametricSparseModelType, ConstantType>> approximationModel;
                // the model that can be instantiated to check the value at a certain point
                std::shared_ptr<SamplingModel<ParametricSparseModelType, ConstantType>> samplingModel;
                // a flag that is true iff the resulting reachability function is constant
                boost::optional<ConstantType> constantResult;

                SparseRegionModelCheckerSettings settings;


                
                // runtimes and other information for statistics. 
                uint_fast64_t numOfCheckedRegions;
                uint_fast64_t numOfRegionsSolvedThroughApproximation;
                uint_fast64_t numOfRegionsSolvedThroughSampling;
                uint_fast64_t numOfRegionsSolvedThroughSmt;
                uint_fast64_t numOfRegionsExistsBoth;
                uint_fast64_t numOfRegionsAllSat;
                uint_fast64_t numOfRegionsAllViolated;

                std::chrono::high_resolution_clock::duration timeSpecifyFormula;
                std::chrono::high_resolution_clock::duration timePreprocessing;
                std::chrono::high_resolution_clock::duration timeInitApproxModel;
                std::chrono::high_resolution_clock::duration timeInitSamplingModel;
                std::chrono::high_resolution_clock::duration timeCheckRegion;
                std::chrono::high_resolution_clock::duration timeSampling;
                std::chrono::high_resolution_clock::duration timeApproximation;
                std::chrono::high_resolution_clock::duration timeSmt;    
            protected:
                std::chrono::high_resolution_clock::duration timeComputeReachabilityFunction;
            };
            
        } //namespace region
    } //namespace modelchecker
} //namespace storm
