#ifndef STORM_SOLVER_MATHSATSMTSOLVER
#define STORM_SOLVER_MATHSATSMTSOLVER

#include "storm-config.h"
#include "src/solver/SmtSolver.h"
#include "src/adapters/MathsatExpressionAdapter.h"
#include <boost/container/flat_map.hpp>

#ifndef STORM_HAVE_MSAT
#define STORM_HAVE_MSAT
#endif

#ifdef STORM_HAVE_MSAT
#include "mathsat.h"
#endif

namespace storm {
	namespace solver {
		class MathsatSmtSolver : public SmtSolver {
		public:
            /*!
             * A class that captures options that may be passed to the Mathsat solver. Settings these options has some
             * implications due to the implementation of MathSAT. For example, enabling interpolation means that the
             * solver must not be used for checking satisfiable formulas.
             */
			class Options {
            public:
                Options() : enableModelGeneration(false), enableUnsatCoreGeneration(false), enableInterpolantGeneration(false) {
                    // Intentionally left empty.
                }
                
                Options(bool enableModelGeneration, bool enableUnsatCoreGeneration, bool enableInterpolantGeneration) : enableModelGeneration(enableModelGeneration), enableUnsatCoreGeneration(enableUnsatCoreGeneration), enableInterpolantGeneration(enableInterpolantGeneration) {
                    // Intentionally left empty.
                }
                
                bool enableModelGeneration = false;
                bool enableUnsatCoreGeneration = false;
                bool enableInterpolantGeneration = false;
			};

			class MathSatModelReference : public SmtSolver::ModelReference {
			public:
#ifdef STORM_HAVE_MSAT
				MathSatModelReference(msat_env const& env, msat_term* model, std::unordered_map<std::string, uint_fast64_t> const& atomNameToSlotMapping);
#endif
				virtual bool getBooleanValue(std::string const& name) const override;
				virtual int_fast64_t getIntegerValue(std::string const& name) const override;
                virtual double getDoubleValue(std::string const& name) const override;
                
			private:
#ifdef STORM_HAVE_MSAT
				msat_env const& env;
                msat_term* model;
                std::unordered_map<std::string, uint_fast64_t> const& atomNameToSlotMapping;
#endif
			};
            
			MathsatSmtSolver(Options const& options = Options());
            
			virtual ~MathsatSmtSolver();

			virtual void push() override;

			virtual void pop() override;

            virtual void pop(uint_fast64_t n) override;
            
			virtual void reset() override;

			virtual void add(storm::expressions::Expression const& assertion) override;

			virtual CheckResult check() override;

			virtual CheckResult checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions) override;

			virtual CheckResult checkWithAssumptions(std::initializer_list<storm::expressions::Expression> const& assumptions) override;

			virtual storm::expressions::SimpleValuation getModel() override;

			virtual std::vector<storm::expressions::SimpleValuation> allSat(std::vector<storm::expressions::Expression> const& important) override;

			virtual uint_fast64_t allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> const& callback) override;

			virtual uint_fast64_t allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(ModelReference&)> const& callback) override;

			virtual std::vector<storm::expressions::Expression> getUnsatAssumptions() override;

			virtual void setInterpolationGroup(uint_fast64_t group) override;

			virtual storm::expressions::Expression getInterpolant(std::vector<uint_fast64_t> const& groupsA) override;

		private:
#ifdef STORM_HAVE_MSAT
			storm::expressions::SimpleValuation convertMathsatModelToValuation();

            // The MathSAT environment.
			msat_env env;
            
            // The expression adapter used to translate expressions to MathSAT's format. This has to be a pointer, since
            // it must be initialized after creating the environment, but the adapter class has no default constructor.
            std::unique_ptr<storm::adapters::MathsatExpressionAdapter> expressionAdapter;

            // A flag storing whether the last call was a check involving assumptions.
			bool lastCheckAssumptions;
            
            // The result of the last call to any of the check methods.
			CheckResult lastResult;
            
            // A mapping of interpolation group indices to their MathSAT identifier.
			typedef	boost::container::flat_map<uint_fast64_t, int> InterpolationGroupMapType;
			InterpolationGroupMapType interpolationGroups;
#endif
		};
	}
}
#endif // STORM_SOLVER_MATHSATSMTSOLVER