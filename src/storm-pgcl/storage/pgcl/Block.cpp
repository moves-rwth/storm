#include "Block.h"
#include <typeinfo>
#include "StatementPrinterVisitor.h"

namespace storm {
namespace pgcl {

PgclBlock::PgclBlock(vector const& statements, std::shared_ptr<storm::expressions::ExpressionManager> expressions, bool hasLoop, bool hasNondet,
                     bool hasObserve)
    : sequenceOfStatements(statements), expressions(expressions), loop(hasLoop), nondet(hasNondet), observe(hasObserve) {}

iterator PgclBlock::begin() {
    return this->sequenceOfStatements.begin();
}

const_iterator PgclBlock::begin() const {
    return this->sequenceOfStatements.begin();
}

iterator PgclBlock::end() {
    return this->sequenceOfStatements.end();
}

const_iterator PgclBlock::end() const {
    return this->sequenceOfStatements.end();
}

bool PgclBlock::empty() {
    return this->sequenceOfStatements.empty();
}

element PgclBlock::front() {
    return this->sequenceOfStatements.front();
}

element PgclBlock::back() {
    return this->sequenceOfStatements.back();
}

unsigned long PgclBlock::size() {
    return this->sequenceOfStatements.size();
}

element PgclBlock::at(size_type n) {
    return this->sequenceOfStatements.at(n);
}

iterator PgclBlock::insert(iterator position, const element& statement) {
    return this->sequenceOfStatements.insert(position, statement);
}

void PgclBlock::clear() {
    this->sequenceOfStatements.clear();
}

std::shared_ptr<storm::expressions::ExpressionManager> const& PgclBlock::getExpressionManager() const {
    return this->expressions;
}

std::vector<storm::expressions::Variable> PgclBlock::getParameters() {
    return this->parameters;
}

bool PgclBlock::hasParameters() const {
    return !(this->parameters.empty());
}

bool PgclBlock::hasObserve() const {
    return this->observe;
}

bool PgclBlock::hasNondet() const {
    return this->nondet;
}

bool PgclBlock::hasLoop() const {
    return this->loop;
}

iterator PgclBlock::find(element& statement) {
    return std::find(this->sequenceOfStatements.begin(), this->sequenceOfStatements.end(), statement);
}

}  // namespace pgcl
}  // namespace storm
