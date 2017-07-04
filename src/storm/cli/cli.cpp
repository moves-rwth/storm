#include "storm/cli/cli.h"

#include "storm/storage/SymbolicModelDescription.h"

#include "storm/models/ModelBase.h"

#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/exceptions/OptionParserException.h"
#include "storm/settings/modules/ResourceSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"

#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/utility/resources.h"
#include "storm/utility/file.h"
#include "storm/utility/storm-version.h"
#include "storm/utility/cli.h"

#include "storm/utility/initialize.h"
#include "storm/utility/Stopwatch.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/ResourceSettings.h"

#include <type_traits>

#include "storm/api/storm.h"

#include "storm/utility/macros.h"

// Includes for the linked libraries and versions header.
#ifdef STORM_HAVE_INTELTBB
#	include "tbb/tbb_stddef.h"
#endif
#ifdef STORM_HAVE_GLPK
#	include "glpk.h"
#endif
#ifdef STORM_HAVE_GUROBI
#	include "gurobi_c.h"
#endif
#ifdef STORM_HAVE_Z3
#	include "z3.h"
#endif
#ifdef STORM_HAVE_MSAT
#   include "mathsat.h"
#endif
#ifdef STORM_HAVE_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#endif

#ifdef STORM_HAVE_SMTRAT
#include "lib/smtrat.h"
#endif

namespace storm {
    namespace cli {
        
        int64_t process(const int argc, const char** argv) {
            storm::utility::setUp();
            storm::cli::printHeader("Storm", argc, argv);
            storm::settings::initializeAll("Storm", "storm");
            
            storm::utility::Stopwatch totalTimer(true);
            if (!storm::cli::parseOptions(argc, argv)) {
                return -1;
            }
            
            processOptions();

            totalTimer.stop();
            if (storm::settings::getModule<storm::settings::modules::ResourceSettings>().isPrintTimeAndMemorySet()) {
                storm::cli::printTimeAndMemoryStatistics(totalTimer.getTimeInMilliseconds());
            }

            storm::utility::cleanUp();
            return 0;
        }
        
        
        void printHeader(std::string const& name, const int argc, const char* argv[]) {
            STORM_PRINT(name << " " << storm::utility::StormVersion::shortVersionString() << std::endl << std::endl);
            
            // "Compute" the command line argument string with which storm was invoked.
            std::stringstream commandStream;
            for (int i = 1; i < argc; ++i) {
                commandStream << argv[i] << " ";
            }
            
            std::string command = commandStream.str();
            
            if (!command.empty()) {
                STORM_PRINT("Command line arguments: " << commandStream.str() << std::endl);
                STORM_PRINT("Current working directory: " << storm::utility::cli::getCurrentWorkingDirectory() << std::endl << std::endl);
            }
        }
        
