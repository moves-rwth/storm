#include "GreatSpnEditorProjectParser.h"
#ifdef STORM_HAVE_XERCES

#include <iostream>
#include <algorithm>


#include "storm-gspn/adapters/XercesAdapter.h"

#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/macros.h"

namespace {
    bool isOnlyWhitespace(std::string const& in) {
        return std::all_of(in.begin(), in.end(), [](char c){
            return std::isspace(static_cast<unsigned char>(c));
        });
    }
}

namespace storm {
    namespace parser {


        storm::gspn::GSPN* GreatSpnEditorProjectParser::parse(xercesc::DOMElement const*  elementRoot) {
            if (storm::adapters::XMLtoString(elementRoot->getTagName()) == "project") {
                traverseProjectElement(elementRoot);
                return builder.buildGspn();
            } else {
                // If the top-level node is not a "pnml" or "" node, then throw an exception.
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Failed to identify the root element.\n");
            }
            
            
        }
        
        void GreatSpnEditorProjectParser::traverseProjectElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("version") == 0 ||
                           name.compare("name") == 0) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("gspn") == 0) {
                    traverseGspnElement(child);
                } else if (isOnlyWhitespace(name)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void GreatSpnEditorProjectParser::traverseGspnElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("name") == 0) {
                    builder.setGspnName(storm::adapters::XMLtoString(attr->getNodeValue()));
                } else if (name.compare("show-color-cmd") == 0 ||
                           name.compare("show-fluid-cmd") == 0) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                        "unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("nodes") == 0) {
                    traverseNodesElement(child);
                } else if (name.compare("edges") == 0) {
                    traverseEdgesElement(child);
                } else if (isOnlyWhitespace(name)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void GreatSpnEditorProjectParser::traverseNodesElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                // Found node or attribute which is at the moment not handled by this parser.
                // Notify the user and continue the parsing.
                STORM_PRINT_AND_LOG("unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("place") == 0) {
                    traversePlaceElement(child);
                } else if(name.compare("transition") == 0) {
                    traverseTransitionElement(child);
                } else if (isOnlyWhitespace(name)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void GreatSpnEditorProjectParser::traverseEdgesElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);


                // Found node or attribute which is at the moment not handled by this parser.
                // Notify the user and continue the parsing.
                STORM_PRINT_AND_LOG("unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");

            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("arc") == 0) {
                    traverseArcElement(child);
                } else if (isOnlyWhitespace(name)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        bool ignorePlaceAttribute(std::string const& name) {
            // TODO we should probably not ignore x-servers but check that it is 0.5.
            if ((name == "label-x") || (name == "label-y") || (name == "x") || (name == "y")) {
                return true;
            }
            return false;
        }


        void GreatSpnEditorProjectParser::traversePlaceElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            std::string placeName;
            uint64_t initialTokens = 0;
            
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("name") == 0) {
                    placeName = storm::adapters::XMLtoString(attr->getNodeValue());
                } else if (name.compare("marking") == 0) {
                    initialTokens = std::stoull(storm::adapters::XMLtoString(attr->getNodeValue()));
                } else if (ignorePlaceAttribute(name)) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                            "unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
                
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (isOnlyWhitespace(name)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
            builder.addPlace(-1, initialTokens, placeName);
        }

        bool ignoreTransitionAttribute(std::string const& name) {
            // TODO we should probably not ignore x-servers but check that it is 0.5.
            if ((name == "label-x") || (name == "label-y") || (name == "x") || (name == "y") || (name == "nservers-x")) {
                return true;
            }
            return false;
        }

        void GreatSpnEditorProjectParser::traverseTransitionElement(xercesc::DOMNode const* const node) {
            std::string transitionName;
            bool immediateTransition;
            double rate = 1.0; // The default rate in GreatSPN.

            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("name") == 0) {
                    transitionName = storm::adapters::XMLtoString(attr->getNodeValue());
                } else if (name.compare("type") == 0) {
                    if (storm::adapters::XMLtoString(attr->getNodeValue()).compare("EXP") == 0) {
                        immediateTransition = false;
                    } else {
                        immediateTransition = true;
                    }
                } else if(name.compare("delay") == 0) {
                    rate = std::stod(storm::adapters::XMLtoString(attr->getNodeValue()));
                } else if (ignoreTransitionAttribute(name)) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                            "unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            if (immediateTransition) {
                builder.addImmediateTransition(0, 0, transitionName);
            } else {
                builder.addTimedTransition(0, rate, transitionName);
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (isOnlyWhitespace(name)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        bool ignoreArcAttribute(std::string const& name) {
            if ((name == "mult-x") || (name == "mult-y") || (name == "mult-k")) {
                return true;
            }
            return false;
        }

        bool ignoreArcChild(std::string const& name) {
            if (name == "point") {
                return true;
            }
            return false;
        }

        void GreatSpnEditorProjectParser::traverseArcElement(xercesc::DOMNode const* const node) {
            std::string head = "____NOT_SET____";
            std::string tail = "____NOT_SET____";
            std::string kind = "____NOT_SET____";
            uint_fast64_t mult = 1;

            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("head") == 0) {
                    head = storm::adapters::XMLtoString(attr->getNodeValue());
                } else if (name.compare("tail") == 0) {
                    tail = storm::adapters::XMLtoString(attr->getNodeValue());
                } else if (name.compare("kind") == 0) {
                    kind = storm::adapters::XMLtoString(attr->getNodeValue());
                } else if (name.compare("mult") == 0) {
                    mult = std::stoull(storm::adapters::XMLtoString(attr->getNodeValue()));
                } else if (ignoreArcAttribute(name)) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                            "unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            STORM_LOG_THROW(head.compare("____NOT_SET____") != 0, storm::exceptions::WrongFormatException, "Arc must have a head");
            STORM_LOG_THROW(tail.compare("____NOT_SET____") != 0, storm::exceptions::WrongFormatException, "Arc must have a tail");
            STORM_LOG_THROW(kind.compare("____NOT_SET____") != 0, storm::exceptions::WrongFormatException, "Arc must have a kind");


            if (kind.compare("INPUT") == 0) {
                builder.addInputArc(tail, head, mult);
            } else if (kind.compare("INHIBITOR") == 0) {
                builder.addInhibitionArc(tail, head, mult);
            } else if (kind.compare("OUTPUT") == 0) {
                builder.addOutputArc(tail, head, mult);
            } else {
                // TODO error!
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (isOnlyWhitespace(name)) {
                    // ignore node (contains only whitespace)
                } else if(ignoreArcChild(name)) {
                    // ignore
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }
        
    }
}
#endif
