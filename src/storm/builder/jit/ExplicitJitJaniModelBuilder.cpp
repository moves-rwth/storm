#include "storm/builder/jit/ExplicitJitJaniModelBuilder.h"

#include <errno.h>
#include <chrono>
#include <cstdio>
#include <iostream>

#include "storm/solver/SmtSolver.h"

#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/EdgeDestination.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/ParallelComposition.h"
#include "storm/storage/jani/Variable.h"
#include "storm/storage/jani/types/AllJaniTypes.h"
#include "storm/storage/jani/visitor/CompositionInformationVisitor.h"

#include "storm/builder/RewardModelInformation.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/macros.h"
#include "storm/utility/solver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/JitBuilderSettings.h"

#include "storm/settings/modules/BuildSettings.h"

#include "storm-config.h"
#include "storm/utility/OsDetection.h"

namespace storm {
namespace builder {
namespace jit {

static const std::string JIT_VARIABLE_EXTENSION = "_jit_";

#ifdef LINUX
static const std::string DYLIB_EXTENSION = ".so";
#endif
#ifdef MACOSX
static const std::string DYLIB_EXTENSION = ".dylib";
#endif
#ifdef WINDOWS
static const std::string DYLIB_EXTENSION = ".dll";
#endif

template<typename ValueType, typename RewardModelType>
storm::jani::ModelFeatures ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getSupportedJaniFeatures() {
    storm::jani::ModelFeatures features;
    features.add(storm::jani::ModelFeature::DerivedOperators);
    features.add(storm::jani::ModelFeature::StateExitRewards);
    // We do not add Functions and arrays as these should ideally be substituted before creating this generator.
    // This is because functions or arrays may also occur in properties and the user of this builder should take care of that.
    return features;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::canHandle(storm::jani::Model const& model) {
    // Check jani features
    auto features = model.getModelFeatures();
    features.remove(storm::jani::ModelFeature::Arrays);  // can be substituted
    features.remove(storm::jani::ModelFeature::DerivedOperators);
    features.remove(storm::jani::ModelFeature::Functions);  // can be substituted
    features.remove(storm::jani::ModelFeature::StateExitRewards);
    if (!features.empty()) {
        STORM_LOG_INFO("Jit engine can not build Jani model due to unsupported jani features.");
        return false;
    }
    // There probably are more cases where the model is unsupported. However, checking these is often more involved.
    // As this method is supposed to be a quick check, we just return true at this point.
    return true;
}

template<typename ValueType, typename RewardModelType>
ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::ExplicitJitJaniModelBuilder(storm::jani::Model const& model,
                                                                                     storm::builder::BuilderOptions const& options)
    : options(options), model(model.substituteConstantsFunctions()), modelComponentsBuilder(model.getModelType()) {
    // Load all options from the settings module.
    storm::settings::modules::JitBuilderSettings const& settings = storm::settings::getModule<storm::settings::modules::JitBuilderSettings>();
    if (settings.isCompilerSet()) {
        compiler = settings.getCompiler();
    } else {
        const char* cxxEnv = std::getenv("CXX");
        if (cxxEnv != nullptr) {
            compiler = std::string(cxxEnv);
        }
        if (compiler.empty()) {
            compiler = "c++";
        }
    }
    if (settings.isCompilerFlagsSet()) {
        compilerFlags = settings.getCompilerFlags();
    } else {
        std::stringstream flagStream;
#ifdef LINUX
        flagStream << "-std=c++14 -fPIC -march=native -shared ";
#endif
#ifdef MACOSX
        flagStream << "-std=c++14 -stdlib=libc++ -fPIC -march=native -shared -undefined dynamic_lookup ";
#endif

        flagStream << "-O" << settings.getOptimizationLevel();
        compilerFlags = flagStream.str();
    }
    if (settings.isBoostIncludeDirectorySet()) {
        boostIncludeDirectory = settings.getBoostIncludeDirectory();
    } else {
        boostIncludeDirectory = STORM_BOOST_INCLUDE_DIR;
    }
    if (settings.isStormIncludeDirectorySet()) {
        stormIncludeDirectory = settings.getStormIncludeDirectory();
    } else {
        stormIncludeDirectory = STORM_BUILD_DIR "/include";
    }
    if (settings.isCarlIncludeDirectorySet()) {
        carlIncludeDirectory = settings.getCarlIncludeDirectory();
    } else {
        carlIncludeDirectory = STORM_CARL_INCLUDE_DIR;
    }
#ifdef STORM_HAVE_CLN
    clnIncludeDirectory = CLN_INCLUDE_DIR;
#else
    clnIncludeDirectory = "";
#endif
#ifdef STORM_HAVE_GMP
    gmpIncludeDirectory = GMP_INCLUDE_DIR;
#else
    gmpIncludeDirectory = "";
#endif
    sparseppIncludeDirectory = STORM_BUILD_DIR "/include/resources/3rdparty/sparsepp/";

    // Register all transient variables as transient.
    for (auto const& variable : this->model.getGlobalVariables().getTransientVariables()) {
        transientVariables.insert(variable.getExpressionVariable());
    }

    // Construct vector of the automata to be put in parallel.
    storm::jani::Composition const& topLevelComposition = this->model.getSystemComposition();
    if (topLevelComposition.isAutomatonComposition()) {
        parallelAutomata.push_back(this->model.getAutomaton(topLevelComposition.asAutomatonComposition().getAutomatonName()));
    } else {
        STORM_LOG_THROW(topLevelComposition.isParallelComposition(), storm::exceptions::WrongFormatException, "Expected parallel composition.");
        storm::jani::ParallelComposition const& parallelComposition = topLevelComposition.asParallelComposition();

        for (auto const& composition : parallelComposition.getSubcompositions()) {
            STORM_LOG_THROW(composition->isAutomatonComposition(), storm::exceptions::WrongFormatException, "Expected flat parallel composition.");
            parallelAutomata.push_back(this->model.getAutomaton(composition->asAutomatonComposition().getAutomatonName()));
        }
    }

    // If the program still contains undefined constants and we are not in a parametric setting, assemble an appropriate error message.
#ifdef STORM_HAVE_CARL
    if (!std::is_same<ValueType, storm::RationalFunction>::value && this->model.hasUndefinedConstants()) {
#else
    if (this->model.hasUndefinedConstants()) {
#endif
        std::vector<std::reference_wrapper<storm::jani::Constant const>> undefinedConstants = this->model.getUndefinedConstants();
        std::stringstream stream;
        bool printComma = false;
        for (auto const& constant : undefinedConstants) {
            if (printComma) {
                stream << ", ";
            } else {
                printComma = true;
            }
            stream << constant.get().getName() << " (" << constant.get().getType() << ")";
        }
        stream << ".";
        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Model still contains these undefined constants: " + stream.str());
    }
#ifdef STORM_HAVE_CARL
    else if (std::is_same<ValueType, storm::RationalFunction>::value && !this->model.undefinedConstantsAreGraphPreserving()) {
        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException,
                        "The input model contains undefined constants that influence the graph structure of the underlying model, which is not allowed.");
    }
#endif
    auto features = model.getModelFeatures();
    features.remove(storm::jani::ModelFeature::DerivedOperators);
    features.remove(storm::jani::ModelFeature::StateExitRewards);
    STORM_LOG_THROW(features.empty(), storm::exceptions::InvalidArgumentException,
                    "The jit model builder does not support the following model feature(s): " << features.toString() << ".");

    // STORM_LOG_THROW(!model.reusesActionsInComposition(), storm::exceptions::InvalidArgumentException, "The jit JANI model builder currently does not support
    // reusing actions in parallel composition");

    // Comment this in to print the JANI model for debugging purposes.
    // this->model.makeStandardJaniCompliant();
    // storm::jani::JsonExporter::toStream(this->model, std::vector<std::shared_ptr<storm::logic::Formula const>>(), std::cout, false);
}

template<typename ValueType, typename RewardModelType>
boost::optional<std::string> ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::execute(std::string command) {
    auto start = std::chrono::high_resolution_clock::now();
    char buffer[128];
    std::stringstream output;
    command += " 2>&1";

    STORM_LOG_TRACE("Executing command: " << command);

    std::unique_ptr<FILE> pipe(popen(command.c_str(), "r"));
    STORM_LOG_THROW(pipe, storm::exceptions::InvalidStateException, "Call to popen failed: " << strerror(errno));

    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != nullptr)
            output << buffer;
    }
    int result = pclose(pipe.get());
    pipe.release();

    auto end = std::chrono::high_resolution_clock::now();
    STORM_LOG_TRACE("Executing command took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");

