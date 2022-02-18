#pragma once

#include <storm/exceptions/IllegalArgumentException.h>
#include "defines.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Variable.h"

#include "ProgramAction.h"
#include "ProgramEdge.h"
#include "ProgramEdgeGroup.h"
#include "ProgramLocation.h"

#include "storm-pgcl/storage/pgcl/VariableDeclaration.h"

namespace storm {
namespace ppg {
/**
 *  Program graph as based on Principles of Model Checking, Def 2.13
 *  Action effects are part of the action.
 */
class ProgramGraph {
   public:
    using EdgeGroupIterator = ProgramLocation::EdgeGroupIterator;
    using ConstLocationIterator = std::unordered_map<ProgramLocationIdentifier, ProgramLocation>::const_iterator;

    ProgramGraph(std::shared_ptr<storm::expressions::ExpressionManager> const& expManager, std::vector<storm::pgcl::VariableDeclaration> const& variables)
        : variables(), expManager(expManager) {
        for (auto const& v : variables) {
            this->variables.emplace(v.getVariable().getIndex(), v.getVariable());
            this->initialValues.emplace(v.getVariable().getIndex(), v.getInitialValueExpression());
        }
        // No Action:
        deterministicActions.emplace(noActionId, DeterministicProgramAction(this, noActionId));
    }

    ProgramGraph(ProgramGraph const&) = delete;

    virtual ~ProgramGraph() {}

    DeterministicProgramAction* addDeterministicAction() {
        ProgramActionIdentifier newId = freeActionIndex();
        assert(!hasAction(newId));
        return &(deterministicActions.emplace(newId, DeterministicProgramAction(this, newId)).first->second);
    }

    ProbabilisticProgramAction* addUniformProbabilisticAction(ProgramVariableIdentifier var, int64_t from, int64_t to) {
        ProgramActionIdentifier newId = freeActionIndex();
        assert(!hasAction(newId));
        return &(probabilisticActions.emplace(newId, ProbabilisticProgramAction(this, newId, var, from, to)).first->second);
    }

    ProgramLocation* addLocation(bool isInitial = false, bool successfulTermination = false, bool observeViolated = false, bool aborted = false,
                                 std::vector<std::string> const& labels = {}) {
        ProgramLocationIdentifier newId = freeLocationIndex();
        assert(!hasLocation(newId));
        locationLabels[newId] = labels;
        if (successfulTermination) {
            locationLabels[newId].push_back(succesfulTerminationLabel);
        }
        if (observeViolated) {
            locationLabels[newId].push_back(observeViolatedLabel);
        }
        if (aborted) {
            locationLabels[newId].push_back(abortLabel);
        }
        return &(locations.emplace(newId, ProgramLocation(this, newId, isInitial)).first->second);
    }

    ProgramEdgeGroup* addProgramEdgeGroup(ProgramLocation& source, storm::expressions::Expression const& probability) {
        ProgramEdgeGroupIdentifier newId = freeEdgeGroupIndex();
        return source.emplaceEdgeGroup(newId, probability);
    }

    ProgramEdgeGroup* addProgramEdgeGroup(ProgramLocationIdentifier sourceId, storm::expressions::Expression const& probability) {
        assert(hasLocation(sourceId));
        return addProgramEdgeGroup(getLocation(sourceId), probability);
    }

    ProgramEdge* addProgramEdge(ProgramEdgeGroup& group, ProgramActionIdentifier action, ProgramLocationIdentifier targetId) {
        return addProgramEdge(group, action, expManager->boolean(true), targetId);
    }

    ProgramEdge* addProgramEdge(ProgramEdgeGroup& group, ProgramActionIdentifier action, storm::expressions::Expression const& condition,
                                ProgramLocationIdentifier targetId) {
        ProgramEdgeIdentifier newId = freeEdgeIndex();
        return group.emplaceEdge(newId, action, condition, targetId);
    }

