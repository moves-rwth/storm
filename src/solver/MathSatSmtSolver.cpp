#include "src/solver/MathSatSmtSolver.h"

#include <vector>

#include "src/exceptions/UnexpectedException.h"

namespace storm {
	namespace solver {

		MathSatSmtSolver::MathSatSmtSolver(Options options)
#ifdef STORM_HAVE_MSAT
			: lastCheckAssumptions(false)
            , lastResult(CheckResult::UNKNOWN)
#endif
		{
#ifdef STORM_HAVE_MSAT
			m_cfg = msat_create_config();
			
			if (static_cast<int>(options)& static_cast<int>(Options::InterpolantComputation)) {
				msat_set_option(m_cfg, "interpolation", "true");
			}
            
            msat_set_option(m_cfg, "model_generation", "true");
            
			m_env = msat_create_env(m_cfg);

			m_adapter = new storm::adapters::MathSatExpressionAdapter(m_env, variableToDeclMap);
#endif
		}
		MathSatSmtSolver::~MathSatSmtSolver() {
			msat_destroy_env(m_env);
			msat_destroy_config(m_cfg);
		};

		void MathSatSmtSolver::push()
		{
#ifdef STORM_HAVE_MSAT
			msat_push_backtrack_point(m_env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathSatSmtSolver::pop()
		{
#ifdef STORM_HAVE_MSAT
			msat_pop_backtrack_point(m_env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathSatSmtSolver::pop(uint_fast64_t n)
		{
#ifdef STORM_HAVE_MSAT
			for (uint_fast64_t i = 0; i < n; ++i) {
				this->pop();
			}
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathSatSmtSolver::reset()
		{
#ifdef STORM_HAVE_MSAT
			msat_reset_env(m_env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathSatSmtSolver::assertExpression(storm::expressions::Expression const& e)
		{
#ifdef STORM_HAVE_MSAT
			msat_assert_formula(m_env, m_adapter->translateExpression(e, true));
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		SmtSolver::CheckResult MathSatSmtSolver::check()
		{
#ifdef STORM_HAVE_MSAT
			lastCheckAssumptions = false;
			switch (msat_solve(m_env)) {
				case MSAT_SAT:
					this->lastResult = SmtSolver::CheckResult::SAT;
					break;
				case MSAT_UNSAT:
					this->lastResult = SmtSolver::CheckResult::UNSAT;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::UNKNOWN;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		SmtSolver::CheckResult MathSatSmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions)
		{
#ifdef STORM_HAVE_MSAT
			lastCheckAssumptions = true;
			std::vector<msat_term> mathSatAssumptions;
			mathSatAssumptions.reserve(assumptions.size());

			for (storm::expressions::Expression assumption : assumptions) {
				mathSatAssumptions.push_back(this->m_adapter->translateExpression(assumption));
			}

			switch (msat_solve_with_assumptions(m_env, mathSatAssumptions.data(), mathSatAssumptions.size())) {
				case MSAT_SAT:
					this->lastResult = SmtSolver::CheckResult::SAT;
					break;
				case MSAT_UNSAT:
					this->lastResult = SmtSolver::CheckResult::UNSAT;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::UNKNOWN;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		SmtSolver::CheckResult MathSatSmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> assumptions)
		{
#ifdef STORM_HAVE_MSAT
			lastCheckAssumptions = true;
			std::vector<msat_term> mathSatAssumptions;
			mathSatAssumptions.reserve(assumptions.size());

			for (storm::expressions::Expression assumption : assumptions) {
				mathSatAssumptions.push_back(this->m_adapter->translateExpression(assumption));
			}

			switch (msat_solve_with_assumptions(m_env, mathSatAssumptions.data(), mathSatAssumptions.size())) {
				case MSAT_SAT:
					this->lastResult = SmtSolver::CheckResult::SAT;
					break;
				case MSAT_UNSAT:
					this->lastResult = SmtSolver::CheckResult::UNSAT;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::UNKNOWN;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		storm::expressions::SimpleValuation MathSatSmtSolver::getModel()
		{
#ifdef STORM_HAVE_MSAT
			
			STORM_LOG_THROW(this->lastResult == SmtSolver::CheckResult::SAT, storm::exceptions::InvalidStateException, "Requested Model but last check result was not SAT.");

			return this->MathSatModelToStorm();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

#ifdef STORM_HAVE_MSAT
		storm::expressions::SimpleValuation MathSatSmtSolver::MathSatModelToStorm() {
			storm::expressions::SimpleValuation stormModel;

			msat_model_iterator model = msat_create_model_iterator(m_env);

            STORM_LOG_THROW(!MSAT_ERROR_MODEL_ITERATOR(model), storm::exceptions::UnexpectedException, "MathSat returned an illegal model iterator.");
            
			while (msat_model_iterator_has_next(model)) {
				msat_term t, v;
				msat_model_iterator_next(model, &t, &v);

				storm::expressions::Expression var_i_interp = this->m_adapter->translateTerm(v);
				char* name = msat_decl_get_name(msat_term_get_decl(t));

				switch (var_i_interp.getReturnType()) {
					case storm::expressions::ExpressionReturnType::Bool:
						
						stormModel.addBooleanIdentifier(std::string(name), var_i_interp.evaluateAsBool());
						break;
					case storm::expressions::ExpressionReturnType::Int:
						stormModel.addIntegerIdentifier(std::string(name), var_i_interp.evaluateAsInt());
						break;
					case storm::expressions::ExpressionReturnType::Double:
						stormModel.addDoubleIdentifier(std::string(name), var_i_interp.evaluateAsDouble());
						break;
					default:
						STORM_LOG_THROW(false, storm::exceptions::ExpressionEvaluationException, "Variable interpretation in model is not of type bool, int or double.")
							break;
				}

				msat_free(name);

			}

			return stormModel;
		}
#endif

		std::vector<storm::expressions::SimpleValuation> MathSatSmtSolver::allSat(std::vector<storm::expressions::Expression> const& important)
		{
#ifdef STORM_HAVE_MSAT
			
			std::vector<storm::expressions::SimpleValuation> valuations;

			this->allSat(important, [&valuations](storm::expressions::SimpleValuation& valuation) -> bool {valuations.push_back(valuation); return true; });

			return valuations;

#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}


#ifdef STORM_HAVE_MSAT
		class AllsatValuationsCallbackUserData {
		public:
			AllsatValuationsCallbackUserData(msat_env &env,
				storm::adapters::MathSatExpressionAdapter &adapter,
				std::function<bool(storm::expressions::SimpleValuation&)> &callback)
				: env(env), adapter(adapter), callback(callback) {
			}
			msat_env &env;
			storm::adapters::MathSatExpressionAdapter &adapter;
			std::function<bool(storm::expressions::SimpleValuation&)> &callback;
		};

		int allsatValuationsCallback(msat_term *model, int size, void *user_data) {
			AllsatValuationsCallbackUserData* user = reinterpret_cast<AllsatValuationsCallbackUserData*>(user_data);

			storm::expressions::SimpleValuation valuation;

			for (int i = 0; i < size; ++i) {
				bool currentTermValue = true;
				msat_term currentTerm = model[i];
				if (msat_term_is_not(user->env, currentTerm)) {
					currentTerm = msat_term_get_arg(currentTerm, 0);
					currentTermValue = false;
				}
				char* name = msat_decl_get_name(msat_term_get_decl(currentTerm));
				std::string name_str(name);
				valuation.addBooleanIdentifier(name_str, currentTermValue);
				msat_free(name);
			}

			if (user->callback(valuation)) {
				return 1;
			} else {
				return 0;
			}
		}
#endif


		uint_fast64_t MathSatSmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> callback)
		{
#ifdef STORM_HAVE_MSAT
			std::vector<msat_term> msatImportant;
			msatImportant.reserve(important.size());

			for (storm::expressions::Expression e : important) {
				if (!e.isVariable()) {
					throw storm::exceptions::InvalidArgumentException() << "The important expressions for AllSat must be atoms, i.e. variable expressions.";
				}
				msatImportant.push_back(m_adapter->translateExpression(e));
			}

			AllsatValuationsCallbackUserData allSatUserData(m_env, *m_adapter, callback);
			int numModels = msat_all_sat(m_env, msatImportant.data(), msatImportant.size(), &allsatValuationsCallback, &allSatUserData);

			return numModels;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		uint_fast64_t MathSatSmtSolver::allSat(std::function<bool(SmtSolver::ModelReference&)> callback, std::vector<storm::expressions::Expression> const& important)
		{
#ifdef STORM_HAVE_MSAT
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not Implemented.");
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		std::vector<storm::expressions::Expression> MathSatSmtSolver::getUnsatAssumptions() {
#ifdef STORM_HAVE_MSAT
			if (lastResult != SmtSolver::CheckResult::UNSAT) {
				throw storm::exceptions::InvalidStateException() << "Unsat Assumptions was called but last state is not unsat.";
			}
			if (!lastCheckAssumptions) {
				throw storm::exceptions::InvalidStateException() << "Unsat Assumptions was called but last check had no assumptions.";
			}

			size_t numUnsatAssumpations;
			msat_term* msatUnsatAssumptions = msat_get_unsat_assumptions(m_env, &numUnsatAssumpations);

			std::vector<storm::expressions::Expression> unsatAssumptions;
			unsatAssumptions.reserve(numUnsatAssumpations);

			for (unsigned int i = 0; i < numUnsatAssumpations; ++i) {
				unsatAssumptions.push_back(this->m_adapter->translateTerm(msatUnsatAssumptions[i]));
			}

			return unsatAssumptions;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathSatSmtSolver::setInterpolationGroup(uint_fast64_t group) {
#ifdef STORM_HAVE_MSAT
			auto groupIter = this->interpolationGroups.find(group);
			if( groupIter == this->interpolationGroups.end() ) {
				int newGroup = msat_create_itp_group(m_env);
				auto insertResult = this->interpolationGroups.insert(std::make_pair(group, newGroup));
				if (!insertResult.second) {
					throw storm::exceptions::InvalidStateException() << "Internal error in MathSAT wrapper: Unable to insert newly created interpolation group.";
				}
				groupIter = insertResult.first;
			}
			msat_set_itp_group(m_env, groupIter->second);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		storm::expressions::Expression MathSatSmtSolver::getInterpolant(std::vector<uint_fast64_t> groupsA) {
#ifdef STORM_HAVE_MSAT
			if (lastResult != SmtSolver::CheckResult::UNSAT) {
				throw storm::exceptions::InvalidStateException() << "getInterpolant was called but last state is not unsat.";
			}
			if (lastCheckAssumptions) {
				throw storm::exceptions::InvalidStateException() << "getInterpolant was called but last check had assumptions.";
			}

			std::vector<int> msatInterpolationGroupsA;
			msatInterpolationGroupsA.reserve(groupsA.size());
			for (auto groupOfA : groupsA) {
				auto groupIter = this->interpolationGroups.find(groupOfA);
				if (groupIter == this->interpolationGroups.end()) {
					throw storm::exceptions::InvalidArgumentException() << "Requested interpolant for non existing interpolation group " << groupOfA;
				}
				msatInterpolationGroupsA.push_back(groupIter->second);
			}
			msat_term interpolant = msat_get_interpolant(m_env, msatInterpolationGroupsA.data(), msatInterpolationGroupsA.size());

			return this->m_adapter->translateTerm(interpolant);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}
	}
}