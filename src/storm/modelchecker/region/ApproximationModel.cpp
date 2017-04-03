#include <stdint.h>

#include "storm/modelchecker/region/ApproximationModel.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/ModelType.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/TerminationCondition.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/GameSolver.h"
#include "storm/utility/macros.h"
#include "storm/utility/region.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"
#include "storm/utility/policyguessing.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace region {
        
            template<typename ParametricSparseModelType, typename ConstantType>
            ApproximationModel<ParametricSparseModelType, ConstantType>::ApproximationModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula) {
                //First some simple checks and initializations
                this->typeOfParametricModel = parametricModel.getType();
                if(formula->isProbabilityOperatorFormula()){
                    this->computeRewards=false;
                    STORM_LOG_THROW(this->typeOfParametricModel==storm::models::ModelType::Dtmc || this->typeOfParametricModel==storm::models::ModelType::Mdp, storm::exceptions::InvalidArgumentException, "Approximation with probabilities is only implemented for Dtmcs and Mdps");
                } else if(formula->isRewardOperatorFormula()){
                    this->computeRewards=true;
                    STORM_LOG_THROW(this->typeOfParametricModel==storm::models::ModelType::Dtmc, storm::exceptions::InvalidArgumentException, "Approximation with rewards is only implemented for Dtmcs");
                    STORM_LOG_THROW(parametricModel.hasUniqueRewardModel(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the approximation model should be unique");
                    STORM_LOG_THROW(parametricModel.getUniqueRewardModel().hasOnlyStateRewards(), storm::exceptions::InvalidArgumentException, "The rewardmodel of the approximation model should have state rewards only");
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Invalid formula: " << formula << ". Approximation model only supports eventually or reachability reward formulae.");
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
                initializeProbabilities(parametricModel, newIndices);
                if(this->computeRewards){
                    initializeRewards(parametricModel);
                }
                this->matrixData.assignment.shrink_to_fit();
                this->vectorData.assignment.shrink_to_fit();
                if(this->typeOfParametricModel==storm::models::ModelType::Mdp){
                    initializePlayer1Matrix(parametricModel);
                    this->solverData.lastPlayer1Scheduler = storm::storage::TotalScheduler(std::vector<uint_fast64_t>(this->solverData.player1Matrix.getRowGroupCount(), 0));
                }
                
                this->solverData.result = std::vector<ConstantType>(maybeStates.getNumberOfSetBits(), this->computeRewards ? storm::utility::one<ConstantType>() : ConstantType(0.5));
                this->solverData.initialStateIndex = newIndices[initialState];
                this->solverData.lastMinimizingScheduler = storm::storage::TotalScheduler(std::vector<uint_fast64_t>(this->matrixData.matrix.getRowGroupCount(), 0));
                this->solverData.lastMaximizingScheduler = storm::storage::TotalScheduler(std::vector<uint_fast64_t>(this->matrixData.matrix.getRowGroupCount(), 0));
                //this->solverData.player1Goal = storm::solver::SolveGoal(storm::logic::isLowerBound(formula->getComparisonType()));
                storm::storage::BitVector filter(this->solverData.result.size(), false);
                filter.set(this->solverData.initialStateIndex, true);
                this->solverData.player1Goal = std::make_unique<storm::solver::BoundedGoal<ConstantType>>(
                            storm::logic::isLowerBound(formula->getComparisonType()) ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize,
                            formula->getComparisonType(), formula->getThresholdAs<ConstantType>(),
                            filter
                        );
            }                

            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::initializeProbabilities(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices) {
                STORM_LOG_DEBUG("Approximation model initialization for probabilities");
                /* First run: get a matrix with dummy entries at the new positions. 
                 * This matrix will have a row group for every row in the original matrix,
                 * each rowgroup containing 2^#par rows, where #par is the number of parameters that occur in the original row.
                 * We also store the substitution that needs to be applied for each row.
                 */
                ConstantType dummyNonZeroValue = storm::utility::one<ConstantType>();
                storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(0, //Unknown number of rows
                                                                                this->maybeStates.getNumberOfSetBits(), //columns
                                                                                0, //Unknown number of entries
                                                                                true, // force dimensions
                                                                                true, //will have custom row grouping
                                                                                0); //Unknown number of rowgroups
                this->matrixData.rowSubstitutions.reserve(this->maybeStates.getNumberOfSetBits());
                storm::storage::BitVector relevantColumns = this->computeRewards ? this->maybeStates : (this->maybeStates | this->targetStates);
                std::size_t curRow = 0;
                for (auto oldRowGroup : this->maybeStates){
                    for (std::size_t oldRow = parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup]; oldRow < parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        matrixBuilder.newRowGroup(curRow);
                        // Find the different substitutions, i.e., mappings from Variables that occur in this row to {lower, upper}
                        std::set<VariableType> occurringVariables;
                        for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                            if(relevantColumns.get(oldEntry.getColumn())){
                                storm::utility::region::gatherOccurringVariables(oldEntry.getValue(), occurringVariables);
                            }
                        }
                        uint_fast64_t numOfSubstitutions=1ull<<occurringVariables.size(); //=2^(#variables). Note that there is still 1 substitution when #variables==0 (the empty substitution)
                        for(uint_fast64_t substitutionId=0ull; substitutionId<numOfSubstitutions; ++substitutionId){
                            //compute actual substitution from substitutionId by interpreting the Id as a bit sequence.
                            //the occurringVariables.size() least significant bits of substitutionId will represent the substitution that we have to consider
                            //(00...0 = lower boundaries for all parameters, 11...1 = upper boundaries for all parameters)
                            std::map<VariableType, RegionBoundary> currSubstitution;
                            std::size_t parameterIndex=0ull;
                            for(auto const& parameter : occurringVariables){
                                if((substitutionId>>parameterIndex)%2==0){
                                    currSubstitution.insert(typename std::map<VariableType, RegionBoundary>::value_type(parameter, RegionBoundary::LOWER));
                                }
                                else{
                                    currSubstitution.insert(typename std::map<VariableType, RegionBoundary>::value_type(parameter, RegionBoundary::UPPER));
                                }
                                ++parameterIndex;
                            }
                            std::size_t substitutionIndex=storm::utility::vector::findOrInsert(this->funcSubData.substitutions, std::move(currSubstitution));
                            this->matrixData.rowSubstitutions.push_back(substitutionIndex);
                            //For every substitution, run again through the row and add a dummy entry
                            //Note that this is still executed once, even if no parameters occur.
                            for(auto const& oldEntry : parametricModel.getTransitionMatrix().getRow(oldRow)){
                                if(this->maybeStates.get(oldEntry.getColumn())){
                                    matrixBuilder.addNextValue(curRow, newIndices[oldEntry.getColumn()], dummyNonZeroValue);
                                }
                            }
                            ++curRow;
                        }
                    }
                }
                //Build the matrix. Override the row count (required e.g. when there are only transitions to target for the last matrixrow)
                this->matrixData.matrix=matrixBuilder.build(curRow);               
                
                //Now run again through both matrices to get the remaining ingredients of the matrixData and vectorData
                this->matrixData.assignment.reserve(this->matrixData.matrix.getEntryCount());
                this->matrixData.targetChoices = storm::storage::BitVector(this->matrixData.matrix.getRowCount(), false);
                this->vectorData.vector = std::vector<ConstantType>(this->matrixData.matrix.getRowCount()); //Important to initialize here since iterators have to remain valid
                auto vectorIt = this->vectorData.vector.begin();
                this->vectorData.assignment.reserve(vectorData.vector.size());
                std::size_t curRowGroup = 0;
                for(auto oldRowGroup : this->maybeStates){
                    for (std::size_t oldRow = parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup]; oldRow < parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        ParametricType targetProbability = storm::utility::region::getNewFunction<ParametricType, CoefficientType>(storm::utility::zero<CoefficientType>());
                        if(!this->computeRewards){
                            //Compute the target probability to insert it in every new row
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
                                        eqSysMatrixEntry->setValue(storm::utility::convertNumber<ConstantType>(storm::utility::region::getConstantPart(oldEntry.getValue())));
                                    } else {
                                        auto functionsIt = this->funcSubData.functions.insert(FunctionEntry(FunctionSubstitution(oldEntry.getValue(), this->matrixData.rowSubstitutions[curRow]), dummyNonZeroValue)).first;
                                        this->matrixData.assignment.emplace_back(eqSysMatrixEntry, functionsIt->second);
                                        //Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
                                    }
                                    ++eqSysMatrixEntry;
                                }
                                if(this->targetStates.get(oldEntry.getColumn())){
                                    //Store that this row has a transition to target
                                    this->matrixData.targetChoices.set(curRow);
                                }
                            }
                            if(!this->computeRewards){
                                if(storm::utility::isConstant(storm::utility::simplify(targetProbability))){
                                    *vectorIt = storm::utility::convertNumber<ConstantType>(storm::utility::region::getConstantPart(targetProbability));
                                } else {
                                    auto functionsIt = this->funcSubData.functions.insert(FunctionEntry(FunctionSubstitution(targetProbability, this->matrixData.rowSubstitutions[curRow]), dummyNonZeroValue)).first;
                                    this->vectorData.assignment.emplace_back(vectorIt, functionsIt->second);
                                    *vectorIt = dummyNonZeroValue;
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
            void ApproximationModel<ParametricSparseModelType, ConstantType>::initializeRewards(ParametricSparseModelType const& parametricModel){
                STORM_LOG_DEBUG("Approximation model initialization for Rewards");
                //Note: Since the original model is assumed to be a DTMC, there is no outgoing transition of a maybeState that leads to an infinity state.
                //Hence, we do not have to set entries of the eqSys vector to infinity (as it would be required for mdp model checking...)
                STORM_LOG_THROW(this->vectorData.vector.size()==this->matrixData.matrix.getRowCount(), storm::exceptions::UnexpectedException, "The size of the eq-sys vector does not match to the number of rows in the eq-sys matrix");
                this->vectorData.assignment.reserve(vectorData.vector.size());
                
                // run through the state reward vector of the parametric model.
                // Constant entries can be set directly.
                // For Parametric entries we set a dummy value and insert the corresponding function and the assignment
                ConstantType dummyNonZeroValue = storm::utility::one<ConstantType>();
                auto vectorIt = this->vectorData.vector.begin();
                for(auto oldState : this->maybeStates){
                    if(storm::utility::isConstant(parametricModel.getUniqueRewardModel().getStateRewardVector()[oldState])){
                        ConstantType reward = storm::utility::convertNumber<ConstantType>(storm::utility::region::getConstantPart(parametricModel.getUniqueRewardModel().getStateRewardVector()[oldState]));
                        //Add one of these entries for every row in the row group of oldState
                        for(auto matrixRow=this->matrixData.matrix.getRowGroupIndices()[oldState]; matrixRow<this->matrixData.matrix.getRowGroupIndices()[oldState+1]; ++matrixRow){
                            *vectorIt = reward;
                            ++vectorIt;
                        }
                    } else {
                        std::set<VariableType> occurringRewVariables;
                        storm::utility::region::gatherOccurringVariables(parametricModel.getUniqueRewardModel().getStateRewardVector()[oldState], occurringRewVariables);
                        // For each row in the row group of oldState, we get the corresponding substitution and insert the FunctionSubstitution
                        for(auto matrixRow=this->matrixData.matrix.getRowGroupIndices()[oldState]; matrixRow<this->matrixData.matrix.getRowGroupIndices()[oldState+1]; ++matrixRow){
                            //Extend the substitution for the probabilities with the reward parameters
                            auto& substitution = this->funcSubData.substitutions[this->matrixData.rowSubstitutions[matrixRow]];
                            for(auto const& rewardVar : occurringRewVariables){
                                //Note that map::insert does nothing if rewardVar is already contained in the substitution (i.e. if rewardVar also occurs in the probability functions)
                                substitution.insert(typename std::map<VariableType, RegionBoundary>::value_type(rewardVar, RegionBoundary::UNSPECIFIED));
                            }
                            // insert the FunctionSubstitution
                            auto functionsIt = this->funcSubData.functions.insert(FunctionEntry(FunctionSubstitution(parametricModel.getUniqueRewardModel().getStateRewardVector()[oldState], this->matrixData.rowSubstitutions[matrixRow]), dummyNonZeroValue)).first;
                            //insert assignment and dummy data
                            this->vectorData.assignment.emplace_back(vectorIt, functionsIt->second);
                            *vectorIt = dummyNonZeroValue;
                            ++vectorIt;
                        }
                    }
                }
                STORM_LOG_THROW(vectorIt==this->vectorData.vector.end(), storm::exceptions::UnexpectedException, "initRewards: The size of the eq-sys vector is not as it was expected");
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::initializePlayer1Matrix(ParametricSparseModelType const& parametricModel){
                std::size_t p1MatrixSize = matrixData.matrix.getRowGroupCount();
                storm::storage::SparseMatrixBuilder<storm::storage::sparse::state_type> matrixBuilder(p1MatrixSize, //rows
                                                                                p1MatrixSize, //columns
                                                                                p1MatrixSize, //entries
                                                                                true, // force dimensions
                                                                                true, //will have custom row grouping
                                                                                this->maybeStates.getNumberOfSetBits()); // number of rowgroups
                std::size_t curRow = 0;
                for (auto oldRowGroup : this->maybeStates){
                    matrixBuilder.newRowGroup(curRow);
                    for (std::size_t oldRow = parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup]; oldRow < parametricModel.getTransitionMatrix().getRowGroupIndices()[oldRowGroup+1]; ++oldRow){
                        matrixBuilder.addNextValue(curRow,curRow, storm::utility::one<storm::storage::sparse::state_type>());
                        ++curRow;
                    }
                }
                this->solverData.player1Matrix = matrixBuilder.build();
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            ApproximationModel<ParametricSparseModelType, ConstantType>::~ApproximationModel() {
                //Intentionally left empty
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            std::vector<ConstantType>  ApproximationModel<ParametricSparseModelType, ConstantType>::computeValues(ParameterRegion<ParametricType> const& region, bool computeLowerBounds) {
                instantiate(region, computeLowerBounds);
                storm::storage::TotalScheduler& scheduler = computeLowerBounds ? this->solverData.lastMinimizingScheduler : this->solverData.lastMaximizingScheduler;
                invokeSolver(computeLowerBounds, scheduler, false);
                
                std::vector<ConstantType> result(this->maybeStates.size());
                storm::utility::vector::setVectorValues(result, this->maybeStates, this->solverData.result);
                storm::utility::vector::setVectorValues(result, this->targetStates, this->computeRewards ? storm::utility::zero<ConstantType>() : storm::utility::one<ConstantType>());
                storm::utility::vector::setVectorValues(result, ~(this->maybeStates | this->targetStates), this->computeRewards ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>());
                
                return result;
            }

            template<typename ParametricSparseModelType, typename ConstantType>
            ConstantType  ApproximationModel<ParametricSparseModelType, ConstantType>::computeInitialStateValue(ParameterRegion<ParametricType> const& region, bool computeLowerBounds) {
                instantiate(region, computeLowerBounds);
                storm::storage::TotalScheduler& scheduler = computeLowerBounds ? this->solverData.lastMinimizingScheduler : this->solverData.lastMaximizingScheduler;
                invokeSolver(computeLowerBounds, scheduler, false);
                return this->solverData.result[this->solverData.initialStateIndex];
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            bool  ApproximationModel<ParametricSparseModelType, ConstantType>::checkFormulaOnRegion(ParameterRegion<ParametricType> const& region, bool computeLowerBounds) {
                instantiate(region, computeLowerBounds);
                storm::storage::TotalScheduler& scheduler = computeLowerBounds ? this->solverData.lastMinimizingScheduler : this->solverData.lastMaximizingScheduler;
                invokeSolver(computeLowerBounds, scheduler, true); //allow early termination
                return this->solverData.player1Goal->achieved(this->solverData.result);
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::instantiate(const ParameterRegion<ParametricType>& region, bool computeLowerBounds) {
                //Instantiate the substitutions
                std::vector<std::map<VariableType, CoefficientType>> instantiatedSubs(this->funcSubData.substitutions.size());
                std::vector<std::set<VariableType>> unspecifiedParameters(this->funcSubData.substitutions.size());
                for(std::size_t substitutionIndex=0; substitutionIndex<this->funcSubData.substitutions.size(); ++substitutionIndex){
                    for(std::pair<VariableType, RegionBoundary> const& sub : this->funcSubData.substitutions[substitutionIndex]){
                        switch(sub.second){
                            case RegionBoundary::LOWER:
                                instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getLowerBoundary(sub.first)));
                                break;
                            case RegionBoundary::UPPER:
                                instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, region.getUpperBoundary(sub.first)));
                                break;
                            case RegionBoundary::UNSPECIFIED:
                                //Insert some dummy value
                                instantiatedSubs[substitutionIndex].insert(std::make_pair(sub.first, storm::utility::one<CoefficientType>()));
                                unspecifiedParameters[substitutionIndex].insert(sub.first);
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
                    auto const& vertices=region.getVerticesOfRegion(unspecifiedParameters[funcSub.second]);
                    for(auto const& vertex : vertices){
                        //extend the substitution
                        for(auto const& vertexSub : vertex){
                            instantiatedSubs[funcSub.second][vertexSub.first]=vertexSub.second;
                        }
                        //evaluate the function
                        ConstantType currValue = storm::utility::convertNumber<ConstantType>(
                                storm::utility::region::evaluateFunction(
                                    funcSub.first,
                                    instantiatedSubs[funcSub.second]
                                    )
                                );
                        result = computeLowerBounds ? std::min(result, currValue) : std::max(result, currValue);
                    }
                }
                
                //write the instantiated values to the matrix and the vector according to the assignment
                for(auto& assignment : this->matrixData.assignment){
                    assignment.first->setValue(assignment.second);
                }
                for(auto& assignment : this->vectorData.assignment){
                    *assignment.first = assignment.second;
                }
            }
            
                        
            template<typename ParametricSparseModelType, typename ConstantType>
            void ApproximationModel<ParametricSparseModelType, ConstantType>::invokeSolver(bool computeLowerBounds, storm::storage::TotalScheduler& scheduler, bool allowEarlyTermination){
                storm::solver::SolveGoal player2Goal(computeLowerBounds);
                std::unique_ptr<storm::solver::TerminationCondition<ConstantType>> terminationCondition;
                if(allowEarlyTermination){
                    if(computeLowerBounds){
                        //Take minimum
                        //Note that value iteration will approach the minimum from above as we start it with values that correspond to some scheduler-induced DTMC
                        terminationCondition = std::make_unique<storm::solver::TerminateIfFilteredExtremumBelowThreshold<ConstantType>>(
                                                                                                                                                 this->solverData.player1Goal->relevantValues(),
                                                                                                                                                 this->solverData.player1Goal->thresholdValue(),
                                                                                                                                                 this->solverData.player1Goal->boundIsStrict(),
                                                                                                                                                 true
                                                                                                                                                 );
                    } else {
                        //Take maximum
                        terminationCondition = std::make_unique<storm::solver::TerminateIfFilteredExtremumExceedsThreshold<ConstantType>>(
                                                                                                                                                   this->solverData.player1Goal->relevantValues(),
                                                                                                                                                   this->solverData.player1Goal->thresholdValue(),
                                                                                                                                                   this->solverData.player1Goal->boundIsStrict(),
                                                                                                                                                   false
                                                                                                                                                   );
                    }
                }
                if(this->typeOfParametricModel == storm::models::ModelType::Dtmc){
                    //Invoke mdp model checking
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ConstantType>> solver = storm::solver::configureMinMaxLinearEquationSolver(player2Goal, storm::solver::GeneralMinMaxLinearEquationSolverFactory<double>(), this->matrixData.matrix);
                    solver->setTerminationCondition(std::move(terminationCondition));
                    storm::utility::policyguessing::solveMinMaxLinearEquationSystem(*solver, this->matrixData.matrix,
                                this->solverData.result, this->vectorData.vector,
                                player2Goal.direction(),
                                scheduler,
                                this->matrixData.targetChoices, (this->computeRewards ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>())
                        );
                } else if(this->typeOfParametricModel == storm::models::ModelType::Mdp){
                    //Invoke stochastic two player game model checking
                    std::unique_ptr<storm::solver::GameSolver<ConstantType>> solver = storm::utility::solver::GameSolverFactory<ConstantType>().create(this->solverData.player1Matrix, this->matrixData.matrix);
                    if(this->solverData.player1Goal->minimize() == computeLowerBounds){
                        //Early termination is only allowed if we play Min-Min or Max-Max!
                        solver->setTerminationCondition(std::move(terminationCondition));
                    }
                    storm::utility::policyguessing::solveGame(*solver, 
                                this->solverData.result, this->vectorData.vector,
                                this->solverData.player1Goal->direction(), player2Goal.direction(), 
                                this->solverData.lastPlayer1Scheduler, scheduler,
                                this->matrixData.targetChoices, (this->computeRewards ? storm::utility::infinity<ConstantType>() : storm::utility::zero<ConstantType>())
                        );
                   // Alternatively(without Scheduler guessing) 
                   // this->solverData.result = std::vector<ConstantType>(this->solverData.result.size(), 0.0);
                   // solver->solveGame(this->solverData.player1Goal.direction(), player2Goal.direction(), this->solverData.result, this->vectorData.vector);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected Type of model");
                }
            }
            

#ifdef STORM_HAVE_CARL
            template class ApproximationModel<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
            template class ApproximationModel<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
#endif
        } //namespace region
    }
}
