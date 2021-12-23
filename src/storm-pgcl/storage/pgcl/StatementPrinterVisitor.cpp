#include "StatementPrinterVisitor.h"

#include "storm-pgcl/storage/pgcl/AssignmentStatement.h"
#include "storm-pgcl/storage/pgcl/IfStatement.h"
#include "storm-pgcl/storage/pgcl/LoopStatement.h"
#include "storm-pgcl/storage/pgcl/NondeterministicBranch.h"
#include "storm-pgcl/storage/pgcl/ObserveStatement.h"
#include "storm-pgcl/storage/pgcl/ProbabilisticBranch.h"

namespace storm {
namespace pgcl {
StatementPrinterVisitor::StatementPrinterVisitor(std::ostream& stream) : stream(stream) {}

void StatementPrinterVisitor::visit(storm::pgcl::AssignmentStatement const& statement) {
    this->stream << statement.getLocationNumber() << ": ";
    if (statement.getExpression().which() == 0) {
        storm::expressions::Expression const& expression = boost::get<storm::expressions::Expression>(statement.getExpression());
        this->stream << statement.getVariable().getType() << " " << statement.getVariable().getName() << " := " << expression << ";\n";
    } else {
        storm::pgcl::UniformExpression const& unif = boost::get<storm::pgcl::UniformExpression>(statement.getExpression());
        this->stream << statement.getVariable().getType() << " " << statement.getVariable().getName() << " := "
                     << "unif(" << unif.getBegin() << ", " << unif.getEnd() << ");\n";
    }
}

void StatementPrinterVisitor::visit(storm::pgcl::ObserveStatement const& statement) {
    this->stream << statement.getLocationNumber() << ": ";
    this->stream << "observe(" << statement.getCondition().getBooleanExpression() << ");\n";
}

void StatementPrinterVisitor::visit(storm::pgcl::IfStatement const& statement) {
    this->stream << statement.getLocationNumber() << ": ";
    this->stream << "if (" << statement.getCondition().getBooleanExpression() << ") {\n";
    int i = 1;
    for (iterator it = (*(statement.getIfBody())).begin(); it != (*(statement.getIfBody())).end(); ++it) {
        (*(*it)).accept(*this);
        i++;
    }
    this->stream << "}\n";
    if (statement.hasElse()) {
        this->stream << "else {\n";
        for (iterator it = (*(statement.getElseBody())).begin(); it != (*(statement.getElseBody())).end(); ++it) {
            (*(*it)).accept(*this);
            i++;
        }
        this->stream << "}\n";
    }
}

void StatementPrinterVisitor::visit(storm::pgcl::LoopStatement const& statement) {
    this->stream << statement.getLocationNumber() << ": ";
    this->stream << "while (" << statement.getCondition().getBooleanExpression() << ") {\n";
    int i = 1;
    for (iterator it = (*(statement.getBody())).begin(); it != (*(statement.getBody())).end(); ++it) {
        (*(*it)).accept(*this);
        i++;
    }
    this->stream << "}\n";
}

void StatementPrinterVisitor::visit(storm::pgcl::NondeterministicBranch const& statement) {
    this->stream << statement.getLocationNumber() << ": ";
    this->stream << "{\n";
    int i = 1;
    for (iterator it = (*(statement.getLeftBranch())).begin(); it != (*(statement.getLeftBranch())).end(); ++it) {
        (*(*it)).accept(*this);
        i++;
    }
    this->stream << "} [] {\n";
    for (iterator it = (*(statement.getRightBranch())).begin(); it != (*(statement.getRightBranch())).end(); ++it) {
        (*(*it)).accept(*this);
        i++;
    }
    this->stream << "}\n";
}

void StatementPrinterVisitor::visit(storm::pgcl::ProbabilisticBranch const& statement) {
    this->stream << statement.getLocationNumber() << ": ";
    this->stream << "{\n";
    int i = 1;
    for (iterator it = (*(statement.getLeftBranch())).begin(); it != (*(statement.getLeftBranch())).end(); ++it) {
        (*(*it)).accept(*this);
        i++;
    }
    this->stream << "} [" << statement.getProbability() << "] {\n";
    for (iterator it = (*(statement.getRightBranch())).begin(); it != (*(statement.getRightBranch())).end(); ++it) {
        (*(*it)).accept(*this);
        i++;
    }
    this->stream << "}\n";
}
}  // namespace pgcl
}  // namespace storm
