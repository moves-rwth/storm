#ifndef STORM_PARSER_AUTOPARSER_H_
#define STORM_PARSER_AUTOPARSER_H_

#include "src/parser/Parser.h"
#include "src/models/AbstractModel.h"

#include <memory>
#include <iostream>
#include <utility>

namespace storm {
namespace parser {

/*!
 *	@brief Checks the given files and parses the model within these files.
 *
 *	This parser analyzes the filename, an optional format hint (in the first
 *	line of the transition file) and the transitions within the file.
 *
 *	If all three (or two, if the hint is not given) are consistent, it will
 *	call the appropriate parser.
 *	If two guesses are the same but the third one contradicts, it will issue
 *	a warning to the user and call the (hopefully) appropriate parser.
 *	If all guesses differ, but a format hint is given, it will issue a
 *	warning to the user and use the format hint to determine the correct
 *	parser.
 *	Otherwise, it will issue an error.
 *
 *	When the files are parsed successfully, the parsed ModelType and Model
 *	can be obtained via getType() and getModel<ModelClass>().
 */
class AutoParser : Parser {
	public:
		AutoParser(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "");
		
		/*!
		 *	@brief 	Returns the type of model that was parsed.
		 */
		storm::models::ModelType getType() {
			return this->model->getType();
		}
		
		/*!
		 *	@brief	Returns the model with the given type.
		 */
		template <typename Model>
		std::shared_ptr<Model> getModel() {
			return this->model->as<Model>();
		}
		
	private:
		
		storm::models::ModelType analyzeFilename(const std::string& filename);
		std::pair<storm::models::ModelType, storm::models::ModelType> analyzeContent(const std::string& filename);
		
		/*!
		 *	@brief Pointer to a parser that has parsed the given transition system.
		 */
		std::shared_ptr<storm::models::AbstractModel> model;
};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_AUTOPARSER_H_ */
