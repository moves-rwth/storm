#include "src/solver/Z3SmtSolver.h"


namespace storm {
	namespace solver {
		Z3SmtSolver::Z3SmtSolver(Options options)
#ifdef STORM_HAVE_Z3
			: m_context()
			, m_solver(m_context)
			, m_adapter(m_context, {})
#endif
		{
			//intentionally left empty
		}
		Z3SmtSolver::~Z3SmtSolver() {};

		void Z3SmtSolver::push()
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.push();
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::pop()
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.pop();
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::pop(uint_fast64_t n)
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.pop((unsigned int)n);
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::reset()
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.reset();
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::assertExpression(storm::expressions::Expression &e)
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.add(m_adapter.translateExpression(e, true));
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::check()
		{
#ifdef STORM_HAVE_Z3
			switch (this->m_solver.check())
			{
				case z3::sat:
					return SmtSolver::CheckResult::SAT;
				case z3::unsat:
					return SmtSolver::CheckResult::UNSAT;
				default:
					break;
			}
			return SmtSolver::CheckResult::UNKNOWN;
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> &assumptions)
		{
#ifdef STORM_HAVE_Z3
			z3::expr_vector z3Assumptions(this->m_context);

			for (storm::expressions::Expression assumption : assumptions) {
				z3Assumptions.push_back(this->m_adapter.translateExpression(assumption));
			}

			switch (this->m_solver.check(z3Assumptions))
			{
			case z3::sat:
				return SmtSolver::CheckResult::SAT;
			case z3::unsat:
				return SmtSolver::CheckResult::UNSAT;
			default:
				break;
			}
			return SmtSolver::CheckResult::UNKNOWN;
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> &assumptions)
		{
#ifdef STORM_HAVE_Z3
			z3::expr_vector z3Assumptions(this->m_context);

			for (storm::expressions::Expression assumption : assumptions) {
				z3Assumptions.push_back(this->m_adapter.translateExpression(assumption));
			}

			switch (this->m_solver.check(z3Assumptions))
			{
			case z3::sat:
				return SmtSolver::CheckResult::SAT;
			case z3::unsat:
				return SmtSolver::CheckResult::UNSAT;
			default:
				break;
			}
			return SmtSolver::CheckResult::UNKNOWN;
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		storm::expressions::SimpleValuation Z3SmtSolver::getModel()
		{
#ifdef STORM_HAVE_Z3
			LOG_THROW(true, storm::exceptions::NotImplementedException, "Model generation is not implemented in this Z3 solver interface.");
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		std::set<storm::expressions::SimpleValuation> Z3SmtSolver::solveAndDiversify(std::set<storm::expressions::SimpleValuation> diversifyers)
		{
#ifdef STORM_HAVE_Z3
			LOG_THROW(true, storm::exceptions::NotImplementedException, "Model generation is not implemented in this Z3 solver interface.");
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		uint_fast64_t Z3SmtSolver::solveAndDiversify(std::set<storm::expressions::SimpleValuation> diversifyers, std::function<bool(storm::expressions::Valuation&) > callback)
		{
#ifdef STORM_HAVE_Z3
			LOG_THROW(true, storm::exceptions::NotImplementedException, "Model generation is not implemented in this Z3 solver interface.");
#else
			LOG_THROW(true, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

	}
}