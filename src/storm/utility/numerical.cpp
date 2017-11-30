#include <vector>
#include <tuple>
#include <cmath>

#include <boost/math/constants/constants.hpp>

#include "storm/utility/macros.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/OutOfRangeException.h"
#include "storm/exceptions/PrecisionExceededException.h"

namespace storm {
    namespace utility {
        namespace numerical {

            template<typename ValueType>
            FoxGlynnResult<ValueType>::FoxGlynnResult() : left(0), right(0), totalWeight(0) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            FoxGlynnResult<ValueType> foxGlynnFinder(ValueType lambda, ValueType epsilon) {
                int left, right;
                FoxGlynn * pFG;
                
                /* tau is only used in underflow checks, which we are going to do in the
                 logarithm domain. */
                tau = log(tau);
                /* In error bound comparisons, we always compare with epsilon*sqrt_2_pi.*/
                epsilon *= sqrt_2_pi;
                /****Compute pFG->left truncation point****/
                if ( m < 25 )
                {
                    /* for lambda below 25 the exponential can be smaller than tau */
                    /* if that is the case we expect underflows and warn the user */
                    if ( -lambda <= tau )
                    {
                        printf("ERROR: Fox-Glynn: 0 < lambda < 25, underflow near Poi(%g,"
                               "0) = %.2e. The results are UNRELIABLE.\n",
                               lambda, exp(-lambda));
                    }
                    /* zero is used as left truncation point for lambda <= 25 */
                    left = 0;
                }
                /* compute the left truncation point for lambda >= 25 */
                /* for lambda < 25 we use zero as left truncation point */
                else
                {
                    const double bl = (1 + 1 / lambda) * exp((1/lambda) * 0.125);
                    const double sqrt_lambda = sqrt(lambda);
                    int k;
                    /*Start looking for the left truncation point*/
                    /* start search at k=4 (taken from original Fox-Glynn paper) */
                    /* increase the left truncation point until we fulfil the error
                     condition */
                    
                    for ( k = 4 ; TRUE ; k++ ) {
                        double max_err;
                        
                        left = m - (int) ceil(k*sqrt_lambda + 0.5);
                        /* for small lambda the above calculation can yield negative truncation points, crop them here */
                        if ( left <= 0 ) {
                            left = 0;
                            break;
                        }
                        /* Note that Propositions 2-4 in Fox--Glynn mix up notation: they
                         write Phi where they mean 1 - Phi. (In Corollaries 1 and 2, phi is
                         used correctly again.) */
                        max_err = bl * exp(-0.5 * (k*k)) / k;
                        if ( max_err * 2 <= epsilon ) {
                            /* If the error on the left hand side is smaller, we can be
                             more lenient on the right hand side. To this end, we now set
                             epsilon to the part of the error that has not yet been eaten
                             up by the left-hand truncation. */
                            epsilon -= max_err;
                            break;
                        }
                    }
                    /*Finally the left truncation point is found*/
                }
                
                /****Compute pFG->right truncation point****/
                {
                    double lambda_max;
                    int m_max, k;
                    /*According to Fox-Glynn, if lambda < 400 we should take lambda = 400,
                     otherwise use the original value. This is for computing the right truncation point*/
                    if ( m < 400 ) {
                        lambda_max = lambda_400;
                        m_max = 400;
                        epsilon *= 0.662608824988162441697980;
                        /* i.e. al = (1+1/400) * exp(1/16) * sqrt_2; epsilon /= al; */
                    } else {
                        lambda_max = lambda;
                        m_max = m;
                        epsilon *= (1 - 1 / (lambda + 1)) * 0.664265347050632847802225;
                        /* i.e. al = (1+1/lambda) * exp(1/16) * sqrt_2; epsilon /= al; */
                    }
                    /* find right truncation point */
                    
                    /* This loop is a modification to the original Fox-Glynn paper.
                     The search for the right truncation point is only terminated by
                     the error condition and not by the stop index from the FG paper.
                     This can yield more accurate results if necessary.*/
                    for ( k = 4 ; TRUE ; k++ )
                    {
                        /* dkl_inv is between 1 - 1e-33 and 1 if lambda_max >= 400 and
                         k >= 4; this will always be rounded to 1.0. We therefore leave the
                         factor out.
                         double dkl_inv=1 - exp(-266/401.0 * (k*sqrt(2*lambda_max) + 1.5));
                         */
                        if ( k * epsilon /* actually: "k * (dkl_inv*epsilon/al)", which
                                          has been calculated above */  >= exp(-0.5*(k*k)) )
                            break;
                    }
                    right = m_max + (int) ceil(k * sqrt(2 * lambda_max) + 0.5);
                    if ( right > m_max + (int) ceil((lambda_max + 1) * 0.5) ) {
                        printf("ERROR: Fox-Glynn: right = %d >> lambda = %g, cannot "
                               "bound the right tail. The results are "
                               "UNRELIABLE.\n", right, lambda_max);
                    }
                }
                
                /*Time to set the initial value for weights*/
                pFG = calloc((size_t) 1, sizeof(FoxGlynn) +
                             (right - left) * sizeof(pFG->weights[0]));
                if ( NULL == pFG ) {
                    err_msg_3(err_MEMORY, "finder(%d,%g,_,%g,_)", m, lambda, omega, NULL);
                }
                pFG->right = right;
                pFG->left = left;
                pFG->weights[m - left] = omega / ( 1.0e+10 * (right - left) );
                
                if ( m >= 25 )
                {
                    /* perform underflow check */
                    double result, log_c_m_inf;
                    int i;
                    
                    /* we are going to compare with tau - log(w[m]) */
                    tau -= log(pFG->weights[m - left]);
                    /*We take the c_m_inf = 0.14627 / sqrt( m ), as for lambda >= 25
                     c_m = 1 / ( sqrt( 2.0 * pi * m ) ) * exp( m - lambda - 1 / ( 12.0 * m ) ) => c_m_inf*/
                    /* Note that m-lambda is in the interval (-1,0],
                     and -1/(12*m) is in [-1/(12*25),0).
                     So, exp(m-lambda - 1/(12*m)) is in (exp(-1-1/(12*25)),exp(0)).
                     Therefore, we can improve the lower bound on c_m to
                     exp(-1-1/(12*25)) / sqrt(2*pi) = ~0.14627. Its logarithm is
                     -1 - 1/(12*25) - log(2*pi) * 0.5 = ~ -1.922272 (rounded towards
                     -infinity). */
                    log_c_m_inf = -1.922272 - log((double) m) * 0.5;
                    
                    /* We use FG's Proposition 6 directly (and not Corollary 4 i and ii),
                     as k_prime may be too large if pFG->left == 0. */
                    i = m - left;
                    if ( i <= left /* equivalent to 2*i <= m,
                                    equivalent to i <= lambda/2 */ )
                    {
                        /* Use Proposition 6 (i). Note that Fox--Glynn are off by one in
                         the proof of this proposition; they sum up to i-1, but should have
                         summed up to i. */
                        result = log_c_m_inf
                        - i * (i+1) * (0.5 + (2*i+1)/(6*lambda)) / lambda;
                    }
                    else
                    {
                        /* Use Corollary 4 (iii). Note that k_prime <= sqrt(m+1)/m is a
                         misprint for k_prime <= m/sqrt(m+1), which is equivalent to
                         left >= 0, which holds trivially. */
                        result = -lambda;
                        if ( 0 != left ) {
                            /* also use Proposition 6 (ii) */
                            double result_1 = log_c_m_inf + i * log(1 - i/(double) (m+1));
                            /*Take the maximum*/
                            if ( result_1 > result )
                                result = result_1;
                        }
                    }
                    if ( result <= tau )
                    {
                        const int log10_result = (int) floor(result * log10_e);
                        printf("ERROR: Fox-Glynn: lambda >= 25, underflow near Poi(%g,%d)"
                               " <= %.2fe%+d. The results are UNRELIABLE.\n",
                               lambda, left, exp(result - log10_result/log10_e),
                               log10_result);
                    }
                    
                    /*We still have to perform an underflow check for the right truncation point when lambda >= 400*/
                    if ( m >= 400 )
                    {
                        /* Use Proposition 5 of Fox--Glynn */
                        i = right - m;
                        result = log_c_m_inf - i * (i + 1) / (2 * lambda);
                        if ( result <= tau )
                        {
                            const int log10_result = (int) floor(result * log10_e);
                            printf("ERROR: Fox-Glynn: lambda >= 400, underflow near "
                                   "Poi(%g,%d) <= %.2fe%+d. The results are "
                                   "UNRELIABLE.\n", lambda, right,
                                   exp(result - log10_result / log10_e),
                                   log10_result);
                        }
                    }
                }
                
                return pFG;
            }
            
