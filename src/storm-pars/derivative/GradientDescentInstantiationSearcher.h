#ifndef STORM_DERIVATIVECHECKER_H
#define STORM_DERIVATIVECHECKER_H

#include <map>
#include "analysis/GraphConditions.h"
#include "storm/exceptions/WrongFormatException.h"
#include "logic/Formula.h"
#include "solver/LinearEquationSolver.h"
#include "storm-pars/analysis/MonotonicityChecker.h"
#include "storm-pars/derivative/DerivativeEvaluationHelper.h"
#include "storm-pars/modelchecker/instantiation/SparseDtmcInstantiationModelChecker.h"
#include "storm-pars/utility/parametric.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/Stopwatch.h"
#include "GradientDescentMethod.h"


namespace storm {
    namespace derivative {
        template <typename ValueType, typename ConstantType>
        class GradientDescentInstantiationSearcher {

        public:
            /**
             * The GradientDescentInstantiationSearcher can find extrema and feasible instantiations in pMCs,
             * for either rewards or probabilities.
             * Internally uses the DerivativeEvaluationHelper to evaluate derivatives at instantiations.
             * @param env The environment. Always pass the same environment to the gradientDescent call!
             * @param model The Dtmc to optimize. This must have _one_
             * target state labeled "target" and _one_ initial state labeled "init". Note this is exactly the
             * kind of Dtmc the SparseParametricDtmcSimplifier spits out.
             * The constructor will setup the matrices used for computing the derivatives by constructing the
             * DerivativeEvaluationHelper. Note this can consume a substantial amount of memory if the model is
             * big and there's a large number of parameters.
             * @param parameters The Dtmc's parameters. See storm::models::sparse::getAllParameters
             * @param formulas The first element of this is considered, which needs to be an eventually formula.
             * A "Rmax=?" or "Pmin=?" formula needs to be input to find an optimum.
             * A "Rmax>=0.5" or "Pmin<=0.159" formula needs to be input to find a feasible instantiation.
             * @param method The method of graident descent that is used. 
             * @param learningRate The learning rate of the Gradient Descent.
             * @param averageDecay Decay of ADAM's decaying average.
             * @param squaredAverageDecay Decay of ADAM's squared decaying average.
             * @param miniBatchSize Size of a minibatch.
             * @param terminationEpsilon A value step smaller than this is considered a "tiny step", after
             * a number of these the algorithm will terminate.
             * @param startPoint Start point of the search (default: all parameters set to 0.5)
             * @param recordRun Records the run into a global variable, which can be converted into JSON
             * using the printRunAsJson function (currently not exposed as API though).
             */
            GradientDescentInstantiationSearcher<ValueType, ConstantType>(
                    Environment const& env,
                    std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> const model,
                    std::set<typename utility::parametric::VariableType<ValueType>::type> const parameters,
                    std::vector<std::shared_ptr<storm::logic::Formula const>> const formulas,
                    GradientDescentMethod method = GradientDescentMethod::ADAM,
                    double learningRate = 0.1,
                    double averageDecay = 0.9,
                    double squaredAverageDecay = 0.999,
                    uint_fast64_t miniBatchSize = 32,
                    double terminationEpsilon = 1e-6,
                    boost::optional<std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>> startPoint = boost::none,
                    bool recordRun = false
            ) : model(model)
              , parameters(parameters)
              , formula(formulas[0])
              , instantiationModelChecker(std::make_unique<modelchecker::SparseDtmcInstantiationModelChecker<models::sparse::Dtmc<ValueType>, ConstantType>>(*model))
              /* , monotonicityChecker(std::make_unique<analysis::MonotonicityChecker<ValueType>>(model, formulas, std::vector<storm::storage::ParameterRegion<ValueType>>(), true)) */
              , miniBatchSize(miniBatchSize)
              , terminationEpsilon(terminationEpsilon)
              , startPoint(startPoint)
              , recordRun(recordRun) {
                if (formula->isProbabilityOperatorFormula()) {
                    resultType = ResultType::PROBABILITY;
                    derivativeEvaluationHelper = std::move(std::make_unique<storm::derivative::DerivativeEvaluationHelper<ValueType, ConstantType>>(env, model, parameters, formulas));
                } else if (formula->isRewardOperatorFormula()) {
                    resultType = ResultType::REWARD;
                    derivativeEvaluationHelper = std::move(std::make_unique<storm::derivative::DerivativeEvaluationHelper<ValueType, ConstantType>>(env, model, parameters, formulas, ResultType::REWARD, std::string("")));
                } else {
                    STORM_LOG_ERROR("Formula must be reward operator formula or probability operator formula!");
                }
                switch (resultType) {
                    case ResultType::PROBABILITY: {
                        STORM_LOG_THROW(formula->asProbabilityOperatorFormula().hasOptimalityType(),
                                storm::exceptions::WrongFormatException,
                                "no optimality type in formula");
                        optimalityType = formula->asProbabilityOperatorFormula().getOptimalityType();
                        if (formula->asProbabilityOperatorFormula().hasBound()) {
                            bound = boost::make_optional(formula->asProbabilityOperatorFormula().getBound());
                        }
                        break;
                    }
                    case ResultType::REWARD: {
                        STORM_LOG_THROW(formula->asRewardOperatorFormula().hasOptimalityType(),
                                storm::exceptions::WrongFormatException,
                                "no optimality type in formula");
                        optimalityType = formula->asRewardOperatorFormula().getOptimalityType();
                        if (formula->asRewardOperatorFormula().hasBound()) {
                            bound = boost::make_optional(formula->asRewardOperatorFormula().getBound());
                        }
                    }
                }

                switch (resultType) {
                    case ResultType::PROBABILITY: {
                        auto formulaWithoutBound = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                                formulas[0]->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), storm::logic::OperatorInformation(boost::none, boost::none));
                        const storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> checkTask
                            = storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>(*formulaWithoutBound);
                        instantiationModelChecker->specifyFormula(checkTask);
                        break;
                    }
                    case ResultType::REWARD: {
                        auto formulaWithoutBound = std::make_shared<storm::logic::RewardOperatorFormula>(
                                formulas[0]->asRewardOperatorFormula().getSubformula().asSharedPointer());
                        const storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> checkTask
                            = storm::modelchecker::CheckTask<storm::logic::Formula, ValueType>(*formulaWithoutBound);
                        instantiationModelChecker->specifyFormula(checkTask);
                        break;
                    }
                }

