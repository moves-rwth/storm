#pragma once

#include "storm-pgcl/storage/pgcl/AbstractStatementVisitor.h"
#include "storm-pgcl/storage/pgcl/PgclProgram.h"
#include "storm-pgcl/storage/pgcl/UniformExpression.h"
#include "storm-pgcl/storage/ppg/ProgramGraph.h"

namespace storm {
namespace builder {
class ProgramGraphBuilder;

class ProgramGraphBuilderVisitor : public storm::pgcl::AbstractStatementVisitor {
   public:
    ProgramGraphBuilderVisitor(ProgramGraphBuilder& builder) : builder(builder) {}

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
        if (graph != nullptr) {
            delete graph;
        }
    }

    storm::ppg::ProgramLocation* currentLoc() const {
        return currentStack.back();
    }

    storm::ppg::ProgramLocation* newLocation() {
        currentStack.push_back(graph->addLocation());
        return currentLoc();
    }

    void storeNextLocation(storm::ppg::ProgramLocation* loc) {
        nextStack.push_back(loc);
    }

    storm::ppg::ProgramLocation* nextLoc() const {
        return nextStack.back();
    }

    storm::ppg::ProgramLocationIdentifier nextLocId() const {
        return nextLoc()->id();
    }

    storm::ppg::ProgramActionIdentifier addAction(storm::expressions::Variable const& var, storm::expressions::Expression const& expr) const {
        storm::ppg::DeterministicProgramAction* action = graph->addDeterministicAction();
        action->addAssignment(graph->getVariableId(var.getName()), expr);
        return action->id();
    }

    storm::ppg::ProgramActionIdentifier addAction(storm::expressions::Variable const& var, storm::pgcl::UniformExpression const& expr) const {
        storm::ppg::ProbabilisticProgramAction* action =
            graph->addUniformProbabilisticAction(graph->getVariableId(var.getName()), expr.getBegin(), expr.getEnd());
        return action->id();
    }

    storm::ppg::ProgramActionIdentifier noAction() const {
        return noActionId;
    }

    std::shared_ptr<storm::expressions::ExpressionManager> const& getExpressionManager() const {
        return program.getExpressionManager();
    }

    void buildBlock(storm::pgcl::PgclBlock const& block) {
        ProgramGraphBuilderVisitor visitor(*this);
        for (auto const& statement : block) {
            if (!statement->isLast()) {
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
    ProgramGraphBuilder(storm::pgcl::PgclProgram const& program) : program(program) {
        graph = new storm::ppg::ProgramGraph(program.getExpressionManager(), program.getVariableDeclarations());
        noActionId = graph->getNoActionId();
    }

    /**
     * Runs the translator.
     */
    void run() {
        currentStack.push_back(graph->addLocation(true));
        // Terminal state.
        nextStack.push_back(graph->addLocation(false, true));
        // Observe Violated State.
        if (program.hasObserve()) {
            observeFailedState = graph->addLocation(false, false, true);
        }
        buildBlock(program);
    }

    /**
     * Extracts the program graph from the builder.
     * @return
     */
    storm::ppg::ProgramGraph* finalize() {
        storm::ppg::ProgramGraph* tmp = graph;
        graph = nullptr;
        return tmp;
    }

    std::vector<storm::ppg::ProgramLocation*> currentStack;
    std::vector<storm::ppg::ProgramLocation*> nextStack;
    /// Action id if no action is to be executed.
    storm::ppg::ProgramActionIdentifier noActionId;
    /// Resulting state of an failed observation.
    storm::ppg::ProgramLocation* observeFailedState = nullptr;
    /// PGCL Program to translate.
    storm::pgcl::PgclProgram const& program;
    /// Obtained Program Graph.
    storm::ppg::ProgramGraph* graph;
};

}  // namespace builder
}  // namespace storm
