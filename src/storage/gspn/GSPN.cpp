#include "src/storage/gspn/GSPN.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"


#include <boost/lexical_cast.hpp>

namespace storm {
    namespace gspn {
        void GSPN::addImmediateTransition(storm::gspn::ImmediateTransition<WeightType> const& transition) {
            this->immediateTransitions.push_back(std::make_shared<storm::gspn::ImmediateTransition<WeightType>>(transition));
        }

        void GSPN::addTimedTransition(storm::gspn::TimedTransition<RateType> const& transition) {
            this->timedTransitions.push_back(std::make_shared<storm::gspn::TimedTransition<RateType>>(transition));
        }

        void GSPN::addPlace(Place const& place) {
            this->places.push_back(place);
        }

        uint_fast64_t GSPN::getNumberOfPlaces() const {
            return places.size();
        }

        std::vector<std::shared_ptr<storm::gspn::TimedTransition<GSPN::RateType>>> const& GSPN::getTimedTransitions() const {
            return this->timedTransitions;
        }

        std::vector<std::shared_ptr<storm::gspn::ImmediateTransition<GSPN::WeightType>>> const& GSPN::getImmediateTransitions() const {
            return this->immediateTransitions;
        }

        std::vector<storm::gspn::Place> const& GSPN::getPlaces() const {
            return places;
        }

        std::shared_ptr<storm::gspn::Marking> GSPN::getInitialMarking(std::map<uint_fast64_t, uint_fast64_t>& numberOfBits, uint_fast64_t const& numberOfTotalBits) const {
            auto m = std::make_shared<storm::gspn::Marking>(getNumberOfPlaces(), numberOfBits, numberOfTotalBits);
            for (auto& place : getPlaces()) {
                m->setNumberOfTokensAt(place.getID(), place.getNumberOfInitialTokens());
            }
            return m;
        }

        std::pair<bool, storm::gspn::Place> GSPN::getPlace(uint_fast64_t const& id) const {
            for (auto& place : places) {
                if (id == place.getID()) {
                    return std::make_pair<bool, storm::gspn::Place const&>(true, place);
                }
            }
            return std::make_pair<bool, storm::gspn::Place>(false, storm::gspn::Place());
        }

        std::pair<bool, storm::gspn::Place> GSPN::getPlace(std::string const& id) const {
            for (auto& place : places) {
                if (id.compare(place.getName()) == 0) {
                    return std::make_pair<bool, storm::gspn::Place const&>(true, place);
                }
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "No place with name " << id);
        };

        std::pair<bool, std::shared_ptr<storm::gspn::TimedTransition<GSPN::RateType>> const> GSPN::getTimedTransition(std::string const& id) const {
            for (auto& trans : timedTransitions) {
                if (id.compare(trans->getName()) == 0) {
                    return std::make_pair<bool, std::shared_ptr<storm::gspn::TimedTransition<GSPN::RateType>> const>(true, static_cast<std::shared_ptr<storm::gspn::TimedTransition<GSPN::RateType>> const>(trans));
                }
            }
            return std::make_pair<bool, std::shared_ptr<storm::gspn::TimedTransition<GSPN::RateType>> const>(false, nullptr);
        }

        std::pair<bool, std::shared_ptr<storm::gspn::ImmediateTransition<GSPN::WeightType>> const> GSPN::getImmediateTransition(std::string const& id) const {
            for (auto& trans : immediateTransitions) {
                if (id.compare(trans->getName()) == 0) {
                    return std::make_pair<bool, std::shared_ptr<storm::gspn::ImmediateTransition<GSPN::WeightType>> const>(true, static_cast<std::shared_ptr<storm::gspn::ImmediateTransition<GSPN::WeightType>> const>(trans));
                }
            }
            return std::make_pair<bool, std::shared_ptr<storm::gspn::ImmediateTransition<GSPN::WeightType>> const>(false, nullptr);
        }

        std::pair<bool, std::shared_ptr<storm::gspn::Transition> const> GSPN::getTransition(std::string const& id) const {
            auto trans = getTimedTransition(id);
            if (trans.first == true) {
                return trans;
            }

            return getImmediateTransition(id);
        }

