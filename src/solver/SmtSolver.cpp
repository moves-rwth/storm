#include "src/solver/SmtSolver.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {
	namespace solver {

        SmtSolver::ModelReference::ModelReference(storm::expressions::ExpressionManager const& manager) : manager(manager) {
            // Intentionally left empty.
        }
        
        storm::expressions::ExpressionManager const& SmtSolver::ModelReference::getManager() const {
            return manager;
        }
        
        SmtSolver::SmtSolver(storm::expressions::ExpressionManager& manager) : manager(manager) {
            // Intentionally left empty.
        }
        
        SmtSolver::~SmtSolver() {
            // Intentionally left empty.
        }
        
        void SmtSolver::add(std::set<storm::expressions::Expression> const& assertions) {
            for (storm::expressions::Expression assertion : assertions) {
                this->add(assertion);
            }
        }
        
        void SmtSolver::add(std::initializer_list<storm::expressions::Expression> const& assertions) {
            for (storm::expressions::Expression assertion : assertions) {
                this->add(assertion);
            }
        }
        
        void SmtSolver::pop(uint_fast64_t n) {
			for (uint_fast64_t i = 0; i < n; ++i) {
				this->pop();
			}
		}
        
        storm::expressions::SimpleValuation SmtSolver::getModelAsValuation() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        std::shared_ptr<SmtSolver::ModelReference> SmtSolver::getModel() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        std::vector<storm::expressions::SimpleValuation> SmtSolver::allSat(std::vector<storm::expressions::Variable> const& important) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        uint_fast64_t SmtSolver::allSat(std::vector<storm::expressions::Variable> const& important, std::function<bool(storm::expressions::SimpleValuation&)> const& callback) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        uint_fast64_t SmtSolver::allSat(std::vector<storm::expressions::Variable> const& important, std::function<bool(ModelReference&)> const& callback) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        std::vector<storm::expressions::Expression> SmtSolver::getUnsatCore() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support generation of unsatisfiable cores.");
        }
        
        std::vector<storm::expressions::Expression> SmtSolver::getUnsatAssumptions() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support generation of unsatisfiable cores.");
        }
        
        void SmtSolver::setInterpolationGroup(uint_fast64_t group) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support generation of interpolants.");
        }

        storm::expressions::Expression SmtSolver::getInterpolant(std::vector<uint_fast64_t> const& groupsA) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support generation of interpolants.");
        }

        storm::expressions::ExpressionManager const& SmtSolver::getManager() const {
            return manager;
        }

        storm::expressions::ExpressionManager& SmtSolver::getManager() {
            return manager;
        }

    } // namespace solver
} // namespace storm