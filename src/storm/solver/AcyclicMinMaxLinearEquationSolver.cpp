#include "storm/solver/AcyclicMinMaxLinearEquationSolver.h"

#include "storm/solver/helper/AcyclicSolverHelper.cpp"

#include "storm/utility/vector.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        AcyclicMinMaxLinearEquationSolver<ValueType>::AcyclicMinMaxLinearEquationSolver() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        AcyclicMinMaxLinearEquationSolver<ValueType>::AcyclicMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A) : StandardMinMaxLinearEquationSolver<ValueType>(A) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        AcyclicMinMaxLinearEquationSolver<ValueType>::AcyclicMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A) : StandardMinMaxLinearEquationSolver<ValueType>(std::move(A)) {
            // Intentionally left empty.
        }
   
        
        template<typename ValueType>
        bool AcyclicMinMaxLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_ASSERT(x.size() == this->A->getRowGroupCount(), "Provided x-vector has invalid size.");
            STORM_LOG_ASSERT(b.size() == this->A->getRowCount(), "Provided b-vector has invalid size.");
            // Allocate memory for the scheduler (if required)
            if (this->isTrackSchedulerSet()) {
                if (this->schedulerChoices) {
                    this->schedulerChoices->resize(this->A->getRowGroupCount());
                } else {
                    this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
                }
            }
            
            if (!multiplier) {
                // We have not allocated cache memory, yet
                rowGroupOrdering = helper::computeTopologicalGroupOrdering(*this->A);
                if (!rowGroupOrdering) {
                    // It is not required to reorder the elements.
                    this->multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, *this->A);
                } else {
                    bFactors.clear();
                    orderedMatrix = helper::createReorderedMatrix(*this->A, *rowGroupOrdering, bFactors);
                    this->multiplier = storm::solver::MultiplierFactory<ValueType>().create(env, *orderedMatrix);
                }
                auxiliaryRowVector = std::vector<ValueType>();
            }
            
            std::vector<ValueType> const* bPtr = &b;
            if (rowGroupOrdering) {
                STORM_LOG_ASSERT(rowGroupOrdering->size() == b.size(), "b-vector has unexpected size.");
                auxiliaryRowVector->resize(b.size());
                storm::utility::vector::selectVectorValues(*auxiliaryRowVector, *rowGroupOrdering, b);
                for (auto const& bFactor : bFactors) {
                    (*auxiliaryRowVector)[bFactor.first] *= bFactor.second;
                }
                bPtr = &auxiliaryRowVector.get();
            }
            
            if (this->isTrackSchedulerSet()) {
                this->multiplier->multiplyAndReduceGaussSeidel(env, dir, x, bPtr, &this->schedulerChoices.get(), true);
            } else {
                this->multiplier->multiplyAndReduceGaussSeidel(env, dir, x, bPtr, nullptr, true);
            }
            
            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            return true;
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements AcyclicMinMaxLinearEquationSolver<ValueType>::getRequirements(Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction, bool const& hasInitialScheduler) const {
            // Return the requirements of the underlying solver
            MinMaxLinearEquationSolverRequirements requirements;
            requirements.requireAcyclic();
            return requirements;
        }
        
        template<typename ValueType>
        void AcyclicMinMaxLinearEquationSolver<ValueType>::clearCache() const {
            multiplier.reset();
            orderedMatrix = boost::none;
            rowGroupOrdering = boost::none;
            auxiliaryRowVector = boost::none;
            bFactors.clear();
        }
        
        // Explicitly instantiate the min max linear equation solver.
        template class AcyclicMinMaxLinearEquationSolver<double>;
        
#ifdef STORM_HAVE_CARL
        template class AcyclicMinMaxLinearEquationSolver<storm::RationalNumber>;
#endif
    }
}
