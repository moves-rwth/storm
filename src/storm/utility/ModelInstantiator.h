#ifndef STORM_UTILITY_MODELINSTANTIATOR_H
#define	STORM_UTILITY_MODELINSTANTIATOR_H

#include <unordered_map>
#include <memory>
#include <type_traits>

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StochasticTwoPlayerGame.h"
#include "storm/utility/parametric.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace utility{
        
        
        /*!
         * This class allows efficient instantiation of the given parametric model.
         * The key to efficiency is to evaluate every distinct transition- (or reward-) function only once
         * instead of evaluating the same function for each occurrence in the model. 
         */
        template<typename ParametricSparseModelType, typename ConstantSparseModelType>
            class ModelInstantiator {
            public:
                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename storm::utility::parametric::VariableType<ParametricType>::type VariableType;
                typedef typename storm::utility::parametric::CoefficientType<ParametricType>::type CoefficientType;
                typedef typename ConstantSparseModelType::ValueType ConstantType;
                
                /*!
                 * Constructs a ModelInstantiator
                 * @param parametricModel The model that is to be instantiated
                 */
                ModelInstantiator(ParametricSparseModelType const& parametricModel);
                
                /*!
                 * Destructs the ModelInstantiator
                 */
                virtual ~ModelInstantiator();

                /*!
                 * Evaluates the occurring parametric functions and retrieves the instantiated model
                 * @param valuation Maps each occurring variables to the value with which it should be substituted
                 * @return The instantiated model
                 */
                ConstantSparseModelType const& instantiate(storm::utility::parametric::Valuation<ParametricType> const& valuation);
                
                /*!
                 *  Check validity
                 */
                void checkValid() const;
            private:
                /*!
                 * Initializes the instantiatedModel with dummy data by considering the model-specific ingredients.
                 * Also initializes other model-specific data, e.g., the exitRate vector of a markov automaton
                 */
                template<typename PMT = ParametricSparseModelType>
                typename std::enable_if<
                            std::is_same<PMT,storm::models::sparse::Dtmc<typename ParametricSparseModelType::ValueType>>::value ||
                            std::is_same<PMT,storm::models::sparse::Mdp<typename ParametricSparseModelType::ValueType>>::value
                >::type
                initializeModelSpecificData(PMT const& parametricModel) {
                    auto stateLabelingCopy = parametricModel.getStateLabeling();
                    auto choiceLabelingCopy = parametricModel.getOptionalChoiceLabeling();
                    this->instantiatedModel = std::make_shared<ConstantSparseModelType>(buildDummyMatrix(parametricModel.getTransitionMatrix()), std::move(stateLabelingCopy), buildDummyRewardModels(parametricModel.getRewardModels()), std::move(choiceLabelingCopy));
                }

                template<typename PMT = ParametricSparseModelType>
                typename std::enable_if<
                std::is_same<PMT,storm::models::sparse::Ctmc<typename ParametricSparseModelType::ValueType>>::value
                >::type
                initializeModelSpecificData(PMT const& parametricModel) {
                    auto stateLabelingCopy = parametricModel.getStateLabeling();
                    auto choiceLabelingCopy = parametricModel.getOptionalChoiceLabeling();
                    std::vector<ConstantType> exitRates(parametricModel.getExitRateVector().size(), storm::utility::one<ConstantType>());
                    this->instantiatedModel = std::make_shared<ConstantSparseModelType>(buildDummyMatrix(parametricModel.getTransitionMatrix()), std::move(exitRates), std::move(stateLabelingCopy), buildDummyRewardModels(parametricModel.getRewardModels()), std::move(choiceLabelingCopy));

                    initializeVectorMapping(this->instantiatedModel->getExitRateVector(), this->functions, this->vectorMapping, parametricModel.getExitRateVector());
                }


                template<typename PMT = ParametricSparseModelType>
                typename std::enable_if<
                            std::is_same<PMT,storm::models::sparse::MarkovAutomaton<typename ParametricSparseModelType::ValueType>>::value
                >::type
                initializeModelSpecificData(PMT const& parametricModel) {
                    auto stateLabelingCopy = parametricModel.getStateLabeling();
                    auto markovianStatesCopy = parametricModel.getMarkovianStates();
                    auto choiceLabelingCopy = parametricModel.getOptionalChoiceLabeling();
                    std::vector<ConstantType> exitRates(parametricModel.getExitRates().size(), storm::utility::one<ConstantType>());
                    this->instantiatedModel = std::make_shared<ConstantSparseModelType>(buildDummyMatrix(parametricModel.getTransitionMatrix()), std::move(stateLabelingCopy), std::move(markovianStatesCopy), std::move(exitRates), true, buildDummyRewardModels(parametricModel.getRewardModels()), std::move(choiceLabelingCopy));
                    
                    initializeVectorMapping(this->instantiatedModel->getExitRates(), this->functions, this->vectorMapping, parametricModel.getExitRates());
                }
                
                template<typename PMT = ParametricSparseModelType>
                typename std::enable_if<
                            std::is_same<PMT,storm::models::sparse::StochasticTwoPlayerGame<typename ParametricSparseModelType::ValueType>>::value
                >::type
                initializeModelSpecificData(PMT const& parametricModel) {
                    auto player1MatrixCopy = parametricModel.getPlayer1Matrix();
                    auto stateLabelingCopy = parametricModel.getStateLabeling();
                    boost::optional<storm::models::sparse::ChoiceLabeling> player1ChoiceLabeling, player2ChoiceLabeling;
                    if(parametricModel.hasPlayer1ChoiceLabeling()) player1ChoiceLabeling = parametricModel.getPlayer1ChoiceLabeling();
                    if(parametricModel.hasPlayer2ChoiceLabeling()) player2ChoiceLabeling = parametricModel.getPlayer2ChoiceLabeling();
                    this->instantiatedModel = std::make_shared<ConstantSparseModelType>(std::move(player1MatrixCopy), buildDummyMatrix(parametricModel.getTransitionMatrix()), std::move(stateLabelingCopy), buildDummyRewardModels(parametricModel.getRewardModels()), std::move(player1ChoiceLabeling), std::move(player2ChoiceLabeling));
                }
                
                /*!
                 * Creates a matrix that has entries at the same position as the given matrix.
                 * The returned matrix is a stochastic matrix, i.e., the rows sum up to one.
                 */
                storm::storage::SparseMatrix<ConstantType> buildDummyMatrix(storm::storage::SparseMatrix<ParametricType> const& parametricMatrix) const;
                
                /*!
                 * Creates a copy of the given reward models with the same names and with state(action)rewards / transitionrewards having the same entry-count and entry-positions.
                 */
                std::unordered_map<std::string, typename ConstantSparseModelType::RewardModelType> buildDummyRewardModels(std::unordered_map<std::string, typename ParametricSparseModelType::RewardModelType> const& parametricRewardModel) const;
                
                /*!
                 * Connects the occurring functions with the corresponding matrix entries
                 * 
                 * @note constantMatrix and parametricMatrix should have entries at the same positions
                 * 
                 * @param constantMatrix The matrix to which the evaluation results are written
                 * @param functions Occurring functions are inserted in this map
                 * @param mapping The connections of functions to matrix entries are push_backed  into this
                 * @param parametricMatrix the source matrix with the functions to consider.
                 */
                void initializeMatrixMapping(storm::storage::SparseMatrix<ConstantType>& constantMatrix,
                                             std::unordered_map<ParametricType, ConstantType>& functions,
                                             std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType*>>& mapping,
                                             storm::storage::SparseMatrix<ParametricType> const& parametricMatrix) const;
                
                /*!
                 * Connects the occurring functions with the corresponding vector entries
                 * 
                 * @note constantVector and parametricVector should have the same size
                 * 
                 * @param constantVector The vector to which the evaluation results are written
                 * @param functions Occurring functions with their placeholders are inserted in this map
                 * @param mapping The connections of functions to vector entries are push_backed  into this
                 * @param parametricVector the source vector with the functions to consider.
                 */
                void initializeVectorMapping(std::vector<ConstantType>& constantVector,
                                             std::unordered_map<ParametricType, ConstantType>& functions,
                                             std::vector<std::pair<typename std::vector<ConstantType>::iterator, ConstantType*>>& mapping,
                                             std::vector<ParametricType> const& parametricVector) const;
                
                /// The resulting model
                std::shared_ptr<ConstantSparseModelType> instantiatedModel;
                /// the occurring functions together with the corresponding placeholders for their evaluated result
                std::unordered_map<ParametricType, ConstantType> functions; 
                /// Connection of matrix entries with placeholders
                std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType*>> matrixMapping; 
                /// Connection of Vector entries with placeholders
                std::vector<std::pair<typename std::vector<ConstantType>::iterator, ConstantType*>> vectorMapping; 
                
                
            };
    }//Namespace utility
} //namespace storm
#endif	/* STORM_UTILITY_MODELINSTANTIATOR_H */

