#pragma once

#include "Model.h"

// JSON parser
#include "json.hpp"
namespace modernjson = nlohmann;

namespace storm {
    namespace jani {
        class JsonExporter {
            JsonExporter() = default;
            
        public:
            static void toFile(storm::jani::Model const& janiModel, std::string const& filepath);
            static void toStream(storm::jani::Model const& janiModel, std::ostream& ostream);
            
        private:
            void convertModel(storm::jani::Model const& model);
            void appendVariableDeclaration(storm::jani::Variable const& variable);
            
            modernjson::json finalize() {
                return jsonStruct;
            }
            
            modernjson::json jsonStruct;
            
            
        };
    }
}