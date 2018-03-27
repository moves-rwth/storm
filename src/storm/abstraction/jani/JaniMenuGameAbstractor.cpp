#include "storm/abstraction/jani/JaniMenuGameAbstractor.h"

#include "storm/abstraction/BottomStateResult.h"
#include "storm/abstraction/GameBddResult.h"
#include "storm/abstraction/ExpressionTranslator.h"

#include "storm/storage/BitVector.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/Edge.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"

#include "storm/utility/Stopwatch.h"
#include "storm/utility/dd.h"
#include "storm/utility/macros.h"
#include "storm/utility/solver.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm-config.h"
#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace abstraction {
        namespace jani {
            
            using storm::settings::modules::AbstractionSettings;
            
            template <storm::dd::DdType DdType, typename ValueType>
            JaniMenuGameAbstractor<DdType, ValueType>::JaniMenuGameAbstractor(storm::jani::Model const& model, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : model(model), smtSolverFactory(smtSolverFactory), abstractionInformation(model.getManager(), model.getAllExpressionVariables(), smtSolverFactory->create(model.getManager())), automata(), initialStateAbstractor(abstractionInformation, {model.getInitialStatesExpression()}, this->smtSolverFactory), validBlockAbstractor(abstractionInformation, smtSolverFactory), currentGame(nullptr), refinementPerformed(true) {
                
                // Check whether the model is linear as the abstraction requires this.
                STORM_LOG_THROW(model.isLinear(), storm::exceptions::WrongFormatException, "Cannot create abstract model from non-linear model.");
                
                // For now, we assume that there is a single module. If the program has more than one module, it needs
                // to be flattened before the procedure.
                STORM_LOG_THROW(model.getNumberOfAutomata() == 1, storm::exceptions::WrongFormatException, "Cannot create abstract model from program containing more than one automaton.");
                
                // Add all variables range expressions to the information object.
                for (auto const& range : this->model.get().getAllRangeExpressions()) {
                    abstractionInformation.addConstraint(range);
                    initialStateAbstractor.constrain(range);
                }
                
                uint_fast64_t totalNumberOfCommands = 0;
                uint_fast64_t maximalUpdateCount = 0;
                std::vector<storm::expressions::Expression> allGuards;
                for (auto const& automaton : model.getAutomata()) {
                    for (auto const& edge : automaton.getEdges()) {
                        maximalUpdateCount = std::max(maximalUpdateCount, static_cast<uint_fast64_t>(edge.getNumberOfDestinations()));
                    }
                    
                    totalNumberOfCommands += automaton.getNumberOfEdges();
                }
                
                // NOTE: currently we assume that 64 player 2 variables suffice, which corresponds to 2^64 possible
                // choices. If for some reason this should not be enough, we could grow this vector dynamically, but
                // odds are that it's impossible to treat such models in any event.
                abstractionInformation.createEncodingVariables(static_cast<uint_fast64_t>(std::ceil(std::log2(totalNumberOfCommands))), 64, static_cast<uint_fast64_t>(std::ceil(std::log2(maximalUpdateCount))));
                
                // For each module of the concrete program, we create an abstract counterpart.
                bool useDecomposition = storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isUseDecompositionSet();
                for (auto const& automaton : model.getAutomata()) {
                    automata.emplace_back(automaton, abstractionInformation, this->smtSolverFactory, useDecomposition);
                }
                
                // Retrieve global BDDs/ADDs so we can multiply them in the abstraction process.
                initialLocationsBdd = automata.front().getInitialLocationsBdd();
                edgeDecoratorAdd = automata.front().getEdgeDecoratorAdd();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void JaniMenuGameAbstractor<DdType, ValueType>::refine(RefinementCommand const& command) {
                // Add the predicates to the global list of predicates and gather their indices.
                std::vector<uint_fast64_t> predicateIndices;
                for (auto const& predicate : command.getPredicates()) {
                    STORM_LOG_THROW(predicate.hasBooleanType(), storm::exceptions::InvalidArgumentException, "Expecting a predicate of type bool.");
                    predicateIndices.push_back(abstractionInformation.getOrAddPredicate(predicate));
                }
                
                // Refine all abstract automata.
                for (auto& automaton : automata) {
                    automaton.refine(predicateIndices);
                }
                
                // Refine initial state abstractor.
                initialStateAbstractor.refine(predicateIndices);
                
                // Refine the valid blocks.
                validBlockAbstractor.refine(predicateIndices);

                refinementPerformed |= !command.getPredicates().empty();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            MenuGame<DdType, ValueType> JaniMenuGameAbstractor<DdType, ValueType>::abstract() {
                if (refinementPerformed) {
                    currentGame = buildGame();
                    refinementPerformed = true;
                }
                return *currentGame;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionInformation<DdType> const& JaniMenuGameAbstractor<DdType, ValueType>::getAbstractionInformation() const {
                return abstractionInformation;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Expression const& JaniMenuGameAbstractor<DdType, ValueType>::getGuard(uint64_t player1Choice) const {
                return automata.front().getGuard(player1Choice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            uint64_t JaniMenuGameAbstractor<DdType, ValueType>::getNumberOfUpdates(uint64_t player1Choice) const {
                return automata.front().getNumberOfUpdates(player1Choice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::map<storm::expressions::Variable, storm::expressions::Expression> JaniMenuGameAbstractor<DdType, ValueType>::getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const {
                return automata.front().getVariableUpdates(player1Choice, auxiliaryChoice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<uint64_t, uint64_t> JaniMenuGameAbstractor<DdType, ValueType>::getPlayer1ChoiceRange() const {
                return std::make_pair(0, automata.front().getNumberOfEdges());
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Expression JaniMenuGameAbstractor<DdType, ValueType>::getInitialExpression() const {
                return model.get().getInitialStatesExpression({model.get().getAutomaton(0)});
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> JaniMenuGameAbstractor<DdType, ValueType>::getStates(storm::expressions::Expression const& expression) {
                storm::abstraction::ExpressionTranslator<DdType> translator(abstractionInformation, smtSolverFactory->create(abstractionInformation.getExpressionManager()));
                return translator.translate(expression);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<MenuGame<DdType, ValueType>> JaniMenuGameAbstractor<DdType, ValueType>::buildGame() {
                // As long as there is only one module, we only build its game representation.
                GameBddResult<DdType> game = automata.front().abstract();
                
                // Add the locations to the transitions.
                game.bdd &= edgeDecoratorAdd.notZero();
                
                // Construct a set of all unnecessary variables, so we can abstract from it.
                std::set<storm::expressions::Variable> variablesToAbstract(abstractionInformation.getPlayer1VariableSet(abstractionInformation.getPlayer1VariableCount()));
                auto player2Variables = abstractionInformation.getPlayer2VariableSet(game.numberOfPlayer2Variables);
                variablesToAbstract.insert(player2Variables.begin(), player2Variables.end());
                auto auxVariables = abstractionInformation.getAuxVariableSet(0, abstractionInformation.getAuxVariableCount());
                variablesToAbstract.insert(auxVariables.begin(), auxVariables.end());
                
                storm::utility::Stopwatch relevantStatesWatch(true);
                storm::dd::Bdd<DdType> nonTerminalStates = this->abstractionInformation.getDdManager().getBddOne();
                if (this->isRestrictToRelevantStatesSet()) {
                    // Compute which states are non-terminal.
                    for (auto const& expression : this->terminalStateExpressions) {
                        nonTerminalStates &= !this->getStates(expression);
                    }
                    if (this->hasTargetStateExpression()) {
                        nonTerminalStates &= !this->getStates(this->getTargetStateExpression());
                    }
                }
                relevantStatesWatch.stop();

                // Do a reachability analysis on the raw transition relation.
                storm::dd::Bdd<DdType> transitionRelation = nonTerminalStates && game.bdd.existsAbstract(variablesToAbstract);
                storm::dd::Bdd<DdType> initialStates = initialLocationsBdd && initialStateAbstractor.getAbstractStates();
                initialStates.addMetaVariables(abstractionInformation.getSourcePredicateVariables());
                storm::dd::Bdd<DdType> reachableStates = storm::utility::dd::computeReachableStates(initialStates, transitionRelation, abstractionInformation.getSourceVariables(), abstractionInformation.getSuccessorVariables());
                
                relevantStatesWatch.start();
                if (this->isRestrictToRelevantStatesSet() && this->hasTargetStateExpression()) {
                    // Cut transition relation to the reachable states for backward search.
                    transitionRelation &= reachableStates;
                    
                    // Get the target state BDD.
                    storm::dd::Bdd<DdType> targetStates = reachableStates && this->getStates(this->getTargetStateExpression());
                    
                    // In the presence of target states, we keep only states that can reach the target states.
                    reachableStates = storm::utility::dd::computeBackwardsReachableStates(targetStates, reachableStates && !initialStates, transitionRelation, abstractionInformation.getSourceVariables(), abstractionInformation.getSuccessorVariables()) || initialStates;
                    
                    // Cut the transition relation to the 'extended backward reachable states', so we have the appropriate self-
                    // loops of (now) deadlock states.
                    transitionRelation &= reachableStates;
                    
                    // Include all successors of reachable states, because the backward search otherwise potentially
                    // cuts probability 0 choices of these states.
                    reachableStates |= reachableStates.relationalProduct(transitionRelation, abstractionInformation.getSourceVariables(), abstractionInformation.getSuccessorVariables());
                    relevantStatesWatch.stop();
                    
                    STORM_LOG_TRACE("Restricting to relevant states took " << relevantStatesWatch.getTimeInMilliseconds() << "ms.");
                }
                
                // Find the deadlock states in the model. Note that this does not find the 'deadlocks' in bottom states,
                // as the bottom states are not contained in the reachable states.
                storm::dd::Bdd<DdType> deadlockStates = transitionRelation.existsAbstract(abstractionInformation.getSuccessorVariables());
                deadlockStates = reachableStates && !deadlockStates;
                
                // If there are deadlock states, we fix them now.
                storm::dd::Add<DdType, ValueType> deadlockTransitions = abstractionInformation.getDdManager().template getAddZero<ValueType>();
                if (!deadlockStates.isZero()) {
                    deadlockTransitions = (deadlockStates && abstractionInformation.getAllPredicateIdentities() && abstractionInformation.getAllLocationIdentities() && abstractionInformation.encodePlayer1Choice(0, abstractionInformation.getPlayer1VariableCount()) && abstractionInformation.encodePlayer2Choice(0, 0, game.numberOfPlayer2Variables) && abstractionInformation.encodeAux(0, 0, abstractionInformation.getAuxVariableCount())).template toAdd<ValueType>();
                }
                
                // Compute bottom states and the appropriate transitions if necessary.
                BottomStateResult<DdType> bottomStateResult(abstractionInformation.getDdManager().getBddZero(), abstractionInformation.getDdManager().getBddZero());
                bottomStateResult = automata.front().getBottomStateTransitions(reachableStates, game.numberOfPlayer2Variables);
                bool hasBottomStates = !bottomStateResult.states.isZero();
                
                // Construct the transition matrix by cutting away the transitions of unreachable states.
                storm::dd::Add<DdType, ValueType> transitionMatrix = (game.bdd && reachableStates).template toAdd<ValueType>();
                transitionMatrix *= edgeDecoratorAdd;
                transitionMatrix += deadlockTransitions;
                
                // Extend the current game information with the 'non-bottom' tag before potentially adding bottom state transitions.
                transitionMatrix *= (abstractionInformation.getBottomStateBdd(true, true) && abstractionInformation.getBottomStateBdd(false, true)).template toAdd<ValueType>();
                reachableStates &= abstractionInformation.getBottomStateBdd(true, true);
                initialStates &= abstractionInformation.getBottomStateBdd(true, true);
                
                // If there are bottom transitions, exnted the transition matrix and reachable states now.
                if (hasBottomStates) {
                    transitionMatrix += bottomStateResult.transitions.template toAdd<ValueType>();
                    reachableStates |= bottomStateResult.states;
                }
                
                std::set<storm::expressions::Variable> allNondeterminismVariables = player2Variables;
                allNondeterminismVariables.insert(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end());
                
                std::set<storm::expressions::Variable> allSourceVariables(abstractionInformation.getSourceVariables());
                allSourceVariables.insert(abstractionInformation.getBottomStateVariable(true));
                std::set<storm::expressions::Variable> allSuccessorVariables(abstractionInformation.getSuccessorVariables());
                allSuccessorVariables.insert(abstractionInformation.getBottomStateVariable(false));
                
                return std::make_unique<MenuGame<DdType, ValueType>>(abstractionInformation.getDdManagerAsSharedPointer(), reachableStates, initialStates, abstractionInformation.getDdManager().getBddZero(), transitionMatrix, bottomStateResult.states, allSourceVariables, allSuccessorVariables, abstractionInformation.getExtendedSourceSuccessorVariablePairs(), std::set<storm::expressions::Variable>(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end()), player2Variables, allNondeterminismVariables, auxVariables, abstractionInformation.getPredicateToBddMap());
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void JaniMenuGameAbstractor<DdType, ValueType>::exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStates, storm::dd::Bdd<DdType> const& filter) const {
                this->exportToDot(*currentGame, filename, highlightStates, filter);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            uint64_t JaniMenuGameAbstractor<DdType, ValueType>::getNumberOfPredicates() const {
                return abstractionInformation.getNumberOfPredicates();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void JaniMenuGameAbstractor<DdType, ValueType>::addTerminalStates(storm::expressions::Expression const& expression) {
                terminalStateExpressions.emplace_back(expression);
            }
                        
            // Explicitly instantiate the class.
            template class JaniMenuGameAbstractor<storm::dd::DdType::CUDD, double>;
            template class JaniMenuGameAbstractor<storm::dd::DdType::Sylvan, double>;
#ifdef STORM_HAVE_CARL
            template class JaniMenuGameAbstractor<storm::dd::DdType::Sylvan, storm::RationalFunction>;
#endif
        }
    }
}
