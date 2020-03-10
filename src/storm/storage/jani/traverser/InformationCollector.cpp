#include "storm/storage/jani/traverser/InformationCollector.h"
#include "storm/storage/jani/traverser/JaniTraverser.h"
#include "storm/storage/jani/Model.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace jani {
        namespace detail {
            class InformationCollector : public ConstJaniTraverser {
            public:
                InformationObject collect(Model const& model) {
                    info = InformationObject();
                    domainSizesSum = 0;
                    this->traverse(model, boost::any());
                    if (info.stateDomainSize > 0) {
                        info.avgVarDomainSize = storm::utility::convertNumber<double>(domainSizesSum) / storm::utility::convertNumber<double>(info.nrVariables);
                    } else {
                        info.avgVarDomainSize = 0.0;
                    }
                    return info;
                }
                
                virtual void traverse(Model const& model, boost::any const& data) override {
                    info.modelType = model.getModelType();
                    info.nrAutomata = model.getNumberOfAutomata();
                    ConstJaniTraverser::traverse(model, data);
                }
                
                virtual void traverse(Automaton const& automaton, boost::any const& data) override {
                    info.nrLocations += automaton.getNumberOfLocations();
                    info.stateDomainSize *= automaton.getNumberOfLocations();
                    domainSizesSum += automaton.getNumberOfLocations();
                    info.nrEdges += automaton.getNumberOfEdges();
                    ConstJaniTraverser::traverse(automaton, data);
                }
                
                virtual void traverse(VariableSet const& variableSet, boost::any const& data) override {
                    info.nrVariables += variableSet.getNumberOfNontransientVariables();
                    ConstJaniTraverser::traverse(variableSet, data);
                }
                
                virtual void traverse(BooleanVariable const& variable, boost::any const& data) override {
                    if (!variable.isTransient()) {
                        info.stateDomainSize *= 2;
                        domainSizesSum += 2;
                    }
                    ConstJaniTraverser::traverse(variable, data);
                }
                
                virtual void traverse(BoundedIntegerVariable const& variable, boost::any const& data) override {
                    if (!variable.isTransient()) {
                        if (variable.hasLowerBound() && variable.hasUpperBound() && !variable.getLowerBound().containsVariables() && !variable.getUpperBound().containsVariables()) {
                            info.stateDomainSize *= (variable.getUpperBound().evaluateAsInt() - variable.getLowerBound().evaluateAsInt());
                            domainSizesSum += (variable.getUpperBound().evaluateAsInt() - variable.getLowerBound().evaluateAsInt());
                        } else {
                            info.stateDomainSize = 0; // i.e. unknown
                        }
                    }
                    ConstJaniTraverser::traverse(variable, data);
                }
                
                virtual void traverse(UnboundedIntegerVariable const& variable, boost::any const& data) override {
                    if (!variable.isTransient()) {
                        info.stateDomainSize = 0; // i.e. unknown
                    }
                    
                    ConstJaniTraverser::traverse(variable, data);
                }
                
                virtual void traverse(RealVariable const& variable, boost::any const& data) override {
                    if (!variable.isTransient()) {
                        info.stateDomainSize = 0; // i.e. unknown
                    }
                    ConstJaniTraverser::traverse(variable, data);
                }
                
                virtual void traverse(ArrayVariable const& variable, boost::any const& data) override {
                    if (!variable.isTransient()) {
                        info.stateDomainSize = 0; // i.e. unknown
                    }
                    ConstJaniTraverser::traverse(variable, data);
                }
                
                virtual void traverse(ClockVariable const& variable, boost::any const& data) override {
                    if (!variable.isTransient()) {
                        info.stateDomainSize = 0; // i.e. unknown
                    }
                    ConstJaniTraverser::traverse(variable, data);
                }
                
            private:
                InformationObject info;
                uint64_t domainSizesSum;
            };
            
            
        }
        
        InformationObject::InformationObject() : nrVariables(0), nrAutomata(0),  nrEdges(0), nrLocations(0), stateDomainSize(1), avgVarDomainSize(0.0) {
            // Intentionally left empty
        }
        

        InformationObject collectModelInformation(Model const& model) {
            return detail::InformationCollector().collect(model);
        }
    }
}