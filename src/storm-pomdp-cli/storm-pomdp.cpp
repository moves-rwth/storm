

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
        auto model = storm::cli::buildPreprocessExportModelWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, storm::RationalNumber>(symbolicInput, engine);
        STORM_LOG_THROW(model && model->getType() == storm::models::ModelType::Pomdp, storm::exceptions::WrongFormatException, "Expected a POMDP.");
        // CHECK if prop maximizes, only apply in those situations
        std::shared_ptr<storm::models::sparse::Pomdp<storm::RationalNumber>> pomdp = model->template as<storm::models::sparse::Pomdp<storm::RationalNumber>>();
        storm::transformer::GlobalPOMDPSelfLoopEliminator<storm::RationalNumber> selfLoopEliminator(*pomdp);
        pomdp = selfLoopEliminator.transform();

        storm::transformer::ApplyFiniteSchedulerToPomdp<storm::RationalNumber> toPMCTransformer(*pomdp);


        if (pomdpSettings.isExportToParametricSet()) {
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