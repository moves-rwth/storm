#include "storm/builder/ChoiceInformationBuilder.h"

namespace storm {
    namespace builder {
         
        void ChoiceInformationBuilder::addLabel(std::string const& label, uint_fast64_t choiceIndex) {
            storm::storage::BitVector& labeledChoices = labels[label];
            labeledChoices.grow(choiceIndex + 1);
            labeledChoices.set(choiceIndex, true);
        }
        
        void ChoiceInformationBuilder::addOriginData(boost::any const& originData, uint_fast64_t choiceIndex) {
            if (dataOfOrigins.size() != choiceIndex) {
                dataOfOrigins.resize(choiceIndex);
            }
            dataOfOrigins.push_back(originData);
        }
        
        boost::optional<storm::models::sparse::ChoiceLabeling> ChoiceInformationBuilder::buildChoiceLabeling(uint_fast64_t totalNumberOfChoices) {
            if (labels.empty()) {
                return boost::none;
            } else {
                storm::models::sparse::ChoiceLabeling result(totalNumberOfChoices);
                for (auto& label : labels) {
                    label.second.resize(totalNumberOfChoices, false);
                    result.addLabel(label.first, std::move(label.second));
                }
                return result;
            }
        }
            
        std::vector<boost::any> ChoiceInformationBuilder::buildDataOfChoiceOrigins(uint_fast64_t totalNumberOfChoices) {
            dataOfOrigins.resize(totalNumberOfChoices);
            return std::move(dataOfOrigins);
        }
        
    }
}