        void GSPN::writeDotToStream(std::ostream& outStream) {
            outStream << "digraph " << this->getName() << " {" << std::endl;

            // print places with initial marking (not printed is the capacity)
            outStream << "\t" << "node [shape=ellipse]" << std::endl;
            for (auto& place : this->getPlaces()) {
                outStream << "\t" << place.getName() << " [label=\"" << place.getName() << "(" << place.getNumberOfInitialTokens();
                outStream << ")\"];" << std::endl;
            }

            // print transitions with weight/rate
            outStream << "\t" << "node [shape=box]" << std::endl;
            for (auto& trans : this->getImmediateTransitions()) {
                outStream << "\t" << trans->getName() << " [fontcolor=white, style=filled, fillcolor=black, label=\"" << trans->getName() << "\"];" << std::endl;
            }

            for (auto& trans : this->getTimedTransitions()) {
                outStream << "\t" << trans->getName() << " [label=\"" << trans->getName();
                outStream << " (" << trans->getRate() << ")\"];" << std::endl;
            }

            // print arcs
            for (auto& trans : this->getImmediateTransitions()) {

                for (auto &placePtr : trans->getInputPlaces()) {
                    outStream << "\t" << placePtr->getName() << " -> " << trans->getName() << "[label=\"" <<
                            trans->getInputArcMultiplicity(*placePtr);
                    outStream << "\"];" << std::endl;
                }

                for (auto &placePtr : trans->getInhibitionPlaces()) {
                    outStream << "\t" << placePtr->getName() << " -> " << trans->getName() << "[arrowhead=\"dot\", label=\"" <<
                            trans->getInhibitionArcMultiplicity(*placePtr);
                    outStream << "\"];" << std::endl;
                }

                for (auto &placePtr : trans->getOutputPlaces()) {
                    outStream << "\t" << trans->getName() << " -> " << placePtr->getName() << "[label=\"" <<
                            trans->getOutputArcMultiplicity(*placePtr);
                    outStream << "\"];" << std::endl;
                }
            }

            for (auto& trans : this->getTimedTransitions()) {
                for (auto &placePtr : trans->getInputPlaces()) {
                    outStream << "\t" << placePtr->getName() << " -> " << trans->getName() << "[label=\"" <<
                            trans->getInputArcMultiplicity(*placePtr);
                    outStream << "\"];" << std::endl;
                }

                for (auto &placePtr : trans->getInhibitionPlaces()) {
                    outStream << "\t" << placePtr->getName() << " -> " << trans->getName() << "[arrowhead=\"dot\", label=\""  <<
                              trans->getInhibitionArcMultiplicity(*placePtr);
                    outStream << "\"];" << std::endl;
                }

                for (auto &placePtr : trans->getOutputPlaces()) {
                    outStream << "\t" << trans->getName() << " -> " << placePtr->getName() << "[label=\"" <<
                              trans->getOutputArcMultiplicity(*placePtr);
                    outStream << "\"];" << std::endl;
                }
            }

            outStream << "}" << std::endl;
        }
        
        void GSPN::setName(std::string const& name) {
            this->name = name;
        }

        std::string const& GSPN::getName() const {
            return this->name;
        }

        bool GSPN::isValid() const {
            bool result = true;
            result |= testPlaces();
            result |= testTransitions();

            return result;
        }

        bool GSPN::testPlaces() const {
            std::vector<std::string> namesOfPlaces;
            std::vector<uint_fast64_t> idsOfPlaces;
            bool result = true;

            for (auto const& place : this->getPlaces()) {

                if (std::find(namesOfPlaces.begin(), namesOfPlaces.end(), place.getName()) != namesOfPlaces.end()) {
                    STORM_PRINT_AND_LOG("duplicates states with the name \"" + place.getName() + "\"\n")
                    result = false;
                }

                if (std::find(idsOfPlaces.begin(), idsOfPlaces.end(), place.getID()) != idsOfPlaces.end()) {
                    STORM_PRINT_AND_LOG("duplicates states with the id \"" + boost::lexical_cast<std::string>(place.getID()) + "\"\n")
                    result = false;
                }

                if (place.getNumberOfInitialTokens() > place.getNumberOfInitialTokens()) {
                    STORM_PRINT_AND_LOG("number of initial tokens is greater than the capacity for place \"" + place.getName() + "\"\n")
                    result = false;
                }
            }

            return result;
        }

