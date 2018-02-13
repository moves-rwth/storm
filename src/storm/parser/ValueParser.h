#ifndef STORM_PARSER_VALUEPARSER_H_
#define STORM_PARSER_VALUEPARSER_H_

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/parser/ExpressionParser.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

namespace storm {
    namespace parser {
        /*!
         * Parser for values according to their ValueType.
         */
        template<typename ValueType>
        class ValueParser {
        public:

            /*!
             * Constructor.
             */
            ValueParser() : manager(new storm::expressions::ExpressionManager()), parser(*manager), evaluator(*manager) {
            }

            /*!
             * Parse ValueType from string.
             *
             * @param value String containing the value.
             *
             * @return ValueType
             */
            ValueType parseValue(std::string const& value) const;

            /*!
             * Add declaration of parameter.
             *
             * @param parameter New parameter.
             */
            void addParameter(std::string const& parameter);

        private:

            std::shared_ptr<storm::expressions::ExpressionManager> manager;

            storm::parser::ExpressionParser parser;

            storm::expressions::ExpressionEvaluator<ValueType> evaluator;

            std::unordered_map<std::string, storm::expressions::Expression> identifierMapping;
        };

    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_VALUEPARSER_H_ */
