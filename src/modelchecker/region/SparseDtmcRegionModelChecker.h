#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_

#include "src/storage/sparse/StateType.h"
#include "src/models/sparse/Dtmc.h"
#include "src/utility/constants.h"
#include "src/utility/regions.h"
#include "src/solver/Smt2SmtSolver.h"
#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker {
        public:
            
            typedef typename storm::utility::regions::VariableType<ParametricType> VariableType;
            typedef typename storm::utility::regions::CoefficientType<ParametricType> CoefficientType;
            
            /*!
             * The possible results for a single Parameter region
             */
            enum class RegionCheckResult { 
                UNKNOWN, /*!< the result is unknown */
                EXISTSSAT, /*!< the formula is satisfied for at least one parameter evaluation that lies in the given region */
                EXISTSVIOLATED, /*!< the formula is violated for at least one parameter evaluation that lies in the given region */
                EXISTSBOTH, /*!< the formula is satisfied for some parameters but also violated for others */
                ALLSAT, /*!< the formula is satisfied for all parameters in the given region */
                ALLVIOLATED /*!< the formula is violated for all parameters in the given region */
            };
            
            class ParameterRegion;
            class ApproximationModel;
            class SamplingModel;
            
            explicit SparseDtmcRegionModelChecker(storm::models::sparse::Dtmc<ParametricType> const& model);

            /*!
             * Checks if the given formula can be handled by This region model checker
             * @param formula the formula to be checked
             */
            bool canHandle(storm::logic::Formula const& formula) const;
            
            /*!
             * Specifies the considered formula.
             * A few preprocessing steps are performed.
             * If another formula has been specified before, all 'context' regarding the old formula is lost.
             * 
             * @param formula the formula to be considered.
             */
            void specifyFormula(storm::logic::Formula const& formula);

            /*!
             * Checks whether the given formula holds for all parameters that lie in the given region.
             * Sets the region checkresult accordingly. Moreover, region.satPoint and/or an region.violatedPoint will be set.
             * 
             * @note A formula has to be specified first.
             * 
             * @param region The considered region
             * 
             */
            void checkRegion(ParameterRegion& region);
            
            /*!
             * Checks for every given region whether the specified formula holds for all parameters that lie in that region.
             * Sets the region checkresult accordingly. Moreover, region.satPoint and/or an region.violatedPoint will be set.
             * 
             * @note A formula has to be specified first.
             * 
             * @param region The considered region
             */
            void checkRegions(std::vector<ParameterRegion>& regions);
            
            /*!
             * Prints statistical information to the given stream.
             */
            void printStatisticsToStream(std::ostream& outstream);
            
            
            
        private:
            
            /*!
             * Computes a model with a single target and a single sink state.
             * Eliminates all states for which the outgoing transitions are constant.
             * 
             * Also checks whether the non constant functions are linear and whether the model checking result is independent of parameters (i.e., constant)
             * The flags of This model checker  are set accordingly.
             */
            void computeSimplifiedModel(storm::storage::BitVector const& targetStates);
            
            /*!
             * Instantiates the approximation model to compute bounds on the maximal/minimal reachability probability.
             * If the current region result is EXISTSSAT (or EXISTSVIOLATED), then this function tries to prove ALLSAT (or ALLVIOLATED).
             * If this succeeded, then the region check result is changed accordingly.
             * If the current region result is UNKNOWN, then this function first tries to prove ALLSAT and if that failed, it tries to prove ALLVIOLATED.
             * In any case, the computed bounds are written to the given lowerBounds/upperBounds.
             * However, if only the lowerBounds (or upperBounds) have been computed, the other vector is set to a vector of size 0.
             * True is returned iff either ALLSAT or ALLVIOLATED could be proved.
             */
            bool checkApproximativeProbabilities(ParameterRegion& region, std::vector<ConstantType>& lowerBounds, std::vector<ConstantType>& upperBounds); 
            
            /*!
             * Checks the value of the function at some sampling points within the given region.
             * May set the satPoint and violatedPoint of the regions if they are not yet specified and such points are found
             * Also changes the regioncheckresult of the region to EXISTSSAT, EXISTSVIOLATED, or EXISTSBOTH
             * 
             * @return true if an violated point as well as a sat point has been found during the process
             */
            bool checkSamplePoints(ParameterRegion& region);
            
            /*!
             * Checks the value of the function at the given sampling point.
             * May set the satPoint and violatedPoint of the regions if thy are not yet specified and such point is given.
             * Also changes the regioncheckresult of the region to EXISTSSAT, EXISTSVIOLATED, or EXISTSBOTH
             * 
             * @param viaReachProbFunction if set, the sampling will be done via the reachProbFunction.
             *                             Otherwise, the model will be instantiated and checked
             * 
             * @return true if an violated point as well as a sat point has been found, i.e., the check result is changed to EXISTSOTH
             */
            bool checkPoint(ParameterRegion& region, std::map<VariableType, CoefficientType>const& point, bool viaReachProbFunction=false);
            
            /*!
             * Returns the reachability probability function. 
             * If it is not yet available, it is computed via state elimination.
             * After that, the function is available and for the next call of this method it will not be computed again.
             */
            ParametricType getReachProbFunction();
            
            /*!
             * Starts the SMTSolver to get the result.
             * The current regioncheckresult of the region should be EXISTSSAT or EXISTVIOLATED.
             * Otherwise, a sampingPoint will be computed.
             * True is returned iff the solver was successful (i.e., it returned sat or unsat)
             * A Sat- or Violated point is set, if the solver has found one (not yet implemented!).
             * The region checkResult of the given region is changed accordingly.
             */
            bool checkFullSmt(ParameterRegion& region); 
            
            //initializes the given solver which can later be used to give an exact result regarding the whole model.
            void initializeSMTSolver(std::shared_ptr<storm::solver::Smt2SmtSolver>& solver, ParametricType const& reachProbFunction, storm::logic::ProbabilityOperatorFormula const& formula);
            
            /*!
             * Returns true iff the given value satisfies the bound given by the specified property
             */
            template <typename ValueType>
            bool valueIsInBoundOfFormula(ValueType value);
            
            // The model this model checker is supposed to analyze.
            storm::models::sparse::Dtmc<ParametricType> const& model;
            
            //classes that provide auxilliary functions
            // Instance of an elimination model checker to access its functions
            storm::modelchecker::SparseDtmcEliminationModelChecker<ParametricType> eliminationModelChecker;
            // comparators that can be used to compare constants.
            storm::utility::ConstantsComparator<ParametricType> parametricTypeComparator;
            storm::utility::ConstantsComparator<ConstantType> constantTypeComparator;
            
            //the following members depend on the currently specified formula:
            //the currently specified formula
            std::unique_ptr<storm::logic::ProbabilityOperatorFormula> probabilityOperatorFormula;
            // the original model after states with constant transitions have been eliminated
            std::shared_ptr<storm::models::sparse::Dtmc<ParametricType>> simplifiedModel;
            // the model that can be instantiated to check the value at a certain point
            std::shared_ptr<SamplingModel> samplingModel;
            // the model that  is used to approximate the probability values
            std::shared_ptr<ApproximationModel> approximationModel;
            // The  function for the reachability probability in the initial state 
            ParametricType reachProbFunction;
            // a flag that is true if there are only linear functions at transitions of the model
            bool hasOnlyLinearFunctions;
            // a flag that is true iff the reachability probability function has been computed
            bool isReachProbFunctionComputed;
            // a flag that is true iff the resulting reachability probability is constant
            bool isResultConstant;
            // the smt solver that is used to prove properties with the help of the reachProbFunction
            std::shared_ptr<storm::solver::Smt2SmtSolver> smtSolver;
            
            // runtimes and other information for statistics. 
            uint_fast64_t numOfCheckedRegions;
            uint_fast64_t numOfRegionsSolvedThroughSampling;
            uint_fast64_t numOfRegionsSolvedThroughApproximation;
            uint_fast64_t numOfRegionsSolvedThroughSubsystemSmt;
            uint_fast64_t numOfRegionsSolvedThroughFullSmt;
            uint_fast64_t numOfRegionsExistsBoth;
            uint_fast64_t numOfRegionsAllSat;
            uint_fast64_t numOfRegionsAllViolated;
            
            std::chrono::high_resolution_clock::duration timePreprocessing;
            std::chrono::high_resolution_clock::duration timeInitialStateElimination;
            std::chrono::high_resolution_clock::duration timeComputeReachProbFunction;
            std::chrono::high_resolution_clock::duration timeCheckRegion;
            std::chrono::high_resolution_clock::duration timeSampling;
            std::chrono::high_resolution_clock::duration timeApproximation;
            std::chrono::high_resolution_clock::duration timeMDPBuild;
            std::chrono::high_resolution_clock::duration timeSubsystemSmt;
            std::chrono::high_resolution_clock::duration timeFullSmt;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_ */
