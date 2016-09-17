#include "JaniProgramGraphBuilder.h"

namespace storm {
    namespace builder {
        unsigned JaniProgramGraphBuilder::janiVersion = 1;
        
        storm::jani::OrderedAssignments buildOrderedAssignments(storm::jani::Automaton& automaton, storm::ppg::DeterministicProgramAction const& act) {
            std::vector<storm::jani::Assignment> vec;
            uint64_t level = 0;
            for(auto const& group : act) {
                for(auto const& assignment : group) {
                    vec.emplace_back(automaton.getVariables().getVariable(act.getProgramGraph().getVariableName(assignment.first)) , assignment.second, level);
                }
                ++level;
            }
            return storm::jani::OrderedAssignments(vec);
        }
        
        std::vector<storm::jani::EdgeDestination> buildProbabilisticDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge ) {
            storm::ppg::ProbabilisticProgramAction const& act = static_cast<storm::ppg::ProbabilisticProgramAction const&>(edge.getAction());
            std::vector<storm::jani::EdgeDestination> vec;
            //for(auto const& value : act ) {
            //    vec.emplace_back(
            //}
            return vec;
        }
    
        std::vector<storm::jani::EdgeDestination> JaniProgramGraphBuilder::buildDestinations(storm::jani::Automaton& automaton, storm::ppg::ProgramEdge const& edge ) {
            if (edge.getAction().isProbabilistic()) {
                return buildProbabilisticDestinations(automaton, edge);
            } else {
                storm::jani::OrderedAssignments oa = buildOrderedAssignments(automaton, static_cast<storm::ppg::DeterministicProgramAction const&>(edge.getAction()));
                storm::jani::EdgeDestination dest(edge.getTargetId(), expManager->rational(1.0), oa);
                return {dest};
            }
        }
        
        
        
        void JaniProgramGraphBuilder::addEdges(storm::jani::Automaton& automaton, storm::ppg::ProgramGraph const& pg, std::map<storm::ppg::ProgramLocationIdentifier, uint64_t>  const& janiLocId) {
            for(auto it = pg.locationBegin(); it != pg.locationEnd(); ++it) {
                ppg::ProgramLocation const& loc = it->second;
                if(loc.nrOutgoingEdgeGroups() == 1) {
                    for(auto const& edge :  **(loc.begin())) {
                        storm::jani::Edge e(janiLocId.at(loc.id()), storm::jani::Model::silentActionIndex, boost::none, edge->getCondition(), buildDestinations(automaton, *edge));
                        automaton.addEdge(e);
                    }
                } else {
                    if(loc.hasNonDeterminism())
                    {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Combi of nondeterminism and probabilistic choices within a loc not supported yet");
                    } else {
                        std::vector<storm::jani::EdgeDestination> destinations;
                        for(auto const& eg : loc) {
                            // TODO add assignments
                            assert(eg->nrEdges() < 2); // Otherwise, non-determinism occurs.
                            assert(eg->nrEdges() > 0); // Empty edge groups should not occur in input.
                            uint64_t target = janiLocId.at((*eg->begin())->getTargetId());
                            destinations.emplace_back(target, eg->getProbability());
                        }
                        storm::jani::Edge e(janiLocId.at(it->second.id()), storm::jani::Model::silentActionIndex, boost::none,  expManager->boolean(true), destinations);
                        automaton.addEdge(e);
                    }
                }
            }
        }

    }
}