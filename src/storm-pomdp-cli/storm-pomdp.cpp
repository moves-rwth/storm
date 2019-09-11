

#include "storm/utility/initialize.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/CuddSettings.h"
#include "storm/settings/modules/SylvanSettings.h"
#include "storm/settings/modules/EigenEquationSolverSettings.h"
#include "storm/settings/modules/GmmxxEquationSolverSettings.h"
#include "storm/settings/modules/NativeEquationSolverSettings.h"
#include "storm/settings/modules/EliminationSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/settings/modules/GameSolverSettings.h"
#include "storm/settings/modules/BisimulationSettings.h"
#include "storm/settings/modules/GlpkSettings.h"
#include "storm/settings/modules/GurobiSettings.h"
#include "storm/settings/modules/Smt2SmtSolverSettings.h"
#include "storm/settings/modules/ExplorationSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/AbstractionSettings.h"
#include "storm/settings/modules/BuildSettings.h"
#include "storm/settings/modules/JitBuilderSettings.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm-pomdp-cli/settings/modules/POMDPSettings.h"

#include "storm/analysis/GraphConditions.h"

#include "storm-cli-utilities/cli.h"
#include "storm-cli-utilities/model-handling.h"

#include "storm-pomdp/transformer/ApplyFiniteSchedulerToPomdp.h"
#include "storm-pomdp/transformer/GlobalPOMDPSelfLoopEliminator.h"
#include "storm-pomdp/transformer/GlobalPomdpMecChoiceEliminator.h"
#include "storm-pomdp/transformer/PomdpMemoryUnfolder.h"
#include "storm-pomdp/transformer/BinaryPomdpTransformer.h"
#include "storm-pomdp/analysis/UniqueObservationStates.h"
#include "storm-pomdp/analysis/QualitativeAnalysis.h"
#include "storm-pomdp/modelchecker/ApproximatePOMDPModelchecker.h"
#include "storm/api/storm.h"

/*!
 * Initialize the settings manager.
 */
void initializeSettings() {
    storm::settings::mutableManager().setName("Storm-POMDP", "storm-pomdp");

    storm::settings::addModule<storm::settings::modules::GeneralSettings>();
    storm::settings::addModule<storm::settings::modules::IOSettings>();
    storm::settings::addModule<storm::settings::modules::CoreSettings>();
    storm::settings::addModule<storm::settings::modules::DebugSettings>();
    storm::settings::addModule<storm::settings::modules::BuildSettings>();
    storm::settings::addModule<storm::settings::modules::GmmxxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::EigenEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::NativeEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::EliminationSettings>();
    storm::settings::addModule<storm::settings::modules::MinMaxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::GameSolverSettings>();
    storm::settings::addModule<storm::settings::modules::BisimulationSettings>();
    storm::settings::addModule<storm::settings::modules::GlpkSettings>();
    storm::settings::addModule<storm::settings::modules::ExplorationSettings>();
    storm::settings::addModule<storm::settings::modules::ResourceSettings>();
    storm::settings::addModule<storm::settings::modules::JitBuilderSettings>();



    storm::settings::addModule<storm::settings::modules::POMDPSettings>();
}

/*!
 * Entry point for the pomdp backend.
 *
 * @param argc The argc argument of main().
 * @param argv The argv argument of main().
 * @return Return code, 0 if successfull, not 0 otherwise.
 */