        void printVersion(std::string const& name) {
            STORM_PRINT(storm::utility::StormVersion::longVersionString() << std::endl);
            STORM_PRINT(storm::utility::StormVersion::buildInfo() << std::endl);
            
#ifdef STORM_HAVE_INTELTBB
            STORM_PRINT("Linked with Intel Threading Building Blocks v" << TBB_VERSION_MAJOR << "." << TBB_VERSION_MINOR << " (Interface version " << TBB_INTERFACE_VERSION << ")." << std::endl);
#endif
#ifdef STORM_HAVE_GLPK
            STORM_PRINT("Linked with GNU Linear Programming Kit v" << GLP_MAJOR_VERSION << "." << GLP_MINOR_VERSION << "." << std::endl);
#endif
#ifdef STORM_HAVE_GUROBI
            STORM_PRINT("Linked with Gurobi Optimizer v" << GRB_VERSION_MAJOR << "." << GRB_VERSION_MINOR << "." << GRB_VERSION_TECHNICAL << "." << std::endl);
#endif
#ifdef STORM_HAVE_Z3
            unsigned int z3Major, z3Minor, z3BuildNumber, z3RevisionNumber;
            Z3_get_version(&z3Major, &z3Minor, &z3BuildNumber, &z3RevisionNumber);
            STORM_PRINT("Linked with Microsoft Z3 Optimizer v" << z3Major << "." << z3Minor << " Build " << z3BuildNumber << " Rev " << z3RevisionNumber << "." << std::endl);
#endif
#ifdef STORM_HAVE_MSAT
            char* msatVersion = msat_get_version();
            STORM_PRINT("Linked with " << msatVersion << "." << std::endl);
            msat_free(msatVersion);
#endif
#ifdef STORM_HAVE_SMTRAT
            STORM_PRINT("Linked with SMT-RAT " << SMTRAT_VERSION << "." << std::endl);
#endif
#ifdef STORM_HAVE_CARL
            // TODO get version string
            STORM_PRINT("Linked with CArL." << std::endl);
#endif
            
#ifdef STORM_HAVE_CUDA
            int deviceCount = 0;
            cudaError_t error_id = cudaGetDeviceCount(&deviceCount);
            
            if (error_id == cudaSuccess) {
                STORM_PRINT("Compiled with CUDA support, ");
                // This function call returns 0 if there are no CUDA capable devices.
                if (deviceCount == 0){
                    STORM_PRINT("but there are no available device(s) that support CUDA." << std::endl);
                } else {
                    STORM_PRINT("detected " << deviceCount << " CUDA capable device(s):" << std::endl);
                }
                
                int dev, driverVersion = 0, runtimeVersion = 0;
                
                for (dev = 0; dev < deviceCount; ++dev) {
                    cudaSetDevice(dev);
                    cudaDeviceProp deviceProp;
                    cudaGetDeviceProperties(&deviceProp, dev);
                    
                    STORM_PRINT("CUDA device " << dev << ": \"" << deviceProp.name << "\"" << std::endl);
                    
                    // Console log
                    cudaDriverGetVersion(&driverVersion);
                    cudaRuntimeGetVersion(&runtimeVersion);
                    STORM_PRINT("  CUDA Driver Version / Runtime Version          " << driverVersion / 1000 << "." << (driverVersion % 100) / 10 << " / " << runtimeVersion / 1000 << "." << (runtimeVersion % 100) / 10 << std::endl);
                    STORM_PRINT("  CUDA Capability Major/Minor version number:    " << deviceProp.major << "." << deviceProp.minor << std::endl);
                }
                STORM_PRINT(std::endl);
            }
            else {
                STORM_PRINT("Compiled with CUDA support, but an error occured trying to find CUDA devices." << std::endl);
            }
#endif
        }
        
        bool parseOptions(const int argc, const char* argv[]) {
            try {
                storm::settings::mutableManager().setFromCommandLine(argc, argv);
            } catch (storm::exceptions::OptionParserException& e) {
                storm::settings::manager().printHelp();
                throw e;
                return false;
            }
            
            storm::settings::modules::GeneralSettings const& general = storm::settings::getModule<storm::settings::modules::GeneralSettings>();

            bool result = true;
            if (general.isHelpSet()) {
                storm::settings::manager().printHelp(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getHelpModuleName());
                result = false;
            }
            
            if (general.isVersionSet()) {
                printVersion("storm");
                result = false;;
            }

            return result;
        }

        void setResourceLimits() {
            storm::settings::modules::ResourceSettings const& resources = storm::settings::getModule<storm::settings::modules::ResourceSettings>();
            
            // If we were given a time limit, we put it in place now.
            if (resources.isTimeoutSet()) {
                storm::utility::resources::setCPULimit(resources.getTimeoutInSeconds());
            }
        }
        
        void setLogLevel() {
            storm::settings::modules::GeneralSettings const& general = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            storm::settings::modules::DebugSettings const& debug = storm::settings::getModule<storm::settings::modules::DebugSettings>();
            
            if (general.isVerboseSet()) {
                storm::utility::setLogLevel(l3pp::LogLevel::INFO);
            }
            if (debug.isDebugSet()) {
                storm::utility::setLogLevel(l3pp::LogLevel::DEBUG);
            }
            if (debug.isTraceSet()) {
                storm::utility::setLogLevel(l3pp::LogLevel::TRACE);
            }
            if (debug.isLogfileSet()) {
                storm::utility::initializeFileLogging();
            }
        }
        
        void setFileLogging() {
            storm::settings::modules::DebugSettings const& debug = storm::settings::getModule<storm::settings::modules::DebugSettings>();
            if (debug.isLogfileSet()) {
                storm::utility::initializeFileLogging();
            }
        }
        
        void setUrgentOptions() {
            setResourceLimits();
            setLogLevel();
            setFileLogging();
        }
        
        struct SymbolicInput {
            // The symbolic model description.
            boost::optional<storm::storage::SymbolicModelDescription> model;
            
            // The properties to check.
            std::vector<storm::jani::Property> properties;
        };
        