    std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramLocation& source, ProgramActionIdentifier action, ProgramLocationIdentifier targetId) {
        return addProgramEdgeToAllGroups(source, action, expManager->boolean(true), targetId);
    }

    std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramLocation& source, ProgramActionIdentifier action,
                                                        storm::expressions::Expression const& condition, ProgramLocationIdentifier targetId) {
        assert(hasLocation(targetId));
        assert(hasAction(action));

        if (source.nrOutgoingEdgeGroups() == 0) {
            addProgramEdgeGroup(source, expManager->rational(1));
        }

        std::vector<ProgramEdge*> res;
        for (EdgeGroupIterator eg = source.getOutgoingEdgeGroupBegin(); eg != source.getOutgoingEdgeGroupEnd(); ++eg) {
            ProgramEdgeIdentifier newId = freeEdgeIndex();
            res.push_back((*eg)->emplaceEdge(newId, action, condition, targetId));
        }

        return res;
    }

    std::set<std::string> getLabels() const {
        std::set<std::string> result;
        for (auto const& locEntry : locationLabels) {
            for (auto const& label : locEntry.second) {
                result.insert(label);
            }
        }
        return result;
    }

    std::vector<std::string> getLabels(ProgramLocationIdentifier loc) const {
        return locationLabels.at(loc);
    }

    bool hasLabel(ProgramLocationIdentifier loc, std::string const& label) const {
        return std::find(locationLabels.at(loc).begin(), locationLabels.at(loc).end(), label) != locationLabels.at(loc).end();
    }

    bool hasSuccessfulTerminationLabel(ProgramLocationIdentifier loc) const {
        return hasLabel(loc, succesfulTerminationLabel);
    }

    bool hasAbortLabel(ProgramLocationIdentifier loc) const {
        return hasLabel(loc, abortLabel);
    }

    bool hasTerminationLabel(ProgramLocationIdentifier loc) const {
        return hasSuccessfulTerminationLabel(loc) || hasAbortLabel(loc);
    }

    ProgramActionIdentifier getNoActionId() const {
        return noActionId;
    }

    std::vector<ProgramEdge*> addProgramEdgeToAllGroups(ProgramLocationIdentifier sourceId, ProgramActionIdentifier action,
                                                        storm::expressions::Expression const& condition, ProgramLocationIdentifier targetId) {
        assert(hasLocation(sourceId));
        return addProgramEdgeToAllGroups(getLocation(sourceId), action, condition, targetId);
    }

    ProgramVariableIdentifier getVariableId(std::string const& varName) const {
        // TODO consider holding a map for this.
        for (auto const& v : variables) {
            if (v.second.getName() == varName) {
                return v.first;
            }
        }
        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Variable " << varName << " was not found.");
        return 0;
    }

    std::string const& getVariableName(ProgramVariableIdentifier id) const {
        return variables.at(id).getName();
    }

    bool hasVariable(std::string const& varName) const {
        for (auto const& v : variables) {
            if (v.second.getName() == varName) {
                return true;
            }
        }
        return false;
    }

    bool hasLocation(ProgramLocationIdentifier id) const {
        return locations.count(id) == 1;
    }

    bool hasAction(ProgramActionIdentifier id) const {
        return deterministicActions.count(id) == 1 || probabilisticActions.count(id);
    }

    ProgramAction const& getAction(ProgramActionIdentifier id) const {
        assert(hasAction(id));
        if (isDeterministicAction(id)) {
            return deterministicActions.at(id);
        } else {
            return probabilisticActions.at(id);
        }
    }

    bool isDeterministicAction(ProgramActionIdentifier id) const {
        assert(hasAction(id));
        return probabilisticActions.count(id) == 0;
    }

    size_t nrLocations() const {
        return locations.size();
    }

    size_t nrVariables() const {
        return variables.size();
    }

    size_t nrActions() const {
        return variables.size();
    }

