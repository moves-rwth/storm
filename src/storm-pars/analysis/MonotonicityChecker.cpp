#include "MonotonicityChecker.h"
#include "storm/api/verification.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidOperationException.h"

#include "storm/models/ModelType.h"

#include "storm/modelchecker/results/CheckResult.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm-pars/analysis/AssumptionChecker.h"


namespace storm {
    namespace analysis {
        template <typename ValueType, typename ConstantType>
        MonotonicityChecker<ValueType, ConstantType>::MonotonicityChecker(std::shared_ptr<models::ModelBase> model, std::vector<std::shared_ptr<logic::Formula const>> formulas,  std::vector<storage::ParameterRegion<ValueType>> regions, uint_fast64_t numberOfSamples, double const& precision, bool dotOutput) {
            assert (model != nullptr);
            STORM_LOG_THROW(regions.size() <= 1, exceptions::NotSupportedException, "Monotonicity checking is not (yet) supported for multiple regions");
            STORM_LOG_THROW(formulas.size() <= 1, exceptions::NotSupportedException, "Monotonicity checking is not (yet) supported for multiple formulas");

            this->model = model;
            this->formulas = formulas;
            this->precision = utility::convertNumber<ConstantType>(precision);
            std::shared_ptr<models::sparse::Model<ValueType>> sparseModel = model->as<models::sparse::Model<ValueType>>();
            this->matrix = sparseModel->getTransitionMatrix();
            this->dotOutput = dotOutput;

            if (regions.size() == 1) {
                this->region = *(regions.begin());
            } else {
                typename storage::ParameterRegion<ValueType>::Valuation lowerBoundaries;
                typename storage::ParameterRegion<ValueType>::Valuation upperBoundaries;
                std::set<VariableType> vars;
                vars = models::sparse::getProbabilityParameters(*sparseModel);
                for (auto var : vars) {
                    typename storage::ParameterRegion<ValueType>::CoefficientType lb = utility::convertNumber<CoefficientType>(0 + precision) ;
                    typename storage::ParameterRegion<ValueType>::CoefficientType ub = utility::convertNumber<CoefficientType>(1 - precision) ;
                    lowerBoundaries.insert(std::make_pair(var, lb));
                    upperBoundaries.insert(std::make_pair(var, ub));
                }
                this->region =  storage::ParameterRegion<ValueType>(std::move(lowerBoundaries), std::move(upperBoundaries));
            }

            if (numberOfSamples > 2) {
                // sampling
                if (model->isOfType(models::ModelType::Dtmc)) {
                    this->resultCheckOnSamples = std::map<VariableType, std::pair<bool, bool>>(
                            checkMonotonicityOnSamples(model->as<models::sparse::Dtmc<ValueType>>(), numberOfSamples));
                } else if (model->isOfType(models::ModelType::Mdp)) {
                    this->resultCheckOnSamples = std::map<VariableType, std::pair<bool, bool>>(
                            checkMonotonicityOnSamples(model->as<models::sparse::Mdp<ValueType>>(), numberOfSamples));
                }
                checkSamples= true;
            } else {
                if (numberOfSamples > 0) {
                    STORM_LOG_WARN("At least 3 sample points are needed to check for monotonicity on samples, not using samples for now");
                }
                checkSamples= false;
            }

            this->extender = new analysis::OrderExtender<ValueType, ConstantType>(sparseModel, formulas[0], region);
        }


        template <typename ValueType, typename ConstantType>
        std::map<analysis::Order*, std::pair<std::shared_ptr<MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>> MonotonicityChecker<ValueType, ConstantType>::checkMonotonicityInBuild(std::ostream& outfile, std::string dotOutfileName){
            createOrder();

            //output of results
            for(auto itr : monResults){
                std::string temp = itr.second.first->toString();
                outfile << temp << std::endl;
            }

            //dotoutput
            if(dotOutput){
                STORM_LOG_WARN_COND(monResults.size() <= 10, "Too many Reachability Orders. Dot Output will only be created for 10.");
                int i = 0;
                auto orderItr = monResults.begin();
                while(i < 10 && orderItr != monResults.end()){
                    std::ofstream dotOutfile;
                    std::string name = dotOutfileName + std::to_string(i);
                    utility::openFile(name, dotOutfile);
                    dotOutfile << "Assumptions:" << std::endl;
                    auto assumptionItr = orderItr->second.second.begin();
                    while(assumptionItr != orderItr->second.second.end()){
                        dotOutfile << *assumptionItr << std::endl;
                        dotOutfile << std::endl;
                        assumptionItr++;
                    }
                    dotOutfile << std::endl;
                    orderItr->first->dotOutputToFile(dotOutfile);
                    utility::closeFile(dotOutfile);
                    i++;
                    orderItr++;
                }
            }
            return monResults;

        }


