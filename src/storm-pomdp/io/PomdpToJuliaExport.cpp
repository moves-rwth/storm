#include "PomdpToJuliaExport.h"
#include "storm-pomdp/analysis/FormulaInformation.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
namespace pomdp {
namespace exporter {

template<typename ValueType>
void exportPomdpToJulia(std::ostream& os, std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> pomdp, double discount, storm::logic::Formula const& formula){
    os << std::setprecision(9);
    STORM_LOG_ERROR_COND(pomdp->hasObservationValuations(), "POMDP was built without observation valuations. Export to Julia format not possible!");
    STORM_LOG_ERROR_COND(pomdp->hasChoiceLabeling(), "POMDP was built without choice labeling. Export to Julia format not possible!");

    // Analyse formula
    auto formulaInfo = storm::pomdp::analysis::getFormulaInformation(*pomdp, formula);
    bool probabilityAnalysis = !formula.isRewardOperatorFormula();
    bool min = formulaInfo.minimize();
    auto targetStates = formulaInfo.getTargetStates().states;

    std::unordered_set<uint64_t> states;
    std::unordered_set<std::string> observations;
    std::set<std::string> actions;

    std::unordered_map<uint64_t, std::vector<std::string>> stateActionMap;

    std::vector<std::unordered_map<uint64_t, ValueType>> choices;

    for (uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
        auto rowIndex = pomdp->getTransitionMatrix().getRowGroupIndices()[state];
        for (uint64_t action = 0; action < pomdp->getNumberOfChoices(state); ++action){
            auto choiceLabeling = pomdp->getChoiceLabeling();
            auto labelsOfChoice = choiceLabeling.getLabelsOfChoice(rowIndex+action);
            if(labelsOfChoice.empty()){
                actions.insert("__sink");
                stateActionMap[state].push_back("__sink");
            } else {
                std::string label = *(labelsOfChoice.begin());
                std::replace( label.begin(), label.end(), '\t', ' ');
                std::replace( label.begin(), label.end(), '\n', ' ');
                actions.insert(label);
                stateActionMap[state].push_back(label);
            }

            std::unordered_map<uint64_t, ValueType> stateProbs;
            for (auto const &pomdpTransition : pomdp->getTransitionMatrix().getRow(state, action)) {
                if (!storm::utility::isZero(pomdpTransition.getValue())) {
                    stateProbs[pomdpTransition.getColumn()] = pomdpTransition.getValue();
                }
            }
            choices.push_back(stateProbs);
        }
    }
    for (uint64_t obsId = 0; obsId < pomdp->getObservationValuations().getNumberOfStates(); ++obsId) {
        auto obs = pomdp->getObservationValuations().getStateInfo(obsId);
        std::replace( obs.begin(), obs.end(), '\t', ' ');
        std::replace( obs.begin(), obs.end(), '\n', ' ');
        observations.insert(obs);
    }

    os << "using QuickPOMDPs: QuickPOMDP \n" << "using POMDPTools: Deterministic, SparseCat\n\n"
          << "pomdp = QuickPOMDP( \n"
          << "   states = [";
    for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state){
        os << "\"" << state << "\"";
        if(state != pomdp->getNumberOfStates()-1){
            os << ",";
        }
    }
    os << "],\n";
    std::string allActions;
    for (auto const & action : actions){
        allActions += "\"" + action + "\", ";
    }
    allActions.pop_back();
    allActions.pop_back();
    os << "   actions = function(s = nothing)\n"
    << "      if isnothing(s)\n"
    << "         return [" << allActions << "]\n"
    << "      end\n";
    for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
        if (state == 0) {
            os << "      if s == \"0\"\n";
        } else if (state != pomdp->getNumberOfStates() - 1) {
            os << "      elseif s == \"" << state << "\"\n";
        } else {
            os << "      else\n";
        }
        std::string actionStr;
        for(auto const &entry : stateActionMap[state]){
            actionStr += "\"" + entry + "\", ";
        }
        actionStr.pop_back();
        actionStr.pop_back();
        os << "         return [" << actionStr << "]\n";
        if (state == pomdp->getNumberOfStates()-1){
            os << "      end\n";
        }
    }
    os << "   end,\n"

       << "   observations = [";
    uint64_t i = 0;
    for(auto const &obs : observations){
        os << "\"" << obs << "\"";
        if(i != observations.size()-1){
            os << ",";
        }
        ++i;
    }
    os << "],\n"
    << "   discount = " << std::to_string(discount) << ",\n";

