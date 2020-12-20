#include "storm-conv/api/storm-conv.h"

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/prism/Program.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/Constant.h"
#include "storm/storage/jani/JaniLocationExpander.h"
#include "storm/storage/jani/JaniScopeChanger.h"
#include "storm/storage/jani/JSONExporter.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/SymbolicModelDescription.h"

#include "storm/api/storm.h"
#include "storm/api/properties.h"
#include "storm/io/file.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

extern "C" {
#include "aiger.h"
}

namespace storm {
    namespace api {
        
        void transformJani(storm::jani::Model& janiModel, std::vector<storm::jani::Property>& properties, storm::converter::JaniConversionOptions const& options) {
        
            if (options.replaceUnassignedVariablesWithConstants) {
                janiModel.replaceUnassignedVariablesWithConstants();
            }
            
            if (options.substituteConstants) {
                janiModel.substituteConstantsInPlace();
            }
            
            if (options.localVars) {
                STORM_LOG_WARN_COND(!options.globalVars, "Ignoring 'globalvars' option, since 'localvars' is also set.");
                storm::jani::JaniScopeChanger().makeVariablesLocal(janiModel, properties);
            } else if (options.globalVars) {
                storm::jani::JaniScopeChanger().makeVariablesGlobal(janiModel);
            }
            
            if (!options.locationVariables.empty()) {
                // Make variables local if necessary/possible
                for (auto const& pair : options.locationVariables) {
                    if (janiModel.hasGlobalVariable(pair.second)) {
                        auto var = janiModel.getGlobalVariable(pair.second).getExpressionVariable();
                        if (storm::jani::JaniScopeChanger().canMakeVariableLocal(var, janiModel, properties, janiModel.getAutomatonIndex(pair.first)).first) {
                            storm::jani::JaniScopeChanger().makeVariableLocal(var, janiModel, janiModel.getAutomatonIndex(pair.first));
                        } else {
                            STORM_LOG_ERROR("Can not transform variable " << pair.second << " into locations since it can not be made local to automaton " << pair.first << ".");
                        }
                    }
                }
                
                for (auto const& pair : options.locationVariables) {
                    storm::jani::JaniLocationExpander expander(janiModel);
                    expander.transform(pair.first, pair.second);
                    janiModel = expander.getResult();
                }
            }

            if (options.simplifyComposition) {
                janiModel.simplifyComposition();
            }
            
            if (options.flatten) {
                std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
                if (storm::settings::hasModule<storm::settings::modules::CoreSettings>()) {
                    smtSolverFactory = std::make_shared<storm::utility::solver::SmtSolverFactory>();
                } else {
                    smtSolverFactory = std::make_shared<storm::utility::solver::Z3SmtSolverFactory>();
                }
                janiModel = janiModel.flattenComposition(smtSolverFactory);
            }

            if (!options.edgeAssignments) {
                janiModel.pushEdgeAssignmentsToDestinations();
            }
            
            auto uneliminatedFeatures = janiModel.restrictToFeatures(options.allowedModelFeatures);
            STORM_LOG_WARN_COND(uneliminatedFeatures.empty(), "The following model features could not be eliminated: " << uneliminatedFeatures.toString());
            
            if (options.modelName) {
                janiModel.setName(options.modelName.get());
            }
            
            if (options.addPropertyConstants) {
                for (auto& f : properties) {
                    for (auto const& constant : f.getUndefinedConstants()) {
                        if (!janiModel.hasConstant(constant.getName())) {
                            janiModel.addConstant(storm::jani::Constant(constant.getName(), constant));
                        }
                    }
                }
            }
            
        }
        
        void transformPrism(storm::prism::Program& prismProgram, std::vector<storm::jani::Property>& properties, bool simplify, bool flatten) {
            if (simplify) {
                prismProgram = prismProgram.simplify().simplify();
                properties = storm::api::substituteConstantsInProperties(properties, prismProgram.getConstantsFormulasSubstitution());
            }
            if (flatten) {
               prismProgram = prismProgram.flattenModules();
               if (simplify) {
                    // Let's simplify the flattened program again ... just to be sure ... twice ...
                    prismProgram = prismProgram.simplify().simplify();
               }
            }
        }

