#pragma  once

#include <map>

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/parser/ExpressionParser.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/storage/dft/DFTBuilder.h"


namespace storm {
    namespace parser {

        template<typename ValueType>
        class DFTGalileoParser {
            storm::storage::DFTBuilder<ValueType> builder;

            std::shared_ptr<storm::expressions::ExpressionManager> manager;

            storm::parser::ExpressionParser parser;

            storm::expressions::ExpressionEvaluator<ValueType> evaluator;

            std::unordered_map<std::string, storm::expressions::Expression> identifierMapping;

        public:
            DFTGalileoParser(bool defaultInclusive = true, bool binaryDependencies = true) : builder(defaultInclusive, binaryDependencies), manager(new storm::expressions::ExpressionManager()), parser(*manager), evaluator(*manager) {
            }

            storm::storage::DFT<ValueType> parseDFT(std::string const& filename);
            
        private:
            void readFile(std::string const& filename);

            std::string stripQuotsFromName(std::string const& name);
            std::string parseNodeIdentifier(std::string const& name);

            ValueType parseRationalExpression(std::string const& expr);
            
            bool defaultInclusive;
        };
    }
}
