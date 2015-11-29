/* 
 * File:   SamplingModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:31 AM
 */

#include "src/modelchecker/region/SamplingModel.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/ModelType.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/utility/macros.h"
#include "src/utility/region.h"
#include "src/utility/solver.h"
#include "src/utility/vector.h"
#include "src/utility/policyguessing.h"
#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "storage/dd/CuddBdd.h"

namespace storm {
    namespace modelchecker {
        namespace region {
        
            template<typename ParametricSparseModelType, typename ConstantType>
            SamplingModel<ParametricSparseModelType, ConstantType>::SamplingModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula){
                //First some simple checks and initializations..
                this->typeOfParametricModel = parametricModel.getType();
                if(formula->isProbabilityOperatorFormula()){
                    this->computeRewards=false;
                    STORM_LOG_THROW(this->typeOfParametricModel==storm::models::ModelType::Dtmc || this->typeOfParametricModel==storm::models::ModelType::Mdp, storm::exceptions::InvalidArgumentException, "Sampling with probabilities is only implemented for Dtmcs and Mdps");
                } else if(formula->isRewardOperatorFormula()){
                    this->computeRewards=true;
                    STORM_LOG_THROW(this->typeOfParametricModel==storm::models::ModelType::Dtmc, storm::exceptions::InvalidArgumentException, "Sampling with rewards is only implemented for Dtmcs");
                    STORM_LOG_THROW(parametricModel.hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the sampling model should be unique");
                    STORM_LOG_THROW(parametricModel.getUniqueRewardModel()->second.hasOnlyStateRewards(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the sampling model should have state rewards only");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid formula: " << formula << ". Sampling model only supports eventually or reachability reward formulae.");
                }
                this->solverData.solveGoal = storm::solver::SolveGoal(storm::logic::isLowerBound(formula->getComparisonType()));
                STORM_LOG_THROW(parametricModel.hasLabel("target"), storm::exceptions::InvalidArgumentException, "The given Model has no \"target\"-statelabel.");
                this->targetStates = parametricModel.getStateLabeling().getStates("target");
                STORM_LOG_THROW(parametricModel.hasLabel("sink"), storm::exceptions::InvalidArgumentException, "The given Model has no \"sink\"-statelabel.");
                storm::storage::BitVector sinkStates=parametricModel.getStateLabeling().getStates("sink");
                this->maybeStates = ~(this->targetStates | sinkStates);
                STORM_LOG_THROW(parametricModel.getInitialStates().getNumberOfSetBits()==1, storm::exceptions::InvalidArgumentException, "The given model has more or less then one initial state");
                storm::storage::sparse::state_type initialState = *parametricModel.getInitialStates().begin();
                STORM_LOG_THROW(maybeStates.get(initialState), storm::exceptions::InvalidArgumentException, "The value in the initial state of the given model is independent of parameters");
                //The (state-)indices in the equation system will be different from the original ones, as the eq-sys only considers maybestates.
                //Hence, we use this vector to translate from old indices to new ones.
                std::vector<std::size_t> newIndices(parametricModel.getNumberOfStates(), parametricModel.getNumberOfStates()); //initialize with some illegal index
                std::size_t newIndex=0;
                for(auto const& index : maybeStates){
                    newIndices[index]=newIndex;
                    ++newIndex;
                }
                
                //Now pre-compute the information for the equation system.
                initializeProbabilities(parametricModel, newIndices);
                if(this->computeRewards){
                    initializeRewards(parametricModel, newIndices);
                }
                this->matrixData.assignment.shrink_to_fit();
                this->vectorData.assignment.shrink_to_fit();
                
                this->solverData.result = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), this->computeRewards ? storm::utility::one<ConstantType>() : ConstantType(0.5));
                this->solverData.initialStateIndex = newIndices[initialState];
                this->solverData.lastPolicy = Policy(this->matrixData.matrix.getRowGroupCount(), 0);
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void SamplingModel<ParametricSparseModelType, ConstantType>::initializeProbabilities(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices){
                //First run: get a matrix with dummy entries at the new positions
                ConstantType dummyValue = storm::utility::one<ConstantType>();
                bool addRowGroups = parametricModel.getTransitionMatrix().hasNontrivialRowGrouping();
                bool isDtmc = (this->typeOfParametricModel==storm::models::ModelType::Dtmc); //The equation system for DTMCs need the (I-P)-matrix (i.e., we need diagonal entries)
                auto numOfMaybeStates = this->maybeStates.getNumberOfSetBits();
                storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(addRowGroups ? parametricModel.getTransitionMatrix().getRowCount() : numOfMaybeStates,
                                                                                numOfMaybeStates,
                                                                                parametricModel.getTransitionMatrix().getEntryCount(),
                                                                                false, // no force dimensions
                                                                                addRowGroups,
                                                                                addRowGroups ? numOfMaybeStates : 0);
                std::size_t curRow = 0;
                for (auto oldRowGroup : this->maybeStates){
                    if(addRowGroups){
                        matrixBuilder.newRowGroup(curRow);
                    }
                    for (std::size_t oldRow = parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup]; oldRow < parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        bool insertedDiagonalEntry = false;
                        for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                            if(this->maybeStates.get(oldEntry.getColumn())){
                                //check if we need to insert a diagonal entry
                                if(isDtmc && !insertedDiagonalEntry && newIndices[oldEntry.getColumn()]>=newIndices[oldRowGroup]){
                                    if(newIndices[oldEntry.getColumn()]>newIndices[oldRowGroup]){
                                        // There is no diagonal entry in the original matrix.
                                        // Since we later need the matrix (I-P), we already know that the diagonal entry will be one (=1-0)
                                        matrixBuilder.addNextValue(curRow, newIndices[oldRowGroup], storm::utility::one<ConstantType>());
                                    }
                                    insertedDiagonalEntry=true;
                                }
                                //Insert dummy entry
                                matrixBuilder.addNextValue(curRow, newIndices[oldEntry.getColumn()], dummyValue);
                            }
                        }
                        if(isDtmc && !insertedDiagonalEntry){
                            // There is no diagonal entry in the original matrix.
                            // Since we later need the matrix (I-P), we already know that the diagonal entry will be one (=1-0)
                            matrixBuilder.addNextValue(curRow, newIndices[oldRowGroup], storm::utility::one<ConstantType>());
                        }
                        ++curRow;
                    }
                }
                this->matrixData.matrix=matrixBuilder.build();
                