        bool GSPN::testTransitions() const {
            bool result = true;

            for (auto const& transition : this->getImmediateTransitions()) {
                if (transition->getInputPlaces().empty() &&
                    transition->getInhibitionPlaces().empty()) {
                    STORM_PRINT_AND_LOG("transition \"" + transition->getName() + "\" has no input or inhibition place\n")
                    result = false;
                }

                if (transition->getOutputPlaces().empty()) {
                    STORM_PRINT_AND_LOG("transition \"" + transition->getName() + "\" has no output place\n")
                    result = false;
                }
            }

            for (auto const& transition : this->getTimedTransitions()) {
                if (transition->getInputPlaces().empty() &&
                    transition->getInputPlaces().empty()) {
                    STORM_PRINT_AND_LOG("transition \"" + transition->getName() + "\" has no input or inhibition place\n")
                    result = false;
                }

                if (transition->getOutputPlaces().empty()) {
                    STORM_PRINT_AND_LOG("transition \"" + transition->getName() + "\" has no output place\n")
                    result = false;
                }
            }

            //test if places exists in the gspn
            for (auto const& transition : this->getImmediateTransitions()) {
                for (auto &placePtr : transition->getInputPlaces()) {
                    bool foundPlace = false;
                    for (auto const& place : places) {
                        if (place.getName() == placePtr->getName()) {
                            foundPlace = true;
                        }
                    }
                    if (!foundPlace) {
                        STORM_PRINT_AND_LOG("input place \"" + placePtr->getName() + "\" of transition \"" + transition->getName() + "\" was not found \n")
                        result = false;
                    }
                }

                for (auto &placePtr : transition->getInhibitionPlaces()) {
                    bool foundPlace = false;
                    for (auto const& place : places) {
                        if (place.getName() == placePtr->getName()) {
                            foundPlace = true;
                        }
                    }
                    if (!foundPlace) {
                        STORM_PRINT_AND_LOG("inhibition place \"" + placePtr->getName() + "\" of transition \"" + transition->getName() + "\" was not found \n")
                        result = false;
                    }
                }

                for (auto &placePtr : transition->getOutputPlaces()) {
                    bool foundPlace = false;
                    for (auto const& place : places) {
                        if (place.getName() == placePtr->getName()) {
                            foundPlace = true;
                        }
                    }
                    if (!foundPlace) {
                        STORM_PRINT_AND_LOG("output place \"" + placePtr->getName() + "\" of transition \"" + transition->getName() + "\" was not found \n")
                        result = false;
                    }
                }
            }

            for (auto const& transition : this->getTimedTransitions()) {
                for (auto &placePtr : transition->getInputPlaces()) {
                    bool foundPlace = false;
                    for (auto const& place : places) {
                        if (place.getName() == placePtr->getName()) {
                            foundPlace = true;
                        }
                    }
                    if (!foundPlace) {
                        STORM_PRINT_AND_LOG("input place \"" + placePtr->getName() + "\" of transition \"" + transition->getName() + "\" was not found \n")
                        result = false;
                    }
                }

                for (auto &placePtr : transition->getInhibitionPlaces()) {
                    bool foundPlace = false;
                    for (auto const& place : places) {
                        if (place.getName() == placePtr->getName()) {
                            foundPlace = true;
                        }
                    }
                    if (!foundPlace) {
                        STORM_PRINT_AND_LOG("inhibition place \"" + placePtr->getName() + "\" of transition \"" + transition->getName() + "\" was not found \n")
                        result = false;
                    }
                }

                for (auto &placePtr : transition->getOutputPlaces()) {
                    bool foundPlace = false;
                    for (auto const& place : places) {
                        if (place.getName() == placePtr->getName()) {
                            foundPlace = true;
                        }
                    }
                    if (!foundPlace) {
                        STORM_PRINT_AND_LOG("output place \"" + placePtr->getName() + "\" of transition \"" + transition->getName() + "\" was not found \n")
                        result = false;
                    }
                }
            }


            return result;
        }

