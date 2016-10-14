#pragma once

#include <memory>

#include <boost/filesystem.hpp>
#include <boost/dll/import.hpp>
#include <boost/function.hpp>

#include "src/storage/jani/Model.h"

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
        
        template <typename ValueType>
        class ExplicitJitJaniModelBuilder {
        public:
            typedef JitModelBuilderInterface<ValueType>* (CreateFunctionType)();
            typedef boost::function<CreateFunctionType> ImportFunctionType;
            
            ExplicitJitJaniModelBuilder(storm::jani::Model const& model);
            
            std::shared_ptr<storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>> build();
            
        private:
            void createBuilder(boost::filesystem::path const& dynamicLibraryPath);
            std::string createSourceCode();
            boost::filesystem::path writeSourceToTemporaryFile(std::string const& source);
            boost::filesystem::path compileSourceToSharedLibrary(boost::filesystem::path const& sourceFile);

            static boost::optional<std::string> execute(std::string command);
            
            storm::jani::Model const& model;
            typename ExplicitJitJaniModelBuilder<ValueType>::ImportFunctionType jitBuilderGetFunction;
            std::unique_ptr<JitModelBuilderInterface<ValueType>> builder;
        };
        
    }
}
