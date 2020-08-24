#include "AcceptanceCondition.h"

namespace storm {
namespace automata {

AcceptanceCondition::AcceptanceCondition(std::size_t numberOfStates, unsigned int numberOfAcceptanceSets, acceptance_expr::ptr acceptance)
    : numberOfStates(numberOfStates), numberOfAcceptanceSets(numberOfAcceptanceSets), acceptance(acceptance) {

    // initialize acceptance sets
    for (unsigned int i = 0; i < numberOfAcceptanceSets; i++) {
        acceptanceSets.push_back(storm::storage::BitVector(numberOfStates));
    }
}

unsigned int AcceptanceCondition::getNumberOfAcceptanceSets() const {
    return numberOfAcceptanceSets;
}


storm::storage::BitVector& AcceptanceCondition::getAcceptanceSet(unsigned int index) {
    return acceptanceSets.at(index);
}

const storm::storage::BitVector& AcceptanceCondition::getAcceptanceSet(unsigned int index) const {
    return acceptanceSets.at(index);
}

AcceptanceCondition::acceptance_expr::ptr AcceptanceCondition::getAcceptanceExpression() const {
    return acceptance;
}


bool AcceptanceCondition::isAccepting(const storm::storage::StateBlock& scc) const {
    return isAccepting(scc, acceptance);
}

bool AcceptanceCondition::isAccepting(const storm::storage::StateBlock& scc, acceptance_expr::ptr expr) const {
    switch (expr->getType()) {
    case acceptance_expr::EXP_AND:
        return isAccepting(scc, expr->getLeft()) && isAccepting(scc, expr->getRight());
    case acceptance_expr::EXP_OR:
        return isAccepting(scc, expr->getLeft()) && isAccepting(scc, expr->getRight());
    case acceptance_expr::EXP_NOT:
        return !isAccepting(scc, expr->getLeft());
    case acceptance_expr::EXP_TRUE:
        return true;
    case acceptance_expr::EXP_FALSE:
        return false;
    case acceptance_expr::EXP_ATOM: {
        const cpphoafparser::AtomAcceptance& atom = expr->getAtom();
        const storm::storage::BitVector& acceptanceSet = acceptanceSets.at(atom.getAcceptanceSet());
        bool negated = atom.isNegated();
        bool rv;
        switch (atom.getType()) {
        case cpphoafparser::AtomAcceptance::TEMPORAL_INF:
            rv = false;
            for (auto& state : scc) {
                if (acceptanceSet.get(state)) {
                    rv = true;
                    break;
                }
            }
            break;
        case cpphoafparser::AtomAcceptance::TEMPORAL_FIN:
            rv = true;
            for (auto& state : scc) {
                if (acceptanceSet.get(state)) {
                    rv = false;
                    break;
                }
            }
            break;
        }

        return (negated ? !rv : rv);
    }
    }

    throw std::runtime_error("Missing case statement");
}

AcceptanceCondition::ptr AcceptanceCondition::lift(std::size_t productNumberOfStates, std::function<std::size_t (std::size_t)> mapping) const {
    AcceptanceCondition::ptr lifted(new AcceptanceCondition(productNumberOfStates, numberOfAcceptanceSets, acceptance));
    for (unsigned int i = 0; i < numberOfAcceptanceSets; i++) {
        const storm::storage::BitVector& set = getAcceptanceSet(i);
        storm::storage::BitVector& liftedSet = lifted->getAcceptanceSet(i);

        for (std::size_t prodState = 0; prodState < productNumberOfStates; prodState++) {
            if (set.get(mapping(prodState))) {
                liftedSet.set(prodState);
            }
        }
    }

    return lifted;
}


}
}
