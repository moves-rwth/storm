#ifndef STORM_PARSER_DIRECTENCODINGPARSER_H_
#define STORM_PARSER_DIRECTENCODINGPARSER_H_

#include "storm-parsers/parser/ValueParser.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/sparse/ModelComponents.h"

namespace storm {
namespace parser {

struct DirectEncodingParserOptions {
    bool buildChoiceLabeling = false;
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
    static std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> parseModel(
        std::string const& fil, DirectEncodingParserOptions const& options = DirectEncodingParserOptions());

   private:
    /*!
     * Parse states and return transition matrix.
     *
     * @param file Input file stream.
     * @param type Model type.
     * @param stateSize No. of states
     * @param placeholders Placeholders for values.
     * @param valueParser Value parser.
     * @param rewardModelNames Names of reward models.
     *
     * @return Transition matrix.
     */
    static std::shared_ptr<storm::storage::sparse::ModelComponents<ValueType, RewardModelType>> parseStates(
        std::istream& file, storm::models::ModelType type, size_t stateSize, size_t nrChoices, std::unordered_map<std::string, ValueType> const& placeholders,
        ValueParser<ValueType> const& valueParser, std::vector<std::string> const& rewardModelNames, DirectEncodingParserOptions const& options);

    /*!
     * Parse value from string while using placeholders.
     * @param valueStr String.
     * @param placeholders Placeholders.
     * @param valueParser Value parser.
     * @return
     */
    static ValueType parseValue(std::string const& valueStr, std::unordered_map<std::string, ValueType> const& placeholders,
                                ValueParser<ValueType> const& valueParser);
};

}  // namespace parser
}  // namespace storm

#endif /* STORM_PARSER_DIRECTENCODINGPARSER_H_ */
