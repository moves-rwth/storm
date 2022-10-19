#include "MonotonicityHelper.h"
#include "storm-pars/analysis/AssumptionChecker.h"
#include "storm-pars/analysis/ReachabilityOrderExtender.h"
#include "storm-pars/analysis/RewardOrderExtender.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/ModelType.h"

namespace storm {
namespace analysis {
/*** Constructor ***/
template<typename ValueType, typename ConstantType>
MonotonicityHelper<ValueType, ConstantType>::MonotonicityHelper(std::shared_ptr<models::sparse::Model<ValueType>> model,
                                                                std::vector<std::shared_ptr<logic::Formula const>> formulas,
                                                                std::vector<storage::ParameterRegion<ValueType>> regions, uint_fast64_t numberOfSamples,
                                                                double const& precision, bool dotOutput)
    : assumptionMaker(model->getTransitionMatrix()) {
    STORM_LOG_ASSERT(model != nullptr, "Expecting model to be provided for monotonicity helper");
    if (model->hasUniqueRewardModel()) {
        this->assumptionMaker.setRewardModel(std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(model->getUniqueRewardModel()));
    }
    this->model = model;
    this->formulas = formulas;
    this->precision = utility::convertNumber<ConstantType>(precision);
    this->matrix = model->getTransitionMatrix();
    this->dotOutput = dotOutput;

    if (regions.size() == 1) {
        this->region = *(regions.begin());
    } else {
        typename storage::ParameterRegion<ValueType>::Valuation lowerBoundaries;
        typename storage::ParameterRegion<ValueType>::Valuation upperBoundaries;
        std::set<VariableType> vars;
        vars = models::sparse::getProbabilityParameters(*model);
        for (auto var : vars) {
            typename storage::ParameterRegion<ValueType>::CoefficientType lb = utility::convertNumber<CoefficientType>(0 + precision);
            typename storage::ParameterRegion<ValueType>::CoefficientType ub = utility::convertNumber<CoefficientType>(1 - precision);
            lowerBoundaries.insert(std::make_pair(var, lb));
            upperBoundaries.insert(std::make_pair(var, ub));
        }
        this->region = storage::ParameterRegion<ValueType>(std::move(lowerBoundaries), std::move(upperBoundaries));
    }

    if (numberOfSamples > 2) {
        // sampling
        if (model->isOfType(models::ModelType::Dtmc)) {
            checkMonotonicityOnSamples(model->template as<models::sparse::Dtmc<ValueType>>(), numberOfSamples);
        } else if (model->isOfType(models::ModelType::Mdp)) {
            checkMonotonicityOnSamples(model->template as<models::sparse::Mdp<ValueType>>(), numberOfSamples);
        }
        checkSamples = true;
    } else {
        if (numberOfSamples > 0) {
            STORM_LOG_WARN("At least 3 sample points are needed to check for monotonicity on samples, not using samples for now");
        }
        checkSamples = false;
    }

    if (model->isOfType(models::ModelType::Dtmc) || model->isOfType(models::ModelType::Mdp)) {
        if (formulas[0]->isProbabilityOperatorFormula()) {
            this->extender = new analysis::ReachabilityOrderExtender<ValueType, ConstantType>(model, formulas[0]);
        } else if (formulas[0]->isRewardOperatorFormula()) {
            this->extender = new analysis::RewardOrderExtender<ValueType, ConstantType>(model, formulas[0]);

        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Monotonicity checking not implemented for property" << formulas[0]);
        }
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Monotonicity checking not implemented for model type: " << model->getType());
    }

    for (uint_fast64_t i = 0; i < matrix.getRowCount(); ++i) {
        std::set<VariableType> occurringVariables;

        for (auto& entry : matrix.getRow(i)) {
            storm::utility::parametric::gatherOccurringVariables(entry.getValue(), occurringVariables);
        }
        for (auto& var : occurringVariables) {
            occuringStatesAtVariable[var].push_back(i);
        }
    }
}

/*** Public methods ***/
template<typename ValueType, typename ConstantType>
std::map<std::shared_ptr<Order>,
         std::pair<std::shared_ptr<MonotonicityResult<typename MonotonicityHelper<ValueType, ConstantType>::VariableType>>, std::vector<Assumption>>>
MonotonicityHelper<ValueType, ConstantType>::checkMonotonicityInBuild(boost::optional<std::ostream&> outfile, bool useBoundsFromPLA,
                                                                      std::string dotOutfileName) {
    if (useBoundsFromPLA) {
        storm::utility::Stopwatch plaWatch(true);
        this->extender->initializeMinMaxValues(region);
        plaWatch.stop();
        STORM_PRINT("\nTotal time for pla checking: " << plaWatch << ".\n\n");
    }
    createOrder();

    // TODO Don't print if order is incomplete

    // output of results
    if (monResults.size() == 1) {
        auto itr = monResults.begin();
        if (itr->first != nullptr) {
            STORM_PRINT("Number of done states: " << itr->first->getNumberOfSufficientStates() << std::endl);
        }
        if (checkSamples) {
            for (auto& entry : resultCheckOnSamples.getMonotonicityResult()) {
                if (entry.second == Monotonicity::Not) {
                    itr->second.first->updateMonotonicityResult(entry.first, entry.second, true);
                }
            }
        }
        if (outfile) {
            outfile.get() << "Assumptions: no\n";
            outfile.get() << "Monotonicity Result: \n"
                          << "    " << itr->second.first->toString() << "\n\n";
        } else {
            STORM_PRINT("Assumptions: no\n"
                        << "Monotonicity Result: \n"
                        << "    " << itr->second.first->toString() << "\n\n");
        }
    } else if (monResults.size() > 1) {
        storm::analysis::MonotonicityResult<VariableType> finalRes;
        for (auto itr : monResults) {
            finalRes.merge(itr.second.first);
        }

        if (checkSamples) {
            for (auto& entry : resultCheckOnSamples.getMonotonicityResult()) {
                if (entry.second == Monotonicity::Not) {
                    finalRes.updateMonotonicityResult(entry.first, entry.second, true);
                }
            }
        }

        if (outfile) {
            outfile.get() << "Assumptions: yes\n";
            outfile.get() << "Monotonicity Result: \n"
                          << "    " << finalRes.toString() << "\n\n";
        } else {
            STORM_PRINT("Assumptions: yes\n"
                        << "Monotonicity Result: \n"
                        << "    " << finalRes.toString() << "\n\n");
        }
    } else {
        if (outfile) {
            outfile.get() << "No monotonicity found, as the order is insufficient\n";
            if (checkSamples) {
                outfile.get() << "Monotonicity Result on samples: " << resultCheckOnSamples.toString() << '\n';
            }
        } else {
            STORM_PRINT("No monotonicity found, as the order is insufficient");
            if (checkSamples) {
                STORM_PRINT("Monotonicity Result on samples: " << resultCheckOnSamples.toString());
            }
        }
    }

    // dotoutput
    if (dotOutput) {
        STORM_LOG_WARN_COND(monResults.size() <= 10, "Too many Reachability Orders. Dot Output will only be created for 10.");
        int i = 0;
        auto orderItr = monResults.begin();
        while (i < 10 && orderItr != monResults.end()) {
            std::ofstream dotOutfile;
            std::string name = dotOutfileName + std::to_string(i);
            utility::openFile(name, dotOutfile);
            dotOutfile << "Assumptions:\n";
            auto assumptionItr = orderItr->second.second.begin();
            while (assumptionItr != orderItr->second.second.end()) {
                dotOutfile << *assumptionItr << '\n';
                dotOutfile << '\n';
                assumptionItr++;
            }
            dotOutfile << '\n';
            orderItr->first->dotOutputToFile(dotOutfile);
            utility::closeFile(dotOutfile);
            i++;
            orderItr++;
        }
    }
    return monResults;
}

/*** Private methods ***/
template<typename ValueType, typename ConstantType>
void MonotonicityHelper<ValueType, ConstantType>::createOrder() {
    // Transform to Orders
    std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> criticalTuple;

    // Create initial order
    auto monRes = std::make_shared<MonotonicityResult<VariableType>>(MonotonicityResult<VariableType>());
    criticalTuple = toOrder(region, monRes);
    // Continue based on not (yet) sorted states
    std::map<std::shared_ptr<Order>, std::vector<Assumption>> result;

    auto val1 = std::get<1>(criticalTuple);
    auto val2 = std::get<2>(criticalTuple);
    auto numberOfStates = model->getNumberOfStates();
    std::vector<Assumption> assumptions;

    if (val1 == numberOfStates && val2 == numberOfStates) {
        auto resAssumptionPair = std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<Assumption>>(monRes, assumptions);
        monResults.insert(std::pair<std::shared_ptr<Order>, std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<Assumption>>>(
            std::get<0>(criticalTuple), resAssumptionPair));
    } else if (val1 != numberOfStates && val2 != numberOfStates) {
        extendOrderWithAssumptions(std::get<0>(criticalTuple), val1, val2, assumptions, monRes);
    } else {
        assert(false);
    }
}

template<typename ValueType, typename ConstantType>
void MonotonicityHelper<ValueType, ConstantType>::extendOrderWithAssumptions(std::shared_ptr<Order> order, uint_fast64_t val1, uint_fast64_t val2,
                                                                             std::vector<Assumption> assumptions,
                                                                             std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
    std::map<std::shared_ptr<Order>, std::vector<Assumption>> result;
    if (order->isInvalid()) {
        STORM_LOG_INFO("Order is invalid, probably found with forward reasoning");
        return;
    }
    for (auto& assumption : assumptions) {
        // Check if the assumptions are still valid, otherwise we can stop
        // One of the assumption was not valid on the entire region, so we don't need to further explore the order
        // We use compareFast as the assumption is used, thus statesAbove of the node is set
        if (assumption.isStateAssumption()) {
            auto state1 = std::stoi(assumption.getAssumption()->getFirstOperand()->toExpression().toString());
            auto state2 = std::stoi(assumption.getAssumption()->getSecondOperand()->toExpression().toString());
            if (assumption.getAssumption()->asBinaryRelationExpression().getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater) {
                if (order->compareFast(state1, state2) != Order::NodeComparison::ABOVE) {
                    STORM_LOG_INFO("Removing order as assumption " << assumption.getAssumption()->toExpression().toString() << " is invalid");
                    return;
                }
            } else {
                if (order->compareFast(state1, state2) != Order::NodeComparison::SAME) {
                    STORM_LOG_INFO("Removing order as assumption " << assumption.getAssumption()->toExpression().toString() << " is invalid");
                    return;
                }
            }
        } else {
            // assumption is based on the actions
            STORM_LOG_INFO("Not quickly validating action assumption as this is too expensive");
        }
    }
    auto numberOfStates = model->getNumberOfStates();
    if (val1 == numberOfStates || val2 == numberOfStates) {
        assert(val1 == val2);
        assert(order->getNumberOfAddedStates() == order->getNumberOfStates());
        auto resAssumptionPair = std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<Assumption>>(monRes, assumptions);
        monResults.insert(std::pair<std::shared_ptr<Order>, std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<Assumption>>>(
            std::move(order), std::move(resAssumptionPair)));
    } else {
        // Make the assumptions
        if (val1 == val2) {
            STORM_LOG_INFO("Creating assumptions for the actions of " << val1 << ". ");
        } else {
            STORM_LOG_INFO("Creating assumptions for " << val1 << " and " << val2 << ". ");
        }
        auto newAssumptions = assumptionMaker.createAndCheckAssumptions(val1, val2, order, region);
        auto itr = newAssumptions.begin();
        if (newAssumptions.size() == 0) {
            monRes = std::make_shared<MonotonicityResult<VariableType>>(MonotonicityResult<VariableType>());
            for (auto& entry : occuringStatesAtVariable) {
                for (auto& state : entry.second) {
                    extender->checkParOnStateMonRes(state, order, entry.first, region, monRes);
                    if (monRes->getMonotonicity(entry.first) == Monotonicity::Unknown) {
                        break;
                    }
                }
                monRes->setDoneForVar(entry.first);
            }
            monResults.insert({order, {monRes, assumptions}});
            STORM_LOG_INFO("    None of the assumptions were valid, we stop exploring the current order");
        } else {
            if (newAssumptions.begin()->first.isStateAssumption()) {
                STORM_LOG_INFO("    Created " << newAssumptions.size() << " state assumptions, we continue extending the current order");
            } else {
                STORM_LOG_INFO("    Created " << newAssumptions.size() << " action assumptions, we continue extending the current order");
            }
        }

        while (itr != newAssumptions.end()) {
            auto assumption = *itr;
            ++itr;
            if (assumption.second != AssumptionStatus::INVALID) {
                if (itr != newAssumptions.end()) {
                    // We make a copy of the order and the assumptions
                    auto orderCopy = order->copy();
                    auto assumptionsCopy = std::vector<Assumption>(assumptions);
                    auto monResCopy = monRes->copy();

                    if (assumption.second == AssumptionStatus::UNKNOWN) {
                        // only add assumption to the set of assumptions if it is unknown whether it holds or not
                        assumptionsCopy.push_back(std::move(assumption.first));
                    }
                    auto criticalTuple = extendOrder(orderCopy, region, monResCopy, assumption.first);
                    extendOrderWithAssumptions(std::get<0>(criticalTuple), std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptionsCopy, monResCopy);
                } else {
                    // It is the last one, so we don't need to create a copy.
                    if (assumption.second == AssumptionStatus::UNKNOWN) {
                        // only add assumption to the set of assumptions if it is unknown whether it holds or not
                        assumptions.push_back(std::move(assumption.first));
                    }
                    auto criticalTuple = extendOrder(order, region, monRes, assumption.first);
                    extendOrderWithAssumptions(std::get<0>(criticalTuple), std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptions, monRes);
                }
            }
        }
    }
}

template<typename ValueType, typename ConstantType>
std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> MonotonicityHelper<ValueType, ConstantType>::toOrder(
    storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
    ReachabilityOrderExtender<ValueType, ConstantType>* castedPointerReach = dynamic_cast<ReachabilityOrderExtender<ValueType, ConstantType>*>(extender);
    if (castedPointerReach != nullptr) {
        return castedPointerReach->toOrder(region, monRes);
    }
    RewardOrderExtender<ValueType, ConstantType>* castedPointerRew = dynamic_cast<RewardOrderExtender<ValueType, ConstantType>*>(extender);
    if (castedPointerRew != nullptr) {
        return castedPointerRew->toOrder(region, monRes);
    }
    STORM_LOG_ASSERT(false, "Unexpected order extender type");
    return extender->toOrder(region, monRes);
}

template<typename ValueType, typename ConstantType>
std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> MonotonicityHelper<ValueType, ConstantType>::extendOrder(
    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes,
    std::optional<Assumption> assumption) {
    ReachabilityOrderExtender<ValueType, ConstantType>* castedPointerReach = dynamic_cast<ReachabilityOrderExtender<ValueType, ConstantType>*>(extender);
    if (castedPointerReach != nullptr) {
        return castedPointerReach->extendOrder(order, region, monRes, assumption);
    }
    RewardOrderExtender<ValueType, ConstantType>* castedPointerRew = dynamic_cast<RewardOrderExtender<ValueType, ConstantType>*>(extender);
    if (castedPointerRew != nullptr) {
        return castedPointerRew->extendOrder(order, region, monRes, assumption);
    }
    STORM_LOG_ASSERT(false, "Unexpected order extender type");
    return extender->extendOrder(order, region, monRes, assumption);
}

template<typename ValueType, typename ConstantType>
void MonotonicityHelper<ValueType, ConstantType>::checkMonotonicityOnSamples(std::shared_ptr<models::sparse::Dtmc<ValueType>> model,
                                                                             uint_fast64_t numberOfSamples) {
    assert(numberOfSamples > 2);

    auto instantiator = utility::ModelInstantiator<models::sparse::Dtmc<ValueType>, models::sparse::Dtmc<ConstantType>>(*model);
    std::set<VariableType> variables = models::sparse::getProbabilityParameters(*model);
    std::vector<std::vector<ConstantType>> samples;
    // For each of the variables create a model in which we only change the value for this specific variable
    for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
        ConstantType previous = -1;
        bool monDecr = true;
        bool monIncr = true;

        // Check monotonicity in variable (*itr) by instantiating the model
        // all other variables fixed on lb, only increasing (*itr)
        for (uint_fast64_t i = 0; (monDecr || monIncr) && i < numberOfSamples; ++i) {
            // Create valuation
            auto valuation = utility::parametric::Valuation<ValueType>();
            for (auto itr2 = variables.begin(); itr2 != variables.end(); ++itr2) {
                // Only change value for current variable
                if ((*itr) == (*itr2)) {
                    auto lb = region.getLowerBoundary(itr->name());
                    auto ub = region.getUpperBoundary(itr->name());
                    // Creates samples between lb and ub, that is: lb, lb + (ub-lb)/(#samples -1), lb + 2* (ub-lb)/(#samples -1), ..., ub
                    valuation[*itr2] = (lb + utility::convertNumber<CoefficientType>(i / (numberOfSamples - 1)) * (ub - lb));
                } else {
                    auto lb = region.getLowerBoundary(itr2->name());
                    valuation[*itr2] = utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(lb);
                }
            }

            // Instantiate model and get result
            models::sparse::Dtmc<ConstantType> sampleModel = instantiator.instantiate(valuation);
            auto checker = modelchecker::SparseDtmcPrctlModelChecker<models::sparse::Dtmc<ConstantType>>(sampleModel);
            std::unique_ptr<modelchecker::CheckResult> checkResult;
            auto formula = formulas[0];
            if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                const modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask =
                    modelchecker::CheckTask<logic::UntilFormula, ConstantType>((*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
            } else if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                const modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask =
                    modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>(
                        (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
                checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
            } else if (formula->isRewardOperatorFormula() && formula->asRewardOperatorFormula().getSubformula().isUntilFormula()) {
                const modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask =
                    modelchecker::CheckTask<logic::UntilFormula, ConstantType>((*formula).asRewardOperatorFormula().getSubformula().asUntilFormula());
                checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
            } else if (formula->isRewardOperatorFormula() && formula->asRewardOperatorFormula().getSubformula().isEventuallyFormula()) {
                const modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask =
                    modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>((*formula).asRewardOperatorFormula().getSubformula().asEventuallyFormula());
                checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
            } else {
                STORM_LOG_THROW(false, exceptions::NotSupportedException, "Expecting until or eventually formula");
            }

            auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<ConstantType>();
            std::vector<ConstantType> values = quantitativeResult.getValueVector();
            auto initialStates = model->getInitialStates();
            ConstantType initial = 0;
            // Get total probability from initial states
            for (auto j = initialStates.getNextSetIndex(0); j < model->getNumberOfStates(); j = initialStates.getNextSetIndex(j + 1)) {
                initial += values[j];
            }
            // Calculate difference with result for previous valuation
            assert(initial >= 0 - precision && initial <= 1 + precision);
            ConstantType diff = previous - initial;
            assert(previous == -1 || (diff >= -1 - precision && diff <= 1 + precision));

            if (previous != -1 && (diff > precision || diff < -precision)) {
                monDecr &= diff > precision;  // then previous value is larger than the current value from the initial states
                monIncr &= diff < -precision;
            }
            previous = initial;
            samples.push_back(std::move(values));
        }
        auto res = (!monIncr && !monDecr) ? MonotonicityResult<VariableType>::Monotonicity::Not : MonotonicityResult<VariableType>::Monotonicity::Unknown;
        resultCheckOnSamples.addMonotonicityResult(*itr, res);
    }
    assumptionMaker.setSampleValues(std::move(samples));
}

template<typename ValueType, typename ConstantType>
void MonotonicityHelper<ValueType, ConstantType>::checkMonotonicityOnSamples(std::shared_ptr<models::sparse::Mdp<ValueType>> model,
                                                                             uint_fast64_t numberOfSamples) {
    assert(numberOfSamples > 2);

    auto instantiator = utility::ModelInstantiator<models::sparse::Mdp<ValueType>, models::sparse::Mdp<ConstantType>>(*model);
    std::set<VariableType> variables = models::sparse::getProbabilityParameters(*model);
    std::vector<std::vector<ConstantType>> samples;
    // For each of the variables create a model in which we only change the value for this specific variable
    for (auto itr = variables.begin(); itr != variables.end(); ++itr) {
        ConstantType previous = -1;
        bool monDecr = true;
        bool monIncr = true;

        // Check monotonicity in variable (*itr) by instantiating the model
        // all other variables fixed on lb, only increasing (*itr)
        for (uint_fast64_t i = 0; (monDecr || monIncr) && i < numberOfSamples; ++i) {
            // Create valuation
            auto valuation = utility::parametric::Valuation<ValueType>();
            for (auto itr2 = variables.begin(); itr2 != variables.end(); ++itr2) {
                // Only change value for current variable
                if ((*itr) == (*itr2)) {
                    auto lb = region.getLowerBoundary(itr->name());
                    auto ub = region.getUpperBoundary(itr->name());
                    // Creates samples between lb and ub, that is: lb, lb + (ub-lb)/(#samples -1), lb + 2* (ub-lb)/(#samples -1), ..., ub
                    valuation[*itr2] =
                        (lb + utility::convertNumber<CoefficientType>(i) / (utility::convertNumber<CoefficientType>(numberOfSamples) - 1) * (ub - lb));
                } else {
                    auto lb = region.getLowerBoundary(itr2->name());
                    valuation[*itr2] = utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(lb);
                }
            }

            // Instantiate model and get result
            models::sparse::Mdp<ConstantType> sampleModel = instantiator.instantiate(valuation);
            auto checker = modelchecker::SparseMdpPrctlModelChecker<models::sparse::Mdp<ConstantType>>(sampleModel);
            std::unique_ptr<modelchecker::CheckResult> checkResult;
            auto formula = formulas[0];
            if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask =
                    modelchecker::CheckTask<logic::UntilFormula, ConstantType>((*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                checkTask.setOptimizationDirection(formula->asProbabilityOperatorFormula().getOptimalityType());
                checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
            } else if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>(
                    (*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
                checkTask.setOptimizationDirection(formula->asProbabilityOperatorFormula().getOptimalityType());
                checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
            } else if (formula->isRewardOperatorFormula() && formula->asRewardOperatorFormula().getSubformula().isUntilFormula()) {
                modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask =
                    modelchecker::CheckTask<logic::UntilFormula, ConstantType>((*formula).asRewardOperatorFormula().getSubformula().asUntilFormula());
                checkTask.setOptimizationDirection(formula->asRewardOperatorFormula().getOptimalityType());
                checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
            } else if (formula->isRewardOperatorFormula() && formula->asRewardOperatorFormula().getSubformula().isEventuallyFormula()) {
                modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask =
                    modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>((*formula).asRewardOperatorFormula().getSubformula().asEventuallyFormula());
                checkTask.setOptimizationDirection(formula->asRewardOperatorFormula().getOptimalityType());
                checkResult = checker.computeReachabilityProbabilities(Environment(), checkTask);
            } else {
                STORM_LOG_THROW(false, exceptions::NotSupportedException, "Expecting until or eventually formula");
            }

            auto quantitativeResult = checkResult->asExplicitQuantitativeCheckResult<ConstantType>();
            std::vector<ConstantType> values = quantitativeResult.getValueVector();
            auto initialStates = model->getInitialStates();
            ConstantType initial = 0;
            // Get total probability from initial states
            for (auto j = initialStates.getNextSetIndex(0); j < model->getNumberOfStates(); j = initialStates.getNextSetIndex(j + 1)) {
                initial += values[j];
            }
            // Calculate difference with result for previous valuation
            assert(initial >= 0 - precision && initial <= 1 + precision);
            ConstantType diff = previous - initial;
            assert(previous == -1 || (diff >= -1 - precision && diff <= 1 + precision));

            if (previous != -1 && (diff > precision || diff < -precision)) {
                monDecr &= diff > precision;  // then previous value is larger than the current value from the initial states
                monIncr &= diff < -precision;
            }
            previous = initial;
            samples.push_back(std::move(values));
        }
        auto res = (!monIncr && !monDecr) ? MonotonicityResult<VariableType>::Monotonicity::Not : MonotonicityResult<VariableType>::Monotonicity::Unknown;
        resultCheckOnSamples.addMonotonicityResult(*itr, res);
    }
    assumptionMaker.setSampleValues(std::move(samples));
}

template class MonotonicityHelper<RationalFunction, double>;
template class MonotonicityHelper<RationalFunction, RationalNumber>;
}  // namespace analysis
}  // namespace storm
