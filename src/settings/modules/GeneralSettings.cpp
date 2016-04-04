#include "src/settings/modules/GeneralSettings.h"

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
            
            const std::string GeneralSettings::moduleName = "general";
            const std::string GeneralSettings::helpOptionName = "help";
            const std::string GeneralSettings::helpOptionShortName = "h";
            const std::string GeneralSettings::versionOptionName = "version";
            const std::string GeneralSettings::verboseOptionName = "verbose";
            const std::string GeneralSettings::verboseOptionShortName = "v";
            const std::string GeneralSettings::precisionOptionName = "precision";
            const std::string GeneralSettings::precisionOptionShortName = "eps";
            const std::string GeneralSettings::exportDotOptionName = "exportdot";
            const std::string GeneralSettings::exportMatOptionName = "exportmat";
            const std::string GeneralSettings::configOptionName = "config";
            const std::string GeneralSettings::configOptionShortName = "c";
            const std::string GeneralSettings::explicitOptionName = "explicit";
            const std::string GeneralSettings::explicitOptionShortName = "exp";
            const std::string GeneralSettings::symbolicOptionName = "symbolic";
            const std::string GeneralSettings::symbolicOptionShortName = "s";
            const std::string GeneralSettings::explorationOrderOptionName = "explorder";
            const std::string GeneralSettings::explorationOrderOptionShortName = "eo";
            const std::string GeneralSettings::propertyOptionName = "prop";
            const std::string GeneralSettings::propertyOptionShortName = "prop";
            const std::string GeneralSettings::transitionRewardsOptionName = "transrew";
            const std::string GeneralSettings::stateRewardsOptionName = "staterew";
            const std::string GeneralSettings::choiceLabelingOptionName = "choicelab";
            const std::string GeneralSettings::counterexampleOptionName = "counterexample";
            const std::string GeneralSettings::counterexampleOptionShortName = "cex";
            const std::string GeneralSettings::dontFixDeadlockOptionName = "nofixdl";
            const std::string GeneralSettings::dontFixDeadlockOptionShortName = "ndl";
            const std::string GeneralSettings::timeoutOptionName = "timeout";
            const std::string GeneralSettings::timeoutOptionShortName = "t";
            const std::string GeneralSettings::eqSolverOptionName = "eqsolver";
            const std::string GeneralSettings::lpSolverOptionName = "lpsolver";
            const std::string GeneralSettings::smtSolverOptionName = "smtsolver";
            const std::string GeneralSettings::constantsOptionName = "constants";
            const std::string GeneralSettings::constantsOptionShortName = "const";
            const std::string GeneralSettings::statisticsOptionName = "statistics";
            const std::string GeneralSettings::statisticsOptionShortName = "stats";
            const std::string GeneralSettings::bisimulationOptionName = "bisimulation";
            const std::string GeneralSettings::bisimulationOptionShortName = "bisim";
            const std::string GeneralSettings::engineOptionName = "engine";
            const std::string GeneralSettings::engineOptionShortName = "e";
            const std::string GeneralSettings::ddLibraryOptionName = "ddlib";
            const std::string GeneralSettings::cudaOptionName = "cuda";
            const std::string GeneralSettings::prismCompatibilityOptionName = "prismcompat";
            const std::string GeneralSettings::prismCompatibilityOptionShortName = "pc";
			const std::string GeneralSettings::minMaxEquationSolvingTechniqueOptionName = "minMaxEquationSolvingTechnique";
            
#ifdef STORM_HAVE_CARL
            const std::string GeneralSettings::parametricOptionName = "parametric";
