#include "src/parser/AutoParser.h"

#include "src/exceptions/WrongFileFormatException.h"

#include "src/parser/DtmcParser.h"
//#include "NonDeterministicSparseTransitionParser.h"

#include <string>

namespace storm {
namespace parser {

AutoParser::AutoParser(std::string const & transitionSystemFile, std::string const & labelingFile,
	std::string const & stateRewardFile, std::string const & transitionRewardFile)
	{

	storm::models::ModelType name = this->analyzeFilename(transitionSystemFile);
	std::pair<storm::models::ModelType,storm::models::ModelType> content = this->analyzeContent(transitionSystemFile);
	storm::models::ModelType hint = content.first, transitions = content.second;
	
	storm::models::ModelType type = storm::models::Unknown;
	
	if (hint == storm::models::Unknown) {
		if (name == transitions) type = name;
		else {
			LOG4CPLUS_ERROR(logger, "Could not determine file type of " << transitionSystemFile << ". Filename suggests " << name << " but transitions look like " << transitions);			LOG4CPLUS_ERROR(logger, "Please fix your file and try again.");
			throw storm::exceptions::WrongFileFormatException() << "Could not determine type of file " << transitionSystemFile;
		}
	} else {
		if ((hint == name) && (name == transitions)) type = name;
		else if (hint == name) {
			LOG4CPLUS_WARN(logger, "Transition format in file " << transitionSystemFile << " of type " << name << " look like " << transitions << " transitions.");
			LOG4CPLUS_WARN(logger, "We will use the parser for " << name << " and hope for the best!");
			type = name;
		}
		else if (hint == transitions) {
			LOG4CPLUS_WARN(logger, "File extension of " << transitionSystemFile << " suggests type " << name << " but the content seems to be " << hint);
			LOG4CPLUS_WARN(logger, "We will use the parser for " << hint << " and hope for the best!");
			type = hint;
		}
		else if (name == transitions) {
			LOG4CPLUS_WARN(logger, "File " << transitionSystemFile << " contains a hint that it is " << hint << " but filename and transition pattern suggests " << name);
			LOG4CPLUS_WARN(logger, "We will use the parser for " << name << " and hope for the best!");
			type = name;
		}
		else {
			LOG4CPLUS_WARN(logger, "File " << transitionSystemFile << " contains a hint that it is " << hint << " but filename suggests " << name << " and transition pattern suggests " << transitions);
			LOG4CPLUS_WARN(logger, "We will stick to the hint, use the parser for " << hint << " and hope for the best!");
			type = hint;
		}
	}
	
	// Do actual parsing
	switch (type) {
		case storm::models::DTMC: {
				DtmcParser* parser = new DtmcParser(transitionSystemFile, labelingFile, stateRewardFile, transitionRewardFile);
				this->model = parser->getDtmc();
				break;
			}
		case storm::models::CTMC:
			break;
		case storm::models::MDP:
			break;
		case storm::models::CTMDP:
			break;
		default: ; // Unknown
	}
}

storm::models::ModelType AutoParser::analyzeFilename(const std::string& filename) {
	storm::models::ModelType type = storm::models::Unknown;
	
	// find file extension
	std::string::size_type extpos = filename.rfind(".");
	if (extpos == std::string::npos) return storm::models::Unknown;
	else extpos++;
	
	// check file extension
	if (filename.substr(extpos) == "dtmc") type = storm::models::DTMC;
	else if (filename.substr(extpos) == "mdp") type = storm::models::MDP;
	
	return type;
}

std::pair<storm::models::ModelType,storm::models::ModelType> AutoParser::analyzeContent(const std::string& filename) {
	
	storm::models::ModelType hintType = storm::models::Unknown, transType = storm::models::Unknown;
	// Open file
	MappedFile file(filename.c_str());
	char* buf = file.data;
	
	// parse hint
	char hint[128];
	sscanf(buf, "%s\n", hint);
	
	// check hint
	if (strncmp(hint, "dtmc", sizeof(hint)) == 0) hintType = storm::models::DTMC;
	else if (strncmp(hint, "mdp", sizeof(hint)) == 0) hintType = storm::models::MDP;
	
	
	// check transition format
	// todo.
	
	return std::pair<storm::models::ModelType,storm::models::ModelType>(hintType, transType);
}

} //namespace parser

} //namespace storm
