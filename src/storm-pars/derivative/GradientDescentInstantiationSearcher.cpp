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

        template<typename FunctionType>
        using VariableType = typename utility::parametric::VariableType<FunctionType>::type;
        template<typename FunctionType>
        using CoefficientType = typename utility::parametric::CoefficientType<FunctionType>::type;

        template<typename FunctionType, typename ConstantType>
        ConstantType GradientDescentInstantiationSearcher<FunctionType, ConstantType>::doStep(
                VariableType<FunctionType> steppingParameter,
                std::map<VariableType<FunctionType>, CoefficientType<FunctionType>> &position,
                const std::map<VariableType<FunctionType>, ConstantType> &gradient,
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
                const ConstantType lengthApproxSMA = maxLengthApproxSMA - ((2 * (utility::convertNumber<ConstantType>(stepNum) + 1) * squaredAverageDecayPow) / (1 - squaredAverageDecayPow));
                // 9: If the variance is tractable, i.e. lengthApproxSMA > 4, then
                if (lengthApproxSMA > 4) {
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
                step += nesterov->momentumTerm * nesterov->pastStep.at(steppingParameter);
                nesterov->pastStep[steppingParameter] = step;
            } else {
                STORM_LOG_ERROR("GradientDescentType was not a known one");
            }

            const CoefficientType<FunctionType> convertedStep = utility::convertNumber<CoefficientType<FunctionType>>(step);
            const CoefficientType<FunctionType> newPos = position[steppingParameter] + convertedStep;
            position[steppingParameter] = newPos;
            const auto precision = storm::utility::convertNumber<CoefficientType<FunctionType>>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
            // Map parameter back to (0, 1).
            position[steppingParameter] = utility::max(utility::zero<CoefficientType<FunctionType>>() + precision, position[steppingParameter]);
            position[steppingParameter] = utility::min(utility::one<CoefficientType<FunctionType>>() - precision, position[steppingParameter]);
            return utility::abs(utility::convertNumber<ConstantType>(oldPos - position[steppingParameter]));
        }

        template<typename FunctionType, typename ConstantType>
        ConstantType GradientDescentInstantiationSearcher<FunctionType, ConstantType>::stochasticGradientDescent(
            Environment const& env,
            std::map<VariableType<FunctionType>, CoefficientType<FunctionType>> &position
        ) {
            uint_fast64_t initialState;           
            const storm::storage::BitVector initialVector = model.getInitialStates();
            for (uint_fast64_t x : initialVector) {
                initialState = x;
                break;
            }

            ConstantType currentValue;
            switch (this->currentCheckTask->getBound().comparisonType) {
                case logic::ComparisonType::Greater:
                case logic::ComparisonType::GreaterEqual:
                    currentValue = -utility::infinity<ConstantType>();
                    break;
                case logic::ComparisonType::Less:
                case logic::ComparisonType::LessEqual:
                    currentValue = utility::infinity<ConstantType>();
                    break;
            }

            // We count the number of iterations where the value changes less than the threshold, and terminate if it is large enough.
            uint64_t tinyChangeIterations = 0;

            std::map<VariableType<FunctionType>, ConstantType> deltaVector;

            std::vector<VariableType<FunctionType>> parameterEnumeration;
            for (auto parameter : this->parameters) {
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

                std::vector<VariableType<FunctionType>> miniBatch;
                for (uint_fast64_t i = parameterNum; i < std::min((uint_fast64_t) parameterEnumeration.size(), parameterNum + miniBatchSize); i++) {
                    miniBatch.push_back(parameterEnumeration[i]);
                }
                
                ConstantType oldValue = currentValue;

                // If nesterov is enabled, we need to compute the gradient on the predicted position
                std::map<VariableType<FunctionType>, CoefficientType<FunctionType>> nesterovPredictedPosition(position);
                if (Nesterov* nesterov = boost::get<Nesterov>(&gradientDescentType)) {
                    for (auto const& parameter : miniBatch) {
                        nesterovPredictedPosition[parameter] += storm::utility::convertNumber<CoefficientType<FunctionType>>(nesterov->momentumTerm)
                            * storm::utility::convertNumber<CoefficientType<FunctionType>>(nesterov->pastStep[parameter]);
                        const auto precision = storm::utility::convertNumber<CoefficientType<FunctionType>>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision());
                        nesterovPredictedPosition[parameter] = utility::max(utility::zero<CoefficientType<FunctionType>>() + precision, nesterovPredictedPosition[parameter]);
                        nesterovPredictedPosition[parameter] = utility::min(utility::one<CoefficientType<FunctionType>>() - precision, nesterovPredictedPosition[parameter]);
                    }
                }

                // Compute the value of our position and terminate if it satisfies the bound or is
                // zero or one when computing probabilities. The "valueVector" (just the probability/expected
                // reward for eventually reaching the target from every state) is also used for computing
                // the gradient later. We only need one computation of the "valueVector" per mini-batch.
                //
                // If nesterov is activated, we need to do this twice. First, to check the value of the current position.
                // Second, to compute the valueVector at the nesterovPredictedPosition.
                
                // If nesterov is deactivated, then nesterovPredictedPosition == position.
                std::unique_ptr<storm::modelchecker::CheckResult> intermediateResult = instantiationModelChecker->check(env, nesterovPredictedPosition);
                std::vector<ConstantType> valueVector = intermediateResult->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
                if (Nesterov* nesterov = boost::get<Nesterov>(&gradientDescentType)) {
                    std::unique_ptr<storm::modelchecker::CheckResult> terminationResult = instantiationModelChecker->check(env, position);
                    std::vector<ConstantType> terminationValueVector = terminationResult->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
                    currentValue = terminationValueVector[initialState];
                } else {
                    currentValue = valueVector[initialState];
                }

                if (currentCheckTask->getBound().isSatisfied(currentValue)) {
                    break;
                }

                for (auto const& parameter : miniBatch) {
                    auto checkResult = derivativeEvaluationHelper->check(env, nesterovPredictedPosition, parameter, valueVector);
                    ConstantType delta = checkResult->getValueVector()[0];
                    if (currentCheckTask->getBound().comparisonType == logic::ComparisonType::Less || currentCheckTask->getBound().comparisonType == logic::ComparisonType::LessEqual) {
                        delta *= -1;
                    }
                    deltaVector[parameter] = delta;
                }

                // Log position and probability information for later use in visualizing the descent, if wished.
                if (recordRun) {
                    VisualizationPoint point;
                    point.position = nesterovPredictedPosition;
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

        template<typename FunctionType, typename ConstantType>
        std::pair<std::map<VariableType<FunctionType>, CoefficientType<FunctionType>>, ConstantType> GradientDescentInstantiationSearcher<FunctionType, ConstantType>::gradientDescent(
            Environment const& env
        ) {
            STORM_LOG_ASSERT(this->currentCheckTask, "Call specifyFormula before calling gradientDescent");

            resetDynamicValues();

            STORM_LOG_ASSERT(this->currentCheckTask->isBoundSet(), "No bound on formula! E.g. P>=0.5 [F \"goal\"]");

            std::map<VariableType<FunctionType>, CoefficientType<FunctionType>> bestInstantiation;
            ConstantType bestValue;
            switch (this->currentCheckTask->getBound().comparisonType) {
                case logic::ComparisonType::Greater:
                case logic::ComparisonType::GreaterEqual:
                    bestValue = -utility::infinity<ConstantType>();
                    break;
                case logic::ComparisonType::Less:
                case logic::ComparisonType::LessEqual:
                    bestValue = utility::infinity<ConstantType>();
                    break;
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
                std::map<VariableType<FunctionType>, CoefficientType<FunctionType>> point;
                for (auto const& param : this->parameters) {
                    if (initialGuess) {
                        if (startPoint) {
                            point[param] = (*startPoint)[param];
                        } else {
                            point[param] = utility::convertNumber<CoefficientType<FunctionType>>(0.5 + 1e-6);
                        }
                    } else {
                        point[param] = utility::convertNumber<CoefficientType<FunctionType>>(dist(engine));
                    }
                }
                initialGuess = false;

                walk.clear();                   

                stochasticWatch.start();
                ConstantType prob = stochasticGradientDescent(env, point);
                stochasticWatch.stop();

                bool isFoundPointBetter = false;
                switch (this->currentCheckTask->getBound().comparisonType) {
                    case logic::ComparisonType::Greater:
                    case logic::ComparisonType::GreaterEqual:
                        isFoundPointBetter = prob > bestValue;
                        break;
                    case logic::ComparisonType::Less:
                    case logic::ComparisonType::LessEqual:
                        isFoundPointBetter = prob < bestValue;
                        break;
                }
                if (isFoundPointBetter) {
                    bestInstantiation = point;
                    bestValue = prob;
                }

                if (currentCheckTask->getBound().isSatisfied(bestValue)) {
                    std::cout << "Aborting because the bound is satisfied" << std::endl;
                    break;
                } else if (storm::utility::resources::isTerminate()) {
                    break;
                } else {
                    std::cout << "Sorry, couldn't satisfy the bound (yet). Best found value so far: " << bestValue << std::endl;
                    continue;
                }
            }

            return std::make_pair(bestInstantiation, bestValue);
        }

        template<typename FunctionType, typename ConstantType>
        void GradientDescentInstantiationSearcher<FunctionType, ConstantType>::resetDynamicValues() {
            if (Adam* adam = boost::get<Adam>(&gradientDescentType)) {
                for (auto const& parameter : this->parameters) {
                    adam->decayingStepAverage[parameter] = 0;
                    adam->decayingStepAverageSquared[parameter] = 0;
                }
            } else if (RAdam* radam = boost::get<RAdam>(&gradientDescentType)) {
                for (auto const& parameter : this->parameters) {
                    radam->decayingStepAverage[parameter] = 0;
                    radam->decayingStepAverageSquared[parameter] = 0;
                }
            } else if (RmsProp* rmsProp = boost::get<RmsProp>(&gradientDescentType)) {
                for (auto const& parameter : this->parameters) {
                    rmsProp->rootMeanSquare[parameter] = 0;
                }
            } else if (Momentum* momentum = boost::get<Momentum>(&gradientDescentType)) {
                for (auto const& parameter : this->parameters) {
                    momentum->pastStep[parameter] = 0;
                }
            } else if (Nesterov* nesterov = boost::get<Nesterov>(&gradientDescentType)) {
                for (auto const& parameter : this->parameters) {
                    nesterov->pastStep[parameter] = 0;
                }
            }
        }

        template<typename FunctionType, typename ConstantType>
        void GradientDescentInstantiationSearcher<FunctionType, ConstantType>::printRunAsJson() {
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

        template<typename FunctionType, typename ConstantType>
        std::vector<typename GradientDescentInstantiationSearcher<FunctionType, ConstantType>::VisualizationPoint> GradientDescentInstantiationSearcher<FunctionType, ConstantType>::getVisualizationWalk() {
            return walk;
        }

        template class GradientDescentInstantiationSearcher<RationalFunction, RationalNumber>;
        template class GradientDescentInstantiationSearcher<RationalFunction, double>;
    }
}