#endif

            GeneralSettings::GeneralSettings(storm::settings::SettingsManager& settingsManager) : ModuleSettings(settingsManager, moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, helpOptionName, false, "Shows all available options, arguments and descriptions.").setShortName(helpOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("hint", "A regular expression to show help for all matching entities or 'all' for the complete help.").setDefaultValueString("all").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, prismCompatibilityOptionName, false, "Enables PRISM compatibility. This may be necessary to process some PRISM models.").setShortName(prismCompatibilityOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, versionOptionName, false, "Prints the version information.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, verboseOptionName, false, "Enables more verbose output.").setShortName(verboseOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, false, "The internally used precision.").setShortName(precisionOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to use.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportDotOptionName, "", "If given, the loaded model will be written to the specified file in the dot format.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to which the model is to be written.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportMatOptionName, "", "If given, the loaded model will be written to the specified file in the mat format.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "the name of the file to which the model is to be writen.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, configOptionName, false, "If given, this file will be read and parsed for additional configuration settings.").setShortName(configOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the configuration.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, explicitOptionName, false, "Parses the model given in an explicit (sparse) representation.").setShortName(explicitOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("transition filename", "The name of the file from which to read the transitions.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build())
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("labeling filename", "The name of the file from which to read the state labeling.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, symbolicOptionName, false, "Parses the model given in a symbolic representation.").setShortName(symbolicOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file from which to read the symbolic model.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());

                std::vector<std::string> explorationOrders = {"dfs", "bfs"};
                this->addOption(storm::settings::OptionBuilder(moduleName, explorationOrderOptionName, false, "Sets which exploration order to use.").setShortName(explorationOrderOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the exploration order to choose. Available are: dfs and bfs.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(explorationOrders)).setDefaultValueString("bfs").build()).build());

                this->addOption(storm::settings::OptionBuilder(moduleName, propertyOptionName, false, "Specifies the formulas to be checked on the model.").setShortName(propertyOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("formula or filename", "The formula or the file containing the formulas.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, counterexampleOptionName, false, "Generates a counterexample for the given PRCTL formulas if not satisfied by the model")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The name of the file to which the counterexample is to be written.").setDefaultValueString("-").setIsOptional(true).build()).setShortName(counterexampleOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, bisimulationOptionName, false, "Sets whether to perform bisimulation minimization.").setShortName(bisimulationOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, transitionRewardsOptionName, false, "If given, the transition rewards are read from this file and added to the explicit model. Note that this requires the model to be given as an explicit model (i.e., via --" + explicitOptionName + ").")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The file from which to read the transition rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, stateRewardsOptionName, false, "If given, the state rewards are read from this file and added to the explicit model. Note that this requires the model to be given as an explicit model (i.e., via --" + explicitOptionName + ").")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The file from which to read the state rewards.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, choiceLabelingOptionName, false, "If given, the choice labels are read from this file and added to the explicit model. Note that this requires the model to be given as an explicit model (i.e., via --" + explicitOptionName + ").")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("filename", "The file from which to read the choice labels.").addValidationFunctionString(storm::settings::ArgumentValidators::existingReadableFileValidator()).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, dontFixDeadlockOptionName, false, "If the model contains deadlock states, they need to be fixed by setting this option.").setShortName(dontFixDeadlockOptionShortName).build());
                
                std::vector<std::string> engines = {"sparse", "hybrid", "dd", "abs"};
                this->addOption(storm::settings::OptionBuilder(moduleName, engineOptionName, false, "Sets which engine is used for model building and model checking.").setShortName(engineOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the engine to use. Available are {sparse, hybrid, dd, ar}.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(engines)).setDefaultValueString("sparse").build()).build());
                
                std::vector<std::string> linearEquationSolver = {"gmm++", "native"};
                this->addOption(storm::settings::OptionBuilder(moduleName, eqSolverOptionName, false, "Sets which solver is preferred for solving systems of linear equations.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the solver to prefer. Available are: gmm++ and native.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(linearEquationSolver)).setDefaultValueString("gmm++").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, timeoutOptionName, false, "If given, computation will abort after the timeout has been reached.").setShortName(timeoutOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("time", "The number of seconds after which to timeout.").setDefaultValueUnsignedInteger(0).build()).build());
                
                std::vector<std::string> ddLibraries = {"cudd", "sylvan"};
                this->addOption(storm::settings::OptionBuilder(moduleName, ddLibraryOptionName, false, "Sets which library is preferred for decision-diagram operations.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the library to prefer. Available are: cudd and sylvan.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(ddLibraries)).setDefaultValueString("cudd").build()).build());
                
                std::vector<std::string> lpSolvers = {"gurobi", "glpk"};
                this->addOption(storm::settings::OptionBuilder(moduleName, lpSolverOptionName, false, "Sets which LP solver is preferred.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an LP solver. Available are: gurobi and glpk.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(lpSolvers)).setDefaultValueString("glpk").build()).build());
                std::vector<std::string> smtSolvers = {"z3", "mathsat"};
                this->addOption(storm::settings::OptionBuilder(moduleName, smtSolverOptionName, false, "Sets which SMT solver is preferred.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of an SMT solver. Available are: z3 and mathsat.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(smtSolvers)).setDefaultValueString("z3").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, constantsOptionName, false, "Specifies the constant replacements to use in symbolic models. Note that Note that this requires the model to be given as an symbolic model (i.e., via --" + symbolicOptionName + ").").setShortName(constantsOptionShortName)
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("values", "A comma separated list of constants and their value, e.g. a=1,b=2,c=3.").setDefaultValueString("").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, statisticsOptionName, false, "Sets whether to display statistics if available.").setShortName(statisticsOptionShortName).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, cudaOptionName, false, "Sets whether to use CUDA to speed up computation time.").build());

				std::vector<std::string> minMaxSolvingTechniques = {"policyIteration", "valueIteration"};
				this->addOption(storm::settings::OptionBuilder(moduleName, minMaxEquationSolvingTechniqueOptionName, false, "Sets which min/max linear equation solving technique is preferred.")
					.addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of a min/max linear equation solving technique. Available are: valueIteration and policyIteration.").addValidationFunctionString(storm::settings::ArgumentValidators::stringInListValidator(minMaxSolvingTechniques)).setDefaultValueString("valueIteration").build()).build());

#ifdef STORM_HAVE_CARL
                this->addOption(storm::settings::OptionBuilder(moduleName, parametricOptionName, false, "Sets whether to use the parametric engine.").build());
#endif
            }
            
            bool GeneralSettings::isHelpSet() const {
                return this->getOption(helpOptionName).getHasOptionBeenSet();
            }
            
            bool GeneralSettings::isVersionSet() const {
                return this->getOption(versionOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getHelpModuleName() const {
                return this->getOption(helpOptionName).getArgumentByName("hint").getValueAsString();
            }
            
            bool GeneralSettings::isVerboseSet() const {
                return this->getOption(verboseOptionName).getHasOptionBeenSet();
            }
            
            double GeneralSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool GeneralSettings::isExportDotSet() const {
                return this->getOption(exportDotOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getExportDotFilename() const {
                return this->getOption(exportDotOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isConfigSet() const {
                return this->getOption(configOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getConfigFilename() const {
                return this->getOption(configOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isExplicitSet() const {
                return this->getOption(explicitOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getTransitionFilename() const {
                return this->getOption(explicitOptionName).getArgumentByName("transition filename").getValueAsString();
            }
                        
            std::string GeneralSettings::getLabelingFilename() const {
                return this->getOption(explicitOptionName).getArgumentByName("labeling filename").getValueAsString();
            }
            
            bool GeneralSettings::isSymbolicSet() const {
                return this->getOption(symbolicOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getSymbolicModelFilename() const {
                return this->getOption(symbolicOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isExplorationOrderSet() const {
                return this->getOption(explorationOrderOptionName).getHasOptionBeenSet();
            }
            
            storm::builder::ExplorationOrder GeneralSettings::getExplorationOrder() const {
                std::string explorationOrderAsString = this->getOption(explorationOrderOptionName).getArgumentByName("name").getValueAsString();
                if (explorationOrderAsString == "dfs") {
                    return storm::builder::ExplorationOrder::Dfs;
                } else if (explorationOrderAsString == "bfs") {
                    return storm::builder::ExplorationOrder::Bfs;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown exploration order '" << explorationOrderAsString << "'.");
            }
            
            bool GeneralSettings::isPropertySet() const {
                return this->getOption(propertyOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getProperty() const {
                return this->getOption(propertyOptionName).getArgumentByName("formula or filename").getValueAsString();
            }
            
            bool GeneralSettings::isTransitionRewardsSet() const {
                return this->getOption(transitionRewardsOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getTransitionRewardsFilename() const {
                return this->getOption(transitionRewardsOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isStateRewardsSet() const {
                return this->getOption(stateRewardsOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getStateRewardsFilename() const {
                return this->getOption(stateRewardsOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isChoiceLabelingSet() const {
                return this->getOption(choiceLabelingOptionName).getHasOptionBeenSet();
            }
                
            std::string GeneralSettings::getChoiceLabelingFilename() const {
                return this->getOption(choiceLabelingOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isCounterexampleSet() const {
                return this->getOption(counterexampleOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getCounterexampleFilename() const {
                return this->getOption(counterexampleOptionName).getArgumentByName("filename").getValueAsString();
            }
            
            bool GeneralSettings::isDontFixDeadlocksSet() const {
                return this->getOption(dontFixDeadlockOptionName).getHasOptionBeenSet();
            }
            
            std::unique_ptr<storm::settings::SettingMemento> GeneralSettings::overrideDontFixDeadlocksSet(bool stateToSet) {
                return this->overrideOption(dontFixDeadlockOptionName, stateToSet);
            }
            
            std::unique_ptr<storm::settings::SettingMemento> GeneralSettings::overridePrismCompatibilityMode(bool stateToSet) {
                return this->overrideOption(prismCompatibilityOptionName, stateToSet);
            }
            
            bool GeneralSettings::isTimeoutSet() const {
                return this->getOption(timeoutOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t GeneralSettings::getTimeoutInSeconds() const {
                return this->getOption(timeoutOptionName).getArgumentByName("time").getValueAsUnsignedInteger();
            }
            
            storm::solver::EquationSolverType  GeneralSettings::getEquationSolver() const {
                std::string equationSolverName = this->getOption(eqSolverOptionName).getArgumentByName("name").getValueAsString();
                if (equationSolverName == "gmm++") {
                    return storm::solver::EquationSolverType::Gmmxx;
                } else if (equationSolverName == "native") {
                    return storm::solver::EquationSolverType::Native;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown equation solver '" << equationSolverName << "'.");
            }
            
            bool GeneralSettings::isEquationSolverSet() const {
                return this->getOption(eqSolverOptionName).getHasOptionBeenSet();
            }
            
            storm::solver::LpSolverType GeneralSettings::getLpSolver() const {
                std::string lpSolverName = this->getOption(lpSolverOptionName).getArgumentByName("name").getValueAsString();
                if (lpSolverName == "gurobi") {
                    return storm::solver::LpSolverType::Gurobi;
                } else if (lpSolverName == "glpk") {
                    return storm::solver::LpSolverType::Glpk;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown LP solver '" << lpSolverName << "'.");
            }
            
            storm::solver::SmtSolverType GeneralSettings::getSmtSolver() const {
                std::string smtSolverName = this->getOption(smtSolverOptionName).getArgumentByName("name").getValueAsString();
                if (smtSolverName == "z3") {
                    return storm::solver::SmtSolverType::Z3;
                } else if (smtSolverName == "mathsat") {
                    return storm::solver::SmtSolverType::Mathsat;
                }
                STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown SMT solver '" << smtSolverName << "'.");
            }
            
            storm::dd::DdType GeneralSettings::getDdLibraryType() const {
                std::string ddLibraryAsString = this->getOption(ddLibraryOptionName).getArgumentByName("name").getValueAsString();
                if (ddLibraryAsString == "sylvan") {
                    return storm::dd::DdType::Sylvan;
                } else {
                    return storm::dd::DdType::CUDD;
                }
            }
            
            bool GeneralSettings::isConstantsSet() const {
                return this->getOption(constantsOptionName).getHasOptionBeenSet();
            }
            
            std::string GeneralSettings::getConstantDefinitionString() const {
                return this->getOption(constantsOptionName).getArgumentByName("values").getValueAsString();
            }
            
            bool GeneralSettings::isShowStatisticsSet() const {
                return this->getOption(statisticsOptionName).getHasOptionBeenSet();
            }
            
            bool GeneralSettings::isBisimulationSet() const {
                return this->getOption(bisimulationOptionName).getHasOptionBeenSet();
            }
            
            GeneralSettings::Engine GeneralSettings::getEngine() const {
                return engine;
            }

            void GeneralSettings::setEngine(Engine newEngine) {
                this->engine = newEngine;
            }
            
            bool GeneralSettings::isPrismCompatibilityEnabled() const {
                return this->getOption(prismCompatibilityOptionName).getHasOptionBeenSet();
            }

			storm::solver::MinMaxTechnique GeneralSettings::getMinMaxEquationSolvingTechnique() const {
				std::string minMaxEquationSolvingTechnique = this->getOption(minMaxEquationSolvingTechniqueOptionName).getArgumentByName("name").getValueAsString();
				if (minMaxEquationSolvingTechnique == "valueIteration") {
					return storm::solver::MinMaxTechnique::ValueIteration;
				} else if (minMaxEquationSolvingTechnique == "policyIteration") {
					return storm::solver::MinMaxTechnique::PolicyIteration;
				}
				STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown min/max equation solving technique '" << minMaxEquationSolvingTechnique << "'.");
			}

			bool GeneralSettings::isMinMaxEquationSolvingTechniqueSet() const {
				return this->getOption(minMaxEquationSolvingTechniqueOptionName).getHasOptionBeenSet();
			}
            
#ifdef STORM_HAVE_CARL
            bool GeneralSettings::isParametricSet() const {
                return this->getOption(parametricOptionName).getHasOptionBeenSet();
            }
#endif

            void GeneralSettings::finalize() {
                // Finalize engine.
                std::string engineStr = this->getOption(engineOptionName).getArgumentByName("name").getValueAsString();
                if (engineStr == "sparse") {
                    engine =  GeneralSettings::Engine::Sparse;
                } else if (engineStr == "hybrid") {
                    engine = GeneralSettings::Engine::Hybrid;
                } else if (engineStr == "dd") {
                    engine = GeneralSettings::Engine::Dd;
                } else if (engineStr == "abs") {
                    engine = GeneralSettings::Engine::AbstractionRefinement;
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentValueException, "Unknown engine '" << engineStr << "'.");
                }
            }

            bool GeneralSettings::check() const {
                // Ensure that the model was given either symbolically or explicitly.
                STORM_LOG_THROW(!isSymbolicSet() || !isExplicitSet(), storm::exceptions::InvalidSettingsException, "The model may be either given in an explicit or a symbolic format, but not both.");
                
                STORM_LOG_THROW(this->getEngine() == Engine::Sparse || !isExplicitSet(), storm::exceptions::InvalidSettingsException, "Cannot use explicit input models with this engine.");
                
                return true;
            }

            bool GeneralSettings::isCudaSet() const {
                return this->getOption(cudaOptionName).getHasOptionBeenSet();
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm