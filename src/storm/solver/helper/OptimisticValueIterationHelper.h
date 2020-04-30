#pragma once

#include <vector>
#include <boost/optional.hpp>

#include "storm/solver/OptimizationDirection.h"
#include "storm/solver/SolverStatus.h"
#include "storm/utility/vector.h"
#include "storm/utility/ProgressMeasurement.h"
#include "storm/storage/BitVector.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/OviSolverEnvironment.h"

#include "storm/utility/macros.h"

namespace storm {
    
    namespace solver {
        namespace helper {
            namespace oviinternal {
    
                template<typename ValueType>
                ValueType computeMaxAbsDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues, storm::storage::BitVector const& relevantValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (auto value : relevantValues) {
                        result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[value] - allOldValues[value]));
                    }
                    return result;
                }
        
                template<typename ValueType>
                ValueType computeMaxAbsDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (uint64_t i = 0; i < allOldValues.size(); ++i) {
                        result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[i] - allOldValues[i]));
                    }
                    return result;
                }
            
                template<typename ValueType>
                ValueType computeMaxRelDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues, storm::storage::BitVector const& relevantValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (auto const& i : relevantValues) {
                        STORM_LOG_ASSERT(!storm::utility::isZero(allNewValues[i]) || storm::utility::isZero(allOldValues[i]), "Unexpected entry in iteration vector.");
                        if (!storm::utility::isZero(allNewValues[i])) {
                            result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[i] - allOldValues[i]) / allNewValues[i]);
                        }
                    }
                    return result;
                }
                
                template<typename ValueType>
                ValueType computeMaxRelDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues) {
                    ValueType result = storm::utility::zero<ValueType>();
                    for (uint64_t i = 0; i < allOldValues.size(); ++i) {
                        STORM_LOG_ASSERT(!storm::utility::isZero(allNewValues[i]) || storm::utility::isZero(allOldValues[i]), "Unexpected entry in iteration vector.");
                        if (!storm::utility::isZero(allNewValues[i])) {
                            result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[i] - allOldValues[i]) / allNewValues[i]);
                        }
                    }
                    return result;
                }
    
                template<typename ValueType>
                ValueType updateIterationPrecision(storm::Environment const& env, std::vector<ValueType> const& currentX, std::vector<ValueType> const& newX, bool const& relative, boost::optional<storm::storage::BitVector> const& relevantValues) {
                    auto factor = storm::utility::convertNumber<ValueType>(env.solver().ovi().getPrecisionUpdateFactor());
                    bool useRelevant = relevantValues.is_initialized() && env.solver().ovi().useRelevantValuesForPrecisionUpdate();
                    if (relative) {
                        return (useRelevant ? computeMaxRelDiff(newX, currentX, relevantValues.get()) : computeMaxRelDiff(newX, currentX)) * factor;
                    } else {
                        return (useRelevant ? computeMaxAbsDiff(newX, currentX, relevantValues.get()) : computeMaxAbsDiff(newX, currentX)) * factor;
                    }
                }
    
                template<typename ValueType>
                void guessUpperBoundRelative(std::vector<ValueType> const& x, std::vector<ValueType> &target, ValueType const& relativeBoundGuessingScaler) {
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(x, target, [&relativeBoundGuessingScaler] (ValueType const& argument) -> ValueType { return argument * relativeBoundGuessingScaler; });
                }
    
                template<typename ValueType>
                void guessUpperBoundAbsolute(std::vector<ValueType> const& x, std::vector<ValueType> &target, ValueType const& precision) {
                    storm::utility::vector::applyPointwise<ValueType, ValueType>(x, target, [&precision] (ValueType const& argument) -> ValueType { return argument + precision; });
                }

                namespace debug {
                    template<typename ValueType>
                    void applyHaddedMonmegeAlwaysEqualResultToLowerBound(std::vector<ValueType>* lowerX, const int n) {
                        switch(n) {
                            case 198:
                                (*lowerX)[0] = 3.5527502373072761e-15;
                                (*lowerX)[1] = 3.5527502373072769e-15;
                                (*lowerX)[2] = 3.5527502373072753e-15;
                                (*lowerX)[3] = 3.5527502373072785e-15;
                                (*lowerX)[4] = 3.5527502373072753e-15;
                                (*lowerX)[5] = 3.5527502373072816e-15;
                                (*lowerX)[6] = 3.5527502373072753e-15;
                                (*lowerX)[7] = 3.5527502373072879e-15;
                                (*lowerX)[8] = 3.5527502373072753e-15;
                                (*lowerX)[9] = 3.5527502373073006e-15;
                                (*lowerX)[10] = 3.5527502373072753e-15;
                                (*lowerX)[11] = 3.5527502373073258e-15;
                                (*lowerX)[12] = 3.5527502373072753e-15;
                                (*lowerX)[13] = 3.5527502373073763e-15;
                                (*lowerX)[14] = 3.5527502373072753e-15;
                                (*lowerX)[15] = 3.5527502373074773e-15;
                                (*lowerX)[16] = 3.5527502373072753e-15;
                                (*lowerX)[17] = 3.5527502373076792e-15;
                                (*lowerX)[18] = 3.552750237072753e-15;
                                (*lowerX)[19] = 3.5527502373080831e-15;
                                (*lowerX)[20] = 3.5527502373072753e-15;
                                (*lowerX)[21] = 3.5527502373088909e-15;
                                (*lowerX)[22] = 3.5527502373072753e-15;
                                (*lowerX)[23] = 3.5527502373105065e-15;
                                (*lowerX)[24] = 3.5527502373072753e-15;
                                (*lowerX)[25] = 3.5527502373137377e-15;
                                (*lowerX)[26] = 3.5527502373072753e-15;
                                (*lowerX)[27] = 3.5527502373202e-15;
                                (*lowerX)[28] = 3.5527502373072753e-15;
                                (*lowerX)[29] = 3.5527502373331247e-15;
                                (*lowerX)[30] = 3.5527502373072753e-15;
                                (*lowerX)[31] = 3.5527502373589741e-15;
                                (*lowerX)[32] = 3.5527502373072753e-15;
                                (*lowerX)[33] = 3.5527502374106729e-15;
                                (*lowerX)[34] = 3.5527502373072753e-15;
                                (*lowerX)[35] = 3.5527502375140705e-15;
                                (*lowerX)[36] = 3.5527502373072753e-15;
                                (*lowerX)[37] = 3.5527502377208656e-15;
                                (*lowerX)[38] = 3.5527502373072753e-15;
                                (*lowerX)[39] = 3.5527502381344559e-15;
                                (*lowerX)[40] = 3.5527502373072753e-15;
                                (*lowerX)[41] = 3.5527502389616373e-15;
                                (*lowerX)[42] = 3.5527502373072753e-15;
                                (*lowerX)[43] = 3.5527502406159986e-15;
                                (*lowerX)[44] = 3.5527502373072753e-15;
                                (*lowerX)[45] = 3.552750243924721e-15;
                                (*lowerX)[46] = 3.5527502373072753e-15;
                                (*lowerX)[47] = 3.5527502505421667e-15;
                                (*lowerX)[48] = 3.5527502373072753e-15;
                                (*lowerX)[49] = 3.5527502637770581e-15;
                                (*lowerX)[50] = 3.5527502373072753e-15;
                                (*lowerX)[51] = 3.55275029024684e-15;
                                (*lowerX)[52] = 3.5527502373072753e-15;
                                (*lowerX)[53] = 3.5527503431864048e-15;
                                (*lowerX)[54] = 3.5527502373072753e-15;
                                (*lowerX)[55] = 3.5527504490655334e-15;
                                (*lowerX)[56] = 3.5527502373072753e-15;
                                (*lowerX)[57] = 3.5527506608237915e-15;
                                (*lowerX)[58] = 3.5527502373072753e-15;
                                (*lowerX)[59] = 3.5527510843403078e-15;
                                (*lowerX)[60] = 3.5527502373072753e-15;
                                (*lowerX)[61] = 3.5527519313733394e-15;
                                (*lowerX)[62] = 3.5527502373072753e-15;
                                (*lowerX)[63] = 3.5527536254394035e-15;
                                (*lowerX)[64] = 3.5527502373072753e-15;
                                (*lowerX)[65] = 3.552757013571531e-15;
                                (*lowerX)[66] = 3.5527502373072753e-15;
                                (*lowerX)[67] = 3.5527637898357866e-15;
                                (*lowerX)[68] = 3.5527502373072753e-15;
                                (*lowerX)[69] = 3.552777342364298e-15;
                                (*lowerX)[70] = 3.5527502373072753e-15;
                                (*lowerX)[71] = 3.5528044474213206e-15;
                                (*lowerX)[72] = 3.5527502373072753e-15;
                                (*lowerX)[73] = 3.552858657535366e-15;
                                (*lowerX)[74] = 3.5527502373072753e-15;
                                (*lowerX)[75] = 3.5529670777634566e-15;
                                (*lowerX)[76] = 3.5527502373072753e-15;
                                (*lowerX)[77] = 3.5531839182196379e-15;
                                (*lowerX)[78] = 3.5527502373072753e-15;
                                (*lowerX)[79] = 3.5536175991319998e-15;
                                (*lowerX)[80] = 3.5527502373072753e-15;
                                (*lowerX)[81] = 3.554484960956725e-15;
                                (*lowerX)[82] = 3.5527502373072753e-15;
                                (*lowerX)[83] = 3.5562196846061739e-15;
                                (*lowerX)[84] = 3.5527502373072753e-15;
                                (*lowerX)[85] = 3.5596891319050725e-15;
                                (*lowerX)[86] = 3.5527502373072753e-15;
                                (*lowerX)[87] = 3.5666280265028689e-15;
                                (*lowerX)[88] = 3.5527502373072753e-15;
                                (*lowerX)[89] = 3.5805058156984624e-15;
                                (*lowerX)[90] = 3.5527502373072753e-15;
                                (*lowerX)[91] = 3.6082613940896487e-15;
                                (*lowerX)[92] = 3.5527502373072753e-15;
                                (*lowerX)[93] = 3.6637725508720213e-15;
                                (*lowerX)[94] = 3.5527502373072753e-15;
                                (*lowerX)[95] = 3.7747948644367674e-15;
                                (*lowerX)[96] = 3.5527502373072753e-15;
                                (*lowerX)[97] = 3.9968394915662579e-15;
                                (*lowerX)[98] = 3.5527502373072737e-15;
                                (*lowerX)[99] = 4.4409287458252396e-15;
                                (*lowerX)[100] = 3.5527502373072722e-15;
                                (*lowerX)[101] = 5.3291072543432039e-15;
                                (*lowerX)[102] = 3.552750237307269e-15;
                                (*lowerX)[103] = 7.1054642713791325e-15;
                                (*lowerX)[104] = 3.5527502373072627e-15;
                                (*lowerX)[105] = 1.0658178305450988e-14;
                                (*lowerX)[106] = 3.5527502373072501e-15;
                                (*lowerX)[107] = 1.7763606373594701e-14;
                                (*lowerX)[108] = 3.5527502373072248e-15;
                                (*lowerX)[109] = 3.1974462509882126e-14;
                                (*lowerX)[110] = 3.5527502373071743e-15;
                                (*lowerX)[111] = 6.0396174782456978e-14;
                                (*lowerX)[112] = 3.5527502373070734e-15;
                                (*lowerX)[113] = 1.1723959932760665e-13;
                                (*lowerX)[114] = 3.5527502373068714e-15;
                                (*lowerX)[115] = 2.3092644841790603e-13;
                                (*lowerX)[116] = 3.5527502373064675e-15;
                                (*lowerX)[117] = 4.5830014659850484e-13;
                                (*lowerX)[118] = 3.5527502373056597e-15;
                                (*lowerX)[119] = 9.1304754295970246e-13;
                                (*lowerX)[120] = 3.5527502373040441e-15;
                                (*lowerX)[121] = 1.8225423356820975e-12;
                                (*lowerX)[122] = 3.552750237300813e-15;
                                (*lowerX)[123] = 3.641531921126888e-12;
                                (*lowerX)[124] = 3.5527502372943506e-15;
                                (*lowerX)[125] = 7.2795110920164697e-12;
                                (*lowerX)[126] = 3.5527502372814259e-15;
                                (*lowerX)[127] = 1.4555469433795628e-11;
                                (*lowerX)[128] = 3.5527502372555765e-15;
                                (*lowerX)[129] = 2.9107386117353949e-11;
                                (*lowerX)[130] = 3.5527502372038777e-15;
                                (*lowerX)[131] = 5.8211219484470597e-11;
                                (*lowerX)[132] = 3.5527502371004786e-15;
                                (*lowerX)[133] = 1.1641888621870388e-10;
                                (*lowerX)[134] = 3.5527502368936819e-15;
                                (*lowerX)[135] = 2.3283421968717047e-10;
                                (*lowerX)[136] = 3.5527502364800868e-15;
                                (*lowerX)[137] = 4.6566488662410365e-10;
                                (*lowerX)[138] = 3.5527502356528983e-15;
                                (*lowerX)[139] = 9.3132622049797001e-10;
                                (*lowerX)[140] = 3.5527502339985197e-15;
                                (*lowerX)[141] = 1.8626488882457027e-09;
                                (*lowerX)[142] = 3.5527502306897634e-15;
                                (*lowerX)[143] = 3.7252942237411686e-09;
                                (*lowerX)[144] = 3.5527502240722506e-15;
                                (*lowerX)[145] = 7.4505848947320987e-09;
                                (*lowerX)[146] = 3.5527502108372251e-15;
                                (*lowerX)[147] = 1.4901166236713957e-08;
                                (*lowerX)[148] = 3.5527501843671734e-15;
                                (*lowerX)[149] = 2.9802328920677681e-08;
                                (*lowerX)[150] = 3.5527501314270691e-15;
                                (*lowerX)[151] = 5.9604654288605128e-08;
                                (*lowerX)[152] = 3.5527500255468605e-15;
                                (*lowerX)[153] = 1.1920930502446e-07;
                                (*lowerX)[154] = 3.5527498137864448e-15;
                                (*lowerX)[155] = 2.3841860649616978e-07;
                                (*lowerX)[156] = 3.5527493902656135e-15;
                                (*lowerX)[157] = 4.7683720943958926e-07;
                                (*lowerX)[158] = 3.5527485432239501e-15;
                                (*lowerX)[159] = 9.5367441532642841e-07;
                                (*lowerX)[160] = 3.5527468491406226e-15;
                                (*lowerX)[161] = 1.9073488271001067e-06;
                                (*lowerX)[162] = 3.5527434609739683e-15;
                                (*lowerX)[163] = 3.8146976506474625e-06;
                                (*lowerX)[164] = 3.5527366846406605e-15;
                                (*lowerX)[165] = 7.6293952977421757e-06;
                                (*lowerX)[166] = 3.5527231319740448e-15;
                                (*lowerX)[167] = 1.52587905919316e-05;
                                (*lowerX)[168] = 3.5526960266408136e-15;
                                (*lowerX)[169] = 3.0517581180310453e-05;
                                (*lowerX)[170] = 3.5526418159743506e-15;
                                (*lowerX)[171] = 6.1035162357068153e-05;
                                (*lowerX)[172] = 3.5525333946414248e-15;
                                (*lowerX)[173] = 0.00012207032471058356;
                                (*lowerX)[174] = 3.5523165519755735e-15;
                                (*lowerX)[175] = 0.00024414064941761436;
                                (*lowerX)[176] = 3.5518828666438713e-15;
                                (*lowerX)[177] = 0.00048828129883167601;
                                (*lowerX)[178] = 3.551015495980466e-15;
                                (*lowerX)[179] = 0.0009765625976597993;
                                (*lowerX)[180] = 3.549280754653656e-15;
                                (*lowerX)[181] = 0.0019531251953160459;
                                (*lowerX)[182] = 3.5458112720000354e-15;
                                (*lowerX)[183] = 0.0039062503906285391;
                                (*lowerX)[184] = 3.5388723066927948e-15;
                                (*lowerX)[185] = 0.0078125007812535254;
                                (*lowerX)[186] = 3.524994376078313e-15;
                                (*lowerX)[187] = 0.015625001562503498;
                                (*lowerX)[188] = 3.4972385148493499e-15;
                                (*lowerX)[189] = 0.031250003125003444;
                                (*lowerX)[190] = 3.4417267923914238e-15;
                                (*lowerX)[191] = 0.062500006250003334;
                                (*lowerX)[192] = 3.3307033474755718e-15;
                                (*lowerX)[193] = 0.12500001250000312;
                                (*lowerX)[194] = 3.1086564576438671e-15;
                                (*lowerX)[195] = 0.25000002500000268;
                                (*lowerX)[196] = 2.6645626779804573e-15;
                                (*lowerX)[197] = 0.50000005000000181;
                                (*lowerX)[198] = 1.7763751186536381e-15;
                                break;
                         }
                     }

                }
            }
            
            
            /*!
             * Performs Optimistic value iteration.
             * See https://arxiv.org/abs/1910.01100 for more information on this algorithm
             *
             * @tparam ValueType
             * @tparam ValueType
             * @param env
             * @param lowerX Needs to be some arbitrary lower bound on the actual values initially
             * @param upperX Does not need to be an upper bound initially
             * @param auxVector auxiliary storage
             * @param valueIterationCallback  Function that should perform standard value iteration on the input vector
             * @param singleIterationCallback Function that should perform a single value iteration step on the input vector e.g. ( x' = min/max(A*x + b))
             * @param relevantValues If given, we only check the precision at the states with the given indices.
             * @return The status upon termination as well as the number of iterations Also, the maximum (relative/absolute) difference between lowerX and upperX will be 2*epsilon
             * with precision parameters as given by the environment env.
             */
            template<typename ValueType, typename ValueIterationCallback, typename SingleIterationCallback>
            std::pair<SolverStatus, uint64_t> solveEquationsOptimisticValueIteration(Environment const& env, std::vector<ValueType>* lowerX, std::vector<ValueType>* upperX, std::vector<ValueType>* auxVector, ValueIterationCallback const& valueIterationCallback, SingleIterationCallback const& singleIterationCallback, boost::optional<storm::storage::BitVector> relevantValues = boost::none) {
                STORM_LOG_ASSERT(lowerX->size() == upperX->size(), "Dimension missmatch.");
                STORM_LOG_ASSERT(lowerX->size() == auxVector->size(), "Dimension missmatch.");
                
                // As we will shuffle pointers around, let's store the original positions here.
                std::vector<ValueType>* initLowerX = lowerX;
                std::vector<ValueType>* initUpperX = upperX;
                std::vector<ValueType>* initAux = auxVector;
                
                uint64_t overallIterations = 0;
                uint64_t lastValueIterationIterations = 0;
                uint64_t currentVerificationIterations = 0;
                uint64_t valueIterationInvocations = 0;
                
                // Get some parameters for the algorithm
                // 2
                ValueType two = storm::utility::convertNumber<ValueType>(2.0);
                // Relative errors
                bool relative = env.solver().minMax().getRelativeTerminationCriterion();
                // Goal precision
                ValueType precision = storm::utility::convertNumber<ValueType>(env.solver().minMax().getPrecision());
                // Desired max difference between upperX and lowerX
                ValueType doublePrecision = precision * two;
                // Upper bound only iterations
                uint64_t upperBoundOnlyIterations = env.solver().ovi().getUpperBoundOnlyIterations();
                // Maximum number of iterations done overall
                uint64_t maxOverallIterations = env.solver().minMax().getMaximalNumberOfIterations();
                ValueType relativeBoundGuessingScaler = (storm::utility::one<ValueType>() + storm::utility::convertNumber<ValueType>(env.solver().ovi().getUpperBoundGuessingFactor()) * precision);
                // Initial precision for the value iteration calls
                ValueType iterationPrecision = precision;
    
                SolverStatus status = SolverStatus::InProgress;

                // DEBUG initialize lower bound to 20 minute result
                if(lowerX->size() == 199) {
                    helper::oviinternal::debug::applyHaddedMonmegeAlwaysEqualResultToLowerBound(lowerX, lowerX->size());
                }
    
                while (status == SolverStatus::InProgress && overallIterations < maxOverallIterations) {
    
                    // Perform value iteration until convergence
                    ++valueIterationInvocations;
                    auto result = valueIterationCallback(lowerX, auxVector, iterationPrecision, relative, overallIterations, maxOverallIterations);
                    lastValueIterationIterations = result.iterations;
                    overallIterations += result.iterations;
    
                    if (result.status != SolverStatus::Converged) {
                        status = result.status;
                    } else {
                        bool intervalIterationNeeded = false;
                        currentVerificationIterations = 0;
    
                        if (relative) {
                            oviinternal::guessUpperBoundRelative(*lowerX, *upperX, relativeBoundGuessingScaler);
                        } else {
                            oviinternal::guessUpperBoundAbsolute(*lowerX, *upperX, precision);
                        }
    
                        bool cancelGuess = false;
                        while (status == SolverStatus::InProgress && overallIterations < maxOverallIterations) {
                            ++overallIterations;
                            ++currentVerificationIterations;
                            // Perform value iteration stepwise for lower bound and guessed upper bound
    
                            // Upper bound iteration
                            singleIterationCallback(upperX, auxVector, overallIterations);
                            // At this point, auxVector contains the old values for the upper bound whereas upperX contains the new ones.
                            
                            // Compare the new upper bound candidate with the old one
                            bool newUpperBoundAlwaysHigherEqual = true;
                            bool newUpperBoundAlwaysLowerEqual = true;
                            for (uint64_t i = 0; i < upperX->size(); ++i) {
                                if ((*auxVector)[i] > (*upperX)[i]) {
                                    newUpperBoundAlwaysHigherEqual = false;
                                } else if ((*auxVector)[i] != (*upperX)[i]) {
                                    newUpperBoundAlwaysLowerEqual = false;
                                }
                            }
                            if (overallIterations % 10000 == 0 && newUpperBoundAlwaysHigherEqual && newUpperBoundAlwaysLowerEqual) {
                                std::cout << "[" << overallIterations << "]: All values stayed the same on upper-only / interval verification in OVI.\n";
                            }
                            if (newUpperBoundAlwaysHigherEqual &! newUpperBoundAlwaysLowerEqual) {
                                // All values moved up or stayed the same
                                // That means the guess for an upper bound is actually a lower bound
                                iterationPrecision = oviinternal::updateIterationPrecision(env, *auxVector, *upperX, relative, relevantValues);
                                // We assume to have a single fixed point. We can thus safely set the new lower bound, to the wrongly guessed upper bound
                                // Set lowerX to the upper bound candidate
                                std::swap(lowerX, upperX);
                                break;
                            } else if (newUpperBoundAlwaysLowerEqual &! newUpperBoundAlwaysHigherEqual) {
                                // All values moved down or stayed the same and we have a maximum difference of twice the requested precision
                                // We can safely use twice the requested precision, as we calculate the center of both vectors
                                bool reachedPrecision;
                                std::cout << "\n\nWrongly return in OVI!\n\n";
                                if (relevantValues) {
                                    reachedPrecision = storm::utility::vector::equalModuloPrecision(*lowerX, *upperX, relevantValues.get(), doublePrecision, relative);
                                } else {
                                    reachedPrecision = storm::utility::vector::equalModuloPrecision(*lowerX, *upperX, doublePrecision, relative);
                                }
                                if (reachedPrecision) {
                                    status = SolverStatus::Converged;
                                    break;
                                } else {
                                    // From now on, we keep updating both bounds
                                    intervalIterationNeeded = true;
                                }
                            }
                            // At this point, the old upper bounds (auxVector) are not needed anymore.
                            
                            // Check whether we tried this guess for too long
                            ValueType scaledIterationCount = storm::utility::convertNumber<ValueType>(currentVerificationIterations) * storm::utility::convertNumber<ValueType>(env.solver().ovi().getMaxVerificationIterationFactor());
                            if (!intervalIterationNeeded && scaledIterationCount >= storm::utility::convertNumber<ValueType>(lastValueIterationIterations)) {
                                cancelGuess = true;
                                // In this case we will make one more iteration on the lower bound (mainly to obtain a new iterationPrecision)
                            }
                            
                            // Lower bound iteration (only if needed)
                            if (cancelGuess || intervalIterationNeeded || currentVerificationIterations > upperBoundOnlyIterations) {
                                singleIterationCallback(lowerX, auxVector, overallIterations);
                                // At this point, auxVector contains the old values for the lower bound whereas lowerX contains the new ones.
    
                                // Check whether the upper and lower bounds have crossed, i.e., the upper bound is smaller than the lower bound.
                                bool valuesCrossed = false;
                                for (uint64_t i = 0; i < lowerX->size(); ++i) {
                                    if ((*upperX)[i] < (*lowerX)[i]) {
                                        valuesCrossed = true;
                                        break;
                                    }
                                }
                                
                                if (cancelGuess || valuesCrossed) {
                                    // A new guess is needed.
                                    iterationPrecision = oviinternal::updateIterationPrecision(env, *auxVector, *lowerX, relative, relevantValues);
                                    break;
                                }
                            }
                        }
                    }
                }
                
                // Swap the results into the output vectors.
                if (initLowerX == lowerX) {
                    // lowerX is already at the correct position. We still have to care for upperX
                    if (initUpperX != upperX) {
                        // UpperX is not at the correct position. It has to be at the auxVector
                        assert(initAux == upperX);
                        std::swap(*initUpperX, *initAux);
                    }
                } else if (initUpperX == upperX) {
                    // UpperX is already at the correct position.
                    // We already know that lowerX is at the wrong position. It has to be at the auxVector
                    assert(initAux == lowerX);
                    std::swap(*initLowerX, *initAux);
                } else if (initAux == auxVector) {
                    // We know that upperX and lowerX are swapped.
                    assert(initLowerX == upperX);
                    assert(initUpperX == lowerX);
                    std::swap(*initUpperX, *initLowerX);
                } else {
                    // Now we know that all vectors are at the wrong position. There are only two possibilities left
                    if (initLowerX == upperX) {
                        assert(initUpperX == auxVector);
                        assert(initAux == lowerX);
                        std::swap(*initLowerX, *initAux);
                        std::swap(*initUpperX, *initAux);
                    } else {
                        assert(initLowerX == auxVector);
                        assert(initUpperX == lowerX);
                        assert (initAux == upperX);
                        std::swap(*initUpperX, *initAux);
                        std::swap(*initLowerX, *initAux);
                    }
                }
                
                if (overallIterations > maxOverallIterations) {
                    status = SolverStatus::MaximalIterationsExceeded;
                }
                
                return {status, overallIterations};
            }
        }
    }
}