        void GSPN::toPnpro(std::ostream &stream) const {
            auto space = "  ";
            auto space2 = "    ";
            auto space3 = "      ";
            auto projectName = "storm-export"; // TODO add to args
            stream << "<project name=\"" << projectName << "\" version=\"121\">" << std::endl;
            stream << space << "<gspn name=\"" << getName() << "\" >" << std::endl;

            u_int32_t x = 1;
            stream << space2 << "<nodes>" << std::endl;
            for (auto& place : places) {
                stream << space3 << "<place marking=\"" << place.getNumberOfInitialTokens() <<"\" ";
                stream << "name =\"" << place.getName() << "\" ";
                stream << "x=\"" << x << "\" ";
                stream << "y=\"1\" ";
                stream << "/>" << std::endl;
                x = x + 3;
            }
            x = 1;
            for (auto& trans : timedTransitions) {
                stream << space3 << "<transition name=\"" << trans->getName() << "\" ";
                stream << "type=\"EXP\" ";
                stream << "nservers-x=\"" << trans->getRate() << "\" ";
                stream << "x=\"" << x << "\" ";
                stream << "y=\"4\" ";
                stream << "/>" << std::endl;
                x = x + 3;
            }
            for (auto& trans : immediateTransitions) {
                stream << space3 << "<transition name=\"" << trans->getName() << "\" ";
                stream << "type=\"IMM\" ";
                stream << "x=\"" << x << "\" ";
                stream << "y=\"4\" ";
                stream << "/>" << std::endl;
                x = x + 3;
            }
            stream << space2 << "</nodes>" << std::endl;

            stream << space2 << "<edges>" << std::endl;
            for (auto& trans : timedTransitions) {
                for (auto &placePtr : trans->getInputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans->getName() << "\" ";
                    stream << "tail=\"" << placePtr->getName() << "\" ";
                    stream << "kind=\"INPUT\" ";
                    stream << "mult=\"" << trans->getInputArcMultiplicity(*placePtr) << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto &placePtr : trans->getInhibitionPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans->getName() << "\" ";
                    stream << "tail=\"" << placePtr->getName() << "\" ";
                    stream << "kind=\"INHIBITOR\" ";
                    stream << "mult=\"" << trans->getInhibitionArcMultiplicity(*placePtr) << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto &placePtr : trans->getOutputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << placePtr->getName() << "\" ";
                    stream << "tail=\"" << trans->getName() << "\" ";
                    stream << "kind=\"OUTPUT\" ";
                    stream << "mult=\"" << trans->getOutputArcMultiplicity(*placePtr) << "\" ";
                    stream << "/>" << std::endl;
                }
            }
            for (auto& trans : immediateTransitions) {
                for (auto &placePtr : trans->getInputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans->getName() << "\" ";
                    stream << "tail=\"" << placePtr->getName() << "\" ";
                    stream << "kind=\"INPUT\" ";
                    stream << "mult=\"" << trans->getInputArcMultiplicity(*placePtr) << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto &placePtr : trans->getInhibitionPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << trans->getName() << "\" ";
                    stream << "tail=\"" << placePtr->getName() << "\" ";
                    stream << "kind=\"INHIBITOR\" ";
                    stream << "mult=\"" << trans->getInhibitionArcMultiplicity(*placePtr) << "\" ";
                    stream << "/>" << std::endl;
                }
                for (auto &placePtr : trans->getOutputPlaces()) {
                    stream << space3 << "<arc ";
                    stream << "head=\"" << placePtr->getName() << "\" ";
                    stream << "tail=\"" << trans->getName() << "\" ";
                    stream << "kind=\"OUTPUT\" ";
                    stream << "mult=\"" << trans->getOutputArcMultiplicity(*placePtr) << "\" ";
                    stream << "/>" << std::endl;
                }
            }
            stream << space2 << "</edges>" << std::endl;
            stream << space << "</gspn>" << std::endl;
            stream << "</project>" << std::endl;
        }

