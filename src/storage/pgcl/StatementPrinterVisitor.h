//
// Created by Lukas Westhofen on 21.04.15.
//

#ifndef STORM_STATEMENTVISITOR_H
#define STORM_STATEMENTVISITOR_H

#include <iostream>
#include <boost/variant/get.hpp>
#include "src/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace pgcl {
        /**
         * This is a sample implementation of a concrete visitor which interface
         * is defined in AbstractStatementVisitor.h. It prints out various
         * details of the given statements when the visit method is called.
         */
        class StatementPrinterVisitor : public AbstractStatementVisitor {
        public:
            /**
             * Constructs a statement printer which prints its output to the
             * given stream.
             * @param stream The stream to print to.
             */
            StatementPrinterVisitor(std::ostream& stream);
            void visit(class AssignmentStatement&);
            void visit(class ObserveStatement&);
            void visit(class IfStatement&);
            void visit(class LoopStatement&);
            void visit(class NondeterministicBranch&);
            void visit(class ProbabilisticBranch&);
        private:
            std::ostream& stream;
        };
    }
}

#endif //STORM_STATEMENTVISITOR_H