        static inline unsigned var2lit(uint64_t var) {
            unsigned lit = (unsigned) ((var + 1) * 2);
            return lit;
        }

        static unsigned bdd2lit(sylvan::Bdd const& b, aiger* aig, unsigned& maxvar) {
            if (b.isOne())
                return 1;
            if (b.isZero())
                return 0;
            // otherwise we need to use Shannon expansion and build
            // subcircuits
            uint32_t idx = b.TopVar();
            sylvan::Bdd t = b.Then();
            sylvan::Bdd e = b.Else();
            unsigned tLit = bdd2lit(t, aig, maxvar);
            unsigned eLit = bdd2lit(e, aig, maxvar);
            // we have circuits for then, else, we need an and here
            unsigned thenCoFactor = var2lit(++maxvar);
            unsigned elseCoFactor = var2lit(++maxvar);
            std::cout << "adding pos cofactor of " << idx << " with lit " << thenCoFactor << std::endl;
            aiger_add_and(aig, thenCoFactor, var2lit(idx), tLit);
            std::cout << "adding neg cofactor of " << idx << " with lit " << elseCoFactor << std::endl;
            aiger_add_and(aig, elseCoFactor, aiger_not(var2lit(idx)), eLit);
            unsigned res = var2lit(++maxvar);
            std::cout << "adding shannon exp of " << idx << " with lit " << res << std::endl;
            aiger_add_and(aig, res, aiger_not(thenCoFactor), aiger_not(elseCoFactor));
            return aiger_not(res);
        }

        aiger* convertPrismToAiger(storm::prism::Program const& program, std::vector<storm::jani::Property> const & properties, storm::converter::PrismToAigerConverterOptions options) {
            // we recover BDD-style information from the prism program by
            // building its symbolic representation
            std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::Sylvan, double>> model = storm::builder::DdPrismModelBuilder<storm::dd::DdType::Sylvan, double>().build(program);
            // we can now start loading the aiger structure
            aiger* aig = aiger_init();
            // STEP 1:
            // the first thing we need is to add input variables, these come
            // from the encoding of the nondeterminism, a.k.a. actions, and
            // the encoding of successors (because of the probabilistic
            // transition function)
            for (auto const& var : model->getNondeterminismVariables()) {
                auto const& ddMetaVar = model->getManagerAsSharedPointer()->getMetaVariable(var);
                for (auto const& i : ddMetaVar.getIndices()) {
                    std::string name = var.getName() + std::to_string(i);
                    std::cout << "adding action input " << i << " with lit " << var2lit(i) << std::endl;
                    aiger_add_input(aig, var2lit(i), name.c_str());
                }
            }
            for (auto const& var : model->getColumnVariables()) {
                auto const& ddMetaVar = model->getManagerAsSharedPointer()->getMetaVariable(var);
                for (auto const& i : ddMetaVar.getIndices()) {
                    std::string name = var.getName() + std::to_string(i);
                    std::cout << "adding successor input " << i << " with lit " << var2lit(i) << std::endl;
                    aiger_add_input(aig, var2lit(i), name.c_str());
                }
            }
            // STEP 2:
            // we need to create latches per state-encoding variable
            unsigned maxvar = aig->maxvar;
            for (auto const& inVar : model->getRowVariables())
                for (unsigned const& i : model->getManagerAsSharedPointer()->getMetaVariable(inVar).getIndices())
                    maxvar = std::max(maxvar, i);
            std::cout << "maxvar before and gates = " << maxvar << std::endl;
            // we will need Boolean logic for the transition relation
            storm::dd::Bdd<storm::dd::DdType::Sylvan> qualTrans = model->getQualitativeTransitionMatrix();
            for (auto const& varPair : model->getRowColumnMetaVariablePairs()) {
                auto inVar = varPair.first;
                auto ddInMetaVar = model->getManagerAsSharedPointer()->getMetaVariable(inVar);
                auto ddOutMetaVar = model->getManagerAsSharedPointer()->getMetaVariable(varPair.second);
                auto inIndices = ddInMetaVar.getIndices();
                auto idxIt = inIndices.begin();
                for (auto const& encVar : ddOutMetaVar.getDdVariables()) {
                    assert(idxIt != inIndices.end());
                    auto encVarFun = qualTrans && encVar;
                    unsigned lit = bdd2lit(encVarFun.getInternalBdd().getSylvanBdd(), aig, maxvar);
                    unsigned idx = (unsigned)(*idxIt);
                    std::string name = inVar.getName() + std::to_string(idx);
                    std::cout << "adding latch " << idx << " with lit " << var2lit(idx) << std::endl;
                    aiger_add_latch(aig, var2lit(idx), lit, name.c_str());
                    idxIt++;
                }
            }
            // STEP 3:
            // we add a "bad" output representing when the transition is
            // invalid
            aiger_add_output(aig,
                             aiger_not(bdd2lit(qualTrans.getInternalBdd().getSylvanBdd(), aig, maxvar)),
                             "invalid_transition");
            // STEP 4:
            // add labels as outputs as a function of state sets
            std::vector<std::string> labels = model->getLabels();
            for (auto const& label : labels) {
               storm::dd::Bdd<storm::dd::DdType::Sylvan> states4label = model->getStates(label);
               unsigned lit = bdd2lit(states4label.getInternalBdd().getSylvanBdd(), aig, maxvar);
               std::cout << "adding output " << label << " with lit " << lit << std::endl;
               aiger_add_output(aig, lit, label.c_str());
            }

            // TODO: how do we make the initial states explicit and not
            // default to 0-values of the latches?
            // for init states, use storm::dd::Bdd<storm::dd::DdType::Sylvan> initStates = model->getInitialStates();
            const char* check = aiger_check(aig);
            if (check != NULL)
                std::cout << "aiger check result: " << check << std::endl;
            return aig;
        }
        
