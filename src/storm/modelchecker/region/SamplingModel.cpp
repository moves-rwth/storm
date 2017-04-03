/* 
 * File:   SamplingModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:31 AM
 */

#include "storm/modelchecker/region/SamplingModel.h"

#include "storm/logic/FragmentSpecification.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/ModelType.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"
#include "storm/utility/region.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"
#include "storm/utility/policyguessing.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        namespace region {
        
            template<typename ParametricSparseModelType, typename ConstantType>
            SamplingModel<ParametricSparseModelType, ConstantType>::SamplingModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula) : modelInstantiator(parametricModel){
                //First some simple checks and initializations..
                this->typeOfParametricModel = parametricModel.getType();
                if(formula->isProbabilityOperatorFormula()){
                    this->computeRewards=false;
                    STORM_LOG_THROW(this->typeOfParametricModel==storm::models::ModelType::Dtmc || this->typeOfParametricModel==storm::models::ModelType::Mdp, storm::exceptions::InvalidArgumentException, "Sampling with probabilities is only implemented for Dtmcs and Mdps");
                    STORM_LOG_THROW(formula->getSubformula().isEventuallyFormula(), storm::exceptions::InvalidArgumentException, "The subformula should be an eventually formula");
                    STORM_LOG_THROW(formula->getSubformula().asEventuallyFormula().getSubformula().isInFragment(storm::logic::propositional()), storm::exceptions::InvalidArgumentException, "The subsubformula should be a propositional formula");
                } else if(formula->isRewardOperatorFormula()){
                    this->computeRewards=true;
                    STORM_LOG_THROW(this->typeOfParametricModel==storm::models::ModelType::Dtmc, storm::exceptions::InvalidArgumentException, "Sampling with rewards is only implemented for Dtmcs");
                    STORM_LOG_THROW(parametricModel.hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the sampling model should be unique");
                    STORM_LOG_THROW(parametricModel.getUniqueRewardModel().hasOnlyStateRewards(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the sampling model should have state rewards only");
                    STORM_LOG_THROW(formula->getSubformula().isEventuallyFormula(), storm::exceptions::InvalidArgumentException, "The subformula should be a reachabilityreward formula");
                    STORM_LOG_THROW(formula->getSubformula().asEventuallyFormula().getSubformula().isInFragment(storm::logic::propositional()), storm::exceptions::InvalidArgumentException, "The subsubformula should be a propositional formula");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid formula: " << formula << ". Sampling model only supports eventually or reachability reward formulae.");
                }
                
                //Compute targetstates.
                storm::modelchecker::SparsePropositionalModelChecker<ParametricSparseModelType> modelChecker(parametricModel);
                std::unique_ptr<CheckResult> targetStatesResultPtr = modelChecker.check(this->computeRewards ?  formula->getSubformula().asReachabilityRewardFormula().getSubformula() : formula->getSubformula().asEventuallyFormula().getSubformula());
                targetStates = std::move(targetStatesResultPtr->asExplicitQualitativeCheckResult().getTruthValuesVector());
                
                //Compute maybestates. This assumes that the given instantiations are graph preserving!
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01;
                storm::storage::BitVector phiStates(parametricModel.getNumberOfStates(),true);
                if(storm::logic::isLowerBound(formula->getComparisonType())){
                    statesWithProbability01 = storm::utility::graph::performProb01Min(parametricModel.getTransitionMatrix(), parametricModel.getTransitionMatrix().getRowGroupIndices(), parametricModel.getBackwardTransitions(), phiStates, this->targetStates);
                } else {
                    statesWithProbability01 = storm::utility::graph::performProb01Max(parametricModel.getTransitionMatrix(), parametricModel.getTransitionMatrix().getRowGroupIndices(), parametricModel.getBackwardTransitions(), phiStates, this->targetStates);
                }
                if(this->computeRewards){
                    this->maybeStates = (statesWithProbability01.second & ~this->targetStates);
                } else {
                    this->maybeStates = ~(statesWithProbability01.first | statesWithProbability01.second);
                }
                //Initial state
                STORM_LOG_THROW(parametricModel.getInitialStates().getNumberOfSetBits()==1, storm::exceptions::InvalidArgumentException, "The given model has more or less then one initial state");
                storm::storage::sparse::state_type initialState = *parametricModel.getInitialStates().begin();
                STORM_LOG_THROW(maybeStates.get(initialState), storm::exceptions::InvalidArgumentException, "The value in the initial state of the given model is independent of parameters");
                //The (state-)indices in the equation system will be different from the original ones, as the eq-sys only considers maybestates.
                //Hence, we use this vector to translate from old indices to new ones.
                std::vector<std::size_t> newIndices(parametricModel.getNumberOfStates(), parametricModel.getNumberOfStates()); //initialize with some illegal index
                std::size_t newIndex=0;
                for(auto const& index : maybeStates){
                    newIndices[index]=newIndex;
                    ++newIndex;
                }

                this->solverData.result = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), this->computeRewards ? storm::utility::one<ConstantType>() : ConstantType(0.5));
                this->solverData.initialStateIndex = newIndices[initialState];
                this->solverData.lastScheduler = storm::storage::TotalScheduler(std::vector<uint_fast64_t>(maybeStates.getNumberOfSetBits(), 0));
                
                storm::storage::BitVector filter(this->solverData.result.size(), false);
                filter.set(this->solverData.initialStateIndex, true);
                this->solverData.solveGoal = std::make_unique<storm::solver::BoundedGoal<ConstantType>>(
                            storm::logic::isLowerBound(formula->getComparisonType()) ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize,
                            formula->getComparisonType(), formula->getThresholdAs<ConstantType>(),
                            filter
                        );
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            SamplingModel<ParametricSparseModelType, ConstantType>::~SamplingModel() {
                //Intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::vector<ConstantType> SamplingModel<ParametricSparseModelType, ConstantType>::computeValues(std::map<VariableType, CoefficientType>const& point) {
                invokeSolver(this->modelInstantiator.instantiate(point), false); //false: no early termination
                std::vector<ConstantType> result(this->maybeStates.size());
                storm::utility::vector::setVectorValues(result, this->maybeStates, this->solverData.result);
                storm::utility::vector::setVectorValues(result, this->targetStates, this->computeRewards ? storm::utility::zero<ConstantType>() : storm::utility::one<ConstantType>());
                storm::utility::vector::setVectorValues(result, ~(this->maybeStates | this->targetStates), this->computeRewards ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>());
                
                return result;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType SamplingModel<ParametricSparseModelType, ConstantType>::computeInitialStateValue(std::map<VariableType, CoefficientType>const& point) {
                invokeSolver(this->modelInstantiator.instantiate(point), false); //false: no early termination
                return this->solverData.result[this->solverData.initialStateIndex];
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool SamplingModel<ParametricSparseModelType, ConstantType>::checkFormulaOnSamplingPoint(std::map<VariableType, CoefficientType>const& point) {
                invokeSolver(this->modelInstantiator.instantiate(point), true); //allow early termination
                return this->solverData.solveGoal->achieved(this->solverData.result);
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void SamplingModel<ParametricSparseModelType, ConstantType>::invokeSolver(ConstantSparseModelType const& instantiatedModel, bool allowEarlyTermination){
                if(this->typeOfParametricModel == storm::models::ModelType::Dtmc){
                    storm::storage::SparseMatrix<ConstantType> submatrix = instantiatedModel.getTransitionMatrix().getSubmatrix(true, this->maybeStates, this->maybeStates, true);
                    submatrix.convertToEquationSystem();
                    std::unique_ptr<storm::solver::LinearEquationSolver<ConstantType>> solver = storm::solver::GeneralLinearEquationSolverFactory<ConstantType>().create(submatrix);
                    std::vector<ConstantType> b;
                    if(this->computeRewards){
                        b.resize(submatrix.getRowCount());
                        storm::utility::vector::selectVectorValues(b, this->maybeStates, instantiatedModel.getUniqueRewardModel().getStateRewardVector());
                    } else {
                        b = instantiatedModel.getTransitionMatrix().getConstrainedRowSumVector(this->maybeStates, this->targetStates);
                    }
                    solver->solveEquations(this->solverData.result, b);
                } else if(this->typeOfParametricModel == storm::models::ModelType::Mdp){
                    storm::storage::SparseMatrix<ConstantType> submatrix = instantiatedModel.getTransitionMatrix().getSubmatrix(true, this->maybeStates, this->maybeStates, false);
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ConstantType>> solver = storm::solver::GeneralMinMaxLinearEquationSolverFactory<ConstantType>().create(submatrix);
                    std::vector<ConstantType> b = instantiatedModel.getTransitionMatrix().getConstrainedRowGroupSumVector(this->maybeStates, this->targetStates);
                    storm::storage::BitVector targetChoices(b.size(), false);
                    for(std::size_t i = 0; i<b.size(); ++i){
                        if(b[i]>storm::utility::zero<ConstantType>())
                                targetChoices.set(i);
                    }
                    solver->setOptimizationDirection(this->solverData.solveGoal->direction());
                    if(allowEarlyTermination){
                        if(this->solverData.solveGoal->minimize()){
                            //Take minimum
                            //Note that value iteration will approach the minimum from above as we start it with values that correspond to some scheduler-induced DTMC
                            solver->setTerminationCondition(std::make_unique<storm::solver::TerminateIfFilteredExtremumBelowThreshold<ConstantType>>(
                                                                                                                                            this->solverData.solveGoal->relevantValues(),
                                                                                                                                            this->solverData.solveGoal->thresholdValue(),
                                                                                                                                            this->solverData.solveGoal->boundIsStrict(),
                                                                                                                                            true
                                                                                                                                            ));
                        } else {
                            //Take maximum
                            solver->setTerminationCondition(std::make_unique<storm::solver::TerminateIfFilteredExtremumExceedsThreshold<ConstantType>>(
                                                                                                                                              this->solverData.solveGoal->relevantValues(),
                                                                                                                                              this->solverData.solveGoal->thresholdValue(),
                                                                                                                                              this->solverData.solveGoal->boundIsStrict(),
                                                                                                                                              false
                                                                                                                                              ));
                        }
                    }
                    storm::utility::policyguessing::solveMinMaxLinearEquationSystem(*solver,submatrix,
                                        this->solverData.result, b,
                                        this->solverData.solveGoal->direction(),
                                        this->solverData.lastScheduler,
                                        targetChoices, (this->computeRewards ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>()));
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected Type of model");
                }
            }
            
        
#ifdef STORM_HAVE_CARL
            template class SamplingModel<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
            template class SamplingModel<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
#endif
        } //namespace region
    }
}
