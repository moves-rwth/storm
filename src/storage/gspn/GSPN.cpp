#include "src/storage/gspn/GSPN.h"

namespace storm {
    namespace gspn {
        void GSPN::addImmediateTransition(storm::gspn::ImmediateTransition<WeightType> const& transition) {
            this->immediateTransitions.push_back(std::make_shared<storm::gspn::ImmediateTransition<WeightType>>(transition));
        }

        void GSPN::addTimedTransition(storm::gspn::TimedTransition<RateType> const& transition) {
            this->timedTransitions.push_back(std::make_shared<storm::gspn::TimedTransition<RateType>>(transition));
        }

        void GSPN::addPlace(Place const& place) {
            //TODO check whether the name or id is already used by an place
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

        std::pair<bool, storm::gspn::Place> GSPN::getPlace(std::string const& id) const {
            for (auto& place : places) {
                if (id.compare(place.getName()) == 0) {
                    return std::make_pair<bool, storm::gspn::Place const&>(true, place);
                }
            }
            return std::make_pair<bool, storm::gspn::Place>(false, storm::gspn::Place());
        }

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
                outStream << "\t" << trans->getName() << " [label=\"" << trans->getName();
                outStream << "(immediate:" << trans->getWeight() << ")\"];" << std::endl;
            }

            for (auto& trans : this->getTimedTransitions()) {
                outStream << "\t" << trans->getName() << " [label=\"" << trans->getName();
                outStream << "(timed:" << trans->getRate() << ")\"];" << std::endl;
            }

            // print arcs
            for (auto& trans : this->getImmediateTransitions()) {
                auto it = trans->getInputPlacesCBegin();
                while (it != trans->getInputPlacesCEnd()) {
                    outStream << "\t" << (**it).getName() << " -> " << trans->getName() << "[label=\"normal:" <<
                            trans->getInputArcMultiplicity(**it);
                    outStream << "\"];" << std::endl;

                    ++it;
                }

                it = trans->getInhibitionPlacesCBegin();
                while (it != trans->getInhibitionPlacesCEnd()) {
                    outStream << "\t" << (**it).getName() << " -> " << trans->getName() << "[label=\"inhibition:" <<
                            trans->getInhibitionArcMultiplicity(**it);
                    outStream << "\"];" << std::endl;
                    ++it;
                }

                it = trans->getOutputPlacesCBegin();
                while (it != trans->getOutputPlacesCEnd()) {
                    outStream << "\t" << trans->getName() << " -> " << (**it).getName() << "[label=\"" <<
                            trans->getOutputArcMultiplicity(**it);
                    outStream << "\"];" << std::endl;
                    ++it;
                }
            }

            for (auto& trans : this->getTimedTransitions()) {
                auto it = trans->getInputPlacesCBegin();
                while (it != trans->getInputPlacesCEnd()) {
                    outStream << "\t" << (**it).getName() << " -> " << trans->getName() << "[label=\"normal:" <<
                            trans->getInputArcMultiplicity(**it);
                    outStream << "\"];" << std::endl;
                    ++it;
                }


                it = trans->getInhibitionPlacesCBegin();
                while (it != trans->getInhibitionPlacesCEnd()) {
                    outStream << "\t" << (**it).getName() << " -> " << trans->getName() << "[label=\"inhibition:" <<
                            trans->getInhibitionArcMultiplicity(**it);
                    outStream << "\"];" << std::endl;
                    ++it;
                }

                it = trans->getOutputPlacesCBegin();
                while (it != trans->getOutputPlacesCEnd()) {
                    outStream << "\t" << trans->getName() << " -> " << (**it).getName() << "[label=\"" <<
                            trans->getOutputArcMultiplicity(**it);
                    outStream << "\"];" << std::endl;
                    ++it;
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
    }
}


