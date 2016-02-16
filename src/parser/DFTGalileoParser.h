#ifndef DFTGALILEOPARSER_H
#define	DFTGALILEOPARSER_H

#include "src/storage/dft/DFT.h"
#include "src/storage/dft/DFTBuilder.h"
#include "src/storage/expressions/ExpressionManager.h"
#include "src/parser/ExpressionParser.h"
#include "src/storage/expressions/ExpressionEvaluator.h"

#include <map>

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
            DFTGalileoParser() : manager(new storm::expressions::ExpressionManager()), parser(*manager), evaluator(*manager) {
            }

            storm::storage::DFT<ValueType> parseDFT(std::string const& filename);
            
        private:
            void readFile(std::string const& filename);

            std::string stripQuotsFromName(std::string const& name);

            ValueType parseRationalExpression(std::string const& expr);
        };
    }
}

#endif	/* DFTGALILEOPARSER_H */

