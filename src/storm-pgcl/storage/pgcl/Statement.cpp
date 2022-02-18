#include "Statement.h"
#include "storm-pgcl/storage/pgcl/Block.h"

namespace storm {
namespace pgcl {
bool Statement::operator==(const Statement& other) const {
    return (other.locationNumber == this->locationNumber);
}

void Statement::setLineNumber(std::size_t lineNumber) {
    this->lineNumber = lineNumber;
}

std::size_t Statement::getLineNumber() const {
    return this->lineNumber;
}

std::size_t Statement::getLocationNumber() const {
    return this->locationNumber;
}

void Statement::setLocationNumber(std::size_t locationNumber) {
    this->locationNumber = locationNumber;
}

bool Statement::isLast() const {
    return this->last;
}

void Statement::setLast(bool isLast) {
    this->last = isLast;
}

bool Statement::isNondet() const {
    return false;
}

void Statement::setParentBlock(PgclBlock* b) {
    this->parentBlock = b;
}

PgclBlock* Statement::getParentBlock() {
    return this->parentBlock;
}

std::size_t Statement::getNumberOfOutgoingTransitions() const {
    return 1;
}
}  // namespace pgcl
}  // namespace storm
