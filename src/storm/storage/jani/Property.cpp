#include "Property.h"
namespace storm {
    namespace jani {
        
        
        
        std::ostream& operator<<(std::ostream& os, FilterExpression const& fe) {
            return os << "Obtain " << toString(fe.getFilterType()) << " the 'initial'-states with values described by '" << *fe.getFormula() << "'";
        }
        
        Property::Property(std::string const& name, std::shared_ptr<storm::logic::Formula const> const& formula, std::string const& comment)
        : name(name), comment(comment), filterExpression(FilterExpression(formula))
        {

        }
        
        Property::Property(std::string const& name, FilterExpression const& fe, std::string const& comment)
        : name(name), comment(comment), filterExpression(fe)
        {
            
        }

        std::string const& Property::getName() const {
            return this->name;
        }

        std::string const& Property::getComment() const {
            return this->comment;
        }
        
        FilterExpression const& Property::getFilter() const {
            return this->filterExpression;
        }
        
        std::ostream& operator<<(std::ostream& os, Property const& p) {
            return os << "(" << p.getName() << ") : " << p.getFilter();
        }
        

    }
}
