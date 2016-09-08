#include "src/storage/pgcl/Statement.h"
#include "Block.h"

namespace storm {
    namespace pgcl {
        const bool Statement::operator==(const Statement& other) const {
            return (other.locationNumber == this->locationNumber);
        }

        void Statement::setLineNumber(std::size_t lineNumber) {
            this->lineNumber = lineNumber;
        }

        std::size_t Statement::getLineNumber() {
            return this->lineNumber;
        }

        std::size_t Statement::getLocationNumber() {
            return this->locationNumber;
        }

        void Statement::setLocationNumber(std::size_t locationNumber) {
            this->locationNumber = locationNumber;
        }

        bool Statement::isLast() {
            return this->last;
        }

        void Statement::setLast(bool isLast) {
            this->last = isLast;
        }

        bool Statement::isNondet() {
            return false;
        }

        void Statement::setParentBlock(PgclBlock* b) {
                this->parentBlock = b;
        }

        PgclBlock* Statement::getParentBlock() {
            return this->parentBlock;
        }


        std::size_t Statement::getNumberOfOutgoingTransitions() {
            return 1;
        }
    }
}