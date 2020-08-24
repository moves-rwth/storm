#include "storm/automata/DeterministicAutomaton.h"
#include "storm/automata/AcceptanceCondition.h"
#include "storm/automata/HOAConsumerDA.h"
#include "cpphoafparser/parser/hoa_parser.hh"
#include "cpphoafparser/parser/hoa_parser_helper.hh"
#include "cpphoafparser/consumer/hoa_intermediate_check_validity.hh"

namespace storm {
    namespace automata {
        DeterministicAutomaton::DeterministicAutomaton(APSet apSet, std::size_t numberOfStates, std::size_t initialState, AcceptanceCondition::ptr acceptance)
                : apSet(apSet), numberOfStates(numberOfStates), initialState(initialState), acceptance(acceptance) {
            // TODO: this could overflow, add check?
            edgesPerState = apSet.alphabetSize();
            numberOfEdges = numberOfStates * edgesPerState;
            successors.resize(numberOfEdges);
        }

        std::size_t DeterministicAutomaton::getInitialState() const {
            return initialState;
        }

        const APSet& DeterministicAutomaton::getAPSet() const {
            return apSet;
        }

        std::size_t DeterministicAutomaton::getSuccessor(std::size_t from, APSet::alphabet_element label) const {
            std::size_t index = from * edgesPerState + label;
            return successors.at(index);
        }

        void DeterministicAutomaton::setSuccessor(std::size_t from, APSet::alphabet_element label, std::size_t successor) {
            std::size_t index = from * edgesPerState + label;
            successors.at(index) = successor;
        }

        std::size_t DeterministicAutomaton::getNumberOfStates() const {
            return numberOfStates;
        }

        std::size_t DeterministicAutomaton::getNumberOfEdgesPerState() const {
            return edgesPerState;
        }

        AcceptanceCondition::ptr DeterministicAutomaton::getAcceptance() const {
            return acceptance;
        }


        void DeterministicAutomaton::printHOA(std::ostream& out) const {
            out << "HOA: v1\n";

            out << "States: " << numberOfStates << "\n";

            out << "Start: " << initialState << "\n";

            out << "AP: " << apSet.size();
            for (unsigned int i = 0; i < apSet.size(); i++) {
                out << " " << cpphoafparser::HOAParserHelper::quote(apSet.getAP(i));
            }
            out << "\n";

            out << "Acceptance: " << acceptance->getNumberOfAcceptanceSets() << " " << *acceptance->getAcceptanceExpression() << "\n";

            out << "--BODY--" << "\n";

            for (std::size_t s = 0; s < getNumberOfStates(); s++) {
                out << "State: " << s;
                out << " {";
                bool first = true;
                for (unsigned int i = 0; i < acceptance->getNumberOfAcceptanceSets(); i++) {
                    if (acceptance->getAcceptanceSet(i).get(s)) {
                        if (!first)
                            out << " ";
                        first = false;
                        out << i;
                    }
                }
                out << "}\n";
                for (std::size_t label = 0; label < getNumberOfEdgesPerState(); label++) {
                    out << getSuccessor(s, label) << "\n";
                }
            }
        }


        DeterministicAutomaton::ptr DeterministicAutomaton::parse(std::istream& in) {
            HOAConsumerDA::ptr consumer(new HOAConsumerDA());
            cpphoafparser::HOAIntermediateCheckValidity::ptr validator(new cpphoafparser::HOAIntermediateCheckValidity(consumer));
            cpphoafparser::HOAParser::parse(in, validator);

            return consumer->getDA();
        }

        DeterministicAutomaton::ptr DeterministicAutomaton::parseFromFile(const std::string& filename) {
            std::ifstream in(filename);
            return parse(in);
        }

    }
}



