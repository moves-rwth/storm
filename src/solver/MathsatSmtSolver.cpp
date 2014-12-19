#include "src/solver/MathsatSmtSolver.h"

#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
	namespace solver {

#ifdef STORM_HAVE_MSAT
        MathsatSmtSolver::MathsatAllsatModelReference::MathsatAllsatModelReference(msat_env const& env, msat_term* model, std::unordered_map<std::string, uint_fast64_t> const& atomNameToSlotMapping) : env(env), model(model), atomNameToSlotMapping(atomNameToSlotMapping) {
            // Intentionally left empty.
        }
#endif
        bool MathsatSmtSolver::MathsatAllsatModelReference::getBooleanValue(std::string const& name) const {
            std::unordered_map<std::string, uint_fast64_t>::const_iterator nameSlotPair = atomNameToSlotMapping.find(name);
            STORM_LOG_THROW(nameSlotPair != atomNameToSlotMapping.end(), storm::exceptions::InvalidArgumentException, "Cannot retrieve value of unknown variable '" << name << "' from model.");
            msat_term selectedTerm = model[nameSlotPair->second];
            
            if (msat_term_is_not(env, selectedTerm)) {
                return false;
            } else {
                return true;
            }
        }
        
        int_fast64_t MathsatSmtSolver::MathsatAllsatModelReference::getIntegerValue(std::string const& name) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unable to retrieve integer value from model that only contains boolean values.");
        }
        
        double MathsatSmtSolver::MathsatAllsatModelReference::getDoubleValue(std::string const& name) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unable to retrieve double value from model that only contains boolean values.");
        }
        
#ifdef STORM_HAVE_MSAT
        MathsatSmtSolver::MathsatModelReference::MathsatModelReference(msat_env const& env, storm::adapters::MathsatExpressionAdapter& expressionAdapter) : env(env), expressionAdapter(expressionAdapter) {
            // Intentionally left empty.
        }
#endif
        bool MathsatSmtSolver::MathsatModelReference::getBooleanValue(std::string const& name) const {
            msat_term msatVariable = expressionAdapter.translateExpression(storm::expressions::Expression::createBooleanVariable(name), false);
            msat_term msatValue = msat_get_model_value(env, msatVariable);
            storm::expressions::Expression value = expressionAdapter.translateTerm(msatValue);
            STORM_LOG_THROW(value.hasBooleanReturnType(), storm::exceptions::InvalidArgumentException, "Unable to retrieve boolean value of non-boolean variable '" << name << "'.");
            return value.evaluateAsBool();
        }
        
        int_fast64_t MathsatSmtSolver::MathsatModelReference::getIntegerValue(std::string const& name) const {
            msat_term msatVariable = expressionAdapter.translateExpression(storm::expressions::Expression::createBooleanVariable(name), false);
            msat_term msatValue = msat_get_model_value(env, msatVariable);
            storm::expressions::Expression value = expressionAdapter.translateTerm(msatValue);
            STORM_LOG_THROW(value.hasIntegralReturnType(), storm::exceptions::InvalidArgumentException, "Unable to retrieve integer value of non-integer variable '" << name << "'.");
            return value.evaluateAsInt();
        }
        
        double MathsatSmtSolver::MathsatModelReference::getDoubleValue(std::string const& name) const {
            msat_term msatVariable = expressionAdapter.translateExpression(storm::expressions::Expression::createBooleanVariable(name), false);
            msat_term msatValue = msat_get_model_value(env, msatVariable);
            storm::expressions::Expression value = expressionAdapter.translateTerm(msatValue);
            STORM_LOG_THROW(value.hasIntegralReturnType(), storm::exceptions::InvalidArgumentException, "Unable to retrieve double value of non-double variable '" << name << "'.");
            return value.evaluateAsDouble();
        }
        
		MathsatSmtSolver::MathsatSmtSolver(Options const& options)
