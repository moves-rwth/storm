#include "src/solver/Z3SmtSolver.h"


namespace storm {
	namespace solver {
#ifdef STORM_HAVE_Z3
		Z3SmtSolver::Z3ModelReference::Z3ModelReference(z3::model &m, storm::adapters::Z3ExpressionAdapter &adapter) : m_model(m), m_adapter(adapter) {

		}
#endif

		bool Z3SmtSolver::Z3ModelReference::getBooleanValue(std::string const& name) const {
#ifdef STORM_HAVE_Z3
			z3::expr z3Expr = this->m_adapter.translateExpression(storm::expressions::Expression::createBooleanVariable(name));
			z3::expr z3ExprValuation = m_model.eval(z3Expr, true);
			return this->m_adapter.translateExpression(z3ExprValuation).evaluateAsBool();
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		int_fast64_t Z3SmtSolver::Z3ModelReference::getIntegerValue(std::string const& name) const {
#ifdef STORM_HAVE_Z3
			z3::expr z3Expr = this->m_adapter.translateExpression(storm::expressions::Expression::createIntegerVariable(name));
			z3::expr z3ExprValuation = m_model.eval(z3Expr, true);
			return this->m_adapter.translateExpression(z3ExprValuation).evaluateAsInt();
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		Z3SmtSolver::Z3SmtSolver(Options options)
#ifdef STORM_HAVE_Z3
			: m_context()
			, m_solver(m_context)
			, m_adapter(m_context, std::map<std::string, z3::expr>())
			, lastCheckAssumptions(false)
            , lastResult(CheckResult::UNKNOWN)
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
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::pop()
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.pop();
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::pop(uint_fast64_t n)
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.pop((unsigned int)n);
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::reset()
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.reset();
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		void Z3SmtSolver::assertExpression(storm::expressions::Expression const& e)
		{
#ifdef STORM_HAVE_Z3
			this->m_solver.add(m_adapter.translateExpression(e, true));
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::check()
		{
#ifdef STORM_HAVE_Z3
			lastCheckAssumptions = false;
			switch (this->m_solver.check()) {
				case z3::sat:
					this->lastResult = SmtSolver::CheckResult::SAT;
					break;
				case z3::unsat:
					this->lastResult = SmtSolver::CheckResult::UNSAT;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::UNKNOWN;
					break;
			}
			return this->lastResult;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions)
		{
#ifdef STORM_HAVE_Z3
			lastCheckAssumptions = true;
			z3::expr_vector z3Assumptions(this->m_context);

			for (storm::expressions::Expression assumption : assumptions) {
				z3Assumptions.push_back(this->m_adapter.translateExpression(assumption));
			}

			switch (this->m_solver.check(z3Assumptions)) {
				case z3::sat:
					this->lastResult = SmtSolver::CheckResult::SAT;
					break;
				case z3::unsat:
					this->lastResult = SmtSolver::CheckResult::UNSAT;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::UNKNOWN;
					break;
			}
			return this->lastResult;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		SmtSolver::CheckResult Z3SmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> assumptions)
		{
#ifdef STORM_HAVE_Z3
			lastCheckAssumptions = true;
			z3::expr_vector z3Assumptions(this->m_context);

			for (storm::expressions::Expression assumption : assumptions) {
				z3Assumptions.push_back(this->m_adapter.translateExpression(assumption));
			}

			switch (this->m_solver.check(z3Assumptions)) {
				case z3::sat:
					this->lastResult = SmtSolver::CheckResult::SAT;
					break;
				case z3::unsat:
					this->lastResult = SmtSolver::CheckResult::UNSAT;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::UNKNOWN;
					break;
			}
			return this->lastResult;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		storm::expressions::SimpleValuation Z3SmtSolver::getModel()
		{
#ifdef STORM_HAVE_Z3
			
			LOG_THROW(this->lastResult == SmtSolver::CheckResult::SAT, storm::exceptions::InvalidStateException, "Requested Model but last check result was not SAT.");

			return this->z3ModelToStorm(this->m_solver.get_model());
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

#ifdef STORM_HAVE_Z3
		storm::expressions::SimpleValuation Z3SmtSolver::z3ModelToStorm(z3::model m) {
			storm::expressions::SimpleValuation stormModel;

			for (unsigned i = 0; i < m.num_consts(); ++i) {
				z3::func_decl var_i = m.get_const_decl(i);
				storm::expressions::Expression var_i_interp = this->m_adapter.translateExpression(m.get_const_interp(var_i));

				switch (var_i_interp.getReturnType()) {
					case storm::expressions::ExpressionReturnType::Bool:
						stormModel.addBooleanIdentifier(var_i.name().str(), var_i_interp.evaluateAsBool());
						break;
					case storm::expressions::ExpressionReturnType::Int:
						stormModel.addIntegerIdentifier(var_i.name().str(), var_i_interp.evaluateAsInt());
						break;
					case storm::expressions::ExpressionReturnType::Double:
						stormModel.addDoubleIdentifier(var_i.name().str(), var_i_interp.evaluateAsDouble());
						break;
					default:
						LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Variable interpretation in model is not of type bool, int or double.")
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

			this->allSat(important, [&valuations](storm::expressions::SimpleValuation& valuation) -> bool {valuations.push_back(valuation); return true; });

			return valuations;

#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		uint_fast64_t Z3SmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> callback)
		{
#ifdef STORM_HAVE_Z3
			for (storm::expressions::Expression e : important) {
				if (!e.isVariable()) {
					throw storm::exceptions::InvalidArgumentException() << "The important expressions for AllSat must be atoms, i.e. variable expressions.";
				}
			}

			uint_fast64_t numModels = 0;
			bool proceed = true;

			this->push();

			while (proceed && this->check() == CheckResult::SAT) {
				++numModels;
				z3::model m = this->m_solver.get_model();

				z3::expr modelExpr = this->m_context.bool_val(true);
				storm::expressions::SimpleValuation valuation;

				for (storm::expressions::Expression importantAtom : important) {
					z3::expr z3ImportantAtom = this->m_adapter.translateExpression(importantAtom);
					z3::expr z3ImportantAtomValuation = m.eval(z3ImportantAtom, true);
					modelExpr = modelExpr && (z3ImportantAtom == z3ImportantAtomValuation);
					if (importantAtom.getReturnType() == storm::expressions::ExpressionReturnType::Bool) {
						valuation.addBooleanIdentifier(importantAtom.getIdentifier(), this->m_adapter.translateExpression(z3ImportantAtomValuation).evaluateAsBool());
					} else if (importantAtom.getReturnType() == storm::expressions::ExpressionReturnType::Int) {
						valuation.addIntegerIdentifier(importantAtom.getIdentifier(), this->m_adapter.translateExpression(z3ImportantAtomValuation).evaluateAsInt());
					} else if (importantAtom.getReturnType() == storm::expressions::ExpressionReturnType::Double) {
						valuation.addDoubleIdentifier(importantAtom.getIdentifier(), this->m_adapter.translateExpression(z3ImportantAtomValuation).evaluateAsDouble());
					} else {
						throw storm::exceptions::InvalidTypeException() << "Important atom has invalid type";
					}
				}

				proceed = callback(valuation);

				this->m_solver.add(!modelExpr);
			}

			this->pop();

			return numModels;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		uint_fast64_t Z3SmtSolver::allSat(std::function<bool(SmtSolver::ModelReference&)> callback, std::vector<storm::expressions::Expression> const& important)
		{
#ifdef STORM_HAVE_Z3
			for (storm::expressions::Expression e : important) {
				if (!e.isVariable()) {
					throw storm::exceptions::InvalidArgumentException() << "The important expressions for AllSat must be atoms, i.e. variable expressions.";
				}
			}

			uint_fast64_t numModels = 0;
			bool proceed = true;

			this->push();

			while (proceed && this->check() == CheckResult::SAT) {
				++numModels;
				z3::model m = this->m_solver.get_model();

				z3::expr modelExpr = this->m_context.bool_val(true);
				storm::expressions::SimpleValuation valuation;

				for (storm::expressions::Expression importantAtom : important) {
					z3::expr z3ImportantAtom = this->m_adapter.translateExpression(importantAtom);
					z3::expr z3ImportantAtomValuation = m.eval(z3ImportantAtom, true);
					modelExpr = modelExpr && (z3ImportantAtom == z3ImportantAtomValuation);
				}

				proceed = callback(Z3ModelReference(m, m_adapter));

				this->m_solver.add(!modelExpr);
			}

			this->pop();

			return numModels;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}

		std::vector<storm::expressions::Expression> Z3SmtSolver::getUnsatAssumptions() {
#ifdef STORM_HAVE_Z3
			if (lastResult != SmtSolver::CheckResult::UNSAT) {
				throw storm::exceptions::InvalidStateException() << "Unsat Assumptions was called but last state is not unsat.";
			}
			if (!lastCheckAssumptions) {
				throw storm::exceptions::InvalidStateException() << "Unsat Assumptions was called but last check had no assumptions.";
			}

			z3::expr_vector z3UnsatAssumptions = this->m_solver.unsat_core();

			std::vector<storm::expressions::Expression> unsatAssumptions;
			
			for (unsigned int i = 0; i < z3UnsatAssumptions.size(); ++i) {
				unsatAssumptions.push_back(this->m_adapter.translateExpression(z3UnsatAssumptions[i]));
			}

			return unsatAssumptions;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without Z3 support.");
#endif
		}
	}
}