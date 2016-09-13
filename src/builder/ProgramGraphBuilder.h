#pragma once

#include "src/storage/pgcl/PgclProgram.h"
#include "src/storage/ppg/ProgramGraph.h"
#include "src/storage/pgcl/AbstractStatementVisitor.h"

namespace storm {
    namespace builder {
        class ProgramGraphBuilder;
        
        
        class ProgramGraphBuilderVisitor:  public storm::pgcl::AbstractStatementVisitor{
        public:
            ProgramGraphBuilderVisitor(ProgramGraphBuilder& builder) : builder(builder) {
                
            }
            
            virtual void visit(storm::pgcl::AssignmentStatement const&);
            virtual void visit(storm::pgcl::ObserveStatement const&);
            virtual void visit(storm::pgcl::IfStatement const&);
            virtual void visit(storm::pgcl::LoopStatement const&);
            virtual void visit(storm::pgcl::NondeterministicBranch const&);
            virtual void visit(storm::pgcl::ProbabilisticBranch const&);
            
        private:
            ProgramGraphBuilder& builder;
        };
        
        
        class ProgramGraphBuilder {
            
            
        public:
            static storm::ppg::ProgramGraph* build(storm::pgcl::PgclProgram const& program) {
                ProgramGraphBuilder builder(program);
                builder.run();
                return builder.finalize();
            }
            
            ~ProgramGraphBuilder() {
                if(graph != nullptr) {
                //    delete graph;
                }
            }
            
            storm::ppg::ProgramLocation* currentLoc() const {
                return currentStack.back();
            }
            
            storm::ppg::ProgramLocation* newLocation() {
                currentStack.push_back(graph->addLocation());
                return currentLoc();
            }
            
            storm::ppg::ProgramLocation* storeNextLocation(storm::ppg::ProgramLocation* loc) {
                nextStack.push_back(loc);
            }
            
            storm::ppg::ProgramLocation* nextLoc() const {
                return nextStack.back();
            }
            
            storm::ppg::ProgramLocationIdentifier nextLocId() const {
                return nextLoc()->id();
            }
            
            storm::ppg::ProgramActionIdentifier getAction() const {
                return graph->addAction();
            }
            
            storm::ppg::ProgramActionIdentifier noAction() const {
                return noActionId;
            }
            
            
            
            void buildBlock(storm::pgcl::PgclBlock const& block) {
                
                ProgramGraphBuilderVisitor visitor(*this);
                for(auto const& statement : block) {
                    std::cout << "Statement " << statement->getLocationNumber() << std::endl;
                    if(!statement->isLast()) {
                        nextStack.push_back(graph->addLocation(false));
                    }
                    assert(!currentStack.empty());
                    assert(!nextStack.empty());
                    statement->accept(visitor);
                    assert(!currentStack.empty());
                    assert(!nextStack.empty());
                    currentStack.back() = nextStack.back();
                    nextStack.pop_back();
                }
            }
            
            
        private:
            ProgramGraphBuilder(storm::pgcl::PgclProgram const& program)
            : program(program)
            {
                graph = new storm::ppg::ProgramGraph(program.getExpressionManager(), program.getVariables());
                noActionId = getAction();
                
            }
             
            void run() {
                currentStack.push_back(graph->addLocation(true));
                // Terminal state.
                nextStack.push_back(graph->addLocation(false));
                // Observe Violated State.
                if(program.hasObserve()) {
                    observeFailedState = graph->addLocation();
                }
                buildBlock(program);
            }
            
            
            storm::ppg::ProgramGraph* finalize() {
                storm::ppg::ProgramGraph* tmp = graph;
                graph = nullptr;
                return tmp;
            }
            
            
            
            
            std::vector<storm::ppg::ProgramLocation*> currentStack;
            std::vector<storm::ppg::ProgramLocation*> nextStack;
            
            storm::ppg::ProgramActionIdentifier noActionId;
            
            storm::ppg::ProgramLocation* observeFailedState = nullptr;
            
            storm::pgcl::PgclProgram const& program;
            storm::ppg::ProgramGraph* graph;
            
            
            
        };
        
    }
}