#include "src/builder/ExplicitJitJaniModelBuilder.h"

#include <iostream>
#include <cstdio>

#include "cpptempl.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace builder {
        
        static const std::string CXX_COMPILER = "clang++";
        static const std::string DYLIB_EXTENSION = ".dylib";
        static const std::string COMPILER_FLAGS = "-std=c++11 -stdlib=libc++ -fPIC -O3 -shared";
        static const std::string STORM_ROOT = "/Users/chris/work/storm2";
        static const std::string BOOST_ROOT = "/usr/local/Cellar/boost/1.61.0_1/include";
        
        template <typename ValueType>
        ExplicitJitJaniModelBuilder<ValueType>::ExplicitJitJaniModelBuilder(storm::jani::Model const& model) : model(model) {
            // Intentionally left empty.
        }
        
        template <typename ValueType>
        std::string ExplicitJitJaniModelBuilder<ValueType>::createSourceCode() {
            std::string sourceTemplate = R"(
            
            #include <cstdint>
            #include <iostream>
            #include <boost/dll/alias.hpp>
            
            #include "src/builder/JitModelBuilderInterface.h"
            #include "resources/3rdparty/sparsepp/sparsepp.h"
            
            namespace storm {
                namespace builder {
                    
                    struct StateType {
                        {% for stateVariable in stateVariables %}int64_t {$stateVariable.name} : {$stateVariable.bitwidth};{% endfor %}
                    };
                    
                    class JitBuilder : public JitModelBuilderInterface<double> {
                    public:
                        JitBuilder() {
                            // Intentionally left empty.
                        }
                        
                        virtual void build() override {
                            std::cout << "building in progress" << std::endl;
                        }
                        
                        static JitModelBuilderInterface<double>* create() {
                            return new JitBuilder();
                        }
                        
                    private:
                        spp::sparse_hash_map<StateType, uint32_t> stateIds;
                    };
                    
                    BOOST_DLL_ALIAS(storm::builder::JitBuilder::create, create_builder)
                    
                }
            }
            )";
            
            cpptempl::data_map modelData;
            return cpptempl::parse(sourceTemplate, modelData);
        }
        
        template <typename ValueType>
        boost::optional<std::string> ExplicitJitJaniModelBuilder<ValueType>::execute(std::string command) {
            char buffer[128];
            std::stringstream output;
            command += " 2>&1";
            
            std::cout << "executing " << command << std::endl;
            
            std::unique_ptr<FILE> pipe(popen(command.c_str(), "r"));
            STORM_LOG_THROW(pipe, storm::exceptions::InvalidStateException, "Call to popen failed.");
            
            while (!feof(pipe.get())) {
                if (fgets(buffer, 128, pipe.get()) != nullptr)
                    output << buffer;
            }
            int result = pclose(pipe.get());
            pipe.release();
            
            if (WEXITSTATUS(result) == 0) {
                return boost::none;
            } else {
                return "Executing command failed. Got response: " + output.str();
            }
        }

        template <typename ValueType>
        void ExplicitJitJaniModelBuilder<ValueType>::createBuilder(boost::filesystem::path const& dynamicLibraryPath) {
            jitBuilderGetFunction = boost::dll::import_alias<typename ExplicitJitJaniModelBuilder<ValueType>::CreateFunctionType>(dynamicLibraryPath, "create_builder");
            builder = std::unique_ptr<JitModelBuilderInterface<ValueType>>(jitBuilderGetFunction());
        }
        
        template <typename ValueType>
        boost::filesystem::path ExplicitJitJaniModelBuilder<ValueType>::writeSourceToTemporaryFile(std::string const& source) {
            boost::filesystem::path temporaryFile = boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%.cpp");
            std::ofstream out(temporaryFile.native());
            out << source << std::endl;
            out.close();
            return temporaryFile;
        }
        
        template <typename ValueType>
        boost::filesystem::path ExplicitJitJaniModelBuilder<ValueType>::compileSourceToSharedLibrary(boost::filesystem::path const& sourceFile) {
            std::string sourceFilename = boost::filesystem::absolute(sourceFile).string();
            auto dynamicLibraryPath = sourceFile;
            dynamicLibraryPath += DYLIB_EXTENSION;
            std::string dynamicLibraryFilename = boost::filesystem::absolute(dynamicLibraryPath).string();
            
            std::string command = CXX_COMPILER + " " + sourceFilename + " " + COMPILER_FLAGS + " -I" + STORM_ROOT + " -I" + BOOST_ROOT + " -o " + dynamicLibraryFilename;
            boost::optional<std::string> error = execute(command);
            
            if (error) {
                boost::filesystem::remove(sourceFile);
                STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Compiling shared library failed. Error: " << error.get());
            }
            
            return dynamicLibraryPath;
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType, storm::models::sparse::StandardRewardModel<ValueType>>> ExplicitJitJaniModelBuilder<ValueType>::build() {
         
            // (1) generate the source code of the shared library
            std::string source = createSourceCode();
            std::cout << "created source code: " << source << std::endl;
            
            // (2) write the source code to a temporary file
            boost::filesystem::path temporarySourceFile = writeSourceToTemporaryFile(source);
            std::cout << "wrote source to file " << temporarySourceFile.native() << std::endl;
            
            // (3) compile the shared library
            boost::filesystem::path dynamicLibraryPath = compileSourceToSharedLibrary(temporarySourceFile);
            std::cout << "successfully compiled shared library" << std::endl;
            
            // (4) remove the source code we just compiled
            boost::filesystem::remove(temporarySourceFile);
            
            // (5) create the loader from the shared library
            createBuilder(dynamicLibraryPath);
                
            // (6) execute the function in the shared lib
            builder->build();
                
            // (7) use result to build the model
                
            // (8) delete the shared library
            boost::filesystem::remove(dynamicLibraryPath);
            
            // FIXME
            return nullptr;
        }
 
        template class ExplicitJitJaniModelBuilder<double>;
        
    }
}
