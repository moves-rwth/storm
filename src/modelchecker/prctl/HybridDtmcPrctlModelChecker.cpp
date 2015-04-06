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
            
            // Create the ODD for the translation between symbolic and explicit storage.
            storm::dd::Odd<DdType> odd(maybeStates);
            
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
                    
                    submatrix.exportToDot("submatrix.dot");
                    
                    // Create the solution vector.
                    std::vector<ValueType> x(maybeStates.getNonZeroCount(), ValueType(0.5));
                    
                    // Translate the symbolic matrix/vector to their explicit representations and solve the equation system.
                    STORM_LOG_DEBUG("Converting the symbolic matrix to a sparse matrix.");
                    storm::storage::SparseMatrix<ValueType> explicitSubmatrix = submatrix.toMatrix(odd, odd);
                    
                    STORM_LOG_DEBUG("Converting the symbolic vector to an explicit vector.");
                    std::vector<ValueType> b = subvector.template toVector<ValueType>(odd);
                    
                    STORM_LOG_DEBUG("Solving explicit linear equation system.");
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory.create(explicitSubmatrix);
                    solver->solveEquationSystem(x, b);
                    
                    // Now that we have the explicit solution of the system, we need to transform it to a symbolic representation.
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
        storm::models::symbolic::Dtmc<DdType> const& HybridDtmcPrctlModelChecker<DdType, ValueType>::getModel() const {
            return this->template getModelAs<storm::models::symbolic::Dtmc<DdType>>();
        }
        
        template class HybridDtmcPrctlModelChecker<storm::dd::DdType::CUDD, double>;
    }
}