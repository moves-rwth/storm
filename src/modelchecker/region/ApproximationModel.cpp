/* 
 * File:   ApproximationModel.cpp
 * Author: tim
 * 
 * Created on August 7, 2015, 9:29 AM
 */
#include "src/modelchecker/region/ApproximationModel.h"

#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/ModelType.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/solver/GameSolver.h"
#include "src/utility/macros.h"
#include "src/utility/region.h"
#include "src/utility/solver.h"
#include "src/utility/vector.h"
#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace region {
        
            template<typename ParametricSparseModelType, typename ConstantType>
            ApproximationModel<ParametricSparseModelType, ConstantType>::ApproximationModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula) : player1Goal(storm::logic::isLowerBound(formula->getComparisonType())){
                //First some simple checks and initializations
                if(formula->isProbabilityOperatorFormula()){
                    this->computeRewards=false;
                } else if(formula->isRewardOperatorFormula()){
                    this->computeRewards=true;
                    STORM_LOG_THROW(parametricModel.getType()==storm::models::ModelType::Dtmc, storm::exceptions::InvalidArgumentException, "Approximation with rewards is only implemented for Dtmcs");
                    STORM_LOG_THROW(parametricModel.hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the approximation model should be unique");
                    STORM_LOG_THROW(parametricModel.getUniqueRewardModel()->second.hasOnlyStateRewards(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the approximation model should have state rewards only");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid formula: " << this->formula << ". Approximation model only supports eventually or reachability reward formulae.");
                }
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
                std::vector<std::size_t> rowSubstitutions;// the substitution used in every row
                initializeProbabilities(parametricModel, newIndices, rowSubstitutions);
                if(this->computeRewards){
                    initializeRewards(parametricModel, newIndices, rowSubstitutions);
                }
                this->matrixData.assignment.shrink_to_fit();
                this->vectorData.assignment.shrink_to_fit();
                if(parametricModel.getType()==storm::models::ModelType::Mdp){
                    initializePlayer1Matrix(parametricModel, newIndices);
                }
                
                this->eqSysResult = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), this->computeRewards ? storm::utility::one<ConstantType>() : ConstantType(0.5));
                this->eqSysInitIndex = newIndices[initialState];
            }                

            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::initializeProbabilities(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices, std::vector<std::size_t>& rowSubstitutions) {
                STORM_LOG_DEBUG("Approximation model initialization for probabilities");
                /* First run: get a matrix with dummy entries at the new positions. 
                 * This matrix will have a row group for every row in the original matrix,
                 * each rowgroup containing 2^#par rows, where #par is the number of parameters that occur in the original row.
                 * We also store the substitution that needs to be applied for each row.
                 */
                ConstantType dummyValue = storm::utility::one<ConstantType>();
                auto numOfMaybeStates = this->maybeStates.getNumberOfSetBits();               
                storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(numOfMaybeStates, //exact number of rows is unknown at this point, but at least this many
                                                                                numOfMaybeStates, //columns
                                                                                0, //Unknown number of entries
                                                                                false, // no force dimensions
                                                                                true, //will have custom row grouping
                                                                                numOfMaybeStates); //exact number of rowgroups is unknown at this point, but at least this many
                rowSubstitutions.push_back(numOfMaybeStates);
                std::size_t curRow = 0;
                for (auto oldRowGroup : this->maybeStates){
                    for (std::size_t oldRow = parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup]; oldRow < parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        matrixBuilder.newRowGroup(curRow);
                        // Find the different substitutions, i.e., mappings from Variables that occur in this row to {lower, upper}
                        std::set<VariableType> occurringVariables;
                        for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                            if(this->maybeStates.get(oldEntry.getColumn())){
                                storm::utility::region::gatherOccurringVariables(oldEntry.getValue(), occurringVariables);
                            }
                        }
                        std::size_t numOfSubstitutions=1ull<<occurringVariables.size(); //=2^(#variables). Note that there is still 1 substitution when #variables==0 (the empty substitution)
                        for(uint_fast64_t substitutionId=0; substitutionId<numOfSubstitutions; ++substitutionId){
                            //compute actual substitution from substitutionId by interpreting the Id as a bit sequence.
                            //the occurringVariables.size() least significant bits of substitutionId will represent the substitution that we have to consider
                            //(00...0 = lower bounds for all parameters, 11...1 = upper bounds for all parameters)
                            std::map<VariableType, TypeOfBound> currSubstitution;
                            std::size_t parameterIndex=0ull;
                            for(auto const& parameter : occurringVariables){
                                if((substitutionId>>parameterIndex)%2==0){
                                    currSubstitution.insert(std::make_pair(parameter, TypeOfBound::LOWER));
                                }
                                else{
                                    currSubstitution.insert(std::make_pair(parameter, TypeOfBound::UPPER));
                                }
                                ++parameterIndex;
                            }
                            std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->funcSubData.substitutions, std::move(currSubstitution));
                            rowSubstitutions.push_back(substitutionIndex);
                            //For every substitution, run again through the row and add a dummy entry
                            //Note that this is still executed once, even if no parameters occur.
                            for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                                if(this->maybeStates.get(oldEntry.getColumn())){
                                    matrixBuilder.addNextValue(curRow, newIndices[oldEntry.getColumn()], dummyValue);
                                }
                            }
                        }
                        ++curRow;
                    }
                }
                this->matrixData.matrix=matrixBuilder.build();               
                
                //Now run again through both matrices to get the remaining ingredients of the matrixData and vectorData
                this->matrixData.assignment.reserve(this->matrixData.matrix.getEntryCount());
                this->vectorData.vector = std::vector<ConstantType>(this->matrixData.matrix.getRowCount()); //Important to initialize here since iterators have to remain valid
                auto vectorIt = this->vectorData.vector.begin();
                this->vectorData.assignment.reserve(vectorData.vector.size());
                std::size_t curRowGroup = 0;
                for(auto oldRowGroup : this->maybeStates){
                    for (std::size_t oldRow = parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup]; oldRow < parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        ParametricType targetProbability = storm::utility::region::getNewFunction<ParametricType, CoefficientType>(storm::utility::zero<CoefficientType>());
                        if(!this->computeRewards){
                            for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                                if(this->targetStates.get(oldEntry.getColumn())){
                                    targetProbability += oldEntry.getValue();
                                }
                            }
                        }
                        //Recall: Every row in the old matrix has a row group in the newly created one.
                        //We will now run through every row that belongs to the rowGroup associated with oldRow.
                        for (curRow = this->matrixData.matrix.getRowGroupIndices()[curRowGroup]; curRow < this->matrixData.matrix.getRowGroupIndices()[curRowGroup+1]; ++curRow){
                            auto eqSysMatrixEntry = this->matrixData.matrix.getRow(curRow).begin();
                            for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                                if(this->maybeStates.get(oldEntry.getColumn())){
                                    STORM_LOG_THROW(eqSysMatrixEntry->getColumn()==newIndices[oldEntry.getColumn()], storm::exceptions::UnexpectedException, "old and new entries do not match");
                                    if(storm::utility::isConstant(oldEntry.getValue())){
                                        eqSysMatrixEntry->setValue(storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(oldEntry.getValue())));
                                    } else {
                                        auto functionsIt = this->funcSubData.functions.insert(FunctionEntry(FunctionSubstitution(oldEntry.getValue(), rowSubstitutions[curRow]), dummyValue)).first;
                                        this->matrixData.assignment.emplace_back(std::make_pair(eqSysMatrixEntry, &(functionsIt->second)));
                                        //Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
                                    }
                                    ++eqSysMatrixEntry;
                                }
                            }
                            if(!this->computeRewards){
                                if(storm::utility::isConstant(targetProbability)){
                                    *vectorIt = storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(targetProbability));
                                } else {
                                    auto functionsIt = this->funcSubData.functions.insert(FunctionEntry(FunctionSubstitution(targetProbability, rowSubstitutions[curRow]), dummyValue)).first;
                                    this->vectorData.assignment.emplace_back(std::make_pair(vectorIt, &(functionsIt->second)));
                                    *vectorIt = dummyValue;
                                }
                            }
                            ++vectorIt;
                        }
                        ++curRowGroup;
                    }
                }
                STORM_LOG_THROW(vectorIt==this->vectorData.vector.end(), storm::exceptions::UnexpectedException, "initProbs: The size of the eq-sys vector is not as it was expected");
                this->matrixData.matrix.updateNonzeroEntryCount();
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::initializeRewards(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices, std::vector<std::size_t> const& rowSubstitutions){
                STORM_LOG_DEBUG("Approximation model initialization for Rewards");
                STORM_LOG_THROW(parametricModel.getType()==storm::models::ModelType::Dtmc, storm::exceptions::InvalidArgumentException, "Rewards are only supported for DTMCs (yet)");
                //Note: Since the original model is assumed to be a DTMC, there is no outgoing transition of a maybeState that leads to an infinity state.
                //Hence, we do not have to set entries of the eqSys vector to infinity (as it would be required for mdp model checking...)
                STORM_LOG_THROW(this->vectorData.vector.size()==this->matrixData.matrix.getRowCount(), storm::exceptions::UnexpectedException, "The size of the eq-sys vector does not match to the number of rows in the eq-sys matrix");
                this->vectorData.assignment.reserve(vectorData.vector.size());
                
                // run through the state reward vector of the parametric model.
                // Constant entries can be set directly.
                // For Parametric entries we set a dummy value and insert the corresponding function and the assignment
                ConstantType dummyValue = storm::utility::one<ConstantType>();
                auto vectorIt = this->vectorData.vector.begin();
                for(auto oldState : this->maybeStates){
                    if(storm::utility::isConstant(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[oldState])){
                        ConstantType reward = storm::utility::region::convertNumber<ConstantType>(storm::utility::region::getConstantPart(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[oldState]));
                        //Add one of these entries for every row in the row group of oldState
                        for(auto matrixRow=this->matrixData.matrix.getRowGroupIndices()[oldState]; matrixRow<this->matrixData.matrix.getRowGroupIndices()[state+1]; ++matrixRow){
                            *vectorIt = reward;
                            ++vectorIt;
                        }
                    } else {
                        std::set<VariableType> occurringRewVariables;
                        storm::utility::region::gatherOccurringVariables(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[oldState], occurringRewVariables);
                        // For each row in the row group of oldState, we get the corresponding substitution and insert the FunctionSubstitution
                        // We might find out that the reward is independent of the probability parameters (and will thus be independent of nondeterministic choices)
                        // In that case, the reward function and the substitution will not change and thus we can use the same FunctionSubstitution
                        bool rewardDependsOnProbVars=true;
                        std::unordered_map<FunctionSubstitution, ConstantType, FuncSubHash>::iterator functionsIt;
                        for(auto matrixRow=this->matrixData.matrix.getRowGroupIndices()[oldState]; matrixRow<this->matrixData.matrix.getRowGroupIndices()[oldState+1]; ++matrixRow){
                            auto probabilitySub = this->funcSubData.substitutions[rowSubstitutions[matrixRow]];
                            if(rewardDependsOnProbVars){ //always executed in first iteration
                                rewardDependsOnProbVars=false; //Assume that independent...
                                //Get the correct substitution for this matrixRow
                                std::map<VariableType, TypeOfBound> substitution;
                                for(auto const& rewardVar : occurringRewVariables){
                                    typename std::map<VariableType, TypeOfBound>::iterator const substitutionIt = probabilitySub.find(rewardVar);
                                    if(substitutionIt==probabilitySub.end()){
                                        substitution.insert(std::make_pair(rewardVar, TypeOfBound::CHOSEOPTIMAL));
                                    } else {
                                        substitution.insert(*substitutionIt);
                                        rewardDependsOnProbVars=true; //.. assumption wrong
                                    }
                                }
                                // insert the substitution and the FunctionSubstitution
                                std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->funcSubData.substitutions, std::move(substitution));
                                functionsIt = this->funcSubData.functions.insert(FunctionEntry(FunctionSubstitution(parametricModel.getUniqueRewardModel()->second.getStateRewardVector()[state], substitutionIndex), dummyValue)).first;
                            }
                            //insert assignment and dummy data
                            this->vectorData.assignment.emplace_back(std::make_pair(vectorIt, &(functionsIt->second)));
                            *vectorIt = dummyValue;
                            ++vectorIt;
                        }
                    }
                }
                STORM_LOG_THROW(vectorIt==this->vectorData.vector.end(), storm::exceptions::UnexpectedException, "initRewards: The size of the eq-sys vector is not as it was expected");
            }



            template<typename ParametricSparseModelType, typename ConstantType>
            ApproximationModel<ParametricSparseModelType, ConstantType>::~ApproximationModel() {
                //Intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::vector<ConstantType>  ApproximationModel<ParametricSparseModelType, ConstantType>::computeValues(ParameterRegion<ParametricType> const& region, bool computeLowerBounds) {
                instantiate(region, computeLowerBounds);
                invokeSolver(computeLowerBounds);
                
                std::vector<ConstantType> result(this->maybeStates.size());
                storm::utility::vector::setVectorValues(result, this->maybeStates, this->eqSysResult);
                storm::utility::vector::setVectorValues(result, this->targetStates, this->computeRewards ? storm::utility::zero<ConstantType>() : storm::utility::one<ConstantType>());
                storm::utility::vector::setVectorValues(result, ~(this->maybeStates | this->targetStates), this->computeRewards ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>());
                
                return result;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType  ApproximationModel<ParametricSparseModelType, ConstantType>::computeInitialStateValue(ParameterRegion<ParametricType> const& region, bool computeLowerBounds) {
                instantiate(region, computeLowerBounds);
                invokeSolver(computeLowerBounds);
                return this->eqSysResult[this->eqSysInitIndex];
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::instantiate(const ParameterRegion<ParametricType>& region, bool computeLowerBounds) {
                //Instantiate the substitutions
                std::vector<std::map<VariableType, CoefficientType>> instantiatedSubs(this->funcSubData.substitutions.size());
                std::vector<std::set<VariableType>> choseOptimalParameters(this->funcSubData.substitutions.size());
                for(std::size_t substitutionIndex=0; substitutionIndex<this->funcSubData.substitutions.size(); ++substitutionIndex){
                    for(std::pair<VariableType, TypeOfBound> const& sub : this->funcSubData.substitutions[substitutionIndex]){
                        switch(sub.second){
                            case TypeOfBound::LOWER:
                                instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getLowerBound(sub.first)));
                                break;
                            case TypeOfBound::UPPER:
                                instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getUpperBound(sub.first)));
                                break;
                            case TypeOfBound::CHOSEOPTIMAL:
                                //Insert some dummy value
                                instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, storm::utility::one<CoefficientType>()));
                                choseOptimalParameters[substitutionIndex].insert(sub.first);
                                break;
                            default:
                                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected Type of Bound");
                        }
                    }
                }
                
                //write function+substitution results into placeholders
                for(auto& functionResult : this->funcSubData.functions){
                    auto& funcSub = functionResult.first;
                    auto& result = functionResult.second;
                    result = computeLowerBounds ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>();
                     //Iterate over the different combinations of lower and upper bounds and update the min and max values
                    auto const& vertices=region.getVerticesOfRegion(this->choseOptimalParameters[funcSub.getSubstitution()]);
                    for(auto const& vertex : vertices){
                        //extend the substitution
                        for(auto const& vertexSub : vertex){
                            instantiatedSubs[funcSub.getSubstitution()][vertexSub.first]=vertexSub.second;
                        }
                        //evaluate the function
                        ConstantType currValue = storm::utility::region::convertNumber<ConstantType>(
                                storm::utility::region::evaluateFunction(
                                    funcSub.getFunction(),
                                    instantiatedSubs[funcSub.getSubstitution()]
                                    )
                                );
                        result = computeLowerBounds ? std::min(result, currValue) : std::max(result, currValue);
                    }
                }
                
                //write the instantiated values to the matrix and the vector according to the assignment
                for(auto& assignment : this->matrixData.assignment){
                    assignment.first->setValue(*(assignment.second));
                }
                for(auto& assignment : this->vectorData.assignment){
                    *assignment.first=*assignment.second;
                }
            }
            
                        
            template<>
            void ApproximationModel<storm::models::sparse::Dtmc<storm::RationalFunction>, double>::invokeSolver(bool computeLowerBounds){
                storm::solver::SolveGoal goal(computeLowerBounds);
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<double>> solver = storm::solver::configureMinMaxLinearEquationSolver(goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<double>(), this->matrixData.matrix);
                solver->solveEquationSystem(this->eqSysResult, this->vectorData.vector);
            }
            
            template<>
            void ApproximationModel<storm::models::sparse::Mdp<storm::RationalFunction>, double>::invokeSolver(bool computeLowerBounds){
                storm::solver::SolveGoal player2Goal(computeLowerBounds);
                std::unique_ptr<storm::solver::GameSolver<double>> solver = storm::utility::solver::GameSolverFactory<double>().create(this->player1Matrix, this->matrixData.matrix);
                solver->solveGame(this->player1Goal.direction(), player2Goal.direction(), this->eqSysResult, this->vectorData.vector);
            }
            
                


#ifdef STORM_HAVE_CARL
            template class ApproximationModel<storm::models::sparse::Dtmc<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
            template class ApproximationModel<storm::models::sparse::Mdp<storm::RationalFunction, storm::models::sparse::StandardRewardModel<storm::RationalFunction>>, double>;
#endif
        } //namespace region
    }
}