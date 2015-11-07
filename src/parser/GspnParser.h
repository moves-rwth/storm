#ifndef STORM_GSPNPARSER_H
#define STORM_GSPNPARSER_H

#include <iostream>
#include <string>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include "src/storage/gspn/GSPN.h"

namespace storm {
    namespace parser {
        // Parses a GSPN in xml format
        class GspnParser {
        public:
            /*!
             * Parses the given file into the GSPN storage class assuming it complies with the PNML.
             *
             * @param filename The name of the file to parse
             * @return The resulting GSPN.
             */
            storm::gspn::GSPN parse(std::string const& filename);

            /*!
             * Transforms the given XML String to a normal string.
             *
             * @param xmlString The given String in the XML format
             * @return The corresponding standard string.
             */
            static std::string XMLtoString(const XMLCh* xmlString);
        private:
            // maps the original name of the state to its numerical representation
            std::map<std::string,uint64_t> stringToState;

            // the constructed gspn
            storm::gspn::GSPN gspn;

            // has the new id for a new node
            uint64_t newNode;

            /*!
             * Parses the root element (TagName = pnml).
             *
             * @param element The root element.
             */
            void parsePNML(xercesc::DOMElement* element);

            /*!
             * Parses a net node (NodeName = net).
             *
             * @param node The net node.
             */
            void parseNet(xercesc::DOMNode* node);

            /*!
             * Parses a page node (NodeName = page).
             *
             * @param node The page node.
a            */
            void parsePage(xercesc::DOMNode* node);

            /*!
             * Parses a place node (NodeName = place).
             *
             * @param node The place node.
             */
            void parsePlace(xercesc::DOMNode* node);

            /*!
             * Parses a transition node (NodeName = transition).
             *
             * @param node The transition node.
             */
            void parseTransition(xercesc::DOMNode* node);

            /*!
             * Parses an arc node (NodeName = arc).
             *
             * @param node The arc node.
             */
            void parseArc(xercesc::DOMNode* node);

            /*!
             * Parses an initial marking Node (Nodename = initialMarking)
             *
             * @param node the initial marking node.
             * @return The number of tokens.
             */
            uint64_t parseInitialMarking(xercesc::DOMNode* node);

            /*!
             * Adds a new entry in the mapping from string to places.
             *
             * @param id The string id for the new place
             * @return The new place.
             */
            uint64_t addNewPlace(std::string id);

            /*!
             * Gives the name of the current node.
             * @param node Current node.
             * @return The name.
             */
            std::string getName(xercesc::DOMNode* node);
        };
    }
}

#endif //STORM_GSPNPARSER_H
