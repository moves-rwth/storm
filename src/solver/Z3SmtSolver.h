#ifndef STORM_SOLVER_Z3SMTSOLVER
#define STORM_SOLVER_Z3SMTSOLVER

#include "storm-config.h"
#include "src/solver/SmtSolver.h"
#include "src/adapters/Z3ExpressionAdapter.h"

#ifdef STORM_HAVE_Z3
#include "z3++.h"
#include "z3.h"
#endif

namespace storm {
	namespace solver {
		class Z3SmtSolver : public SmtSolver {
		public:
			class Z3ModelReference : public SmtSolver::ModelReference {
			public:
#ifdef STORM_HAVE_Z3
				Z3ModelReference(z3::model& m, storm::adapters::Z3ExpressionAdapter &adapter);
#endif
				virtual bool getBooleanValue(std::string const& name) const override;
				virtual int_fast64_t getIntegerValue(std::string const& name) const override;
			private:
#ifdef STORM_HAVE_Z3
				z3::model &m_model;
				storm::adapters::Z3ExpressionAdapter &m_adapter;
#endif
			};
		public:
			Z3SmtSolver(Options options = Options::ModelGeneration);
			virtual ~Z3SmtSolver();

			virtual void push() override;

			virtual void pop() override;

			virtual void pop(uint_fast64_t n) override;

			virtual void reset() override;

			virtual void assertExpression(storm::expressions::Expression const& e) override;

			virtual CheckResult check() override;

			virtual CheckResult checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions) override;

			virtual CheckResult checkWithAssumptions(std::initializer_list<storm::expressions::Expression> assumptions) override;

			virtual storm::expressions::SimpleValuation getModel() override;

			virtual std::vector<storm::expressions::SimpleValuation> allSat(std::vector<storm::expressions::Expression> const& important) override;

			virtual uint_fast64_t allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> callback) override;

			virtual uint_fast64_t allSat(std::function<bool(ModelReference&)> callback, std::vector<storm::expressions::Expression> const& important) override;

			virtual std::vector<storm::expressions::Expression> getUnsatAssumptions() override;

		protected:
#ifdef STORM_HAVE_Z3
			virtual storm::expressions::SimpleValuation z3ModelToStorm(z3::model m);
#endif
		private:

#ifdef STORM_HAVE_Z3
			z3::context m_context;
			z3::solver m_solver;
			storm::adapters::Z3ExpressionAdapter m_adapter;

			bool lastCheckAssumptions;
			CheckResult lastResult;
#endif
		};
	}
}
#endif // STORM_SOLVER_Z3SMTSOLVER