            template<typename ValueType>
            FoxGlynnResult<ValueType> foxGlynnWeighter(ValueType lambda, ValueType epsilon) {
                /*The magic m point*/
                const uint64_t m = (int) floor(lambda);
                int j, t;
                FoxGlynn * pFG;
                
                pFG = finder(m, lambda, tau, omega, epsilon);
                if ( NULL == pFG ) {
                    err_msg_4(err_CALLBY, "weighter(%g,%g,%g,%g)", lambda,
                              tau, omega, epsilon, NULL);
                }
                
                /*Fill the left side of the array*/
                for ( j = m - pFG->left ; j > 0 ; j-- )
                    pFG->weights[j-1] = (j+pFG->left) / lambda * pFG->weights[j];
                
                t = pFG->right - pFG->left;
                /*Fill the right side of the array, have two cases lambda < 400 & lambda >= 400*/
                if ( m < 400 )
                {
                    /*Perform the underflow check, according to Fox-Glynn*/
                    if( pFG->right > 600 )
                    {
                        printf("ERROR: Fox-Glynn: pFG->right > 600, underflow is possible\n");
                        freeFG(pFG);
                        return NULL;
                    }
                    /*Compute weights*/
                    for ( j = m - pFG->left ; j < t ; j++ )
                    {
                        double q = lambda / (j + 1 + pFG->left);
                        if ( pFG->weights[j] > tau / q )
                        {
                            pFG->weights[j + 1] = q * pFG->weights[j];
                        }else{
                            t = j;
                            pFG->right = j + pFG->left;
                            break; /*It's time to compute W*/
                        }
                    }
                }else{
                    /*Compute weights*/
                    for ( j = m - pFG->left ; j < t ; j++ )
                        pFG->weights[j+1] = lambda / (j+1 + pFG->left) * pFG->weights[j];
                }
                
                /*It is time to compute the normalization weight W*/
                pFG->total_weight = 0.0;
                j = 0;
                /* t was set above */
                
                while( j < t )
                {
                    if ( pFG->weights[j] <= pFG->weights[t] )
                    {
                        pFG->total_weight += pFG->weights[j];
                        j++;
                    }else{
                        pFG->total_weight += pFG->weights[t];
                        t--;
                    }
                }
                pFG->total_weight += pFG->weights[j];
                
                /* printf("Fox-Glynn: ltp = %d, rtp = %d, w = %10.15le \n", pFG->left, pFG->right, pFG->total_weight); */
                
                return pFG;
            }
            
            template<typename ValueType>
            FoxGlynnResult<ValueType> foxGlynn(ValueType lambda, ValueType epsilon) {
                STORM_LOG_THROW(lambda > 0, storm::exceptions::InvalidArgumentException, "Fox-Glynn requires positive lambda.");
                return foxGlynnWeighter(lambda, epsilon);
            }

            template FoxGlynnResult<double> foxGlynn(double lambda, double epsilon);
            
        }
    }
}