int main(const int argc, const char** argv) {
    try {
        storm::utility::setUp();
        storm::cli::printHeader("Storm-pomdp", argc, argv);
        initializeSettings();

        bool optionsCorrect = storm::cli::parseOptions(argc, argv);
        if (!optionsCorrect) {
            return -1;
        }


        auto const& coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
        auto const& pomdpSettings = storm::settings::getModule<storm::settings::modules::POMDPSettings>();
        auto const &general = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
        auto const &debug = storm::settings::getModule<storm::settings::modules::DebugSettings>();

        if (general.isVerboseSet()) {
            storm::utility::setLogLevel(l3pp::LogLevel::INFO);
        }
        if (debug.isDebugSet()) {
            storm::utility::setLogLevel(l3pp::LogLevel::DEBUG);
        }
        if (debug.isTraceSet()) {
            storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
        }

        // For several engines, no model building step is performed, but the verification is started right away.
        storm::settings::modules::CoreSettings::Engine engine = coreSettings.getEngine();

        storm::cli::SymbolicInput symbolicInput = storm::cli::parseAndPreprocessSymbolicInput();
        // We should not export here if we are going to do some processing first.
        auto model = storm::cli::buildPreprocessExportModelWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalNumber>(symbolicInput, engine);
        STORM_LOG_THROW(model && model->getType() == storm::models::ModelType::Pomdp, storm::exceptions::WrongFormatException, "Expected a POMDP.");
        std::shared_ptr<storm::models::sparse::Pomdp<storm::RationalNumber>> pomdp = model->template as<storm::models::sparse::Pomdp<storm::RationalNumber>>();

        // For ease of testing
        storm::pomdp::modelchecker::ApproximatePOMDPModelchecker<storm::RationalNumber> checker = storm::pomdp::modelchecker::ApproximatePOMDPModelchecker<storm::RationalNumber>();

        std::shared_ptr<storm::logic::Formula const> formula;
        if (!symbolicInput.properties.empty()) {
            formula = symbolicInput.properties.front().getRawFormula();
            STORM_PRINT_AND_LOG("Analyzing property '" << *formula << "'" << std::endl);
            STORM_LOG_WARN_COND(symbolicInput.properties.size() == 1, "There is currently no support for multiple properties. All other properties will be ignored.");
        }
        
        if (pomdpSettings.isAnalyzeUniqueObservationsSet()) {
            STORM_PRINT_AND_LOG("Analyzing states with unique observation ..." << std::endl);
            storm::analysis::UniqueObservationStates<storm::RationalNumber> uniqueAnalysis(*pomdp);
            std::cout << uniqueAnalysis.analyse() << std::endl;
        }
        
        if (formula) {
            if (formula->isProbabilityOperatorFormula()) {
                if (pomdpSettings.isSelfloopReductionSet() && !storm::solver::minimize(formula->asProbabilityOperatorFormula().getOptimalityType())) {
                    STORM_PRINT_AND_LOG("Eliminating self-loop choices ...");
                    uint64_t oldChoiceCount = pomdp->getNumberOfChoices();
                    storm::transformer::GlobalPOMDPSelfLoopEliminator<storm::RationalNumber> selfLoopEliminator(*pomdp);
                    pomdp = selfLoopEliminator.transform();
                    STORM_PRINT_AND_LOG(oldChoiceCount - pomdp->getNumberOfChoices() << " choices eliminated through self-loop elimination." << std::endl);
                }
                if (pomdpSettings.isQualitativeReductionSet()) {
                    storm::analysis::QualitativeAnalysis<storm::RationalNumber> qualitativeAnalysis(*pomdp);
                    STORM_PRINT_AND_LOG("Computing states with probability 0 ...");
                    std::cout << qualitativeAnalysis.analyseProb0(formula->asProbabilityOperatorFormula()) << std::endl;
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    STORM_PRINT_AND_LOG("Computing states with probability 1 ...");
                    std::cout << qualitativeAnalysis.analyseProb1(formula->asProbabilityOperatorFormula()) << std::endl;
                    STORM_PRINT_AND_LOG(" done." << std::endl);
                    std::cout << "actual reduction not yet implemented..." << std::endl;
                }
            } else if (formula->isRewardOperatorFormula()) {
                if (pomdpSettings.isSelfloopReductionSet() && storm::solver::minimize(formula->asRewardOperatorFormula().getOptimalityType())) {
                    STORM_PRINT_AND_LOG("Eliminating self-loop choices ...");
                    uint64_t oldChoiceCount = pomdp->getNumberOfChoices();
                    storm::transformer::GlobalPOMDPSelfLoopEliminator<storm::RationalNumber> selfLoopEliminator(*pomdp);
                    pomdp = selfLoopEliminator.transform();
                    STORM_PRINT_AND_LOG(oldChoiceCount - pomdp->getNumberOfChoices() << " choices eliminated through self-loop elimination." << std::endl);
                }
            }
            if (pomdpSettings.getMemoryBound() > 1) {
                STORM_PRINT_AND_LOG("Computing the unfolding for memory bound " << pomdpSettings.getMemoryBound() << " and memory pattern '" << storm::storage::toString(pomdpSettings.getMemoryPattern()) << "' ...");
                storm::storage::PomdpMemory memory = storm::storage::PomdpMemoryBuilder().build(pomdpSettings.getMemoryPattern(), pomdpSettings.getMemoryBound());
                std::cout << memory.toString() << std::endl;
                storm::transformer::PomdpMemoryUnfolder<storm::RationalNumber> memoryUnfolder(*pomdp, memory);
                pomdp = memoryUnfolder.transform();
                STORM_PRINT_AND_LOG(" done." << std::endl);
                pomdp->printModelInformationToStream(std::cout);
            } else {
                STORM_PRINT_AND_LOG("Assumming memoryless schedulers." << std::endl;)
            }
            
            // From now on the pomdp is considered memoryless
            
            if (pomdpSettings.isMecReductionSet()) {
                STORM_PRINT_AND_LOG("Eliminating mec choices ...");
                // Note: Elimination of mec choices only preserves memoryless schedulers.
                uint64_t oldChoiceCount = pomdp->getNumberOfChoices();
                storm::transformer::GlobalPomdpMecChoiceEliminator<storm::RationalNumber> mecChoiceEliminator(*pomdp);
                pomdp = mecChoiceEliminator.transform(*formula);
                STORM_PRINT_AND_LOG(" done." << std::endl);
                STORM_PRINT_AND_LOG(oldChoiceCount - pomdp->getNumberOfChoices() << " choices eliminated through MEC choice elimination." << std::endl);
                pomdp->printModelInformationToStream(std::cout);
            }
            
            if (pomdpSettings.isTransformBinarySet() || pomdpSettings.isTransformSimpleSet()) {
                if (pomdpSettings.isTransformSimpleSet()) {
                    STORM_PRINT_AND_LOG("Transforming the POMDP to a simple POMDP.");
                    pomdp = storm::transformer::BinaryPomdpTransformer<storm::RationalNumber>().transform(*pomdp, true);
                } else {
                    STORM_PRINT_AND_LOG("Transforming the POMDP to a binary POMDP.");
                    pomdp = storm::transformer::BinaryPomdpTransformer<storm::RationalNumber>().transform(*pomdp, false);
                }
                pomdp->printModelInformationToStream(std::cout);
                STORM_PRINT_AND_LOG(" done." << std::endl);
            }

            
            if (pomdpSettings.isExportToParametricSet()) {
                STORM_PRINT_AND_LOG("Transforming memoryless POMDP to pMC...");
                storm::transformer::ApplyFiniteSchedulerToPomdp<storm::RationalNumber> toPMCTransformer(*pomdp);
                std::string transformMode = pomdpSettings.getFscApplicationTypeString();
                auto pmc = toPMCTransformer.transform(storm::transformer::parsePomdpFscApplicationMode(transformMode));
                STORM_PRINT_AND_LOG(" done." << std::endl);
                pmc->printModelInformationToStream(std::cout);
                STORM_PRINT_AND_LOG("Simplifying pMC...");
                //if (generalSettings.isBisimulationSet()) {
                pmc = storm::api::performBisimulationMinimization<storm::RationalFunction>(pmc->as<storm::models::sparse::Dtmc<storm::RationalFunction>>(),{formula}, storm::storage::BisimulationType::Strong)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

                //}
                STORM_PRINT_AND_LOG(" done." << std::endl);
                pmc->printModelInformationToStream(std::cout);
                STORM_PRINT_AND_LOG("Exporting pMC...");
                storm::analysis::ConstraintCollector<storm::RationalFunction> constraints(*pmc);
                auto const& parameterSet = constraints.getVariables();
                std::vector<storm::RationalFunctionVariable> parameters(parameterSet.begin(), parameterSet.end());
                std::vector<std::string> parameterNames;
                for (auto const& parameter : parameters) {
                    parameterNames.push_back(parameter.name());
                }
                storm::api::exportSparseModelAsDrn(pmc, pomdpSettings.getExportToParametricFilename(), parameterNames);
                STORM_PRINT_AND_LOG(" done." << std::endl);
            }

        } else {
            STORM_LOG_WARN("Nothing to be done. Did you forget to specify a formula?");
        }



        // All operations have now been performed, so we clean up everything and terminate.
        storm::utility::cleanUp();
        return 0;
    } catch (storm::exceptions::BaseException const &exception) {
        STORM_LOG_ERROR("An exception caused Storm-pomdp to terminate. The message of the exception is: " << exception.what());
        return 1;
    } catch (std::exception const &exception) {
        STORM_LOG_ERROR("An unexpected exception occurred and caused Storm-pomdp to terminate. The message of this exception is: " << exception.what());
        return 2;
    }
}
