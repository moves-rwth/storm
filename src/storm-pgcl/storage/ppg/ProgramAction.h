#pragma once
#include "defines.h"
#include "storm/storage/IntegerInterval.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Variable.h"

namespace storm {
namespace ppg {

class ProgramAction {
   public:
    ProgramAction(ProgramGraph* graph, ProgramActionIdentifier id) : graph(graph), actId(id) {}

    virtual ~ProgramAction() = default;

    ProgramActionIdentifier id() const {
        return actId;
    }

    ProgramGraph const& getProgramGraph() const {
        return *graph;
    }

    virtual bool isProbabilistic() const = 0;

   private:
    ProgramGraph* graph;
    ProgramActionIdentifier actId;
};

struct ValueProbabilityPair {
    ValueProbabilityPair(int64_t value, storm::expressions::Expression const& probability) : value(value), probability(probability) {
        // Intentionally left empty.
    }

    int64_t value;
    storm::expressions::Expression probability;
};

class ProbabilisticProgramAction : public ProgramAction {
   public:
    // TODO in the long run, we probably need own iterators for this.
    using iterator = std::vector<ValueProbabilityPair>::iterator;
    using const_iterator = std::vector<ValueProbabilityPair>::const_iterator;

    /**
     * Constructs a uniform assignment operation to a variable;
     * Action assigns a variable according to a uniform distribution [from, to]
     */
    ProbabilisticProgramAction(ProgramGraph* graph, ProgramActionIdentifier actId, ProgramVariableIdentifier var, int64_t from, int64_t to);

    bool isProbabilistic() const override {
        return true;
    }

    std::string const& getVariableName() const;

    ProgramVariableIdentifier getVariableIdentifier() const {
        return var;
    }

    storm::storage::IntegerInterval getSupportInterval() const {
        assert(!values.empty());
        int64_t min = values.front().value;
        int64_t max = values.front().value;
        for (auto const& valEntry : values) {
            if (valEntry.value < min) {
                min = valEntry.value;
            } else if (valEntry.value > max) {
                max = valEntry.value;
            }
        }
        return storm::storage::IntegerInterval(min, max);
    }

    iterator begin() {
        return values.begin();
    }

    iterator end() {
        return values.end();
    }

    const_iterator begin() const {
        return values.begin();
    }

    const_iterator end() const {
        return values.end();
    }

   private:
    // TODO not the smartest representation (but at least it is internal!)
    std::vector<ValueProbabilityPair> values;
    ProgramVariableIdentifier var;
};

struct AssignmentGroup {
    using iterator = std::unordered_map<uint64_t, storm::expressions::Expression>::iterator;
    using const_iterator = std::unordered_map<uint64_t, storm::expressions::Expression>::const_iterator;

    storm::expressions::Expression& operator[](uint64_t varIndex) {
        return map[varIndex];
    }

    bool hasVariable(uint64_t varIndex) const {
        return map.count(varIndex) > 0;
    }

    iterator begin() {
        return map.begin();
    }

    iterator end() {
        return map.end();
    }

    const_iterator begin() const {
        return map.begin();
    }

    const_iterator end() const {
        return map.end();
    }

   private:
    std::unordered_map<ProgramVariableIdentifier, storm::expressions::Expression> map;
};

class DeterministicProgramAction : public ProgramAction {
   public:
    using iterator = std::vector<AssignmentGroup>::iterator;
    using const_iterator = std::vector<AssignmentGroup>::const_iterator;

    DeterministicProgramAction(ProgramGraph* graph, ProgramActionIdentifier actId) : ProgramAction(graph, actId) {}

    void addAssignment(ProgramVariableIdentifier varIndex, storm::expressions::Expression const& expr, uint64_t level = 0) {
        if (assignments.size() <= level) {
            assignments.resize(level + 1);
        }
        assert(!assignments[level].hasVariable(varIndex));
        assignments[level][varIndex] = expr;
    }

    size_t nrLevels() const {
        return assignments.size();
    }

    iterator begin() {
        return assignments.begin();
    }

    iterator end() {
        return assignments.end();
    }

    const_iterator begin() const {
        return assignments.begin();
    }

    const_iterator end() const {
        return assignments.end();
    }

    bool isProbabilistic() const override {
        return false;
    }

   protected:
    std::vector<AssignmentGroup> assignments;
};

}  // namespace ppg
}  // namespace storm
