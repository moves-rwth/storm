#pragma once

#include <boost/variant/get.hpp>
#include <iostream>
#include "storm-pgcl/storage/pgcl/AbstractStatementVisitor.h"

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
    void visit(AssignmentStatement const&);
    void visit(ObserveStatement const&);
    void visit(IfStatement const&);
    void visit(LoopStatement const&);
    void visit(NondeterministicBranch const&);
    void visit(ProbabilisticBranch const&);

   private:
    std::ostream& stream;
};
}  // namespace pgcl
}  // namespace storm
