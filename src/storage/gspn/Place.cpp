#include "Place.h"

#include "src/exceptions/IllegalArgumentValueException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace gspn {
        void Place::setName(std::string const& name) {
            this->name = name;
        }

        std::string Place::getName() const {
            return this->name;
        }

        void Place::setID(uint_fast64_t const& id) {
            this->id = id;
        }

        uint_fast64_t Place::getID() const {
            return this->id;
        }

        void Place::setNumberOfInitialTokens(uint_fast64_t const& tokens) {
            this->numberOfInitialTokens = tokens;
        }

        uint_fast64_t Place::getNumberOfInitialTokens() const {
            return this->numberOfInitialTokens;
        }

        void Place::setCapacity(uint64_t cap) {
            std::cout << this->name << std::endl;
            this->capacity = cap;
        }

        uint64_t Place::getCapacity() const {
            return capacity.get();
        }
        
        bool Place::hasRestrictedCapacity() const {
            return capacity != boost::none;
        }
    }
}