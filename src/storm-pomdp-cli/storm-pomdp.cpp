

#include "storm/utility/initialize.h"

#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/CounterexampleGeneratorSettings.h"
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
#include "storm/settings/modules/TopologicalValueIterationEquationSolverSettings.h"
#include "storm/settings/modules/ExplorationSettings.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/AbstractionSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"
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
#include "storm-pomdp/analysis/UniqueObservationStates.h"
#include "storm-pomdp/analysis/QualitativeAnalysis.h"

/*!
 * Initialize the settings manager.
 */
void initializeSettings() {
    storm::settings::mutableManager().setName("Storm-POMDP", "storm-pomdp");

    storm::settings::addModule<storm::settings::modules::GeneralSettings>();
    storm::settings::addModule<storm::settings::modules::IOSettings>();
    storm::settings::addModule<storm::settings::modules::CoreSettings>();
    storm::settings::addModule<storm::settings::modules::DebugSettings>();
    storm::settings::addModule<storm::settings::modules::CounterexampleGeneratorSettings>(false);
    storm::settings::addModule<storm::settings::modules::CuddSettings>();
    storm::settings::addModule<storm::settings::modules::SylvanSettings>();
    storm::settings::addModule<storm::settings::modules::GmmxxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::EigenEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::NativeEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::EliminationSettings>();
    storm::settings::addModule<storm::settings::modules::MinMaxEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::GameSolverSettings>();
    storm::settings::addModule<storm::settings::modules::BisimulationSettings>();
    storm::settings::addModule<storm::settings::modules::GlpkSettings>();
    storm::settings::addModule<storm::settings::modules::GurobiSettings>();
    storm::settings::addModule<storm::settings::modules::TopologicalValueIterationEquationSolverSettings>();
    storm::settings::addModule<storm::settings::modules::Smt2SmtSolverSettings>();
    storm::settings::addModule<storm::settings::modules::ExplorationSettings>();
    storm::settings::addModule<storm::settings::modules::ResourceSettings>();
    storm::settings::addModule<storm::settings::modules::AbstractionSettings>();
    storm::settings::addModule<storm::settings::modules::JaniExportSettings>();
    storm::settings::addModule<storm::settings::modules::JitBuilderSettings>();
    storm::settings::addModule<storm::settings::modules::MultiObjectiveSettings>();


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

        // For several engines, no model building step is performed, but the verification is started right away.
        storm::settings::modules::CoreSettings::Engine engine = coreSettings.getEngine();

        storm::cli::SymbolicInput symbolicInput = storm::cli::parseAndPreprocessSymbolicInput();
        // We should not export here if we are going to do some processing first.
        auto model = storm::cli::buildPreprocessExportModelWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalNumber>(symbolicInput, engine);
        STORM_LOG_THROW(model && model->getType() == storm::models::ModelType::Pomdp, storm::exceptions::WrongFormatException, "Expected a POMDP.");
        std::shared_ptr<storm::models::sparse::Pomdp<storm::RationalNumber>> pomdp = model->template as<storm::models::sparse::Pomdp<storm::RationalNumber>>();
        
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
                    STORM_PRINT_AND_LOG("Computing states with probability 1 ...");
                    std::cout << qualitativeAnalysis.analyseProb1(formula->asProbabilityOperatorFormula()) << std::endl;
                    std::cout << "actual reduction not yet implemented..." << std::endl;
                }
                if (pomdpSettings.getMemoryBound() > 1) {
                    storm::transformer::PomdpMemoryUnfolder<storm::RationalNumber> memoryUnfolder(*pomdp, pomdpSettings.getMemoryBound());
                    pomdp = memoryUnfolder.transform();
                }
                
                // From now on the pomdp is considered memoryless
                
                if (pomdpSettings.isMecReductionSet()) {
                    STORM_PRINT_AND_LOG("Eliminating mec choices ...");
                    // Note: Elimination of mec choices only preserves memoryless schedulers.
                    uint64_t oldChoiceCount = pomdp->getNumberOfChoices();
                    storm::transformer::GlobalPomdpMecChoiceEliminator<storm::RationalNumber> mecChoiceEliminator(*pomdp);
                    pomdp = mecChoiceEliminator.transform(*formula);
                    STORM_PRINT_AND_LOG(oldChoiceCount - pomdp->getNumberOfChoices() << " choices eliminated through MEC choice elimination." << std::endl);
                }
                
                if (pomdpSettings.isExportToParametricSet()) {
                    storm::transformer::ApplyFiniteSchedulerToPomdp<storm::RationalNumber> toPMCTransformer(*pomdp);
                    auto const &pmc = toPMCTransformer.transform();
                    storm::analysis::ConstraintCollector<storm::RationalFunction> constraints(*pmc);
                    pmc->printModelInformationToStream(std::cout);
                    auto const& parameterSet = constraints.getVariables();
                    std::vector<storm::RationalFunctionVariable> parameters(parameterSet.begin(), parameterSet.end());
                    std::vector<std::string> parameterNames;
                    for (auto const& parameter : parameters) {
                        parameterNames.push_back(parameter.getName());
                    }
                    storm::api::exportSparseModelAsDrn(pmc, pomdpSettings.getExportToParametricFilename(), parameterNames);
                }

            }
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