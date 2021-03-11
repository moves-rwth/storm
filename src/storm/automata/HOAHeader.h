#pragma once

#include "storm/automata/APSet.h"
#include "cpphoafparser/consumer/hoa_consumer.hh"
#include <boost/optional.hpp>

namespace storm {
    namespace automata {
        class HOAHeader {
        public:
            boost::optional<unsigned int> startState;
            boost::optional<unsigned int> numberOfStates;
            APSet apSet;

            boost::optional<unsigned int> numberOfAcceptanceSets;
            cpphoafparser::HOAConsumer::acceptance_expr::ptr acceptance_expression;
            boost::optional<std::string> accName;
            boost::optional<std::vector<cpphoafparser::IntOrString>> accNameExtraInfo;

            AcceptanceCondition::ptr getAcceptanceCondition() {
                return AcceptanceCondition::ptr(new AcceptanceCondition(*numberOfStates, *numberOfAcceptanceSets, acceptance_expression));
            }

        };
    }
}
