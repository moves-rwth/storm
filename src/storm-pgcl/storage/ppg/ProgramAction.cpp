#include "ProgramAction.h"
#include "ProgramGraph.h"

namespace storm {
namespace ppg {

ProbabilisticProgramAction::ProbabilisticProgramAction(ProgramGraph* graph, ProgramActionIdentifier actId, ProgramVariableIdentifier var, int64_t from,
                                                       int64_t to)
    : ProgramAction(graph, actId), var(var) {
    assert(from <= to);
    storm::expressions::Expression prob = graph->getExpressionManager()->integer(1) / graph->getExpressionManager()->integer(to - from + 1);
    for (int64_t i = from; i <= to; ++i) {
        values.emplace_back(i, prob);
    }
}

std::string const& ProbabilisticProgramAction::getVariableName() const {
    return getProgramGraph().getVariableName(var);
}
}  // namespace ppg
}  // namespace storm
