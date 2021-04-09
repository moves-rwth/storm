#include "GradientDescentInstantiationSearcher.h"
#include "analysis/GraphConditions.h"
#include "environment/Environment.h"
#include "environment/solver/SolverEnvironment.h"
#include "environment/solver/GmmxxSolverEnvironment.h"
#include "environment/solver/MinMaxSolverEnvironment.h"
#include "environment/solver/NativeSolverEnvironment.h"
#include "environment/solver/SolverEnvironment.h"
#include "environment/solver/SolverEnvironment.h"
#include "solver/helper/SoundValueIterationHelper.h"
#include "modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "settings/SettingsManager.h"
#include "settings/modules/GeneralSettings.h"
#include "storm-pars/modelchecker/instantiation/SparseDtmcInstantiationModelChecker.h"
#include "storm/solver/EliminationLinearEquationSolver.h"
#include "utility/SignalHandler.h"
#include "utility/graph.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/constants.h"
#include <cln/random.h>
#include <cln/real.h>
#include <random>

namespace storm {
    namespace derivative {

        template<typename ValueType>
        using VariableType = typename utility::parametric::VariableType<ValueType>::type;
        template<typename ValueType>
        using CoefficientType = typename utility::parametric::CoefficientType<ValueType>::type;

        template<typename ValueType, typename ConstantType>
        ConstantType GradientDescentInstantiationSearcher<ValueType, ConstantType>::doStep(
                VariableType<ValueType> steppingParameter,
                std::map<VariableType<ValueType>, CoefficientType<ValueType>> &position,
                const std::map<VariableType<ValueType>, ConstantType> &gradient,
                uint64_t stepNum
            ) {
            const ConstantType precisionAsConstant = utility::convertNumber<ConstantType>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
            auto const oldPos = position[steppingParameter];

            // Compute step based on used gradient descent method
            ConstantType step; 
            if (Adam* adam = boost::get<Adam>(&gradientDescentType)) {
                // For this algorihm, see the various sources available on the ADAM algorithm. This implementation should
                // be correct, as it is compared with a run of keras's ADAM optimizer in the test.
                adam->decayingStepAverage[steppingParameter] = adam->averageDecay * adam->decayingStepAverage[steppingParameter] +
                    (1 - adam->averageDecay) * gradient.at(steppingParameter);
                adam->decayingStepAverageSquared[steppingParameter] = adam->squaredAverageDecay * adam->decayingStepAverageSquared[steppingParameter] +
                    (1 - adam->squaredAverageDecay) * utility::pow(gradient.at(steppingParameter), 2);

                const ConstantType correctedGradient = adam->decayingStepAverage[steppingParameter] / (1 - utility::pow(adam->averageDecay, stepNum + 1));
                const ConstantType correctedSquaredGradient = adam->decayingStepAverageSquared[steppingParameter] / (1 - utility::pow(adam->squaredAverageDecay, stepNum + 1));

                const ConstantType toSqrt = correctedSquaredGradient;
                ConstantType sqrtResult = constantTypeSqrt(toSqrt);
                /* if (std::is_same<ConstantType, double>::value) { */
                /*     sqrtResult = constantTypeSqrt(toSqrt); */
                /* } else { */
                /*     sqrtResult = carl::sqrt(toSqrt); */
                /* } */
                
                step = (adam->learningRate / (sqrtResult + precisionAsConstant)) * correctedGradient;
            } else if (RAdam* radam = boost::get<RAdam>(&gradientDescentType)) {
                // You can compare this with the RAdam paper's "Algorithm 2: Rectified Adam".
                // The line numbers and comments are matched.
                // Initializing / Compute Gradient: Already happened.
                // 2: Compute maximum length of approximated simple moving average
                const ConstantType maxLengthApproxSMA = 2.0 / (1.0 - radam->squaredAverageDecay) - 1.0;

                // 5: Update exponential moving 2nd moment
                radam->decayingStepAverageSquared[steppingParameter] = radam->squaredAverageDecay * radam->decayingStepAverageSquared[steppingParameter] +
                    (1.0 - radam->squaredAverageDecay) * utility::pow(gradient.at(steppingParameter), 2);
                // 6: Update exponential moving 1st moment
                radam->decayingStepAverage[steppingParameter] = radam->averageDecay * radam->decayingStepAverage[steppingParameter] +
                    (1 - radam->averageDecay) * gradient.at(steppingParameter);
                // 7: Compute bias corrected moving average
                const ConstantType biasCorrectedMovingAverage = radam->decayingStepAverage[steppingParameter] / (1 - utility::pow(radam->averageDecay, stepNum + 1));
                const ConstantType squaredAverageDecayPow = utility::pow(radam->squaredAverageDecay, stepNum + 1);
                // 8: Compute the length of the approximated single moving average
                const ConstantType lengthApproxSMA = maxLengthApproxSMA - ((2 * (stepNum + 1) * squaredAverageDecayPow) / (1 - squaredAverageDecayPow));
                // 9: If the variance is tractable, i.e. lengthApproxSMA > 4, then
                if (lengthApproxSMA > 5) {
                    // 10: Compute adaptive learning rate
                    const ConstantType adaptiveLearningRate = constantTypeSqrt((1 - squaredAverageDecayPow) / radam->decayingStepAverageSquared[steppingParameter]);
                    // 11: Compute the variance rectification term
                    const ConstantType varianceRectification = constantTypeSqrt(
                            ((lengthApproxSMA - 4.0) / (maxLengthApproxSMA - 4.0)) *
                            ((lengthApproxSMA - 2.0) / (maxLengthApproxSMA - 2.0)) *
                            ((maxLengthApproxSMA) / (lengthApproxSMA))
                    );
                    // 12: Update parameters with adaptive momentum
                    step = radam->learningRate * varianceRectification * biasCorrectedMovingAverage * adaptiveLearningRate;
                    /* std::cout << "Adaptive step" << std::endl; */
                    /* std::cout << step << std::endl; */
                    /* std::cout << radam->learningRate << ", " << varianceRectification << ", " << biasCorrectedMovingAverage << ", " << adaptiveLearningRate << std::endl; */
                    /* std::cout << lengthApproxSMA << "," << maxLengthApproxSMA << std::endl; */
                } else {
                    // 14: Update parameters with un-adapted momentum
                    step = radam->learningRate * biasCorrectedMovingAverage;
                    /* std::cout << "Non-adaptive step" << std::endl; */
                }
            } else if (RmsProp* rmsProp = boost::get<RmsProp>(&gradientDescentType)) {
                rmsProp->rootMeanSquare[steppingParameter] = rmsProp->averageDecay * rmsProp->rootMeanSquare[steppingParameter] +
                    (1 - rmsProp->averageDecay) * gradient.at(steppingParameter) * gradient.at(steppingParameter);

                const ConstantType toSqrt = rmsProp->rootMeanSquare[steppingParameter] + precisionAsConstant;
                ConstantType sqrtResult = constantTypeSqrt(toSqrt);

                step = (rmsProp->learningRate / sqrtResult) * gradient.at(steppingParameter);
            } else if (Plain* plain = boost::get<Plain>(&gradientDescentType)) {
                if (useSignsOnly) {
                    if (gradient.at(steppingParameter) < 0) {
                        step = -plain->learningRate;
                    } else if (gradient.at(steppingParameter) > 0) {
                        step = plain->learningRate;
                    } else {
                        step = 0;
                    }
                } else {
                    step = plain->learningRate * gradient.at(steppingParameter);
                }
            } else if (Momentum* momentum = boost::get<Momentum>(&gradientDescentType)) {
                if (useSignsOnly) {
                    if (gradient.at(steppingParameter) < 0) {
                        step = -momentum->learningRate;
                    } else if (gradient.at(steppingParameter) > 0) {
                        step = momentum->learningRate;
                    } else {
                        step = 0;
                    }
                } else {
                    step = momentum->learningRate * gradient.at(steppingParameter);
                }
                step += momentum->momentumTerm * momentum->pastStep.at(steppingParameter);
                momentum->pastStep[steppingParameter] = step;
            } else if (Nesterov* nesterov = boost::get<Nesterov>(&gradientDescentType)) {
                if (useSignsOnly) {
                    if (gradient.at(steppingParameter) < 0) {
                        step = -nesterov->learningRate;
                    } else if (gradient.at(steppingParameter) > 0) {
                        step = nesterov->learningRate;
                    } else {
                        step = 0;
                    }
                } else {
                    step = nesterov->learningRate * gradient.at(steppingParameter);
                }
                step -= nesterov->momentumTerm * nesterov->pastStep[steppingParameter];

                step += nesterov->momentumTerm * nesterov->pastStep.at(steppingParameter);
                nesterov->pastStep[steppingParameter] = step;

                step += nesterov->momentumTerm * step;
            } else {
                STORM_LOG_ERROR("GradientDescentType was not a known one");
            }

            const CoefficientType<ValueType> convertedStep = utility::convertNumber<CoefficientType<ValueType>>(step);
            const CoefficientType<ValueType> newPos = position[steppingParameter] + convertedStep;
            position[steppingParameter] = newPos;
            const auto precision = storm::utility::convertNumber<CoefficientType<ValueType>>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
            // Map parameter back to (0, 1).
            position[steppingParameter] = utility::max(utility::zero<CoefficientType<ValueType>>() + precision, position[steppingParameter]);
            position[steppingParameter] = utility::min(utility::one<CoefficientType<ValueType>>() - precision, position[steppingParameter]);
            return utility::abs(utility::convertNumber<ConstantType>(oldPos - position[steppingParameter]));
        }

