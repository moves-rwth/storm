#ifndef STORM_SOLVER_Z3SMTSOLVER
#define STORM_SOLVER_Z3SMTSOLVER

#include "storm-config.h"
#include "src/solver/SmtSolver.h"
#include "src/adapters/Z3ExpressionAdapter.h"

#include "z3++.h"
#include "z3.h"

namespace storm {
	namespace solver {
		class Z3SmtSolver : public SmtSolver {
		public:
			Z3SmtSolver(Options options = Options::ModelGeneration);
			virtual ~Z3SmtSolver();

			virtual void push();

			virtual void pop();

			virtual void pop(uint_fast64_t n);

			virtual void reset();

			virtual void assertExpression(storm::expressions::Expression &e);

			virtual CheckResult check();

			virtual CheckResult checkWithAssumptions(std::set<storm::expressions::Expression> &assumptions);

			virtual CheckResult checkWithAssumptions(std::initializer_list<storm::expressions::Expression> assumptions);

			virtual storm::expressions::SimpleValuation getModel();

			virtual std::set<storm::expressions::SimpleValuation> solveAndDiversify(std::set<storm::expressions::SimpleValuation> diversifyers);

			virtual uint_fast64_t solveAndDiversify(std::set<storm::expressions::SimpleValuation> diversifyers, std::function<bool(storm::expressions::Valuation&) > callback);

		private:

#ifdef STORM_HAVE_Z3
			z3::context m_context;
			z3::solver m_solver;
			storm::adapters::Z3ExpressionAdapter m_adapter;
#endif
		};
	}
}
#endif // STORM_SOLVER_Z3SMTSOLVER