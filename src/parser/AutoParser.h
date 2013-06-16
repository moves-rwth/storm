#ifndef STORM_PARSER_AUTOPARSER_H_
#define STORM_PARSER_AUTOPARSER_H_

#include "src/parser/Parser.h"
#include "src/models/AbstractModel.h"

#include "src/exceptions/WrongFormatException.h"
#include "src/models/AbstractModel.h"
#include "src/parser/DeterministicModelParser.h"
#include "src/parser/NondeterministicModelParser.h"

#include <memory>
#include <iostream>
#include <utility>
#include <string>
#include <cctype>

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
template<class T>
class AutoParser {
	public:
		AutoParser(std::string const & transitionSystemFile, std::string const & labelingFile,
				std::string const & stateRewardFile = "", std::string const & transitionRewardFile = "") : model(nullptr) {
			storm::models::ModelType type = this->analyzeHint(transitionSystemFile);

			if (type == storm::models::Unknown) {
				LOG4CPLUS_ERROR(logger, "Could not determine file type of " << transitionSystemFile << ".");
				LOG4CPLUS_ERROR(logger, "The first line of the file should contain a format hint. Please fix your file and try again.");
				throw storm::exceptions::WrongFormatException() << "Could not determine type of file " << transitionSystemFile;
			} else {
				LOG4CPLUS_INFO(logger, "Model type seems to be " << type);
			}

			// Do actual parsing
			switch (type) {
				case storm::models::DTMC: {
					this->model.reset(new storm::models::Dtmc<double>(std::move(DeterministicModelParserAsDtmc(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile))));
					break;
				}
				case storm::models::CTMC: {
					this->model.reset(new storm::models::Ctmc<double>(std::move(DeterministicModelParserAsCtmc(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile))));
					break;
				}
				case storm::models::MDP: {
					this->model.reset(new storm::models::Mdp<double>(std::move(NondeterministicModelParserAsMdp(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile))));
					break;
				}
				case storm::models::CTMDP: {
					this->model.reset(new storm::models::Ctmdp<double>(std::move(NondeterministicModelParserAsCtmdp(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile))));
					break;
				}
				default: ;  // Unknown
			}


			if (!this->model) {
				LOG4CPLUS_WARN(logger, "Unknown/Unhandled Model Type. Model is still null.");
			}
		}
		
		/*!
		 *	@brief 	Returns the type of model that was parsed.
		 */
		storm::models::ModelType getType() {
			if (this->model) {
				return this->model->getType();
			} else {
				return storm::models::Unknown;
			}
		}
		
		/*!
		 *	@brief	Returns the model with the given type.
		 */
		template <typename Model>
		std::shared_ptr<Model> getModel() {
			return this->model->template as<Model>();
		}

	private:
		
		/*!
		 *	@brief	Open file and read file format hint.
		 */
		storm::models::ModelType analyzeHint(const std::string& filename) {
			storm::models::ModelType hintType = storm::models::Unknown;
			
			// Parse the File and check for the Line Endings
			storm::parser::SupportedLineEndingsEnum lineEndings = storm::parser::findUsedLineEndings(filename);
			
			// Open file
			MappedFile file(filename.c_str());
			char* buf = file.data;

			// parse hint
			char hint[128];
			// %20s => The Input Hint can be AT MOST 120 chars long			
			storm::parser::scanForModelHint(hint, sizeof(hint), buf, lineEndings);

			for (char* c = hint; *c != '\0'; c++) *c = toupper(*c);

			// check hint
			if (strncmp(hint, "DTMC", sizeof(hint)) == 0) hintType = storm::models::DTMC;
			else if (strncmp(hint, "CTMC", sizeof(hint)) == 0) hintType = storm::models::CTMC;
			else if (strncmp(hint, "MDP", sizeof(hint)) == 0) hintType = storm::models::MDP;
			else if (strncmp(hint, "CTMDP", sizeof(hint)) == 0) hintType = storm::models::CTMDP;

			return hintType;
		}
		
		/*!
		 *	@brief	Pointer to a parser that has parsed the given transition system.
		 */
		std::shared_ptr<storm::models::AbstractModel<T>> model;
};

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_AUTOPARSER_H_ */