        template<typename ValueType, typename ConstantType>
        ConstantType GradientDescentInstantiationSearcher<ValueType, ConstantType>::stochasticGradientDescent(
            Environment const& env,
            std::map<VariableType<ValueType>, CoefficientType<ValueType>> &position
        ) {

            uint_fast64_t initialState;           
            const storm::storage::BitVector initialVector = model->getStates("init");
            for (uint_fast64_t x : initialVector) {
                initialState = x;
                break;
            }

            ConstantType currentValue;
            switch (optimalityType) {
                case storm::OptimizationDirection::Maximize:
                    currentValue = -utility::infinity<ConstantType>();
                    break;
                case storm::OptimizationDirection::Minimize:
                    currentValue = utility::infinity<ConstantType>();
                    break;
            }

            // We count the number of iterations where the value changes less than the threshold, and terminate if it is large enough.
            uint64_t tinyChangeIterations = 0;

            std::map<VariableType<ValueType>, ConstantType> deltaVector;

            std::vector<VariableType<ValueType>> parameterEnumeration;
            for (auto parameter : parameters) {
                parameterEnumeration.push_back(parameter);
            }

            utility::Stopwatch printUpdateStopwatch;
            printUpdateStopwatch.start();
            
            // The index to keep track of what parameter(s) to consider next.
            // The "mini-batch", so the parameters to consider, are parameterNum..parameterNum+miniBatchSize-1
            uint_fast64_t parameterNum = 0;
            for (uint_fast64_t stepNum = 0; true; ++stepNum) {
                if (printUpdateStopwatch.getTimeInSeconds() >= 15) {
                    printUpdateStopwatch.restart();
                    std::cout << "Currently at ";
                    std::cout << currentValue << std::endl;
                }

                std::vector<VariableType<ValueType>> miniBatch;
                for (uint_fast64_t i = parameterNum; i < std::min(parameterEnumeration.size(), parameterNum + miniBatchSize); i++) {
                    miniBatch.push_back(parameterEnumeration[i]);
                }
                
                ConstantType oldValue = currentValue;

                // Compute the value of our position and terminate if it satisfies the bound or is
                // zero or one when computing probabilities. The "valueVector" (just the probability/expected
                // reward for eventually reaching the target from every state) is also used for computing
                // the gradient later. We only need one computation of the "valueVector" per mini-batch.
                std::unique_ptr<storm::modelchecker::CheckResult> result = instantiationModelChecker->check(env, position);
                std::vector<ConstantType> valueVector = result->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
                currentValue = valueVector[initialState];
                
                if (bound && bound->isSatisfied(currentValue)) {
                    std::cout << "Satisfied the bound! Done!" << std::endl;
                    break;
                }

                if (resultType == ResultType::PROBABILITY) {
                    auto precision = storm::utility::convertNumber<ConstantType>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
                    if (!bound && currentValue >= 1 - precision && optimalityType == storm::OptimizationDirection::Maximize) {
                        std::cout << "Probability is 1! Done!" << std::endl;
                        break;
                    }
                    if (!bound && currentValue <= precision && optimalityType == storm::OptimizationDirection::Minimize) {
                        std::cout << "Probability is 0! Done!" << std::endl;
                        break;
                    }
                }

                for (auto const& parameter : miniBatch) {
                    ConstantType delta = derivativeEvaluationHelper->calculateDerivative(env, parameter, position, valueVector); 
                    if (optimalityType == storm::OptimizationDirection::Minimize) {
                        delta *= -1;
                    }
                    deltaVector[parameter] = delta;
                }

                // Log position and probability information for later use in visualizing the descent, if wished.
                if (recordRun) {
                    VisualizationPoint point;
                    point.position = position;
                    point.value = currentValue;
                    walk.push_back(point);
                }

                // Perform the step. The actualChange is the change in position the step caused. This is different from the
                // delta in multiple ways: First, it's multiplied with the learning rate and stuff. Second, if the current value
                // is at epsilon, and the delta would step out of the constrained which is then corrected, the actualChange is the
                // change from the last to the current corrected position (so might be zero while the delta is not).
                for (auto const& parameter : miniBatch) {
                    doStep(parameter, position, deltaVector, stepNum);
                }

                if (storm::utility::abs<ConstantType>(oldValue - currentValue) < terminationEpsilon) {
                    tinyChangeIterations += miniBatch.size();
                    if (tinyChangeIterations > parameterEnumeration.size()) {
                        break;
                    }
                } else {
                    tinyChangeIterations = 0;
                }

                // Consider the next parameter
                parameterNum = parameterNum + miniBatchSize;
                if (parameterNum >= parameterEnumeration.size()) {
                    parameterNum = 0;
                }

                if (storm::utility::resources::isTerminate()) {
                    STORM_LOG_WARN("Aborting Gradient Descent, returning non-optimal value.");
                    break;
                }
            }
            return currentValue;
        }

