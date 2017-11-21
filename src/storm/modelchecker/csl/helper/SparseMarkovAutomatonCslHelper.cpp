#include "storm/modelchecker/csl/helper/SparseMarkovAutomatonCslHelper.h"

#include "storm/modelchecker/prctl/helper/SparseMdpPrctlHelper.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"
#include "storm/settings/modules/MarkovAutomatonSettings.h"

#include "storm/utility/macros.h"
#include "storm/utility/vector.h"
#include "storm/utility/graph.h"

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/utility/numerical.h"

#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/LpSolver.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/UncheckedRequirementException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            void SparseMarkovAutomatonCslHelper::computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                
                // Start by computing four sparse matrices:
                // * a matrix aMarkovian with all (discretized) transitions from Markovian non-goal states to all Markovian non-goal states.
                // * a matrix aMarkovianToProbabilistic with all (discretized) transitions from Markovian non-goal states to all probabilistic non-goal states.
                // * a matrix aProbabilistic with all (non-discretized) transitions from probabilistic non-goal states to other probabilistic non-goal states.
                // * a matrix aProbabilisticToMarkovian with all (non-discretized) transitions from probabilistic non-goal states to all Markovian non-goal states.
                typename storm::storage::SparseMatrix<ValueType> aMarkovian = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, markovianNonGoalStates, true);
                typename storm::storage::SparseMatrix<ValueType> aMarkovianToProbabilistic = transitionMatrix.getSubmatrix(true, markovianNonGoalStates, probabilisticNonGoalStates);
                typename storm::storage::SparseMatrix<ValueType> aProbabilistic = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, probabilisticNonGoalStates);
                typename storm::storage::SparseMatrix<ValueType> aProbabilisticToMarkovian = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, markovianNonGoalStates);
                
                // The matrices with transitions from Markovian states need to be digitized.
                // Digitize aMarkovian. Based on whether the transition is a self-loop or not, we apply the two digitization rules.
                uint64_t rowIndex = 0;
                for (auto state : markovianNonGoalStates) {
                    for (auto& element : aMarkovian.getRow(rowIndex)) {
                        ValueType eTerm = std::exp(-exitRates[state] * delta);
                        if (element.getColumn() == rowIndex) {
                            element.setValue((storm::utility::one<ValueType>() - eTerm) * element.getValue() + eTerm);
                        } else {
                            element.setValue((storm::utility::one<ValueType>() - eTerm) * element.getValue());
                        }
                    }
                    ++rowIndex;
                }
                
                // Digitize aMarkovianToProbabilistic. As there are no self-loops in this case, we only need to apply the digitization formula for regular successors.
                rowIndex = 0;
                for (auto state : markovianNonGoalStates) {
                    for (auto& element : aMarkovianToProbabilistic.getRow(rowIndex)) {
                        element.setValue((1 - std::exp(-exitRates[state] * delta)) * element.getValue());
                    }
                    ++rowIndex;
                }
                
                // Initialize the two vectors that hold the variable one-step probabilities to all target states for probabilistic and Markovian (non-goal) states.
                std::vector<ValueType> bProbabilistic(aProbabilistic.getRowCount());
                std::vector<ValueType> bMarkovian(markovianNonGoalStates.getNumberOfSetBits());
                
                // Compute the two fixed right-hand side vectors, one for Markovian states and one for the probabilistic ones.
                std::vector<ValueType> bProbabilisticFixed = transitionMatrix.getConstrainedRowGroupSumVector(probabilisticNonGoalStates, goalStates);
                std::vector<ValueType> bMarkovianFixed;
                bMarkovianFixed.reserve(markovianNonGoalStates.getNumberOfSetBits());
                for (auto state : markovianNonGoalStates) {
                    bMarkovianFixed.push_back(storm::utility::zero<ValueType>());
                    
                    for (auto& element : transitionMatrix.getRowGroup(state)) {
                        if (goalStates.get(element.getColumn())) {
                            bMarkovianFixed.back() += (1 - std::exp(-exitRates[state] * delta)) * element.getValue();
                        }
                    }
                }

                // Check for requirements of the solver.
                // The solution is unique as we assume non-zeno MAs.
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(true, dir);
                requirements.clearBounds();
                STORM_LOG_THROW(requirements.empty(), storm::exceptions::UncheckedRequirementException, "Cannot establish requirements for solver.");

                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(aProbabilistic);
                solver->setHasUniqueSolution();
                solver->setBounds(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>());
                solver->setRequirementsChecked();
                solver->setCachingEnabled(true);
                
                // Perform the actual value iteration
                // * loop until the step bound has been reached
                // * in the loop:
                // *    perform value iteration using A_PSwG, v_PS and the vector b where b = (A * 1_G)|PS + A_PStoMS * v_MS
                //      and 1_G being the characteristic vector for all goal states.
                // *    perform one timed-step using v_MS := A_MSwG * v_MS + A_MStoPS * v_PS + (A * 1_G)|MS
                std::vector<ValueType> markovianNonGoalValuesSwap(markovianNonGoalValues);
                for (uint64_t currentStep = 0; currentStep < numberOfSteps; ++currentStep) {
                    // Start by (re-)computing bProbabilistic = bProbabilisticFixed + aProbabilisticToMarkovian * vMarkovian.
                    aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                    storm::utility::vector::addVectors(bProbabilistic, bProbabilisticFixed, bProbabilistic);
                    
                    // Now perform the inner value iteration for probabilistic states.
                    solver->solveEquations(dir, probabilisticNonGoalValues, bProbabilistic);
                    
                    // (Re-)compute bMarkovian = bMarkovianFixed + aMarkovianToProbabilistic * vProbabilistic.
                    aMarkovianToProbabilistic.multiplyWithVector(probabilisticNonGoalValues, bMarkovian);
                    storm::utility::vector::addVectors(bMarkovian, bMarkovianFixed, bMarkovian);
                    aMarkovian.multiplyWithVector(markovianNonGoalValues, markovianNonGoalValuesSwap);
                    std::swap(markovianNonGoalValues, markovianNonGoalValuesSwap);
                    storm::utility::vector::addVectors(markovianNonGoalValues, bMarkovian, markovianNonGoalValues);
                }
                
                // After the loop, perform one more step of the value iteration for PS states.
                aProbabilisticToMarkovian.multiplyWithVector(markovianNonGoalValues, bProbabilistic);
                storm::utility::vector::addVectors(bProbabilistic, bProbabilisticFixed, bProbabilistic);
                solver->solveEquations(dir, probabilisticNonGoalValues, bProbabilistic);
            }
             
            template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            void SparseMarkovAutomatonCslHelper::computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded reachability probabilities is unsupported for this value type.");
            }
            
            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            void SparseMarkovAutomatonCslHelper::printTransitions(std::vector<std::vector<ValueType>> relReachability, std::vector<ValueType> const& exitRateVector, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector const& cycleStates, storm::storage::BitVector const& cycleGoalStates, std::vector<std::vector<ValueType>>& vd, std::vector<std::vector<ValueType>>& vu, std::vector<std::vector<ValueType>>& wu){
                std::ofstream logfile("U+logfile.txt", std::ios::app);
                
                auto const& rowGroupIndices = fullTransitionMatrix.getRowGroupIndices();
                auto numberOfStates = fullTransitionMatrix.getRowGroupCount();            

                logfile << "number of states = num of row group count " << numberOfStates << "\n";
                for (uint_fast64_t i = 0; i < fullTransitionMatrix.getRowGroupCount(); i++) {
                    logfile << " from node  " << i << " ";
                    auto from = rowGroupIndices[i];
                    auto to = rowGroupIndices[i+1];
                    for (auto j = from ; j < to; j++){
                        for (auto &v : fullTransitionMatrix.getRow(j)) {
                            if (markovianStates[i]){
                                logfile   << v.getValue()  *exitRateVector[i] << " -> "<< v.getColumn()  << "\t";
                            } else {
                                logfile   << v.getValue() << " -> "<< v.getColumn()  << "\t";
                            }
                        }
                        logfile << "\n";
                    }
                }
                logfile << "\n";

                logfile << "probStates\tmarkovianStates\tgoalStates\tcycleStates\tcycleGoalStates\n";
                for (int i =0 ; i< markovianStates.size() ; i++){
                    logfile << (~markovianStates)[i] << "\t\t" << markovianStates[i] << "\t\t" << psiStates[i] << "\t\t" << cycleStates[i] << "\t\t" << cycleGoalStates[i] << "\n";
                }

                for (int i =0; i<relReachability.size(); i++){
                    for (int j=0; j<relReachability[i].size(); j++){
                        logfile << relReachability[i][j] << "\n";
                    }
                    logfile << "\n";
                }


                logfile << "vd: \n";
                for (uint_fast64_t i =0 ; i<vd.size(); i++){
                    for(uint_fast64_t j=0; j<fullTransitionMatrix.getRowGroupCount(); j++){
                        logfile << vd[i][j] << "\t" ;
                    }
                    logfile << "\n";
                }

                logfile << "\nvu:\n";
                for (uint_fast64_t i =0 ; i<vu.size(); i++){
                    for(uint_fast64_t j=0; j<fullTransitionMatrix.getRowGroupCount(); j++){
                        logfile << vu[i][j] << "\t" ;
                    }
                    logfile << "\n";
                }

                logfile << "\nwu\n";
                for (uint_fast64_t i =0 ; i<wu.size(); i++){
                    for(uint_fast64_t j=0; j<fullTransitionMatrix.getRowGroupCount(); j++){
                        logfile << wu[i][j] << "\t" ;
                    }
                    logfile << "\n";
                }

                logfile.close();
            }

            template<typename ValueType>
            ValueType SparseMarkovAutomatonCslHelper::poisson(ValueType lambda, uint64_t i) {
                ValueType res = pow(lambda, i);
                ValueType fac = 1;
                for (uint64_t j = i ; j>0 ; j--){
                    fac = fac *j;
                }
                res = res / fac ;
                res = res * exp(-lambda);
                return res;
            }

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            void SparseMarkovAutomatonCslHelper::calculateVu(OptimizationDirection dir, uint64_t k, uint64_t node, ValueType lambda, std::vector<std::vector<ValueType>>& vu, std::vector<std::vector<ValueType>>& wu, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates){
                if (vu[k][node]!=-1){return;} //dynamic programming. avoiding multiple calculation.
                uint64_t N = vu.size()-1;
                auto rowGroupIndices = fullTransitionMatrix.getRowGroupIndices();

                ValueType res =0;
                for (uint64_t i = k ; i < N ; i++ ){
                    if (wu[N-1-(i-k)][node]==-1){
                        calculateWu(dir, (N-1-(i-k)),node,lambda,wu,fullTransitionMatrix,markovianStates,psiStates);
                    }
                    res+=poisson(lambda, i)*wu[N-1-(i-k)][node];
                }
                vu[k][node]=res;
            }

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            void SparseMarkovAutomatonCslHelper::calculateWu(OptimizationDirection dir, uint64_t k, uint64_t node, ValueType lambda, std::vector<std::vector<ValueType>>& wu, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates){
                if (wu[k][node]!=-1){return;} //dynamic programming. avoiding multiple calculation.
                uint64_t N = wu.size()-1;
                auto const& rowGroupIndices = fullTransitionMatrix.getRowGroupIndices( );

                ValueType res;
                if (k==N){
                    wu[k][node]=0;
                    return;
                }

                if (psiStates[node]){
                    wu[k][node]=1;
                    return;
                }

                if (markovianStates[node]){
                    res = 0;
                    auto line = fullTransitionMatrix.getRow(rowGroupIndices[node]);
                    for (auto &element : line){
                        uint64_t to = element.getColumn();
                        if (wu[k+1][to]==-1){
                            calculateWu(dir, k+1,to,lambda,wu,fullTransitionMatrix,markovianStates,psiStates);
                        }
                        res+=element.getValue()*wu[k+1][to];
                    }
                } else {
                    res = -1;
                    uint64_t rowStart = rowGroupIndices[node];
                    uint64_t rowEnd = rowGroupIndices[node+1];
                    for (uint64_t i = rowStart; i< rowEnd; i++){
                        auto line = fullTransitionMatrix.getRow(i);
                        ValueType between = 0;
                        for (auto& element: line){
                            uint64_t to = element.getColumn();
                            if (to==node){
                                continue;
                            }
                            if (wu[k][to]==-1){
                                calculateWu(dir, k,to,lambda,wu,fullTransitionMatrix,markovianStates,psiStates);
                            }
                            between+=element.getValue()*wu[k][to];
                        }
                        if (maximize(dir)){
                            res = std::max(res,between);
                        } else {
                            if (res!=-1){
                                res = std::min(res,between);
                            } else {
                                res = between;
                            }
                        }
                    }
                } // end no goal-prob state
                
                wu[k][node]=res;
            }

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            void SparseMarkovAutomatonCslHelper::calculateVd(OptimizationDirection dir, uint64_t k, uint64_t node, ValueType lambda, std::vector<std::vector<ValueType>>& vd, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates){
               
                std::ofstream logfile("U+logfile.txt", std::ios::app);

                if (vd[k][node]!=-1){return;} //dynamic programming. avoiding multiple calculation.
                logfile << "calculating vd for k = " << k << " node "<< node << " \t";
                uint64_t N = vd.size()-1;
                auto const& rowGroupIndices = fullTransitionMatrix.getRowGroupIndices();

                ValueType res;
                if (k==N){
                    logfile << "k == N! res = 0\n";
                    vd[k][node]=0;
                    return;
                }
                
                //goal state
                if (psiStates[node]){
                    res = storm::utility::zero<ValueType>();
                    for (uint64_t i = k ; i<N ; i++){
                        ValueType between = poisson(lambda,i);
                        res+=between;
                    }
                    vd[k][node]=res;
                    logfile << "goal state node " << node << " res = " << res << "\n";
                    return;
                }

                // no-goal markovian state
                if (markovianStates[node]){
                    logfile << "markovian state: ";
                    res = 0;
                    auto line = fullTransitionMatrix.getRow(rowGroupIndices[node]);
                    for (auto &element : line){
                        uint64_t to = element.getColumn();
                        if (vd[k+1][to]==-1){
                            calculateVd(dir,k+1,to,lambda,vd, fullTransitionMatrix, markovianStates,psiStates);
                        }
                        res+=element.getValue()*vd[k+1][to];
                    }
                } else { //no-goal prob state
                    logfile << "prob state: ";
                    res = -1;
                    uint64_t rowStart = rowGroupIndices[node];
                    uint64_t rowEnd = rowGroupIndices[node+1];
                    for (uint64_t i = rowStart; i< rowEnd; i++){
                        auto line = fullTransitionMatrix.getRow(i);
                        ValueType between = 0;
                        for (auto& element: line){
                            uint64_t to = element.getColumn();
                            if (to==node){
                                logfile << "ignoring self loops for now";
                                continue;
                            }
                            if (vd[k][to]==-1){
                                calculateVd(dir, k,to,lambda,vd, fullTransitionMatrix, markovianStates,psiStates);
                            }
                            between+=element.getValue()*vd[k][to];
                        }
                        if (maximize(dir)){
                                res = std::max(res, between);
                        } else {
                            if (res!=-1){
                                res =std::min(res,between);
                            } else {
                                res = between;
                            }
                        }
                    }
                } 
                vd[k][node]=res;
                logfile << " res = " << res << "\n";
            }

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
            uint64_t SparseMarkovAutomatonCslHelper::trajans(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, uint64_t node, std::vector<uint64_t >& disc, std::vector<uint64_t >& finish, uint64_t* counter) {
                auto const& rowGroupIndice = transitionMatrix.getRowGroupIndices();

                disc[node] = *counter;
                finish[node] = *counter;
                (*counter)+=1;

                auto from = rowGroupIndice[node];
                auto to = rowGroupIndice[node+1];

                for(uint64_t i =from; i<to ; i++ ) {
                    for(auto element : transitionMatrix.getRow(i)){
                        if (element.getValue()==0){
                            continue;
                        }
                        if (disc[element.getColumn()]==0){
                            uint64_t back = trajans(transitionMatrix,element.getColumn(),disc,finish, counter);
                            finish[node]=std::min(finish[node], back);
                        } else {
                            finish[node]=std::min(finish[node], disc[element.getColumn()]);
                        }
                    }
                }

                return finish[node];
            }

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
            storm::storage::BitVector SparseMarkovAutomatonCslHelper::identifyProbCyclesGoalStates(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,  storm::storage::BitVector const& cycleStates) {

                storm::storage::BitVector goalStates(cycleStates.size(), false);
                auto const&  rowGroupIndices = transitionMatrix.getRowGroupIndices();

                for (uint64_t i = 0 ; i < transitionMatrix.getRowGroupCount() ; i++){
                    if (!cycleStates[i]){
                        continue;
                    }
                    auto from = rowGroupIndices[i];
                    auto to = rowGroupIndices[i+1];
                    for (auto j = from ; j<to; j++){
                        for (auto element: transitionMatrix.getRow(j)) {
                            if (!cycleStates[element.getColumn()]){
                                goalStates.set(element.getColumn(),true);
                            }
                        }
                    }
                }
                return goalStates;

            }

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
            storm::storage::BitVector SparseMarkovAutomatonCslHelper::identifyProbCycles(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates){

                storm::storage::BitVector const& probabilisticStates = ~markovianStates;
                storm::storage::BitVector const& probabilisticNonGoalStates = ~markovianStates & ~psiStates;

                storm::storage::SparseMatrix<ValueType> const& probMatrix = transitionMatrix.getSubmatrix(true, probabilisticNonGoalStates, probabilisticNonGoalStates);
                uint64_t  probSize = probMatrix.getRowGroupCount();
                std::vector<uint64_t> disc(probSize, 0), finish(probSize, 0);

                uint64_t counter =1;

                for (uint64_t i =0; i<probSize; i++){
                    if (disc[i]==0) {
                        trajans(probMatrix, i, disc, finish, &counter);
                    }
                }


                storm::storage::BitVector cycleStates(markovianStates.size(), false);
                for (int i = 0 ; i< finish.size() ; i++){
                    auto f = finish[i];
                    for (int j =i+1; j<finish.size() ; j++){
                        if (finish[j]==f){
                            cycleStates.set(transformIndice(probabilisticNonGoalStates,i),true);
                            cycleStates.set(transformIndice(probabilisticNonGoalStates,j),true);
                        }
                    }
                }

                return cycleStates;
            }


            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
            void SparseMarkovAutomatonCslHelper::deleteProbDiagonals(storm::storage::SparseMatrix<ValueType>& transitionMatrix, storm::storage::BitVector const& markovianStates){
                auto const&  rowGroupIndices = transitionMatrix.getRowGroupIndices();

                for (uint64_t i =0; i<transitionMatrix.getRowGroupCount(); i++) {
                    if (markovianStates[i]) {
                        continue;
                    }
                    auto from = rowGroupIndices[i];
                    auto to = rowGroupIndices[i + 1];
                    for (uint64_t j = from; j < to; j++) {
                        ValueType selfLoop = 0;
                        for (auto& element: transitionMatrix.getRow(j)){
                            if (element.getColumn()==i){
                                selfLoop = element.getValue();
                            }
                        }

                        if (selfLoop==0){
                            continue;
                        }
                        for (auto& element : transitionMatrix.getRow(j)){
                            if (element.getColumn()!=i && selfLoop!=1){
                                element.setValue(element.getValue()/(1-selfLoop));
                            } else {
                                element.setValue(0);

                            }
                        }
                    }
                }
            }

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::unifPlus(OptimizationDirection dir, std::pair<double, double> const& boundsPair, std::vector<ValueType> const& exitRateVector, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates){
                STORM_LOG_TRACE("Using UnifPlus to compute bounded until probabilities.");

                std::ofstream logfile("U+logfile.txt", std::ios::app);
                ValueType maxNorm = 0;

                //bitvectors to identify different kind of states
                storm::storage::BitVector allStates(markovianStates.size(), true);

                //vectors to save calculation
                std::vector<std::vector<ValueType>> vd,vu,wu;


                //transition matrix with diagonal entries. The values can be changed during uniformisation
                std::vector<ValueType> exitRate{exitRateVector};
                typename storm::storage::SparseMatrix<ValueType> fullTransitionMatrix = transitionMatrix.getSubmatrix(true, allStates , allStates , true);
                auto rowGroupIndices = fullTransitionMatrix.getRowGroupIndices();





                //(1) define horizon, epsilon, kappa , N, lambda,
                uint64_t numberOfStates = fullTransitionMatrix.getRowGroupCount();
                double T = boundsPair.second;
                ValueType kappa = storm::utility::one<ValueType>() /10; // would be better as option-parameter
                ValueType epsilon = storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision();
                ValueType lambda = exitRateVector[0];
                for (ValueType act: exitRateVector) {
                    lambda = std::max(act, lambda);
                }
                uint64_t N;

                //calculate relative ReachabilityVectors
                std::vector<ValueType> in(fullTransitionMatrix.getRowCount(), 0);
                std::vector<std::vector<ValueType>> relReachability(numberOfStates,in);

                printTransitions(relReachability, exitRateVector, fullTransitionMatrix, markovianStates, psiStates, psiStates, psiStates, vd,vu,wu); // TODO: delete when develepmont is finished
                for(int i=0 ; i<numberOfStates; i++){
                     auto from = rowGroupIndices[i];
                     auto to = rowGroupIndices[i+1];
                     for (auto j=from ; j,to; j++){
                         std::vector<ValueType>& act = relReachability[j];
                         for(auto element: fullTransitionMatrix.getRow(j)){
                             if (markovianStates[element.getColumn()]){
                                 act[element.getColumn]=element.getValue();
                             }
                         }

                     }
                }

                    // while not close enough to precision:
                do {
                    // (2) update parameter
                    N = ceil(lambda*T*exp(2)-log(kappa*epsilon));

                    // (3) uniform  - just applied to markovian states
                    for (uint_fast64_t i = 0; i < fullTransitionMatrix.getRowGroupCount(); i++) {
                        if (!markovianStates[i]) {
                            continue;
                        }
                        uint64_t from = rowGroupIndices[i]; //markovian state -> no Nondeterminism -> only one row

                        if (exitRate[i] == lambda) {
                            continue; //already unified
                        }

                        auto line = fullTransitionMatrix.getRow(from);
                        ValueType exitOld = exitRate[i];
                        ValueType exitNew = lambda;
                        for (auto &v : line) {
                            if (v.getColumn() == i) { //diagonal element
                                ValueType newSelfLoop = exitNew - exitOld + v.getValue();
                                ValueType newRate = newSelfLoop / exitNew;
                                v.setValue(newRate);
                            } else { //modify probability
                                ValueType propOld = v.getValue();
                                ValueType propNew = propOld * exitOld / exitNew;
                                v.setValue(propNew);
                            }
                        }
                        exitRate[i] = exitNew;
                    }

                    // (4) define vectors/matrices
                    std::vector<ValueType> init(numberOfStates, -1);
                    vd = std::vector<std::vector<ValueType>> (N + 1, init);
                    vu = std::vector<std::vector<ValueType>> (N + 1, init);
                    wu = std::vector<std::vector<ValueType>> (N + 1, init);


                    // (5) calculate vectors and maxNorm
                    for (uint64_t i = 0; i < numberOfStates; i++) {
                        for (uint64_t k = N; k <= N; k--) {
                                calculateVd(dir, k, i, T*lambda, vd, fullTransitionMatrix, markovianStates, psiStates);
                                calculateWu(dir, k, i, T*lambda, wu, fullTransitionMatrix, markovianStates, psiStates);
                                calculateVu(dir, k, i, T*lambda, vu, wu, fullTransitionMatrix, markovianStates, psiStates);
                                //also use iteration to keep maxNorm of vd and vu up to date, so the loop-condition is easy to prove
                                ValueType diff = std::abs(vd[k][i]-vu[k][i]);
                                maxNorm  = std::max(maxNorm, diff);
                            }
                    }


                    // (6) double lambda 
                    lambda=2*lambda;

                } while (maxNorm>epsilon*(1-kappa));
                return vd[0];
            }

            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilitiesImca(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                STORM_LOG_TRACE("Using IMCA's technique to compute bounded until probabilities.");
                
                uint64_t numberOfStates = transitionMatrix.getRowGroupCount();
                
                // 'Unpack' the bounds to make them more easily accessible.
                double lowerBound = boundsPair.first;
                double upperBound = boundsPair.second;
                
                // (1) Compute the accuracy we need to achieve the required error bound.
                ValueType maxExitRate = 0;
                for (auto value : exitRateVector) {
                    maxExitRate = std::max(maxExitRate, value);
                }
                ValueType delta = (2 * storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision()) / (upperBound * maxExitRate * maxExitRate);
                
                // (2) Compute the number of steps we need to make for the interval.
                uint64_t numberOfSteps = static_cast<uint64_t>(std::ceil((upperBound - lowerBound) / delta));
                STORM_LOG_INFO("Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [" << lowerBound << ", " << upperBound << "]." << std::endl);
                
                // (3) Compute the non-goal states and initialize two vectors
                // * vProbabilistic holds the probability values of probabilistic non-goal states.
                // * vMarkovian holds the probability values of Markovian non-goal states.
                storm::storage::BitVector const& markovianNonGoalStates = markovianStates & ~psiStates;
                storm::storage::BitVector const& probabilisticNonGoalStates = ~markovianStates & ~psiStates;
                std::vector<ValueType> vProbabilistic(probabilisticNonGoalStates.getNumberOfSetBits());
                std::vector<ValueType> vMarkovian(markovianNonGoalStates.getNumberOfSetBits());
                
                computeBoundedReachabilityProbabilities(dir, transitionMatrix, exitRateVector, psiStates, markovianNonGoalStates, probabilisticNonGoalStates, vMarkovian, vProbabilistic, delta, numberOfSteps, minMaxLinearEquationSolverFactory);
                
                // (4) If the lower bound of interval was non-zero, we need to take the current values as the starting values for a subsequent value iteration.
                if (lowerBound != storm::utility::zero<ValueType>()) {
                    std::vector<ValueType> vAllProbabilistic((~markovianStates).getNumberOfSetBits());
                    std::vector<ValueType> vAllMarkovian(markovianStates.getNumberOfSetBits());
                    
                    // Create the starting value vectors for the next value iteration based on the results of the previous one.
                    storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, psiStates % ~markovianStates, storm::utility::one<ValueType>());
                    storm::utility::vector::setVectorValues<ValueType>(vAllProbabilistic, ~psiStates % ~markovianStates, vProbabilistic);
                    storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, psiStates % markovianStates, storm::utility::one<ValueType>());
                    storm::utility::vector::setVectorValues<ValueType>(vAllMarkovian, ~psiStates % markovianStates, vMarkovian);
                    
                    // Compute the number of steps to reach the target interval.
                    numberOfSteps = static_cast<uint64_t>(std::ceil(lowerBound / delta));
                    STORM_LOG_INFO("Performing " << numberOfSteps << " iterations (delta=" << delta << ") for interval [0, " << lowerBound << "]." << std::endl);
                    
                    // Compute the bounded reachability for interval [0, b-a].
                    computeBoundedReachabilityProbabilities(dir, transitionMatrix, exitRateVector, storm::storage::BitVector(numberOfStates), markovianStates, ~markovianStates, vAllMarkovian, vAllProbabilistic, delta, numberOfSteps, minMaxLinearEquationSolverFactory);
                    
                    // Create the result vector out of vAllProbabilistic and vAllMarkovian and return it.
                    std::vector<ValueType> result(numberOfStates, storm::utility::zero<ValueType>());
                    storm::utility::vector::setVectorValues(result, ~markovianStates, vAllProbabilistic);
                    storm::utility::vector::setVectorValues(result, markovianStates, vAllMarkovian);
                    
                    return result;
                } else {
                    // Create the result vector out of 1_G, vProbabilistic and vMarkovian and return it.
                    std::vector<ValueType> result(numberOfStates);
                    storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
                    storm::utility::vector::setVectorValues(result, probabilisticNonGoalStates, vProbabilistic);
                    storm::utility::vector::setVectorValues(result, markovianNonGoalStates, vMarkovian);
                    return result;
                }
            }
            
            template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                
                auto const& markovAutomatonSettings = storm::settings::getModule<storm::settings::modules::MarkovAutomatonSettings>();
                if (markovAutomatonSettings.getTechnique() == storm::settings::modules::MarkovAutomatonSettings::BoundedReachabilityTechnique::Imca) {
                    return computeBoundedUntilProbabilitiesImca(dir, transitionMatrix, exitRateVector, markovianStates, psiStates, boundsPair, minMaxLinearEquationSolverFactory);
                } else {
                    STORM_LOG_ASSERT(markovAutomatonSettings.getTechnique() == storm::settings::modules::MarkovAutomatonSettings::BoundedReachabilityTechnique::UnifPlus, "Unknown solution technique.");
                    
                    // Why is optimization direction not passed?
                    return unifPlus(dir, boundsPair, exitRateVector, transitionMatrix, markovianStates, psiStates);
                }
            }
              
            template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Computing bounded until probabilities is unsupported for this value type.");
            }

           
            template<typename ValueType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                return std::move(storm::modelchecker::helper::SparseMdpPrctlHelper<ValueType>::computeUntilProbabilities(dir, transitionMatrix, backwardTransitions, phiStates, psiStates, qualitative, false, minMaxLinearEquationSolverFactory).values);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                
                // Get a reward model where the state rewards are scaled accordingly
                std::vector<ValueType> stateRewardWeights(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    stateRewardWeights[markovianState] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                std::vector<ValueType> totalRewardVector = rewardModel.getTotalActionRewardVector(transitionMatrix, stateRewardWeights);
                RewardModelType scaledRewardModel(boost::none, std::move(totalRewardVector));
                
                return SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(dir, transitionMatrix, backwardTransitions, scaledRewardModel, psiStates, false, false, minMaxLinearEquationSolverFactory).values;
            }
            
            template<typename ValueType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
            
                uint64_t numberOfStates = transitionMatrix.getRowGroupCount();

                // If there are no goal states, we avoid the computation and directly return zero.
                if (psiStates.empty()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>());
                }
                
                // Likewise, if all bits are set, we can avoid the computation and set.
                if (psiStates.full()) {
                    return std::vector<ValueType>(numberOfStates, storm::utility::one<ValueType>());
                }
                
                // Otherwise, reduce the long run average probabilities to long run average rewards.
                // Every Markovian goal state gets reward one.
                std::vector<ValueType> stateRewards(transitionMatrix.getRowGroupCount(), storm::utility::zero<ValueType>());
                storm::utility::vector::setVectorValues(stateRewards, markovianStates & psiStates, storm::utility::one<ValueType>());
                storm::models::sparse::StandardRewardModel<ValueType> rewardModel(std::move(stateRewards));
                
                return computeLongRunAverageRewards(dir, transitionMatrix, backwardTransitions, exitRateVector, markovianStates, rewardModel, minMaxLinearEquationSolverFactory);
                
            }
            
            template<typename ValueType, typename RewardModelType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                
                uint64_t numberOfStates = transitionMatrix.getRowGroupCount();

                // Start by decomposing the Markov automaton into its MECs.
                storm::storage::MaximalEndComponentDecomposition<ValueType> mecDecomposition(transitionMatrix, backwardTransitions);
                
                // Get some data members for convenience.
                std::vector<uint64_t> const& nondeterministicChoiceIndices = transitionMatrix.getRowGroupIndices();
                
                // Now start with compute the long-run average for all end components in isolation.
                std::vector<ValueType> lraValuesForEndComponents;
                
                // While doing so, we already gather some information for the following steps.
                std::vector<uint64_t> stateToMecIndexMap(numberOfStates);
                storm::storage::BitVector statesInMecs(numberOfStates);
                
                for (uint64_t currentMecIndex = 0; currentMecIndex < mecDecomposition.size(); ++currentMecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[currentMecIndex];
                    
                    // Gather information for later use.
                    for (auto const& stateChoicesPair : mec) {
                        uint64_t state = stateChoicesPair.first;
                        
                        statesInMecs.set(state);
                        stateToMecIndexMap[state] = currentMecIndex;
                    }
                    
                    // Compute the LRA value for the current MEC.
                    lraValuesForEndComponents.push_back(computeLraForMaximalEndComponent(dir, transitionMatrix, exitRateVector, markovianStates, rewardModel, mec, minMaxLinearEquationSolverFactory));
                }
                
                // For fast transition rewriting, we build some auxiliary data structures.
                storm::storage::BitVector statesNotContainedInAnyMec = ~statesInMecs;
                uint64_t firstAuxiliaryStateIndex = statesNotContainedInAnyMec.getNumberOfSetBits();
                uint64_t lastStateNotInMecs = 0;
                uint64_t numberOfStatesNotInMecs = 0;
                std::vector<uint64_t> statesNotInMecsBeforeIndex;
                statesNotInMecsBeforeIndex.reserve(numberOfStates);
                for (auto state : statesNotContainedInAnyMec) {
                    while (lastStateNotInMecs <= state) {
                        statesNotInMecsBeforeIndex.push_back(numberOfStatesNotInMecs);
                        ++lastStateNotInMecs;
                    }
                    ++numberOfStatesNotInMecs;
                }
                uint64_t numberOfSspStates = numberOfStatesNotInMecs + mecDecomposition.size();
                
                // Finally, we are ready to create the SSP matrix and right-hand side of the SSP.
                std::vector<ValueType> b;
                typename storm::storage::SparseMatrixBuilder<ValueType> sspMatrixBuilder(0, numberOfSspStates , 0, false, true, numberOfSspStates);
                
                // If the source state is not contained in any MEC, we copy its choices (and perform the necessary modifications).
                uint64_t currentChoice = 0;
                for (auto state : statesNotContainedInAnyMec) {
                    sspMatrixBuilder.newRowGroup(currentChoice);
                    
                    for (uint64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice, ++currentChoice) {
                        std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                        b.push_back(storm::utility::zero<ValueType>());
                        
                        for (auto element : transitionMatrix.getRow(choice)) {
                            if (statesNotContainedInAnyMec.get(element.getColumn())) {
                                // If the target state is not contained in an MEC, we can copy over the entry.
                                sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                            } else {
                                // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                // so that we are able to write the cumulative probability to the MEC into the matrix.
                                auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
                            }
                        }
                        
                        // Now insert all (cumulative) probability values that target an MEC.
                        for (uint64_t mecIndex = 0; mecIndex < auxiliaryStateToProbabilityMap.size(); ++mecIndex) {
                            if (auxiliaryStateToProbabilityMap[mecIndex] != 0) {
                                sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + mecIndex, auxiliaryStateToProbabilityMap[mecIndex]);
                            }
                        }
                    }
                }
                
                // Now we are ready to construct the choices for the auxiliary states.
                for (uint64_t mecIndex = 0; mecIndex < mecDecomposition.size(); ++mecIndex) {
                    storm::storage::MaximalEndComponent const& mec = mecDecomposition[mecIndex];
                    sspMatrixBuilder.newRowGroup(currentChoice);
                    
                    for (auto const& stateChoicesPair : mec) {
                        uint64_t state = stateChoicesPair.first;
                        boost::container::flat_set<uint64_t> const& choicesInMec = stateChoicesPair.second;
                        
                        for (uint64_t choice = nondeterministicChoiceIndices[state]; choice < nondeterministicChoiceIndices[state + 1]; ++choice) {
                            
                            // If the choice is not contained in the MEC itself, we have to add a similar distribution to the auxiliary state.
                            if (choicesInMec.find(choice) == choicesInMec.end()) {
                                std::vector<ValueType> auxiliaryStateToProbabilityMap(mecDecomposition.size());
                                b.push_back(storm::utility::zero<ValueType>());
                                
                                for (auto element : transitionMatrix.getRow(choice)) {
                                    if (statesNotContainedInAnyMec.get(element.getColumn())) {
                                        // If the target state is not contained in an MEC, we can copy over the entry.
                                        sspMatrixBuilder.addNextValue(currentChoice, statesNotInMecsBeforeIndex[element.getColumn()], element.getValue());
                                    } else {
                                        // If the target state is contained in MEC i, we need to add the probability to the corresponding field in the vector
                                        // so that we are able to write the cumulative probability to the MEC into the matrix.
                                        auxiliaryStateToProbabilityMap[stateToMecIndexMap[element.getColumn()]] += element.getValue();
                                    }
                                }
                                
                                // Now insert all (cumulative) probability values that target an MEC.
                                for (uint64_t targetMecIndex = 0; targetMecIndex < auxiliaryStateToProbabilityMap.size(); ++targetMecIndex) {
                                    if (auxiliaryStateToProbabilityMap[targetMecIndex] != 0) {
                                        sspMatrixBuilder.addNextValue(currentChoice, firstAuxiliaryStateIndex + targetMecIndex, auxiliaryStateToProbabilityMap[targetMecIndex]);
                                    }
                                }
                                
                                ++currentChoice;
                            }
                        }
                    }
                    
                    // For each auxiliary state, there is the option to achieve the reward value of the LRA associated with the MEC.
                    ++currentChoice;
                    b.push_back(lraValuesForEndComponents[mecIndex]);
                }
                
                // Finalize the matrix and solve the corresponding system of equations.
                storm::storage::SparseMatrix<ValueType> sspMatrix = sspMatrixBuilder.build(currentChoice, numberOfSspStates, numberOfSspStates);
                
                std::vector<ValueType> x(numberOfSspStates);
                
                // Check for requirements of the solver.
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(true, dir);
                requirements.clearBounds();
                STORM_LOG_THROW(requirements.empty(), storm::exceptions::UncheckedRequirementException, "Cannot establish requirements for solver.");

                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = minMaxLinearEquationSolverFactory.create(sspMatrix);
                solver->setHasUniqueSolution();
                solver->setLowerBound(storm::utility::zero<ValueType>());
                solver->setUpperBound(*std::max_element(lraValuesForEndComponents.begin(), lraValuesForEndComponents.end()));
                solver->setRequirementsChecked();
                solver->solveEquations(dir, x, b);
                
                // Prepare result vector.
                std::vector<ValueType> result(numberOfStates);
                
                // Set the values for states not contained in MECs.
                storm::utility::vector::setVectorValues(result, statesNotContainedInAnyMec, x);
                
                // Set the values for all states in MECs.
                for (auto state : statesInMecs) {
                    result[state] = x[firstAuxiliaryStateIndex + stateToMecIndexMap[state]];
                }
                
                return result;
            }
            
            template <typename ValueType>
            std::vector<ValueType> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                
                // Get a reward model representing expected sojourn times
                std::vector<ValueType> rewardValues(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
                for (auto const markovianState : markovianStates) {
                    rewardValues[transitionMatrix.getRowGroupIndices()[markovianState]] = storm::utility::one<ValueType>() / exitRateVector[markovianState];
                }
                storm::models::sparse::StandardRewardModel<ValueType> rewardModel(boost::none, std::move(rewardValues));
                
                return SparseMdpPrctlHelper<ValueType>::computeReachabilityRewards(dir, transitionMatrix, backwardTransitions, rewardModel, psiStates, false, false, minMaxLinearEquationSolverFactory).values;
            }

            template<typename ValueType, typename RewardModelType>
            ValueType SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                
                // If the mec only consists of a single state, we compute the LRA value directly
                if (++mec.begin() == mec.end()) {
                    uint64_t state = mec.begin()->first;
                    STORM_LOG_THROW(markovianStates.get(state), storm::exceptions::InvalidOperationException, "Markov Automaton has Zeno behavior. Computation of Long Run Average values not supported.");
                    ValueType result = rewardModel.hasStateRewards() ? rewardModel.getStateReward(state) : storm::utility::zero<ValueType>();
                    if (rewardModel.hasStateActionRewards() || rewardModel.hasTransitionRewards()) {
                        STORM_LOG_ASSERT(mec.begin()->second.size() == 1, "Markovian state has nondeterministic behavior.");
                        uint64_t choice = *mec.begin()->second.begin();
                        result += exitRateVector[state] * rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, storm::utility::zero<ValueType>());
                    }
                    return result;
                }
                
                // Solve MEC with the method specified in the settings
                storm::solver::LraMethod method = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>().getLraMethod();
                if (method == storm::solver::LraMethod::LinearProgramming) {
                    return computeLraForMaximalEndComponentLP(dir, transitionMatrix, exitRateVector, markovianStates, rewardModel, mec);
                } else if (method == storm::solver::LraMethod::ValueIteration) {
                    return computeLraForMaximalEndComponentVI(dir, transitionMatrix, exitRateVector, markovianStates, rewardModel, mec, minMaxLinearEquationSolverFactory);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            ValueType SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec) {
                std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> lpSolverFactory(new storm::utility::solver::LpSolverFactory<ValueType>());
                std::unique_ptr<storm::solver::LpSolver<ValueType>> solver = lpSolverFactory->create("LRA for MEC");
                solver->setOptimizationDirection(invert(dir));
                
                // First, we need to create the variables for the problem.
                std::map<uint64_t, storm::expressions::Variable> stateToVariableMap;
                for (auto const& stateChoicesPair : mec) {
                    std::string variableName = "x" + std::to_string(stateChoicesPair.first);
                    stateToVariableMap[stateChoicesPair.first] = solver->addUnboundedContinuousVariable(variableName);
                }
                storm::expressions::Variable k = solver->addUnboundedContinuousVariable("k", storm::utility::one<ValueType>());
                solver->update();
                
                // Now we encode the problem as constraints.
                std::vector<uint64_t> const& nondeterministicChoiceIndices = transitionMatrix.getRowGroupIndices();
                for (auto const& stateChoicesPair : mec) {
                    uint64_t state = stateChoicesPair.first;
                    
                    // Now, based on the type of the state, create a suitable constraint.
                    if (markovianStates.get(state)) {
                        STORM_LOG_ASSERT(stateChoicesPair.second.size() == 1, "Markovian state " << state << " is not deterministic: It has " << stateChoicesPair.second.size() << " choices.");
                        uint64_t choice = *stateChoicesPair.second.begin();
                        
                        storm::expressions::Expression constraint = stateToVariableMap.at(state);
                        
                        for (auto element : transitionMatrix.getRow(nondeterministicChoiceIndices[state])) {
                            constraint = constraint - stateToVariableMap.at(element.getColumn()) * solver->getManager().rational((element.getValue()));
                        }
                        
                        constraint = constraint + solver->getManager().rational(storm::utility::one<ValueType>() / exitRateVector[state]) * k;
                        
                        storm::expressions::Expression rightHandSide = solver->getManager().rational(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, (ValueType) (storm::utility::one<ValueType>() / exitRateVector[state])));
                        if (dir == OptimizationDirection::Minimize) {
                            constraint = constraint <= rightHandSide;
                        } else {
                            constraint = constraint >= rightHandSide;
                        }
                        solver->addConstraint("state" + std::to_string(state), constraint);
                    } else {
                        // For probabilistic states, we want to add the constraint x_s <= sum P(s, a, s') * x_s' where a is the current action
                        // and the sum ranges over all states s'.
                        for (auto choice : stateChoicesPair.second) {
                            storm::expressions::Expression constraint = stateToVariableMap.at(state);
                            
                            for (auto element : transitionMatrix.getRow(choice)) {
                                constraint = constraint - stateToVariableMap.at(element.getColumn()) * solver->getManager().rational(element.getValue());
                            }

                            storm::expressions::Expression rightHandSide = solver->getManager().rational(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, storm::utility::zero<ValueType>()));
                            if (dir == OptimizationDirection::Minimize) {
                                constraint = constraint <= rightHandSide;
                            } else {
                                constraint = constraint >= rightHandSide;
                            }
                            solver->addConstraint("state" + std::to_string(state), constraint);
                        }
                    }
                }
                
                solver->optimize();
                return solver->getContinuousValue(k);
            }
            
            template<typename ValueType, typename RewardModelType>
            ValueType SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory) {
                
                // Initialize data about the mec
                
                storm::storage::BitVector mecStates(transitionMatrix.getRowGroupCount(), false);
                storm::storage::BitVector mecChoices(transitionMatrix.getRowCount(), false);
                for (auto const& stateChoicesPair : mec) {
                    mecStates.set(stateChoicesPair.first);
                    for (auto const& choice : stateChoicesPair.second) {
                        mecChoices.set(choice);
                    }
                }
                storm::storage::BitVector markovianMecStates = mecStates & markovianStates;
                storm::storage::BitVector probabilisticMecStates = mecStates & ~markovianStates;
                storm::storage::BitVector probabilisticMecChoices = transitionMatrix.getRowFilter(probabilisticMecStates) & mecChoices;
                STORM_LOG_THROW(!markovianMecStates.empty(), storm::exceptions::InvalidOperationException, "Markov Automaton has Zeno behavior. Computation of Long Run Average values not supported.");
                
                // Get the uniformization rate
                
                ValueType uniformizationRate = storm::utility::vector::max_if(exitRateVector, markovianMecStates);
                // To ensure that the model is aperiodic, we need to make sure that every Markovian state gets a self loop.
                // Hence, we increase the uniformization rate a little.
                uniformizationRate += storm::utility::one<ValueType>(); // Todo: try other values such as *=1.01

                // Get the transitions of the submodel, that is
                // * a matrix aMarkovian with all (uniformized) transitions from Markovian mec states to all Markovian mec states.
                // * a matrix aMarkovianToProbabilistic with all (uniformized) transitions from Markovian mec states to all probabilistic mec states.
                // * a matrix aProbabilistic with all transitions from probabilistic mec states to other probabilistic mec states.
                // * a matrix aProbabilisticToMarkovian with all  transitions from probabilistic mec states to all Markovian mec states.
                typename storm::storage::SparseMatrix<ValueType> aMarkovian = transitionMatrix.getSubmatrix(true, markovianMecStates, markovianMecStates, true);
                typename storm::storage::SparseMatrix<ValueType> aMarkovianToProbabilistic = transitionMatrix.getSubmatrix(true, markovianMecStates, probabilisticMecStates);
                typename storm::storage::SparseMatrix<ValueType> aProbabilistic = transitionMatrix.getSubmatrix(false, probabilisticMecChoices, probabilisticMecStates);
                typename storm::storage::SparseMatrix<ValueType> aProbabilisticToMarkovian = transitionMatrix.getSubmatrix(false, probabilisticMecChoices, markovianMecStates);
                // The matrices with transitions from Markovian states need to be uniformized.
                uint64_t subState = 0;
                for (auto state : markovianMecStates) {
                    ValueType uniformizationFactor = exitRateVector[state] / uniformizationRate;
                    for (auto& entry : aMarkovianToProbabilistic.getRow(subState)) {
                        entry.setValue(entry.getValue() * uniformizationFactor);
                    }
                    for (auto& entry : aMarkovian.getRow(subState)) {
                        if (entry.getColumn() == subState) {
                            entry.setValue(storm::utility::one<ValueType>() - uniformizationFactor * (storm::utility::one<ValueType>() - entry.getValue()));
                        } else {
                            entry.setValue(entry.getValue() * uniformizationFactor);
                        }
                    }
                    ++subState;
                }

                // Compute the rewards obtained in a single uniformization step
                
                std::vector<ValueType> markovianChoiceRewards;
                markovianChoiceRewards.reserve(aMarkovian.getRowCount());
                for (auto const& state : markovianMecStates) {
                    ValueType stateRewardScalingFactor = storm::utility::one<ValueType>() / uniformizationRate;
                    ValueType actionRewardScalingFactor = exitRateVector[state] / uniformizationRate;
                    assert(transitionMatrix.getRowGroupSize(state) == 1);
                    uint64_t choice = transitionMatrix.getRowGroupIndices()[state];
                    markovianChoiceRewards.push_back(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, stateRewardScalingFactor, actionRewardScalingFactor));
                }
                
                std::vector<ValueType> probabilisticChoiceRewards;
                probabilisticChoiceRewards.reserve(aProbabilistic.getRowCount());
                for (auto const& state : probabilisticMecStates) {
                    uint64_t groupStart = transitionMatrix.getRowGroupIndices()[state];
                    uint64_t groupEnd = transitionMatrix.getRowGroupIndices()[state + 1];
                    for (uint64_t choice = probabilisticMecChoices.getNextSetIndex(groupStart); choice < groupEnd; choice = probabilisticMecChoices.getNextSetIndex(choice + 1)) {
                        probabilisticChoiceRewards.push_back(rewardModel.getTotalStateActionReward(state, choice, transitionMatrix, storm::utility::zero<ValueType>()));
                    }
                }
                
                // start the iterations
                
                ValueType precision = storm::utility::convertNumber<ValueType>(storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>().getPrecision()) / uniformizationRate;
                std::vector<ValueType> v(aMarkovian.getRowCount(), storm::utility::zero<ValueType>());
                std::vector<ValueType> w = v;
                std::vector<ValueType> x(aProbabilistic.getRowGroupCount(), storm::utility::zero<ValueType>());
                std::vector<ValueType> b = probabilisticChoiceRewards;
                
                // Check for requirements of the solver.
                // The solution is unique as we assume non-zeno MAs.
                storm::solver::MinMaxLinearEquationSolverRequirements requirements = minMaxLinearEquationSolverFactory.getRequirements(true, dir);
                requirements.clearLowerBounds();
                STORM_LOG_THROW(requirements.empty(), storm::exceptions::UncheckedRequirementException, "Cannot establish requirements for solver.");

                auto solver = minMaxLinearEquationSolverFactory.create(std::move(aProbabilistic));
                solver->setLowerBound(storm::utility::zero<ValueType>());
                solver->setHasUniqueSolution(true);
                solver->setRequirementsChecked(true);
                solver->setCachingEnabled(true);
                
                while (true) {
                    // Compute the expected total rewards for the probabilistic states
                    solver->solveEquations(dir, x, b);
                    
                    // now compute the values for the markovian states. We also keep track of the maximal and minimal difference between two values (for convergence checking)
                    auto vIt = v.begin();
                    uint64_t row = 0;
                    ValueType newValue = markovianChoiceRewards[row] + aMarkovianToProbabilistic.multiplyRowWithVector(row, x) + aMarkovian.multiplyRowWithVector(row, w);
                    ValueType maxDiff = newValue - *vIt;
                    ValueType minDiff = maxDiff;
                    *vIt = newValue;
                    for (++vIt, ++row; row < aMarkovian.getRowCount(); ++vIt, ++row) {
                        newValue = markovianChoiceRewards[row] + aMarkovianToProbabilistic.multiplyRowWithVector(row, x) + aMarkovian.multiplyRowWithVector(row, w);
                        ValueType diff = newValue - *vIt;
                        maxDiff = std::max(maxDiff, diff);
                        minDiff = std::min(minDiff, diff);
                        *vIt = newValue;
                    }

                    // Check for convergence
                    if (maxDiff - minDiff < precision) {
                        break;
                    }
                    
                    // update the rhs of the MinMax equation system
                    ValueType referenceValue = v.front();
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(v, w, [&referenceValue] (ValueType const& v_i) -> ValueType { return v_i - referenceValue; });
                    aProbabilisticToMarkovian.multiplyWithVector(w, b);
                    storm::utility::vector::addVectors(b, probabilisticChoiceRewards, b);

                }
                return v.front() * uniformizationRate;
            
            }
            
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
                
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
                
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);

            template std::vector<double> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
                
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
            
            template std::vector<double> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::SparseMatrix<double> const& backwardTransitions, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
                
            template void SparseMarkovAutomatonCslHelper::computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<double>& markovianNonGoalValues, std::vector<double>& probabilisticNonGoalValues, double delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
                
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
                
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template double SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(OptimizationDirection dir, storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<double> const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<double> const& minMaxLinearEquationSolverFactory);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeBoundedUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
                
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeUntilProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
                
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeReachabilityRewards(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);

            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeLongRunAverageProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeLongRunAverageRewards(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
            
            template std::vector<storm::RationalNumber> SparseMarkovAutomatonCslHelper::computeReachabilityTimes(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, storm::storage::SparseMatrix<storm::RationalNumber> const& backwardTransitions, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
                
            template void SparseMarkovAutomatonCslHelper::computeBoundedReachabilityProbabilities(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<storm::RationalNumber>& markovianNonGoalValues, std::vector<storm::RationalNumber>& probabilisticNonGoalValues, storm::RationalNumber delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
                
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponent(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
            
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentLP(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec);
            
            template storm::RationalNumber SparseMarkovAutomatonCslHelper::computeLraForMaximalEndComponentVI(OptimizationDirection dir, storm::storage::SparseMatrix<storm::RationalNumber> const& transitionMatrix, std::vector<storm::RationalNumber> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::models::sparse::StandardRewardModel<storm::RationalNumber> const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<storm::RationalNumber> const& minMaxLinearEquationSolverFactory);
                
        }
    }
}