    storm::expressions::Expression getInitialValue(ProgramVariableIdentifier v) const {
        return initialValues.at(v);
    }

    void collectInitialValues();

    ConstLocationIterator locationBegin() const {
        return locations.begin();
    }

    ConstLocationIterator locationEnd() const {
        return locations.end();
    }

    std::unordered_map<ProgramVariableIdentifier, storm::expressions::Variable> const& getVariables() const {
        return variables;
    }

    std::shared_ptr<storm::expressions::ExpressionManager> const& getExpressionManager() const {
        return expManager;
    }

    std::vector<ProgramVariableIdentifier> noeffectVariables() const;

    std::vector<ProgramVariableIdentifier> rewardVariables() const;

    std::vector<ProgramVariableIdentifier> constantAssigned() const;

    std::vector<ProgramVariableIdentifier> constants() const;

    storm::storage::IntegerInterval supportForConstAssignedVariable(ProgramVariableIdentifier i) const;

    void checkValid() {}

    void printInfo(std::ostream& os) const {
        os << "Number of variables: " << nrVariables() << '\n';
        os << "Number of locations: " << nrLocations() << '\n';
        os << "Number of actions: " << nrActions() << '\n';
    }

    void printDot(std::ostream& os) const;

   protected:
    std::vector<ProgramLocationIdentifier> initialLocationIdentifiers() const {
        std::vector<ProgramLocationIdentifier> result;
        for (auto const& loc : locations) {
            if (loc.second.isInitial()) {
                result.push_back(loc.first);
            }
        }
        return result;
    }

    /**
     *  Returns the set of variables which do not occur in guards.
     */
    std::vector<ProgramVariableIdentifier> variablesNotInGuards() const;

    std::pair<bool, bool> checkIfRewardVariableHelper(storm::expressions::Variable const& var,
                                                      std::unordered_map<ProgramActionIdentifier, DeterministicProgramAction> const& detActions) const;

    ProgramLocation& getLocation(ProgramLocationIdentifier id) {
        return locations.at(id);
    }

    /**
     * Gets a free location index (based on whatever scheme we are using).
     */
    ProgramLocationIdentifier freeLocationIndex() {
        return newLocationId++;
    }

    ProgramActionIdentifier freeActionIndex() {
        return newActionId++;
    }

    ProgramEdgeIdentifier freeEdgeIndex() {
        return newEdgeId++;
    }

    ProgramEdgeGroupIdentifier freeEdgeGroupIndex() {
        return newEdgeGroupId++;
    }

    std::unordered_map<ProgramActionIdentifier, DeterministicProgramAction> deterministicActions;
    std::unordered_map<ProgramActionIdentifier, ProbabilisticProgramAction> probabilisticActions;
    std::unordered_map<ProgramLocationIdentifier, ProgramLocation> locations;
    storm::expressions::Expression initialValueRestriction;
    std::unordered_map<ProgramVariableIdentifier, storm::expressions::Variable> variables;
    ///
    std::unordered_map<ProgramVariableIdentifier, storm::expressions::Expression> initialValues;
    // If heavily used, then it might be better to use a bitvector and a seperate list for the names.
    std::unordered_map<ProgramVariableIdentifier, std::vector<std::string>> locationLabels;
    std::shared_ptr<storm::expressions::ExpressionManager> expManager;

   private:
    // Helper for IDs, may be changed later.
    ProgramEdgeGroupIdentifier newEdgeGroupId = 0;
    ProgramLocationIdentifier newLocationId = 0;
    ProgramEdgeIdentifier newEdgeId = 0;
    ProgramActionIdentifier newActionId = 1;
    ProgramActionIdentifier noActionId = 0;
    std::string succesfulTerminationLabel = "_ret0_";
    std::string abortLabel = "_ret1_";
    std::string observeViolatedLabel = "_viol_";
};
}  // namespace ppg
}  // namespace storm
