#pragma once

#include "storm-config.h"
#ifdef STORM_HAVE_XERCES
#include <string>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLString.hpp>

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm-parsers/parser/ExpressionParser.h"

#include "storm-gspn/storage/gspn/GSPN.h"

#include "storm-gspn/storage/gspn/GspnBuilder.h"

namespace storm {
    namespace parser {
        class GreatSpnEditorProjectParser {
            
        public:
            
            GreatSpnEditorProjectParser(std::string const& constantDefinitionString);
            
            /*!
             * Parses the given file into the GSPN.
             *
             * @param filename The name of the file to parse.
             * @return The resulting GSPN.
             */
            storm::gspn::GSPN* parse(xercesc::DOMElement const*  elementRoot);
        private:
            void traverseProjectElement(xercesc::DOMNode const* const node);
            
            void traverseGspnElement(xercesc::DOMNode const* const node);
            void traverseNodesElement(xercesc::DOMNode const* const node);
            void traverseEdgesElement(xercesc::DOMNode const* const node);
            
            void traverseConstantElement(xercesc::DOMNode const* const node, std::unordered_map<std::string, storm::expressions::Expression>& identifierMapping);
            void traversePlaceElement(xercesc::DOMNode const* const node);
            void traverseTransitionElement(xercesc::DOMNode const* const node);
            void traverseArcElement(xercesc::DOMNode const* const node);
            
            
            // the constructed gspn
            storm::gspn::GspnBuilder builder;
            std::shared_ptr<storm::expressions::ExpressionManager> manager;
            storm::parser::ExpressionParser expressionParser;
            std::unordered_map<std::string, std::string> constantDefinitions;
        };
    }
}
#endif
