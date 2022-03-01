#pragma once

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
    virtual ~AbstractStatementVisitor() = default;

    // Those functions need to be implemented for every possible
    // statement instantiation.
    virtual void visit(class AssignmentStatement const&) = 0;
    virtual void visit(class ObserveStatement const&) = 0;
    virtual void visit(class IfStatement const&) = 0;
    virtual void visit(class LoopStatement const&) = 0;
    virtual void visit(class NondeterministicBranch const&) = 0;
    virtual void visit(class ProbabilisticBranch const&) = 0;
};
}  // namespace pgcl
}  // namespace storm
