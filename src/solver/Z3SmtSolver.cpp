#include "src/solver/Z3SmtSolver.h"


namespace storm {
	namespace solver {
		Z3SmtSolver::Z3SmtSolver(Options options)
			: m_context()
			, m_solver(m_context)
			, m_adapter(m_context, {}) {
			//intentionally left empty
		}
		Z3SmtSolver::~Z3SmtSolver() {};

		void Z3SmtSolver::push()
		{
			this->m_solver.push();
		}

		void Z3SmtSolver::pop()
		{
			this->m_solver.pop();
		}

		void Z3SmtSolver::pop(uint_fast64_t n)
		{
			this->m_solver.pop(n);
		}

		void Z3SmtSolver::reset()
		{
			this->m_solver.reset();
		}

		void Z3SmtSolver::assertExpression(storm::expressions::Expression &e)
		{
			this->m_solver.add(m_adapter.translateExpression(e, true));
		}

		SmtSolver::CheckResult Z3SmtSolver::check()
		{
			switch (this->m_solver.check())
			{
				case z3::sat:
					return SmtSolver::CheckResult::SAT;
				case z3::unsat:
					return SmtSolver::CheckResult::UNSAT;
				default:
					break;
			}
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> &assumptions)
		{
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
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::unordered_set<storm::expressions::Expression> &assumptions)
		{
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
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> &assumptions)
		{
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
		}

		storm::expressions::SimpleValuation Z3SmtSolver::getModel()
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		std::set<storm::expressions::SimpleValuation> Z3SmtSolver::solveAndDiversify(std::set<storm::expressions::SimpleValuation> diversifyers)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		uint_fast64_t Z3SmtSolver::solveAndDiversify(std::set<storm::expressions::SimpleValuation> diversifyers, std::function<bool(storm::expressions::Valuation&) > callback)
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	}
}