                switch (method) {
                    case GradientDescentMethod::ADAM: {
                        Adam adam;
                        adam.learningRate = learningRate;
                        adam.averageDecay = averageDecay;
                        adam.averageDecay = averageDecay;
                        adam.squaredAverageDecay = squaredAverageDecay;
                        for (auto const& parameter : parameters) {
                            adam.decayingStepAverage[parameter] = 0;
                            adam.decayingStepAverageSquared[parameter] = 0;
                        }
                        gradientDescentType = adam;
                        break;
                    }
                    case GradientDescentMethod::RADAM: {
                        RAdam radam;
                        radam.learningRate = learningRate;
                        radam.averageDecay = averageDecay;
                        radam.averageDecay = averageDecay;
                        radam.squaredAverageDecay = squaredAverageDecay;
                        for (auto const& parameter : parameters) {
                            radam.decayingStepAverage[parameter] = 0;
                            radam.decayingStepAverageSquared[parameter] = 0;
                        }
                        gradientDescentType = radam;
                        break;
                    }
                    case GradientDescentMethod::RMSPROP: {
                        RmsProp rmsProp;
                        rmsProp.learningRate = learningRate;
                        rmsProp.averageDecay = averageDecay;
                        for (auto const& parameter : parameters) {
                            rmsProp.rootMeanSquare[parameter] = 0;
                        }
                        gradientDescentType = rmsProp;
                        break;
                    }
                    case GradientDescentMethod::PLAIN:
                    case GradientDescentMethod::PLAIN_SIGN: {
                        Plain plain;
                        plain.learningRate = learningRate;
                        gradientDescentType = plain;
                        if (method == GradientDescentMethod::PLAIN_SIGN) {
                            useSignsOnly = true;
                        } else {
                            useSignsOnly = false;
                        }
                        break;
                    }
                    case GradientDescentMethod::MOMENTUM: 
                    case GradientDescentMethod::MOMENTUM_SIGN: {
                        Momentum momentum;
                        momentum.learningRate = learningRate;
                        // TODO Document this
                        momentum.momentumTerm = averageDecay;
                        for (auto const& parameter : parameters) {
                            momentum.pastStep[parameter] = 0;
                        }
                        gradientDescentType = momentum;
                        if (method == GradientDescentMethod::MOMENTUM_SIGN) {
                            useSignsOnly = true;
                        } else {
                            useSignsOnly = false;
                        }
                        break;
                    }
                    case GradientDescentMethod::NESTEROV:
                    case GradientDescentMethod::NESTEROV_SIGN: {
                        Nesterov nesterov;
                        nesterov.learningRate = learningRate;
                        // TODO Document this
                        nesterov.momentumTerm = averageDecay;
                        for (auto const& parameter : parameters) {
                            nesterov.predictedLastStep[parameter] = 0;
                            nesterov.actualLastStep[parameter] = 0;
                        }
                        gradientDescentType = nesterov;
                        if (method == GradientDescentMethod::NESTEROV_SIGN) {
                            useSignsOnly = true;
                        } else {
                            useSignsOnly = false;
                        }
                        break;
                    }
                }
            }
            /**
             * Perform Gradient Descent.
             * @param env The environment. Always pass the same environment as the constructor!
             * @param findFeasibleInstantiation true iff a feasible instantiation needs to be found, false iff an optimum
             * needs to be found. This must be compatible with the input formula.
             */
            std::pair<std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>, ConstantType> gradientDescent(
                Environment const& env,
                bool findFeasibleInstantiation
            );
            /**
             * Print the previously done run as JSON. This run can be retrieved using getVisualizationWalk.
             */
            void printRunAsJson();
            
