/* 
 * File:   Statement.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:41
 */

#include "src/storage/pgcl/Statement.h"

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

        void Statement::setParentProgram(std::shared_ptr<storm::pgcl::PgclProgram> parentProgram) {
                this->parentProgram = parentProgram;
        }

        boost::optional<std::shared_ptr<storm::pgcl::PgclProgram> > Statement::getParentProgram() {
            return this->parentProgram;
        }

        void Statement::setParentStatement(std::shared_ptr<storm::pgcl::Statement> parentStatement) {
            this->parentStatement = parentStatement;
        }

        boost::optional<std::shared_ptr<storm::pgcl::Statement> > Statement::getParentStatement() {
            return this->parentStatement;
        }

        std::size_t Statement::getNumberOfOutgoingTransitions() {
            return 1;
        }
    }
}