#include "src/solver/MathsatSmtSolver.h"

#include <vector>

#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
	namespace solver {

		MathsatSmtSolver::MathsatSmtSolver(Options const& options)
#ifdef STORM_HAVE_MSAT
			: lastCheckAssumptions(false), lastResult(CheckResult::Unknown)
#endif
		{
#ifdef STORM_HAVE_MSAT
			msat_config config = msat_create_config();
			if (options.enableInterpolantGeneration) {
				msat_set_option(config, "interpolation", "true");
			}
            if (options.enableModelGeneration) {
                msat_set_option(config, "model_generation", "true");
            }
            if (options.enableUnsatCoreGeneration) {
                msat_set_option(config, "unsat_core_generation", "true");
            }
            STORM_LOG_THROW(!MSAT_ERROR_CONFIG(config), storm::exceptions::UnexpectedException, "Unable to create Mathsat configuration.");
            
            // Based on the configuration, build the environment, check for errors and destroy the configuration.
			env = msat_create_env(config);
            STORM_LOG_THROW(!MSAT_ERROR_ENV(env), storm::exceptions::UnexpectedException, "Unable to create Mathsat environment.");
            msat_destroy_config(config);
            
			expressionAdapter = std::unique_ptr<storm::adapters::MathsatExpressionAdapter>(new storm::adapters::MathsatExpressionAdapter(env));
#endif
		}
        
		MathsatSmtSolver::~MathsatSmtSolver() {
            STORM_LOG_THROW(MSAT_ERROR_ENV(env), storm::exceptions::UnexpectedException, "Illegal MathSAT environment.");
			msat_destroy_env(env);
		}

		void MathsatSmtSolver::push() {
#ifdef STORM_HAVE_MSAT
			msat_push_backtrack_point(env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathsatSmtSolver::pop() {
#ifdef STORM_HAVE_MSAT
			msat_pop_backtrack_point(env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathsatSmtSolver::reset()
		{
#ifdef STORM_HAVE_MSAT
			msat_reset_env(env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathsatSmtSolver::add(storm::expressions::Expression const& e)
		{
#ifdef STORM_HAVE_MSAT
			msat_assert_formula(env, expressionAdapter->translateExpression(e, true));
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		SmtSolver::CheckResult MathsatSmtSolver::check()
		{
#ifdef STORM_HAVE_MSAT
			lastCheckAssumptions = false;
			switch (msat_solve(env)) {
				case MSAT_SAT:
					this->lastResult = SmtSolver::CheckResult::Sat;
					break;
				case MSAT_UNSAT:
					this->lastResult = SmtSolver::CheckResult::Unsat;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::Unknown;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		SmtSolver::CheckResult MathsatSmtSolver::checkWithAssumptions(std::set<storm::expressions::Expression> const& assumptions)
		{
#ifdef STORM_HAVE_MSAT
			lastCheckAssumptions = true;
			std::vector<msat_term> mathSatAssumptions;
			mathSatAssumptions.reserve(assumptions.size());

			for (storm::expressions::Expression assumption : assumptions) {
				mathSatAssumptions.push_back(this->expressionAdapter->translateExpression(assumption));
			}

			switch (msat_solve_with_assumptions(env, mathSatAssumptions.data(), mathSatAssumptions.size())) {
				case MSAT_SAT:
					this->lastResult = SmtSolver::CheckResult::Sat;
					break;
				case MSAT_UNSAT:
					this->lastResult = SmtSolver::CheckResult::Unsat;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::Unknown;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		SmtSolver::CheckResult MathsatSmtSolver::checkWithAssumptions(std::initializer_list<storm::expressions::Expression> const& assumptions)
		{
#ifdef STORM_HAVE_MSAT
			lastCheckAssumptions = true;
			std::vector<msat_term> mathSatAssumptions;
			mathSatAssumptions.reserve(assumptions.size());

			for (storm::expressions::Expression assumption : assumptions) {
				mathSatAssumptions.push_back(this->expressionAdapter->translateExpression(assumption));
			}

			switch (msat_solve_with_assumptions(env, mathSatAssumptions.data(), mathSatAssumptions.size())) {
				case MSAT_SAT:
					this->lastResult = SmtSolver::CheckResult::Sat;
					break;
				case MSAT_UNSAT:
					this->lastResult = SmtSolver::CheckResult::Unsat;
					break;
				default:
					this->lastResult = SmtSolver::CheckResult::Unknown;
					break;
			}
			return this->lastResult;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		storm::expressions::SimpleValuation MathsatSmtSolver::getModel()
		{
#ifdef STORM_HAVE_MSAT
			
			STORM_LOG_THROW(this->lastResult == SmtSolver::CheckResult::Sat, storm::exceptions::InvalidStateException, "Requested Model but last check result was not SAT.");
			return this->convertMathsatModelToValuation();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

#ifdef STORM_HAVE_MSAT
		storm::expressions::SimpleValuation MathsatSmtSolver::convertMathsatModelToValuation() {
			storm::expressions::SimpleValuation stormModel;

			msat_model_iterator model = msat_create_model_iterator(env);

            STORM_LOG_THROW(!MSAT_ERROR_MODEL_ITERATOR(model), storm::exceptions::UnexpectedException, "MathSat returned an illegal model iterator.");
            
			while (msat_model_iterator_has_next(model)) {
				msat_term t, v;
				msat_model_iterator_next(model, &t, &v);

				storm::expressions::Expression var_i_interp = this->expressionAdapter->translateTerm(v);
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

		std::vector<storm::expressions::SimpleValuation> MathsatSmtSolver::allSat(std::vector<storm::expressions::Expression> const& important)
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
				storm::adapters::MathsatExpressionAdapter &adapter,
				std::function<bool(storm::expressions::SimpleValuation&)> const& callback)
				: env(env), adapter(adapter), callback(callback) {
			}
			msat_env &env;
			storm::adapters::MathsatExpressionAdapter &adapter;
			std::function<bool(storm::expressions::SimpleValuation&)> const& callback;
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


		uint_fast64_t MathsatSmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> const& callback)
		{
#ifdef STORM_HAVE_MSAT
			std::vector<msat_term> msatImportant;
			msatImportant.reserve(important.size());

			for (storm::expressions::Expression e : important) {
				if (!e.isVariable()) {
					throw storm::exceptions::InvalidArgumentException() << "The important expressions for AllSat must be atoms, i.e. variable expressions.";
				}
				msatImportant.push_back(expressionAdapter->translateExpression(e));
			}

			AllsatValuationsCallbackUserData allSatUserData(env, *expressionAdapter, callback);
			int numModels = msat_all_sat(env, msatImportant.data(), msatImportant.size(), &allsatValuationsCallback, &allSatUserData);

			return numModels;
#else
			LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		uint_fast64_t MathsatSmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(SmtSolver::ModelReference&)> const& callback)
		{
#ifdef STORM_HAVE_MSAT
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Not Implemented.");
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		std::vector<storm::expressions::Expression> MathsatSmtSolver::getUnsatAssumptions() {
#ifdef STORM_HAVE_MSAT
			if (lastResult != SmtSolver::CheckResult::Unsat) {
				throw storm::exceptions::InvalidStateException() << "Unsat Assumptions was called but last state is not unsat.";
			}
			if (!lastCheckAssumptions) {
				throw storm::exceptions::InvalidStateException() << "Unsat Assumptions was called but last check had no assumptions.";
			}

			size_t numUnsatAssumpations;
			msat_term* msatUnsatAssumptions = msat_get_unsat_assumptions(env, &numUnsatAssumpations);

			std::vector<storm::expressions::Expression> unsatAssumptions;
			unsatAssumptions.reserve(numUnsatAssumpations);

			for (unsigned int i = 0; i < numUnsatAssumpations; ++i) {
				unsatAssumptions.push_back(this->expressionAdapter->translateTerm(msatUnsatAssumptions[i]));
			}

			return unsatAssumptions;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		void MathsatSmtSolver::setInterpolationGroup(uint_fast64_t group) {
#ifdef STORM_HAVE_MSAT
			auto groupIter = this->interpolationGroups.find(group);
			if( groupIter == this->interpolationGroups.end() ) {
				int newGroup = msat_create_itp_group(env);
				auto insertResult = this->interpolationGroups.insert(std::make_pair(group, newGroup));
				if (!insertResult.second) {
					throw storm::exceptions::InvalidStateException() << "Internal error in MathSAT wrapper: Unable to insert newly created interpolation group.";
				}
				groupIter = insertResult.first;
			}
			msat_set_itp_group(env, groupIter->second);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}

		storm::expressions::Expression MathsatSmtSolver::getInterpolant(std::vector<uint_fast64_t> const& groupsA) {
#ifdef STORM_HAVE_MSAT
			if (lastResult != SmtSolver::CheckResult::Unsat) {
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
			msat_term interpolant = msat_get_interpolant(env, msatInterpolationGroupsA.data(), msatInterpolationGroupsA.size());

            STORM_LOG_THROW(!MSAT_ERROR_TERM(interpolant), storm::exceptions::UnexpectedException, "Unable to retrieve an interpolant.");
            
			return this->expressionAdapter->translateTerm(interpolant);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "StoRM is compiled without MathSat support.");
#endif
		}
	}
}