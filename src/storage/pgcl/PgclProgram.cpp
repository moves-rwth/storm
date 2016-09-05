/* 
 * File:   PgclProgram.cpp
 * Author: Lukas Westhofen
 * 
 * Created on 11. April 2015, 17:39
 */

#include "PgclProgram.h"
#include "StatementPrinterVisitor.h"
#include <typeinfo>

namespace storm {
    namespace pgcl {
        PgclProgram::PgclProgram(vector const& statements, vector const& locationToStatement, std::vector<storm::expressions::Variable> const& parameters, std::shared_ptr<storm::expressions::ExpressionManager> expressions, bool hasLoop, bool hasNondet, bool hasObserve, bool isTop) :
            sequenceOfStatements(statements),
            locationToStatement(locationToStatement),
            parameters(parameters),
            expressions(expressions),
            loop(hasLoop),
            nondet(hasNondet),
            observe(hasObserve),
            top(isTop) {
        }

        PgclProgram::PgclProgram(vector const &statements, std::shared_ptr<storm::expressions::ExpressionManager> expressions, bool hasLoop, bool hasNondet, bool hasObserve, bool isTop) :
            sequenceOfStatements(statements),
            expressions(expressions),
            loop(hasLoop),
            nondet(hasNondet),
            observe(hasObserve),
            top(isTop) {
        }

        iterator PgclProgram::begin() {
            return this->sequenceOfStatements.begin();
        }

        iterator PgclProgram::end() {
            return this->sequenceOfStatements.end();
        }

        bool PgclProgram::empty() {
            return this->sequenceOfStatements.empty();
        }
        
        element PgclProgram::front() {
            return this->sequenceOfStatements.front();
        }
        
        element PgclProgram::back() {
            return this->sequenceOfStatements.back();
        }

        unsigned long PgclProgram::size() {
            return this->sequenceOfStatements.size();
        }
        
        element PgclProgram::at(size_type n) {
            return this->sequenceOfStatements.at(n);
        }
        
        iterator PgclProgram::insert(iterator position, const element& statement) {
            return this->sequenceOfStatements.insert(position, statement);
        }
        
        void PgclProgram::clear() {
            this->sequenceOfStatements.clear();
        }

        std::shared_ptr<storm::expressions::ExpressionManager> PgclProgram::getExpressionManager() {
            return this->expressions;
        }

        std::vector<storm::expressions::Variable> PgclProgram::getParameters() {
            return this->parameters;
        }

        bool PgclProgram::hasParameters() const {
            return !(this->parameters.empty());
        }

        bool PgclProgram::hasObserve() const {
            return this->observe;
        }

        bool PgclProgram::hasNondet() const {
            return this->nondet;
        }

        bool PgclProgram::hasLoop() const {
            return this->loop;
        }

        vector PgclProgram::getLocationToStatementVector() {
            return this->locationToStatement;
        }

        iterator PgclProgram::find(element &statement) {
            return std::find(this->sequenceOfStatements.begin(), this->sequenceOfStatements.end(), statement);
        }

        bool PgclProgram::isTop() const {
            return this->top;
        }

        std::ostream& operator<<(std::ostream& stream, PgclProgram& program) {
            storm::pgcl::StatementPrinterVisitor printer(stream);
            for(iterator statement = program.begin(); statement != program.end(); statement++) {
                (*statement)->accept(printer);
            }
            return stream;
        }
    }
}