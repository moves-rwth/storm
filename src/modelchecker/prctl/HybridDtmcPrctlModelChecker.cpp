#include "src/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"

#include "src/storage/dd/CuddOdd.h"

#include "src/utility/macros.h"
#include "src/utility/graph.h"

#include "src/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "src/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "src/modelchecker/results/HybridQuantitativeCheckResult.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType DdType, typename ValueType>
        HybridDtmcPrctlModelChecker<DdType, ValueType>::HybridDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType> const& model, std::unique_ptr<storm::utility::solver::LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        HybridDtmcPrctlModelChecker<DdType, ValueType>::HybridDtmcPrctlModelChecker(storm::models::symbolic::Dtmc<DdType> const& model) : SymbolicPropositionalModelChecker<DdType>(model), linearEquationSolverFactory(new storm::utility::solver::LinearEquationSolverFactory<ValueType>()) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        bool HybridDtmcPrctlModelChecker<DdType, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            return formula.isPctlStateFormula() || formula.isPctlPathFormula() || formula.isRewardPathFormula();
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeUntilProbabilitiesHelper(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, bool qualitative, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
            // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
            // probability 0 and 1 of satisfying the until-formula.
            std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>> statesWithProbability01 = storm::utility::graph::performProb01(model, transitionMatrix, phiStates, psiStates);
            storm::dd::Bdd<DdType> maybeStates = !statesWithProbability01.first && !statesWithProbability01.second && model.getReachableStates();
            
            // Perform some logging.
            STORM_LOG_INFO("Found " << statesWithProbability01.first.getNonZeroCount() << " 'no' states.");
            STORM_LOG_INFO("Found " << statesWithProbability01.second.getNonZeroCount() << " 'yes' states.");
            STORM_LOG_INFO("Found " << maybeStates.getNonZeroCount() << " 'maybe' states.");
            
            // Check whether we need to compute exact probabilities for some states.
            if (qualitative) {
                // Set the values for all maybe-states to 0.5 to indicate that their probability values are neither 0 nor 1.
                return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), statesWithProbability01.second.toAdd() + maybeStates.toAdd() * model.getManager().getConstant(0.5)));
            } else {
                // If there are maybe states, we need to solve an equation system.
                if (!maybeStates.isZero()) {
                    // Create the ODD for the translation between symbolic and explicit storage.
                    storm::dd::Odd<DdType> odd(maybeStates);
                    
                    // Create the matrix and the vector for the equation system.
                    storm::dd::Add<DdType> maybeStatesAdd = maybeStates.toAdd();
                    
                    // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                    // non-maybe states in the matrix.
                    storm::dd::Add<DdType> submatrix = transitionMatrix * maybeStatesAdd;
                    
                    // Then compute the vector that contains the one-step probabilities to a state with probability 1 for all
                    // maybe states.
                    storm::dd::Add<DdType> prob1StatesAsColumn = statesWithProbability01.second.toAdd();
                    prob1StatesAsColumn = prob1StatesAsColumn.swapVariables(model.getRowColumnMetaVariablePairs());
                    storm::dd::Add<DdType> subvector = submatrix * prob1StatesAsColumn;
                    subvector = subvector.sumAbstract(model.getColumnVariables());
                    
                    // Finally cut away all columns targeting non-maybe states and convert the matrix into the matrix needed
                    // for solving the equation system (i.e. compute (I-A)).
                    submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                    submatrix = (model.getRowColumnIdentity() * maybeStatesAdd) - submatrix;
                    
                    // Create the solution vector.
                    std::vector<ValueType> x(maybeStates.getNonZeroCount(), ValueType(0.5));
                    
                    // Translate the symbolic matrix/vector to their explicit representations and solve the equation system.
                    storm::storage::SparseMatrix<ValueType> explicitSubmatrix = submatrix.toMatrix(odd, odd);
                    std::vector<ValueType> b = subvector.template toVector<ValueType>(odd);
                    
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(explicitSubmatrix);
                    solver->solveEquationSystem(x, b);
                    
                    // Return a hybrid check result that stores the numerical values explicitly.
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, statesWithProbability01.second.toAdd(), maybeStates, odd, x));
                } else {
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), statesWithProbability01.second.toAdd()));
                }
            }
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return this->computeUntilProbabilitiesHelper(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), qualitative, *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeNextProbabilities(storm::logic::NextFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(pathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(this->getModel().getReachableStates(), this->computeNextProbabilitiesHelper(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector())));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeNextProbabilitiesHelper(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& nextStates) {
            storm::dd::Add<DdType> result = transitionMatrix * nextStates.swapVariables(model.getRowColumnMetaVariablePairs()).toAdd();
            return result.sumAbstract(model.getColumnVariables());
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeBoundedUntilProbabilities(storm::logic::BoundedUntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(pathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            SymbolicQualitativeCheckResult<DdType> const& leftResult = leftResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            SymbolicQualitativeCheckResult<DdType> const& rightResult = rightResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return this->computeBoundedUntilProbabilitiesHelper(this->getModel(), this->getModel().getTransitionMatrix(), leftResult.getTruthValuesVector(), rightResult.getTruthValuesVector(), pathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeBoundedUntilProbabilitiesHelper(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& phiStates, storm::dd::Bdd<DdType> const& psiStates, uint_fast64_t stepBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
            // We need to identify the states which have to be taken out of the matrix, i.e. all states that have
            // probability 0 or 1 of satisfying the until-formula.
            storm::dd::Bdd<DdType> statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(model, transitionMatrix.notZero(), phiStates, psiStates, stepBound);
            storm::dd::Bdd<DdType> maybeStates = statesWithProbabilityGreater0 && !psiStates && model.getReachableStates();
            
            // If there are maybe states, we need to perform matrix-vector multiplications.
            if (!maybeStates.isZero()) {
                // Create the ODD for the translation between symbolic and explicit storage.
                storm::dd::Odd<DdType> odd(maybeStates);
                
                // Create the matrix and the vector for the equation system.
                storm::dd::Add<DdType> maybeStatesAdd = maybeStates.toAdd();
                
                // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                // non-maybe states in the matrix.
                storm::dd::Add<DdType> submatrix = transitionMatrix * maybeStatesAdd;
                
                // Then compute the vector that contains the one-step probabilities to a state with probability 1 for all
                // maybe states.
                storm::dd::Add<DdType> prob1StatesAsColumn = psiStates.toAdd().swapVariables(model.getRowColumnMetaVariablePairs());
                storm::dd::Add<DdType> subvector = (submatrix * prob1StatesAsColumn).sumAbstract(model.getColumnVariables());
                
                // Finally cut away all columns targeting non-maybe states.
                submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                
                // Create the solution vector.
                std::vector<ValueType> x(maybeStates.getNonZeroCount(), storm::utility::zero<ValueType>());
                
                // Translate the symbolic matrix/vector to their explicit representations.
                storm::storage::SparseMatrix<ValueType> explicitSubmatrix = submatrix.toMatrix(odd, odd);
                std::vector<ValueType> b = subvector.template toVector<ValueType>(odd);
                
                std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(explicitSubmatrix);
                solver->performMatrixVectorMultiplication(x, &b, stepBound);
                
                // Return a hybrid check result that stores the numerical values explicitly.
                return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, psiStates.toAdd(), maybeStates, odd, x));
            } else {
                return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), psiStates.toAdd()));
            }
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeCumulativeRewards(storm::logic::CumulativeRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return this->computeCumulativeRewardsHelper(this->getModel(), this->getModel().getTransitionMatrix(), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeCumulativeRewardsHelper(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, uint_fast64_t stepBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
            // Only compute the result if the model has at least one reward this->getModel().
            STORM_LOG_THROW(model.hasStateRewards() || model.hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Compute the reward vector to add in each step based on the available reward models.
            storm::dd::Add<DdType> totalRewardVector = model.hasStateRewards() ? model.getStateRewardVector() : model.getManager().getAddZero();
            if (model.hasTransitionRewards()) {
                totalRewardVector += (transitionMatrix * model.getTransitionRewardMatrix()).sumAbstract(model.getColumnVariables());
            }
            
            // Create the ODD for the translation between symbolic and explicit storage.
            storm::dd::Odd<DdType> odd(model.getReachableStates());
            
            // Create the solution vector.
            std::vector<ValueType> x(model.getNumberOfStates(), storm::utility::zero<ValueType>());
            
            // Translate the symbolic matrix/vector to their explicit representations.
            storm::storage::SparseMatrix<ValueType> explicitMatrix = transitionMatrix.toMatrix(odd, odd);
            std::vector<ValueType> b = totalRewardVector.template toVector<ValueType>(odd);
            
            // Perform the matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(explicitMatrix);
            solver->performMatrixVectorMultiplication(x, &b, stepBound);
            
            // Return a hybrid check result that stores the numerical values explicitly.
            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().getAddZero(), model.getReachableStates(), odd, x));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeInstantaneousRewards(storm::logic::InstantaneousRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            STORM_LOG_THROW(rewardPathFormula.hasDiscreteTimeBound(), storm::exceptions::InvalidArgumentException, "Formula needs to have a discrete time bound.");
            return this->computeInstantaneousRewardsHelper(this->getModel(), this->getModel().getTransitionMatrix(), rewardPathFormula.getDiscreteTimeBound(), *this->linearEquationSolverFactory);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeInstantaneousRewardsHelper(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, uint_fast64_t stepBound, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory) {
            // Only compute the result if the model has at least one reward this->getModel().
            STORM_LOG_THROW(model.hasStateRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");
            
            // Create the ODD for the translation between symbolic and explicit storage.
            storm::dd::Odd<DdType> odd(model.getReachableStates());
            
            // Create the solution vector (and initialize it to the state rewards of the model).
            std::vector<ValueType> x = model.getStateRewardVector().template toVector<ValueType>(odd);
            
            // Translate the symbolic matrix to its explicit representations.
            storm::storage::SparseMatrix<ValueType> explicitMatrix = transitionMatrix.toMatrix(odd, odd);
            
            // Perform the matrix-vector multiplication.
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(explicitMatrix);
            solver->performMatrixVectorMultiplication(x, nullptr, stepBound);
            
            // Return a hybrid check result that stores the numerical values explicitly.
            return std::unique_ptr<CheckResult>(new HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getManager().getBddZero(), model.getManager().getAddZero(), model.getReachableStates(), odd, x));
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            SymbolicQualitativeCheckResult<DdType> const& subResult = subResultPointer->asSymbolicQualitativeCheckResult<DdType>();
            return this->computeReachabilityRewardsHelper(this->getModel(), this->getModel().getTransitionMatrix(), subResult.getTruthValuesVector(), *this->linearEquationSolverFactory, qualitative);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<CheckResult> HybridDtmcPrctlModelChecker<DdType, ValueType>::computeReachabilityRewardsHelper(storm::models::symbolic::Model<DdType> const& model, storm::dd::Add<DdType> const& transitionMatrix, storm::dd::Bdd<DdType> const& targetStates, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& linearEquationSolverFactory, bool qualitative) {
            // Only compute the result if the model has at least one reward model.
            STORM_LOG_THROW(model.hasStateRewards() || model.hasTransitionRewards(), storm::exceptions::InvalidPropertyException, "Missing reward model for formula. Skipping formula.");

            // Determine which states have a reward of infinity by definition.
            storm::dd::Bdd<DdType> infinityStates = storm::utility::graph::performProb1(model, transitionMatrix.notZero(), model.getReachableStates(), targetStates);
            infinityStates = !infinityStates && model.getReachableStates();
            storm::dd::Bdd<DdType> maybeStates = (!targetStates && !infinityStates) && model.getReachableStates();
            STORM_LOG_INFO("Found " << infinityStates.getNonZeroCount() << " 'infinity' states.");
            STORM_LOG_INFO("Found " << targetStates.getNonZeroCount() << " 'target' states.");
            STORM_LOG_INFO("Found " << maybeStates.getNonZeroCount() << " 'maybe' states.");

            // Check whether we need to compute exact rewards for some states.
            if (qualitative) {
                // Set the values for all maybe-states to 1 to indicate that their reward values
                // are neither 0 nor infinity.
                return std::unique_ptr<CheckResult>(new SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), infinityStates.toAdd() * model.getManager().getConstant(storm::utility::infinity<ValueType>()) + maybeStates.toAdd() * model.getManager().getConstant(storm::utility::one<ValueType>())));
            } else {
                // If there are maybe states, we need to solve an equation system.
                if (!maybeStates.isZero()) {
                    // Create the ODD for the translation between symbolic and explicit storage.
                    storm::dd::Odd<DdType> odd(maybeStates);
                    
                    // Create the matrix and the vector for the equation system.
                    storm::dd::Add<DdType> maybeStatesAdd = maybeStates.toAdd();
                    
                    // Start by cutting away all rows that do not belong to maybe states. Note that this leaves columns targeting
                    // non-maybe states in the matrix.
                    storm::dd::Add<DdType> submatrix = transitionMatrix * maybeStatesAdd;
                    
                    // Then compute the state reward vector to use in the computation.
                    storm::dd::Add<DdType> subvector = model.hasStateRewards() ? maybeStatesAdd * model.getStateRewardVector() : model.getManager().getAddZero();
                    if (model.hasTransitionRewards()) {
                        subvector += (submatrix * model.getTransitionRewardMatrix()).sumAbstract(model.getColumnVariables());
                    }
                    
                    // Finally cut away all columns targeting non-maybe states and convert the matrix into the matrix needed
                    // for solving the equation system (i.e. compute (I-A)).
                    submatrix *= maybeStatesAdd.swapVariables(model.getRowColumnMetaVariablePairs());
                    submatrix = (model.getRowColumnIdentity() * maybeStatesAdd) - submatrix;
                    
                    // Create the solution vector.
                    std::vector<ValueType> x(maybeStates.getNonZeroCount(), ValueType(0.5));
                    
                    // Translate the symbolic matrix/vector to their explicit representations.
                    storm::storage::SparseMatrix<ValueType> explicitSubmatrix = submatrix.toMatrix(odd, odd);
                    std::vector<ValueType> b = subvector.template toVector<ValueType>(odd);
                    
                    // Now solve the resulting equation system.
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(explicitSubmatrix);
                    solver->solveEquationSystem(x, b);
                    
                    // Return a hybrid check result that stores the numerical values explicitly.
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::HybridQuantitativeCheckResult<DdType>(model.getReachableStates(), model.getReachableStates() && !maybeStates, infinityStates.toAdd() * model.getManager().getConstant(storm::utility::infinity<ValueType>()), maybeStates, odd, x));
                } else {
                    return std::unique_ptr<CheckResult>(new storm::modelchecker::SymbolicQuantitativeCheckResult<DdType>(model.getReachableStates(), infinityStates.toAdd() * model.getManager().getConstant(storm::utility::infinity<ValueType>())));
                }
            }
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::models::symbolic::Dtmc<DdType> const& HybridDtmcPrctlModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Dtmc<DdType>>();
        }
        
        template class HybridDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double>;
    }
}