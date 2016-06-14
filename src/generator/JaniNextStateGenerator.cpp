#include "src/generator/JaniNextStateGenerator.h"

#include "src/models/sparse/StateLabeling.h"

#include "src/storage/expressions/SimpleValuation.h"

#include "src/solver/SmtSolver.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/utility/solver.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace generator {
     
        template<typename ValueType, typename StateType>
        JaniNextStateGenerator<ValueType, StateType>::JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options) : JaniNextStateGenerator(model.substituteConstants(), options, false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        JaniNextStateGenerator<ValueType, StateType>::JaniNextStateGenerator(storm::jani::Model const& model, NextStateGeneratorOptions const& options, bool flag) : NextStateGenerator<ValueType, StateType>(model.getExpressionManager(), VariableInformation(model), options), model(model) {
            STORM_LOG_THROW(!this->model.hasDefaultComposition(), storm::exceptions::WrongFormatException, "The explicit next-state generator currently does not support custom system compositions.");
            STORM_LOG_THROW(!this->options.isBuildAllRewardModelsSet() && this->options.getRewardModelNames().empty(), storm::exceptions::InvalidSettingsException, "The explicit next-state generator currently does not support building reward models.");
            
            // If there are terminal states we need to handle, we now need to translate all labels to expressions.
            if (this->options.hasTerminalStates()) {
                for (auto const& expressionOrLabelAndBool : this->options.getTerminalStates()) {
                    if (expressionOrLabelAndBool.first.isExpression()) {
                        this->terminalStates.push_back(std::make_pair(expressionOrLabelAndBool.first.getExpression(), expressionOrLabelAndBool.second));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Cannot make label terminal for JANI models.");
                    }
                }
            }
        }
        
        template<typename ValueType, typename StateType>
        ModelType JaniNextStateGenerator<ValueType, StateType>::getModelType() const {
            switch (model.getModelType()) {
                case storm::jani::ModelType::DTMC: return ModelType::DTMC;
                case storm::jani::ModelType::CTMC: return ModelType::CTMC;
                case storm::jani::ModelType::MDP: return ModelType::MDP;
                case storm::jani::ModelType::MA: return ModelType::MA;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Invalid model type.");
            }
        }
        
        template<typename ValueType, typename StateType>
        bool JaniNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
            return model.isDeterministicModel();
        }

        template<typename ValueType, typename StateType>
        std::vector<StateType> JaniNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
            // Prepare an SMT solver to enumerate all initial states.
            storm::utility::solver::SmtSolverFactory factory;
            std::unique_ptr<storm::solver::SmtSolver> solver = factory.create(model.getExpressionManager());
            
            std::vector<storm::expressions::Expression> rangeExpressions = model.getAllRangeExpressions();
            for (auto const& expression : rangeExpressions) {
                solver->add(expression);
            }
            solver->add(model.getInitialStatesExpression(true));
            
            // Proceed ss long as the solver can still enumerate initial states.
            std::vector<StateType> initialStateIndices;
            while (solver->check() == storm::solver::SmtSolver::CheckResult::Sat) {
                // Create fresh state.
                CompressedState initialState(this->variableInformation.getTotalBitOffset());
                
                // Read variable assignment from the solution of the solver. Also, create an expression we can use to
                // prevent the variable assignment from being enumerated again.
                storm::expressions::Expression blockingExpression;
                std::shared_ptr<storm::solver::SmtSolver::ModelReference> model = solver->getModel();
                for (auto const& booleanVariable : this->variableInformation.booleanVariables) {
                    bool variableValue = model->getBooleanValue(booleanVariable.variable);
                    storm::expressions::Expression localBlockingExpression = variableValue ? !booleanVariable.variable : booleanVariable.variable;
                    blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                    initialState.set(booleanVariable.bitOffset, variableValue);
                }
                for (auto const& integerVariable : this->variableInformation.integerVariables) {
                    int_fast64_t variableValue = model->getIntegerValue(integerVariable.variable);
                    storm::expressions::Expression localBlockingExpression = integerVariable.variable != model->getManager().integer(variableValue);
                    blockingExpression = blockingExpression.isInitialized() ? blockingExpression || localBlockingExpression : localBlockingExpression;
                    initialState.setFromInt(integerVariable.bitOffset, integerVariable.bitWidth, static_cast<uint_fast64_t>(variableValue - integerVariable.lowerBound));
                }
                
                // Register initial state and return it.
                StateType id = stateToIdCallback(initialState);
                initialStateIndices.push_back(id);
                
                // Block the current initial state to search for the next one.
                solver->add(blockingExpression);
            }
            
            return initialStateIndices;
        }

        template<typename ValueType, typename StateType>
        CompressedState JaniNextStateGenerator<ValueType, StateType>::applyUpdate(CompressedState const& state, storm::jani::EdgeDestination const& destination) {
            CompressedState newState(state);
            
            auto assignmentIt = destination.getAssignments().begin();
            auto assignmentIte = destination.getAssignments().end();
            
            // Iterate over all boolean assignments and carry them out.
            auto boolIt = this->variableInformation.booleanVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getAssignedExpression().hasBooleanType(); ++assignmentIt) {
                while (assignmentIt->getExpressionVariable() != boolIt->variable) {
                    ++boolIt;
                }
                newState.set(boolIt->bitOffset, this->evaluator.asBool(assignmentIt->getExpressionVariable()));
            }
            
            // Iterate over all integer assignments and carry them out.
            auto integerIt = this->variableInformation.integerVariables.begin();
            for (; assignmentIt != assignmentIte && assignmentIt->getAssignedExpression().hasIntegerType(); ++assignmentIt) {
                while (assignmentIt->getExpressionVariable() != integerIt->variable) {
                    ++integerIt;
                }
                int_fast64_t assignedValue = this->evaluator.asInt(assignmentIt->getAssignedExpression());
                STORM_LOG_THROW(assignedValue <= integerIt->upperBound, storm::exceptions::WrongFormatException, "The update " << assignmentIt->getExpressionVariable().getName() << " := " << assignmentIt->getAssignedExpression() << " leads to an out-of-bounds value (" << assignedValue << ") for the variable '" << assignmentIt->getExpressionVariable().getName() << "'.");
                newState.setFromInt(integerIt->bitOffset, integerIt->bitWidth, assignedValue - integerIt->lowerBound);
                STORM_LOG_ASSERT(static_cast<int_fast64_t>(newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth)) + integerIt->lowerBound == assignedValue, "Writing to the bit vector bucket failed (read " << newState.getAsInt(integerIt->bitOffset, integerIt->bitWidth) << " but wrote " << assignedValue << ").");
            }
            
            // Check that we processed all assignments.
            STORM_LOG_ASSERT(assignmentIt == assignmentIte, "Not all assignments were consumed.");
            
            return newState;
        }
        
        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> JaniNextStateGenerator<ValueType, StateType>::expand(StateToIdCallback const& stateToIdCallback) {
            
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> JaniNextStateGenerator<ValueType, StateType>::getSilentActionChoices(CompressedState const& state, StateToIdCallback stateToIdCallback) {
            
        }
        
        template<typename ValueType, typename StateType>
        std::vector<Choice<ValueType>> JaniNextStateGenerator<ValueType, StateType>::getNonsilentActionChoices(CompressedState const& state, StateToIdCallback stateToIdCallback) {
            
        }
        
        template<typename ValueType, typename StateType>
        std::size_t JaniNextStateGenerator<ValueType, StateType>::getNumberOfRewardModels() const {
            return 0;
        }
        
        template<typename ValueType, typename StateType>
        RewardModelInformation JaniNextStateGenerator<ValueType, StateType>::getRewardModelInformation(uint64_t const& index) const {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Cannot retrieve reward model information.");
            return RewardModelInformation("", false, false, false);
        }
        
        template<typename ValueType, typename StateType>
        storm::models::sparse::StateLabeling JaniNextStateGenerator<ValueType, StateType>::label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices) {
            return NextStateGenerator<ValueType, StateType>::label(states, initialStateIndices, {});
        }

        template class JaniNextStateGenerator<double>;
        template class JaniNextStateGenerator<storm::RationalFunction>;
        
    }
}