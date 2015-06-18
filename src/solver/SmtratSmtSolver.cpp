#include "src/solver/SmtratSmtSolver.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/InvalidStateException.h"

#ifdef STORM_HAVE_SMTRAT
#include "lib/smtrat.h"
#endif 

namespace storm {
	namespace solver {

        
		SmtratSmtSolver::SmtratSmtSolver(storm::expressions::ExpressionManager& manager) : SmtSolver(manager)
		{
#ifndef STORM_HAVE_SMTRAT
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without SMT-RAT support.");
#else 
                     // Construct the settingsManager.
                    //smtrat::RuntimeSettingsManager settingsManager;
                    
                    // Construct solver.
                    smtrat::RatOne* solver = new smtrat::RatOne();

                    //std::list<std::pair<std::string, smtrat::RuntimeSettings*> > settingsObjects =
                    smtrat::addModules( solver );

                    // Introduce the settingsObjects from the modules to the manager.
                    //settingsManager.addSettingsObject( settingsObjects );
                    //settingsObjects.clear();
#endif 
                }  
        
		SmtratSmtSolver::~SmtratSmtSolver() {
                    delete solver;
                }

		void SmtratSmtSolver::push()
		{
#ifndef STORM_HAVE_SMTRAT
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without SMT-RAT support.");
#else
                    this->solver->push();
#endif
		}

		void SmtratSmtSolver::pop()
		{
#ifndef STORM_HAVE_SMTRAT
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without SMT-RAT support.");
#else
                    this->solver->pop();
#endif		
                }

		void SmtratSmtSolver::pop(uint_fast64_t n)
		{
#ifndef STORM_HAVE_SMTRAT
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without SMT-RAT support.");
#else
                    this->solver->pop(static_cast<unsigned int>(n));
#endif
		}

		
		SmtSolver::CheckResult SmtratSmtSolver::check()
		{
#ifdef STORM_HAVE_SMTRAT
                    switch (this->solver->check()) {
                        case smtrat::Answer::True:
                            this->lastResult = SmtSolver::CheckResult::Sat;
                            break;
                        case smtrat::Answer::False:
                            this->lastResult = SmtSolver::CheckResult::Unsat;
                            break;
                        case smtrat::Answer::Unknown:
                            this->lastResult = SmtSolver::CheckResult::Unknown;
                            break;
                        default:
                            // maybe exit
                            this->lastResult = SmtSolver::CheckResult::Unknown;
                            break;
                    }
                    return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}
                
                void SmtratSmtSolver::add(const storm::RawPolynomial& pol, storm::CompareRelation cr) {
                    this->solver->add(smtrat::FormulaT(pol, cr));
                }
                
                template<>
                smtrat::Model SmtratSmtSolver::getModel() const
                {
                    return this->solver->model();
                }
                
                std::vector<smtrat::FormulasT> const& SmtratSmtSolver::getUnsatisfiableCores() const
                {
                    return this->solver->infeasibleSubsets();
                }
                

        }
}
