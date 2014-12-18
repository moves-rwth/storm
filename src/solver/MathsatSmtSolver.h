#ifndef STORM_SOLVER_MATHSATSMTSOLVER
#define STORM_SOLVER_MATHSATSMTSOLVER

#include "storm-config.h"
#include "src/solver/SmtSolver.h"
#include "src/adapters/MathSatExpressionAdapter.h"
#include <boost/container/flat_map.hpp>

#ifndef STORM_HAVE_MSAT
#define STORM_HAVE_MSAT
#endif

#ifdef STORM_HAVE_MSAT
#include "mathsat.h"
#endif

namespace storm {
	namespace solver {
		class MathSatSmtSolver : public SmtSolver {
		public:
            /*!
             * A class that captures options that may be passed to Mathsat solver.
             */
			class Options {
                bool enableModelGeneration = false;
                bool enableUnsatCoreGeneration = false;
                bool enableInterpolantGeneration = false;
			};

			class MathSatModelReference : public SmtSolver::ModelReference {
			public:
#ifdef STORM_HAVE_MSAT
				MathSatModelReference(msat_env& env, storm::adapters::MathSatExpressionAdapter &adapter);
#endif
				virtual bool getBooleanValue(std::string const& name) const override;
				virtual int_fast64_t getIntegerValue(std::string const& name) const override;
			private:
#ifdef STORM_HAVE_MSAT
				msat_env& env;
				storm::adapters::MathSatExpressionAdapter &m_adapter;
#endif
			};
		public:
			MathSatSmtSolver(Options options = Options::ModelGeneration);
			virtual ~MathSatSmtSolver();

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

			virtual void setInterpolationGroup(uint_fast64_t group) override;

			virtual storm::expressions::Expression getInterpolant(std::vector<uint_fast64_t> groupsA) override;

		protected:
#ifdef STORM_HAVE_MSAT
			virtual storm::expressions::SimpleValuation MathSatModelToStorm();
#endif
		private:

#ifdef STORM_HAVE_MSAT
			msat_env m_env;
			storm::adapters::MathSatExpressionAdapter *m_adapter;

			bool lastCheckAssumptions;
			CheckResult lastResult;
			typedef	boost::container::flat_map<uint_fast64_t, int> InterpolationGroupMap;
			InterpolationGroupMap interpolationGroups;
			std::map<std::string, msat_decl> variableToDeclMap;
#endif
		};
	}
}
#endif // STORM_SOLVER_MATHSATSMTSOLVER