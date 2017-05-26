#ifndef STORM_PARSER_DIRECTENCODINGPARSER_H_
#define STORM_PARSER_DIRECTENCODINGPARSER_H_

#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/parser/ExpressionParser.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/sparse/ModelComponents.h"

namespace storm {
    namespace parser {

        /*!
         * Parser for values according to their ValueType.
         */
        template<typename ValueType>
        class ValueParser {
        public:

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

        /*!
         *	Parser for models in the DRN format with explicit encoding.
         */
        template<typename ValueType, typename RewardModelType = models::sparse::StandardRewardModel<ValueType>>
        class DirectEncodingParser {
        public:

            /*!
             * Load a model in DRN format from a file and create the model.
             *
             * @param file The DRN file to be parsed.
             *
             * @return A sparse model
             */
            static std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> parseModel(std::string const& file);

        private:

            /*!
             * Parse states and return transition matrix.
             *
             * @param file      Input file stream.
             * @param type      Model type.
             * @param stateSize No. of states
             *
             * @return Transition matrix.
             */
            static std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> parseStates(std::istream& file, storm::models::ModelType type, size_t stateSize, ValueParser<ValueType> const& valueParser);
        };

    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_DIRECTENCODINGPARSER_H_ */
