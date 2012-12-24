#include "src/parser/AutoTransitionParser.h"

#include "src/exceptions/WrongFileFormatException.h"

#include "DeterministicSparseTransitionParser.h"
#include "NonDeterministicSparseTransitionParser.h"

namespace mrmc {
namespace parser {

AutoTransitionParser::AutoTransitionParser(const std::string& filename)
	: type(Unknown) {

	TransitionType name = this->analyzeFilename(filename);
	std::pair<TransitionType,TransitionType> content = this->analyzeContent(filename);
	TransitionType hint = content.first, transitions = content.second;
	
	if (hint == Unknown) {
		if (name == transitions) this->type = name;
		else {
			LOG4CPLUS_ERROR(logger, "Could not determine file type of " << filename << ". Filename suggests " << name << " but transitions look like " << transitions);
			LOG4CPLUS_ERROR(logger, "Please fix your file and try again.");
			throw mrmc::exceptions::WrongFileFormatException() << "Could not determine type of file " << filename;
		}
	} else {
		if ((hint == name) && (name == transitions)) this->type = name;
		else if (hint == name) {
			LOG4CPLUS_WARN(logger, "Transition format in file " << filename << " of type " << name << " look like " << transitions << " transitions.");
			LOG4CPLUS_WARN(logger, "We will use the parser for " << name << " and hope for the best!");
			this->type = name;
		}
		else if (hint == transitions) {
			LOG4CPLUS_WARN(logger, "File extension of " << filename << " suggests type " << name << " but the content seems to be " << hint);
			LOG4CPLUS_WARN(logger, "We will use the parser for " << hint << " and hope for the best!");
			this->type = hint;
		}
		else if (name == transitions) {
			LOG4CPLUS_WARN(logger, "File " << filename << " contains a hint that it is " << hint << " but filename and transition pattern suggests " << name);
			LOG4CPLUS_WARN(logger, "We will use the parser for " << name << " and hope for the best!");
			this->type = name;
		}
		else {
			LOG4CPLUS_WARN(logger, "File " << filename << " contains a hint that it is " << hint << " but filename suggests " << name << " and transition pattern suggests " << transitions);
			LOG4CPLUS_WARN(logger, "We will stick to the hint, use the parser for " << hint << " and hope for the best!");
			this->type = hint;
		}
	}
	
	// Do actual parsing
	switch (this->type) {
		case DTMC:
			this->parser = new DeterministicSparseTransitionParser(filename);
			break;
		case NDTMC:
			this->parser = new NonDeterministicSparseTransitionParser(filename);
			break;
		default: ; // Unknown
	}
}

TransitionType AutoTransitionParser::analyzeFilename(const std::string& filename) {
	TransitionType type = Unknown;
	
	return type;
}

std::pair<TransitionType,TransitionType> AutoTransitionParser::analyzeContent(const std::string& filename) {
	
	TransitionType hintType = Unknown, transType = Unknown;
	// Open file
	MappedFile file(filename.c_str());
	//char* buf = file.data;
	
	
	return std::pair<TransitionType,TransitionType>(hintType, transType);
}

} //namespace parser

} //namespace mrmc
