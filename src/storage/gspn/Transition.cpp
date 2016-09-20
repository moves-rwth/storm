#include "src/storage/gspn/Transition.h"

#include "src/utility/macros.h"

namespace storm {
    namespace gspn {

        void Transition::setInputArcMultiplicity(storm::gspn::Place const& place, uint_fast64_t multiplicity) {
            auto ptr = std::make_shared<storm::gspn::Place>(place);
            inputMultiplicities[ptr] = multiplicity;
            inputPlaces.push_back(ptr);
        }

        bool Transition::removeInputArc(storm::gspn::Place const& place) {
            if (existsInputArc(place)) {
                auto ptr = std::make_shared<storm::gspn::Place>(place);
                inputMultiplicities.erase(ptr);
                inputPlaces.erase(std::find(inputPlaces.begin(), inputPlaces.end(), ptr));
                return true;
            } else {
                return false;
            }
        }

        bool Transition::existsInputArc(storm::gspn::Place const& place) const {
            auto ptr = std::make_shared<storm::gspn::Place>(place);
            return inputMultiplicities.end() != inputMultiplicities.find(ptr);
        }

        void Transition::setOutputArcMultiplicity(storm::gspn::Place const& place, uint_fast64_t multiplicity) {
            auto ptr = std::make_shared<storm::gspn::Place>(place);
            outputMultiplicities[ptr] = multiplicity;
            outputPlaces.push_back(ptr);
        }

        bool Transition::removeOutputArc(storm::gspn::Place const& place) {
            if (existsOutputArc(place)) {
                auto ptr = std::make_shared<storm::gspn::Place>(place);
                outputMultiplicities.erase(ptr);
                outputPlaces.erase(std::find(outputPlaces.begin(), outputPlaces.end(), ptr));
                return true;
            } else {
                return false;
            }
        }

        bool Transition::existsOutputArc(storm::gspn::Place const& place) const {
            auto ptr = std::make_shared<storm::gspn::Place>(place);
            return outputMultiplicities.end() != outputMultiplicities.find(ptr);
        }

        void Transition::setInhibitionArcMultiplicity(storm::gspn::Place const& place,
                                                      uint_fast64_t multiplicity) {
            auto ptr = std::make_shared<storm::gspn::Place>(place);
            inhibitionMultiplicities[ptr] = multiplicity;
            inhibitionPlaces.push_back(ptr);
        }

        bool Transition::removeInhibitionArc(storm::gspn::Place const& place) {
            if (existsInhibitionArc(place)) {
                auto ptr = std::make_shared<storm::gspn::Place>(place);
                inhibitionMultiplicities.erase(ptr);
                inhibitionPlaces.erase(std::find(inhibitionPlaces.begin(), inhibitionPlaces.end(), ptr));
                return true;
            } else {
                return false;
            }
        }

        bool Transition::existsInhibitionArc(storm::gspn::Place const& place) const {
            auto ptr = std::make_shared<storm::gspn::Place>(place);
            return inhibitionMultiplicities.end() != inhibitionMultiplicities.find(ptr);
        }

        bool Transition::isEnabled(storm::gspn::Marking const& marking) const {
            auto inputIterator = inputMultiplicities.cbegin();
            while (inputIterator != inputMultiplicities.cend()) {
                if (marking.getNumberOfTokensAt(inputIterator->first->getID()) < inputIterator->second) {
                    return false;
                }

                ++inputIterator;
            }

            auto inhibitionIterator = inhibitionMultiplicities.cbegin();
            while (inhibitionIterator != inhibitionMultiplicities.cend()) {
                if (marking.getNumberOfTokensAt(inhibitionIterator->first->getID()) >= inhibitionIterator->second) {
                    return false;
                }

                ++inhibitionIterator;
            }

            auto outputIterator = outputMultiplicities.cbegin();
            while (outputIterator != outputMultiplicities.cend()) {
                if (outputIterator->first->getCapacity() >= 0) {
                    if (marking.getNumberOfTokensAt(outputIterator->first->getID()) + outputIterator->second > outputIterator->first->getCapacity()) {
                        return false;
                    }
                }

                ++outputIterator;
            }

            return true;
        }

        storm::gspn::Marking Transition::fire(storm::gspn::Marking const& marking) const {
            storm::gspn::Marking newMarking(marking);

            auto inputIterator = inputMultiplicities.cbegin();
            while (inputIterator != inputMultiplicities.cend()) {
                newMarking.setNumberOfTokensAt(inputIterator->first->getID(),
                                               newMarking.getNumberOfTokensAt(inputIterator->first->getID()) - inputIterator->second);
                ++inputIterator;
            }

            auto outputIterator = outputMultiplicities.cbegin();
            while (outputIterator != outputMultiplicities.cend()) {
                newMarking.setNumberOfTokensAt(outputIterator->first->getID(),
                                               newMarking.getNumberOfTokensAt(outputIterator->first->getID()) + outputIterator->second);
                ++outputIterator;
            }

            return newMarking;
        }

        void Transition::setName(std::string const& name) {
            this->name = name;
        }

        std::string const& Transition::getName() const {
            return this->name;
        }

        std::vector<std::shared_ptr<storm::gspn::Place>>::const_iterator  Transition::getInputPlacesCBegin() const {
            return this->inputPlaces.cbegin();
        }

        std::vector<std::shared_ptr<storm::gspn::Place>>::const_iterator Transition::getInputPlacesCEnd() const {
            return this->inputPlaces.cend();
        }

        std::vector<std::shared_ptr<storm::gspn::Place>>::const_iterator Transition::getOutputPlacesCBegin() const {
            return this->outputPlaces.cbegin();
        }

        std::vector<std::shared_ptr<storm::gspn::Place>>::const_iterator Transition::getOutputPlacesCEnd() const {
            return this->outputPlaces.cend();
        }

        std::vector<std::shared_ptr<storm::gspn::Place>>::const_iterator Transition::getInhibitionPlacesCBegin() const {
            return this->inhibitionPlaces.cbegin();
        }

        std::vector<std::shared_ptr<storm::gspn::Place>>::const_iterator Transition::getInhibitionPlacesCEnd() const {
            return this->inhibitionPlaces.cend();
        }

        uint_fast64_t Transition::getInputArcMultiplicity(storm::gspn::Place const& place) const {
            if (existsInputArc(place)) {
                auto ptr = std::make_shared<storm::gspn::Place>(place);
                return inputMultiplicities.at(ptr);
            } else {
                return 0;
            }
        }

        uint_fast64_t Transition::getInhibitionArcMultiplicity(storm::gspn::Place const& place) const {
            if (existsInhibitionArc(place)) {
                auto ptr = std::make_shared<storm::gspn::Place>(place);
                return inhibitionMultiplicities.at(ptr);
            } else {
                return 0;
            }
        }

        uint_fast64_t Transition::getOutputArcMultiplicity(storm::gspn::Place const& place) const {
            if (existsOutputArc(place)) {
                auto ptr = std::make_shared<storm::gspn::Place>(place);
                return outputMultiplicities.at(ptr);
            } else {
                return 0;
            }
        }

        void Transition::setPriority(uint_fast64_t const& priority) {
            this->priority = priority;
        }

        uint_fast64_t Transition::getPriority() const {
            return this->priority;
        }
    }
}
