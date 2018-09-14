#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class FunctionDefinition {
        public:
            /*!
             * Creates a functionDefinition.
             */
            FunctionDefinition(std::string const& name, storm::expressions::Type const& type, std::vector<storm::expressions::Variable> const& parameters, storm::expressions::Expression const& functionBody);
            
            /*!
             * Retrieves the name of the function.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the type of the function.
             */
            storm::expressions::Type const& getType() const;
            
            /*!
             * Retrieves the parameters of the function
             */
            std::vector<storm::expressions::Variable> const& getParameters() const;
            
            /*!
             * Retrieves the expression that defines the function
             */
            storm::expressions::Expression const& getFunctionBody() const;
            
            /*!
             * sets the expression that defines the function
             */
            void setFunctionBody(storm::expressions::Expression const& body);
            
        private:
            // The name of the function.
            std::string name;
            
            // The type of the function
            storm::expressions::Type type;

            // The parameters
            std::vector<storm::expressions::Variable> parameters;
            
            // The body of the function
            storm::expressions::Expression functionBody;
        };
        
    }
}
