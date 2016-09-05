/* 
 * File:   PgclProgram.h
 * Author: Lukas Westhofen
 *
 * Created on 11. April 2015, 17:39
 */

#ifndef PGCLPROGRAM_H
#define	PGCLPROGRAM_H

#include <vector>
#include "src/storage/pgcl/Statement.h"
#include "src/storage/pgcl/StatementPrinterVisitor.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace pgcl {
        
        typedef std::shared_ptr<storm::pgcl::Statement> element;
        typedef std::vector<element> vector;
        typedef std::vector<element>::iterator iterator;
        typedef std::vector<element>::const_iterator const_iterator;
        typedef std::vector<element>::size_type size_type;

        /**
         * This class represents a complete and functional PGCL program. It
         * contains an expression manager which keeps track of the current
         * identifiers and variable valuations. Other than that, it basically
         * wraps a std::vector of program statements and is intended to be used
         * as such.
         */
        class PgclProgram {
        public:
            PgclProgram() = default;
            /**
             * Constructs the PGCL program with the given sequence of statements
             * (they may contain other PGCL programs). The expression manager
             * handles the expressions and variables of the PGCL program.
             * @param statements The sequence of statements representing the
             * program.
             * @param locationToStatements A vector containing the statement
             * with location number i at the i-th position.
             * @param parameters The list of parameters of the program.
             * @param expressions The manager responsible for the expressions
             * and variables of the program.
             * @param hasLoop Whether the program contains a loop
             * @param hasNondet Whether the program contains a nondeterministic
             * statement.
             * @param hasParam Whether the program is parameterized.
             */
            PgclProgram(vector const& statements, vector const& locationToStatement, std::vector<storm::expressions::Variable> const& parameters, std::shared_ptr<storm::expressions::ExpressionManager> expressions, bool hasLoop, bool hasNondet, bool hasObserve, bool isTop);
            /**
             * Does the same as the beforementioned constructor, but sets the
             * location to statement vector to the empty vector. This
             * constructor should be used for sub-programs, for which the
             * location to statement relation doesn't make much sense.
             * @param statements The sequence of statements representing the
             * program.
             * @param expressions The manager responsible for the expressions
             * and variables of the program.
             * @param hasLoop Whether the program contains a loop
             * @param hasNondet Whether the program contains a nondeterministic
             * statement.
             * @param hasParam Whether the program is parameterized.
             */
            PgclProgram(vector const& statements, std::shared_ptr<storm::expressions::ExpressionManager> expressions, bool hasLoop, bool hasNondet, bool hasObserve, bool isTop);
            PgclProgram(const PgclProgram & orig) = default;
            PgclProgram & operator=(PgclProgram const& other) = default;
            iterator begin();
            iterator end();
            element front();
            element back();
            unsigned long size();
            element at(size_type n);
            iterator insert(iterator position, const element& statement);
            iterator find(element& statement);
            void clear();
            bool empty();
            /**
             * Returns a vector that has the statement with location number i at
             * its i-th position. This allows for O(1)-access of statements if
             * only the location number is given.
             */
            vector getLocationToStatementVector();
            /**
             * Returns the list of parameters of the PGCL program.
             */
            std::vector<storm::expressions::Variable> getParameters();
            /**
             * Returns the expression manager of the PGCL program, which is
             * responsible for managing all expressions and variables of the
             * the program and all its subprograms.
             * @return The expression manager of the program.
             */
            std::shared_ptr<storm::expressions::ExpressionManager> getExpressionManager();
            /**
             * Returns true if the program contains a loop statement.
             * @return True if the program has a loop.
             */
            bool hasLoop() const;
            /**
             * Returns true if the program contains a nondeterministic
             * statement.
             * @return True if the program has a nondeterministic statement.
             */
            bool hasNondet() const;
            /**
             * Returns true if the program contains an observe statement.
             * @return True if the program has an observe statement.
             */
            bool hasObserve() const;
            /**
             * Returns true if the program is parameterized.
             * @return True if the program has at least one parameter.
             */
            bool hasParameters() const;
            /**
             * Returns whether the program is no subprogram.
             * @return True if the program is no subprogram of another
             * program.
             */
            bool isTop() const;
        private:
            /**
             * We are basically wrapping a std::vector which represents the
             * ordered single statements of the program.
             */
            vector sequenceOfStatements;
            /**
             * Contains the statement with location i at its i-th position.
             * Imagine this as the "unrolled" sequence of statements, so the
             * recursion is resolved here.
             */
            vector locationToStatement;
            /**
             * Stores the parameters a.k.a. free variables of the PGCL program.
             */
            std::vector<storm::expressions::Variable> parameters;
            /**
             * Handles the expressions and variables for the whole program.
             * The expressions of every subprogram are also handled by this
             * manager. We are using a shared pointer since all subprograms
             * are referring to that expression manager, too.
             */
            std::shared_ptr<storm::expressions::ExpressionManager> expressions;
            /**
             * Boolean variables to save some properties of the PGCL program.
             * They are later on used by the model builder to possibly
             * construct simpler models (e.g. if no loops, params and nondets
             * are used, a DTMC suffices).
             * The values are set to true if the PGCL parser hits a loop resp.
             * nondet resp. observe resp. parameter statement.
             */
            bool loop    = false;
            bool nondet  = false;
            bool observe = false;
            bool top     = false;
        };
        /**
         * Prints every statement of the program along with their location
         * numbers.
         * @param stream The stream to print the program to.
         * @param program The program to print.
         */
        std::ostream& operator<<(std::ostream& stream, PgclProgram& program);
    }
}

#endif	/* PGCLPROGRAM_H */

