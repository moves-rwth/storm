#ifndef STORM_PARSER_DIRECTENCODINGPARSER_H_
#define STORM_PARSER_DIRECTENCODINGPARSER_H_

#include "storm/models/sparse/Model.h"


namespace storm {
    namespace parser {

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

        };

    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_DIRECTENCODINGPARSER_H_ */