    os << "   transition = function(s, a)\n";
    for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state){
        auto rowIndex = pomdp->getTransitionMatrix().getRowGroupIndices()[state];
        if(state == 0){
            os << "      if s == \"0\"\n";
        }
        else if (state != pomdp->getNumberOfStates()-1){
            os << "      elseif s == \"" << state << "\"\n";
        } else {
            os << "      else\n";
        }
        for (uint64_t action = 0; action < pomdp->getNumberOfChoices(state); ++action){
            std::string label;
            auto labelsOfChoice = pomdp->getChoiceLabeling().getLabelsOfChoice(rowIndex+action);
            if(labelsOfChoice.empty()){
                label = "__sink";
            } else {
                label = *(labelsOfChoice.begin());
            }
            std::replace( label.begin(), label.end(), '\t', ' ');
            std::replace( label.begin(), label.end(), '\n', ' ');
            if(action == 0) {
                os << "         if a == \"" << label << "\"\n";
            } else if (action != pomdp->getNumberOfChoices(state)-1){
                os << "         elseif a == \"" << label << "\"\n";
            } else {
                os << "         else\n";
            }
            // Collect successors
            std::string successorStateStr;
            std::string successorProbStr;
            for(auto const &entry : choices[rowIndex + action]){
                successorStateStr += "\"" + std::to_string(entry.first) + "\", ";
                successorProbStr += std::to_string(storm::utility::convertNumber<double>(entry.second)) + ", ";
            }
            successorStateStr.pop_back();
            successorProbStr.pop_back();
            successorStateStr.pop_back();
            successorProbStr.pop_back();

            os << "            return SparseCat([" << successorStateStr << "], [" << successorProbStr << "])\n";

            if (action == pomdp->getNumberOfChoices(state)-1){
                os << "         end\n";
            }
        }

        if (state == pomdp->getNumberOfStates()-1){
            os << "      end\n";
        }
    }
    os << "   end,\n";

    os << "   observation = function(a, sp)\n";
    for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
        if (state == 0) {
            os << "      if sp == \"0\"\n";
        } else if (state != pomdp->getNumberOfStates() - 1) {
            os << "      elseif sp == \"" << state << "\"\n";
        } else {
            os << "      else\n";
        }
        auto obs = pomdp->getObservationValuations().getStateInfo(pomdp->getObservation(state));
        std::replace( obs.begin(), obs.end(), '\t', ' ');
        std::replace( obs.begin(), obs.end(), '\n', ' ');
        os << "         return Deterministic(\"" << obs << "\")\n";
        if (state == pomdp->getNumberOfStates()-1){
            os << "      end\n";
        }
    }
    os << "   end,\n";

    os << "   reward = function(s, a, sp)\n";
    i = 0;
    if(probabilityAnalysis) {
        // Generate a standard reward structure for probability analysis
        // TODO Check if this need a sink to move to
        for(auto const &target : targetStates){
            if(i == 0){
                os << "      if sp == \"" << target << "\"\n";
            } else if (i != targetStates.getNumberOfSetBits()-1) {
                os << "      elseif sp == \"" << target << "\"\n";
            } else {
                os << "      else\n";
            }
                os << "         return ";
                if(min){
                    os << "-";
                }
                  os << "1.0\n";
            if (i == targetStates.getNumberOfSetBits()-1){
                os << "      end\n";
            }
            ++i;
        }
    } else {
        auto rewardModel = pomdp->getRewardModel(formulaInfo.getRewardModelName());
        for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
            if (state == 0) {
                os << "      if s == \"0\"\n";
            } else if (state != pomdp->getNumberOfStates() - 1) {
                os << "      elseif s == \"" << state << "\"\n";
            } else {
                os << "      else\n";
            }
            if(rewardModel.hasOnlyStateRewards()){
                os << "         return " << rewardModel.getStateReward(state) << "\n";
            } else {
                auto rowIndex = pomdp->getTransitionMatrix().getRowGroupIndices()[state];
                for (uint64_t action = 0; action < pomdp->getNumberOfChoices(state); ++action){
                    std::string label;
                    auto labelsOfState = pomdp->getChoiceLabeling().getLabelsOfChoice(rowIndex+action);
                    if(labelsOfState.empty()){
                        label = "__sink";
                    } else {
                        label = *(labelsOfState.begin());
                    }
                    std::replace( label.begin(), label.end(), '\t', ' ');
                    std::replace( label.begin(), label.end(), '\n', ' ');
                    if(action == 0) {
                        os << "         if a == \"" << label << "\"\n";
                    } else if (action != pomdp->getNumberOfChoices(state)-1){
                        os << "         elseif a == \"" << label << "\"\n";
                    } else {
                        os << "         else\n";
                    }
                    if(rewardModel.hasTransitionRewards()){
                        //TODO iterate over all successors
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Transition rewards are currently not supported by the transformation!");
                    } else {
                        os << "            return ";
                        if(min){
                            os << "-";
                        }
                        os << rewardModel.getStateActionReward(rowIndex + action) << "\n";
                    }
                    if (action == pomdp->getNumberOfChoices(state)-1){
                        os << "         end\n";
                    }
                }
            }
            if (state == pomdp->getNumberOfStates() - 1) {
                os << "      end\n";
            }
        }
    }
    os << "   end,\n";

    os << "   initialstate = Deterministic(\"" << pomdp->getInitialStates().getNextSetIndex(0) << "\"),\n";
    os << "   isterminal = s -> (" ;
    i=0;
    for(auto const &target : targetStates){
        os << "s == \"" << target << "\"";
        if (i != targetStates.getNumberOfSetBits()-1){
            os << " || ";
        }
        ++i;
    }
    os << ")\n";
    os << ")\n";
}

template void exportPomdpToJulia<double>(std::ostream& os, std::shared_ptr<storm::models::sparse::Pomdp<double>> pomdp, double discount, storm::logic::Formula const& formula);
template void exportPomdpToJulia<storm::RationalNumber>(std::ostream& os, std::shared_ptr<storm::models::sparse::Pomdp<storm::RationalNumber>> pomdp, double discount, storm::logic::Formula const& formula);
}
}
}
