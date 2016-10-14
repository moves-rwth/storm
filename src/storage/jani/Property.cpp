#include "Property.h"
namespace storm {
    namespace jani {
        Property::Property(std::string const& name, std::shared_ptr<storm::logic::Formula const> const& formula, std::string const& comment)
        : name(name), filterExpression(FilterExpression(formula)), comment(comment)
        {

        }

        std::string const& Property::getName() const {
            return this->name;
        }

        std::string const& Property::getComment() const {
            return this->comment;
        }
        

    }
}