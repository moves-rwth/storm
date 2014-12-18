#include "src/solver/SmtSolver.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {
	namespace solver {

        SmtSolver::SmtSolver() {
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
        
        storm::expressions::SimpleValuation SmtSolver::getModel() {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        std::vector<storm::expressions::SimpleValuation> SmtSolver::allSat(std::vector<storm::expressions::Expression> const& important) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        uint_fast64_t SmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> callback) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support model generation.");
        }
        
        uint_fast64_t SmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(ModelReference&)> callback) {
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

        storm::expressions::Expression SmtSolver::getInterpolant(std::vector<uint_fast64_t> groupsA) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This solver does not support generation of interpolants.");
        }

    } // namespace solver
} // namespace storm