#ifdef STORM_HAVE_MSAT
			: expressionAdapter(nullptr), lastCheckAssumptions(false), lastResult(CheckResult::Unknown)
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
            STORM_LOG_THROW(!MSAT_ERROR_ENV(env), storm::exceptions::UnexpectedException, "Illegal MathSAT environment.");
			msat_destroy_env(env);
		}

		void MathsatSmtSolver::push() {
#ifdef STORM_HAVE_MSAT
			msat_push_backtrack_point(env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

		void MathsatSmtSolver::pop() {
#ifdef STORM_HAVE_MSAT
			msat_pop_backtrack_point(env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

        void MathsatSmtSolver::pop(uint_fast64_t n) {
            SmtSolver::pop(n);
        }
        
		void MathsatSmtSolver::reset()
		{
#ifdef STORM_HAVE_MSAT
			msat_reset_env(env);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

		void MathsatSmtSolver::add(storm::expressions::Expression const& e)
		{
#ifdef STORM_HAVE_MSAT
			msat_assert_formula(env, expressionAdapter->translateExpression(e, true));
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
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
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
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
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
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
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

		storm::expressions::SimpleValuation MathsatSmtSolver::getModelAsValuation()
		{
#ifdef STORM_HAVE_MSAT
			STORM_LOG_THROW(this->lastResult == SmtSolver::CheckResult::Sat, storm::exceptions::InvalidStateException, "Unable to create model for formula that was not determined to be satisfiable.");
			return this->convertMathsatModelToValuation();
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}
        
        std::shared_ptr<SmtSolver::ModelReference> MathsatSmtSolver::getModel() {
#ifdef STORM_HAVE_MSAT
			STORM_LOG_THROW(this->lastResult == SmtSolver::CheckResult::Sat, storm::exceptions::InvalidStateException, "Unable to create model for formula that was not determined to be satisfiable.");
            return std::shared_ptr<SmtSolver::ModelReference>(new MathsatModelReference(env, *expressionAdapter));
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
        }

#ifdef STORM_HAVE_MSAT
		storm::expressions::SimpleValuation MathsatSmtSolver::convertMathsatModelToValuation() {
			storm::expressions::SimpleValuation stormModel;

			msat_model_iterator modelIterator = msat_create_model_iterator(env);
            STORM_LOG_THROW(!MSAT_ERROR_MODEL_ITERATOR(modelIterator), storm::exceptions::UnexpectedException, "MathSat returned an illegal model iterator.");
            
			while (msat_model_iterator_has_next(modelIterator)) {
				msat_term t, v;
				msat_model_iterator_next(modelIterator, &t, &v);

				storm::expressions::Expression variableInterpretation = this->expressionAdapter->translateTerm(v);
				char* name = msat_decl_get_name(msat_term_get_decl(t));

				switch (variableInterpretation.getReturnType()) {
					case storm::expressions::ExpressionReturnType::Bool:
						stormModel.addBooleanIdentifier(std::string(name), variableInterpretation.evaluateAsBool());
						break;
					case storm::expressions::ExpressionReturnType::Int:
						stormModel.addIntegerIdentifier(std::string(name), variableInterpretation.evaluateAsInt());
						break;
					case storm::expressions::ExpressionReturnType::Double:
						stormModel.addDoubleIdentifier(std::string(name), variableInterpretation.evaluateAsDouble());
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
			this->allSat(important, [&valuations](storm::expressions::SimpleValuation const& valuation) -> bool { valuations.push_back(valuation); return true; });
			return valuations;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}


#ifdef STORM_HAVE_MSAT
		class AllsatValuationCallbackUserData {
		public:
			AllsatValuationCallbackUserData(msat_env& env, std::function<bool(storm::expressions::SimpleValuation&)> const& callback) : env(env), callback(callback) {
                // Intentionally left empty.
			}

            static int allsatValuationsCallback(msat_term* model, int size, void* user_data) {
                AllsatValuationCallbackUserData* user = reinterpret_cast<AllsatValuationCallbackUserData*>(user_data);
                
                storm::expressions::SimpleValuation valuation;
                for (int i = 0; i < size; ++i) {
                    bool currentTermValue = true;
                    msat_term currentTerm = model[i];
                    if (msat_term_is_not(user->env, currentTerm)) {
                        currentTerm = msat_term_get_arg(currentTerm, 0);
                        currentTermValue = false;
                    }
                    char* name = msat_decl_get_name(msat_term_get_decl(currentTerm));
                    std::string nameAsString(name);
                    msat_free(name);
                    valuation.addBooleanIdentifier(nameAsString, currentTermValue);
                }
                
                if (user->callback(valuation)) {
                    return 1;
                } else {
                    return 0;
                }
            }
            
        protected:
            // The MathSAT environment. It is used to retrieve the values of the atoms in a model.
			msat_env& env;
            
            // The function that is to be called when the MathSAT model has been translated to a valuation.
			std::function<bool(storm::expressions::SimpleValuation&)> const& callback;
		};
        
        class AllsatModelReferenceCallbackUserData {
        public:
            AllsatModelReferenceCallbackUserData(msat_env& env, std::unordered_map<std::string, uint_fast64_t> const& atomNameToSlotMapping, std::function<bool(storm::solver::SmtSolver::ModelReference&)> const& callback) : env(env), atomNameToSlotMapping(atomNameToSlotMapping), callback(callback) {
                // Intentionally left empty.
            }
            
            static int allsatModelReferenceCallback(msat_term* model, int size, void* user_data) {
                AllsatModelReferenceCallbackUserData* user = reinterpret_cast<AllsatModelReferenceCallbackUserData*>(user_data);
                MathsatSmtSolver::MathsatAllsatModelReference modelReference(user->env, model, user->atomNameToSlotMapping);
                if (user->callback(modelReference)) {
                    return 1;
                } else {
                    return 0;
                }
            }
            
        protected:
            // The MathSAT environment. It is used to retrieve the values of the atoms in a model.
            msat_env& env;
            
            // Store a mapping from the names of atoms to their slots in the model.
            std::unordered_map<std::string, uint_fast64_t> const& atomNameToSlotMapping;
            
            // The function that is to be called when the MathSAT model has been translated to a valuation.
            std::function<bool(storm::solver::SmtSolver::ModelReference&)> const& callback;
        };
#endif


		uint_fast64_t MathsatSmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(storm::expressions::SimpleValuation&)> const& callback)
		{
#ifdef STORM_HAVE_MSAT
            // Create a backtracking point, because MathSAT will modify the assertions stack during its AllSat procedure.
            this->push();
            
			std::vector<msat_term> msatImportant;
			msatImportant.reserve(important.size());

			for (storm::expressions::Expression const& atom : important) {
                STORM_LOG_THROW(atom.isVariable() && atom.hasBooleanReturnType(), storm::exceptions::InvalidArgumentException, "The important atoms for AllSat must be boolean variables.");
				msatImportant.push_back(expressionAdapter->translateExpression(atom));
			}

			AllsatValuationCallbackUserData allSatUserData(env, callback);
            int numberOfModels = msat_all_sat(env, msatImportant.data(), msatImportant.size(), &AllsatValuationCallbackUserData::allsatValuationsCallback, &allSatUserData);

            // Restore original assertion stack and return.
            this->pop();
			return static_cast<uint_fast64_t>(numberOfModels);
#else
			LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

		uint_fast64_t MathsatSmtSolver::allSat(std::vector<storm::expressions::Expression> const& important, std::function<bool(SmtSolver::ModelReference&)> const& callback)
		{
#ifdef STORM_HAVE_MSAT
            // Create a backtracking point, because MathSAT will modify the assertions stack during its AllSat procedure.
            this->push();
            
            std::vector<msat_term> msatImportant;
            msatImportant.reserve(important.size());
            std::unordered_map<std::string, uint_fast64_t> atomNameToSlotMapping;
            
            for (storm::expressions::Expression const& atom : important) {
                STORM_LOG_THROW(atom.isVariable() && atom.hasBooleanReturnType(), storm::exceptions::InvalidArgumentException, "The important atoms for AllSat must be boolean variables.");
                msatImportant.push_back(expressionAdapter->translateExpression(atom));
                atomNameToSlotMapping[atom.getIdentifier()] = msatImportant.size() - 1;
            }
            
            AllsatModelReferenceCallbackUserData allSatUserData(env, atomNameToSlotMapping, callback);
            int numberOfModels = msat_all_sat(env, msatImportant.data(), msatImportant.size(), &AllsatModelReferenceCallbackUserData::allsatModelReferenceCallback, &allSatUserData);
            
            // Restore original assertion stack and return.
            this->pop();
            return static_cast<uint_fast64_t>(numberOfModels);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

		std::vector<storm::expressions::Expression> MathsatSmtSolver::getUnsatAssumptions() {
#ifdef STORM_HAVE_MSAT
            STORM_LOG_THROW(lastResult == SmtSolver::CheckResult::Unsat, storm::exceptions::InvalidStateException, "Unable to generate unsatisfiable core of assumptions, because the last check did not determine the formulas to be unsatisfiable.")
            STORM_LOG_THROW(lastCheckAssumptions, storm::exceptions::InvalidStateException, "Unable to generate unsatisfiable core of assumptions, because the last check did not involve assumptions.");
            
			size_t numUnsatAssumpations;
			msat_term* msatUnsatAssumptions = msat_get_unsat_assumptions(env, &numUnsatAssumpations);

			std::vector<storm::expressions::Expression> unsatAssumptions;
			unsatAssumptions.reserve(numUnsatAssumpations);

			for (unsigned int i = 0; i < numUnsatAssumpations; ++i) {
				unsatAssumptions.push_back(this->expressionAdapter->translateTerm(msatUnsatAssumptions[i]));
			}

			return unsatAssumptions;
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

		void MathsatSmtSolver::setInterpolationGroup(uint_fast64_t group) {
#ifdef STORM_HAVE_MSAT
			auto groupIter = this->interpolationGroups.find(group);
			if (groupIter == this->interpolationGroups.end() ) {
				int newGroup = msat_create_itp_group(env);
				auto insertResult = this->interpolationGroups.insert(std::make_pair(group, newGroup));
				groupIter = insertResult.first;
			}
			msat_set_itp_group(env, groupIter->second);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}

		storm::expressions::Expression MathsatSmtSolver::getInterpolant(std::vector<uint_fast64_t> const& groupsA) {
#ifdef STORM_HAVE_MSAT
            STORM_LOG_THROW(lastResult == SmtSolver::CheckResult::Unsat, storm::exceptions::InvalidStateException, "Unable to generate interpolant, because the last check did not determine the formulas to be unsatisfiable.");
            STORM_LOG_THROW(!lastCheckAssumptions, storm::exceptions::InvalidStateException, "Unable to generate interpolant, because the last check for satisfiability involved assumptions.");

			std::vector<int> msatInterpolationGroupsA;
			msatInterpolationGroupsA.reserve(groupsA.size());
			for (auto groupOfA : groupsA) {
				auto groupIter = this->interpolationGroups.find(groupOfA);
                STORM_LOG_THROW(groupIter != this->interpolationGroups.end(), storm::exceptions::InvalidArgumentException, "Unable to generate interpolant, because an unknown interpolation group was referenced.");
				msatInterpolationGroupsA.push_back(groupIter->second);
			}
			msat_term interpolant = msat_get_interpolant(env, msatInterpolationGroupsA.data(), msatInterpolationGroupsA.size());

            STORM_LOG_THROW(!MSAT_ERROR_TERM(interpolant), storm::exceptions::UnexpectedException, "Unable to retrieve an interpolant.");
            
			return this->expressionAdapter->translateTerm(interpolant);
#else
			STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "StoRM is compiled without MathSAT support.");
#endif
		}
	}
}