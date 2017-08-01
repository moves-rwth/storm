#pragma once

#include <set>
#include <string>
#include <vector>
#include <memory>

#include "storm/models/symbolic/Model.h"
#include "storm/storage/bisimulation/BisimulationType.h"

#include "storm/logic/Formula.h"

#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            template <storm::dd::DdType DdType, typename ValueType>
            class PreservationInformation {
            public:
                PreservationInformation() = default;
            
                PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, storm::storage::BisimulationType const& bisimulationType);
                PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::string> const& labels, storm::storage::BisimulationType const& bisimulationType);
                PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<storm::expressions::Expression> const& expressions, storm::storage::BisimulationType const& bisimulationType);
                PreservationInformation(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType const& bisimulationType);
                
                void addLabel(std::string const& label);
                void addExpression(storm::expressions::Expression const& expression);
                
                std::set<std::string> const& getLabels() const;
                std::set<storm::expressions::Expression> const& getExpressions() const;
                
            private:
                std::set<std::string> labels;
                std::set<storm::expressions::Expression> expressions;
            };
            
        }
    }
}
