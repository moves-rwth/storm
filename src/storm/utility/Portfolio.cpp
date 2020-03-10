#include "storm/utility/Portfolio.h"

#include <sstream>

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/traverser/InformationCollector.h"
#include "storm/logic/Formula.h"
#include "storm/logic/FormulaInformation.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace utility {
        
        namespace pfinternal {
            enum class PropertyType{
                Bounded,
                Unbounded,
                LongRun
            };
            
            PropertyType getPropertyType(storm::jani::Property const& property) {
                auto formulaInfo = property.getRawFormula()->info();
                if (formulaInfo.containsBoundedUntilFormula() || formulaInfo.containsNextFormula() || formulaInfo.containsCumulativeRewardFormula()) {
                    STORM_LOG_INFO("Assuming step or time bounded property:" << property);
                    return PropertyType::Bounded;
                } else if (formulaInfo.containsLongRunFormula()) {
                    STORM_LOG_INFO("Assuming Long Run property:" << property);
                    return PropertyType::LongRun;
                } else {
                    STORM_LOG_INFO("Unbounded property:" << property);
                    return PropertyType::Unbounded;
                }
            }
            
            struct Features {
                Features(storm::jani::Model const& model, storm::jani::Property const& property) {
                    continuousTime = !model.isDiscreteTimeModel();
                    nondeterminism = !model.isDeterministicModel();
                    propertyType = getPropertyType(property);
                    auto modelInfo = model.getModelInformation();
                    numVariables = modelInfo.nrVariables;
                    numEdges = modelInfo.nrEdges;
                    numAutomata = modelInfo.nrAutomata;
                    stateDomainSize = modelInfo.stateDomainSize;
                    avgDomainSize = modelInfo.avgVarDomainSize;
                    stateEstimate = 0;
                }
                
                std::string toString() const {
                    std::stringstream str;
                    str << std::boolalpha << "continuous-time=" << continuousTime << "\tnondeterminism=" << nondeterminism << "\tpropertytype=";
                    switch (propertyType) {
                        case PropertyType::Unbounded:
                            str << "unbounded";
                            break;
                        case PropertyType::Bounded:
                            str << "bounded";
                            break;
                        case PropertyType::LongRun:
                            str << "longrun";
                            break;
                    }
                    str << "\tnumVariables=" << numVariables;
                    str << "\tnumEdges=" << numEdges;
                    str << "\tnumAutomata=" << numAutomata;
                    if (stateDomainSize > 0) {
                        str << "\tstateDomainSize=" << stateDomainSize;
                    }
                    if (avgDomainSize > 0.0) {
                        str << "\tavgDomainSize=" << avgDomainSize;
                    }
                    if (stateEstimate > 0) {
                        str << "\tstateEstimate=" << stateEstimate;
                    }
                    return str.str();
                }
                
                bool continuousTime;
                bool nondeterminism;
                PropertyType propertyType;
                uint64_t numVariables;
                uint64_t numAutomata;
                uint64_t numEdges;
                uint64_t stateDomainSize;
                uint64_t stateEstimate;
                double avgDomainSize;
            };
        }
        
        Portfolio::Portfolio() : engine(storm::utility::Engine::Unknown), useBisimulation(false), useExact(false) {
            // Intentionally left empty
        }
        
        void Portfolio::predict(storm::jani::Model const& model, storm::jani::Property const& property) {
            typedef pfinternal::PropertyType PropertyType;
            auto f = pfinternal::Features(model, property);
            STORM_LOG_INFO("Portfolio engine using features " << f.toString() << ".");
            
            { // Decision tree start
                if (f.numEdges <= 618) {
                    if (!f.continuousTime) {
                        if (f.numVariables <= 3) {
                            exact();
                        } else { // f.numVariables > 3
                            if (f.propertyType == PropertyType::Bounded) {
                                if (f.numEdges <= 25) {
                                    dd();
                                } else { // f.numEdges > 25
                                    ddbisim();
                                }
                            } else { // !(f.propertyType == PropertyType::Bounded)
                                if (f.numEdges <= 46) {
                                    if (f.numVariables <= 8) {
                                        hybrid();
                                    } else { // f.numVariables > 8
                                        if (f.numAutomata <= 1) {
                                            ddbisim();
                                        } else { // f.numAutomata > 1
                                            if (f.numVariables <= 18) {
                                                if (f.numEdges <= 36) {
                                                    if (f.numEdges <= 30) {
                                                        if (f.numAutomata <= 9) {
                                                            sparse();
                                                        } else { // f.numAutomata > 9
                                                            hybrid();
                                                        }
                                                    } else { // f.numEdges > 30
                                                        dd();
                                                    }
                                                } else { // f.numEdges > 36
                                                    ddbisim();
                                                }
                                            } else { // f.numVariables > 18
                                                hybrid();
                                            }
                                        }
                                    }
                                } else { // f.numEdges > 46
                                    if (!f.nondeterminism) {
                                        if (f.numEdges <= 92) {
                                            ddbisim();
                                        } else { // f.numEdges > 92
                                            dd();
                                        }
                                    } else { // f.nondeterminism
                                        if (f.numVariables <= 51) {
                                            if (f.numAutomata <= 6) {
                                                if (f.numAutomata <= 3) {
                                                    if (f.numEdges <= 85) {
                                                        sparse();
                                                    } else { // f.numEdges > 85
                                                        hybrid();
                                                    }
                                                } else { // f.numAutomata > 3
                                                    hybrid();
                                                }
                                            } else { // f.numAutomata > 6
                                                if (f.numAutomata <= 9) {
                                                    ddbisim();
                                                } else { // f.numAutomata > 9
                                                    hybrid();
                                                }
                                            }
                                        } else { // f.numVariables > 51
                                            sparse();
                                        }
                                    }
                                }
                            }
                        }
                    } else { // f.continuousTime
                        if (!f.nondeterminism) {
                            if (f.numAutomata <= 5) {
                                hybrid();
                            } else { // f.numAutomata > 5
                                if (f.numVariables <= 8) {
                                    sparse();
                                } else { // f.numVariables > 8
                                    if (f.numEdges <= 19) {
                                        exact();
                                    } else { // f.numEdges > 19
                                        if (f.numVariables <= 21) {
                                            hybrid();
                                        } else { // f.numVariables > 21
                                            sparse();
                                        }
                                    }
                                }
                            }
                        } else { // f.nondeterminism
                            sparse();
                        }
                    }
                } else { // f.numEdges > 618
                    sparse();
                }
            } // Decision tree end
            
        }
        
        void Portfolio::predict(storm::jani::Model const& model, storm::jani::Property const& property, uint64_t stateEstimate) {
            typedef pfinternal::PropertyType PropertyType;
            auto f = pfinternal::Features(model, property);
            f.stateEstimate = stateEstimate;
            STORM_LOG_INFO("Portfolio engine using features " << f.toString() << ".");

            // TODO: Actually make use of the estimate
            predict(model, property);
        }

        storm::utility::Engine Portfolio::getEngine() const {
            STORM_LOG_THROW(engine != storm::utility::Engine::Unknown, storm::exceptions::InvalidOperationException, "Tried to get the engine but apparently no prediction was done before.");
            return engine;
        }
        
        bool Portfolio::enableBisimulation() const {
            return useBisimulation;
        }
        
        bool Portfolio::enableExact() const {
            return useExact;
        }
        
        void Portfolio::sparse() {
            engine = storm::utility::Engine::Sparse;
            useBisimulation = false;
            useExact = false;
        }
        
        void Portfolio::hybrid() {
            engine = storm::utility::Engine::Hybrid;
            useBisimulation = false;
            useExact = false;
        }
        
        void Portfolio::dd() {
            engine = storm::utility::Engine::Dd;
            useBisimulation = false;
            useExact = false;
        }
        
        void Portfolio::exact() {
            engine = storm::utility::Engine::Sparse;
            useBisimulation = false;
            useExact = true;
        }
        
        void Portfolio::ddbisim() {
            engine = storm::utility::Engine::DdSparse;
            useBisimulation = true;
            useExact = false;
        }
        
    }
}