                //Now run again through both matrices to get the remaining ingredients of the matrixData and vectorData.
                //Note that we need the matrix (I-P) in case of a dtmc.
                this->matrixData.assignment.reserve(this->matrixData.matrix.getEntryCount()); 
                this->matrixData.targetChoices = storm::storage::BitVector(this->matrixData.matrix.getRowCount(), false);
                this->vectorData.vector = std::vector<ConstantType>(this->matrixData.matrix.getRowCount()); //Important to initialize here since iterators have to remain valid
                auto vectorIt = this->vectorData.vector.begin();
                this->vectorData.assignment.reserve(vectorData.vector.size());
                curRow = 0;
                for(auto oldRowGroup : this->maybeStates){
                    for (std::size_t oldRow = parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup]; oldRow < parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        auto eqSysMatrixEntry = this->matrixData.matrix.getRow(curRow).begin();
                        ParametricType targetProbability = storm::utility::region::getNewFunction<ParametricType, CoefficientType>(storm::utility::zero<CoefficientType>());
                        for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                            if(this->maybeStates.get(oldEntry.getColumn())){
                                if(isDtmc && eqSysMatrixEntry->getColumn()==newIndices[oldRowGroup] && eqSysMatrixEntry->getColumn()!=newIndices[oldEntry.getColumn()]){
                                    //We are at one of the diagonal entries that have been inserted above and for which there is no entry in the original matrix.
                                    //These have already been set to 1 above, so they do not need to be handled here.
                                    ++eqSysMatrixEntry;
                                }
                                STORM_LOG_THROW(eqSysMatrixEntry->getColumn()==newIndices[oldEntry.getColumn()], storm::exceptions::UnexpectedException, "old and new entries do not match");
                                if(storm::utility::isConstant(oldEntry.getValue())){
                                    if(isDtmc){
                                        if(eqSysMatrixEntry->getColumn()==newIndices[oldRowGroup]){ //Diagonal entries get 1-c
                                            eqSysMatrixEntry->setValue(storm::utility::one<ConstantType>() - storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(oldEntry.getValue())));
                                        } else {
                                            eqSysMatrixEntry->setValue(storm::utility::zero<ConstantType>() - storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(oldEntry.getValue())));
                                        }
                                    } else {
                                        eqSysMatrixEntry->setValue(storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(oldEntry.getValue())));
                                    }
                                } else {
                                    typename std::unordered_map<ParametricType, ConstantType>::iterator functionsIt;
                                    if(isDtmc){
                                        if(eqSysMatrixEntry->getColumn()==newIndices[oldRowGroup]){ //Diagonal entries get 1-f(x)
                                            functionsIt = this->functions.insert(FunctionEntry(storm::utility::one<ParametricType>()-oldEntry.getValue(), dummyValue)).first;
                                        } else {
                                            functionsIt = this->functions.insert(FunctionEntry(storm::utility::zero<ParametricType>()-oldEntry.getValue(), dummyValue)).first;
                                        }
                                    } else {
                                        functionsIt = this->functions.insert(FunctionEntry(oldEntry.getValue(), dummyValue)).first;
                                    }
                                    this->matrixData.assignment.emplace_back(std::make_pair(eqSysMatrixEntry, &(functionsIt->second)));
                                    //Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
                                }
                                ++eqSysMatrixEntry;
                            }
                            else if(this->targetStates.get(oldEntry.getColumn())){
                                if(!this->computeRewards){
                                    targetProbability += oldEntry.getValue();
                                }
                                this->matrixData.targetChoices.set(curRow);
                            }
                        }
                        if(!this->computeRewards){
                            if(storm::utility::isConstant(storm::utility::simplify(targetProbability))){
                                *vectorIt = storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(targetProbability));
                            } else {
                                typename std::unordered_map<ParametricType, ConstantType>::iterator functionsIt = this->functions.insert(FunctionEntry(targetProbability, dummyValue)).first;
                                this->vectorData.assignment.emplace_back(std::make_pair(vectorIt, &(functionsIt->second)));
                                *vectorIt = dummyValue;
                            }
                        }
                        ++vectorIt;
                        ++curRow;
                    }
                }
                STORM_LOG_THROW(vectorIt==this->vectorData.vector.end(), storm::exceptions::UnexpectedException, "initProbs: The size of the eq-sys vector is not as it was expected");
                this->matrixData.matrix.updateNonzeroEntryCount();
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void SamplingModel<ParametricSparseModelType, ConstantType>::initializeRewards(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices){
                //Run through the state reward vector... Note that this only works for dtmcs
                STORM_LOG_THROW(this->vectorData.vector.size()==this->matrixData.matrix.getRowCount(), storm::exceptions::UnexpectedException, "The size of the eq-sys vector does not match to the number of rows in the eq-sys matrix");
                this->vectorData.assignment.reserve(vectorData.vector.size());
                ConstantType dummyValue = storm::utility::one<ConstantType>();
                auto vectorIt = this->vectorData.vector.begin();
                for(auto state : this->maybeStates){
                    if(storm::utility::isConstant(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state])){
                        *vectorIt = storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state]));
                    } else {
                        typename std::unordered_map<ParametricType, ConstantType>::iterator functionsIt = this->functions.insert(FunctionEntry(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state], dummyValue)).first;
                        this->vectorData.assignment.emplace_back(std::make_pair(vectorIt, &(functionsIt->second)));
                        *vectorIt = dummyValue;
                    }
                    ++vectorIt;
                }
                STORM_LOG_THROW(vectorIt==this->vectorData.vector.end(), storm::exceptions::UnexpectedException, "initRewards: The size of the eq-sys vector is not as it was expected");
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            SamplingModel<ParametricSparseModelType, ConstantType>::~SamplingModel() {
                //Intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::vector<ConstantType> SamplingModel<ParametricSparseModelType, ConstantType>::computeValues(std::map<VariableType, CoefficientType>const& point) {
                instantiate(point);
                invokeSolver();
                std::vector<ConstantType> result(this->maybeStates.size());
                storm::utility::vector::setVectorValues(result, this->maybeStates, this->solverData.result);
                storm::utility::vector::setVectorValues(result, this->targetStates, this->computeRewards ? storm::utility::zero<ConstantType>() : storm::utility::one<ConstantType>());
                storm::utility::vector::setVectorValues(result, ~(this->maybeStates | this->targetStates), this->computeRewards ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>());
                
                return result;
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType SamplingModel<ParametricSparseModelType, ConstantType>::computeInitialStateValue(std::map<VariableType, CoefficientType>const& point) {
                instantiate(point);
                invokeSolver();
                return this->solverData.result[this->solverData.initialStateIndex];
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void SamplingModel<ParametricSparseModelType, ConstantType>::instantiate(std::map<VariableType, CoefficientType>const& point) {
                //write results into the placeholders
                for(auto& functionResult : this->functions){
                    functionResult.second=storm::utility::region::convertNumber<ConstantType>(
                            storm::utility::region::evaluateFunction(functionResult.first, point));
                }
                for(auto& functionResult : this->functions){
                    functionResult.second=storm::utility::region::convertNumber<ConstantType>(
                            storm::utility::region::evaluateFunction(functionResult.first, point));
                }

                //write the instantiated values to the matrix and the vector according to the assignment
                for(auto& assignment : this->matrixData.assignment){
                    assignment.first->setValue(*(assignment.second));
                }
                for(auto& assignment : this->vectorData.assignment){
                    *assignment.first=*assignment.second;
                }
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void SamplingModel<ParametricSparseModelType, ConstantType>::invokeSolver(){
                if(this->typeOfParametricModel == storm::models::ModelType::Dtmc){
                    std::unique_ptr<storm::solver::LinearEquationSolver<double>> solver = storm::utility::solver::LinearEquationSolverFactory<double>().create(this->matrixData.matrix);
                    solver->solveEquationSystem(this->solverData.result, this->vectorData.vector);
                } else if(this->typeOfParametricModel == storm::models::ModelType::Mdp){
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<double>> solver = storm::solver::configureMinMaxLinearEquationSolver(this->solverData.solveGoal, storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(), this->matrixData.matrix);
                    storm::utility::policyguessing::solveMinMaxLinearEquationSystem(*solver,
                                        this->solverData.result, this->vectorData.vector,
                                        this->solverData.solveGoal.direction(),
                                        this->solverData.lastPolicy,
                                        this->matrixData.targetChoices, (this->computeRewards ? storm::utility::infinity<double>() : storm::utility::zero<double>()));
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected Type of model");
                }
            }
            
        
#ifdef STORM_HAVE_CARL
            template class SamplingModel<storm::models::sparse::Model<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
#endif
        } //namespace region
    }
}