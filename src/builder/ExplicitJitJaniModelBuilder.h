#pragma once

#include <memory>

#include <boost/filesystem.hpp>
#include <boost/dll/import.hpp>
#include <boost/function.hpp>

#include "cpptempl.h"

#include "src/storage/jani/Model.h"
#include "src/storage/expressions/ToCppVisitor.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/builder/JitModelBuilderInterface.h"

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
        
        template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class ExplicitJitJaniModelBuilder {
        public:
            typedef JitModelBuilderInterface<ValueType>* (CreateFunctionType)();
            typedef boost::function<CreateFunctionType> ImportFunctionType;
            
            ExplicitJitJaniModelBuilder(storm::jani::Model const& model);
            
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> build();
            
        private:
            void createBuilder(boost::filesystem::path const& dynamicLibraryPath);
            std::string createSourceCode();
            boost::filesystem::path writeSourceToTemporaryFile(std::string const& source);
            boost::filesystem::path compileSourceToSharedLibrary(boost::filesystem::path const& sourceFile);

            static boost::optional<std::string> execute(std::string command);
            
            // Functions that generate data maps or data templates.
            cpptempl::data_list generateInitialStates();
            cpptempl::data_map generateStateVariables();
            cpptempl::data_list generateNonSynchronizingEdges();

            cpptempl::data_map generateEdge(storm::jani::Edge const& edge);
            cpptempl::data_map generateDestination(storm::jani::EdgeDestination const& destination);
            
            template <typename ValueTypePrime>
            cpptempl::data_map generateAssignment(storm::jani::Variable const& variable, ValueTypePrime value) const;
            
            cpptempl::data_map generateLocationAssignment(storm::jani::Automaton const& automaton, uint64_t value) const;

            cpptempl::data_map generateAssignment(storm::jani::Assignment const& assignment, std::string const& prefix = "");
            
            // Auxiliary functions that perform regularly needed steps.
            std::string const& getVariableName(storm::expressions::Variable const& variable) const;
            std::string getQualifiedVariableName(storm::jani::Automaton const& automaton, storm::jani::Variable const& variable) const;
            std::string getQualifiedVariableName(storm::jani::Variable const& variable) const;
            std::string getLocationVariableName(storm::jani::Automaton const& automaton) const;
            std::string asString(bool value) const;
            
            template <typename ValueTypePrime>
            std::string asString(ValueTypePrime value) const;
            
            storm::jani::Model model;
            typename ExplicitJitJaniModelBuilder<ValueType, RewardModelType>::ImportFunctionType jitBuilderGetFunction;
            std::unique_ptr<JitModelBuilderInterface<ValueType>> builder;
            
            std::unordered_map<storm::expressions::Variable, std::string> variableToName;
            storm::expressions::ToCppVisitor expressionTranslator;
        };
        
    }
}
