#include "Property.h"
namespace storm {
    namespace jani {
        
        
        
        std::ostream& operator<<(std::ostream& os, FilterExpression const& fe) {
            return os << "Obtain " << toString(fe.getFilterType()) << " of the '" << fe.getStatesFormula() << "'-states with values described by '" << *fe.getFormula() << "'";
        }
        
        Property::Property(std::string const& name, std::shared_ptr<storm::logic::Formula const> const& formula, std::string const& comment)
        : name(name), comment(comment), filterExpression(FilterExpression(formula)) {
            // Intentionally left empty.
        }
        
        Property::Property(std::string const& name, FilterExpression const& fe, std::string const& comment)
        : name(name), comment(comment), filterExpression(fe) {
            // Intentionally left empty.
        }

        std::string const& Property::getName() const {
            return this->name;
        }

        std::string const& Property::getComment() const {
            return this->comment;
        }
        
        Property Property::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return Property(name, filterExpression.substitute(substitution), comment);
        }
        
        Property Property::substituteLabels(std::map<std::string, std::string> const& substitution) const {
            return Property(name, filterExpression.substituteLabels(substitution), comment);
        }
        
        FilterExpression const& Property::getFilter() const {
            return this->filterExpression;
        }
        
        std::shared_ptr<storm::logic::Formula const> Property::getRawFormula() const {
            return this->filterExpression.getFormula();
        }
        
        std::ostream& operator<<(std::ostream& os, Property const& p) {
            return os << "(" << p.getName() << ") : " << p.getFilter();
        }
        
    }
}
