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
                actions.insert("__unlabeled");
                stateActionMap[state].push_back("__unlabeled");
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
    os << "using QuickPOMDPs: QuickPOMDP \n" << "using POMDPTools: Deterministic, SparseCat\n\n";
    os << "function ExportedPOMDP()\n";
    os << "   return QuickPOMDP( \n"
       << "   states = [";
    for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state){
        os << "\"" << state << "\",";
    }
    os << "\"__sink\"],\n";
    std::string allActions;
    for (auto const & action : actions){
        allActions += "\"" + action + "\", ";
    }
    allActions.pop_back();
    allActions.pop_back();
    os << "   actions = [" << allActions << "],\n"

       << "   observations = [";
    uint64_t i = 0;
    for(auto const &obs : observations){
        os << "\"" << obs << "\",";
    }
    os << "\"__sink\"],\n"
    << "   discount = " << std::to_string(discount) << ",\n";

    os << "   transition = function(s, a)\n";
    for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state){
        auto rowIndex = pomdp->getTransitionMatrix().getRowGroupIndices()[state];
        if(state == 0){
            os << "      if s == \"0\"\n";
        }
        else {
            os << "      elseif s == \"" << state << "\"\n";
        }
        for (uint64_t action = 0; action < pomdp->getNumberOfChoices(state); ++action){
            std::string label;
            auto labelsOfChoice = pomdp->getChoiceLabeling().getLabelsOfChoice(rowIndex+action);
            if(labelsOfChoice.empty()){
                label = "__unlabeled";
            } else {
                label = *(labelsOfChoice.begin());
            }
            std::replace( label.begin(), label.end(), '\t', ' ');
            std::replace( label.begin(), label.end(), '\n', ' ');
            if(action == 0) {
                os << "         if a == \"" << label << "\"\n";
            } else {
                os << "         elseif a == \"" << label << "\"\n";
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
        }
        os << "         else\n"
           << "            return Deterministic(\"__sink\")\n"
           << "         end\n";
    }

    os << "      else\n"
       << "         return Deterministic(\"__sink\")\n"
       << "      end\n";

    os << "   end,\n";

    os << "   observation = function(a, sp)\n";
    for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
        if (state == 0) {
            os << "      if sp == \"0\"\n";
        } else {
            os << "      elseif sp == \"" << state << "\"\n";
        }
        auto obs = pomdp->getObservationValuations().getStateInfo(pomdp->getObservation(state));
        std::replace( obs.begin(), obs.end(), '\t', ' ');
        std::replace( obs.begin(), obs.end(), '\n', ' ');
        os << "         return Deterministic(\"" << obs << "\")\n";
    }
    os << "      else\n"
       << "         return Deterministic(\"__sink\")\n"
       << "      end\n";
    os << "   end,\n";

    os << "   reward = function(s, a, sp)\n";
    i = 0;
    if(probabilityAnalysis) {
        // Generate a standard reward structure for probability analysis
        // TODO Check if this need a sink to move to
        for(auto const &target : targetStates){
            if(i == 0){
                os << "      if sp == \"" << target << "\"\n";
            } else {
                os << "      elseif sp == \"" << target << "\"\n";
            }
                os << "         return ";
                if(min){
                    os << "-";
                }
                  os << "1.0\n";
            if (i == targetStates.getNumberOfSetBits()-1){

            }
            ++i;
        }
        os << "      else\n"
           << "         return 0.0\n"
           << "      end\n";
    } else {
        // TODO check if correct for state based rewards
        auto rewardModel = pomdp->getRewardModel(formulaInfo.getRewardModelName());
        for(uint64_t state = 0; state < pomdp->getNumberOfStates(); ++state) {
            if (state == 0) {
                os << "      if s == \"0\"\n";
            } else {
                os << "      elseif s == \"" << state << "\"\n";
            }
            if(rewardModel.hasOnlyStateRewards()){
                os << "         return ";
                if(min){
                    os << "-";
                }
                os << rewardModel.getStateReward(state) << "\n";
            } else {
                auto rowIndex = pomdp->getTransitionMatrix().getRowGroupIndices()[state];
                for (uint64_t action = 0; action < pomdp->getNumberOfChoices(state); ++action){
                    std::string label;
                    auto labelsOfState = pomdp->getChoiceLabeling().getLabelsOfChoice(rowIndex+action);
                    if(labelsOfState.empty()){
                        label = "__unlabeled";
                    } else {
                        label = *(labelsOfState.begin());
                    }
                    std::replace( label.begin(), label.end(), '\t', ' ');
                    std::replace( label.begin(), label.end(), '\n', ' ');
                    if(action == 0) {
                        os << "         if a == \"" << label << "\"\n";
                    } else {
                        os << "         elseif a == \"" << label << "\"\n";
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
                }
                os << "         else\n"
                   << "            return -typemax(Int64)"
                   << "         end\n";
            }
        }
        os << "      else\n"
           << "         return -typemax(Int64)\n"
           << "      end\n";
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
    os << "end";
}

template void exportPomdpToJulia<double>(std::ostream& os, std::shared_ptr<storm::models::sparse::Pomdp<double>> pomdp, double discount, storm::logic::Formula const& formula);
template void exportPomdpToJulia<storm::RationalNumber>(std::ostream& os, std::shared_ptr<storm::models::sparse::Pomdp<storm::RationalNumber>> pomdp, double discount, storm::logic::Formula const& formula);
}
}
}
