#pragma once

#include <memory>

#include <boost/filesystem.hpp>
#include <boost/dll/import.hpp>
#include <boost/function.hpp>

#include "cpptempl.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/ParallelComposition.h"
#include "storm/storage/expressions/ToCppVisitor.h"

#include "storm/builder/BuilderOptions.h"
#include "storm/builder/jit/JitModelBuilderInterface.h"
#include "storm/builder/jit/ModelComponentsBuilder.h"

namespace storm {
    namespace models {
        namespace sparse {
            template <typename ValueType, typename RewardModelType>
            class Model;
            
            template <typename ValueType>
            class StandardRewardModel;
        }
    }

    namespace jani {
        class OrderedAssignments;
        class Assignment;
        class Variable;
        class Edge;
        class EdgeDestination;
    }
    
    namespace builder {
        namespace jit {
            
            typedef uint32_t IndexType;
            
            template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
            class ExplicitJitJaniModelBuilder {
            public:
                typedef JitModelBuilderInterface<IndexType, ValueType>* (CreateFunctionType)(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder);
                typedef boost::function<CreateFunctionType> ImportCreateFunctionType;
                
                /*!
                 * Returns the jani features with which this builder can deal natively.
                 */
                static storm::jani::ModelFeatures getSupportedJaniFeatures();
                
                /*!
                 * A quick check to detect whether the given model is not supported.
                 * This method only over-approximates the set of models that can be handled, i.e., if this
                 * returns true, the model might still be unsupported.
                 */
                static bool canHandle(storm::jani::Model const& model);
                
                /*!
                 * Creates a model builder for the given model. The provided options are used to determine which part of
                 * the model is built.
                 */
                ExplicitJitJaniModelBuilder(storm::jani::Model const& model, storm::builder::BuilderOptions const& options = storm::builder::BuilderOptions());
                
                /*!
                 * Builds and returns the sparse model.
                 */
                std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> build();
                
                /*!
                 * Performs some checks that can help debug why the model builder does not work. Returns true if the
                 * general infrastructure for the model builder appears to be working.
                 */
                bool doctor() const;

            private:
                // Helper methods for the doctor() procedure.
                bool checkTemporaryFileWritable() const;
                bool checkCompilerWorks() const;
                bool checkCompilerFlagsWork() const;
                bool checkBoostAvailable() const;
                bool checkBoostDllAvailable() const;
                bool checkStormHeadersAvailable() const;
                bool checkCarlAvailable() const;
                
                /*!
                 * Executes the given command. If the command fails with a non-zero error code, the error stream content
                 * is returned and boost::none otherwise.
                 */
                static boost::optional<std::string> execute(std::string command);
                
                /*!
                 * Writes the given content to a temporary file. The temporary file is created to have the provided suffix.
                 */
                static boost::filesystem::path writeToTemporaryFile(std::string const& content, std::string const& suffix = ".cpp");

                /*!
                 * Assembles the information of the model such that it can be put into the source skeleton.
                 */
                cpptempl::data_map generateModelData();

                void generateVariables(cpptempl::data_map& modelData);
                cpptempl::data_map generateBooleanVariable(storm::jani::BooleanVariable const& variable);
                cpptempl::data_map generateBoundedIntegerVariable(storm::jani::BoundedIntegerVariable const& variable);
                cpptempl::data_map generateUnboundedIntegerVariable(storm::jani::UnboundedIntegerVariable const& variable);
                cpptempl::data_map generateRealVariable(storm::jani::RealVariable const& variable);
                cpptempl::data_map generateLocationVariable(storm::jani::Automaton const& automaton);

                void generateInitialStates(cpptempl::data_map& modelData);
                void generateRewards(cpptempl::data_map& modelData);
                void generateLocations(cpptempl::data_map& modelData);
                void generateLabels(cpptempl::data_map& modelData);
                void generateTerminalExpressions(cpptempl::data_map& modelData);
                void generateParameters(cpptempl::data_map& modelData);
                
