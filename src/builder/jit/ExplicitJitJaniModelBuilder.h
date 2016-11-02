#pragma once

#include <memory>

#include <boost/filesystem.hpp>
#include <boost/dll/import.hpp>
#include <boost/function.hpp>

#include "cpptempl.h"

#include "src/storage/jani/Model.h"
#include "src/storage/jani/ParallelComposition.h"
#include "src/storage/expressions/ToCppVisitor.h"

#include "src/builder/BuilderOptions.h"
#include "src/builder/jit/JitModelBuilderInterface.h"
#include "src/builder/jit/ModelComponentsBuilder.h"

namespace storm {
    namespace models {
        namespace sparse {
            template <typename ValueType, typename RewardModelType>
            class Model;
            
            template <typename ValueType>
            class StandardRewardModel;
        }
    }
    
    namespace builder {
        namespace jit {
            
            typedef uint32_t IndexType;
            
            template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
            class ExplicitJitJaniModelBuilder {
            public:
                typedef JitModelBuilderInterface<IndexType, ValueType>* (CreateFunctionType)(ModelComponentsBuilder<IndexType, ValueType>& modelComponentsBuilder);
                typedef boost::function<CreateFunctionType> ImportFunctionType;
                
                ExplicitJitJaniModelBuilder(storm::jani::Model const& model, storm::builder::BuilderOptions const& options = storm::builder::BuilderOptions());
                
                std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> build();
                
            private:
                void createBuilder(boost::filesystem::path const& dynamicLibraryPath);
                std::string createSourceCode();
                boost::filesystem::path writeSourceToTemporaryFile(std::string const& source);
                boost::filesystem::path compileSourceToSharedLibrary(boost::filesystem::path const& sourceFile);
                
                static boost::optional<std::string> execute(std::string command);
                
                // Functions that generate data maps or data templates.
                cpptempl::data_list generateInitialStates();
                
                cpptempl::data_map generateBooleanVariable(storm::jani::BooleanVariable const& variable);
                cpptempl::data_map generateBoundedIntegerVariable(storm::jani::BoundedIntegerVariable const& variable);
                cpptempl::data_map generateUnboundedIntegerVariable(storm::jani::UnboundedIntegerVariable const& variable);
                cpptempl::data_map generateRealVariable(storm::jani::RealVariable const& variable);
                cpptempl::data_map generateLocationVariable(storm::jani::Automaton const& automaton);
                void generateVariables(cpptempl::data_map& modelData);

                void generateLocations(cpptempl::data_map& modelData);
                void generateRewards(cpptempl::data_map& modelData);
                
                cpptempl::data_list generateLabels();
                cpptempl::data_list generateTerminalExpressions();
                void generateEdges(cpptempl::data_map& modelData);
                cpptempl::data_map generateSynchronizationVector(storm::jani::ParallelComposition const& parallelComposition, storm::jani::SynchronizationVector const& synchronizationVector, uint64_t synchronizationVectorIndex);
                cpptempl::data_list generateLevels(storm::jani::OrderedAssignments const& assignments);
                
                cpptempl::data_map generateEdge(storm::jani::Automaton const& automaton, uint64_t edgeIndex, storm::jani::Edge const& edge);
                cpptempl::data_map generateDestination(uint64_t destinationIndex, storm::jani::EdgeDestination const& destination);
                
                template <typename ValueTypePrime>
                cpptempl::data_map generateAssignment(storm::jani::Variable const& variable, ValueTypePrime value) const;
                
                cpptempl::data_map generateLocationAssignment(storm::jani::Automaton const& automaton, uint64_t value) const;
                
                cpptempl::data_map generateAssignment(storm::jani::Assignment const& assignment);
                
                // Auxiliary functions that perform regularly needed steps.
                std::string const& getVariableName(storm::expressions::Variable const& variable) const;
                std::string const& registerVariable(storm::expressions::Variable const& variable, bool transient = false);
                storm::expressions::Variable const& getLocationVariable(storm::jani::Automaton const& automaton) const;
                std::string asString(bool value) const;
                storm::expressions::Expression shiftVariablesWrtLowerBound(storm::expressions::Expression const& expression);

                template <typename ValueTypePrime>
                std::string asString(ValueTypePrime value) const;
                
                storm::builder::BuilderOptions options;
                storm::jani::Model model;
                
                ModelComponentsBuilder<IndexType, ValueType> modelComponentsBuilder;
                typename ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::ImportFunctionType jitBuilderGetFunction;
                std::unique_ptr<JitModelBuilderInterface<IndexType, ValueType>> builder;
                
                std::unordered_map<storm::expressions::Variable, std::string> variableToName;
                std::map<std::string, storm::expressions::Variable> automatonToLocationVariable;
                
                storm::expressions::ToCppVisitor expressionTranslator;
                std::map<storm::expressions::Variable, storm::expressions::Expression> lowerBoundShiftSubstitution;
                std::map<storm::expressions::Variable, int_fast64_t> lowerBounds;
                std::set<storm::expressions::Variable> transientVariables;
                std::set<storm::expressions::Variable> nontransientVariables;
                std::unordered_map<storm::expressions::Variable, std::string> variablePrefixes;
            };

        }
    }
}
