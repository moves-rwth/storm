#ifndef STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_

#include "storm/storage/BitVector.h"
#include "storm/storage/MaximalEndComponent.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/utility/NumberTraits.h"

namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace helper {
            
            class SparseMarkovAutomatonCslHelper {
            public:

                /*!
                 * Computes TBU according to the UnifPlus algorithm
                 *
                 * @param boundsPair With precondition that the first component is 0, the second one gives the time bound
                 * @param exitRateVector the exit-rates of the given MA
                 * @param transitionMatrix the transitions of the given MA
                 * @param markovianStates bitvector refering to the markovian states
                 * @param psiStates bitvector refering to the goal states
                 *
                 * @return the probability vector
                 *
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static std::vector<ValueType> unifPlus(Environment const& env, OptimizationDirection dir, std::pair<double, double> const& boundsPair, std::vector<ValueType> const& exitRateVector, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilitiesImca(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static std::vector<ValueType> computeBoundedUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::pair<double, double> const& boundsPair, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType>
                static std::vector<ValueType> computeUntilProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool qualitative, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeReachabilityRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);

                
                template <typename ValueType>
                static std::vector<ValueType> computeLongRunAverageProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType, typename RewardModelType>
                static std::vector<ValueType> computeLongRunAverageRewards(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                template <typename ValueType>
                static std::vector<ValueType> computeReachabilityTimes(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            private:
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void identify(storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates,storm::storage::BitVector const& psiStates);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void calculateUnifPlusVector(Environment const& env, uint64_t k, uint64_t node, uint64_t const kind, ValueType lambda, uint64_t probSize, std::vector<std::vector<ValueType>> const& relativeReachability, OptimizationDirection dir, std::vector<std::vector<std::vector<ValueType>>>& unifVectors, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> const& solver, std::ofstream& logfile, std::vector<double> const& poisson);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void deleteProbDiagonals(storm::storage::SparseMatrix<ValueType>& transitionMatrix, storm::storage::BitVector const& markovianStates);

                static uint64_t transformIndice(storm::storage::BitVector const& subset, uint64_t fakeId){
                    uint64_t id =0;
                    uint64_t counter =0;
                    while(counter<=fakeId){
                        if(subset[id]){
                            counter++;
                        }
                        id++;
                    }
                    return id-1;
                }

                    template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static storm::storage::BitVector identifyProbCyclesGoalStates(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,  storm::storage::BitVector const& cycleStates);


                    template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static storm::storage::BitVector identifyProbCycles(storm::storage::SparseMatrix<ValueType> const& TransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates);

                //TODO: move this

                typedef struct FoxGlynn
                {
                    int left;
                    int right;
                    double total_weight;
                    double *weights;
                } FoxGlynn;

                static std::vector<double> foxGlynnProb(double lambdaT, int N, double precision){

                    FoxGlynn* fg = NULL;
                    if(!fox_glynn(lambdaT, DBL_MIN, DBL_MAX, precision, &fg)) {
                        printf("ERROR: fox-glynn failed\n");
                        return std::vector<double>{};
                    }

                    long double sumOfPoissonProbs = 0.0;
                    std::vector<double> poisson_p(N,0.0);
                    unsigned long iter_num;

                    std::cout << "fg left " << fg->left << " fh right " << fg->right <<"\n";
                    //for(int i=fg->left; i<=fg->right; i++) {
                    for (int i = 0; i<N ; i++){
                        poisson_p[i] = fg->weights[i-fg->left]/fg->total_weight;
                        sumOfPoissonProbs+=poisson_p[i];
                    }

                    for(int i=fg->left-1; i>=0; i--) {
                        poisson_p[i] = poisson_p[i+1]*((i+1)/(lambdaT));
                        sumOfPoissonProbs+=poisson_p[i];
                    }

                    iter_num = fg->right;

                    freeFG(fg);

                    return poisson_p;
                }

                static bool finder(const int m, const double lambda, const double tau, const double omega,
                                   const double epsilon, double * pw_m, FoxGlynn *pFG)
                {
                    /*The pi constant*/
                    static const double pi = 3.14159265358979323846264;
                    static const double lambda_25 = 25.0;
                    static const double lambda_400 = 40;

                    const double sqrt_2_pi = sqrt( 2.0 * pi );
                    const double sqrt_2 = sqrt(2.0);
                    const double sqrt_lambda = sqrt(lambda);
                    double lambda_max, k, k_rtp = HUGE_VAL, k_prime, c_m_inf, result, al, dkl, bl;

                    /*Simple bad cases, when we quit*/
                    if( lambda == 0.0 )
                    {
                        printf("ERROR: Fox-Glynn: lambda = 0, terminating the algorithm\n");
                        return false;
                    }
                    /* The requested error level must not be smaller than the minimum machine precision
                       (needed to guarantee the convergence of the error conditions) */
                    if( epsilon < tau)
                    {
                        printf("ERROR: Fox-Glynn: epsilon < tau, invalid error level, terminating the algorithm\n");
                        printf("epsilon %f, tau %f\n",epsilon,tau);
                        return false;
                    }
                    /* zero is used as left truncation point for lambda <= 25 */
                    pFG->left = 0;
                    lambda_max = lambda;

                    /* for lambda below 25 the exponential can be smaller than tau */
                    /* if that is the case we expect underflows and warn the user */
                    if( 0.0 < lambda && lambda <= lambda_25 )
                    {
                        if( exp( -lambda ) <= tau )
                        {
                            printf("ERROR: Fox-Glynn: 0 < lambda < 25, underflow. The results are UNRELIABLE.\n");
                        }
                    }

                    bl = (1.0 + 1.0/lambda) * exp(1.0 / (8.0 * lambda));

                    /****Compute pFG->right truncation point****/
                    /*According to Fox-Glynn, if lambda < 400 we should take lambda = 400,
                      otherwise use the original value. This is for computing the right truncation point*/
                    if(lambda < lambda_400)
                        lambda_max = lambda_400;
                    k = 4;
                    al = (1.0+1.0/lambda_max) * exp(1.0/16.0) * sqrt_2;
                    dkl = exp(-2.0/9.0 * (k * sqrt(2.0 * lambda_max) + 1.5 ));
                    dkl = 1.0 / (1.0 - dkl);
                    /* find right truncation point */

                    /* This loop is a modification to the original Fox-Glynn paper.
                       The search for the right truncation point is only terminated by
                       the error condition and not by the stop index from the FG paper.
                       This can yield more accurate results if neccesary.*/
                    while((epsilon/2.0) < ((al * dkl * exp(-(k*k)/2.0))/(k*sqrt_2_pi)))
                    {
                        k++;
                        dkl = exp(-2.0/9.0 * (k * sqrt_2 * sqrt(lambda_max) + 1.5 ));
                        dkl = 1.0 / (1.0 - dkl);
                    }
                    k_rtp = k;
                    pFG->right = (int)ceil(m + k_rtp * sqrt_2 * sqrt(lambda_max) + 1.5 );


                    /****Compute pFG->left truncation point****/
                    /* compute the left truncation point for lambda > 25 */
                    /* for lambda <= 25 we use zero as left truncation point */
                    if(lambda > lambda_25)
                    {
                        /*Start looking for the left truncation point*/
                        /* start search at k=4 (taken from original Fox-Glynn paper */
                        k = 4;
                        /* increase the left truncation point as long as we fulfill the error condition */

                        /* This loop is a modification to the original Fox-Glynn paper.
                           The search for the left truncation point is only terminated by
                           the error condition and not by the stop index from the FG paper.
                           This can yield more accurate results if neccesary.*/
                        while((epsilon/2.0) < ((bl * exp(-(k*k)/2.0))/(k * sqrt_2_pi)))
                            k++;
                        /*Finally the left truncation point is found*/
                        pFG->left = (int)floor(m - k*sqrt(lambda)- 1.5 );
                        /* for small lambda the above calculation can yield negative truncation points, crop them here */
                        if(pFG->left < 0)
                            pFG->left = 0;
                        /* perform underflow check */
                        k_prime = k + 3.0 / (2.0 * sqrt_lambda);
                        /*We take the c_m_inf = 0.02935 / sqrt( m ), as for lambda >= 25
                         c_m = 1 / ( sqrt( 2.0 * pi * m ) ) * exp( m - lambda - 1 / ( 12.0 * m ) ) => c_m_inf*/
                        c_m_inf = 0.02935 / sqrt((double) m);
                        result = 0.0;
                        if( 0.0 < k_prime && k_prime <= sqrt_lambda / 2.0 )
                        {
                            result = c_m_inf * exp( - pow(k_prime,2.0) / 2.0 - pow(k_prime, 3.0) / (3.0 * sqrt_lambda) );
                        }
                        else
                        {
                            if( k_prime <= sqrt( m + 1.0 ) / m )
                            {
                                double result_1 = c_m_inf * pow(
                                        1.0 - k_prime / sqrt((double) (m + 1)),
                                        k_prime * sqrt((double) (m + 1)));
                                double result_2 = exp( - lambda );
                                /*Take the maximum*/
                                result = ( result_1 > result_2 ? result_1 : result_2);
                            }
                            else
                            {
                                /*NOTE: It will be an underflow error*/;
                                printf("ERROR: Fox-Glynn: lambda >= 25, underflow. The results are UNRELIABLE.\n");
                            }
                        }
                        if ( result * omega / ( 1.0e+10 * ( pFG->right - pFG->left ) ) <= tau )
                        {
                            printf("ERROR: Fox-Glynn: lambda >= 25, underflow. The results are UNRELIABLE.\n");
                        }
                    }



                    /*We still have to perform an underflow check for the right truncation point when lambda >= 400*/
                    if( lambda >= lambda_400 )
                    {
                        k_prime = k_rtp * sqrt_2 + 3.0 / (2.0 * sqrt_lambda);
                        /*We take the c_m_inf = 0.02935 / sqrt( m ), as for lambda >= 25
                         c_m = 1 / ( sqrt( 2.0 * pi * m ) ) * exp( m - lambda - 1 / ( 12.0 * m ) ) => c_m_inf*/
                        c_m_inf = 0.02935 / sqrt((double) m);
                        result = c_m_inf * exp( - pow( k_prime + 1.0 , 2.0 ) / 2.0 );
                        if( result * omega / ( 1.0e+10 * ( pFG->right - pFG->left ) ) <= tau)
                        {
                            printf("ERROR: Fox-Glynn: lambda >= 400, underflow. The results are UNRELIABLE.\n");
                        }
                    }
                    /*Time to set the initial value for weights*/
                    *pw_m = omega / ( 1.0e+10 * ( pFG->right - pFG->left ) );

                    return true;
                }

/*****************************************************************************
Name		: weighter
Role		: The WEIGHTER function from the Fox-Glynn algorithm
@param		: double lambda: (rate of uniformization)*(mission time)
@param		: double tau: underflow
@param		: double omega: overflow
@param		: double epsilon: error bound
@param		: FoxGlynn *: return by reference
@return		: TRUE if everything is fine, otherwise FALSE.
              This is the F parameter of Fox-Glynn finder function.
remark	    :
******************************************************************************/
                static bool weighter(const double lambda, const double tau, const double omega, const double epsilon, FoxGlynn *pFG)
                {
                    static const double pi = 3.14159265358979323846264;
                    static const double lambda_25 = 25.0;
                    static const double lambda_400 = 40;
                    /*The magic m point*/
                    const int m = (int)floor(lambda);
                    double w_m = 0;
                    int j, s, t;

                    if( ! finder( m, lambda, tau, omega, epsilon, &w_m, pFG ) )
                        return false;

                    /*Allocate space for weights*/
                    pFG->weights = (double *) calloc((size_t) (pFG->right - pFG->left + 1),
                                                     sizeof(double));
                    /*Set an initial weight*/
                    pFG->weights[ m - pFG->left ] = w_m;

                    /*Fill the left side of the array*/
                    for( j = m; j > pFG->left; j-- )
                        pFG->weights[ ( j - pFG->left ) - 1  ] = ( j / lambda ) * pFG->weights[ j - pFG->left ];

                    /*Fill the right side of the array, have two cases lambda < 400 & lambda >= 400*/
                    if( lambda < lambda_400 )
                    {
                        /*Perform the underflow check, according to Fox-Glynn*/
                        if( pFG->right > 600 )
                        {
                            printf("ERROR: Fox-Glynn: pFG->right > 600, underflow is possible\n");
                            return false;
                        }
                        /*Compute weights*/
                        for( j = m; j < pFG->right; j++ )
                        {
                            double q = lambda / ( j + 1 );
                            if( pFG->weights[ j - pFG->left ] > tau / q )
                            {
                                pFG->weights[ ( j - pFG->left ) + 1  ] = q * pFG->weights[ j - pFG->left ];
                            }else{
                                pFG->right = j;
                                break; /*It's time to compute W*/
                            }
                        }
                    }else{
                        /*Compute weights*/
                        for( j = m; j < pFG->right; j++ )
                            pFG->weights[ ( j - pFG->left ) + 1  ] = ( lambda / ( j + 1 ) ) * pFG->weights[ j - pFG->left ];
                    }

                    /*It is time to compute the normalization weight W*/
                    pFG->total_weight = 0.0;
                    s = pFG->left;
                    t = pFG->right;

                    while( s < t )
                    {
                        if( pFG->weights[ s - pFG->left ] <= pFG->weights[ t - pFG->left ] )
                        {
                            pFG->total_weight += pFG->weights[ s - pFG->left ];
                            s++;
                        }else{
                            pFG->total_weight += pFG->weights[ t - pFG->left ];
                            t--;
                        }
                    }
                    pFG->total_weight += pFG->weights[ s - pFG->left ];

                    /* printf("Fox-Glynn: ltp = %d, rtp = %d, w = %10.15le \n", pFG->left, pFG->right, pFG->total_weight); */

                    return true;
                }

/*****************************************************************************
Name		: fox_glynn
Role		: get poisson probabilities.
@param		: double lambda: (rate of uniformization)*(mission time)
@param		: double tau: underflow
@param		: double omega: overflow
@param		: double epsilon: error bound
@param		: FoxGlynn **: return a new FoxGlynn structure by reference
@return	: TRUE if it worked fine, otherwise false
remark		:
******************************************************************************/
                static bool fox_glynn(const double lambda, const double tau, const double omega, const double epsilon, FoxGlynn **ppFG)
                {
                    /* printf("Fox-Glynn: lambda = %3.3le, epsilon = %1.8le\n",lambda, epsilon); */

                    *ppFG = (FoxGlynn *) calloc((size_t) 1, sizeof(FoxGlynn));
                    (*ppFG)->weights = NULL;

                    return weighter(lambda, tau, omega, epsilon, *ppFG);
                }


/**
* Fries the memory allocated for the FoxGlynn structure
* @param fg the structure to free
*/
                static void freeFG(FoxGlynn * fg)
                {
                    if( fg ){
                        if( fg->weights )
                            free(fg->weights);
                        free(fg);
                    }
                }

                /*!
                 * Computes the poission-distribution
                 *
                 *
                 * @param parameter lambda to use
                 * @param point i
                 * TODO: replace with Fox-Lynn
                 * @return the probability
                 */


                /*!
                 * Computes the poission-distribution
                 *
                 *
                 * @param parameter lambda to use
                 * @param point i
                 * TODO: replace with Fox-glynn
                 * @return the probability
                 */
                template <typename ValueType>
                static ValueType poisson(ValueType lambda, uint64_t i);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static uint64_t trajans(storm::storage::SparseMatrix<ValueType> const& TransitionMatrix, uint64_t node, std::vector<uint64_t>& disc, std::vector<uint64_t>& finish, uint64_t * counter);

                /*
                 * Computes vu vector according to UnifPlus
                 *
                 */
                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void calculateVu(Environment const& env, std::vector<std::vector<ValueType>> const& relativeReachability, OptimizationDirection dir, uint64_t k, uint64_t node, uint64_t const kind, ValueType lambda, uint64_t probSize, std::vector<std::vector<std::vector<ValueType>>>& unifVectors, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, storm::storage::BitVector const& markovianStates, storm::storage::BitVector const& psiStates, std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> const& solver, std::ofstream& logfile, std::vector<double> const& poisson);




                /*!
                 * Prints the TransitionMatrix and the vectors vd, vu, wu to the logfile
                 * TODO: delete when development is finished
                 *
                 */

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type=0>
                static void printTransitions(const uint64_t  N, ValueType const diff, storm::storage::SparseMatrix<ValueType> const& fullTransitionMatrix, std::vector<ValueType> const& exitRateVector,  storm::storage::BitVector const& markovianStates,
                                             storm::storage::BitVector const& psiStates,  std::vector<std::vector<ValueType>> relReachability,
                                             storm::storage::BitVector const& cycleStates , storm::storage::BitVector const& cycleGoalStates ,std::vector<std::vector<std::vector<ValueType>>>& unifVectors, std::ofstream& logfile);

                template <typename ValueType, typename std::enable_if<storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                static void computeBoundedReachabilityProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                 template <typename ValueType, typename std::enable_if<!storm::NumberTraits<ValueType>::SupportsExponential, int>::type = 0>
                 static void computeBoundedReachabilityProbabilities(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRates, storm::storage::BitVector const& goalStates, storm::storage::BitVector const& markovianNonGoalStates, storm::storage::BitVector const& probabilisticNonGoalStates, std::vector<ValueType>& markovianNonGoalValues, std::vector<ValueType>& probabilisticNonGoalValues, ValueType delta, uint64_t numberOfSteps, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
                /*!
                 * Computes the long-run average value for the given maximal end component of a Markov automaton.
                 *
                 * Implementations are based on Linear Programming (LP) and Value Iteration (VI).
                 *
                 * @param dir Sets whether the long-run average is to be minimized or maximized.
                 * @param transitionMatrix The transition matrix of the underlying Markov automaton.
                 * @param markovianStates A bit vector storing all markovian states.
                 * @param exitRateVector A vector with exit rates for all states. Exit rates of probabilistic states are
                 * assumed to be zero.
                 * @param rewardModel The considered reward model
                 * @param actionRewards The action rewards (earned instantaneously).
                 * @param mec The maximal end component to consider for computing the long-run average.
                 * @param minMaxLinearEquationSolverFactory The factory for creating MinMaxLinearEquationSolvers (if needed by the performed method
                 * @return The long-run average of being in a goal state for the given MEC.
                 */
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponent(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentLP(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec);
                template <typename ValueType, typename RewardModelType>
                static ValueType computeLraForMaximalEndComponentVI(Environment const& env, OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& exitRateVector, storm::storage::BitVector const& markovianStates, RewardModelType const& rewardModel, storm::storage::MaximalEndComponent const& mec, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& minMaxLinearEquationSolverFactory);
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_MARKOVAUTOMATON_CSL_MODELCHECKER_HELPER_H_ */
