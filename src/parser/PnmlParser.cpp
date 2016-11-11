#include "src/parser/PnmlParser.h"

#include <iostream>

#include "src/adapters/XercesAdapter.h"

#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace parser {
        storm::gspn::GSPN * PnmlParser::parse(xercesc::DOMElement const*  elementRoot ) {
            if (storm::adapters::getName(elementRoot) == "pnml") {
                traversePnmlElement(elementRoot);
            } else {
                // If the top-level node is not a "pnml" or "" node, then throw an exception.
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Failed to identify the root element.\n");
            }
            return builder.buildGspn();
        }

        void PnmlParser::traversePnmlElement(xercesc::DOMElement const* const element) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < element->getAttributes()->getLength(); ++i) {
                auto attr = element->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                // Found node or attribute which is at the moment nod handled by this parser.
                // Notify the user and continue the parsing.
                STORM_PRINT_AND_LOG("unknown attribute (node=pnml): " + name + "\n");
            }

            // traverse children
            for (uint_fast64_t i = 0; i < element->getChildNodes()->getLength(); ++i) {
                auto child = element->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("net") == 0) {
                    traverseNetOrPage(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=pnml): " + name + "\n");
                }
            }
        }

        void PnmlParser::traverseNetOrPage(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("id") == 0) {
                    builder.setGspnName(storm::adapters::XMLtoString(attr->getNodeValue()));
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("place") == 0) {
                    traversePlace(child);
                } else if (name.compare("transition") == 0) {
                    traverseTransition(child);
                } else if (name.compare("arc") == 0) {
                    traverseArc(child);
                } else if (name.compare("page") == 0) {
                    // Some pnml files have a child named page.
                    // The page node has the same children like the net node (e.g., place, transition, arc)
                    traverseNetOrPage(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + storm::adapters::XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void PnmlParser::traversePlace(xercesc::DOMNode const* const node) {
            std::string placeName;
            // the first entry is false if the corresponding information was not found in the pnml file
            std::pair<bool, uint_fast64_t> numberOfInitialTokens(false, defaultNumberOfInitialTokens);
            std::pair<bool, int_fast64_t> capacity(false, defaultCapacity);

            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("id") == 0) {
                    placeName = storm::adapters::XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=place): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("initialMarking") == 0) {
                    numberOfInitialTokens.first = true;
                    numberOfInitialTokens.second = traverseInitialMarking(child);
                } else if(name.compare("capacity") == 0) {
                    capacity.first = true;
                    capacity.second = traverseCapacity(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else if (name.compare("name") == 0 ||
                           name.compare("graphics") == 0) {
                    // ignore these tags
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=place): " + name + "\n");
                }
            }

            if (!numberOfInitialTokens.first) {
                // no information about the number of initial tokens is found
                // use the default number of initial tokens
                STORM_PRINT_AND_LOG("unknown numberOfInitialTokens (place=" + placeName + ")\n");
            }
            if (!capacity.first) {
                // no information about the capacity is found
                // use default capacity
                STORM_PRINT_AND_LOG("unknown capacity (place=" + placeName + ")\n");
            }
            builder.addPlace(capacity.first ? capacity.second : -1, numberOfInitialTokens.first ? numberOfInitialTokens.second : 0, placeName);
        }

        void PnmlParser::traverseTransition(xercesc::DOMNode const* const node) {
            // the first entry is false if the corresponding information was not found in the pnml file
            std::pair<bool, bool> timed(false, defaultTransitionType);
            std::pair<bool, std::string> value(false, "");
            std::string id;
            uint_fast64_t priority = defaultPriority;

            // parse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("id") == 0) {
                    id = storm::adapters::XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=transition): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);

                if (name.compare("rate") == 0) {
                    value.first = true;
                    value.second = traverseTransitionValue(child);
                } else if (name.compare("timed") == 0) {
                    timed.first = true;
                    timed.second = traverseTransitionType(child);
                } else if (name.compare("priority") == 0) {
                    priority = traversePriority(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else if (name.compare("graphics") == 0 ||
                           name.compare("name") == 0 ||
                           name.compare("orientation") == 0) {
                    // ignore these tags
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=transition): " + name + "\n");
                }
            }

            // build transition and add it to the gspn
            if (!timed.first) {
                // found unknown transition type
                // abort parsing
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "unknown transition type (transition=" + id + ")\n");
                return;
            }

            if (timed.second) {
                if (!value.first) {
                    // no information about the rate is found
                    // abort the parsing
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown transition rate (transition=" + id + ")\n");
                }
                builder.addTimedTransition(priority, std::stod(value.second), id);
            } else {
                if (!value.first) {
                    // no information about the weight is found
                    // continue with the default weight
                    STORM_PRINT_AND_LOG("unknown transition weight (transition=" + id + ")\n");
                }
                builder.addImmediateTransition(priority, std::stod(value.second), id);
            }
        }

        void PnmlParser::traverseArc(xercesc::DOMNode const* const node) {
            // the first entry is false if the corresponding information was not found in the pnml file
            std::pair<bool, std::string> source(false, "");
            std::pair<bool, std::string> target(false, "");
            std::pair<bool, std::string> type(false, defaultArcType);
            std::pair<bool, uint_fast64_t> multiplicity(false, defaultMultiplicity);
            std::string id;

            // parse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);

                if (name.compare("source") == 0) {
                    source.first = true;
                    source.second = storm::adapters::XMLtoString(attr->getNodeValue());
                } else if (name.compare("target") == 0) {
                    target.first = true;
                    target.second = storm::adapters::XMLtoString(attr->getNodeValue());
                } else if (name.compare("id") == 0) {
                    id = storm::adapters::XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=arc): " + name + "\n");
                }
            }

            // parse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);
                if (name.compare("type") == 0) {
                    type.first = true;
                    type.second = traverseArcType(child);
                } else if(name.compare("inscription") == 0) {
                    multiplicity.first = true;
                    multiplicity.second = traverseMultiplicity(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else if (name.compare("graphics") == 0 ||
                           name.compare("arcpath") == 0 ||
                           name.compare("tagged") == 0) {
                    // ignore these tags
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=arc): " + name + "\n");
                }
            }

            // check if all necessary information where stored in the pnml file
            if (!source.first) {
                // could not find start of the arc
                // abort parsing
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown arc source (arc=" + id + ")\n");
            }
            if (!target.first) {
                // could not find the target of the arc
                // abort parsing
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown arc target (arc=" + id + ")\n");
            }
            if (!multiplicity.first) {
                // no information about the multiplicity of the arc
                // continue and use the default multiplicity
                STORM_PRINT_AND_LOG("unknown multiplicity (node=arc): " + id + "\n");
            }

            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "No arc type specified for arc '" + id + "'");
            if (type.second == "normal") {
                builder.addNormalArc(source.second, target.second, multiplicity.second);
            } else if (type.second == "inhibition") {
                builder.addInhibitionArc(source.second, target.second, multiplicity.second);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Arc type '" << type.second << "' in arc '"  << id << "' is unknown.");
            }
        }

        uint_fast64_t PnmlParser::traverseInitialMarking(xercesc::DOMNode const* const node) {
            uint_fast64_t result = defaultNumberOfInitialTokens;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);
                if (name.compare("text") == 0) {
                    result = std::stoull(storm::adapters::getName(child->getFirstChild()));
                } else if (name.compare("value") == 0) {
                    auto value = storm::adapters::getName(child->getFirstChild());
                    value = value.substr(std::string("Default,").length());
                    result = std::stoull(value);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else if (name.compare("graphics") == 0) {
                    // ignore these tags
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=initialMarking): " + name + "\n");
                }
            }
            return result;
        }

        int_fast64_t PnmlParser::traverseCapacity(xercesc::DOMNode const* const node) {
            int_fast64_t result= defaultCapacity;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);
                if (name.compare("value") == 0) {
                    auto value = storm::adapters::getName(child->getFirstChild());
                    if (value.find("Default,") == 0) {
                        value = value.substr(std::string("Default,").length());
                    }
                    result = std::stoull(value);
                } else if (name.compare("graphics") == 0) {
                    // ignore these nodes
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=capacity): " + name + "\n");
                }
            }
            return result;
        }

        uint_fast64_t PnmlParser::traverseMultiplicity(xercesc::DOMNode const* const node) {
            uint_fast64_t result = defaultMultiplicity;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);
                if (name.compare("value") == 0) {
                    auto value = storm::adapters::getName(child->getFirstChild());
                    if (value.find("Default,") == 0) {
                        value = value.substr(std::string("Default,").length());
                    }
                    result = std::stoull(value);
                } else if (name.compare("graphics") == 0) {
                    // ignore these nodes
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=inscription): " + name + "\n");
                }
            }
            return result;
        }


        std::string PnmlParser::traverseTransitionValue(xercesc::DOMNode const* const node) {
            std::string result;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);
                if (name.compare("value") == 0) {
                    result = storm::adapters::getName(child->getFirstChild());
                } else  if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=rate): " + name + "\n");
                }
            }
            return result;
        }

        bool PnmlParser::traverseTransitionType(xercesc::DOMNode const* const node) {
            bool result;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);
                if (name.compare("value") == 0) {
                    result = storm::adapters::getName(child->getFirstChild()).compare("true") == 0 ? true : false;
                } else  if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=timed): " + name + "\n");
                }
            }
            return result;
        }

        std::string PnmlParser::traverseArcType(xercesc::DOMNode const* const node) {
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = storm::adapters::getName(attr);
                if (name.compare("value") == 0) {
                    return storm::adapters::XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=type): " + name + "\n");
                }
            }
            return defaultArcType;
        }

        uint_fast64_t PnmlParser::traversePriority(xercesc::DOMNode const* const node) {
            uint_fast64_t result = defaultPriority;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = storm::adapters::getName(child);
                if (name.compare("text") == 0) {
                    result = std::stoull(storm::adapters::getName(child->getFirstChild()));
                } else if (name.compare("value") == 0) {
                    auto value = storm::adapters::getName(child->getFirstChild());
                    value = value.substr(std::string("Default,").length());
                    result = std::stoull(value);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else if (name.compare("graphics") == 0) {
                    // ignore these tags
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=priority): " + name + "\n");
                }
            }
            return result;
        }
    }
}