    if (WEXITSTATUS(result) == 0) {
        return boost::none;
    } else {
        return "Executing command failed. Got response: " + output.str();
    }
}

template<typename ValueType, typename RewardModelType>
boost::filesystem::path ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::writeToTemporaryFile(std::string const& content, std::string const& suffix) {
    boost::filesystem::path temporaryFile = boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%" + suffix);
    std::ofstream out(temporaryFile.native());
    out << content << '\n';
    out.close();
    return temporaryFile;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::checkTemporaryFileWritable() const {
    bool result = true;
    std::string problem = "Unable to write to a temporary file. Are the appropriate permissions set?";
    try {
        boost::filesystem::path temporaryFile = writeToTemporaryFile("Hello world.");
    } catch (std::exception const& e) {
        result = false;
        STORM_LOG_ERROR(problem);
    }

    return result;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::checkCompilerWorks() const {
    bool result = true;
    std::string problem = "Unable to compile empty program with C++ compiler. Is the C++ compiler '" + compiler + "' installed and working?";
    try {
        std::string emptyProgram = R"(
                    int main() {
                        return 0;
                    }
                    )";
        boost::filesystem::path temporaryFile = writeToTemporaryFile(emptyProgram);
        std::string temporaryFilename = boost::filesystem::absolute(temporaryFile).string();
        boost::filesystem::path outputFile = temporaryFile;
        outputFile += ".out";
        std::string outputFilename = boost::filesystem::absolute(outputFile).string();
        boost::optional<std::string> error = execute(compiler + " " + temporaryFilename + " -o " + outputFilename);

        if (error) {
            result = false;
            STORM_LOG_ERROR(problem);
            STORM_LOG_TRACE(error.get());
        } else {
            boost::filesystem::remove(outputFile);
        }
    } catch (std::exception const& e) {
        result = false;
        STORM_LOG_ERROR(problem);
    }

    return result;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::checkCompilerFlagsWork() const {
    bool result = true;
    std::string problem = "Unable to compile program with the flags '" + compilerFlags + "'. Does the C++ compiler accept these flags?";
    try {
        std::string emptyProgram = R"(
#include <cstdint>
                    
                    int main() {
                        return 0;
                    }
                    )";
        boost::filesystem::path temporaryFile = writeToTemporaryFile(emptyProgram);
        std::string temporaryFilename = boost::filesystem::absolute(temporaryFile).string();
        boost::filesystem::path outputFile = temporaryFile;
        outputFile += ".out";
        std::string outputFilename = boost::filesystem::absolute(outputFile).string();
        boost::optional<std::string> error = execute(compiler + " " + compilerFlags + " " + temporaryFilename + " -o " + outputFilename);

        if (error) {
            result = false;
            STORM_LOG_ERROR(problem);
            STORM_LOG_TRACE(error.get());
        } else {
            boost::filesystem::remove(outputFile);
        }
    } catch (std::exception const& e) {
        result = false;
        STORM_LOG_ERROR(problem);
    }

    return result;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::checkBoostAvailable() const {
    bool result = true;
    std::string problem = "Unable to compile program using boost. Is boosts's include directory '" + boostIncludeDirectory + "' set correctly?";
    try {
        std::string program = R"(
#include <boost/optional.hpp>
                    
                    int main() {
                        return 0;
                    }
                    )";
        boost::filesystem::path temporaryFile = writeToTemporaryFile(program);
        std::string temporaryFilename = boost::filesystem::absolute(temporaryFile).string();
        boost::filesystem::path outputFile = temporaryFile;
        outputFile += ".out";
        std::string outputFilename = boost::filesystem::absolute(outputFile).string();
        boost::optional<std::string> error =
            execute(compiler + " " + compilerFlags + " " + temporaryFilename + " -I" + boostIncludeDirectory + " -o " + outputFilename);

        if (error) {
            result = false;
            STORM_LOG_ERROR(problem);
            STORM_LOG_TRACE(error.get());
        } else {
            boost::filesystem::remove(outputFile);
        }
    } catch (std::exception const& e) {
        result = false;
        STORM_LOG_ERROR(problem);
    }
    return result;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::checkBoostDllAvailable() const {
    bool result = true;
    std::string problem = "Unable to compile program using boost's dll library. Is boosts's include directory '" + boostIncludeDirectory +
                          "' pointing to a boost installation with version at least 1.61?";
    try {
        std::string program = R"(
#include <boost/dll/alias.hpp>
                    
                    int main() {
                        return 0;
                    }
                    )";
        boost::filesystem::path temporaryFile = writeToTemporaryFile(program);
        std::string temporaryFilename = boost::filesystem::absolute(temporaryFile).string();
        boost::filesystem::path outputFile = temporaryFile;
        outputFile += ".out";
        std::string outputFilename = boost::filesystem::absolute(outputFile).string();
        boost::optional<std::string> error =
            execute(compiler + " " + compilerFlags + " " + temporaryFilename + " -I" + boostIncludeDirectory + " -o " + outputFilename);

        if (error) {
            result = false;
            STORM_LOG_ERROR(problem);
            STORM_LOG_TRACE(error.get());
        } else {
            boost::filesystem::remove(outputFile);
        }
    } catch (std::exception const& e) {
        result = false;
        STORM_LOG_ERROR(problem);
    }
    return result;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::checkStormHeadersAvailable() const {
    bool result = true;
    std::string problem = "Unable to compile program using storm headers. Is storm's include directory '" + stormIncludeDirectory +
                          "' set correctly? Does the directory contain all the headers (in particular 'storm-config.h')?";
    try {
        std::string program = R"(
#include "storm-config.h"
#include "storm/builder/RewardModelInformation.h"
                    
                    int main() {
                        return 0;
                    }
                    )";
        boost::filesystem::path temporaryFile = writeToTemporaryFile(program);
        std::string temporaryFilename = boost::filesystem::absolute(temporaryFile).string();
        boost::filesystem::path outputFile = temporaryFile;
        outputFile += ".out";
        std::string outputFilename = boost::filesystem::absolute(outputFile).string();
        boost::optional<std::string> error =
            execute(compiler + " " + compilerFlags + " " + temporaryFilename + " -I" + stormIncludeDirectory + " -o " + outputFilename);

        if (error) {
            result = false;
            STORM_LOG_ERROR(problem);
            STORM_LOG_TRACE(error.get());
        } else {
            boost::filesystem::remove(outputFile);
        }
    } catch (std::exception const& e) {
        result = false;
        STORM_LOG_ERROR(problem);
    }
    return result;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::checkCarlAvailable() const {
    bool result = true;
    std::string problem = "Unable to compile program using Carl data structures. Is Carls's include directory '" + carlIncludeDirectory + "' set correctly?";
    try {
        std::string program = R"(
#include "storm/adapters/RationalFunctionAdapter.h"
                        
                    int main() {
                        return 0;
                    }
                    )";
        boost::filesystem::path temporaryFile = writeToTemporaryFile(program);
        std::string temporaryFilename = boost::filesystem::absolute(temporaryFile).string();
        boost::filesystem::path outputFile = temporaryFile;
        outputFile += ".out";
        std::string outputFilename = boost::filesystem::absolute(outputFile).string();
        std::string includes = "";
        for (std::string const& dir : {stormIncludeDirectory, carlIncludeDirectory, clnIncludeDirectory, gmpIncludeDirectory}) {
            if (dir != "") {
                includes += " -I" + dir;
            }
        }
        boost::optional<std::string> error = execute(compiler + " " + compilerFlags + " " + temporaryFilename + includes + " -o " + outputFilename);

        if (error) {
            result = false;
            STORM_LOG_ERROR(problem);
            STORM_LOG_TRACE(error.get());
        } else {
            boost::filesystem::remove(outputFile);
        }
    } catch (std::exception const& e) {
        result = false;
        STORM_LOG_ERROR(problem);
    }
    return result;
}

template<typename ValueType, typename RewardModelType>
bool ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::doctor() const {
    bool result = true;

    STORM_LOG_DEBUG("Checking whether temporary file is writable.");
    result = checkTemporaryFileWritable();
    if (!result) {
        return result;
    }
    STORM_LOG_DEBUG("Success.");

    STORM_LOG_DEBUG("Checking whether compiler invocation works.");
    result = checkCompilerWorks();
    if (!result) {
        return result;
    }
    STORM_LOG_DEBUG("Success.");

    STORM_LOG_DEBUG("Checking whether compiler flags work.");
    result = checkCompilerFlagsWork();
    if (!result) {
        return result;
    }
    STORM_LOG_DEBUG("Success.");

    STORM_LOG_DEBUG("Checking whether boost is available.");
    result = checkBoostAvailable();
    if (!result) {
        return result;
    }
    STORM_LOG_DEBUG("Success.");

    STORM_LOG_DEBUG("Checking whether boost dll library is available.");
    result = checkBoostDllAvailable();
    if (!result) {
        return result;
    }
    STORM_LOG_DEBUG("Success.");

    STORM_LOG_DEBUG("Checking whether Storm's headers are available.");
    result = checkStormHeadersAvailable();
    if (!result) {
        return result;
    }
    STORM_LOG_DEBUG("Success.");

    if (std::is_same<storm::RationalNumber, ValueType>::value) {
        STORM_LOG_DEBUG("Checking whether Carl's headers are available.");
        result = checkCarlAvailable();
        if (!result) {
            return result;
        }
        STORM_LOG_DEBUG("Success.");
    }

    return result;
}

template<typename ValueType, typename RewardModelType>
std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::build() {
    // (0) Assemble information about the model.
    cpptempl::data_map modelData = generateModelData();

    // (1) generate the source code of the shared library
    std::string source;
    try {
        source = createSourceCodeFromSkeleton(modelData);
    } catch (std::exception const& e) {
        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not create the source code for model generation (error: " << e.what() << ").");
    }
    STORM_LOG_TRACE("Successfully created source code for model generation: " << source);

    // (2) Write the source code to a temporary file.
    boost::filesystem::path temporarySourceFile = writeToTemporaryFile(source);

    // (3) Compile the source code to a shared library.
    boost::filesystem::path dynamicLibraryPath = compileToSharedLibrary(temporarySourceFile);
    STORM_LOG_TRACE("Successfully compiled shared library.");

    // (4) Remove the source code of the shared library we just compiled.
    boost::filesystem::remove(temporarySourceFile);

    // (5) Create the builder from the shared library.
    createBuilder(dynamicLibraryPath);

    // (6) Execute the build function of the builder in the shared library and build the actual model.
    auto start = std::chrono::high_resolution_clock::now();

    std::shared_ptr<storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>> sparseModel(nullptr);
    boost::optional<std::string> error;
    try {
        sparseModel = std::shared_ptr<storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>>(builder->build());
        STORM_LOG_THROW(sparseModel, storm::exceptions::UnexpectedException, "An unexpected error occurred.");
        STORM_LOG_TRACE("Successfully got model from jit-builder.");
    } catch (std::exception const& e) {
        error = e.what();
    }

    auto end = std::chrono::high_resolution_clock::now();
    STORM_LOG_TRACE("Building model took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");

    // (7) Delete the shared library.
    boost::filesystem::remove(dynamicLibraryPath);

    STORM_LOG_THROW(!error, storm::exceptions::WrongFormatException, "Model building failed. Reason: " << error.get());

    // Return the constructed model.
    return sparseModel;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateModelData() {
    cpptempl::data_map modelData;
    // Generate trivial model-information.
    modelData["deterministic_model"] = asString(model.isDeterministicModel());
    modelData["discrete_time_model"] = asString(model.isDiscreteTimeModel());

    // Use lists here to enable if query in skeleton program even if the property is just boolean.
    cpptempl::data_list list;
    if (options.isExplorationChecksSet()) {
        list.push_back(cpptempl::data_map());
    }
    modelData["exploration_checks"] = cpptempl::make_data(list);
    list = cpptempl::data_list();
    if (options.isShowProgressSet()) {
        list.push_back(cpptempl::data_map());
    }
    modelData["expl_progress"] = cpptempl::make_data(list);

    std::stringstream progressDelayStream;
    progressDelayStream << options.getShowProgressDelay();
    modelData["expl_progress_interval"] = cpptempl::make_data(progressDelayStream.str());
    list = cpptempl::data_list();
    if (std::is_same<storm::RationalNumber, ValueType>::value) {
        list.push_back(cpptempl::data_map());
    }
    modelData["exact"] = cpptempl::make_data(list);
    list = cpptempl::data_list();
    if (std::is_same<storm::RationalFunction, ValueType>::value) {
        list.push_back(cpptempl::data_map());
    }
    modelData["parametric"] = cpptempl::make_data(list);
    list = cpptempl::data_list();
    if (std::is_same<double, ValueType>::value) {
        list.push_back(cpptempl::data_map());
    }
    modelData["double"] = cpptempl::make_data(list);

    list = cpptempl::data_list();
    if (storm::settings::getModule<storm::settings::modules::BuildSettings>().isDontFixDeadlocksSet()) {
        list.push_back(cpptempl::data_map());
    }
    modelData["dontFixDeadlocks"] = cpptempl::make_data(list);

    // If we are building a possibly parametric model, we need to create the parameters.
    if (std::is_same<storm::RationalFunction, ValueType>::value) {
        generateParameters(modelData);
    }

    // Generate non-trivial model-information.
    generateVariables(modelData);
    generateInitialStates(modelData);
    generateRewards(modelData);  // We need to generate the reward information before the edges, because we already use it there.
    generateEdges(modelData);
    generateLocations(modelData);
    generateLabels(modelData);
    generateTerminalExpressions(modelData);

    return modelData;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateBooleanVariable(storm::jani::Variable const& variable) {
    STORM_LOG_ASSERT(variable.getType().isBasicType() && variable.getType().asBasicType().isBooleanType(),
                     "Cannot generate BooleanVariable for non BooleanVariable");
    cpptempl::data_map result;
    result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
    if (variable.hasInitExpression()) {
        result["init"] = asString(variable.getInitExpression().evaluateAsBool());
    }
    return result;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateBoundedIntegerVariable(storm::jani::Variable const& variable) {
    STORM_LOG_ASSERT(variable.getType().isBoundedType() && variable.getType().asBoundedType().isIntegerType(),
                     "Cannot generate BoundedIntegerVariable for non BoundedIntegerVariable");
    cpptempl::data_map result;
    auto const& type = variable.getType().asBoundedType();
    STORM_LOG_THROW(type.hasLowerBound() && type.hasUpperBound(), storm::exceptions::NotSupportedException,
                    "Bounded integer variable is only bounded from one side. This is not supported by the jit builder.");

    int_fast64_t lowerBound = type.getLowerBound().evaluateAsInt();
    int_fast64_t upperBound = type.getUpperBound().evaluateAsInt();

    lowerBounds[variable.getExpressionVariable()] = lowerBound;
    if (lowerBound != 0) {
        lowerBoundShiftSubstitution[variable.getExpressionVariable()] = variable.getExpressionVariable() - model.getManager().integer(-lowerBound);
    }
    uint64_t range = static_cast<uint64_t>(upperBound - lowerBound + 1);
    uint64_t numberOfBits = static_cast<uint64_t>(std::ceil(std::log2(range)));

    result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
    result["original_name"] = variable.getExpressionVariable().getName();
    result["numberOfBits"] = std::to_string(numberOfBits);
    if (variable.hasInitExpression()) {
        result["init"] = asString(variable.getInitExpression().evaluateAsInt() - lowerBound);
    }

    result["lower"] = asString(lowerBound);
    result["upper"] = asString(upperBound);

    return result;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateUnboundedIntegerVariable(storm::jani::Variable const& variable) {
    STORM_LOG_ASSERT(variable.getType().isBasicType() && variable.getType().asBasicType().isIntegerType(),
                     "Cannot generate IntegerVariable for non IntegerVariable");

    cpptempl::data_map result;

    result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
    if (variable.hasInitExpression()) {
        result["init"] = asString(variable.getInitExpression().evaluateAsInt());
    }

    return result;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateRealVariable(storm::jani::Variable const& variable) {
    STORM_LOG_ASSERT(variable.getType().isBasicType() && variable.getType().asBasicType().isRealType(), "Cannot generate RealVariable for non RealVariable");

    cpptempl::data_map result;

    result["name"] = registerVariable(variable.getExpressionVariable(), variable.isTransient());
    realVariables.insert(variable.getExpressionVariable());
    if (variable.hasInitExpression()) {
        if (std::is_same<double, ValueType>::value) {
            result["init"] = expressionTranslator.translate(
                variable.getInitExpression(),
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastDouble));
        } else if (std::is_same<storm::RationalNumber, ValueType>::value) {
            result["init"] = expressionTranslator.translate(
                variable.getInitExpression(),
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalNumber));
        } else if (std::is_same<storm::RationalFunction, ValueType>::value) {
            result["init"] = expressionTranslator.translate(
                variable.getInitExpression(),
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalFunction));
        }
    }

    return result;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLocationVariable(storm::jani::Automaton const& automaton) {
    cpptempl::data_map result;

    result["name"] = registerVariable(getLocationVariable(automaton), false);
    result["numberOfBits"] = static_cast<uint64_t>(std::ceil(std::log2(automaton.getNumberOfLocations())));

    return result;
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateVariables(cpptempl::data_map& modelData) {
    cpptempl::data_list nonTransientBooleanVariables;
    cpptempl::data_list transientBooleanVariables;
    cpptempl::data_list nonTransientBoundedIntegerVariables;
    cpptempl::data_list transientBoundedIntegerVariables;
    cpptempl::data_list nonTransientUnboundedIntegerVariables;
    cpptempl::data_list transientUnboundedIntegerVariables;
    cpptempl::data_list nonTransientRealVariables;
    cpptempl::data_list transientRealVariables;
    cpptempl::data_list locationVariables;

    for (auto const& variable : model.getGlobalVariables().getBooleanVariables()) {
        cpptempl::data_map newBooleanVariable = generateBooleanVariable(variable);
        if (variable.isTransient()) {
            transientBooleanVariables.push_back(newBooleanVariable);
        } else {
            nonTransientBooleanVariables.push_back(newBooleanVariable);
        }
    }
    for (auto const& variable : model.getGlobalVariables().getBoundedIntegerVariables()) {
        cpptempl::data_map newBoundedIntegerVariable = generateBoundedIntegerVariable(variable);
        if (variable.isTransient()) {
            transientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
        } else {
            nonTransientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
        }
    }
    for (auto const& variable : model.getGlobalVariables().getUnboundedIntegerVariables()) {
        cpptempl::data_map newUnboundedIntegerVariable = generateUnboundedIntegerVariable(variable);
        if (variable.isTransient()) {
            transientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
        } else {
            nonTransientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
        }
    }
    for (auto const& variable : model.getGlobalVariables().getRealVariables()) {
        cpptempl::data_map newRealVariable = generateRealVariable(variable);
        if (variable.isTransient()) {
            transientRealVariables.push_back(newRealVariable);
        } else {
            nonTransientRealVariables.push_back(newRealVariable);
        }
    }
    for (auto const& automatonRef : parallelAutomata) {
        storm::jani::Automaton const& automaton = automatonRef.get();
        for (auto const& variable : automaton.getVariables().getBooleanVariables()) {
            cpptempl::data_map newBooleanVariable = generateBooleanVariable(variable);
            if (variable.isTransient()) {
                transientBooleanVariables.push_back(newBooleanVariable);
            } else {
                nonTransientBooleanVariables.push_back(newBooleanVariable);
            }
        }
        for (auto const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
            cpptempl::data_map newBoundedIntegerVariable = generateBoundedIntegerVariable(variable);
            if (variable.isTransient()) {
                transientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
            } else {
                nonTransientBoundedIntegerVariables.push_back(newBoundedIntegerVariable);
            }
        }
        for (auto const& variable : automaton.getVariables().getUnboundedIntegerVariables()) {
            cpptempl::data_map newUnboundedIntegerVariable = generateUnboundedIntegerVariable(variable);
            if (variable.isTransient()) {
                transientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
            } else {
                nonTransientUnboundedIntegerVariables.push_back(newUnboundedIntegerVariable);
            }
        }
        for (auto const& variable : automaton.getVariables().getRealVariables()) {
            cpptempl::data_map newRealVariable = generateRealVariable(variable);
            if (variable.isTransient()) {
                transientRealVariables.push_back(newRealVariable);
            } else {
                nonTransientRealVariables.push_back(newRealVariable);
            }
        }

        // Only generate a location variable if there is more than one location for the automaton.
        if (automaton.getNumberOfLocations() > 1) {
            locationVariables.push_back(generateLocationVariable(automaton));
        }
    }

    cpptempl::data_map nonTransientVariables;
    nonTransientVariables["boolean"] = cpptempl::make_data(nonTransientBooleanVariables);
    nonTransientVariables["boundedInteger"] = cpptempl::make_data(nonTransientBoundedIntegerVariables);
    nonTransientVariables["unboundedInteger"] = cpptempl::make_data(nonTransientUnboundedIntegerVariables);
    nonTransientVariables["real"] = cpptempl::make_data(nonTransientRealVariables);
    nonTransientVariables["locations"] = cpptempl::make_data(locationVariables);
    modelData["nontransient_variables"] = nonTransientVariables;

    cpptempl::data_map transientVariables;
    transientVariables["boolean"] = cpptempl::make_data(transientBooleanVariables);
    transientVariables["boundedInteger"] = cpptempl::make_data(transientBoundedIntegerVariables);
    transientVariables["unboundedInteger"] = cpptempl::make_data(transientUnboundedIntegerVariables);
    transientVariables["real"] = cpptempl::make_data(transientRealVariables);
    modelData["transient_variables"] = transientVariables;
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateInitialStates(cpptempl::data_map& modelData) {
    cpptempl::data_list initialStates;

    // Prepare an SMT solver to enumerate all initial states.
    storm::utility::solver::SmtSolverFactory factory;
    std::unique_ptr<storm::solver::SmtSolver> solver = factory.create(model.getExpressionManager());

    std::vector<storm::expressions::Expression> rangeExpressions = model.getAllRangeExpressions();
    for (auto const& expression : rangeExpressions) {
        solver->add(expression);
    }
    solver->add(model.getInitialStatesExpression(parallelAutomata));

    // Proceed as long as the solver can still enumerate initial states.
    while (solver->check() == storm::solver::SmtSolver::CheckResult::Sat) {
        // Create fresh state.
        cpptempl::data_list initialStateAssignment;

        // Read variable assignment from the solution of the solver. Also, create an expression we can use to
        // prevent the variable assignment from being enumerated again.
        storm::expressions::Expression blockingExpression;
        std::shared_ptr<storm::solver::SmtSolver::ModelReference> model = solver->getModel();
        for (auto const& variable : this->model.getGlobalVariables().getBooleanVariables()) {
            if (variable.isTransient()) {
                continue;
            }

            storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
            bool variableValue = model->getBooleanValue(expressionVariable);
            initialStateAssignment.push_back(generateAssignment(variable, variableValue));

            storm::expressions::Expression localBlockingExpression = variableValue ? !expressionVariable : expressionVariable;
            blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
        }
        for (auto const& variable : this->model.getGlobalVariables().getBoundedIntegerVariables()) {
            if (variable.isTransient()) {
                continue;
            }

            storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
            int_fast64_t variableValue = model->getIntegerValue(expressionVariable);
            initialStateAssignment.push_back(generateAssignment(variable, variableValue));

            storm::expressions::Expression localBlockingExpression = expressionVariable != model->getManager().integer(variableValue);
            blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
        }
        for (auto const& automatonRef : parallelAutomata) {
            storm::jani::Automaton const& automaton = automatonRef.get();
            for (auto const& variable : automaton.getVariables().getBooleanVariables()) {
                if (variable.isTransient()) {
                    continue;
                }

                storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
                bool variableValue = model->getBooleanValue(expressionVariable);
                initialStateAssignment.push_back(generateAssignment(variable, variableValue));

                storm::expressions::Expression localBlockingExpression = variableValue ? !expressionVariable : expressionVariable;
                blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
            }
            for (auto const& variable : automaton.getVariables().getBoundedIntegerVariables()) {
                if (variable.isTransient()) {
                    continue;
                }

                storm::expressions::Variable const& expressionVariable = variable.getExpressionVariable();
                int_fast64_t variableValue = model->getIntegerValue(expressionVariable);
                initialStateAssignment.push_back(generateAssignment(variable, variableValue));

                storm::expressions::Expression localBlockingExpression = expressionVariable != model->getManager().integer(variableValue);
                blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
            }
        }

        // Gather iterators to the initial locations of all the automata.
        std::vector<std::set<uint64_t>::const_iterator> initialLocationsIterators;
        for (auto const& automaton : parallelAutomata) {
            initialLocationsIterators.push_back(automaton.get().getInitialLocationIndices().cbegin());
        }

        // Now iterate through all combinations of initial locations.
        while (true) {
            cpptempl::data_list completeAssignment(initialStateAssignment);

            for (uint64_t index = 0; index < initialLocationsIterators.size(); ++index) {
                storm::jani::Automaton const& automaton = parallelAutomata[index].get();
                if (automaton.getNumberOfLocations() > 1) {
                    completeAssignment.push_back(generateLocationAssignment(automaton, *initialLocationsIterators[index]));
                }
            }
            initialStates.push_back(cpptempl::make_data(completeAssignment));

            uint64_t index = 0;
            for (; index < initialLocationsIterators.size(); ++index) {
                ++initialLocationsIterators[index];
                if (initialLocationsIterators[index] == parallelAutomata[index].get().getInitialLocationIndices().cend()) {
                    initialLocationsIterators[index] = parallelAutomata[index].get().getInitialLocationIndices().cbegin();
                } else {
                    break;
                }
            }

            // If we are at the end, leave the loop.
            if (index == initialLocationsIterators.size()) {
                break;
            }
        }

        // Block the current initial state to search for the next one.
        if (!blockingExpression.isInitialized()) {
            break;
        }
        solver->add(blockingExpression);
    }

    modelData["initialStates"] = cpptempl::make_data(initialStates);
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateRewards(cpptempl::data_map& modelData) {
    // Extract the reward models from the program based on the names we were given.
    std::vector<storm::expressions::Variable> rewardVariables;
    if (this->options.isBuildAllRewardModelsSet()) {
        for (auto const& rewExpr : model.getAllRewardModelExpressions()) {
            STORM_LOG_ERROR_COND(rewExpr.second.isVariable(), "The jit builder can not build the non-trivial reward expression '" << rewExpr.second << "'.");
            rewardVariables.push_back(rewExpr.second.getBaseExpression().asVariableExpression().getVariable());
        }
    } else {
        for (auto const& rewardModelName : this->options.getRewardModelNames()) {
            auto const& rewExpr = model.getRewardModelExpression(rewardModelName);
            STORM_LOG_ERROR_COND(rewExpr.isVariable(), "The jit builder can not build the non-trivial reward expression '" << rewExpr << "'.");
            rewardVariables.push_back(rewExpr.getBaseExpression().asVariableExpression().getVariable());
        }
    }
    // Sort the reward variables to match the order in the ordered assignments
    std::sort(rewardVariables.begin(), rewardVariables.end());

    std::vector<storm::builder::RewardModelInformation> rewardModels;
    cpptempl::data_list rewards;
    cpptempl::data_list locationRewards;
    cpptempl::data_list edgeRewards;
    cpptempl::data_list destinationRewards;
    uint64_t rewardModelIndex = 0;
    uint64_t stateActionRewardCount = 0;
    for (auto const& variable : rewardVariables) {
        bool hasLocationRewards = false;
        bool hasEdgeRewards = false;
        bool hasDestinationRewards = false;

        for (auto const& automatonRef : parallelAutomata) {
            storm::jani::Automaton const& automaton = automatonRef.get();

            for (auto const& location : automaton.getLocations()) {
                for (auto const& assignment : location.getAssignments()) {
                    if (assignment.getExpressionVariable() == variable) {
                        hasLocationRewards = true;
                        break;
                    }
                }
            }

            for (auto const& edge : automaton.getEdges()) {
                for (auto const& assignment : edge.getAssignments()) {
                    if (assignment.getExpressionVariable() == variable) {
                        hasEdgeRewards = true;
                    }
                }

                for (auto const& destination : edge.getDestinations()) {
                    for (auto const& assignment : destination.getOrderedAssignments()) {
                        if (assignment.getExpressionVariable() == variable) {
                            hasDestinationRewards = true;
                        }
                    }
                }
            }
        }

        rewardModels.emplace_back(variable.getName(), hasLocationRewards, hasEdgeRewards || hasDestinationRewards, false);

        if (hasEdgeRewards || hasDestinationRewards) {
            ++stateActionRewardCount;
        }

        cpptempl::data_map reward;
        reward["name"] = variable.getName();
        reward["location_rewards"] = asString(hasLocationRewards);
        reward["edge_rewards"] = asString(hasEdgeRewards);
        reward["destination_rewards"] = asString(hasDestinationRewards);
        rewards.push_back(reward);

        if (hasLocationRewards) {
            cpptempl::data_map locationReward;
            locationReward["variable"] = variable.getName() + JIT_VARIABLE_EXTENSION;
            locationRewards.push_back(locationReward);
        }
        if (hasEdgeRewards) {
            cpptempl::data_map edgeReward;
            edgeReward["variable"] = variable.getName() + JIT_VARIABLE_EXTENSION;
            edgeReward["index"] = asString(rewardModelIndex);
            edgeRewards.push_back(edgeReward);
        }
        if (hasDestinationRewards) {
            cpptempl::data_map destinationReward;
            destinationReward["index"] = asString(rewardModelIndex);
            destinationReward["variable"] = variable.getName() + JIT_VARIABLE_EXTENSION;
            destinationRewards.push_back(destinationReward);
        }
        ++rewardModelIndex;
    }

    modelData["location_rewards"] = cpptempl::make_data(locationRewards);
    modelData["edge_rewards"] = cpptempl::make_data(edgeRewards);
    modelData["destination_rewards"] = cpptempl::make_data(destinationRewards);
    modelData["rewards"] = cpptempl::make_data(rewards);
    modelData["edge_destination_rewards_count"] = asString(stateActionRewardCount);
}

static std::ostream& indent(std::ostream& out, uint64_t indentLevel) {
    for (uint64_t i = 0; i < indentLevel; ++i) {
        out << "\t";
    }
    return out;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateSynchronizationVector(
    cpptempl::data_map& modelData, storm::jani::ParallelComposition const& parallelComposition, storm::jani::SynchronizationVector const& synchronizationVector,
    uint64_t synchronizationVectorIndex) {
    std::stringstream vectorSource;
    uint64_t numberOfActionInputs = synchronizationVector.getNumberOfActionInputs();

    // First, we check whether we need to generate code for a) different assignment levels and b) transient variables.
    uint64_t position = 0;
    int_fast64_t lowestLevel;
    int_fast64_t highestLevel;
    bool firstDestination = true;
    bool generateTransient = false;
    for (auto const& inputActionName : synchronizationVector.getInput()) {
        if (!storm::jani::SynchronizationVector::isNoActionInput(inputActionName)) {
            storm::jani::Automaton const& automaton =
                model.getAutomaton(parallelComposition.getSubcomposition(position).asAutomatonComposition().getAutomatonName());
            uint64_t actionIndex = model.getActionIndex(inputActionName);
            for (auto const& edge : automaton.getEdges()) {
                if (edge.getActionIndex() == actionIndex) {
                    for (auto const& destination : edge.getDestinations()) {
                        if (!destination.getOrderedAssignments().empty()) {
                            if (firstDestination) {
                                lowestLevel = destination.getOrderedAssignments().getLowestLevel();
                                highestLevel = destination.getOrderedAssignments().getHighestLevel();
                                firstDestination = false;
                            } else {
                                lowestLevel = std::min(lowestLevel, destination.getOrderedAssignments().getLowestLevel());
                                highestLevel = std::max(highestLevel, destination.getOrderedAssignments().getHighestLevel());
                            }
                        }
                        if (!generateTransient) {
                            for (auto const& assignment : destination.getOrderedAssignments()) {
                                if (assignment.isTransient()) {
                                    generateTransient = true;
                                }
                                std::set<storm::expressions::Variable> usedVariables = assignment.getAssignedExpression().getVariables();
                                for (auto const& variable : usedVariables) {
                                    if (transientVariables.find(variable) != transientVariables.end()) {
                                        generateTransient = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ++position;
    }
    bool generateLevelCode = lowestLevel != highestLevel;

    uint64_t indentLevel = 4;
    indent(vectorSource, indentLevel - 4) << "void performSynchronizedDestinations_" << synchronizationVectorIndex
                                          << "(StateType const& in, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore, ";
    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        vectorSource << "Destination const& destination" << index << ", ";
    }
    if (generateLevelCode) {
        vectorSource << "int64_t lowestLevel, int64_t highestLevel, ";
    }
    vectorSource << "ValueType const& rate, Choice<IndexType, ValueType>& choice) {\n";
    if (options.isExplorationChecksSet()) {
        indent(vectorSource, indentLevel + 1) << "VariableWrites variableWrites;\n";
    }
    indent(vectorSource, indentLevel + 1) << "StateType out(in);\n";
    indent(vectorSource, indentLevel + 1) << "TransientVariables transientIn;\n";
    indent(vectorSource, indentLevel + 1) << "TransientVariables transientOut;\n";

    if (generateLevelCode) {
        indent(vectorSource, indentLevel + 1) << "for (int64_t level = lowestLevel; level <= highestLevel; ++level) {\n";
        ++indentLevel;
    }
    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        indent(vectorSource, indentLevel + 1) << "destination" << index << ".perform(";
        if (generateLevelCode) {
            vectorSource << "level, ";
        }
        vectorSource << "in, out, transientIn, transientOut";
        if (options.isExplorationChecksSet()) {
            vectorSource << ", variableWrites";
        }
        vectorSource << ");\n";
    }
    if (generateLevelCode) {
        --indentLevel;
        indent(vectorSource, indentLevel + 1) << "}\n";
    }
    indent(vectorSource, indentLevel + 1) << "IndexType outStateIndex = getOrAddIndex(out, statesToExplore);\n";
    indent(vectorSource, indentLevel + 1) << "ValueType probability = ";
    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        vectorSource << "destination" << index << ".probability(in)";
        if (index + 1 < numberOfActionInputs) {
            vectorSource << " * ";
        }
    }
    vectorSource << ";\n";
    indent(vectorSource, indentLevel + 1) << "ValueType value = rate * probability;\n";
    indent(vectorSource, indentLevel + 1) << "choice.add(outStateIndex, value);\n";

    std::stringstream tmp;
    indent(tmp, indentLevel + 1)
        << "{% for reward in destination_rewards %}choice.addReward({$reward.index}, probability * transientOut.{$reward.variable});\n";
    indent(tmp, indentLevel + 1) << "{% endfor %}\n";
    vectorSource << cpptempl::parse(tmp.str(), modelData);
    indent(vectorSource, indentLevel) << "}\n\n";

    indent(vectorSource, indentLevel) << "void performSynchronizedDestinations_" << synchronizationVectorIndex
                                      << "(StateType const& in, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore, ";
    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        vectorSource << "Edge const& edge" << index << ", ";
    }
    vectorSource << "ValueType const& rate, Choice<IndexType, ValueType>& choice) {\n";
    if (generateLevelCode) {
        indent(vectorSource, indentLevel + 1) << "int64_t lowestLevel; int64_t highestLevel;";
    }
    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        indent(vectorSource, indentLevel + 1 + index) << "for (auto const& destination" << index << " : edge" << index << ") {\n";
        if (generateLevelCode) {
            if (index == 0) {
                indent(vectorSource, indentLevel + 2 + index) << "lowestLevel = destination" << index << ".lowestLevel();\n";
                indent(vectorSource, indentLevel + 2 + index) << "highestLevel = destination" << index << ".highestLevel();\n";
            } else {
                indent(vectorSource, indentLevel + 2 + index) << "lowestLevel = std::min(lowestLevel, destination" << index << ".lowestLevel());\n";
                indent(vectorSource, indentLevel + 2 + index) << "highestLevel = std::max(highestLevel, destination" << index << ".highestLevel());\n";
            }
        }
    }
    indent(vectorSource, indentLevel + 1 + numberOfActionInputs)
        << "performSynchronizedDestinations_" << synchronizationVectorIndex << "(in, behaviour, statesToExplore, ";
    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        vectorSource << "destination" << index << ", ";
    }
    if (generateLevelCode) {
        vectorSource << "lowestLevel, highestLevel, ";
    }
    vectorSource << "rate, choice);\n";
    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        indent(vectorSource, indentLevel + numberOfActionInputs - index) << "}\n";
    }
    indent(vectorSource, indentLevel) << "}\n\n";

    for (uint64_t index = 0; index < numberOfActionInputs; ++index) {
        indent(vectorSource, indentLevel) << "void performSynchronizedEdges_" << synchronizationVectorIndex << "_" << index
                                          << "(StateType const& in, std::vector<std::vector<std::reference_wrapper<Edge const>>> const& edges,  "
                                             "StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore";
        if (index > 0) {
            vectorSource << ", ";
        }
        for (uint64_t innerIndex = 0; innerIndex < index; ++innerIndex) {
            vectorSource << "Edge const& edge" << innerIndex;
            if (innerIndex + 1 < index) {
                vectorSource << ", ";
            }
        }
        if (index > 0) {
            vectorSource << ", TransientVariables const& transientIn, TransientVariables& transientOut";
            if (options.isExplorationChecksSet()) {
                vectorSource << ", VariableWrites& variableWrites";
            }
        }
        vectorSource << ", ValueType const& rate) {\n";
        if (index == 0) {
            if (options.isExplorationChecksSet()) {
                indent(vectorSource, indentLevel + 1) << "VariableWrites variableWrites;\n";
            }
            indent(vectorSource, indentLevel + 1) << "TransientVariables transientIn;\n";
            indent(vectorSource, indentLevel + 1) << "TransientVariables transientOut;\n";
        }
        indent(vectorSource, indentLevel + 1) << "for (auto const& edge" << index << " : edges[" << index << "]) {\n";
        indent(vectorSource, indentLevel + 2) << "edge" << index << ".get().perform(in, transientIn, transientOut";
        if (options.isExplorationChecksSet()) {
            vectorSource << ", variableWrites";
        }
        vectorSource << ");\n";
        if (index + 1 < numberOfActionInputs) {
            indent(vectorSource, indentLevel + 2) << "performSynchronizedEdges_" << synchronizationVectorIndex << "_" << (index + 1)
                                                  << "(in, edges, behaviour, statesToExplore, ";
            for (uint64_t innerIndex = 0; innerIndex <= index; ++innerIndex) {
                vectorSource << "edge" << innerIndex << ", ";
            }
            vectorSource << "transientIn, transientOut, ";
            if (options.isExplorationChecksSet()) {
                vectorSource << "variableWrites, ";
            }
            vectorSource << "rate * edge" << index << ".get().rate(in));\n";
        } else {
            indent(vectorSource, indentLevel + 2) << "Choice<IndexType, ValueType>& choice = behaviour.addChoice();\n";

            std::stringstream tmp;
            indent(tmp, indentLevel + 2) << "choice.resizeRewards({$edge_destination_rewards_count});\n";
            indent(tmp, indentLevel + 2) << "{% for reward in edge_rewards %}choice.addReward({$reward.index}, transientOut.{$reward.variable});\n";
            indent(tmp, indentLevel + 2) << "{% endfor %}\n";

            vectorSource << cpptempl::parse(tmp.str(), modelData);

            indent(vectorSource, indentLevel + 2) << "performSynchronizedDestinations_" << synchronizationVectorIndex << "(in, behaviour, statesToExplore, ";
            for (uint64_t innerIndex = 0; innerIndex <= index; ++innerIndex) {
                vectorSource << "edge" << innerIndex << ", ";
            }
            vectorSource << "rate * edge" << index << ".get().rate(in), choice);\n";
        }
        indent(vectorSource, indentLevel + 1) << "}\n";
        indent(vectorSource, indentLevel) << "}\n\n";
    }

    indent(vectorSource, indentLevel)
        << "void get_edges_" << synchronizationVectorIndex
        << "(StateType const& state, TransientVariables const& transientIn, std::vector<std::reference_wrapper<Edge const>>& edges, uint64_t position) {\n";
    position = 0;
    uint64_t participatingPosition = 0;
    for (auto const& inputActionName : synchronizationVector.getInput()) {
        if (!storm::jani::SynchronizationVector::isNoActionInput(inputActionName)) {
            indent(vectorSource, indentLevel + 1) << "if (position == " << participatingPosition << ") {\n";

            storm::jani::Automaton const& automaton =
                model.getAutomaton(parallelComposition.getSubcomposition(position).asAutomatonComposition().getAutomatonName());
            uint64_t actionIndex = model.getActionIndex(inputActionName);
            uint64_t edgeIndex = 0;
            for (auto const& edge : automaton.getEdges()) {
                if (edge.getActionIndex() == actionIndex) {
                    std::string edgeName = automaton.getName() + "_" + std::to_string(edgeIndex);
                    indent(vectorSource, indentLevel + 2) << "if (edge_enabled_" << edgeName << "(state, transientIn)) {\n";
                    indent(vectorSource, indentLevel + 3) << "edges.emplace_back(edge_" << edgeName << ");\n";
                    indent(vectorSource, indentLevel + 2) << "}\n";
                }
                ++edgeIndex;
            }

            indent(vectorSource, indentLevel + 1) << "}\n";
            ++participatingPosition;
        }
        ++position;
    }
    indent(vectorSource, indentLevel) << "}\n\n";

    indent(vectorSource, indentLevel) << "void exploreSynchronizationVector_" << synchronizationVectorIndex
                                      << "(StateType const& state, TransientVariables const& transientIn, StateBehaviour<IndexType, ValueType>& behaviour, "
                                         "StateSet<StateType>& statesToExplore) {\n";
    indent(vectorSource, indentLevel + 1) << "#ifndef NDEBUG\n";
    indent(vectorSource, indentLevel + 1) << "std::cout << \"Exploring synchronization vector " << synchronizationVectorIndex << ".\\n\";\n";
    indent(vectorSource, indentLevel + 1) << "#endif\n";
    indent(vectorSource, indentLevel + 1) << "std::vector<std::vector<std::reference_wrapper<Edge const>>> edges("
                                          << synchronizationVector.getNumberOfActionInputs() << ");\n";

    participatingPosition = 0;
    for (auto const& input : synchronizationVector.getInput()) {
        if (!storm::jani::SynchronizationVector::isNoActionInput(input)) {
            indent(vectorSource, indentLevel + 1) << "get_edges_" << synchronizationVectorIndex << "(state, transientIn, edges[" << participatingPosition
                                                  << "], " << participatingPosition << ");\n";
            indent(vectorSource, indentLevel + 1) << "if (edges[" << participatingPosition << "].empty()) {\n";
            indent(vectorSource, indentLevel + 2) << "return;\n";
            indent(vectorSource, indentLevel + 1) << "}\n";
            ++participatingPosition;
        }
    }
    indent(vectorSource, indentLevel + 1) << "performSynchronizedEdges_" << synchronizationVectorIndex
                                          << "_0(state, edges, behaviour, statesToExplore, storm::utility::one<ValueType>());\n";
    indent(vectorSource, indentLevel) << "}\n\n";

    cpptempl::data_map vector;
    vector["functions"] = vectorSource.str();
    vector["index"] = asString(synchronizationVectorIndex);
    return vector;
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateEdges(cpptempl::data_map& modelData) {
    STORM_LOG_THROW(model.hasStandardCompliantComposition(), storm::exceptions::WrongFormatException,
                    "Model builder only supports non-nested parallel compositions.");

    cpptempl::data_list nonSynchronizingEdges;
    cpptempl::data_list synchronizingEdges;
    cpptempl::data_list vectors;

    storm::jani::Composition const& topLevelComposition = model.getSystemComposition();
    if (topLevelComposition.isAutomatonComposition()) {
        storm::jani::Automaton const& automaton = model.getAutomaton(topLevelComposition.asAutomatonComposition().getAutomatonName());
        uint64_t edgeIndex = 0;
        for (auto const& edge : automaton.getEdges()) {
            nonSynchronizingEdges.push_back(generateEdge(automaton, edgeIndex, edge));
            ++edgeIndex;
        }
    } else {
        storm::jani::ParallelComposition const& parallelComposition = topLevelComposition.asParallelComposition();

        std::vector<std::set<uint64_t>> synchronizingActions(parallelComposition.getNumberOfSubcompositions());
        uint64_t synchronizationVectorIndex = 0;
        for (auto const& synchronizationVector : parallelComposition.getSynchronizationVectors()) {
            // If the synchronization vector has at most one action input, there is no synchronization going on.
            if (synchronizationVector.getNumberOfActionInputs() <= 1) {
                continue;
            }

            bool createVector = true;
            uint64_t position = 0;
            for (auto const& inputActionName : synchronizationVector.getInput()) {
                if (!storm::jani::SynchronizationVector::isNoActionInput(inputActionName)) {
                    uint64_t actionIndex = model.getActionIndex(inputActionName);
                    synchronizingActions[position].insert(actionIndex);

                    storm::jani::Automaton const& automaton =
                        model.getAutomaton(parallelComposition.getSubcomposition(position).asAutomatonComposition().getAutomatonName());
                    bool hasParticipatingEdge = false;
                    for (auto const& edge : automaton.getEdges()) {
                        if (edge.getActionIndex() == actionIndex) {
                            hasParticipatingEdge = true;
                            break;
                        }
                    }

                    if (!hasParticipatingEdge) {
                        createVector = false;
                    }
                }
                ++position;
            }

            if (createVector) {
                cpptempl::data_map vector = generateSynchronizationVector(modelData, parallelComposition, synchronizationVector, synchronizationVectorIndex);
                vectors.push_back(vector);
            }
            ++synchronizationVectorIndex;
        }

        uint64_t position = 0;
        for (auto const& composition : parallelComposition.getSubcompositions()) {
            storm::jani::Automaton const& automaton = model.getAutomaton(composition->asAutomatonComposition().getAutomatonName());

            // Add all edges with an action index that is not mentioned in any synchronization vector as
            // non-synchronizing edges.
            uint64_t edgeIndex = 0;
            for (auto const& edge : automaton.getEdges()) {
                if (synchronizingActions[position].find(edge.getActionIndex()) != synchronizingActions[position].end()) {
                    synchronizingEdges.push_back(generateEdge(automaton, edgeIndex, edge));
                } else {
                    nonSynchronizingEdges.push_back(generateEdge(automaton, edgeIndex, edge));
                }
                ++edgeIndex;
            }

            ++position;
        }
    }

    modelData["nonsynch_edges"] = cpptempl::make_data(nonSynchronizingEdges);
    modelData["synch_edges"] = cpptempl::make_data(synchronizingEdges);
    modelData["synch_vectors"] = cpptempl::make_data(vectors);
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateEdge(storm::jani::Automaton const& automaton, uint64_t edgeIndex,
                                                                                         storm::jani::Edge const& edge) {
    cpptempl::data_map edgeData;

    std::set<storm::expressions::Variable> transientVariablesInEdge;
    cpptempl::data_list edgeAssignments;
    for (auto const& assignment : edge.getAssignments()) {
        transientVariablesInEdge.insert(assignment.getExpressionVariable());
        std::set<storm::expressions::Variable> usedVariables = assignment.getAssignedExpression().getVariables();
        for (auto const& variable : usedVariables) {
            if (transientVariables.find(variable) != transientVariables.end()) {
                transientVariablesInEdge.insert(variable);
            }
        }
        edgeAssignments.push_back(generateAssignment(assignment));
    }

    cpptempl::data_list destinations;
    uint64_t destinationIndex = 0;
    std::set<storm::expressions::Variable> transientVariablesInDestinations;
    for (auto const& destination : edge.getDestinations()) {
        destinations.push_back(generateDestination(automaton, destinationIndex, destination));

        for (auto const& assignment : destination.getOrderedAssignments().getAllAssignments()) {
            if (assignment.isTransient()) {
                transientVariablesInDestinations.insert(assignment.getExpressionVariable());
            }
            std::set<storm::expressions::Variable> usedVariables = assignment.getAssignedExpression().getVariables();
            for (auto const& variable : usedVariables) {
                if (transientVariables.find(variable) != transientVariables.end()) {
                    transientVariablesInDestinations.insert(variable);
                }
            }
        }

        ++destinationIndex;
    }

    if (automaton.getNumberOfLocations() > 1) {
        edgeData["guard"] = expressionTranslator.translate(
            shiftVariablesWrtLowerBound(edge.getGuard()) && getLocationVariable(automaton) == model.getManager().integer(edge.getSourceLocationIndex()),
            storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
    } else {
        edgeData["guard"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(edge.getGuard()),
                                                           storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
    }
    edgeData["destinations"] = cpptempl::make_data(destinations);
    edgeData["name"] = automaton.getName() + "_" + std::to_string(edgeIndex);
    edgeData["transient_assignments"] = cpptempl::make_data(edgeAssignments);
    edgeData["markovian"] = asString(edge.hasRate());
    if (edge.hasRate()) {
        if (std::is_same<double, ValueType>::value) {
            edgeData["rate"] = expressionTranslator.translate(
                shiftVariablesWrtLowerBound(edge.getRate()),
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastDouble));
        } else if (std::is_same<storm::RationalNumber, ValueType>::value) {
            edgeData["rate"] = expressionTranslator.translate(
                shiftVariablesWrtLowerBound(edge.getRate()),
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalNumber));
        } else if (std::is_same<storm::RationalFunction, ValueType>::value) {
            edgeData["rate"] = expressionTranslator.translate(
                shiftVariablesWrtLowerBound(edge.getRate()),
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalFunction));
        }
    } else {
        edgeData["rate"] = std::string("storm::utility::one<ValueType>()");
    }

    cpptempl::data_list transientVariablesInDestinationsData;
    for (auto const& variable : transientVariablesInDestinations) {
        transientVariablesInDestinationsData.push_back(getVariableName(variable));
    }
    edgeData["transient_variables_in_destinations"] = cpptempl::make_data(transientVariablesInDestinationsData);
    cpptempl::data_list transientVariablesInEdgeData;
    for (auto const& variable : transientVariablesInEdge) {
        transientVariablesInEdgeData.push_back(getVariableName(variable));
    }
    edgeData["transient_variables_in_edge"] = cpptempl::make_data(transientVariablesInEdgeData);
    return edgeData;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateDestination(storm::jani::Automaton const& automaton,
                                                                                                uint64_t destinationIndex,
                                                                                                storm::jani::EdgeDestination const& destination) {
    cpptempl::data_map destinationData;

    cpptempl::data_list levels = generateLevels(automaton, destination.getLocationIndex(), destination.getOrderedAssignments());
    destinationData["name"] = asString(destinationIndex);
    destinationData["levels"] = cpptempl::make_data(levels);
    storm::expressions::Expression shiftedProbabilityExpression = shiftVariablesWrtLowerBound(destination.getProbability());
    if (std::is_same<double, ValueType>::value) {
        destinationData["probability"] = expressionTranslator.translate(
            shiftedProbabilityExpression,
            storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastDouble));
    } else if (std::is_same<storm::RationalNumber, ValueType>::value) {
        destinationData["probability"] = expressionTranslator.translate(
            shiftedProbabilityExpression,
            storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalNumber));
    } else if (std::is_same<storm::RationalFunction, ValueType>::value) {
        destinationData["probability"] = expressionTranslator.translate(
            shiftedProbabilityExpression,
            storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalFunction));
    }
    if (destination.getOrderedAssignments().empty()) {
        destinationData["lowestLevel"] = "0";
        destinationData["highestLevel"] = "0";
    } else {
        destinationData["lowestLevel"] = asString(destination.getOrderedAssignments().getLowestLevel());
        destinationData["highestLevel"] = asString(destination.getOrderedAssignments().getHighestLevel());
    }

    return destinationData;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_list ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLevels(storm::jani::Automaton const& automaton,
                                                                                            uint64_t destinationLocationIndex,
                                                                                            storm::jani::OrderedAssignments const& orderedAssignments) {
    cpptempl::data_list levels;

    auto const& assignments = orderedAssignments.getAllAssignments();
    if (!assignments.empty()) {
        int64_t currentLevel = assignments.front().getLevel();

        cpptempl::data_list nonTransientAssignmentData;
        cpptempl::data_list transientAssignmentData;
        for (auto const& assignment : assignments) {
            if (assignment.getLevel() != currentLevel) {
                cpptempl::data_map level;
                level["non_transient_assignments"] = cpptempl::make_data(nonTransientAssignmentData);
                level["transient_assignments"] = cpptempl::make_data(transientAssignmentData);
                level["index"] = asString(currentLevel);
                levels.push_back(level);

                nonTransientAssignmentData = cpptempl::data_list();
                transientAssignmentData = cpptempl::data_list();
                currentLevel = assignment.getLevel();
            }

            if (assignment.isTransient()) {
                transientAssignmentData.push_back(generateAssignment(assignment));
            } else {
                nonTransientAssignmentData.push_back(generateAssignment(assignment));
            }
        }

        // Close the last (open) level.
        cpptempl::data_map level;
        if (automaton.getNumberOfLocations() > 1) {
            nonTransientAssignmentData.push_back(generateLocationAssignment(automaton, destinationLocationIndex));
        }
        level["non_transient_assignments"] = cpptempl::make_data(nonTransientAssignmentData);
        level["transient_assignments"] = cpptempl::make_data(transientAssignmentData);
        level["index"] = asString(currentLevel);
        levels.push_back(level);
    } else if (automaton.getNumberOfLocations() > 1) {
        // Create (an otherwise) empty level to perform the location assignment.
        cpptempl::data_map level;
        cpptempl::data_list nonTransientAssignmentData;
        cpptempl::data_list transientAssignmentData;
        nonTransientAssignmentData.push_back(generateLocationAssignment(automaton, destinationLocationIndex));
        level["non_transient_assignments"] = cpptempl::make_data(nonTransientAssignmentData);
        level["transient_assignments"] = cpptempl::make_data(transientAssignmentData);
        level["index"] = asString(0);
        levels.push_back(level);
    }

    return levels;
}

template<typename ValueType, typename RewardModelType>
template<typename ValueTypePrime>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateAssignment(storm::jani::Variable const& variable,
                                                                                               ValueTypePrime value) const {
    cpptempl::data_map result;
    result["variable"] = getVariableName(variable.getExpressionVariable());

    // Check if the variable has a non-zero lower bound and, if so, shift it appropriately.
    auto it = lowerBounds.find(variable.getExpressionVariable());
    if (it != lowerBounds.end()) {
        result["value"] = asString(value) + " + " + asString(-it->second);
    } else {
        result["value"] = asString(value);
    }
    return result;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLocationAssignment(storm::jani::Automaton const& automaton,
                                                                                                       uint64_t value) const {
    cpptempl::data_map result;
    result["variable"] = getVariableName(getLocationVariable(automaton));
    result["value"] = asString(value);
    return result;
}

template<typename ValueType, typename RewardModelType>
cpptempl::data_map ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateAssignment(storm::jani::Assignment const& assignment) {
    cpptempl::data_map result;
    result["variable"] = getVariableName(assignment.getExpressionVariable());
    auto lowerBoundIt = lowerBounds.find(assignment.getExpressionVariable());

    storm::expressions::ToCppTranslationOptions options(variablePrefixes, variableToName);
    if (realVariables.find(assignment.getExpressionVariable()) != realVariables.end()) {
        if (std::is_same<double, ValueType>::value) {
            options = storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastDouble);
        } else if (std::is_same<storm::RationalNumber, ValueType>::value) {
            options =
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalNumber);
        } else if (std::is_same<storm::RationalFunction, ValueType>::value) {
            options =
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastRationalFunction);
        }
    }

    if (lowerBoundIt != lowerBounds.end()) {
        result["value"] = expressionTranslator.translate(
            shiftVariablesWrtLowerBound(assignment.getAssignedExpression()) + model.getManager().integer(-lowerBoundIt->second), options);
    } else {
        result["value"] = expressionTranslator.translate(shiftVariablesWrtLowerBound(assignment.getAssignedExpression()), options);
    }
    return result;
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLocations(cpptempl::data_map& modelData) {
    cpptempl::data_list locations;

    for (auto const& automatonRef : parallelAutomata) {
        storm::jani::Automaton const& automaton = automatonRef.get();
        cpptempl::data_map locationData;
        uint64_t locationIndex = 0;
        for (auto const& location : automaton.getLocations()) {
            cpptempl::data_list assignments;
            for (auto const& assignment : location.getAssignments()) {
                assignments.push_back(generateAssignment(assignment));
            }
            locationData["assignments"] = cpptempl::make_data(assignments);
            if (automaton.getNumberOfLocations() > 1) {
                locationData["guard"] = expressionTranslator.translate(
                    shiftVariablesWrtLowerBound(getLocationVariable(automaton) == this->model.getManager().integer(locationIndex)),
                    storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName));
            } else {
                locationData["guard"] = asString(true);
            }
            ++locationIndex;
        }
        if (!locationData["assignments"]->empty()) {
            locations.push_back(locationData);
        }
    }

    modelData["locations"] = cpptempl::make_data(locations);
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateLabels(cpptempl::data_map& modelData) {
    cpptempl::data_list labels;

    // As in JANI we can use transient boolean variable assignments in locations to identify states, we need to
    // create a list of boolean transient variables and the expressions that define them.
    for (auto const& variable : model.getGlobalVariables().getTransientVariables()) {
        if (variable.getType().isBasicType() && variable.getType().asBasicType().isBooleanType()) {
            if (this->options.isBuildAllLabelsSet() || this->options.getLabelNames().find(variable.getName()) != this->options.getLabelNames().end()) {
                cpptempl::data_map label;
                label["name"] = variable.getName();
                label["predicate"] = expressionTranslator.translate(
                    shiftVariablesWrtLowerBound(model.getLabelExpression(variable, parallelAutomata)),
                    storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastDouble));
                labels.push_back(label);
            }
        }
    }

    std::set<std::string> expressionLabelStrings;
    for (auto const& expressionLabel : this->options.getExpressionLabels()) {
        cpptempl::data_map label;
        std::string const& expressionLabelString = expressionLabel.first;
        auto const& expression = expressionLabel.second;
        if (expressionLabelStrings.count(expressionLabelString) == 0) {
            label["name"] = expressionLabelString;
            label["predicate"] = expressionTranslator.translate(
                shiftVariablesWrtLowerBound(expression),
                storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName, storm::expressions::ToCppTranslationMode::CastDouble));
            labels.push_back(label);
            expressionLabelStrings.insert(expressionLabelString);
        }
    }

    modelData["labels"] = cpptempl::make_data(labels);
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateTerminalExpressions(cpptempl::data_map& modelData) {
    cpptempl::data_list terminalExpressions;

    for (auto const& terminalEntry : options.getTerminalStates()) {
        LabelOrExpression const& labelOrExpression = terminalEntry.first;
        if (labelOrExpression.isLabel()) {
            auto const& variables = model.getGlobalVariables();
            STORM_LOG_THROW(variables.hasVariable(labelOrExpression.getLabel()), storm::exceptions::WrongFormatException,
                            "Terminal label refers to unknown identifier '" << labelOrExpression.getLabel() << "'.");
            auto const& variable = variables.getVariable(labelOrExpression.getLabel());
            STORM_LOG_THROW(variable.getType().isBasicType() && variable.getType().asBasicType().isBooleanType(), storm::exceptions::WrongFormatException,
                            "Terminal label refers to non-boolean variable '" << variable.getName() << ".");
            STORM_LOG_THROW(variable.isTransient(), storm::exceptions::WrongFormatException,
                            "Terminal label refers to non-transient variable '" << variable.getName() << ".");
            auto labelExpression = model.getLabelExpression(variable, parallelAutomata);
            if (!terminalEntry.second) {
                labelExpression = !labelExpression;
            }
            terminalExpressions.push_back(expressionTranslator.translate(shiftVariablesWrtLowerBound(labelExpression),
                                                                         storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName)));
        } else {
            auto expression = terminalEntry.second ? labelOrExpression.getExpression() : !labelOrExpression.getExpression();
            terminalExpressions.push_back(expressionTranslator.translate(shiftVariablesWrtLowerBound(expression),
                                                                         storm::expressions::ToCppTranslationOptions(variablePrefixes, variableToName)));
        }
    }

    modelData["terminalExpressions"] = cpptempl::make_data(terminalExpressions);
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::generateParameters(cpptempl::data_map& modelData) {
    cpptempl::data_list parameters;
    for (auto const& constant : model.getConstants()) {
        if (!constant.isDefined() && constant.isRealConstant()) {
            cpptempl::data_map parameter;
            parameter["name"] = constant.getName();
            parameters.push_back(parameter);
        }
    }
    modelData["parameters"] = cpptempl::make_data(parameters);
}

template<typename ValueType, typename RewardModelType>
std::string const& ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getVariableName(storm::expressions::Variable const& variable) const {
    return variableToName.at(variable);
}

template<typename ValueType, typename RewardModelType>
std::string const& ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::registerVariable(storm::expressions::Variable const& variable, bool transient) {
    // Since the variable name might be illegal as a C++ identifier, we need to prepare it a bit.
    variableToName[variable] = variable.getName() + JIT_VARIABLE_EXTENSION;
    if (transient) {
        transientVariables.insert(variable);
        variablePrefixes[variable] = "transientIn.";
    } else {
        nontransientVariables.insert(variable);
        variablePrefixes[variable] = "in.";
    }
    return variableToName[variable];
}

template<typename ValueType, typename RewardModelType>
storm::expressions::Variable const& ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::getLocationVariable(
    storm::jani::Automaton const& automaton) const {
    return automaton.getLocationExpressionVariable();
}

template<typename ValueType, typename RewardModelType>
std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::asString(bool value) const {
    std::stringstream out;
    out << std::boolalpha << value;
    return out.str();
}

template<typename ValueType, typename RewardModelType>
storm::expressions::Expression ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::shiftVariablesWrtLowerBound(
    storm::expressions::Expression const& expression) {
    return expression.substitute(lowerBoundShiftSubstitution);
}

template<typename ValueType, typename RewardModelType>
template<typename ValueTypePrime>
std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::asString(ValueTypePrime value) const {
    return std::to_string(value);
}

template<typename ValueType, typename RewardModelType>
std::string ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::createSourceCodeFromSkeleton(cpptempl::data_map& modelData) {
    std::string sourceTemplate = R"(
#define NDEBUG
                
{% if expl_progress %}
#define EXPL_PROGRESS
{% endif %}
                
#include <boost/dll/alias.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
                
{% if exact %}
#include "storm/adapters/RationalNumberAdapter.h"
{% endif %}
{% if parametric %}
#include "storm/adapters/RationalFunctionAdapter.h"
{% endif %}
                
#include <sparsepp/spp.h>
                
#include "storm/builder/RewardModelInformation.h"
#include "storm/builder/jit/JitModelBuilderInterface.h"
#include "storm/builder/jit/ModelComponentsBuilder.h"
#include "storm/builder/jit/StateBehaviour.h"
#include "storm/builder/jit/StateSet.h"
                
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/constants.h"
                
                namespace storm {
                    namespace builder {
                        namespace jit {
                            
                            typedef uint32_t IndexType;
                            
                            {% if double %}
                            typedef double ValueType;
                            {% endif %}
                            {% if exact %}
                            typedef storm::RationalNumber ValueType;
                            {% endif %}
                            {% if parametric %}
                            typedef storm::RationalFunction ValueType;
                            {% endif %}
                            
                            struct StateType {
                                // Boolean variables.
                                {% for variable in nontransient_variables.boolean %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Bounded integer variables.
                                {% for variable in nontransient_variables.boundedInteger %}// {$variable.original_name}: [{$variable.lower} ... {$variable.upper}]
                                uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                                // Unbounded integer variables.
                                {% for variable in nontransient_variables.unboundedInteger %}int64_t {$variable.name};
                                {% endfor %}
                                // Real variables.
                                {% for variable in nontransient_variables.real %}double {$variable.name};
                                {% endfor %}
                                // Location variables.
                                {% for variable in nontransient_variables.locations %}uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                            };
                            
                            bool operator==(StateType const& first, StateType const& second) {
                                bool result = true;
                                {% for variable in nontransient_variables.boolean %}result &= !(first.{$variable.name} ^ second.{$variable.name});
                                {% endfor %}
                                {% for variable in nontransient_variables.boundedInteger %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                {% for variable in nontransient_variables.unboundedInteger %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                {% for variable in nontransient_variables.real %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                {% for variable in nontransient_variables.locations %}result &= first.{$variable.name} == second.{$variable.name};
                                {% endfor %}
                                return result;
                            }
                            
                            std::ostream& operator<<(std::ostream& out, StateType const& in) {
                                out << "<";
                                {% for variable in nontransient_variables.boolean %}out << "{$variable.name}=" << std::boolalpha << in.{$variable.name} << ", " << std::noboolalpha;
                                {% endfor %}
                                {% for variable in nontransient_variables.boundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in nontransient_variables.unboundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in nontransient_variables.real %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in nontransient_variables.locations %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                out << ">";
                                return out;
                            }
                            
                            struct TransientVariables {
                                TransientVariables() {
                                    reset();
                                }
                                
                                void reset() {
                                    {% for variable in transient_variables.boolean %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                    {% for variable in transient_variables.boundedInteger %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                    {% for variable in transient_variables.unboundedInteger %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                    {% for variable in transient_variables.real %}{$variable.name} = {$variable.init};
                                    {% endfor %}
                                }
                                
                                // Boolean variables.
                                {% for variable in transient_variables.boolean %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Bounded integer variables.
                                {% for variable in transient_variables.boundedInteger %}uint64_t {$variable.name} : {$variable.numberOfBits};
                                {% endfor %}
                                // Unbounded integer variables.
                                {% for variable in transient_variables.unboundedInteger %}int64_t {$variable.name};
                                {% endfor %}
                                // Real variables.
                                {% if double %}
                                {% for variable in transient_variables.real %}double {$variable.name};
                                {% endfor %}
                                {% endif %}
                                {% if exact %}
                                {% for variable in transient_variables.real %}storm::RationalNumber {$variable.name};
                                {% endfor %}
                                {% endif %}
                                {% if parametric %}
                                {% for variable in transient_variables.real %}storm::RationalFunction {$variable.name};
                                {% endfor %}
                                {% endif %}
                            };
                            
                            std::ostream& operator<<(std::ostream& out, TransientVariables const& in) {
                                out << "<";
                                {% for variable in transient_variables.boolean %}out << "{$variable.name}=" << std::boolalpha << in.{$variable.name} << ", " << std::noboolalpha;
                                {% endfor %}
                                {% for variable in transient_variables.boundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in transient_variables.unboundedInteger %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                {% for variable in transient_variables.real %}out << "{$variable.name}=" << in.{$variable.name} << ", ";
                                {% endfor %}
                                out << ">";
                                return out;
                            }
                            
                            {% if exploration_checks %}
                            struct VariableWrites {
                                VariableWrites() {
                                    reset();
                                }
                                
                                void reset() {
                                    {% for variable in nontransient_variables.boolean %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in nontransient_variables.boundedInteger %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in nontransient_variables.unboundedInteger %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in nontransient_variables.real %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in nontransient_variables.locations %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in transient_variables.boolean %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in transient_variables.boundedInteger %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in transient_variables.unboundedInteger %}{$variable.name} = false;
                                    {% endfor %}
                                    {% for variable in transient_variables.real %}{$variable.name} = false;
                                    {% endfor %}
                                }
                                
                                // Boolean variables.
                                {% for variable in nontransient_variables.boolean %}bool {$variable.name} : 1;
                                {% endfor %}
                                {% for variable in transient_variables.boolean %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Bounded integer variables.
                                {% for variable in nontransient_variables.boundedInteger %}bool {$variable.name} : 1;
                                {% endfor %}
                                {% for variable in transient_variables.boundedInteger %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Unbounded integer variables.
                                {% for variable in nontransient_variables.unboundedInteger %}bool {$variable.name} : 1;
                                {% endfor %}
                                {% for variable in transient_variables.unboundedInteger %}bool {$variable.name} : 1;
                                {% endfor %}
                                // Real variables.
                                {% for variable in nontransient_variables.real %}bool {$variable.name} : 1;
                                {% endfor %}
                                {% for variable in transient_variables.real %}bool {$variable.name} : 1;
                                {% endfor %}

                                // Location variables.
                                {% for variable in nontransient_variables.locations %}bool {$variable.name} : 1;
                                {% endfor %}
                            };
                            {% endif %}
                            
                        }
                    }
                }
                
                namespace std {
                    template <>
                    struct hash<storm::builder::jit::StateType> {
                        std::size_t operator()(storm::builder::jit::StateType const& in) const {
                            // Note: this is faster than viewing the struct as a bit field and taking hash_combine of the bytes.
                            std::size_t seed = 0;
                            {% for variable in nontransient_variables.boolean %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.boundedInteger %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.unboundedInteger %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.real %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            {% for variable in nontransient_variables.locations %}spp::hash_combine(seed, in.{$variable.name});
                            {% endfor %}
                            return seed;
                        }
                    };
                }
                
                namespace storm {
                    namespace builder {
                        namespace jit {
                            
                            static bool model_is_deterministic() {
                                return {$deterministic_model};
                            }
                            
                            static bool model_is_discrete_time() {
                                return {$discrete_time_model};
                            }
                            
                            static bool perform_exploration_checks() {
                                {% if exploration_checks %}
                                return true;
                                {% endif %}
                                {% if not exploration_checks %}
                                return false;
                                {% endif %}
                            }
                            
                            {% if parametric %}
                            {% for parameter in parameters %}static storm::RationalFunction {$parameter.name};
                            {% endfor %}
                            
                            void initialize_parameters(std::vector<storm::RationalFunction> const& parameters) {
#ifndef NDEBUG
                                std::cout << "initializing parameters\n";
#endif
                                {% for parameter in parameters %}{$parameter.name} = parameters[{$loop.index} - 1];
                                {% endfor %}
                            }
                            {% endif %}
                            
                            // Non-synchronizing edges.
                            {% for edge in nonsynch_edges %}static bool edge_enabled_{$edge.name}(StateType const& in, TransientVariables const& transientIn) {
                                if ({$edge.guard}) {
                                    return true;
                                }
                                return false;
                            }
                            
                            static ValueType edge_rate_{$edge.name}(StateType const& in) {
                                return {$edge.rate};
                            }
                            
                            static void edge_perform_{$edge.name}(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for assignment in edge.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                {% if exploration_checks %}
                                if (variableWrites.{$assignment.variable}) {
                                    throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                } else {
                                    variableWrites.{$assignment.variable} = true;
                                }
                                {% endif %}
                                {% endfor %}
                            }
                            
                            {% for destination in edge.destinations %}
                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% if exploration_checks %}
                                    if (variableWrites.{$assignment.variable}) {
                                        throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                    } else {
                                        variableWrites.{$assignment.variable} = true;
                                    }
                                    {% endif %}
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out {% if exploration_checks %}, variableWrites {% endif %});
                                {% endfor %}
                            }
                            
                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% if exploration_checks %}
                                    if (variableWrites.{$assignment.variable}) {
                                        throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                    } else {
                                        variableWrites.{$assignment.variable} = true;
                                    }
                                    {% endif %}
                                    {% endfor %}
                                    {% for assignment in level.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                    {% if exploration_checks %}
                                    if (variableWrites.{$assignment.variable}) {
                                        throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                    } else {
                                        variableWrites.{$assignment.variable} = true;
                                    }
                                    {% endif %}
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out, transientIn, transientOut {% if exploration_checks %}, variableWrites {% endif %});
                                {% endfor %}
                            }

                            static ValueType destination_probability_{$edge.name}_{$destination.name}(StateType const& in) {
                                return {$destination.probability};
                            }
                            {% endfor %}{% endfor %}
                            
                            // Synchronizing edges.
                            {% for edge in synch_edges %}static bool edge_enabled_{$edge.name}(StateType const& in, TransientVariables const& transientIn) {
                                if ({$edge.guard}) {
                                    return true;
                                }
                                return false;
                            }
                            
                            static ValueType edge_rate_{$edge.name}(StateType const& in) {
                                return {$edge.rate};
                            }
                            
                            static void edge_perform_{$edge.name}(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for assignment in edge.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                {% if exploration_checks %}
                                if (variableWrites.{$assignment.variable}) {
                                    throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                } else {
                                    variableWrites.{$assignment.variable} = true;
                                }
                                {% endif %}
                                {% endfor %}
                            }
                            
                            {% for destination in edge.destinations %}
                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% if exploration_checks %}
                                    if (variableWrites.{$assignment.variable}) {
                                        throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                    } else {
                                        variableWrites.{$assignment.variable} = true;
                                    }
                                    {% endif %}
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out {% if exploration_checks %}, variableWrites {% endif %});
                                {% endfor %}
                            }
                            
                            static void destination_perform_level_{$edge.name}_{$destination.name}(int_fast64_t level, StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}if (level == {$level.index}) {
                                    {% for assignment in level.non_transient_assignments %}out.{$assignment.variable} = {$assignment.value};
                                    {% if exploration_checks %}
                                    if (variableWrites.{$assignment.variable}) {
                                        throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                    } else {
                                        variableWrites.{$assignment.variable} = true;
                                    }
                                    {% endif %}
                                    {% endfor %}
                                    {% for assignment in level.transient_assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                    {% if exploration_checks %}
                                    if (variableWrites.{$assignment.variable}) {
                                        throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                    } else {
                                        variableWrites.{$assignment.variable} = true;
                                    }
                                    {% endif %}
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            static void destination_perform_{$edge.name}_{$destination.name}(StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for level in destination.levels %}
                                destination_perform_level_{$edge.name}_{$destination.name}({$level.index}, in, out, transientIn, transientOut {% if exploration_checks %}, variableWrites {% endif %});
                                {% endfor %}
                            }
                            
                            static ValueType destination_probability_{$edge.name}_{$destination.name}(StateType const& in) {
                                return {$destination.probability};
                            }
                            {% endfor %}{% endfor %}
                            
                            typedef ValueType (*DestinationProbabilityFunctionPtr)(StateType const&);
                            typedef void (*DestinationLevelFunctionPtr)(int_fast64_t, StateType const&, StateType&, TransientVariables const&, TransientVariables& {% if exploration_checks %}, VariableWrites& {% endif %});
                            typedef void (*DestinationFunctionPtr)(StateType const&, StateType&, TransientVariables const&, TransientVariables& {% if exploration_checks %}, VariableWrites& {% endif %});
                            typedef void (*DestinationWithoutTransientLevelFunctionPtr)(int_fast64_t, StateType const&, StateType& {% if exploration_checks %}, VariableWrites& {% endif %});
                            typedef void (*DestinationWithoutTransientFunctionPtr)(StateType const&, StateType& {% if exploration_checks %}, VariableWrites& {% endif %});
                            
                            class Destination {
                            public:
                                Destination() : mLowestLevel(0), mHighestLevel(0), destinationProbabilityFunction(nullptr), destinationLevelFunction(nullptr), destinationFunction(nullptr), destinationWithoutTransientLevelFunction(nullptr), destinationWithoutTransientFunction(nullptr) {
                                    // Intentionally left empty.
                                }
                                
                                Destination(int_fast64_t lowestLevel, int_fast64_t highestLevel, DestinationProbabilityFunctionPtr destinationProbabilityFunction, DestinationLevelFunctionPtr destinationLevelFunction, DestinationFunctionPtr destinationFunction, DestinationWithoutTransientLevelFunctionPtr destinationWithoutTransientLevelFunction, DestinationWithoutTransientFunctionPtr destinationWithoutTransientFunction) : mLowestLevel(lowestLevel), mHighestLevel(highestLevel), destinationProbabilityFunction(destinationProbabilityFunction), destinationLevelFunction(destinationLevelFunction), destinationFunction(destinationFunction), destinationWithoutTransientLevelFunction(destinationWithoutTransientLevelFunction), destinationWithoutTransientFunction(destinationWithoutTransientFunction) {
                                    // Intentionally left empty.
                                }
                                
                                int_fast64_t lowestLevel() const {
                                    return mLowestLevel;
                                }
                                
                                int_fast64_t highestLevel() const {
                                    return mHighestLevel;
                                }
                                
                                ValueType probability(StateType const& in) const {
                                    return destinationProbabilityFunction(in);
                                }
                                
                                void perform(int_fast64_t level, StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) const {
                                    destinationLevelFunction(level, in, out, transientIn, transientOut {% if exploration_checks %}, variableWrites {% endif %});
                                }
                                
                                void perform(StateType const& in, StateType& out, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) const {
                                    destinationFunction(in, out, transientIn, transientOut {% if exploration_checks %}, variableWrites {% endif %});
                                }
                                
                                void perform(int_fast64_t level, StateType const& in, StateType& out {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) const {
                                    destinationWithoutTransientLevelFunction(level, in, out {% if exploration_checks %}, variableWrites {% endif %});
                                }
                                
                                void perform(StateType const& in, StateType& out {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) const {
                                    destinationWithoutTransientFunction(in, out {% if exploration_checks %}, variableWrites {% endif %});
                                }
                                
                            private:
                                int_fast64_t mLowestLevel;
                                int_fast64_t mHighestLevel;
                                DestinationProbabilityFunctionPtr destinationProbabilityFunction;
                                DestinationLevelFunctionPtr destinationLevelFunction;
                                DestinationFunctionPtr destinationFunction;
                                DestinationWithoutTransientLevelFunctionPtr destinationWithoutTransientLevelFunction;
                                DestinationWithoutTransientFunctionPtr destinationWithoutTransientFunction;
                            };
                            
                            typedef bool (*EdgeEnabledFunctionPtr)(StateType const&, TransientVariables const& transientIn);
                            typedef ValueType (*EdgeRateFunctionPtr)(StateType const&);
                            typedef void (*EdgeTransientFunctionPtr)(StateType const&, TransientVariables const& transientIn, TransientVariables& out {% if exploration_checks %}, VariableWrites& variableWrites {% endif %});
                            
                            class Edge {
                            public:
                                typedef std::vector<Destination> ContainerType;
                                
                                Edge() : edgeEnabledFunction(nullptr), edgeRateFunction(nullptr), edgeTransientFunction(nullptr) {
                                    // Intentionally left empty.
                                }
                                
                                Edge(EdgeEnabledFunctionPtr edgeEnabledFunction, EdgeRateFunctionPtr edgeRateFunction, EdgeTransientFunctionPtr edgeTransientFunction = nullptr) : edgeEnabledFunction(edgeEnabledFunction), edgeRateFunction(edgeRateFunction), edgeTransientFunction(edgeTransientFunction) {
                                    // Intentionally left empty.
                                }
                                
                                bool isEnabled(StateType const& in, TransientVariables const& transientIn) const {
                                    return edgeEnabledFunction(in, transientIn);
                                }
                                
                                void addDestination(Destination const& destination) {
                                    destinations.push_back(destination);
                                }
                                
                                void addDestination(int_fast64_t lowestLevel, int_fast64_t highestLevel, DestinationProbabilityFunctionPtr destinationProbabilityFunction, DestinationLevelFunctionPtr destinationLevelFunction, DestinationFunctionPtr destinationFunction, DestinationWithoutTransientLevelFunctionPtr destinationWithoutTransientLevelFunction, DestinationWithoutTransientFunctionPtr destinationWithoutTransientFunction) {
                                    destinations.emplace_back(lowestLevel, highestLevel, destinationProbabilityFunction, destinationLevelFunction, destinationFunction, destinationWithoutTransientLevelFunction, destinationWithoutTransientFunction);
                                }
                                
                                std::vector<Destination> const& getDestinations() const {
                                    return destinations;
                                }
                                
                                ContainerType::const_iterator begin() const {
                                    return destinations.begin();
                                }
                                
                                ContainerType::const_iterator end() const {
                                    return destinations.end();
                                }
                                
                                ValueType rate(StateType const& in) const {
                                    return edgeRateFunction(in);
                                }
                                
                                void perform(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) const {
                                    edgeTransientFunction(in, transientIn, transientOut {% if exploration_checks %}, variableWrites {% endif %});
                                }
                                
                            private:
                                EdgeEnabledFunctionPtr edgeEnabledFunction;
                                EdgeRateFunctionPtr edgeRateFunction;
                                EdgeTransientFunctionPtr edgeTransientFunction;
                                ContainerType destinations;
                            };
                            
                            void locations_perform(StateType const& in, TransientVariables const& transientIn, TransientVariables& transientOut {% if exploration_checks %}, VariableWrites& variableWrites {% endif %}) {
                                {% for location in locations %}if ({$location.guard}) {
                                    {% for assignment in location.assignments %}transientOut.{$assignment.variable} = {$assignment.value};
                                    {% if exploration_checks %}
                                    if (variableWrites.{$assignment.variable}) {
                                        throw storm::exceptions::WrongFormatException("Illegal write to {$assignment.variable}, variable has been written before.");
                                    } else {
                                        variableWrites.{$assignment.variable} = true;
                                    }
                                    {% endif %}
                                    {% endfor %}
                                }
                                {% endfor %}
                            }
                            
                            class JitBuilder : public JitModelBuilderInterface<IndexType, ValueType> {
                            public:
                                JitBuilder(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder) : JitModelBuilderInterface(modelComponentsBuilder), timeOfStart(std::chrono::high_resolution_clock::now()), timeOfLastMessage(std::chrono::high_resolution_clock::now()), numberOfExploredStates(0), numberOfExploredStatesSinceLastMessage(0) {
                                    {% for state in initialStates %}{
                                        StateType state;
                                        {% for assignment in state %}state.{$assignment.variable} = {$assignment.value};
                                        {% endfor %}
                                        initialStates.push_back(state);
                                    }{% endfor %}
                                    {% for edge in nonsynch_edges %}{
                                        edge_{$edge.name} = Edge(&edge_enabled_{$edge.name}, &edge_rate_{$edge.name}, edge_perform_{$edge.name});
                                        {% for destination in edge.destinations %}edge_{$edge.name}.addDestination({$destination.lowestLevel}, {$destination.highestLevel}, &destination_probability_{$edge.name}_{$destination.name}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name});
                                        {% endfor %}
                                    }
                                    {% endfor %}
                                    {% for edge in synch_edges %}{
                                        edge_{$edge.name} = Edge(&edge_enabled_{$edge.name}, &edge_rate_{$edge.name}, edge_perform_{$edge.name});
                                        {% for destination in edge.destinations %}edge_{$edge.name}.addDestination({$destination.lowestLevel}, {$destination.highestLevel}, &destination_probability_{$edge.name}_{$destination.name}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name}, &destination_perform_level_{$edge.name}_{$destination.name}, &destination_perform_{$edge.name}_{$destination.name});
                                        {% endfor %}
                                    }
                                    {% endfor %}
                                    {% for reward in rewards %}
                                    modelComponentsBuilder.registerRewardModel(RewardModelInformation("{$reward.name}", {$reward.location_rewards}, {$reward.edge_rewards} || {$reward.destination_rewards}, false));
                                    {% endfor %}
                                }
                                
                                virtual storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>* build() override {
#ifndef NDEBUG
                                    std::cout << "starting building process\n";
#endif
                                    explore(initialStates);
#ifndef NDEBUG
                                    std::cout << "finished building process with " << stateIds.size() << " states\n";
#endif
                                    
#ifndef NDEBUG
                                    std::cout << "building labeling\n";
#endif
                                    label();
#ifndef NDEBUG
                                    std::cout << "finished building labeling\n";
#endif
                                    
                                    return this->modelComponentsBuilder.build(stateIds.size());
                                }
                                
                                void label() {
                                    uint64_t labelCount = 0;
                                    {% for label in labels %}this->modelComponentsBuilder.registerLabel("{$label.name}", stateIds.size());
                                    ++labelCount;
                                    {% endfor %}
                                    this->modelComponentsBuilder.registerLabel("init", stateIds.size());
                                    this->modelComponentsBuilder.registerLabel("deadlock", stateIds.size());
                                    
                                    for (auto const& stateEntry : stateIds) {
                                        auto const& in = stateEntry.first;
                                        {% for label in labels %}if ({$label.predicate}) {
                                            this->modelComponentsBuilder.addLabel(stateEntry.second, {$loop.index} - 1);
                                        }
                                        {% endfor %}
                                    }
                                    
                                    for (auto const& state : initialStates) {
                                        auto stateIt = stateIds.find(state);
                                        if (stateIt != stateIds.end()) {
                                            this->modelComponentsBuilder.addLabel(stateIt->second, labelCount);
                                        }
                                    }
                                    
                                    for (auto const& stateId : deadlockStates) {
                                        this->modelComponentsBuilder.addLabel(stateId, labelCount + 1);
                                    }
                                }
                                
                                void explore(std::vector<StateType> const& initialStates) {
                                    for (auto const& state : initialStates) {
                                        explore(state);
                                    }
                                }
                                
                                void explore(StateType const& initialState) {
                                    StateSet<StateType> statesToExplore;
                                    getOrAddIndex(initialState, statesToExplore);
                                    
                                    StateBehaviour<IndexType, ValueType> behaviour;
                                    
                                    while (!statesToExplore.empty()) {
                                        StateType currentState = statesToExplore.get();
                                        IndexType currentIndex = getIndex(currentState);
                                        
                                        if (!isTerminalState(currentState)) {
#ifndef NDEBUG
                                            std::cout << "Exploring state " << currentState << ", id " << currentIndex << '\n';
#endif
                                            
                                            behaviour.setExpanded();
                                            
                                            {% if exploration_checks %}VariableWrites variableWrites;
                                            {% endif %}
                                            
                                            // Perform transient location assignments.
                                            TransientVariables transientIn;
                                            TransientVariables transientOut;
                                            locations_perform(currentState, transientIn, transientOut {% if exploration_checks %}, variableWrites {% endif %});
                                            {% for reward in location_rewards %}
                                            behaviour.addStateReward(transientOut.{$reward.variable});
                                            {% endfor %}
                                            
                                            // Explore all edges that do not take part in synchronization vectors.
                                            exploreNonSynchronizingEdges(currentState, transientOut, behaviour, statesToExplore);
                                            
                                            // Explore all edges that participate in synchronization vectors.
                                            exploreSynchronizingEdges(currentState, transientOut, behaviour, statesToExplore);
                                        }

                                        {% if dontFixDeadlocks %}
                                        if (behaviour.empty() && behaviour.isExpanded() ) {
                                            std::cout << "found deadlock state: " << currentState << '\n';
                                            throw storm::exceptions::WrongFormatException("Error while creating sparse matrix from JANI model: found deadlock state and fixing deadlocks was explicitly disabled.");
                                        }
                                        {% endif %}

                                        this->addStateBehaviour(currentIndex, behaviour);
                                        behaviour.clear();
                                        
#ifdef EXPL_PROGRESS
                                        ++numberOfExploredStatesSinceLastMessage;
                                        ++numberOfExploredStates;

                                        auto now = std::chrono::high_resolution_clock::now();
                                        auto durationSinceLastMessage = std::chrono::duration_cast<std::chrono::seconds>(now - timeOfLastMessage).count();
                                        if (static_cast<uint64_t>(durationSinceLastMessage) >= {$expl_progress_interval}) {
                                            auto statesPerSecond = numberOfExploredStatesSinceLastMessage / durationSinceLastMessage;
                                            auto durationSinceStart = std::chrono::duration_cast<std::chrono::seconds>(now - timeOfStart).count();
                                            std::cout << "Explored " << numberOfExploredStates << " states in " << durationSinceStart << " seconds (currently " << statesPerSecond << " states per second).\n";
                                            timeOfLastMessage = std::chrono::high_resolution_clock::now();
                                            numberOfExploredStatesSinceLastMessage = 0;
                                        }
#endif
                                    }
                                }
                                
                                bool isTerminalState(StateType const& in) const {
                                    {% for expression in terminalExpressions %}if ({$expression}) {
                                        return true;
                                    }
                                    {% endfor %}
                                    return false;
                                }
                                
                                void exploreNonSynchronizingEdges(StateType const& in, TransientVariables const& transientIn, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore) {
                                    {% for edge in nonsynch_edges %}{
#ifndef NDEBUG
                                        std::cout << "Exploring non-synchronizing edge {$edge.name}.\n";
#endif
                                        if ({$edge.guard}) {
                                            Choice<IndexType, ValueType>& choice = behaviour.addChoice(!model_is_deterministic() && !model_is_discrete_time() && {$edge.markovian});
                                            choice.resizeRewards({$edge_destination_rewards_count});
                                            {
                                                {% if exploration_checks %}VariableWrites variableWrites;
                                                {% endif %}

                                                TransientVariables transient;
                                                {% if edge.transient_assignments %}
                                                edge_perform_{$edge.name}(in, transient, transient {% if exploration_checks %}, variableWrites{% endif %});
                                                {% endif %}
                                                {% for reward in edge_rewards %}
                                                choice.addReward({$reward.index}, transient.{$reward.variable});
                                                {% endfor %}
                                            }
                                            auto rate = edge_rate_{$edge.name}(in);
                                            {% for destination in edge.destinations %}{
                                                {% if exploration_checks %}VariableWrites variableWrites;
                                                {% endif %}

                                                StateType out(in);
                                                TransientVariables transientIn;
                                                TransientVariables transientOut;
                                                destination_perform_{$edge.name}_{$destination.name}(in, out{% if edge.transient_variables_in_destinations %}, transientIn, transientOut{% endif %}{% if exploration_checks %}, variableWrites{% endif %});
                                                IndexType outStateIndex = getOrAddIndex(out, statesToExplore);
                                                auto probability = destination_probability_{$edge.name}_{$destination.name}(in);
                                                choice.add(outStateIndex, rate * probability);
                                                {% for reward in destination_rewards %}
                                                choice.addReward({$reward.index}, probability * transientOut.{$reward.variable});
                                                {% endfor %}
                                            }
                                            {% endfor %}
                                        }
                                    }
                                    {% endfor %}
                                }
                                
                                {% for vector in synch_vectors %}{$vector.functions}
                                {% endfor %}
                                
                                void exploreSynchronizingEdges(StateType const& state, TransientVariables const& transientIn, StateBehaviour<IndexType, ValueType>& behaviour, StateSet<StateType>& statesToExplore) {
                                    {% for vector in synch_vectors %}{
                                        exploreSynchronizationVector_{$vector.index}(state, transientIn, behaviour, statesToExplore);
                                    }
                                    {% endfor %}
                                }
                                
                                IndexType getOrAddIndex(StateType const& state, StateSet<StateType>& statesToExplore) {
                                    auto it = stateIds.find(state);
                                    if (it != stateIds.end()) {
                                        return it->second;
                                    } else {
                                        IndexType newIndex = static_cast<IndexType>(stateIds.size());
                                        stateIds.insert(std::make_pair(state, newIndex));
                                        statesToExplore.add(state);
                                        return newIndex;
                                    }
                                }
                                
                                IndexType getIndex(StateType const& state) const {
                                    auto it = stateIds.find(state);
                                    if (it != stateIds.end()) {
                                        return it->second;
                                    } else {
                                        return stateIds.at(state);
                                    }
                                }
                                
                                void addStateBehaviour(IndexType const& stateId, StateBehaviour<IndexType, ValueType>& behaviour) {
                                    if (behaviour.empty() && behaviour.isExpanded()) {
                                        deadlockStates.push_back(stateId);
                                    }
                                    
                                    JitModelBuilderInterface<IndexType, ValueType>::addStateBehaviour(stateId, behaviour);
                                }
                                
                                static JitModelBuilderInterface<IndexType, ValueType>* create(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder) {
                                    return new JitBuilder(modelComponentsBuilder);
                                }
                                
                            private:
                                // State storage.
                                spp::sparse_hash_map<StateType, IndexType> stateIds;
                                std::vector<StateType> initialStates;
                                std::vector<IndexType> deadlockStates;

                                // Edges.
                                {% for edge in nonsynch_edges %}Edge edge_{$edge.name};
                                {% endfor %}
                                {% for edge in synch_edges %}Edge edge_{$edge.name};
                                {% endfor %}
                                
                                // Statistics.
                                std::chrono::high_resolution_clock::time_point timeOfStart;
                                std::chrono::high_resolution_clock::time_point timeOfLastMessage;
                                uint64_t numberOfExploredStates;
                                uint64_t numberOfExploredStatesSinceLastMessage;
                            };
                            
                            BOOST_DLL_ALIAS(storm::builder::jit::JitBuilder::create, create_builder)
                            {% if parametric %}
                            BOOST_DLL_ALIAS(storm::builder::jit::initialize_parameters, initialize_parameters)
                            {% endif %}
                        }
                    }
                }
                )";

    return cpptempl::parse(sourceTemplate, modelData);
}

template<typename ValueType, typename RewardModelType>
boost::filesystem::path ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::compileToSharedLibrary(boost::filesystem::path const& sourceFile) {
    std::string sourceFilename = boost::filesystem::absolute(sourceFile).string();
    auto dynamicLibraryPath = sourceFile;
    dynamicLibraryPath += DYLIB_EXTENSION;
    std::string dynamicLibraryFilename = boost::filesystem::absolute(dynamicLibraryPath).string();
    std::string includes = "";
    for (std::string const& dir :
         {stormIncludeDirectory, sparseppIncludeDirectory, boostIncludeDirectory, carlIncludeDirectory, clnIncludeDirectory, gmpIncludeDirectory}) {
        if (dir != "") {
            includes += " -I" + dir;
        }
    }
    std::string command = compiler + " " + sourceFilename + " " + compilerFlags + includes + " -o " + dynamicLibraryFilename;
    boost::optional<std::string> error = execute(command);

    if (error) {
        boost::filesystem::remove(sourceFile);
        STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Compiling shared library failed. Error: " << error.get());
    }

    return dynamicLibraryPath;
}

template<typename RationalFunctionType, typename TP = typename RationalFunctionType::PolyType, carl::EnableIf<carl::needs_cache<TP>> = carl::dummy>
RationalFunctionType convertVariableToPolynomial(storm::RationalFunctionVariable const& variable, std::shared_ptr<storm::RawPolynomialCache> cache) {
    return RationalFunctionType(typename RationalFunctionType::PolyType(typename RationalFunctionType::PolyType::PolyType(variable), cache));
}

template<typename RationalFunctionType, typename TP = typename RationalFunctionType::PolyType, carl::DisableIf<carl::needs_cache<TP>> = carl::dummy>
RationalFunctionType convertVariableToPolynomial(storm::RationalFunctionVariable const& variable, std::shared_ptr<storm::RawPolynomialCache>) {
    return RationalFunctionType(variable);
}

template<typename ValueType>
std::vector<storm::RationalFunction> getParameters(storm::jani::Model const&, std::shared_ptr<storm::RawPolynomialCache>) {
    STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "This function must not be called for this type.");
}

template<>
std::vector<storm::RationalFunction> getParameters<storm::RationalFunction>(storm::jani::Model const& model, std::shared_ptr<storm::RawPolynomialCache> cache) {
    std::vector<storm::RationalFunction> parameters;
    for (auto const& constant : model.getConstants()) {
        if (!constant.isDefined() && constant.isRealConstant()) {
            parameters.push_back(convertVariableToPolynomial<storm::RationalFunction>(carl::freshRealVariable(constant.getName()), cache));
        }
    }
    return parameters;
}

template<typename ValueType, typename RewardModelType>
void ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::createBuilder(boost::filesystem::path const& dynamicLibraryPath) {
    jitBuilderCreateFunction =
        boost::dll::import_alias<typename ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::CreateFunctionType>(dynamicLibraryPath, "create_builder");
    builder = std::unique_ptr<JitModelBuilderInterface<IndexType, ValueType>>(jitBuilderCreateFunction(modelComponentsBuilder));

    if (std::is_same<storm::RationalFunction, ValueType>::value) {
        typedef void(InitializeParametersFunctionType)(std::vector<storm::RationalFunction> const&);
        typedef boost::function<InitializeParametersFunctionType> ImportInitializeParametersFunctionType;

        // Create the carl cache if we are building a parametric model.
        cache = std::make_shared<storm::RawPolynomialCache>();

        ImportInitializeParametersFunctionType initializeParametersFunction =
            boost::dll::import_alias<InitializeParametersFunctionType>(dynamicLibraryPath, "initialize_parameters");
        std::vector<storm::RationalFunction> parameters = getParameters<ValueType>(this->model, cache);
        initializeParametersFunction(parameters);
    }
}

template class ExplicitJitJaniModelBuilder<double, storm::models::sparse::StandardRewardModel<double>>;
template class ExplicitJitJaniModelBuilder<storm::RationalNumber, storm::models::sparse::StandardRewardModel<storm::RationalNumber>>;
template class ExplicitJitJaniModelBuilder<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>;

}  // namespace jit
}  // namespace builder
}  // namespace storm
