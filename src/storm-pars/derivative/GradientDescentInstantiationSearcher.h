#ifndef STORM_DERIVATIVECHECKER_H
#define STORM_DERIVATIVECHECKER_H

#include <map>
#include <memory>
#include "analysis/GraphConditions.h"
#include "settings/modules/GeneralSettings.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm/exceptions/WrongFormatException.h"
#include "logic/Formula.h"
#include "solver/LinearEquationSolver.h"
#include "storm-pars/analysis/MonotonicityHelper.h"
#include "storm-pars/derivative/SparseDerivativeInstantiationModelChecker.h"
#include "storm-pars/modelchecker/instantiation/SparseDtmcInstantiationModelChecker.h"
#include "storm-pars/utility/parametric.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/Stopwatch.h"
#include "GradientDescentMethod.h"


namespace storm {
    namespace derivative {
        template <typename FunctionType, typename ConstantType>
        class GradientDescentInstantiationSearcher {

        public:
            /**
             * The GradientDescentInstantiationSearcher can find extrema and feasible instantiations in pMCs,
             * for either rewards or probabilities.
             * Internally uses the SparseDerivativeInstantiationModelChecker to evaluate derivatives at instantiations.
             * @param env The environment. Always pass the same environment to the gradientDescent call!
             * @param model The Dtmc to optimize.
             * @param method The method of gradient descent that is used. 
             * @param learningRate The learning rate of the Gradient Descent.
             * @param averageDecay Decay of ADAM's decaying average.
             * @param squaredAverageDecay Decay of ADAM's squared decaying average.
             * @param miniBatchSize Size of a minibatch.
             * @param terminationEpsilon A value step smaller than this is considered a "tiny step", after
             * a number of these the algorithm will terminate.
             * @param startPoint Start point of the search (default: all parameters set to 0.5)
             * @param recordRun Records the run into a global variable, which can be converted into JSON
             * using the printRunAsJson function 
             */
            GradientDescentInstantiationSearcher<FunctionType, ConstantType>(
                    storm::models::sparse::Dtmc<FunctionType> const model,
                    GradientDescentMethod method = GradientDescentMethod::ADAM,
                    double learningRate = 0.1,
                    double averageDecay = 0.9,
                    double squaredAverageDecay = 0.999,
                    uint_fast64_t miniBatchSize = 32,
                    double terminationEpsilon = 1e-6,
                    boost::optional<std::map<typename utility::parametric::VariableType<FunctionType>::type, typename utility::parametric::CoefficientType<FunctionType>::type>> startPoint = boost::none,
                    bool recordRun = false
            ) : model(model)
              , derivativeEvaluationHelper(std::make_unique<SparseDerivativeInstantiationModelChecker<FunctionType, ConstantType>>(model))
              , instantiationModelChecker(std::make_unique<modelchecker::SparseDtmcInstantiationModelChecker<models::sparse::Dtmc<FunctionType>, ConstantType>>(model))
              , startPoint(startPoint)
              , miniBatchSize(miniBatchSize)
              , terminationEpsilon(terminationEpsilon)
              , recordRun(recordRun) {
                switch (method) {
                    case GradientDescentMethod::ADAM: {
                        Adam adam;
                        adam.learningRate = learningRate;
                        adam.averageDecay = averageDecay;
                        adam.averageDecay = averageDecay;
                        adam.squaredAverageDecay = squaredAverageDecay;
                        gradientDescentType = adam;
                        break;
                    }
                    case GradientDescentMethod::RADAM: {
                        RAdam radam;
                        radam.learningRate = learningRate;
                        radam.averageDecay = averageDecay;
                        radam.squaredAverageDecay = squaredAverageDecay;
                        gradientDescentType = radam;
                        break;
                    }
                    case GradientDescentMethod::RMSPROP: {
                        RmsProp rmsProp;
                        rmsProp.learningRate = learningRate;
                        rmsProp.averageDecay = averageDecay;
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
             * specifyFormula specifies a CheckTask.
             * This will setup the matrices used for computing the derivatives by constructing the
             * SparseDerivativeInstantiationModelChecker. Note this can consume a substantial amount of memory if the model is
             * big and there's a large number of parameters.
             * @param env The environment. Pass the same environment as to gradientDescent. We need this because we need to know what kind of
             * equation solver we are dealing with.
             * @param checkTask The CheckTask.
             */
            void specifyFormula(Environment const& env, modelchecker::CheckTask<logic::Formula, FunctionType> const& checkTask) {
                this->currentFormula = checkTask.getFormula().asSharedPointer();
                this->currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, FunctionType>>(checkTask.substituteFormula(*currentFormula).template convertValueType<FunctionType>());

                if (!checkTask.isRewardModelSet()) {
                    this->currentFormulaNoBound = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                        checkTask.getFormula().asProbabilityOperatorFormula().getSubformula().asSharedPointer(), storm::logic::OperatorInformation(boost::none, boost::none));
                } else {
                    // No worries, this works as intended, the API is just weird.
                    this->currentFormulaNoBound = std::make_shared<storm::logic::RewardOperatorFormula>(
                        checkTask.getFormula().asRewardOperatorFormula().getSubformula().asSharedPointer());
                }
                this->currentCheckTaskNoBound = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, FunctionType>>(*currentFormulaNoBound);
                this->currentCheckTaskNoBoundConstantType = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, ConstantType>>(*currentFormulaNoBound);

                this->parameters = storm::models::sparse::getProbabilityParameters(model);
                if (checkTask.isRewardModelSet()) {
                    for (auto const& rewardParameter : storm::models::sparse::getRewardParameters(model)) {
                        this->parameters.insert(rewardParameter);
                    }
                }
                instantiationModelChecker->specifyFormula(*this->currentCheckTaskNoBound);
                derivativeEvaluationHelper->specifyFormula(env, *this->currentCheckTaskNoBound);

                // TODO: Monotonicity Checker stub :)
                /* const auto precision = storm::utility::convertNumber<typename utility::parametric::CoefficientType<FunctionType>::type>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision()); */
                /* storm::utility::parametric::Valuation<FunctionType> lowerBoundaries; */
                /* storm::utility::parametric::Valuation<FunctionType> upperBoundaries; */

                /* // TODO Make regions configurable */
                /* for (auto parameter : this->parameters) { */
                /*     lowerBoundaries[parameter] = precision; */
                /*     upperBoundaries[parameter] = 1 - precision; */
                /* } */

                /* std::shared_ptr<const storm::logic::Formula> formula = this->currentFormulaNoBound; */
                /* storage::ParameterRegion<FunctionType> region = storm::storage::ParameterRegion<FunctionType>(lowerBoundaries, upperBoundaries); */
                /* std::vector<storage::ParameterRegion<FunctionType>> regions = { region }; */
                /* std::vector<std::shared_ptr<logic::Formula const>> formulas { formula }; */
                /* std::shared_ptr<models::sparse::Dtmc<FunctionType>> modelSharedPtr = std::make_shared<models::sparse::Dtmc<FunctionType>>(model); */
                
                /* this->monotonicityHelper = std::make_unique<storm::analysis::MonotonicityHelper<FunctionType, ConstantType>>(modelSharedPtr, formulas, regions); */

                /* std::cout << "Checking monotonicity..." << std::endl; */
                /* std::map<std::shared_ptr<storm::analysis::Order>, std::pair<std::shared_ptr<storm::analysis::MonotonicityResult<typename utility::parametric::VariableType<FunctionType>::type>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>> resultMap = monotonicityHelper->checkMonotonicityInBuild(std::cout); */
                /* for (auto const& orderResult : resultMap) { */
                /*     for (auto const& parameter : this->parameters) { */
                /*         if (orderResult.second.first->isMonotone(parameter)) { */
                /*             std::cout << "Is montone in " << parameter << std::endl; */
                /*         } */
                /*     } */
                /* } */
            }


            /**
             * Perform Gradient Descent.
             * @param env The environment. Pass the same environment as to specifyFormula.
             */
            std::pair<std::map<typename utility::parametric::VariableType<FunctionType>::type, typename utility::parametric::CoefficientType<FunctionType>::type>, ConstantType> gradientDescent(
                Environment const& env
            );

            /**
             * Print the previously done run as JSON. This run can be retrieved using getVisualizationWalk.
             */
            void printRunAsJson();

            /**
             * A point in the Gradient Descent walk, recorded if recordRun is set to true in the constructor (false by default).
             */
            struct VisualizationPoint {
                std::map<typename utility::parametric::VariableType<FunctionType>::type, typename utility::parametric::CoefficientType<FunctionType>::type> position;
                ConstantType value;
            };
            /**
             * Get the visualization walk that is recorded if recordRun is set to true in the constructor (false by default).
             */
            std::vector<VisualizationPoint> getVisualizationWalk();
            
        private:
            void resetDynamicValues();

            std::unique_ptr<modelchecker::CheckTask<storm::logic::Formula, FunctionType>> currentCheckTask;
            std::unique_ptr<modelchecker::CheckTask<storm::logic::Formula, FunctionType>> currentCheckTaskNoBound;
            std::unique_ptr<modelchecker::CheckTask<storm::logic::Formula, ConstantType>> currentCheckTaskNoBoundConstantType;
            std::shared_ptr<storm::logic::Formula const> currentFormula;
            std::shared_ptr<storm::logic::Formula const> currentFormulaNoBound;

            const models::sparse::Dtmc<FunctionType> model;
            std::set<typename utility::parametric::VariableType<FunctionType>::type> parameters;
            std::unique_ptr<storm::derivative::SparseDerivativeInstantiationModelChecker<FunctionType, ConstantType>> derivativeEvaluationHelper;
            std::unique_ptr<storm::analysis::MonotonicityHelper<FunctionType, ConstantType>> monotonicityHelper;
            const std::unique_ptr<modelchecker::SparseDtmcInstantiationModelChecker<models::sparse::Dtmc<FunctionType>, ConstantType>> instantiationModelChecker;
            boost::optional<std::map<typename utility::parametric::VariableType<FunctionType>::type, typename utility::parametric::CoefficientType<FunctionType>::type>> startPoint;
            const uint_fast64_t miniBatchSize;
            const double terminationEpsilon;

            // This is for visualizing data
            const bool recordRun;
            std::vector<VisualizationPoint> walk; 


            // Gradient Descent types and data that belongs to them, with hyperparameters and running data.
            struct Adam {
                double averageDecay;
                double squaredAverageDecay;
                double learningRate;
                std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> decayingStepAverageSquared;
                std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> decayingStepAverage;
            };
            struct RAdam {
                double averageDecay;
                double squaredAverageDecay;
                double learningRate;
                std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> decayingStepAverageSquared;
                std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> decayingStepAverage;
            };
            struct RmsProp {
                double averageDecay;
                double learningRate;
                std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> rootMeanSquare;
            };
            struct Plain {
                double learningRate;
            };
            struct Momentum {
                double learningRate;
                double momentumTerm;
                std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> pastStep;
            };
            struct Nesterov {
                double learningRate;
                double momentumTerm;
                std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> pastStep;
            };
            typedef boost::variant<Adam, RAdam, RmsProp, Plain, Momentum, Nesterov> GradientDescentType;
            GradientDescentType gradientDescentType;
            // Only respected by some Gradient Descent methods, the ones that have a "sign" version in the GradientDescentMethod enum
            bool useSignsOnly;

            ConstantType stochasticGradientDescent(
                Environment const& env,
                std::map<typename utility::parametric::VariableType<FunctionType>::type, typename utility::parametric::CoefficientType<FunctionType>::type> &position
            );
            ConstantType doStep(
                typename utility::parametric::VariableType<FunctionType>::type steppingParameter,
                std::map<typename utility::parametric::VariableType<FunctionType>::type, typename utility::parametric::CoefficientType<FunctionType>::type> &position,
                const std::map<typename utility::parametric::VariableType<FunctionType>::type, ConstantType> &gradient,
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
