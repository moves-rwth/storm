#include "src/solver/Z3SmtSolver.h"

#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
	namespace solver {
#ifdef STORM_HAVE_Z3
		Z3SmtSolver::Z3ModelReference::Z3ModelReference(z3::model const& model, storm::adapters::Z3ExpressionAdapter& expressionAdapter) : model(model), expressionAdapter(expressionAdapter) {
            // Intentionally left empty.
		}
#endif

		bool Z3SmtSolver::Z3ModelReference::getBooleanValue(std::string const& name) const {
#ifdef STORM_HAVE_Z3
			z3::expr z3Expr = this->expressionAdapter.translateExpression(storm::expressions::Expression::createBooleanVariable(name));
			z3::expr z3ExprValuation = model.eval(z3Expr, true);
			return this->expressionAdapter.translateExpression(z3ExprValuation).evaluateAsBool();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		int_fast64_t Z3SmtSolver::Z3ModelReference::getIntegerValue(std::string const& name) const {
#ifdef STORM_HAVE_Z3
			z3::expr z3Expr = this->expressionAdapter.translateExpression(storm::expressions::Expression::createIntegerVariable(name));
			z3::expr z3ExprValuation = model.eval(z3Expr, true);
			return this->expressionAdapter.translateExpression(z3ExprValuation).evaluateAsInt();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

        double Z3SmtSolver::Z3ModelReference::getDoubleValue(std::string const& name) const {
#ifdef STORM_HAVE_Z3
			z3::expr z3Expr = this->expressionAdapter.translateExpression(storm::expressions::Expression::createDoubleVariable(name));
			z3::expr z3ExprValuation = model.eval(z3Expr, true);
			return this->expressionAdapter.translateExpression(z3ExprValuation).evaluateAsDouble();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}
        
		Z3SmtSolver::Z3SmtSolver()
#ifdef STORM_HAVE_Z3
			: context(), solver(context), expressionAdapter(context), lastCheckAssumptions(false), lastResult(CheckResult::Unknown)
#endif
		{
			// Intentionally left empty.
		}
        
		Z3SmtSolver::~Z3SmtSolver() {
            // Intentionally left empty.
        }

		void Z3SmtSolver::push()
		{
#ifdef STORM_HAVE_Z3
			this->solver.push();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::pop()
		{
#ifdef STORM_HAVE_Z3
			this->solver.pop();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::pop(uint_fast64_t n)
		{
#ifdef STORM_HAVE_Z3
			this->solver.pop(static_cast<unsigned int>(n));
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::reset()
		{
#ifdef STORM_HAVE_Z3
			this->solver.reset();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::add(storm::expressions::Expression const& assertion)
		{
#ifdef STORM_HAVE_Z3
			this->solver.add(expressionAdapter.translateExpression(assertion, true));
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::check()
		{
#ifdef STORM_HAVE_Z3
			lastCheckAssumptions = false;
			switch (this->solver.check()) {
				case z3::sat:
					this->lastResult = SmtSolver::CheckResult::Sat;
					break;
				case z3::unsat:
					this->lastResult = SmtSolver::CheckResult::Unsat;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::Unknown;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions)
		{
#ifdef STORM_HAVE_Z3
			lastCheckAssumptions = true;
			z3::expr_vector z3Assumptions(this->context);

			for (storm::expressions::Expression assumption : assumptions) {
				z3Assumptions.push_back(this->expressionAdapter.translateExpression(assumption));
			}

			switch (this->solver.check(z3Assumptions)) {
				case z3::sat:
					this->lastResult = SmtSolver::CheckResult::Sat;
					break;
				case z3::unsat:
					this->lastResult = SmtSolver::CheckResult::Unsat;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::Unknown;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> assumptions)
		{
#ifdef STORM_HAVE_Z3
			lastCheckAssumptions = true;
			z3::expr_vector z3Assumptions(this->context);

			for (storm::expressions::Expression assumption : assumptions) {
				z3Assumptions.push_back(this->expressionAdapter.translateExpression(assumption));
			}

			switch (this->solver.check(z3Assumptions)) {
				case z3::sat:
					this->lastResult = SmtSolver::CheckResult::Sat;
					break;
				case z3::unsat:
					this->lastResult = SmtSolver::CheckResult::Unsat;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::Unknown;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		storm::expressions::SimpleValuation Z3SmtSolver::getModel()
		{
#ifdef STORM_HAVE_Z3
			STORM_LOG_THROW(this->lastResult == SmtSolver::CheckResult::Sat, storm::exceptions::InvalidStateException, "Unable to create model for formula that was not determined to be satisfiable.");
			return this->convertZ3ModelToValuation(this->solver.get_model());
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

#ifdef STORM_HAVE_Z3
		storm::expressions::SimpleValuation Z3SmtSolver::convertZ3ModelToValuation(z3::model const& model) {
			storm::expressions::SimpleValuation stormModel;

			for (unsigned i = 0; i < model.num_consts(); ++i) {
				z3::func_decl variableI = model.get_const_decl(i);
				storm::expressions::Expression variableIInterpretation = this->expressionAdapter.translateExpression(model.get_const_interp(variableI));

				switch (variableIInterpretation.getReturnType()) {
					case storm::expressions::ExpressionReturnType::Bool:
						stormModel.addBooleanIdentifier(variableI.name().str(), variableIInterpretation.evaluateAsBool());
						break;
					case storm::expressions::ExpressionReturnType::Int:
						stormModel.addIntegerIdentifier(variableI.name().str(), variableIInterpretation.evaluateAsInt());
						break;
					case storm::expressions::ExpressionReturnType::Double:
						stormModel.addDoubleIdentifier(variableI.name().str(), variableIInterpretation.evaluateAsDouble());
						break;
					default:
						STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Variable interpretation in model is not of type bool, int or double.")
							break;
				}

			}

			return stormModel;
		}
#endif

		std::vector<storm::expressions::SimpleValuation> Z3SmtSolver::allSat(std::vector<storm::expressions::Expression> const& important)
		{
#ifdef STORM_HAVE_Z3
			std::vector<storm::expressions::SimpleValuation> valuations;
			this->allSat(important, [&valuations](storm::expressions::SimpleValuation& valuation) -> bool { valuations.push_back(valuation); return true; });
			return valuations;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		uint_fast64_t Z3SmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> const& callback)
		{
#ifdef STORM_HAVE_Z3
			for (storm::expressions::Expression const& atom : important) {
                STORM_LOG_THROW(atom.isVariable(), storm::exceptions::InvalidArgumentException, "The important atoms for AllSat must be atoms, i.e. variables.");
			}

			uint_fast64_t numberOfModels = 0;
			bool proceed = true;

            // Save the current assertion stack, to be able to backtrack after the procedure.
			this->push();

            // Enumerate models as long as the conjunction is satisfiable and the callback has not aborted the enumeration.
			while (proceed && this->check() == CheckResult::Sat) {
				++numberOfModels;
				z3::model model = this->solver.get_model();

				z3::expr modelExpr = this->context.bool_val(true);
				storm::expressions::SimpleValuation valuation;

				for (storm::expressions::Expression const& importantAtom : important) {
					z3::expr z3ImportantAtom = this->expressionAdapter.translateExpression(importantAtom);
					z3::expr z3ImportantAtomValuation = model.eval(z3ImportantAtom, true);
					modelExpr = modelExpr && (z3ImportantAtom == z3ImportantAtomValuation);
					if (importantAtom.getReturnType() == storm::expressions::ExpressionReturnType::Bool) {
						valuation.addBooleanIdentifier(importantAtom.getIdentifier(), this->expressionAdapter.translateExpression(z3ImportantAtomValuation).evaluateAsBool());
					} else if (importantAtom.getReturnType() == storm::expressions::ExpressionReturnType::Int) {
						valuation.addIntegerIdentifier(importantAtom.getIdentifier(), this->expressionAdapter.translateExpression(z3ImportantAtomValuation).evaluateAsInt());
					} else if (importantAtom.getReturnType() == storm::expressions::ExpressionReturnType::Double) {
						valuation.addDoubleIdentifier(importantAtom.getIdentifier(), this->expressionAdapter.translateExpression(z3ImportantAtomValuation).evaluateAsDouble());
					} else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Important atom has invalid type.");
					}
				}

                // Check if we are required to proceed, and if so rule out the current model.
				proceed = callback(valuation);
                if (proceed) {
                    this->solver.add(!modelExpr);
                }
			}

            // Restore the old assertion stack and return.
			this->pop();
			return numberOfModels;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		uint_fast64_t Z3SmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(SmtSolver::ModelReference&)> const& callback)
		{
#ifdef STORM_HAVE_Z3
			for (storm::expressions::Expression const& atom : important) {
                STORM_LOG_THROW(atom.isVariable(), storm::exceptions::InvalidArgumentException, "The important atoms for AllSat must be atoms, i.e. variables.");
			}

			uint_fast64_t numberOfModels = 0;
			bool proceed = true;

            // Save the current assertion stack, to be able to backtrack after the procedure.
			this->push();

            // Enumerate models as long as the conjunction is satisfiable and the callback has not aborted the enumeration.
			while (proceed && this->check() == CheckResult::Sat) {
				++numberOfModels;
				z3::model model = this->solver.get_model();

				z3::expr modelExpr = this->context.bool_val(true);
				storm::expressions::SimpleValuation valuation;

				for (storm::expressions::Expression const& importantAtom : important) {
					z3::expr z3ImportantAtom = this->expressionAdapter.translateExpression(importantAtom);
					z3::expr z3ImportantAtomValuation = model.eval(z3ImportantAtom, true);
					modelExpr = modelExpr && (z3ImportantAtom == z3ImportantAtomValuation);
				}
				Z3ModelReference modelRef(model, expressionAdapter);

                // Check if we are required to proceed, and if so rule out the current model.
				proceed = callback(modelRef);
                if (proceed) {
                    this->solver.add(!modelExpr);
                }
			}

			this->pop();
			return numberOfModels;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}

		std::vector<storm::expressions::Expression> Z3SmtSolver::getUnsatAssumptions() {
#ifdef STORM_HAVE_Z3
            STORM_LOG_THROW(lastResult == SmtSolver::CheckResult::Unsat, storm::exceptions::InvalidStateException, "Unable to generate unsatisfiable core of assumptions, because the last check did not determine the formulas to be unsatisfiable.")
            STORM_LOG_THROW(lastCheckAssumptions, storm::exceptions::InvalidStateException, "Unable to generate unsatisfiable core of assumptions, because the last check did not involve assumptions.");

			z3::expr_vector z3UnsatAssumptions = this->solver.unsat_core();
			std::vector<storm::expressions::Expression> unsatAssumptions;
			
			for (unsigned int i = 0; i < z3UnsatAssumptions.size(); ++i) {
				unsatAssumptions.push_back(this->expressionAdapter.translateExpression(z3UnsatAssumptions[i]));
			}

			return unsatAssumptions;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without Z3 support.");
#endif
		}
	}
}