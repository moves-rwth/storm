#pragma once

#include <set>
#include <string>

#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            class PreservationInformation {
            public:
                PreservationInformation() = default;
                
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
