#include "src/settings/modules/CoreSettings.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/SettingMemento.h"
#include "src/settings/Option.h"
#include "src/settings/OptionBuilder.h"
#include "src/settings/ArgumentBuilder.h"
#include "src/settings/Argument.h"
#include "src/solver/SolverSelectionOptions.h"

#include "src/storage/dd/DdType.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string CoreSettings::moduleName = "core";
            const std::string CoreSettings::counterexampleOptionName = "counterexample";
            const std::string CoreSettings::counterexampleOptionShortName = "cex";
            const std::string CoreSettings::dontFixDeadlockOptionName = "nofixdl";
            const std::string CoreSettings::dontFixDeadlockOptionShortName = "ndl";
            const std::string CoreSettings::eqSolverOptionName = "eqsolver";
            const std::string CoreSettings::lpSolverOptionName = "lpsolver";
            const std::string CoreSettings::smtSolverOptionName = "smtsolver";
            const std::string CoreSettings::statisticsOptionName = "statistics";
            const std::string CoreSettings::statisticsOptionShortName = "stats";
            const std::string CoreSettings::engineOptionName = "engine";
            const std::string CoreSettings::engineOptionShortName = "e";
            const std::string CoreSettings::ddLibraryOptionName = "ddlib";
            const std::string CoreSettings::cudaOptionName = "cuda";
			const std::string CoreSettings::minMaxEquationSolvingTechniqueOptionName = "ndmethod";
            
            CoreSettings::CoreSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, counterexampleOptionName, false, "Generates a counterexample for the given PRCTL formulas if not satisfied by the model")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to which the counterexample is to be written.").setDefaultValueString("-").setIsOptional(true).build()).setShortName(counterexampleOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, dontFixDeadlockOptionName, false, "If the model contains deadlock states, they need to be fixed by setting this option.").setShortName(dontFixDeadlockOptionShortName).build());
                
                std::vector<std::string> engines = {"sparse", "hybrid", "dd", "expl", "abs"};
                this->addOption(storm::settings::OptionBuilder(moduleName, engineOptionName, false, "Sets which engine is used for model building and model checking.").setShortName(engineOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the engine to use. Available are {sparse, hybrid, dd, expl, abs}.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(engines)).setDefaultValueString("sparse").build()).build());
                
                std::vector<std::string> linearEquationSolver = {"gmm++", "native", "eigen", "elimination"};
                this->addOption(storm::settings::OptionBuilder(moduleName, eqSolverOptionName, false, "Sets which solver is preferred for solving systems of linear equations.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++, native, eigen, elimination.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(linearEquationSolver)).setDefaultValueString("gmm++").build()).build());
                
                std::vector<std::string> ddLibraries = {"cudd", "sylvan"};
                this->addOption(storm::settings::OptionBuilder(moduleName, ddLibraryOptionName, false, "Sets which library is preferred for decision-diagram operations.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the library to prefer. Available are: cudd and sylvan.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(ddLibraries)).setDefaultValueString("cudd").build()).build());
                
                std::vector<std::string> lpSolvers = {"gurobi", "glpk"};
                this->addOption(storm::settings::OptionBuilder(moduleName, lpSolverOptionName, false, "Sets which LP solver is preferred.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an LP solver. Available are: gurobi and glpk.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(lpSolvers)).setDefaultValueString("glpk").build()).build());
                std::vector<std::string> smtSolvers = {"z3", "mathsat"};
                this->addOption(storm::settings::OptionBuilder(moduleName, smtSolverOptionName, false, "Sets which SMT solver is preferred.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an SMT solver. Available are: z3 and mathsat.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(smtSolvers)).setDefaultValueString("z3").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, statisticsOptionName, false, "Sets whether to display statistics if available.").setShortName(statisticsOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, cudaOptionName, false, "Sets whether to use CUDA to speed up computation time.").build());

				std::vector<std::string> minMaxSolvingTechniques = {"vi", "value-iteration", "pi", "policy-iteration"};
				this->addOption(storm::settings::OptionBuilder(moduleName, minMaxEquationSolvingTechniqueOptionName, false, "Sets which min/max linear equation solving technique is preferred.")
					.addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of a min/max linear equation solving technique. Available are: value-iteration (vi) and policy-iteration (pi).").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(minMaxSolvingTechniques)).setDefaultValueString("vi").build()).build());
            }

            bool CoreSettings::isCounterexampleSet() const {
                return this->getOption(counterexampleOptionName).getHasOptionBeenSet();
            }
            
            std::string CoreSettings::getCounterexampleFilename() const {
                return this->getOption(counterexampleOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool CoreSettings::isDontFixDeadlocksSet() const {
                return this->getOption(dontFixDeadlockOptionName).getHasOptionBeenSet();
            }
            
            std::unique_ptr<storm::settings::SettingMemento> CoreSettings::overrideDontFixDeadlocksSet(bool stateToSet) {
                return this->overrideOption(dontFixDeadlockOptionName, stateToSet);
            }
            
            storm::solver::EquationSolverType  CoreSettings::getEquationSolver() const {
                std::string equationSolverName = this->getOption(eqSolverOptionName).getArgumentByName("name").getValueAsString();
                if (equationSolverName == "gmm++") {
                    return storm::solver::EquationSolverType::Gmmxx;
                } else if (equationSolverName == "native") {
                    return storm::solver::EquationSolverType::Native;
                } else if (equationSolverName == "eigen") {
                    return storm::solver::EquationSolverType::Eigen;
                } else if (equationSolverName == "elimination") {
                    return storm::solver::EquationSolverType::Elimination;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown equation solver '" << equationSolverName << "'.");
            }
            
            bool CoreSettings::isEquationSolverSet() const {
                return this->getOption(eqSolverOptionName).getHasOptionBeenSet();
            }
            
            storm::solver::LpSolverType CoreSettings::getLpSolver() const {
                std::string lpSolverName = this->getOption(lpSolverOptionName).getArgumentByName("name").getValueAsString();
                if (lpSolverName == "gurobi") {
                    return storm::solver::LpSolverType::Gurobi;
                } else if (lpSolverName == "glpk") {
                    return storm::solver::LpSolverType::Glpk;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown LP solver '" << lpSolverName << "'.");
            }
            
            storm::solver::SmtSolverType CoreSettings::getSmtSolver() const {
                std::string smtSolverName = this->getOption(smtSolverOptionName).getArgumentByName("name").getValueAsString();
                if (smtSolverName == "z3") {
                    return storm::solver::SmtSolverType::Z3;
                } else if (smtSolverName == "mathsat") {
                    return storm::solver::SmtSolverType::Mathsat;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown SMT solver '" << smtSolverName << "'.");
            }
            
            storm::dd::DdType CoreSettings::getDdLibraryType() const {
                std::string ddLibraryAsString = this->getOption(ddLibraryOptionName).getArgumentByName("name").getValueAsString();
                if (ddLibraryAsString == "sylvan") {
                    return storm::dd::DdType::Sylvan;
                } else {
                    return storm::dd::DdType::CUDD;
                }
            }
            
            bool CoreSettings::isShowStatisticsSet() const {
                return this->getOption(statisticsOptionName).getHasOptionBeenSet();
            }
            
            bool CoreSettings::isCudaSet() const {
                return this->getOption(cudaOptionName).getHasOptionBeenSet();
            }
            
            CoreSettings::Engine CoreSettings::getEngine() const {
                return engine;
            }

            void CoreSettings::setEngine(Engine newEngine) {
                this->engine = newEngine;
            }
            
            storm::solver::MinMaxTechnique CoreSettings::getMinMaxEquationSolvingTechnique() const {
				std::string minMaxEquationSolvingTechnique = this->getOption(minMaxEquationSolvingTechniqueOptionName).getArgumentByName("name").getValueAsString();
				if (minMaxEquationSolvingTechnique == "value-iteration" || minMaxEquationSolvingTechnique == "vi") {
					return storm::solver::MinMaxTechnique::ValueIteration;
				} else if (minMaxEquationSolvingTechnique == "policy-iteration" || minMaxEquationSolvingTechnique == "pi") {
					return storm::solver::MinMaxTechnique::PolicyIteration;
				}
				STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown min/max equation solving technique '" << minMaxEquationSolvingTechnique << "'.");
			}

			bool CoreSettings::isMinMaxEquationSolvingTechniqueSet() const {
				return this->getOption(minMaxEquationSolvingTechniqueOptionName).getHasOptionBeenSet();
			}
            
            void CoreSettings::finalize() {
                // Finalize engine.
                std::string engineStr = this->getOption(engineOptionName).getArgumentByName("name").getValueAsString();
                if (engineStr == "sparse") {
                    engine =  CoreSettings::Engine::Sparse;
                } else if (engineStr == "hybrid") {
                    engine = CoreSettings::Engine::Hybrid;
                } else if (engineStr == "dd") {
                    engine = CoreSettings::Engine::Dd;
                } else if (engineStr == "expl") {
                    engine = CoreSettings::Engine::Exploration;
                } else if (engineStr == "abs") {
                    engine = CoreSettings::Engine::AbstractionRefinement;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown engine '" << engineStr << "'.");
                }
            }

            bool CoreSettings::check() const {
                return true;
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
