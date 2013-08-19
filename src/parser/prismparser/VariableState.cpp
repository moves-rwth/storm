#include "VariableState.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
namespace parser {
namespace prism {

using namespace storm::ir;
using namespace storm::ir::expressions;

template<typename T>
struct SymbolDump {
	SymbolDump(std::ostream& out) : out(out) {}
	void operator() (std::basic_string<char> s, T elem) {
		this->out << "\t" << s << " -> " << elem << std::endl;
	}
private:
	std::ostream& out;
};
template<typename T>
std::ostream& operator<<(std::ostream& out, qi::symbols<char, T>& symbols) {
	out << "Dumping symbol table" << std::endl;
	SymbolDump<T> dump(out);
	symbols.for_each(dump);
	return out;
}
std::ostream& operator<<(std::ostream& out, VariableState::variableNamesStruct& symbols) {
	SymbolDump<std::string> dump(out);
	symbols.for_each(dump);
	return out;
}


VariableState::VariableState(bool firstRun)	: firstRun(firstRun), keywords(), nextLocalBooleanVariableIndex(0), nextLocalIntegerVariableIndex(0), nextGlobalBooleanVariableIndex(0), nextGlobalIntegerVariableIndex(0) {
    // Nothing to do here.
}

uint_fast64_t VariableState::getNextLocalBooleanVariableIndex() const {
    return this->nextLocalBooleanVariableIndex;
}
    
uint_fast64_t VariableState::getNextLocalIntegerVariableIndex() const {
    return this->nextLocalIntegerVariableIndex;
}
    
uint_fast64_t VariableState::getNextGlobalBooleanVariableIndex() const {
    return this->nextGlobalBooleanVariableIndex;
}
    
uint_fast64_t VariableState::getNextGlobalIntegerVariableIndex() const {
    return this->nextGlobalIntegerVariableIndex;
}
    
uint_fast64_t VariableState::addBooleanVariable(std::string const& name) {
	if (firstRun) {
		LOG4CPLUS_TRACE(logger, "Adding boolean variable " << name << " with new id " << this->nextGlobalBooleanVariableIndex << ".");
		this->booleanVariables_.add(name, std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::bool_, this->nextGlobalBooleanVariableIndex, name)));
		this->booleanVariableNames_.add(name, name);
		++this->nextGlobalBooleanVariableIndex;
        ++this->nextLocalBooleanVariableIndex;
		return this->nextGlobalBooleanVariableIndex - 1;
	} else {
		std::shared_ptr<VariableExpression> variableExpression = this->booleanVariables_.at(name);
		if (variableExpression != nullptr) {
			return variableExpression->getGlobalVariableIndex();
		} else {
			LOG4CPLUS_ERROR(logger, "Boolean variable " << name << " does not exist.");
            throw storm::exceptions::InvalidArgumentException() << "Boolean variable " << name << " does not exist.";
		}
	}
}

uint_fast64_t VariableState::addIntegerVariable(std::string const& name) {
	if (firstRun) {
		LOG4CPLUS_TRACE(logger, "Adding integer variable " << name << " with new id " << this->nextGlobalIntegerVariableIndex << ".");
		this->integerVariables_.add(name, std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::int_, this->nextGlobalIntegerVariableIndex, name)));
		this->integerVariableNames_.add(name, name);
		++this->nextGlobalIntegerVariableIndex;
        ++this->nextLocalIntegerVariableIndex;
		return this->nextGlobalIntegerVariableIndex - 1;
	} else {
		std::shared_ptr<VariableExpression> variableExpression = this->integerVariables_.at(name);
		if (variableExpression != nullptr) {
			return variableExpression->getGlobalVariableIndex();
		} else {
			LOG4CPLUS_ERROR(logger, "Integer variable " << name << " does not exist.");
            throw storm::exceptions::InvalidArgumentException() << "Integer variable " << name << " does not exist.";
		}
	}
}

std::shared_ptr<VariableExpression> VariableState::getBooleanVariableExpression(std::string const& name) const {
	std::shared_ptr<VariableExpression> const* variableExpression = this->booleanVariables_.find(name);
	if (variableExpression != nullptr) {
		return *variableExpression;
	} else {
		if (firstRun) {
			LOG4CPLUS_TRACE(logger, "Trying to retrieve boolean variable " << name << " that was not yet created; returning dummy instead.");
			return std::shared_ptr<VariableExpression>(new VariableExpression(BaseExpression::bool_, name));
		} else {
			LOG4CPLUS_ERROR(logger, "Boolean variable " << name << " does not exist.");
            throw storm::exceptions::InvalidArgumentException() << "Boolean variable " << name << " does not exist.";
		}
	}
}

std::shared_ptr<VariableExpression> VariableState::getIntegerVariableExpression(std::string const& name) const {
	std::shared_ptr<VariableExpression> const* variableExpression = this->integerVariables_.find(name);
	if (variableExpression != nullptr) {
		return *variableExpression;
	} else {
		if (firstRun) {
			LOG4CPLUS_TRACE(logger, "Trying to retrieve integer variable " << name << " that was not yet created; returning dummy instead.");
			return std::shared_ptr<VariableExpression>(new VariableExpression(BaseExpression::int_, name));
		} else {
			LOG4CPLUS_ERROR(logger, "Integer variable " << name << " does not exist.");
            throw storm::exceptions::InvalidArgumentException() << "Integer variable " << name << " does not exist.";
		}
	}
}
    
std::shared_ptr<VariableExpression> VariableState::getVariableExpression(std::string const& name) const {
	std::shared_ptr<VariableExpression> const* variableExpression = this->integerVariables_.find(name);
	if (variableExpression != nullptr) {
		return *variableExpression;
	}
    
    variableExpression = this->booleanVariables_.find(name);
	if (variableExpression != nullptr) {
		return *variableExpression;
	}
    LOG4CPLUS_ERROR(logger, "Variable " << name << " does not exist.");
    throw storm::exceptions::InvalidArgumentException() << "Variable " << name << " does not exist.";
}

void VariableState::clearLocalVariables() {
	this->localBooleanVariables_.clear();
	this->localIntegerVariables_.clear();
    this->nextLocalBooleanVariableIndex = 0;
    this->nextLocalIntegerVariableIndex = 0;
}

bool VariableState::isFreeIdentifier(std::string const& identifier) const {
	if (this->integerVariableNames_.find(identifier) != nullptr) return false;
	if (this->allConstantNames_.find(identifier) != nullptr) return false;
	if (this->labelNames_.find(identifier) != nullptr) return false;
	if (this->moduleNames_.find(identifier) != nullptr) return false;
	if (this->keywords.find(identifier) != nullptr) return false;
	return true;
}
    
bool VariableState::isIdentifier(std::string const& identifier) const {
	if (this->allConstantNames_.find(identifier) != nullptr) return false;
	if (this->keywords.find(identifier) != nullptr) return false;
	return true;
}

void VariableState::prepareForSecondRun() {
	integerConstants_.clear();
	booleanConstants_.clear();
	doubleConstants_.clear();
	allConstantNames_.clear();
	this->firstRun = false;
}

} // namespace prism
} // namespace parser
} // namespace storm
