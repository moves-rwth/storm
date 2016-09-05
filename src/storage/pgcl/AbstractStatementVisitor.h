//
// Created by Lukas Westhofen on 22.04.15.
//

#ifndef STORM_ABSTRACTSTATEVISITOR_H
#define STORM_ABSTRACTSTATEVISITOR_H

namespace storm {
    namespace pgcl {
        /**
         * This class implements the visitor part of the visitor pattern. Every
         * statement accepts such a visitor which then again calls the
         * corresponding visit method of the visitor object. In such a way
         * double dynamic dispatching can be realized. Every structure or
         * function that requires to differentiate between concrete statement
         * instantiations (such as if statements, assignments statements, ...)
         * should be handled by a concrete visitor implementation.
         */
        class AbstractStatementVisitor {
        public:
            // Those functions need to be implemented for every possible
            // statement instantiation.
            virtual void visit(class AssignmentStatement&) = 0;
            virtual void visit(class ObserveStatement&) = 0;
            virtual void visit(class IfStatement&) = 0;
            virtual void visit(class LoopStatement&) = 0;
            virtual void visit(class NondeterministicBranch&) = 0;
            virtual void visit(class ProbabilisticBranch&) = 0;
        };
    }
}

#endif //STORM_ABSTRACTSTATEVISITOR_H
