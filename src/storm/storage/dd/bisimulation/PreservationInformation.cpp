#include "storm/storage/dd/bisimulation/PreservationInformation.h"

namespace storm {
    namespace dd {
        namespace bisimulation {
            
            void PreservationInformation::addLabel(std::string const& label) {
                labels.insert(label);
            }
            
            void PreservationInformation::addExpression(storm::expressions::Expression const& expression) {
                expressions.insert(expression);
            }
            
            std::set<std::string> const& PreservationInformation::getLabels() const {
                return labels;
            }
            
            std::set<storm::expressions::Expression> const& PreservationInformation::getExpressions() const {
                return expressions;
            }
        }
    }
}