        template<typename ValueType, typename ConstantType>
        std::pair<std::map<VariableType<ValueType>, CoefficientType<ValueType>>, ConstantType> GradientDescentInstantiationSearcher<ValueType, ConstantType>::gradientDescent(
            Environment const& env,
            bool findFeasibleInstantiation
        ) {
            std::map<VariableType<ValueType>, CoefficientType<ValueType>> bestInstantiation;
            ConstantType bestValue;
            switch (optimalityType) {
                case storm::OptimizationDirection::Maximize:
                    bestValue = -utility::infinity<ConstantType>();
                    break;
                case storm::OptimizationDirection::Minimize:
                    bestValue = utility::infinity<ConstantType>();
                    break;
            }

            // For bounded formulas, always start from random positions until point found
            if (findFeasibleInstantiation) {
                switch(resultType) {
                    case ResultType::PROBABILITY: {
                        STORM_LOG_ASSERT(formula->asProbabilityOperatorFormula().hasBound(), "Formula has to have a bound to find a feasible instantiation satisfying it");
                        break;
                    }
                    case ResultType::REWARD: {
                        STORM_LOG_ASSERT(formula->asRewardOperatorFormula().hasBound(), "Formula has to have a bound to find a feasible instantiation satisfying it");
                    }
                }
                std::random_device device;
                std::default_random_engine engine(device());
                std::uniform_real_distribution<> dist(0, 1);
                bool initialGuess = true;
                while (true) {
                    std::cout << "Trying out a new starting point" << std::endl;
                    if (initialGuess) {
                        std::cout << "Trying initial guess (p->0.5 for every parameter p or set start point)" << std::endl;
                    }
                    // Generate random starting point
                    std::map<VariableType<ValueType>, CoefficientType<ValueType>> point;
                    for (auto const& param : parameters) {
                        if (initialGuess) {
                            if (startPoint) {
                                point[param] = (*startPoint)[param];
                            } else {
                                point[param] = utility::convertNumber<CoefficientType<ValueType>>(0.5 + 1e-6);
                            }
                        } else {
                            point[param] = utility::convertNumber<CoefficientType<ValueType>>(dist(engine));
                        }
                    }
                    initialGuess = false;

                    walk.clear();                   

                    stochasticWatch.start();
                    ConstantType prob = stochasticGradientDescent(env, point);
                    stochasticWatch.stop();

                    if ((optimalityType == OptimizationDirection::Maximize && bestValue < prob)
                        || (optimalityType == solver::OptimizationDirection::Minimize && bestValue > prob)) {
                        bestInstantiation = point;
                        bestValue = prob;
                    }

                    if ((resultType == ResultType::PROBABILITY && formula->asProbabilityOperatorFormula().getBound().isSatisfied(prob)) ||
                        (resultType == ResultType::REWARD && formula->asRewardOperatorFormula().getBound().isSatisfied(prob))) {
                        std::cout << "Aborting because the bound is satisfied" << std::endl;
                        break;
                    } else {
                        std::cout << "Sorry, couldn't satisfy the bound (yet). Best found value so far: " << bestValue << std::endl;
                        continue;
                    }
                }
            } else {
                switch(resultType) {
                    case ResultType::PROBABILITY: {
                        STORM_LOG_ASSERT(!formula->asProbabilityOperatorFormula().hasBound(), "Formula cannot have bound when searching for extremum");
                        break;
                    }
                    case ResultType::REWARD: {
                        STORM_LOG_ASSERT(!formula->asRewardOperatorFormula().hasBound(), "Formula cannot have bound when searching for extremum");
                    }
                }
                std::map<VariableType<ValueType>, CoefficientType<ValueType>> point;
                for (auto const& param : parameters) {
                    if (startPoint) {
                        point[param] = (*startPoint)[param];
                    } else {
                        point[param] = utility::convertNumber<CoefficientType<ValueType>>(0.5 + 1e-6);
                    }
                }

                stochasticWatch.start();
                ConstantType prob = stochasticGradientDescent(env, point);
                stochasticWatch.stop();

                bestInstantiation = point;
                bestValue = prob;
            }

            return std::make_pair(bestInstantiation, bestValue);
        }

