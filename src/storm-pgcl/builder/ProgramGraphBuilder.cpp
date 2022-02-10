#include "ProgramGraphBuilder.h"
#include "storm-pgcl/storage/pgcl/AssignmentStatement.h"
#include "storm-pgcl/storage/pgcl/IfStatement.h"
#include "storm-pgcl/storage/pgcl/LoopStatement.h"
#include "storm-pgcl/storage/pgcl/NondeterministicBranch.h"
#include "storm-pgcl/storage/pgcl/ObserveStatement.h"
#include "storm-pgcl/storage/pgcl/ProbabilisticBranch.h"

namespace storm {
namespace builder {
void ProgramGraphBuilderVisitor::visit(storm::pgcl::AssignmentStatement const& s) {
    if (s.isDeterministic()) {
        builder.currentLoc()->addProgramEdgeToAllGroups(builder.addAction(s.getVariable(), boost::get<storm::expressions::Expression>(s.getExpression())),
                                                        builder.nextLocId());
    } else {
        builder.currentLoc()->addProgramEdgeToAllGroups(builder.addAction(s.getVariable(), boost::get<storm::pgcl::UniformExpression>(s.getExpression())),
                                                        builder.nextLocId());
    }
}
void ProgramGraphBuilderVisitor::visit(storm::pgcl::ObserveStatement const& s) {
    builder.currentLoc()->addProgramEdgeToAllGroups(builder.noAction(), s.getCondition().getBooleanExpression(), builder.nextLocId());
}
void ProgramGraphBuilderVisitor::visit(storm::pgcl::IfStatement const& s) {
    storm::expressions::Expression elseCondition;
    storm::ppg::ProgramLocation* beforeStatementLocation = builder.currentLoc();
    builder.storeNextLocation(builder.nextLoc());
    storm::ppg::ProgramLocation* ifbodyStart = builder.newLocation();
    builder.buildBlock(*s.getIfBody());
    storm::ppg::ProgramLocation* elsebodyStart;
    if (s.hasElse()) {
        builder.storeNextLocation(builder.nextLoc());
        elsebodyStart = builder.newLocation();
        builder.buildBlock(*s.getElseBody());
    }
    beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), s.getCondition().getBooleanExpression(), ifbodyStart->id());
    elseCondition = !s.getCondition().getBooleanExpression();
    if (s.hasElse()) {
        beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), elseCondition, elsebodyStart->id());
    } else {
        beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), elseCondition, builder.nextLocId());
    }
}
void ProgramGraphBuilderVisitor::visit(storm::pgcl::LoopStatement const& s) {
    storm::ppg::ProgramLocation* beforeStatementLocation = builder.currentLoc();
    builder.storeNextLocation(beforeStatementLocation);
    storm::ppg::ProgramLocation* bodyStart = builder.newLocation();
    builder.buildBlock(*s.getBody());
    beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), s.getCondition().getBooleanExpression(), bodyStart->id());
    beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), !s.getCondition().getBooleanExpression(), builder.nextLocId());
}

void ProgramGraphBuilderVisitor::visit(storm::pgcl::NondeterministicBranch const& s) {
    storm::ppg::ProgramLocation* beforeStatementLocation = builder.currentLoc();
    builder.storeNextLocation(builder.nextLoc());
    storm::ppg::ProgramLocation* bodyStart = builder.newLocation();
    builder.buildBlock(*s.getLeftBranch());
    beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), bodyStart->id());
    builder.storeNextLocation(builder.nextLoc());
    bodyStart = builder.newLocation();
    beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), bodyStart->id());
    builder.buildBlock(*s.getRightBranch());
}
void ProgramGraphBuilderVisitor::visit(storm::pgcl::ProbabilisticBranch const& s) {
    storm::ppg::ProgramLocation* beforeStatementLocation = builder.currentLoc();
    storm::ppg::ProgramLocation* afterStatementLocation = builder.nextLoc();
    builder.storeNextLocation(afterStatementLocation);
    storm::ppg::ProgramLocation* bodyStart = builder.newLocation();
    beforeStatementLocation->addProgramEdgeGroup(s.getProbability())->addEdge(builder.noAction(), bodyStart->id());
    builder.buildBlock(*s.getLeftBranch());
    builder.storeNextLocation(afterStatementLocation);
    bodyStart = builder.newLocation();
    beforeStatementLocation->addProgramEdgeGroup(1 - s.getProbability())->addEdge(builder.noAction(), bodyStart->id());
    builder.buildBlock(*s.getRightBranch());
}
}  // namespace builder
}  // namespace storm