        template <typename ValueType, typename ConstantType>
        MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType> MonotonicityChecker<ValueType, ConstantType>::checkMonotonicity(std::vector<ConstantType> minValues, std::vector<ConstantType> maxValues) {
            auto order = createOrder(minValues, maxValues);
            return analyseMonotonicity(order);
        }

        template <typename ValueType, typename ConstantType>
        std::map<analysis::Order*, std::map<typename MonotonicityChecker<ValueType, ConstantType>::VariableType, std::pair<bool, bool>>> MonotonicityChecker<ValueType, ConstantType>::checkMonotonicity(std::ostream& outfile, std::map<analysis::Order*, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>> map) {
            std::map<analysis::Order *, std::map<VariableType, std::pair<bool, bool>>> result;

            if (map.size() == 0) {
                // Nothing is known
                outfile << " No assumptions -";
                STORM_PRINT("No valid assumptions, couldn't build a sufficient order");
                if (resultCheckOnSamples.size() != 0) {
                    STORM_PRINT("\n" << "Based results on samples");
                } else {
                    outfile << " ?";
                }

                for (auto entry : resultCheckOnSamples) {
                    if (!entry.second.first && ! entry.second.second) {
                        outfile << " SX " << entry.first << " ";
                    } else if (entry.second.first && ! entry.second.second) {
                        outfile << " SI " << entry.first << " ";
                    } else if (entry.second.first && entry.second.second) {
                        outfile << " SC " << entry.first << " ";
                    } else {
                        outfile << " SD " << entry.first << " ";
                    }
                }

            } else {
                size_t i = 0;
                // For each order
                for (auto itr = map.begin(); i < map.size() && itr != map.end(); ++itr) {
                    auto order = itr->first;

                    auto addedStates = order->getAddedStates()->getNumberOfSetBits();
                    assert (addedStates == order->getAddedStates()->size());
                    // Get monotonicity result for this order
                    std::map<VariableType, std::pair<bool, bool>> varsMonotone = analyseMonotonicity(i, order);

                    auto assumptions = itr->second;
                    if (assumptions.size() > 0) {
                        bool first = true;
                        for (auto itr2 = assumptions.begin(); itr2 != assumptions.end(); ++itr2) {
                            if (!first) {
                                outfile << (" ^ ");
                            } else {
                                first = false;
                            }
                            outfile << (*(*itr2));
                        }
                        outfile << " - ";
                    } else if (assumptions.size() == 0) {
                        outfile << "No assumptions - ";
                    }

                    if (varsMonotone.size() == 0) {
                        outfile << "No params";
                    } else {
                        auto itr2 = varsMonotone.begin();
                        while (itr2 != varsMonotone.end()) {
                            if (checkSamples &&
                                resultCheckOnSamples.find(itr2->first) != resultCheckOnSamples.end() &&
                                (!resultCheckOnSamples[itr2->first].first &&
                                 !resultCheckOnSamples[itr2->first].second)) {
                                // Not monotone
                                outfile << "X " << itr2->first;
                            } else {
                                // itr2->first == parameter
                                // itr2->second == boolean pair
                                if (itr2->second.first && itr2->second.second) {
                                    // Constant
                                    outfile << "C " << itr2->first;
                                } else if (itr2->second.first) {
                                    // Increasing
                                    outfile << "I " << itr2->first;
                                } else if (itr2->second.second) {
                                    // Decreasing
                                    outfile << "D " << itr2->first;
                                } else {
                                    // Don't know
                                    outfile << "? " << itr2->first;
                                }
                            }
                            ++itr2;
                            if (itr2 != varsMonotone.end()) {
                                outfile << " ";
                            }
                        }
                        result.insert(
                                std::pair<analysis::Order *, std::map<VariableType, std::pair<bool, bool>>>(
                                        order, varsMonotone));
                    }
                    ++i;
                    outfile << ";";
                }
            }
            outfile << ", ";

            return result;
        }

