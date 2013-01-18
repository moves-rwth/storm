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
 *	This parser analyzes the format hint in the first line of the transition
 *	file. If this is a valid format, it will use the parser for this format,
 *	otherwise it will throw an exception.
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
			if (this->model) return this->model->getType();
			else return storm::models::Unknown;
		}
		
		/*!
		 *	@brief	Returns the model with the given type.
		 */
		template <typename Model>
		std::shared_ptr<Model> getModel() {
			return this->model->as<Model>();
		}
		
	private:
		
		/*!
		 *	@brief	Open file and read file format hint.
		 */
		storm::models::ModelType analyzeHint(const std::string& filename);
		
		/*!
		 *	@brief	Pointer to a parser that has parsed the given transition system.
		 */
		std::shared_ptr<storm::models::AbstractModel> model;
};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_AUTOPARSER_H_ */
