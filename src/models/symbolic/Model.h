#ifndef STORM_MODELS_SYMBOLIC_MODEL_H_
#define STORM_MODELS_SYMBOLIC_MODEL_H_

#include <memory>
#include <set>
#include <boost/optional.hpp>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/Variable.h"
#include "src/adapters/DdExpressionAdapter.h"
#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/models/ModelBase.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            /*!
             * Base class for all symbolic models.
             */
            template<storm::dd::DdType Type>
            class Model : public storm::models::ModelBase {
            public:
                Model(Model<Type> const& other) = default;
                Model& operator=(Model<Type> const& other) = default;
                
#ifndef WINDOWS
                Model(Model<Type>&& other) = default;
                Model& operator=(Model<Type>&& other) = default;
#endif
                
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param modelType The type of the model.
                 * @param manager The manager responsible for the decision diagrams.
                 * @param reachableStates A DD representing the reachable states.
                 * @param initialStates A DD representing the initial states of the model.
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param rowVariables The set of row meta variables used in the DDs.
                 * @param rowExpressionAdapter An object that can be used to translate expressions in terms of the row
                 * meta variables.
                 * @param columVariables The set of column meta variables used in the DDs.
                 * @param columnExpressionAdapter An object that can be used to translate expressions in terms of the
                 * column meta variables.
                 * @param rowColumnMetaVariablePairs All pairs of row/column meta variables.
                 * @param labelToExpressionMap A mapping from label names to their defining expressions.
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 */
                Model(storm::models::ModelType const& modelType,
                      std::shared_ptr<storm::dd::DdManager<Type>> manager,
                      storm::dd::Dd<Type> reachableStates,
                      storm::dd::Dd<Type> initialStates,
                      storm::dd::Dd<Type> transitionMatrix,
                      std::set<storm::expressions::Variable> const& rowVariables,
                      std::shared_ptr<storm::adapters::DdExpressionAdapter<Type>> rowExpressionAdapter,
                      std::set<storm::expressions::Variable> const& columnVariables,
                      std::shared_ptr<storm::adapters::DdExpressionAdapter<Type>> columnExpressionAdapter,
                      std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                      std::map<std::string, storm::expressions::Expression> labelToExpressionMap = std::map<std::string, storm::expressions::Expression>(),
                      boost::optional<storm::dd::Dd<Type>> const& optionalStateRewardVector = boost::optional<storm::dd::Dd<Type>>(),
                      boost::optional<storm::dd::Dd<Type>> const& optionalTransitionRewardMatrix = boost::optional<storm::dd::Dd<Type>>());
                
                virtual uint_fast64_t getNumberOfStates() const override;
                
                virtual uint_fast64_t getNumberOfTransitions() const override;
                
                /*!
                 * Retrieves the reachable states of the model.
                 *
                 * @return The reachble states of the model.
                 */
                storm::dd::Dd<Type> const& getReachableStates() const;
                
                /*!
                 * Retrieves the initial states of the model.
                 *
                 * @return The initial states of the model.
                 */
                storm::dd::Dd<Type> const& getInitialStates() const;
                
                /*!
                 * Returns the sets of states labeled with the given label.
                 *
                 * @param label The label for which to get the labeled states.
                 * @return The set of states labeled with the requested label in the form of a bit vector.
                 */
                storm::dd::Dd<Type> getStates(std::string const& label) const;
                
                /*!
                 * Returns the set of states labeled satisfying the given expression (that must be of boolean type).
                 *
                 * @param expression The expression that needs to hold in the states.
                 * @return The set of states labeled satisfying the given expression.
                 */
                storm::dd::Dd<Type> getStates(storm::expressions::Expression const& expression) const;
                
                /*!
                 * Retrieves whether the given label is a valid label in this model.
                 *
                 * @param label The label to be checked for validity.
                 * @return True if the given label is valid in this model.
                 */
                bool hasLabel(std::string const& label) const;
                
                /*!
                 * Retrieves the matrix representing the transitions of the model.
                 *
                 * @return A matrix representing the transitions of the model.
                 */
                storm::dd::Dd<Type> const& getTransitionMatrix() const;
                
                /*!
                 * Retrieves the matrix representing the transitions of the model.
                 *
                 * @return A matrix representing the transitions of the model.
                 */
                storm::dd::Dd<Type>& getTransitionMatrix();
                
                /*!
                 * Retrieves the matrix representing the transition rewards of the model. Note that calling this method
                 * is only valid if the model has transition rewards.
                 *
                 * @return The matrix representing the transition rewards of the model.
                 */
                storm::dd::Dd<Type> const& getTransitionRewardMatrix() const;
                
                /*!
                 * Retrieves the matrix representing the transition rewards of the model. Note that calling this method
                 * is only valid if the model has transition rewards.
                 *
                 * @return The matrix representing the transition rewards of the model.
                 */
                storm::dd::Dd<Type>& getTransitionRewardMatrix();
                
                /*!
                 * Retrieves a vector representing the state rewards of the model. Note that calling this method is only
                 * valid if the model has state rewards.
                 *
                 * @return A vector representing the state rewards of the model.
                 */
                storm::dd::Dd<Type> const& getStateRewardVector() const;
                
                /*!
                 * Retrieves whether this model has state rewards.
                 *
                 * @return True iff this model has state rewards.
                 */
                bool hasStateRewards() const;
                
                /*!
                 * Retrieves whether this model has transition rewards.
                 *
                 * @return True iff this model has transition rewards.
                 */
                bool hasTransitionRewards() const;
                
                /*!
                 * Retrieves the meta variables used to encode the rows of the transition matrix and the vector indices.
                 *
                 * @return The meta variables used to encode the rows of the transition matrix and the vector indices.
                 */
                std::set<storm::expressions::Variable> const& getRowVariables() const;

                /*!
                 * Retrieves the meta variables used to encode the columns of the transition matrix and the vector indices.
                 *
                 * @return The meta variables used to encode the columns of the transition matrix and the vector indices.
                 */
                std::set<storm::expressions::Variable> const& getColumnVariables() const;
                
                virtual std::size_t getSizeInBytes() const override;
                
                virtual void printModelInformationToStream(std::ostream& out) const override;
                
            protected:
                
                /*!
                 * Sets the transition matrix of the model.
                 *
                 * @param transitionMatrix The new transition matrix of the model.
                 */
                void setTransitionMatrix(storm::dd::Dd<Type> const& transitionMatrix);
                
                /*!
                 * Retrieves the mapping of labels to their defining expressions.
                 *
                 * @returns The mapping of labels to their defining expressions.
                 */
                std::map<std::string, storm::expressions::Expression> const& getLabelToExpressionMap() const;
                
            private:
                // The manager responsible for the decision diagrams.
                std::shared_ptr<storm::dd::DdManager<Type>> manager;
                
                // A vector representing the reachable states of the model.
                storm::dd::Dd<Type> reachableStates;
                
                // A vector representing the initial states of the model.
                storm::dd::Dd<Type> initialStates;
                
                // A matrix representing transition relation.
                storm::dd::Dd<Type> transitionMatrix;
                
                // The meta variables used to encode the rows of the transition matrix.
                std::set<storm::expressions::Variable> rowVariables;
                
                // An adapter that can translate expressions to DDs over the row meta variables.
                std::shared_ptr<storm::adapters::DdExpressionAdapter<Type>> rowExpressionAdapter;
                
                // The meta variables used to encode the columns of the transition matrix.
                std::set<storm::expressions::Variable> columnVariables;
                
                // An adapter that can translate expressions to DDs over the column meta variables.
                std::shared_ptr<storm::adapters::DdExpressionAdapter<Type>> columnExpressionAdapter;
                
                // A vector holding all pairs of row and column meta variable pairs. This is used to swap the variables
                // in the DDs from row to column variables and vice versa.
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowColumnMetaVariablePairs;
                
                // A mapping from labels to expressions defining them.
                std::map<std::string, storm::expressions::Expression> labelToExpressionMap;
                
                // If set, a vector representing the rewards of the states.
                boost::optional<storm::dd::Dd<Type>> stateRewardVector;
                
                // If set, a matrix representing the rewards of transitions.
                boost::optional<storm::dd::Dd<Type>> transitionRewardMatrix;
            };
            
        } // namespace symbolic
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SYMBOLIC_MODEL_H_ */
