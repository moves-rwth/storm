#ifndef STORM_UTILITY_PRISM_H_
#define STORM_UTILITY_PRISM_H_

#include <map>
#include <set>
#include <vector>

#include <boost/optional.hpp>

namespace storm {
    namespace expressions {
        class Variable;
        class Expression;
    }
    
    namespace prism {
        class Program;
    }
    
    namespace utility {
        namespace prism {
            
            template<typename ValueType>
            storm::prism::Program preprocessProgram(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions = boost::none, boost::optional<std::set<std::string>> const& restrictedLabelSet = boost::none, boost::optional<std::vector<storm::expressions::Expression>> const& expressionLabels = boost::none);
            
            std::map<storm::expressions::Variable, storm::expressions::Expression> parseConstantDefinitionString(storm::prism::Program const& program, std::string const& constantDefinitionString);
            
        } // namespace prism
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_PRISM_H_ */
