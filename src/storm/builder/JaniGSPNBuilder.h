#pragma once

#include "storm/storage/gspn/GSPN.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace builder {
        class JaniGSPNBuilder {
        public:
            JaniGSPNBuilder(storm::gspn::GSPN const& gspn, std::shared_ptr<storm::expressions::ExpressionManager> const& expManager) : gspn(gspn), expressionManager(expManager) {
                gspn.writeDotToStream(std::cout);
            }
            
            virtual ~JaniGSPNBuilder() {
                for (auto const& varEntry : vars) {
                    delete varEntry.second;
                }
            }
            
            void setIgnoreWeights(bool ignore = true)  {
                ignoreWeights = ignore;
            }
            
            
            storm::jani::Model* build() {
                storm::jani::Model* model = new storm::jani::Model(gspn.getName(), storm::jani::ModelType::MA, janiVersion, expressionManager);
                storm::jani::Automaton mainAutomaton("immediate");
                addVariables(model);
                uint64_t locId = addLocation(mainAutomaton);
                addEdges(mainAutomaton, locId);
                model->addAutomaton(mainAutomaton);
                model->setStandardSystemComposition();
                return model;
            }
            
            void addVariables(storm::jani::Model* model) {
                for (auto const& place : gspn.getPlaces()) {
                    storm::jani::Variable* janiVar = nullptr;
                    if (!place.hasRestrictedCapacity()) {
                        // Effectively no capacity limit known
                        janiVar = new storm::jani::UnboundedIntegerVariable(place.getName(), expressionManager->declareIntegerVariable(place.getName()), expressionManager->integer(place.getNumberOfInitialTokens()));
                    } else {
                        assert(place.hasRestrictedCapacity());
                        janiVar = new storm::jani::BoundedIntegerVariable(place.getName(), expressionManager->declareIntegerVariable(place.getName()), expressionManager->integer(place.getNumberOfInitialTokens()), expressionManager->integer(0), expressionManager->integer(place.getCapacity()));
                    }
                    assert(janiVar != nullptr);
                    assert(vars.count(place.getID()) == 0);
                    vars[place.getID()] = janiVar;
                    model->addVariable(*janiVar);
                }
            }
            
            uint64_t addLocation(storm::jani::Automaton& automaton) {
                uint64_t janiLoc = automaton.addLocation(storm::jani::Location("loc"));
                automaton.addInitialLocation("loc");
                return janiLoc;
            }
            
            void addEdges(storm::jani::Automaton& automaton, uint64_t locId) {
                
                storm::expressions::Expression guard = expressionManager->boolean(true);
                for (auto const& trans : gspn.getImmediateTransitions()) {
                    if (ignoreWeights || trans.noWeightAttached()) {
                        std::vector<storm::jani::Assignment> assignments;
                        for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                            guard = guard && (vars[inPlaceEntry.first]->getExpressionVariable() > inPlaceEntry.second);
                            assignments.emplace_back( *vars[inPlaceEntry.first], (vars[inPlaceEntry.first])->getExpressionVariable() - inPlaceEntry.second);
                        }
                        for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                            guard = guard && (vars[inhibPlaceEntry.first]->getExpressionVariable() > inhibPlaceEntry.second);
                        }
                        for (auto const& outputPlaceEntry : trans.getOutputPlaces()) {
                            assignments.emplace_back( *vars[outputPlaceEntry.first], (vars[outputPlaceEntry.first])->getExpressionVariable() + outputPlaceEntry.second );
                        }
                        storm::jani::OrderedAssignments oa(assignments);
                        storm::jani::EdgeDestination dest(locId, expressionManager->integer(1), oa);
                        storm::jani::Edge e(locId, storm::jani::Model::SILENT_ACTION_INDEX, boost::none, guard, {dest});
                        automaton.addEdge(e);
                    }
                    
                }
                if(!ignoreWeights) {
                
                    // TODO here there is something to fix if we add transition partitions.
                    storm::expressions::Expression guard = expressionManager->boolean(false);
                    std::vector<storm::jani::EdgeDestination> weightedDestinations;
                    
                    // Compute enabled weight expression.
                    storm::expressions::Expression totalWeight = expressionManager->rational(0.0);
                    for (auto const& trans : gspn.getImmediateTransitions()) {
                        if (trans.noWeightAttached()) {
                            continue;
                        }
                        storm::expressions::Expression destguard = expressionManager->boolean(true);
                        for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                            destguard = destguard && (vars[inPlaceEntry.first]->getExpressionVariable() > inPlaceEntry.second);
                        }
                        for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                            destguard = destguard && (vars[inhibPlaceEntry.first]->getExpressionVariable() > inhibPlaceEntry.second);
                        }
                        totalWeight = totalWeight + storm::expressions::ite(destguard, expressionManager->rational(trans.getWeight()), expressionManager->rational(0.0));
                        
                    }
                    totalWeight = totalWeight.simplify();
                    
                    for (auto const& trans : gspn.getImmediateTransitions()) {
                        if (trans.noWeightAttached()) {
                            continue;
                        }
                        storm::expressions::Expression destguard = expressionManager->boolean(true);
                        std::vector<storm::jani::Assignment> assignments;
                        for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                            destguard = destguard && (vars[inPlaceEntry.first]->getExpressionVariable() > inPlaceEntry.second);
                            assignments.emplace_back( *vars[inPlaceEntry.first], (vars[inPlaceEntry.first]->getExpressionVariable() - inPlaceEntry.second) );
                        }
                        for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                            destguard = destguard && (vars[inhibPlaceEntry.first]->getExpressionVariable() > inhibPlaceEntry.second);
                        }
                        for (auto const& outputPlaceEntry : trans.getOutputPlaces()) {
                            assignments.emplace_back( *vars[outputPlaceEntry.first], (vars[outputPlaceEntry.first]->getExpressionVariable() + outputPlaceEntry.second) );
                        }
                        destguard = destguard.simplify();
                        guard = guard || destguard;
                        storm::jani::OrderedAssignments oa(assignments);
                        storm::jani::EdgeDestination dest(locId, storm::expressions::ite(destguard, (expressionManager->rational(trans.getWeight()) / totalWeight), expressionManager->rational(0.0)), oa);
                        weightedDestinations.push_back(dest);
                    }
                    storm::jani::Edge e(locId, storm::jani::Model::SILENT_ACTION_INDEX, boost::none, guard.simplify(), weightedDestinations);
                    automaton.addEdge(e);
                }
                for (auto const& trans : gspn.getTimedTransitions()) {
                    storm::expressions::Expression guard = expressionManager->boolean(true);
                    
                    std::vector<storm::jani::Assignment> assignments;
                    for (auto const& inPlaceEntry : trans.getInputPlaces()) {
                        guard = guard && (vars[inPlaceEntry.first]->getExpressionVariable() > inPlaceEntry.second);
                        assignments.emplace_back( *vars[inPlaceEntry.first], (vars[inPlaceEntry.first]->getExpressionVariable() - inPlaceEntry.second) );
                    }
                    for (auto const& inhibPlaceEntry : trans.getInhibitionPlaces()) {
                        guard = guard && (vars[inhibPlaceEntry.first]->getExpressionVariable() > inhibPlaceEntry.second);
                    }
                    for (auto const& outputPlaceEntry : trans.getOutputPlaces()) {
                        assignments.emplace_back( *vars[outputPlaceEntry.first], (vars[outputPlaceEntry.first]->getExpressionVariable() + outputPlaceEntry.second) );
                    }
                    storm::jani::OrderedAssignments oa(assignments);
                    storm::jani::EdgeDestination dest(locId, expressionManager->integer(1), oa);
                    storm::jani::Edge e(locId, storm::jani::Model::SILENT_ACTION_INDEX, expressionManager->rational(trans.getRate()), guard, {dest});
                    automaton.addEdge(e);
                }
            }
            
        private:
            bool ignoreWeights;
            const uint64_t janiVersion = 1;
            storm::gspn::GSPN const& gspn;
            std::map<uint64_t, storm::jani::Variable*> vars;
            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
            
        };
    }
}