        template <typename ValueType, typename ConstantType>
        void MonotonicityChecker<ValueType, ConstantType>::createOrder() {
            // Transform to Orders
            std::tuple<analysis::Order *, uint_fast64_t, uint_fast64_t> criticalTuple;

            // Create initial order
            auto monRes = std::make_shared<MonotonicityResult<VariableType>>(MonotonicityResult<VariableType>());
            criticalTuple = extender->toOrder(monRes);
            // Continue based on not (yet) sorted states
            std::map<analysis::Order*, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>> result;

            auto val1 = std::get<1>(criticalTuple);
            auto val2 = std::get<2>(criticalTuple);
            auto numberOfStates = model->getNumberOfStates();
            std::vector<std::shared_ptr<expressions::BinaryRelationExpression>> assumptions;

            if (val1 == numberOfStates && val2 == numberOfStates) {
                auto resAssumptionPair = std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>(monRes, assumptions);
                monResults.insert(std::pair<Order*, std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>>(std::get<0>(criticalTuple), resAssumptionPair));
            } else if (val1 != numberOfStates && val2 != numberOfStates) {
                analysis::AssumptionChecker<ValueType, ConstantType> *assumptionChecker;
                if (model->isOfType(models::ModelType::Dtmc)) {
                    auto dtmc = model->as<models::sparse::Dtmc<ValueType>>();
                    assumptionChecker = new analysis::AssumptionChecker<ValueType, ConstantType>(formulas[0], dtmc, region, 3);
                } else if (model->isOfType(models::ModelType::Mdp)) {
                    auto mdp = model->as<models::sparse::Mdp<ValueType>>();
                    assumptionChecker = new analysis::AssumptionChecker<ValueType, ConstantType>(formulas[0], mdp, 3);
                } else {
                    STORM_LOG_THROW(false, exceptions::InvalidOperationException,"Unable to perform monotonicity analysis on the provided model type.");
                }
                auto assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(assumptionChecker, numberOfStates);
                extendOrderWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, val1, val2, assumptions, monRes);
            } else {
                assert (false);
            }
        }

        template <typename ValueType, typename ConstantType>
        Order * MonotonicityChecker<ValueType, ConstantType>::createOrder(std::vector<ConstantType> minValues, std::vector<ConstantType> maxValues) {
            auto monRes = std::make_shared<MonotonicityResult<VariableType>>(MonotonicityResult<VariableType>());
            return extender->toOrder(minValues, maxValues, monRes);
        }

