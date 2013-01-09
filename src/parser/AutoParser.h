#ifndef STORM_PARSER_AUTOPARSER_H_
#define STORM_PARSER_AUTOPARSER_H_

#include "src/models/AtomicPropositionsLabeling.h"
#include "boost/integer/integer_mask.hpp"

#include "src/parser/Parser.h"

#include <memory>
#include <iostream>
#include <utility>

namespace storm {
namespace parser {

/*!
 *	@brief	Enumeration of all supported types of models.
 */
enum ModelType {
	Unknown, DTMC, NDTMC
};

std::ostream& operator<<(std::ostream& os, const ModelType type)
{
	switch (type) {
		case Unknown: os << "Unknown"; break;
		case DTMC: os << "DTMC"; break;
		case NDTMC: os << "NDTMC"; break;
		default: os << "Invalid ModelType";
	}
	return os;
}

/*!
 *	@brief Checks the given file and tries to call the correct parser.
 *
 *	This parser analyzes the filename, an optional format hint (in the first
 *	line of the file) and the transitions within the file.
 *
 *	If all three (or two, if the hint is not given) are consistent, it will
 *	call the appropriate parser.
 *	If two guesses are the same but the third one contradicts, it will issue
 *	a warning to the user and call the (hopefully) appropriate parser.
 *	If all guesses differ, but a format hint is given, it will issue a
 *	warning to the user and use the format hint to determine the correct
 *	parser.
 *	Otherwise, it will issue an error.
 */
class AutoParser : Parser {
	public:
		AutoParser(const std::string& filename);
		
		/*!
		 *	@brief 	Returns the type of transition system that was detected.
		 */
		ModelType getModelType() {
			return this->type;
		}
		
		template <typename T>
		T* getParser() {
			return dynamic_cast<T*>( this->parser );
		}
		
		~AutoParser() {
			delete this->parser;
		}
	private:
		
		ModelType analyzeFilename(const std::string& filename);
		std::pair<ModelType,ModelType> analyzeContent(const std::string& filename);
		
		/*!
		 *	@brief Type of the transition system.
		 */
		ModelType type;
		
		/*!
		 *	@brief Pointer to a parser that has parsed the given transition system.
		 */
		Parser* parser;
};

} // namespace parser
} // namespace storm

#endif /* STORM_PARSER_AUTOPARSER_H_ */