        void parseSymbolicModelDescription(storm::settings::modules::IOSettings const& ioSettings, SymbolicInput& input) {
            if (ioSettings.isPrismOrJaniInputSet()) {
                if (ioSettings.isPrismInputSet()) {
                    input.model = storm::api::parseProgram(ioSettings.getPrismInputFilename());
                } else {
                    auto janiInput = storm::api::parseJaniModel(ioSettings.getJaniInputFilename());
                    input.model = janiInput.first;
                    auto const& janiPropertyInput = janiInput.second;
                    
                    if (ioSettings.isJaniPropertiesSet()) {
                        for (auto const& propName : ioSettings.getJaniProperties()) {
                            auto propertyIt = janiPropertyInput.find(propName);
                            STORM_LOG_THROW(propertyIt != janiPropertyInput.end(), storm::exceptions::InvalidArgumentException, "No JANI property with name '" << propName << "' is known.");
                            input.properties.emplace_back(propertyIt->second);
                        }
                    }
                }
            }
        }
        
        void parseProperties(storm::settings::modules::IOSettings const& ioSettings, SymbolicInput& input, boost::optional<std::set<std::string>> const& propertyFilter) {
            if (ioSettings.isPropertySet()) {
                std::vector<storm::jani::Property> newProperties;
                if (input.model) {
                    newProperties = storm::api::parsePropertiesForSymbolicModelDescription(ioSettings.getProperty(), input.model.get(), propertyFilter);
                } else {
                    newProperties = storm::api::parseProperties(ioSettings.getProperty(), propertyFilter);
                }
                
                input.properties.insert(input.properties.end(), newProperties.begin(), newProperties.end());
            }
        }
        
        SymbolicInput parseSymbolicInput() {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();

            // Parse the property filter, if any is given.
            boost::optional<std::set<std::string>> propertyFilter = storm::api::parsePropertyFilter(ioSettings.getPropertyFilter());
            
            SymbolicInput input;
            parseSymbolicModelDescription(ioSettings, input);
            parseProperties(ioSettings, input, propertyFilter);
            
            return input;
        }
        
        SymbolicInput preprocessSymbolicInput(SymbolicInput const& input) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();

            SymbolicInput output = input;
            
            // Substitute constant definitions in symbolic input.
            std::string constantDefinitionString = ioSettings.getConstantDefinitionString();
            std::map<storm::expressions::Variable, storm::expressions::Expression> constantDefinitions;
            if (output.model) {
                constantDefinitions = output.model.get().parseConstantDefinitions(constantDefinitionString);
                output.model = output.model.get().preprocess(constantDefinitions);
            }
            if (!output.properties.empty()) {
                output.properties = storm::api::substituteConstantsInProperties(output.properties, constantDefinitions);
            }
            
            // Check whether conversion for PRISM to JANI is requested or necessary.
            if (input.model && input.model.get().isPrismProgram()) {
                bool transformToJani = ioSettings.isPrismToJaniSet();
                bool transformToJaniForJit = coreSettings.getEngine() == storm::settings::modules::CoreSettings::Engine::Sparse && ioSettings.isJitSet();
                STORM_LOG_WARN_COND(transformToJani || !transformToJaniForJit, "The JIT-based model builder is only available for JANI models, automatically converting the PRISM input model.");
                transformToJani |= transformToJaniForJit;
                
                if (transformToJani) {
                    SymbolicInput output;

                    storm::prism::Program const& model = output.model.get().asPrismProgram();
                    auto modelAndRenaming = model.toJaniWithLabelRenaming(true);
                    output.model = modelAndRenaming.first;
                    
                    if (!modelAndRenaming.second.empty()) {
                        std::map<std::string, std::string> const& labelRenaming = modelAndRenaming.second;
                        std::vector<storm::jani::Property> amendedProperties;
                        for (auto const& property : output.properties) {
                            amendedProperties.emplace_back(property.substituteLabels(labelRenaming));
                        }
                        output.properties = std::move(amendedProperties);
                    }
                }
            }
            
            return output;
        }
        