        template <typename ValueType, typename ConstantType>
        void MonotonicityChecker<ValueType, ConstantType>::extendOrderWithAssumptions(analysis::Order* order, analysis::AssumptionMaker<ValueType, ConstantType>* assumptionMaker, uint_fast64_t val1, uint_fast64_t val2, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>> assumptions, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
            std::map<analysis::Order*, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>> result;

            auto numberOfStates = model->getNumberOfStates();
            if (val1 == numberOfStates || val2 == numberOfStates) {
                assert (val1 == val2);
                assert (order->getAddedStates()->size() == order->getAddedStates()->getNumberOfSetBits());
                auto resAssumptionPair = std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>(monRes, assumptions);
                monResults.insert(std::pair<Order*, std::pair<std::shared_ptr<MonotonicityResult<VariableType>>, std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>>>(std::move(order), std::move(resAssumptionPair)));
            } else {
                // Make the three assumptions
                auto newAssumptions = assumptionMaker->createAndCheckAssumptions(val1, val2, order);
                assert (newAssumptions.size() == 3);
                auto itr = newAssumptions.begin();

                while (itr != newAssumptions.end()) {
                    auto assumption = *itr;
                    ++itr;
                    if (assumption.second != AssumptionStatus::INVALID) {
                        if (itr != newAssumptions.end()) {
                            // We make a copy of the order and the assumptions
                            auto orderCopy = new Order(order);
                            auto assumptionsCopy = std::vector<std::shared_ptr<expressions::BinaryRelationExpression>>(assumptions);
                            auto monResCopy = monRes->copy();


                            if (assumption.second == AssumptionStatus::UNKNOWN) {
                                // only add assumption to the set of assumptions if it is unknown whether it holds or not
                                assumptionsCopy.push_back(std::move(assumption.first));
                            }

                            auto criticalTuple = extender->extendOrder(orderCopy, std::make_shared<MonotonicityResult<VariableType>>(*monResCopy), assumption.first);
                            if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                                extendOrderWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptionsCopy, std::make_shared<MonotonicityResult<VariableType>>(*monResCopy));
                            }
                        } else {
                            // It is the last one, so we don't need to create a copy.

                            if (assumption.second == AssumptionStatus::UNKNOWN) {
                                // only add assumption to the set of assumptions if it is unknown whether it holds or not
                                assumptions.push_back(std::move(assumption.first));
                            }

                            auto criticalTuple = extender->extendOrder(order, std::make_shared<MonotonicityResult<VariableType>>(*monRes), assumption.first);
                            if (somewhereMonotonicity(std::get<0>(criticalTuple))) {
                                extendOrderWithAssumptions(std::get<0>(criticalTuple), assumptionMaker, std::get<1>(criticalTuple), std::get<2>(criticalTuple), assumptions, std::make_shared<MonotonicityResult<VariableType>>(*monRes));
                            }
                        }
                    }
                }
            }
        }

        template <typename ValueType, typename ConstantType>
        ValueType MonotonicityChecker<ValueType, ConstantType>::getDerivative(ValueType function, typename MonotonicityChecker<ValueType, ConstantType>::VariableType var) {
            if (function.isConstant()) {
                return utility::zero<ValueType>();
            }
            if ((derivatives[function]).find(var) == (derivatives[function]).end()) {
                (derivatives[function])[var] = function.derivative(var);
            }
            return (derivatives[function])[var];
        }

        template <typename ValueType, typename ConstantType>
        std::map<typename MonotonicityChecker<ValueType, ConstantType>::VariableType, std::pair<bool, bool>> MonotonicityChecker<ValueType, ConstantType>::analyseMonotonicity(uint_fast64_t j, analysis::Order* order) {
            std::map<VariableType, std::pair<bool, bool>> varsMonotone;

            // go over all rows, check for each row local monotonicity
            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                auto row = matrix.getRow(i);
                // only enter if you are in a state with at least two successors (so there must be successors,
                // and first prob shouldn't be 1)
                if (row.begin() != row.end() && !row.begin()->getValue().isOne()) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    // Gather all states which are reached with a non constant probability
                    std::vector<uint_fast64_t> states;
                    std::set<VariableType> vars;
                    for (auto const& entry : row) {
                        if (!entry.getValue().isConstant()) {
                            transitions.insert(std::pair<uint_fast64_t, ValueType>(entry.getColumn(), entry.getValue()));
                            auto varsRow = entry.getValue().gatherVariables();
                            vars.insert(varsRow.begin(), varsRow.end());
                            // Used to set the states we need to sort
                            states.push_back(entry.getColumn());
                        }
                    }

                    // Copy info from checkMonotonicityOnSamples
                    if (checkSamples) {
                        for (auto var : vars) {
                            assert (resultCheckOnSamples.find(var) != resultCheckOnSamples.end());
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = resultCheckOnSamples[var].first;
                                varsMonotone[var].second = resultCheckOnSamples[var].second;
                            } else {
                                varsMonotone[var].first &= resultCheckOnSamples[var].first;
                                varsMonotone[var].second &= resultCheckOnSamples[var].second;
                            }
                        }
                    } else {
                        for (auto var : vars) {
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = true;
                                varsMonotone[var].second = true;
                            }
                        }
                    }

                    // Sort the states based on the order
                    auto sortedStates = order->sortStates(&states);
                    if (sortedStates[sortedStates.size() - 1] == matrix.getColumnCount()) {
                    // If the states are not all sorted, we still might obtain some monotonicity
                        for (auto var: vars) {
                            // current value of monotonicity
                            std::pair<bool, bool> *value = &varsMonotone.find(var)->second;

                            // Go over all transitions to successor states, compare all of them
                            for (auto itr2 = transitions.begin(); (value->first || value->second)
                                    && itr2 != transitions.end(); ++itr2) {
                                for (auto itr3 = transitions.begin(); (value->first || value->second)
                                        && itr3 != transitions.end(); ++itr3) {
                                    if (itr2->first < itr3->first) {
                                        auto derivative2 = getDerivative(itr2->second, var);
                                        auto derivative3 = getDerivative(itr3->second, var);
                                        auto compare = order->compare(itr2->first, itr3->first);

                                        if (compare == Order::ABOVE) {
                                            // As the first state (itr2) is above the second state (itr3) it
                                            // is sufficient to look at the derivative of itr2.
                                            std::pair<bool, bool> mon2 = checkDerivative(derivative2, region);
                                            value->first &= mon2.first;
                                            value->second &= mon2.second;
                                        } else if (compare == Order::BELOW) {
                                            // As the second state (itr3) is above the first state (itr2) it
                                            // is sufficient to look at the derivative of itr3.
                                            std::pair<bool, bool> mon3 = checkDerivative(derivative3, region);
                                            value->first &= mon3.first;
                                            value->second &= mon3.second;
                                        } else if (compare == Order::SAME) {
                                            // Behaviour doesn't matter, as the states are at the same level.
                                        } else {
                                            // only if derivatives are the same we can continue
                                            if (derivative2 != derivative3) {
                                                // As the relation between the states is unknown, we can't claim
                                                // anything about the monotonicity.
                                                value->first = false;
                                                value->second = false;
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        // The states are all sorted
                        for (auto var : vars) {
                            std::pair<bool, bool> *value = &varsMonotone.find(var)->second;
                            bool change = false;
                            for (auto const &i : sortedStates) {
                                auto res = checkDerivative(getDerivative(transitions[i], var), region);
                                change = change || (!(value->first && value->second) // they do not hold both
                                                    && ((value->first && !res.first)
                                                        || (value->second && !res.second)));

                                if (change) {
                                    value->first &= res.second;
                                    value->second &= res.first;
                                } else {
                                    value->first &= res.first;
                                    value->second &= res.second;
                                }
                                if (!value->first && !value->second) {
                                    break;
                                }
                            }

                        }
                    }
                }
            }
            return varsMonotone;
        }

        template <typename ValueType, typename ConstantType>
        MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType> MonotonicityChecker<ValueType, ConstantType>::analyseMonotonicity(analysis::Order *order) {
            // TODO: @Svenja, when your implementation is working, you could also use this here
            std::shared_ptr<MonotonicityResult<VariableType>> varsMonotone = std::make_shared<MonotonicityResult<VariableType>>(MonotonicityResult<VariableType>());
            if (order->getAddedStates()->getNumberOfSetBits() == matrix.getColumnCount()) {
                varsMonotone->setDone();
            }

            // go over all rows, check for each row local monotonicity
            std::set<VariableType> vars;

            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                auto row = matrix.getRow(i);
                // only enter if you are in a state with at least two successors (so there must be successors,
                // and first prob shouldn't be 1)
                if (row.begin() != row.end() && !row.begin()->getValue().isOne()) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    // Gather all states which are reached with a non constant probability
                    std::vector<uint_fast64_t> states;
                    // = models::sparse::getProbabilityParameters(*model);
                    for (auto const& entry : row) {
                        if (!entry.getValue().isConstant()) {
                            // only analyse take non constant transitions
                            transitions.insert(std::pair<uint_fast64_t, ValueType>(entry.getColumn(), entry.getValue()));
                            auto varsRow = entry.getValue().gatherVariables();
                            vars.insert(varsRow.begin(), varsRow.end());
                            // Used to set the states we need to sort
                            states.push_back(entry.getColumn());
                        }

                    }

                    // Sort the states based on the order
                    auto sortedStates = order->sortStates(&states);
                    // if the last entry in sorted states equals the number of states in the model, we know that not all states are sorted
                    if (sortedStates.size() == 0 || sortedStates[sortedStates.size() - 1] == matrix.getColumnCount()) {
                        // If the states are not all sorted, we still might obtain some monotonicity
                        for (auto var: vars) {
                            // current value of monotonicity is constant
                            auto monRes = varsMonotone->getMonotonicityResult();
                            typename MonotonicityResult<VariableType>::Monotonicity value;

                            auto itr = monRes.find(var);
                            if (itr != monRes.end()) {
                                value = itr->second;
                            } else {
                                value = MonotonicityResult<VariableType>::Monotonicity::Constant;
                            }

                            if ((value == MonotonicityResult<VariableType>::Monotonicity::Not ||
                                 value == MonotonicityResult<VariableType>::Monotonicity::Unknown)) {
                                continue;
                            }

                            // Go over all transitions to successor states, compare all of them
                            for (auto itr2 = transitions.begin(); (value != MonotonicityResult<VariableType>::Monotonicity::Not &&
                                                                    value != MonotonicityResult<VariableType>::Monotonicity::Unknown)
                                                                  && itr2 != transitions.end(); ++itr2) {
                                for (auto itr3 = transitions.begin(); (value != MonotonicityResult<VariableType>::Monotonicity::Not &&
                                                                       value != MonotonicityResult<VariableType>::Monotonicity::Unknown)
                                                                      && itr3 != transitions.end(); ++itr3) {
                                    if (itr2->first < itr3->first) {
                                        auto derivative2 = getDerivative(itr2->second, var);
                                        auto derivative3 = getDerivative(itr3->second, var);

                                        auto compare = order->compare(itr2->first, itr3->first);

                                        std::pair<bool, bool> mon = std::make_pair(false, false);

                                        if (compare == Order::ABOVE) {
                                            // As the first state (itr2) is above the second state (itr3) it
                                            // is sufficient to look at the derivative of itr2.
                                            mon = checkDerivative(derivative2, region);
                                        } else if (compare == Order::BELOW) {
                                            // As the second state (itr3) is above the first state (itr2) it
                                            // is sufficient to look at the derivative of itr3.
                                            mon = checkDerivative(derivative3, region);
                                        }

                                        if(mon.first && !mon.second){
                                            value = MonotonicityResult<VariableType>::Monotonicity::Incr;
                                        }
                                        else if(!mon.first && mon.second){
                                            value = MonotonicityResult<VariableType>::Monotonicity::Decr;
                                        }
                                        else if(mon.first && mon.second){
                                            value = MonotonicityResult<VariableType>::Monotonicity::Constant;
                                        }
                                        else{
                                            value = MonotonicityResult<VariableType>::Monotonicity::Unknown;
                                        }
                                        varsMonotone->updateMonotonicityResult(var, value);
                                    }
                                }
                            }
                        }
                    } else {
                        // The states are all sorted
                        for (auto var : vars) {
                            checkParOnStateMonRes(i, sortedStates, var, varsMonotone);
                        }
                    }
                }
            }
            varsMonotone->setAllMonotonicity();
            for (auto var : vars) {
                auto varRes = varsMonotone->getMonotonicityResult()[var];
                if ( varRes != MonotonicityResult<VariableType>::Monotonicity::Unknown
                    && varRes != MonotonicityResult<VariableType>::Monotonicity::Not) {
                    varsMonotone->setSomewhereMonotonicity();
                }
                if ( varRes == MonotonicityResult<VariableType>::Monotonicity::Unknown
                     || varRes == MonotonicityResult<VariableType>::Monotonicity::Not) {
                    varsMonotone->setAllMonotonicity(false);
                }
                if (varsMonotone->isSomewhereMonotonicity() && !varsMonotone->isAllMonotonicity()) {
                    break;
                }
            }
            return *varsMonotone;
        }

        template <typename ValueType, typename ConstantType>
        bool MonotonicityChecker<ValueType, ConstantType>::somewhereMonotonicity(Order* order) {
            // TODO: @Svenja, when your implementation is working, you could also use this here?
            // TODO: @Svenja, right now this method is calles sometimes to see if we can stop already (if there is no monotonicity). Does it make sense to update the monotonicity while creating the order? so if all successors of a state are added, we check for local monotonicity for this state, and update the general monotonicity list with this.
            std::map<VariableType, std::pair<bool, bool>> varsMonotone;

            for (uint_fast64_t i = 0; i < matrix.getColumnCount(); ++i) {
                // go over all rows
                auto row = matrix.getRow(i);
                auto first = (*row.begin());
                if (first.getValue() != ValueType(1)) {
                    std::map<uint_fast64_t, ValueType> transitions;

                    for (auto itr = row.begin(); itr != row.end(); ++itr) {
                        transitions.insert(std::pair<uint_fast64_t, ValueType>((*itr).getColumn(), (*itr).getValue()));
                    }

                    auto val = first.getValue();
                    auto vars = val.gatherVariables();
                    // Copy info from checkMonotonicityOnSamples
                    if (checkSamples) {
                        for (auto var : vars) {
                            assert (resultCheckOnSamples.find(var) != resultCheckOnSamples.end());
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = resultCheckOnSamples[var].first;
                                varsMonotone[var].second = resultCheckOnSamples[var].second;
                            } else {
                                varsMonotone[var].first &= resultCheckOnSamples[var].first;
                                varsMonotone[var].second &= resultCheckOnSamples[var].second;
                            }
                        }
                    } else {
                        for (auto var : vars) {
                            if (varsMonotone.find(var) == varsMonotone.end()) {
                                varsMonotone[var].first = true;
                                varsMonotone[var].second = true;
                            }
                        }
                    }

                    for (auto var: vars) {
                        // current value of monotonicity
                        std::pair<bool, bool> *value = &varsMonotone.find(var)->second;

                        // Go over all transitions to successor states, compare all of them
                        for (auto itr2 = transitions.begin(); (value->first || value->second)
                                                              && itr2 != transitions.end(); ++itr2) {
                            for (auto itr3 = transitions.begin(); (value->first || value->second)
                                                                  && itr3 != transitions.end(); ++itr3) {
                                if (itr2->first < itr3->first) {

                                    auto derivative2 = getDerivative(itr2->second, var);
                                    auto derivative3 = getDerivative(itr3->second, var);

                                    auto compare = order->compare(itr2->first, itr3->first);

                                    if (compare == Order::ABOVE) {
                                        // As the first state (itr2) is above the second state (itr3) it
                                        // is sufficient to look at the derivative of itr2.
                                        std::pair<bool, bool> mon2;
                                        mon2 = checkDerivative(derivative2, region);
                                        value->first &= mon2.first;
                                        value->second &= mon2.second;
                                    } else if (compare == Order::BELOW) {
                                        // As the second state (itr3) is above the first state (itr2) it
                                        // is sufficient to look at the derivative of itr3.
                                        std::pair<bool, bool> mon3;

                                        mon3 = checkDerivative(derivative3, region);
                                        value->first &= mon3.first;
                                        value->second &= mon3.second;
                                    } else if (compare == Order::SAME) {
                                        // Behaviour doesn't matter, as the states are at the same level.
                                    } else {
                                        // As the relation between the states is unknown, we don't do anything
                                    }
                                }
                            }
                        }
                    }
                }
            }

            bool result = false;

            for (auto itr = varsMonotone.begin(); !result && itr != varsMonotone.end(); ++itr) {
                result = itr->second.first || itr->second.second;
            }
            return result;
        }

        template <typename ValueType, typename ConstantType>
        std::map<typename MonotonicityChecker<ValueType, ConstantType>::VariableType, std::pair<bool, bool>> MonotonicityChecker<ValueType, ConstantType>::checkMonotonicityOnSamples(std::shared_ptr<models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples) {
            assert (numberOfSamples > 2);
            std::map<VariableType, std::pair<bool, bool>> result;

            auto instantiator = utility::ModelInstantiator<models::sparse::Dtmc<ValueType>, models::sparse::Dtmc<ConstantType>>(*model);
            std::set<VariableType> variables =  models::sparse::getProbabilityParameters(*model);

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
                            valuation[*itr2] = utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(lb + i*(ub-lb)/(numberOfSamples-1));
                        } else {
                            auto lb = region.getLowerBoundary(itr->name());
                            valuation[*itr2] = utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(lb);
                        }
                    }

                    // Instantiate model and get result
                    models::sparse::Dtmc<ConstantType> sampleModel = instantiator.instantiate(valuation);
                    auto checker = modelchecker::SparseDtmcPrctlModelChecker<models::sparse::Dtmc<ConstantType>>(sampleModel);
                    std::unique_ptr<modelchecker::CheckResult> checkResult;
                    auto formula = formulas[0];
                    if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                        const modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::UntilFormula, ConstantType>((*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                        checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
                    } else if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                        const modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>((*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
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
                    assert (initial >= 0 - precision && initial <= 1 + precision);
                    ConstantType diff = previous - initial;
                    assert (previous == -1 || diff >= -1 - precision && diff <= 1 + precision);

                    if (previous != -1 && (diff > precision || diff < -precision)) {
                        monDecr &= diff > precision; // then previous value is larger than the current value from the initial states
                        monIncr &= diff < -precision;
                    }
                    previous = initial;
                }
                result.insert(std::pair<VariableType,  std::pair<bool, bool>>(*itr, std::pair<bool,bool>(monIncr, monDecr)));
            }
            resultCheckOnSamples = result;
            return result;
        }

        template <typename ValueType, typename ConstantType>
        std::map<typename MonotonicityChecker<ValueType, ConstantType>::VariableType, std::pair<bool, bool>> MonotonicityChecker<ValueType, ConstantType>::checkMonotonicityOnSamples(std::shared_ptr<models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples) {
            assert(numberOfSamples > 2);
            std::map<VariableType, std::pair<bool, bool>> result;

            auto instantiator = utility::ModelInstantiator<models::sparse::Mdp<ValueType>, models::sparse::Mdp<ConstantType>>(*model);
            std::set<VariableType> variables =  models::sparse::getProbabilityParameters(*model);

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
                            valuation[*itr2] = utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(lb + i*(ub-lb)/(numberOfSamples-1));
                        } else {
                            auto lb = region.getLowerBoundary(itr->name());
                            valuation[*itr2] = utility::convertNumber<typename utility::parametric::CoefficientType<ValueType>::type>(lb);
                        }
                    }

                    // Instantiate model and get result
                    models::sparse::Mdp<ConstantType> sampleModel = instantiator.instantiate(valuation);
                    auto checker = modelchecker::SparseMdpPrctlModelChecker<models::sparse::Mdp<ConstantType>>(sampleModel);
                    std::unique_ptr<modelchecker::CheckResult> checkResult;
                    auto formula = formulas[0];
                    if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                        const modelchecker::CheckTask<logic::UntilFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::UntilFormula, ConstantType>((*formula).asProbabilityOperatorFormula().getSubformula().asUntilFormula());
                        checkResult = checker.computeUntilProbabilities(Environment(), checkTask);
                    } else if (formula->isProbabilityOperatorFormula() && formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()) {
                        const modelchecker::CheckTask<logic::EventuallyFormula, ConstantType> checkTask = modelchecker::CheckTask<logic::EventuallyFormula, ConstantType>((*formula).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula());
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
                    assert (initial >= 0 - precision && initial <= 1 + precision);
                    ConstantType diff = previous - initial;
                    assert (previous == -1 || diff >= -1 - precision && diff <= 1 + precision);

                    if (previous != -1 && (diff > precision || diff < -precision)) {
                        monDecr &= diff > precision; // then previous value is larger than the current value from the initial states
                        monIncr &= diff < -precision;
                    }
                    previous = initial;
                }
                result.insert(std::pair<VariableType,  std::pair<bool, bool>>(*itr, std::pair<bool,bool>(monIncr, monDecr)));
            }
            resultCheckOnSamples = result;
            return result;
        }

        // Checks if a transition (from -> to) is monotonic increasing / decreasing in param on region
        template <typename ValueType, typename ConstantType>
        typename MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType>::Monotonicity MonotonicityChecker<ValueType, ConstantType>::checkTransitionMonRes(uint_fast64_t from, uint_fast64_t to, typename MonotonicityChecker<ValueType, ConstantType>::VariableType param){
            if (to < matrix.getColumnCount()) {
                ValueType function = getMatrixEntry(from, to);
                function = getDerivative(function, param);
                std::pair<bool, bool> res = checkDerivative(function, region);
                if(res.first && !res.second){
                    return MonotonicityResult<VariableType>::Monotonicity::Incr;
                }
                else if(!res.first && res.second){
                    return MonotonicityResult<VariableType>::Monotonicity::Decr;
                }
                else if(res.first && res.second){
                    return MonotonicityResult<VariableType>::Monotonicity::Constant;
                }
                else{
                    return MonotonicityResult<VariableType>::Monotonicity::Not;
                }

            } /*else {
                auto row = matrix.getRow(from);
                for (auto column : row) {
                    auto vars = column.getValue().gatherVariables();
                    if (std::find(vars.begin(), vars.end(), param) != vars.end()) {
                        return MonotonicityResult<VariableType>::Monotonicity::Unknown;
                    }
                }
                return MonotonicityResult<VariableType>::Monotonicity::Constant;

            }*/
        }


        // Checks the local monotonicity in param at state s (succ has to be sorted already) and adjusts Monotonicity Array accordingly
        template <typename ValueType, typename ConstantType>
        void MonotonicityChecker<ValueType, ConstantType>::checkParOnStateMonRes(uint_fast64_t s, const std::vector<uint_fast64_t>& succ, typename MonotonicityChecker<ValueType, ConstantType>::VariableType param, std::shared_ptr<MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType>> monResult){
            uint_fast64_t succSize = succ.size();

            // Create + fill Vector containing the Monotonicity of the transitions to the succs
            std::vector<typename MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType>::Monotonicity> succsMon;
            for (auto &&itr:succ) {
                auto temp = checkTransitionMonRes(s, itr, param);
                succsMon.push_back(temp);
            }

            uint_fast64_t index = 0;
            typename MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType>::Monotonicity monCandidate = MonotonicityResult<VariableType>::Monotonicity::Constant;
            typename MonotonicityResult<typename MonotonicityChecker<ValueType, ConstantType>::VariableType>::Monotonicity temp;

            //go to first inc / dec
            while(index < succSize && monCandidate == MonotonicityResult<VariableType>::Monotonicity::Constant){
                temp = succsMon[index];
                if(temp != MonotonicityResult<VariableType>::Monotonicity::Not){
                    monCandidate = temp;
                }
                else{
                    monResult->updateMonotonicityResult(param, MonotonicityResult<VariableType>::Monotonicity::Unknown);
                    return;
                }
                index++;
            }
            if(index == succSize){
                monResult->updateMonotonicityResult(param, monCandidate);
                return;
            }

            //go to first non-inc / non-dec
            while(index < succSize){
                temp = succsMon[index];
                if(temp == MonotonicityResult<VariableType>::Monotonicity::Not){
                    monResult->updateMonotonicityResult(param, MonotonicityResult<VariableType>::Monotonicity::Unknown);
                    return;
                }
                else if(temp == MonotonicityResult<VariableType>::Monotonicity::Constant || temp == monCandidate){
                    index++;
                }
                else{
                    monCandidate = temp;
                    break;
                }
            }

            //check if it doesn't change until the end of vector
            while(index < succSize){
                temp = succsMon[index];
                if(temp == MonotonicityResult<VariableType>::Monotonicity::Constant || temp == monCandidate){
                    index++;
                }
                else{
                    monResult->updateMonotonicityResult(param, MonotonicityResult<VariableType>::Monotonicity::Unknown);
                    return;
                }
            }

            if(monCandidate == MonotonicityResult<VariableType>::Monotonicity::Incr){
                monResult->updateMonotonicityResult(param, MonotonicityResult<VariableType>::Monotonicity::Decr);
                return;
            }
            else{
                monResult->updateMonotonicityResult(param, MonotonicityResult<VariableType>::Monotonicity::Incr);
                return;
            }


        }
                
        // Returns a specific matrix entry
        template <typename ValueType, typename ConstantType>
        ValueType MonotonicityChecker<ValueType, ConstantType>::getMatrixEntry(uint_fast64_t rowIndex, uint_fast64_t columnIndex){
            // Check if indices are out of bounds
            assert (rowIndex < matrix.getRowCount() && columnIndex < matrix.getColumnCount());

            auto row = matrix.getRow(rowIndex);
            for(auto itr = row.begin(); itr!=row.end(); itr++){
                if(itr->getColumn() == columnIndex){
                    return itr->getValue();
                }
            }
            //Change to ValueType?
            return ValueType(0);
        }

        template class MonotonicityChecker<RationalFunction, double>;
        template class MonotonicityChecker<RationalFunction, RationalNumber>;
    }
}





















