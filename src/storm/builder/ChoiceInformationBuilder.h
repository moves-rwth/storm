#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <boost/any.hpp>

#include "storm/models/sparse/ChoiceLabeling.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/sparse/PrismChoiceOrigins.h"
#include "storm/storage/prism/Program.h"


namespace storm {
    namespace builder {
        
        /*!
         * This class collects information regarding the choices
         */
        class ChoiceInformationBuilder {
        public:
            
            ChoiceInformationBuilder() = default;

            void addLabel(std::string const& label, uint_fast64_t choiceIndex);
            
            void addOriginData(boost::any const& originData, uint_fast64_t choiceIndex);
            
            boost::optional<storm::models::sparse::ChoiceLabeling> buildChoiceLabeling(uint_fast64_t totalNumberOfChoices);
            
            std::vector<boost::any> buildDataOfChoiceOrigins(uint_fast64_t totalNumberOfChoices);
            
        private:
            std::unordered_map<std::string, storm::storage::BitVector> labels;
            std::vector<boost::any> dataOfOrigins;
        };
    }
}
    