                // Functions related to the generation of edge data.
                void generateEdges(cpptempl::data_map& modelData);
                cpptempl::data_map generateSynchronizationVector(cpptempl::data_map& modelData, storm::jani::ParallelComposition const& parallelComposition, storm::jani::SynchronizationVector const& synchronizationVector, uint64_t synchronizationVectorIndex);
                cpptempl::data_list generateLevels(storm::jani::Automaton const& automaton, uint64_t destinationLocationIndex, storm::jani::OrderedAssignments const& assignments);
                cpptempl::data_map generateEdge(storm::jani::Automaton const& automaton, uint64_t edgeIndex, storm::jani::Edge const& edge);
                cpptempl::data_map generateDestination(storm::jani::Automaton const& automaton, uint64_t destinationIndex, storm::jani::EdgeDestination const& destination);
                template <typename ValueTypePrime>
                cpptempl::data_map generateAssignment(storm::jani::Variable const& variable, ValueTypePrime value) const;
                cpptempl::data_map generateLocationAssignment(storm::jani::Automaton const& automaton, uint64_t value) const;
                cpptempl::data_map generateAssignment(storm::jani::Assignment const& assignment);

                // Auxiliary functions that perform regularly needed steps.
                std::string const& getVariableName(storm::expressions::Variable const& variable) const;
                std::string const& registerVariable(storm::expressions::Variable const& variable, bool transient = false);
                storm::expressions::Variable const& getLocationVariable(storm::jani::Automaton const& automaton) const;
                storm::expressions::Expression shiftVariablesWrtLowerBound(storm::expressions::Expression const& expression);
                
                // Conversion functions.
                template <typename ValueTypePrime>
                std::string asString(ValueTypePrime value) const;
                std::string asString(bool value) const;

                /*!
                 * Creates the source code for the shared library that performs the model generation.
                 *
                 * @param modelData The assembled data of the model to be put into the blueprint. Note that this is not
                 * modified in this function, but the data needs to be passed as non-const to cpptempl.
                 */
                std::string createSourceCodeFromSkeleton(cpptempl::data_map& modelData);
                
                /*!
                 * Compiles the provided source file to a shared library and returns a path object to the resulting
                 * binary file.
                 */
                boost::filesystem::path compileToSharedLibrary(boost::filesystem::path const& sourceFile);

                /*!
                 * Loads the given shared library and creates the builder from it.
                 */
                void createBuilder(boost::filesystem::path const& dynamicLibraryPath);

                /// The options to use for model building.
                storm::builder::BuilderOptions options;
                
                /// The model specification that is to be built.
                storm::jani::Model model;
                
                /// A vector of automata that is to be put in parallel. The automata are references from the model specification.
                std::vector<std::reference_wrapper<storm::jani::Automaton const>> parallelAutomata;
                
                /// An object that is responsible for building the components of the model. The shared library gets this
                /// as a mechanism to perform a callback in order to register transitions that were found.
                ModelComponentsBuilder<IndexType, ValueType> modelComponentsBuilder;
                
                /// The function that was loaded from the shared library. We have to keep this function around, because
                /// otherwise the shared library gets unloaded.
                typename ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::ImportCreateFunctionType jitBuilderCreateFunction;
                
                /// The pointer to the builder object created via the shared library.
                std::unique_ptr<JitModelBuilderInterface<IndexType, ValueType>> builder;
                
                // Members that store information about the model. They are used in the process of assembling the model
                // data that is used in the skeleton.
                std::unordered_map<storm::expressions::Variable, std::string> variableToName;
                storm::expressions::ToCppVisitor expressionTranslator;
                std::map<storm::expressions::Variable, storm::expressions::Expression> lowerBoundShiftSubstitution;
                std::map<storm::expressions::Variable, int_fast64_t> lowerBounds;
                std::set<storm::expressions::Variable> transientVariables;
                std::set<storm::expressions::Variable> nontransientVariables;
                std::set<storm::expressions::Variable> realVariables;
                std::unordered_map<storm::expressions::Variable, std::string> variablePrefixes;

                /// The compiler binary.
                std::string compiler;

                /// The flags passed to the compiler.
                std::string compilerFlags;
                
                /// The include directory of boost.
                std::string boostIncludeDirectory;
                
                /// The include directory of storm.
                std::string stormIncludeDirectory;
                
                /// The include directory of carl.
                std::string carlIncludeDirectory;
                
                /// The include directory of sparsepp.
                std::string sparseppIncludeDirectory;
                
                /// The include directory for cln
                std::string clnIncludeDirectory;
                
                /// The include directory for gmp
                std::string gmpIncludeDirectory;
                
                /// A cache that is used by carl.
                std::shared_ptr<storm::RawPolynomialCache> cache;
            };

        }
    }
}
