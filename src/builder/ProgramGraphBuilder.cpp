#include "ProgramGraphBuilder.h"
#include "src/storage/pgcl/ObserveStatement.h"
#include "src/storage/pgcl/LoopStatement.h"
#include "src/storage/pgcl/IfStatement.h"
#include "src/storage/pgcl/NondeterministicBranch.h"
#include "src/storage/pgcl/ProbabilisticBranch.h"


namespace storm {
    namespace builder {
        void ProgramGraphBuilderVisitor::visit(storm::pgcl::AssignmentStatement const& s) {
            builder.currentLoc()->addProgramEdgeToAllGroups(builder.getAction(),  builder.nextLocId());
            
        }
        void ProgramGraphBuilderVisitor::visit(storm::pgcl::ObserveStatement const& s) {
            builder.currentLoc()->addProgramEdgeToAllGroups(builder.noAction(), s.getCondition().getBooleanExpression(), builder.nextLocId());
        }
        void ProgramGraphBuilderVisitor::visit(storm::pgcl::IfStatement const& s) {
            storm::expressions::Expression elseCondition;
            storm::ppg::ProgramLocation* beforeStatementLocation = builder.currentLoc();
            builder.storeNextLocation(builder.nextLoc());
            storm::ppg::ProgramLocation* bodyStart = builder.newLocation();
            builder.buildBlock(*s.getIfBody());
            if(s.hasElse()) {
                builder.storeNextLocation(builder.nextLoc());
                bodyStart = builder.newLocation();
                builder.buildBlock(*s.getElseBody());
            }
            beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), s.getCondition().getBooleanExpression(), bodyStart->id());
            if(s.hasElse()) {
                elseCondition = !s.getCondition().getBooleanExpression();
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
            beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), builder.nextLocId());
            builder.storeNextLocation(builder.nextLoc());
            bodyStart = builder.newLocation();
            beforeStatementLocation->addProgramEdgeToAllGroups(builder.noAction(), builder.nextLocId());
            builder.buildBlock(*s.getRightBranch());

        }
        void ProgramGraphBuilderVisitor::visit(storm::pgcl::ProbabilisticBranch const& s) {
            storm::ppg::ProgramLocation* beforeStatementLocation = builder.currentLoc();
            builder.storeNextLocation(builder.nextLoc());
            storm::ppg::ProgramLocation* bodyStart = builder.newLocation();
            beforeStatementLocation->addProgramEdgeGroup(s.getProbability());
            builder.buildBlock(*s.getLeftBranch());
            builder.storeNextLocation(builder.nextLoc());
            bodyStart = builder.newLocation();
            beforeStatementLocation->addProgramEdgeGroup(1 - s.getProbability());
            builder.buildBlock(*s.getRightBranch());
        }
    }
}