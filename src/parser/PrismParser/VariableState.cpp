#include "VariableState.h"

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


VariableState::VariableState(bool firstRun)
		: firstRun(firstRun), keywords(), nextBooleanVariableIndex(0), nextIntegerVariableIndex(0) {
}

uint_fast64_t VariableState::addBooleanVariable(const std::string& name) {
	if (firstRun) {
		std::shared_ptr<VariableExpression> varExpr = std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::bool_, this->nextBooleanVariableIndex, name));
		LOG4CPLUS_DEBUG(logger, "Adding boolean variable " << name << " with new id " << this->nextBooleanVariableIndex);
		this->booleanVariables_.add(name, varExpr);
		this->booleanVariableNames_.add(name, name);
		this->nextBooleanVariableIndex++;
		return varExpr->getVariableIndex();
	} else {
		std::shared_ptr<VariableExpression> res = this->booleanVariables_.at(name);
		if (res != nullptr) {
			return res->getVariableIndex();
		} else {
			LOG4CPLUS_ERROR(logger, "Boolean variable " << name << " was not created in first run.");
			return 0;
		}
	}
}

uint_fast64_t VariableState::addIntegerVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> lower, const std::shared_ptr<storm::ir::expressions::BaseExpression> upper) {
	if (firstRun) {
		std::shared_ptr<VariableExpression> varExpr = std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::int_, this->nextIntegerVariableIndex, name, lower, upper));
		LOG4CPLUS_DEBUG(logger, "Adding integer variable " << name << " with new id " << this->nextIntegerVariableIndex);
		this->integerVariables_.add(name, varExpr);
		this->integerVariableNames_.add(name, name);
		this->nextIntegerVariableIndex++;
		return varExpr->getVariableIndex();
	} else {
		std::shared_ptr<VariableExpression> res = this->integerVariables_.at(name);
		if (res != nullptr) {
			return res->getVariableIndex();
		} else {

			LOG4CPLUS_ERROR(logger, "Integer variable " << name << " was not created in first run.");
			return 0;
		}
	}
}

std::shared_ptr<VariableExpression> VariableState::getBooleanVariable(const std::string& name) {
	std::shared_ptr<VariableExpression>* res = this->booleanVariables_.find(name);
	if (res != nullptr) {
		return *res;
	} else {
		if (firstRun) {
			LOG4CPLUS_DEBUG(logger, "Getting boolean variable " << name << ", was not yet created.");
			return std::shared_ptr<VariableExpression>(new VariableExpression(BaseExpression::bool_, std::numeric_limits<uint_fast64_t>::max(), "bool", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
		} else {
			LOG4CPLUS_ERROR(logger, "Getting boolean variable " << name << ", but was not found. This variable does not exist.");
			return std::shared_ptr<VariableExpression>(nullptr);
		}
	}
}

std::shared_ptr<VariableExpression> VariableState::getIntegerVariable(const std::string& name) {
	std::shared_ptr<VariableExpression>* res = this->integerVariables_.find(name);
	if (res != nullptr) {
		return *res;
	} else {
		if (firstRun) {
			LOG4CPLUS_DEBUG(logger, "Getting integer variable " << name << ", was not yet created.");
			return std::shared_ptr<VariableExpression>(new VariableExpression(BaseExpression::int_, std::numeric_limits<uint_fast64_t>::max(), "int", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
		} else {
			LOG4CPLUS_ERROR(logger, "Getting integer variable " << name << ", but was not found. This variable does not exist.");
			return std::shared_ptr<VariableExpression>(nullptr);
		}
	}
}
std::shared_ptr<VariableExpression> VariableState::getVariable(const std::string& name) {
	std::shared_ptr<VariableExpression>* res = this->integerVariables_.find(name);
	if (res != nullptr) {
		return *res;
	} else {
		res = this->booleanVariables_.find(name);
		if (res != nullptr) {
			return *res;
		} else {
			return std::shared_ptr<VariableExpression>(nullptr);
		}
	}
}

void VariableState::performRenaming(const std::map<std::string, std::string>& renaming) {
	for (auto it: renaming) {
		std::shared_ptr<VariableExpression>* original = this->integerVariables_.find(it.first);
		if (original != nullptr) {
			std::shared_ptr<VariableExpression>* next = this->integerVariables_.find(it.second);
			if (next == nullptr) {
				this->addIntegerVariable(it.second, (*original)->getLowerBound(), (*original)->getUpperBound());
			}
		}
		original = this->booleanVariables_.find(it.first);
		if (original != nullptr) {
			if (this->booleanVariables_.find(it.second) == nullptr) {
				this->addBooleanVariable(it.second);
			}
		}
		std::string* oldCmdName = this->commandNames_.find(it.first);
		if (oldCmdName != nullptr) {
			LOG4CPLUS_DEBUG(logger, "Adding new command name " << it.second << " due to module renaming.");
			this->commandNames_.at(it.second) = it.second;
		}
	}
}

void VariableState::startModule() {
	this->localBooleanVariables_.clear();
	this->localIntegerVariables_.clear();
}

bool VariableState::isFreeIdentifier(std::string& s) const {
	if (this->integerVariableNames_.find(s) != nullptr) return false;
	if (this->allConstantNames_.find(s) != nullptr) return false;
	if (this->labelNames_.find(s) != nullptr) return false;
	if (this->moduleNames_.find(s) != nullptr) return false;
	if (this->keywords.find(s) != nullptr) return false;
	return true;
}
bool VariableState::isIdentifier(std::string& s) const {
	if (this->allConstantNames_.find(s) != nullptr) return false;
	if (this->keywords.find(s) != nullptr) return false;
	return true;
}

void VariableState::prepareForSecondRun() {
	integerConstants_.clear();
	booleanConstants_.clear();
	doubleConstants_.clear();
	allConstantNames_.clear();
	this->firstRun = false;
}

}
}
}