        std::pair<storm::jani::Model, std::vector<storm::jani::Property>> convertPrismToJani(storm::prism::Program const& program, std::vector<storm::jani::Property> const& properties, storm::converter::PrismToJaniConverterOptions options) {
        
            // Perform conversion
            auto res = program.toJani(properties, options.allVariablesGlobal);
            if (res.second.empty()) {
                std::vector<storm::jani::Property> clonedProperties;
                for (auto const& p : properties) {
                    clonedProperties.push_back(p.clone());
                }
                res.second = std::move(clonedProperties);
            }
            
            // Postprocess Jani model based on the options
            transformJani(res.first, res.second, options.janiOptions);
            
            return res;
        }

        void exportJaniToFile(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filename, bool compact) {
            storm::jani::JsonExporter::toFile(model, properties, filename, true, compact);
        }
        
        void printJaniToStream(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::ostream& ostream, bool compact) {
            storm::jani::JsonExporter::toStream(model, properties, ostream, true, compact);
        }
        
        void exportPrismToFile(storm::prism::Program const& program, std::vector<storm::jani::Property> const& properties, std::string const& filename) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            stream << program << std::endl;
            storm::utility::closeFile(stream);
            
            if (!properties.empty()) {
                storm::utility::openFile(filename + ".props", stream);
                for (auto const& prop : properties) {
                    stream << prop.asPrismSyntax() << std::endl;
                    STORM_LOG_WARN_COND(!prop.containsUndefinedConstants(), "A property contains undefined constants. These might not be exported correctly.");
                }
                storm::utility::closeFile(stream);
            }
        }
        void printPrismToStream(storm::prism::Program const& program, std::vector<storm::jani::Property> const& properties, std::ostream& ostream) {
            ostream << program << std::endl;
            for (auto const& prop : properties) {
                STORM_LOG_WARN_COND(!prop.containsUndefinedConstants(), "A property contains undefined constants. These might not be exported correctly.");
                ostream << prop.asPrismSyntax() << std::endl;
            }
        }
        
    }
}
