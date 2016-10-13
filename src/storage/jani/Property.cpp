#include "Property.h"
namespace storm {
    namespace jani {
//        Property::Property(std::string const& name, std::string const& comment)
//        : name(name), formula(formula), comment(comment)
//        {
//
//        }

        std::string const& Property::getName() const {
            return this->name;
        }

        std::string const& Property::getComment() const {
            return this->comment;
        }
        

    }
}