            /**
             * A point in the Gradient Descent walk, recorded if recordRun is set to true in the constructor (false by default).
             */
            struct VisualizationPoint {
                std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type> position;
                ConstantType value;
            };
            /**
             * Get the visualization walk that is recorded if recordRun is set to true in the constructor (false by default).
             */
            std::vector<VisualizationPoint> getVisualizationWalk();
            
        private:
            const std::shared_ptr<models::sparse::Dtmc<ValueType>> model;
            const std::set<typename utility::parametric::VariableType<ValueType>::type> parameters;
            std::unique_ptr<storm::derivative::DerivativeEvaluationHelper<ValueType, ConstantType>> derivativeEvaluationHelper;
            const std::shared_ptr<storm::logic::Formula const> formula;
            const std::unique_ptr<modelchecker::SparseDtmcInstantiationModelChecker<models::sparse::Dtmc<ValueType>, ConstantType>> instantiationModelChecker;
            const std::unique_ptr<storm::analysis::MonotonicityChecker<ValueType>> monotonicityChecker;
            const uint_fast64_t miniBatchSize;
            const double terminationEpsilon;
            ResultType resultType;
            boost::optional<storm::logic::Bound> bound;
            OptimizationDirection optimalityType;
            boost::optional<std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type>> startPoint;

            // This is for visualizing data
            const bool recordRun;
            std::vector<VisualizationPoint> walk; 


            // Gradient Descent types and data that belongs to them, with hyperparameters and running data.
            struct Adam {
                double averageDecay;
                double squaredAverageDecay;
                double learningRate;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> decayingStepAverageSquared;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> decayingStepAverage;
            };
            struct RAdam {
                double averageDecay;
                double squaredAverageDecay;
                double learningRate;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> decayingStepAverageSquared;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> decayingStepAverage;
            };
            struct RmsProp {
                double averageDecay;
                double learningRate;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> rootMeanSquare;
            };
            struct Plain {
                double learningRate;
            };
            struct Momentum {
                double learningRate;
                double momentumTerm;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> pastStep;
            };
            struct Nesterov {
                double learningRate;
                double momentumTerm;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> actualLastStep;
                std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> predictedLastStep;
            };
            typedef boost::variant<Adam, RAdam, RmsProp, Plain, Momentum, Nesterov> GradientDescentType;
            GradientDescentType gradientDescentType;
            // Only respected by some Gradient Descent methods, the ones that have a "sign" version in the GradientDescentMethod enum
            bool useSignsOnly;

            ConstantType stochasticGradientDescent(
                Environment const& env,
                std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type> &position
            );
            ConstantType doStep(
                typename utility::parametric::VariableType<ValueType>::type steppingParameter,
                std::map<typename utility::parametric::VariableType<ValueType>::type, typename utility::parametric::CoefficientType<ValueType>::type> &position,
                const std::map<typename utility::parametric::VariableType<ValueType>::type, ConstantType> &gradient,
                uint_fast64_t stepNum
            );
            ConstantType constantTypeSqrt(ConstantType input) {
                if (std::is_same<ConstantType, double>::value) {
                    return utility::sqrt(input);
                } else {
                    return carl::sqrt(input);
                }
            }

            utility::Stopwatch stochasticWatch;
            utility::Stopwatch batchWatch;
            utility::Stopwatch startingPointCalculationWatch;
        };
    }
}

#endif //STORM_DERIVATIVECHECKER_H
