#include "JaniProgramGraphBuilder.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/jani/EdgeDestination.h"

namespace storm {
namespace builder {
unsigned JaniProgramGraphBuilder::janiVersion = 1;

void JaniProgramGraphBuilder::addProcedureVariables(storm::jani::Model& model, storm::jani::Automaton& automaton) {
    for (auto const& v : programGraph.getVariables()) {
        if (isConstant(v.first)) {
            storm::jani::Constant constant(v.second.getName(), v.second, programGraph.getInitialValue(v.first));
            model.addConstant(constant);
        } else if (v.second.hasBooleanType()) {
            auto janiVar = storm::jani::Variable::makeBooleanVariable(v.second.getName(), v.second, programGraph.getInitialValue(v.first), false);
            automaton.addVariable(*janiVar);
            variables.emplace(v.first, janiVar);
        } else if (isRestrictedVariable(v.first) && !isRewardVariable(v.first)) {
            storm::storage::IntegerInterval const& bounds = variableBounds(v.first);
            if (bounds.hasLeftBound()) {
                if (bounds.hasRightBound()) {
                    auto janiVar = storm::jani::Variable::makeBoundedIntegerVariable(v.second.getName(), v.second, programGraph.getInitialValue(v.first), false,
                                                                                     expManager->integer(bounds.getLeftBound().get()),
                                                                                     expManager->integer(bounds.getRightBound().get()));
                    variables.emplace(v.first, janiVar);
                    automaton.addVariable(*janiVar);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unbounded right bound is not supported yet");
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unbounded left bound is not supported yet");
            }
        } else {
            auto janiVar =
                storm::jani::Variable::makeIntegerVariable(v.second.getName(), v.second, programGraph.getInitialValue(v.first), isRewardVariable(v.first));
            if (isRewardVariable(v.first)) {
                model.addVariable(*janiVar);
            } else {
                automaton.addVariable(*janiVar);
            }
            variables.emplace(v.first, janiVar);
        }
    }
}

storm::jani::OrderedAssignments JaniProgramGraphBuilder::buildOrderedAssignments(storm::jani::Automaton& automaton,
                                                                                 storm::ppg::DeterministicProgramAction const& act) {
    std::vector<storm::jani::Assignment> vec;
    uint64_t level = 0;
    for (auto const& group : act) {
        for (auto const& assignment : group) {
            if (isRewardVariable(assignment.first)) {
                std::unordered_map<storm::expressions::Variable, storm::expressions::Expression> eval;
                eval.emplace((variables.at(assignment.first))->getExpressionVariable(), expManager->integer(0));
                vec.emplace_back(storm::jani::LValue(*(variables.at(assignment.first))), assignment.second.substitute(eval).simplify(), level);
            } else {
                vec.emplace_back(storm::jani::LValue(*(variables.at(assignment.first))), assignment.second, level);
            }
        }
        ++level;
    }
    return storm::jani::OrderedAssignments(vec);
}

std::vector<std::pair<uint64_t, storm::expressions::Expression>> JaniProgramGraphBuilder::buildProbabilisticDestinations(
    storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge, storm::jani::TemplateEdge& templateEdge) {
    storm::ppg::ProbabilisticProgramAction const& act = static_cast<storm::ppg::ProbabilisticProgramAction const&>(edge.getAction());
    std::vector<std::pair<uint64_t, storm::expressions::Expression>> vec;
    for (auto const& assign : act) {
        storm::jani::Assignment assignment(storm::jani::LValue(automaton.getVariables().getVariable(act.getVariableName())), expManager->integer(assign.value),
                                           0);
        templateEdge.addDestination(storm::jani::TemplateEdgeDestination(storm::jani::OrderedAssignments(assignment)));
        vec.emplace_back(janiLocId.at(edge.getTargetId()), assign.probability);
    }
    return vec;
}

std::vector<std::pair<uint64_t, storm::expressions::Expression>> JaniProgramGraphBuilder::buildDestinations(storm::jani::Automaton& automaton,
                                                                                                            storm::ppg::ProgramEdge const& edge,
                                                                                                            storm::jani::TemplateEdge& templateEdge) {
    if (edge.getAction().isProbabilistic()) {
        return buildProbabilisticDestinations(automaton, edge, templateEdge);
    } else {
        storm::jani::OrderedAssignments oa = buildOrderedAssignments(automaton, static_cast<storm::ppg::DeterministicProgramAction const&>(edge.getAction()));
        templateEdge.addDestination(storm::jani::TemplateEdgeDestination(oa));
        return {std::make_pair(janiLocId.at(edge.getTargetId()), expManager->rational(1.0))};
    }
}

storm::expressions::Expression simplifyExpression(storm::expressions::Expression const& in) {
    // TODO use bound restrictions etc.
    return in.simplify();
}

std::pair<std::vector<storm::jani::Edge>, storm::expressions::Expression> JaniProgramGraphBuilder::addVariableChecks(storm::jani::Automaton& automaton,
                                                                                                                     storm::ppg::ProgramEdge const& edge) {
    std::vector<storm::jani::Edge> edges;
    storm::expressions::Expression newGuard;
    newGuard = expManager->boolean(true);
    if (edge.getAction().isProbabilistic()) {
        // No check necessary currently, but at least we should statically check that the bounds are okay.
        storm::ppg::ProbabilisticProgramAction const& act = static_cast<storm::ppg::ProbabilisticProgramAction const&>(edge.getAction());
        if (isUserRestrictedVariable(act.getVariableIdentifier())) {
            storm::storage::IntegerInterval const& bound = userVariableRestrictions.at(act.getVariableIdentifier());
            storm::storage::IntegerInterval supportInterval = act.getSupportInterval();
            STORM_LOG_THROW(bound.contains(supportInterval), storm::exceptions::NotSupportedException,
                            "User provided bounds must contain all constant expressions");
        }
    } else {
        storm::ppg::DeterministicProgramAction const& act = static_cast<storm::ppg::DeterministicProgramAction const&>(edge.getAction());
        STORM_LOG_THROW(act.nrLevels() <= 1, storm::exceptions::NotSupportedException, "Multi-level assignments with user variable bounds not supported");
        for (auto const& group : act) {
            for (auto const& assignment : group) {
                if (isUserRestrictedVariable(assignment.first)) {
                    assert(userVariableRestrictions.count(assignment.first) == 1);
                    storm::storage::IntegerInterval const& bound = userVariableRestrictions.at(assignment.first);
                    if (!assignment.second.containsVariables()) {
                        // Constant assignments can be checked statically.
                        // TODO we might still want to allow assignments which go out of bounds.
                        STORM_LOG_THROW(bound.contains(assignment.second.evaluateAsInt()), storm::exceptions::NotSupportedException,
                                        "User provided bounds must contain all constant expressions");
                    } else {
                        // TODO currently only fully bounded restrictions are supported;
                        assert(userVariableRestrictions.at(assignment.first).hasLeftBound() && userVariableRestrictions.at(assignment.first).hasRightBound());
                        storm::expressions::Expression newCondition = simplifyExpression(
                            edge.getCondition() && (assignment.second > bound.getRightBound().get() || assignment.second < bound.getLeftBound().get()));

                        std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(newCondition);
                        automaton.registerTemplateEdge(templateEdge);
                        templateEdge->addDestination(storm::jani::TemplateEdgeDestination());
                        storm::jani::Edge e(janiLocId.at(edge.getSourceId()), storm::jani::Model::SILENT_ACTION_INDEX, boost::none, templateEdge,
                                            {varOutOfBoundsLocations.at(assignment.first)}, {expManager->rational(1.0)});
                        edges.push_back(e);
                        newGuard = newGuard && assignment.second <= bound.getRightBound().get() && assignment.second >= bound.getLeftBound().get();
                    }
                }
            }
        }
    }
    return {edges, newGuard};
}

void JaniProgramGraphBuilder::addEdges(storm::jani::Automaton& automaton) {
    for (auto it = programGraph.locationBegin(); it != programGraph.locationEnd(); ++it) {
        ppg::ProgramLocation const& loc = it->second;
        if (loc.nrOutgoingEdgeGroups() == 0) {
            std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(expManager->boolean(true));
            automaton.registerTemplateEdge(templateEdge);
            templateEdge->addDestination(storm::jani::TemplateEdgeDestination());
            storm::jani::Edge e(janiLocId.at(loc.id()), storm::jani::Model::SILENT_ACTION_INDEX, boost::none, templateEdge, {janiLocId.at(loc.id())},
                                {expManager->rational(1.0)});
            automaton.addEdge(e);
        } else if (loc.nrOutgoingEdgeGroups() == 1) {
            for (auto const& edge : **(loc.begin())) {
                std::pair<std::vector<storm::jani::Edge>, storm::expressions::Expression> checks = addVariableChecks(automaton, *edge);
                for (auto const& check : checks.first) {
                    automaton.addEdge(check);
                }
                std::shared_ptr<storm::jani::TemplateEdge> templateEdge =
                    std::make_shared<storm::jani::TemplateEdge>(simplifyExpression(edge->getCondition() && checks.second));
                automaton.registerTemplateEdge(templateEdge);

                std::vector<std::pair<uint64_t, storm::expressions::Expression>> destinationLocationsAndProbabilities =
                    buildDestinations(automaton, *edge, *templateEdge);

                storm::jani::Edge e(janiLocId.at(loc.id()), storm::jani::Model::SILENT_ACTION_INDEX, boost::none, templateEdge,
                                    destinationLocationsAndProbabilities);
                automaton.addEdge(e);
            }
        } else {
            // We have probabilistic branching
            if (loc.hasNonDeterminism()) {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                "Combi of nondeterminism and probabilistic choices within a loc not supported yet");
            } else {
                std::shared_ptr<storm::jani::TemplateEdge> templateEdge = std::make_shared<storm::jani::TemplateEdge>(expManager->boolean(true));
                automaton.registerTemplateEdge(templateEdge);

                std::vector<storm::expressions::Expression> destinationProbabilities;
                std::vector<uint64_t> destinationLocations;
                for (auto const& eg : loc) {
                    // TODO add assignments
                    assert(eg->nrEdges() < 2);  // Otherwise, non-determinism occurs.
                    assert(eg->nrEdges() > 0);  // Empty edge groups should not occur in input.
                    destinationLocations.push_back(janiLocId.at((*eg->begin())->getTargetId()));
                    destinationProbabilities.push_back(eg->getProbability());
                    templateEdge->addDestination(storm::jani::TemplateEdgeDestination());
                }

                storm::jani::Edge e(janiLocId.at(it->second.id()), storm::jani::Model::SILENT_ACTION_INDEX, boost::none, templateEdge, destinationLocations,
                                    destinationProbabilities);
                automaton.addEdge(e);
            }
        }
    }
}

}  // namespace builder
}  // namespace storm