        void exportSymbolicInput(SymbolicInput const& input) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            if (input.model && input.model.get().isJaniModel()) {
                storm::storage::SymbolicModelDescription const& model = input.model.get();
                if (ioSettings.isExportJaniDotSet()) {
                    storm::api::exportJaniModelAsDot(model.asJaniModel(), ioSettings.getExportJaniDotFilename());
                }
                
                if (model.isJaniModel() && storm::settings::getModule<storm::settings::modules::JaniExportSettings>().isJaniFileSet()) {
                    storm::api::exportJaniModel(model.asJaniModel(), input.properties, storm::settings::getModule<storm::settings::modules::JaniExportSettings>().getJaniFilename());
                }
            }
        }
        
        SymbolicInput parseAndPreprocessSymbolicInput() {
            SymbolicInput input = parseSymbolicInput();
            input = preprocessSymbolicInput(input);
            exportSymbolicInput(input);
            return input;
        }
        
        std::vector<std::shared_ptr<storm::logic::Formula const>> createFormulasToRespect(std::vector<storm::jani::Property> const& properties) {
            std::vector<std::shared_ptr<storm::logic::Formula const>> result = storm::api::extractFormulasFromProperties(properties);
            
            for (auto const& property : properties) {
                if (!property.getFilter().getStatesFormula()->isInitialFormula()) {
                    result.push_back(property.getFilter().getStatesFormula());
                }
            }

            return result;
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModelDd(SymbolicInput const& input) {
            return storm::api::buildSymbolicModel<DdType, ValueType>(input.model.get(), createFormulasToRespect(input.properties));
        }

        template <typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModelSparse(SymbolicInput const& input, storm::settings::modules::IOSettings const& ioSettings) {
            auto counterexampleGeneratorSettings = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>();
            return storm::api::buildSparseModel<ValueType>(input.model.get(), createFormulasToRespect(input.properties), ioSettings.isBuildChoiceLabelsSet(), counterexampleGeneratorSettings.isMinimalCommandSetGenerationSet());
        }

        template <typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModelExplicit(storm::settings::modules::IOSettings const& ioSettings) {
            std::shared_ptr<storm::models::ModelBase> result;
            if (ioSettings.isExplicitSet()) {
                result = storm::api::buildExplicitModel<ValueType>(ioSettings.getTransitionFilename(), ioSettings.getLabelingFilename(), ioSettings.isStateRewardsSet() ? boost::optional<std::string>(ioSettings.getStateRewardsFilename()) : boost::none, ioSettings.isTransitionRewardsSet() ? boost::optional<std::string>(ioSettings.getTransitionRewardsFilename()) : boost::none, ioSettings.isChoiceLabelingSet() ? boost::optional<std::string>(ioSettings.getChoiceLabelingFilename()) : boost::none);
            } else {
                result = storm::api::buildExplicitDRNModel<ValueType>(ioSettings.getExplicitDRNFilename());
            }
            return result;
        }

        template <storm::dd::DdType DdType, typename ValueType>
        std::shared_ptr<storm::models::ModelBase> buildModel(storm::settings::modules::CoreSettings::Engine const& engine, SymbolicInput const& input, storm::settings::modules::IOSettings const& ioSettings) {
            storm::utility::Stopwatch modelBuildingWatch(true);

            std::shared_ptr<storm::models::ModelBase> result;
            if (input.model) {
                if (engine == storm::settings::modules::CoreSettings::Engine::Dd || engine == storm::settings::modules::CoreSettings::Engine::Hybrid) {
                    result = buildModelDd<DdType, ValueType>(input);
                } else if (engine == storm::settings::modules::CoreSettings::Engine::Sparse) {
                    result = buildModelSparse<ValueType>(input, ioSettings);
                }
            } else if (ioSettings.isExplicitSet() || ioSettings.isExplicitDRNSet()) {
                STORM_LOG_THROW(engine == storm::settings::modules::CoreSettings::Engine::Sparse, storm::exceptions::InvalidSettingsException, "Can only use sparse engine with explicit input.");
                result = buildModelExplicit<ValueType>(ioSettings);
            }
            
            modelBuildingWatch.stop();
            if (result) {
                STORM_PRINT_AND_LOG("Time for model construction: " << modelBuildingWatch << "." << std::endl << std::endl);
            }

            return result;
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> preprocessSparseMarkovAutomaton(std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> const& model) {
            std::shared_ptr<storm::models::sparse::Model<ValueType>> result = model;
            model->close();
            if (model->hasOnlyTrivialNondeterminism()) {
                result = model->convertToCTMC();
            }
            return result;
        }

        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> preprocessSparseModelBisimulation(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input, storm::settings::modules::BisimulationSettings const& bisimulationSettings) {
            storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
            if (bisimulationSettings.isWeakBisimulationSet()) {
                bisimType = storm::storage::BisimulationType::Weak;
            }
            
            STORM_LOG_INFO("Performing bisimulation minimization...");
            return storm::api::performBisimulationMinimization<ValueType>(model, createFormulasToRespect(input.properties), bisimType);
        }
        
        template <typename ValueType>
        std::pair<std::shared_ptr<storm::models::sparse::Model<ValueType>>, bool> preprocessSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
            auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            auto bisimulationSettings = storm::settings::getModule<storm::settings::modules::BisimulationSettings>();
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            
            std::pair<std::shared_ptr<storm::models::sparse::Model<ValueType>>, bool> result = std::make_pair(model, false);
            
            if (result.first->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                result.first = preprocessSparseMarkovAutomaton(result.first->template as<storm::models::sparse::MarkovAutomaton<ValueType>>());
                result.second = true;
            }
            
            if (generalSettings.isBisimulationSet()) {
                result.first = preprocessSparseModelBisimulation(result.first, input, bisimulationSettings);
                result.second = true;
            }
            
            return result;
        }
        
        template <typename ValueType>
        void exportSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, SymbolicInput const& input) {
            auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
            
            if (ioSettings.isExportExplicitSet()) {
                storm::api::exportSparseModelAsDrn(model, ioSettings.getExportExplicitFilename(), input.model ? input.model.get().getParameterNames() : std::vector<std::string>());
            }
            
            if (ioSettings.isExportDotSet()) {
                storm::api::exportSparseModelAsDot(model, ioSettings.getExportDotFilename());
            }
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void exportDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input) {
            // Intentionally left empty.
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void exportModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            if (model->isSparseModel()) {
                exportSparseModel<ValueType>(model->as<storm::models::sparse::Model<ValueType>>(), input);
            } else {
                exportDdModel<DdType, ValueType>(model->as<storm::models::symbolic::Model<DdType, ValueType>>(), input);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessDdModel(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model, SymbolicInput const& input) {
            return std::make_pair(model, false);
        }

        template <storm::dd::DdType DdType, typename ValueType>
        std::pair<std::shared_ptr<storm::models::ModelBase>, bool> preprocessModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            storm::utility::Stopwatch preprocessingWatch(true);
            
            std::pair<std::shared_ptr<storm::models::ModelBase>, bool> result = std::make_pair(model, false);
            if (model->isSparseModel()) {
                result = preprocessSparseModel<ValueType>(result.first->as<storm::models::sparse::Model<ValueType>>(), input);
            } else {
                STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
                result = preprocessDdModel<DdType, ValueType>(result.first->as<storm::models::symbolic::Model<DdType, ValueType>>(), input);
            }
            
            if (result.second) {
                STORM_PRINT_AND_LOG(std::endl << "Time for model preprocessing: " << preprocessingWatch << "." << std::endl << std::endl);
            }
            return result;
        }
        
        void printComputingCounterexample(storm::jani::Property const& property) {
            STORM_PRINT_AND_LOG("Computing counterexample for property " << *property.getRawFormula() << " ..." << std::endl);
        }
        
        void printCounterexample(std::shared_ptr<storm::counterexamples::Counterexample> const& counterexample, storm::utility::Stopwatch* watch = nullptr) {
            if (counterexample) {
                STORM_PRINT_AND_LOG(*counterexample << std::endl);
                if (watch) {
                    STORM_PRINT_AND_LOG("Time for computation: " << *watch << "." << std::endl);
                }
            } else {
                STORM_PRINT_AND_LOG(" failed." << std::endl);
            }
        }

        template <typename ValueType>
        void generateCounterexamples(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Counterexample generation is not supported for this data-type.");
        }
        
        template <>
        void generateCounterexamples<double>(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            typedef double ValueType;
            
            STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::NotSupportedException, "Counterexample generation is currently only supported for sparse models.");
            auto sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            
            STORM_LOG_THROW(sparseModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::NotSupportedException, "Counterexample is currently only supported for MDPs.");
            auto mdp = sparseModel->template as<storm::models::sparse::Mdp<ValueType>>();
            
            auto counterexampleSettings = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>();
            if (counterexampleSettings.isMinimalCommandSetGenerationSet()) {
                STORM_LOG_THROW(input.model && input.model.get().isPrismProgram(), storm::exceptions::NotSupportedException, "Minimal command set counterexamples are only supported for PRISM model input.");
                storm::prism::Program const& program = input.model.get().asPrismProgram();

                bool useMilp = counterexampleSettings.isUseMilpBasedMinimalCommandSetGenerationSet();
                for (auto const& property : input.properties) {
                    std::shared_ptr<storm::counterexamples::Counterexample> counterexample;
                    printComputingCounterexample(property);
                    storm::utility::Stopwatch watch(true);
                    if (useMilp) {
                        counterexample = storm::api::computePrismHighLevelCounterexampleMilp(program, mdp, property.getRawFormula());
                    } else {
                        counterexample = storm::api::computePrismHighLevelCounterexampleMaxSmt(program, mdp, property.getRawFormula());
                    }
                    watch.stop();
                    printCounterexample(counterexample, &watch);
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The selected counterexample formalism is unsupported.");
            }
        }
        
        template<typename ValueType>
        void printFilteredResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::modelchecker::FilterType ft) {
            if (result->isQuantitative()) {
                switch (ft) {
                    case storm::modelchecker::FilterType::VALUES:
                        STORM_PRINT_AND_LOG(*result);
                        break;
                    case storm::modelchecker::FilterType::SUM:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().sum());
                        break;
                    case storm::modelchecker::FilterType::AVG:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().average());
                        break;
                    case storm::modelchecker::FilterType::MIN:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().getMin());
                        break;
                    case storm::modelchecker::FilterType::MAX:
                        STORM_PRINT_AND_LOG(result->asQuantitativeCheckResult<ValueType>().getMax());
                        break;
                    case storm::modelchecker::FilterType::ARGMIN:
                    case storm::modelchecker::FilterType::ARGMAX:
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Outputting states is not supported.");
                    case storm::modelchecker::FilterType::EXISTS:
                    case storm::modelchecker::FilterType::FORALL:
                    case storm::modelchecker::FilterType::COUNT:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Filter type only defined for qualitative results.");
                }
            } else {
                switch (ft) {
                    case storm::modelchecker::FilterType::VALUES:
                        STORM_PRINT_AND_LOG(*result << std::endl);
                        break;
                    case storm::modelchecker::FilterType::EXISTS:
                        STORM_PRINT_AND_LOG(result->asQualitativeCheckResult().existsTrue());
                        break;
                    case storm::modelchecker::FilterType::FORALL:
                        STORM_PRINT_AND_LOG(result->asQualitativeCheckResult().forallTrue());
                        break;
                    case storm::modelchecker::FilterType::COUNT:
                        STORM_PRINT_AND_LOG(result->asQualitativeCheckResult().count());
                        break;
                    case storm::modelchecker::FilterType::ARGMIN:
                    case storm::modelchecker::FilterType::ARGMAX:
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Outputting states is not supported.");
                    case storm::modelchecker::FilterType::SUM:
                    case storm::modelchecker::FilterType::AVG:
                    case storm::modelchecker::FilterType::MIN:
                    case storm::modelchecker::FilterType::MAX:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Filter type only defined for quantitative results.");
                }
            }
            STORM_PRINT_AND_LOG(std::endl);
        }
        
        void printModelCheckingProperty(storm::jani::Property const& property) {
            STORM_PRINT_AND_LOG(std::endl << "Model checking property " << *property.getRawFormula() << " ..." << std::endl);
        }

        template<typename ValueType>
        void printResult(std::unique_ptr<storm::modelchecker::CheckResult> const& result, storm::jani::Property const& property, storm::utility::Stopwatch* watch = nullptr) {
            if (result) {
                std::stringstream ss;
                ss << "'" << *property.getFilter().getStatesFormula() << "'";
                STORM_PRINT_AND_LOG("Result (for " << (property.getFilter().getStatesFormula()->isInitialFormula() ? "initial" : ss.str()) << " states): ");
                printFilteredResult<ValueType>(result, property.getFilter().getFilterType());
                if (watch) {
                    STORM_PRINT_AND_LOG("Time for model checking: " << *watch << "." << std::endl);
                }
            } else {
                STORM_PRINT_AND_LOG(" failed, property is unsupported by selected engine/settings." << std::endl);
            }
        }
        
        struct PostprocessingIdentity {
            void operator()(std::unique_ptr<storm::modelchecker::CheckResult> const&) {
                // Intentionally left empty.
            }
        };
        
        template<typename ValueType>
        void verifyProperties(std::vector<storm::jani::Property> const& properties, std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states)> const& verificationCallback, std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> const& postprocessingCallback = PostprocessingIdentity()) {
            for (auto const& property : properties) {
                printModelCheckingProperty(property);
                storm::utility::Stopwatch watch(true);
                std::unique_ptr<storm::modelchecker::CheckResult> result = verificationCallback(property.getRawFormula(), property.getFilter().getStatesFormula());
                watch.stop();
                postprocessingCallback(result);
                printResult<ValueType>(result, property, &watch);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithAbstractionRefinementEngine(SymbolicInput const& input) {
            STORM_LOG_ASSERT(input.model, "Expected symbolic model description.");
            verifyProperties<ValueType>(input.properties, [&input] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Abstraction-refinement can only filter initial states.");
                return storm::api::verifyWithAbstractionRefinementEngine<DdType, ValueType>(input.model.get(), storm::api::createTask<ValueType>(formula, true));
            });
        }

        template <typename ValueType>
        void verifyWithExplorationEngine(SymbolicInput const& input) {
            STORM_LOG_ASSERT(input.model, "Expected symbolic model description.");
            STORM_LOG_THROW((std::is_same<ValueType, double>::value), storm::exceptions::NotSupportedException, "Exploration does not support other data-types than floating points.");
            verifyProperties<ValueType>(input.properties, [&input] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                STORM_LOG_THROW(states->isInitialFormula(), storm::exceptions::NotSupportedException, "Exploration can only filter initial states.");
                return storm::api::verifyWithExplorationEngine<ValueType>(input.model.get(), storm::api::createTask<ValueType>(formula, true));
            });
        }
        
        template <typename ValueType>
        void verifyWithSparseEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            auto sparseModel = model->as<storm::models::sparse::Model<ValueType>>();
            verifyProperties<ValueType>(input.properties,
                                        [&sparseModel] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                                            bool filterForInitialStates = states->isInitialFormula();
                                            auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);
                                            std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithSparseEngine<ValueType>(sparseModel, task);
                                            
                                            std::unique_ptr<storm::modelchecker::CheckResult> filter;
                                            if (filterForInitialStates) {
                                                filter = std::make_unique<storm::modelchecker::ExplicitQualitativeCheckResult>(sparseModel->getInitialStates());
                                            } else {
                                                filter = storm::api::verifyWithSparseEngine<ValueType>(sparseModel, storm::api::createTask<ValueType>(states, false));
                                            }
                                            result->filter(filter->asQualitativeCheckResult());
                                            return result;
                                        });
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithHybridEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            verifyProperties<ValueType>(input.properties, [&model] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                bool filterForInitialStates = states->isInitialFormula();
                auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);
                
                auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
                std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithHybridEngine<DdType, ValueType>(symbolicModel, task);
                
                std::unique_ptr<storm::modelchecker::CheckResult> filter;
                if (filterForInitialStates) {
                    filter = std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<DdType>>(symbolicModel->getReachableStates(), symbolicModel->getInitialStates());
                } else {
                    filter = storm::api::verifyWithHybridEngine<DdType, ValueType>(symbolicModel, storm::api::createTask<ValueType>(states, false));
                }
                
                result->filter(filter->asQualitativeCheckResult());
                return result;
            });
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void verifyWithDdEngine(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input) {
            verifyProperties<ValueType>(input.properties, [&model] (std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
                bool filterForInitialStates = states->isInitialFormula();
                auto task = storm::api::createTask<ValueType>(formula, filterForInitialStates);

                auto symbolicModel = model->as<storm::models::symbolic::Model<DdType, ValueType>>();
                std::unique_ptr<storm::modelchecker::CheckResult> result = storm::api::verifyWithDdEngine<DdType, ValueType>(model->as<storm::models::symbolic::Model<DdType, ValueType>>(), storm::api::createTask<ValueType>(formula, true));

                std::unique_ptr<storm::modelchecker::CheckResult> filter;
                if (filterForInitialStates) {
                    filter = std::make_unique<storm::modelchecker::SymbolicQualitativeCheckResult<DdType>>(symbolicModel->getReachableStates(), symbolicModel->getInitialStates());
                } else {
                    filter = storm::api::verifyWithDdEngine<DdType, ValueType>(symbolicModel, storm::api::createTask<ValueType>(states, false));
                }
                
                result->filter(filter->asQualitativeCheckResult());
                return result;
            });
        }

        template <storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<DdType != storm::dd::DdType::CUDD || std::is_same<ValueType, double>::value, void>::type verifySymbolicModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, storm::settings::modules::CoreSettings const& coreSettings) {
            bool hybrid = coreSettings.getEngine() == storm::settings::modules::CoreSettings::Engine::Hybrid;
            if (hybrid) {
                verifyWithHybridEngine<DdType, ValueType>(model, input);
            } else {
                verifyWithDdEngine<DdType, ValueType>(model, input);
            }
        }

        template <storm::dd::DdType DdType, typename ValueType>
        typename std::enable_if<DdType == storm::dd::DdType::CUDD && !std::is_same<ValueType, double>::value, void>::type verifySymbolicModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, storm::settings::modules::CoreSettings const& coreSettings) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "CUDD does not support the selected data-type.");
        }

        template <storm::dd::DdType DdType, typename ValueType>
        void verifyModel(std::shared_ptr<storm::models::ModelBase> const& model, SymbolicInput const& input, storm::settings::modules::CoreSettings const& coreSettings) {
            if (model->isSparseModel()) {
                verifyWithSparseEngine<ValueType>(model, input);
            } else {
                STORM_LOG_ASSERT(model->isSymbolicModel(), "Unexpected model type.");
                verifySymbolicModel<DdType, ValueType>(model, input, coreSettings);
            }
        }
        
        template <storm::dd::DdType DdType, typename ValueType>
        void processInputWithValueTypeAndDdlib(SymbolicInput const& input) {
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            
            // For several engines, no model building step is performed, but the verification is started right away.
            storm::settings::modules::CoreSettings::Engine engine = coreSettings.getEngine();
            if (engine == storm::settings::modules::CoreSettings::Engine::AbstractionRefinement) {
                verifyWithAbstractionRefinementEngine<DdType, ValueType>(input);
            } else if (engine == storm::settings::modules::CoreSettings::Engine::Exploration) {
                verifyWithExplorationEngine<ValueType>(input);
            } else {
                auto ioSettings = storm::settings::getModule<storm::settings::modules::IOSettings>();
                
                std::shared_ptr<storm::models::ModelBase> model;
                if (!ioSettings.isNoBuildModelSet()) {
                    model = buildModel<DdType, ValueType>(engine, input, ioSettings);
                }
                
                if (model) {
                    model->printModelInformationToStream(std::cout);
                }
                
                STORM_LOG_THROW(model || input.properties.empty(), storm::exceptions::InvalidSettingsException, "No input model.");
                
                if (model) {
                    auto preprocessingResult = preprocessModel<DdType, ValueType>(model, input);
                    if (preprocessingResult.second) {
                        model = preprocessingResult.first;
                        model->printModelInformationToStream(std::cout);
                    }
                }
                
                if (model) {
                    exportModel<DdType, ValueType>(model, input);
                    
                    if (coreSettings.isCounterexampleSet()) {
                        generateCounterexamples<ValueType>(model, input);
                    } else {
                        verifyModel<DdType, ValueType>(model, input, coreSettings);
                    }
                }
            }
        }
        
        template <typename ValueType>
        void processInputWithValueType(SymbolicInput const& input) {
            auto coreSettings = storm::settings::getModule<storm::settings::modules::CoreSettings>();
            
            if (coreSettings.getDdLibraryType() == storm::dd::DdType::CUDD) {
                processInputWithValueTypeAndDdlib<storm::dd::DdType::CUDD, ValueType>(input);
            } else {
                STORM_LOG_ASSERT(coreSettings.getDdLibraryType() == storm::dd::DdType::Sylvan, "Unknown DD library.");
                processInputWithValueTypeAndDdlib<storm::dd::DdType::Sylvan, ValueType>(input);
            }
        }
        
        void processOptions() {
            // Start by setting some urgent options (log levels, resources, etc.)
            setUrgentOptions();
            
            // Parse and preprocess symbolic input (PRISM, JANI, properties, etc.)
            SymbolicInput symbolicInput = parseAndPreprocessSymbolicInput();
            
            auto generalSettings = storm::settings::getModule<storm::settings::modules::GeneralSettings>();
            if (generalSettings.isParametricSet()) {
#ifdef STORM_HAVE_CARL
                processInputWithValueType<storm::RationalFunction>(symbolicInput);
#else
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No parameters are supported in this build.");
#endif
            } else if (generalSettings.isExactSet()) {
#ifdef STORM_HAVE_CARL
                processInputWithValueType<storm::RationalNumber>(symbolicInput);
#else
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "No exact numbers are supported in this build.");
#endif
            } else {
                processInputWithValueType<double>(symbolicInput);
            }
        }

        void printTimeAndMemoryStatistics(uint64_t wallclockMilliseconds) {
            struct rusage ru;
            getrusage(RUSAGE_SELF, &ru);
            
            std::cout << std::endl << "Performance statistics:" << std::endl;
#ifdef MACOS
            // For Mac OS, this is returned in bytes.
            uint64_t maximumResidentSizeInMegabytes = ru.ru_maxrss / 1024 / 1024;
#endif
#ifdef LINUX
            // For Linux, this is returned in kilobytes.
            uint64_t maximumResidentSizeInMegabytes = ru.ru_maxrss / 1024;
#endif
            std::cout << "  * peak memory usage: " << maximumResidentSizeInMegabytes << "MB" << std::endl;
            char oldFillChar = std::cout.fill('0');
            std::cout << "  * CPU time: " << ru.ru_utime.tv_sec << "." << std::setw(3) << ru.ru_utime.tv_usec/1000 << "s" << std::endl;
            if (wallclockMilliseconds != 0) {
                std::cout << "  * wallclock time: " << (wallclockMilliseconds/1000) << "." << std::setw(3) << (wallclockMilliseconds % 1000) << "s" << std::endl;
            }
            std::cout.fill(oldFillChar);
        }
        
    }
}