        void GSPN::toPnml(std::ostream &stream) const {
            std::string space = "  ";
            std::string space2 = "    ";
            std::string space3 = "      ";
            std::string space4 = "        ";

            stream << "<pnml>" << std::endl;
            stream << space << "<net id=\"" << getName() << "\">" << std::endl;

            // add places
            for (const auto &place : places) {
                stream << space2 << "<place id=\"" << place.getName() << "\">" << std::endl;
                stream << space3 << "<initialMarking>" << std::endl;
                stream << space4 << "<value>Default," << place.getNumberOfInitialTokens() << "</value>" << std::endl;
                stream << space3 << "</initialMarking>" << std::endl;
                stream << space2 << "</place>" << std::endl;
            }

            // add immediate transitions
            for (const auto &trans : immediateTransitions) {
                stream << space2 << "<transition id=\"" << trans->getName() << "\">" << std::endl;
                stream << space3 << "<rate>" << std::endl;
                stream << space4 << "<value>" << trans->getWeight() << "</value>" << std::endl;
                stream << space3 << "</rate>" << std::endl;
                stream << space3 << "<timed>" << std::endl;
                stream << space4 << "<value>false</value>" << std::endl;
                stream << space3 << "</timed>" << std::endl;
                stream << space2 << "</transition>" << std::endl;
            }

            // add timed transitions
            for (const auto &trans : timedTransitions) {
                stream << space2 << "<transition id=\"" << trans->getName() << "\">" << std::endl;
                stream << space3 << "<rate>" << std::endl;
                stream << space4 << "<value>" << trans->getRate() << "</value>" << std::endl;
                stream << space3 << "</rate>" << std::endl;
                stream << space3 << "<timed>" << std::endl;
                stream << space4 << "<value>true</value>" << std::endl;
                stream << space3 << "</timed>" << std::endl;
                stream << space2 << "</transition>" << std::endl;
            }

            uint_fast64_t i = 0;
            // add arcs for immediate transitions
            for (const auto &trans : immediateTransitions) {
                // add input arcs
                for (auto &placePtr : trans->getInputPlaces()) {
                    stream << space2 << "<arc ";
                    stream << "id=\"arc" << i++ << "\" ";
                    stream << "source=\"" << placePtr->getName() << "\" ";
                    stream << "target=\"" << trans->getName() << "\" ";
                    stream << ">" << std::endl;

                    stream << space3 << "<inscription>" << std::endl;
                    stream << space4 << "<value>Default," << trans->getInputArcMultiplicity(*placePtr) << "</value>" << std::endl;
                    stream << space3 << "</inscription>" << std::endl;

                    stream << space3 << "<type value=\"normal\" />" << std::endl;

                    stream << space2 << "</arc>" << std::endl;
                }

                // add inhibition arcs
                for (auto &placePtr : trans->getInhibitionPlaces()) {
                    stream << space2 << "<arc ";
                    stream << "id=\"arc" << i++ << "\" ";
                    stream << "source=\"" << placePtr->getName() << "\" ";
                    stream << "target=\"" << trans->getName() << "\" ";
                    stream << ">" << std::endl;

                    stream << space3 << "<inscription>" << std::endl;
                    stream << space4 << "<value>Default," << trans->getInputArcMultiplicity(*placePtr) << "</value>" << std::endl;
                    stream << space3 << "</inscription>" << std::endl;

                    stream << space3 << "<type value=\"inhibition\" />" << std::endl;

                    stream << space2 << "</arc>" << std::endl;
                }

                // add output arcs
                for (auto &placePtr : trans->getOutputPlaces()) {
                    stream << space2 << "<arc ";
                    stream << "id=\"arc" << i++ << "\" ";
                    stream << "source=\"" << trans->getName() << "\" ";
                    stream << "target=\"" << placePtr->getName() << "\" ";
                    stream << ">" << std::endl;

                    stream << space3 << "<inscription>" << std::endl;
                    stream << space4 << "<value>Default," << trans->getInputArcMultiplicity(*placePtr) << "</value>" << std::endl;
                    stream << space3 << "</inscription>" << std::endl;

                    stream << space3 << "<type value=\"normal\" />" << std::endl;

                    stream << space2 << "</arc>" << std::endl;
                }
            }

            stream << space << "</net>" << std::endl;
            stream << "</pnml>" << std::endl;
        }
    }
}