        template<typename ValueType, typename ConstantType>
        void GradientDescentInstantiationSearcher<ValueType, ConstantType>::printRunAsJson() {
            std::cout << "[";
            for (auto s = walk.begin(); s != walk.end(); ++s) {
                std::cout << "{";
                auto point = s->position;
                for (auto iter = point.begin(); iter != point.end(); ++iter) {
                    std::cout << "\"" << iter->first.name() << "\"";
                    std::cout << ":";
                    std::cout << utility::convertNumber<double>(iter->second);
                    std::cout << ",";
                }
                std::cout << "\"value\":";
                std::cout << s->value;
                std::cout << "}";
                if (std::next(s) != walk.end()) {
                    std::cout << ",";
                }
            }
            std::cout << "]" << std::endl;
            // Print value at last step for data collection
            std::cout << storm::utility::convertNumber<double>(walk.at(walk.size() - 1).value) << std::endl;
        }

        template<typename ValueType, typename ConstantType>
        std::vector<typename GradientDescentInstantiationSearcher<ValueType, ConstantType>::VisualizationPoint> GradientDescentInstantiationSearcher<ValueType, ConstantType>::getVisualizationWalk() {
            return walk;
        }

        template class GradientDescentInstantiationSearcher<RationalFunction, RationalNumber>;
        template class GradientDescentInstantiationSearcher<RationalFunction, double>;
    }
}
