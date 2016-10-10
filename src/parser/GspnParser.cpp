#include "src/parser/GspnParser.h"

#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "src/exceptions/UnexpectedException.h"
#include "src/exceptions/WrongFormatException.h"
#include "src/storage/gspn/Place.h"
#include "src/storage/gspn/ImmediateTransition.h"
#include "src/utility/macros.h"

namespace storm {
    namespace parser {
        storm::gspn::GSPN const& GspnParser::parse(std::string const& filename) {
            // initialize parser
            newNode = 0;
            gspn = storm::gspn::GSPN();

            // initialize xercesc
            try {
                xercesc::XMLPlatformUtils::Initialize();
            }
            catch (xercesc::XMLException const& toCatch) {
                // Error occurred during the initialization process. Abort parsing since it is not possible.
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Failed to initialize xercesc\n");
            }

            auto parser = new xercesc::XercesDOMParser();
            parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
            parser->setDoNamespaces(false);
            parser->setDoSchema(false);
            parser->setLoadExternalDTD(false);
            parser->setIncludeIgnorableWhitespace(false);

            auto errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
            parser->setErrorHandler(errHandler);

            // parse file
            try {
                parser->parse(filename.c_str());
            }
            catch (xercesc::XMLException const& toCatch) {
                auto message = xercesc::XMLString::transcode(toCatch.getMessage());
                // Error occurred while parsing the file. Abort constructing the gspn since the input file is not valid
                // or the parser run into a problem.
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, message);
                xercesc::XMLString::release(&message);
            }
            catch (const xercesc::DOMException& toCatch) {
                auto message = xercesc::XMLString::transcode(toCatch.msg);
                // Error occurred while parsing the file. Abort constructing the gspn since the input file is not valid
                // or the parser run into a problem.
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, message);
                xercesc::XMLString::release(&message);
            }
            catch (...) {
                // Error occurred while parsing the file. Abort constructing the gspn since the input file is not valid
                // or the parser run into a problem.
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Failed to parse pnml file.\n");
            }

            // build gspn by traversing the DOM object
            parser->getDocument()->normalizeDocument();
            xercesc::DOMElement* elementRoot = parser->getDocument()->getDocumentElement();

            if (getName(elementRoot).compare("pnml") == 0) {
                traversePnmlElement(elementRoot);
            } else if (getName(elementRoot).compare("project") == 0) {
                traverseProjectElement(elementRoot);
            } else {
                // If the top-level node is not a "pnml" or "" node, then throw an exception.
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Failed to identify the root element.\n");
            }





            // clean up
            delete parser;
            delete errHandler;
            xercesc::XMLPlatformUtils::Terminate();

            return gspn;
        }

        void GspnParser::traversePnmlElement(xercesc::DOMElement const* const element) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < element->getAttributes()->getLength(); ++i) {
                auto attr = element->getAttributes()->item(i);
                auto name = getName(attr);

                // Found node or attribute which is at the moment nod handled by this parser.
                // Notify the user and continue the parsing.
                STORM_PRINT_AND_LOG("unknown attribute (node=pnml): " + name + "\n");
            }

            // traverse children
            for (uint_fast64_t i = 0; i < element->getChildNodes()->getLength(); ++i) {
                auto child = element->getChildNodes()->item(i);
                auto name = getName(child);

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

        void GspnParser::traverseNetOrPage(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("id") == 0) {
                    gspn.setName(XMLtoString(attr->getNodeValue()));
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

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
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void GspnParser::traversePlace(xercesc::DOMNode const* const node) {
            std::string placeName;
            // the first entry is false if the corresponding information was not found in the pnml file
            std::pair<bool, uint_fast64_t> numberOfInitialTokens(false, defaultNumberOfInitialTokens);
            std::pair<bool, int_fast64_t> capacity(false, defaultCapacity);

            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("id") == 0) {
                    placeName = XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=place): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

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

            // build place and add it to the gspn
            storm::gspn::Place place;
            place.setName(placeName);
            if (!numberOfInitialTokens.first) {
                // no information about the number of initial tokens is found
                // use the default number of initial tokens
                STORM_PRINT_AND_LOG("unknown numberOfInitialTokens (place=" + placeName + ")\n");
            }
            place.setNumberOfInitialTokens(numberOfInitialTokens.second);
            if (!capacity.first) {
                // no information about the capacity is found
                // use default capacity
                STORM_PRINT_AND_LOG("unknown capacity (place=" + placeName + ")\n");
            }
            place.setCapacity(capacity.second);
            place.setID(newNode);
            ++newNode;
            gspn.addPlace(place);
        }

        void GspnParser::traverseTransition(xercesc::DOMNode const* const node) {
            // the first entry is false if the corresponding information was not found in the pnml file
            std::pair<bool, bool> timed(false, defaultTransitionType);
            std::pair<bool, std::string> value(false, defaultTransitionValue);
            std::string id;
            uint_fast64_t priority = defaultPriority;

            // parse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("id") == 0) {
                    id = XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=transition): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

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
                storm::gspn::TimedTransition<storm::gspn::GSPN::RateType> transition;
                if (!value.first) {
                    // no information about the rate is found
                    // abort the parsing
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown transition rate (transition=" + id + ")\n");
                }
                transition.setRate(std::stod(value.second));
                transition.setName(id);
                transition.setPriority(priority);
                gspn.addTimedTransition(transition);
            } else {
                storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> transition;
                if (!value.first) {
                    // no information about the weight is found
                    // continue with the default weight
                    STORM_PRINT_AND_LOG("unknown transition weight (transition=" + id + ")\n");
                }
                transition.setWeight(std::stod(value.second));
                transition.setName(id);
                transition.setPriority(priority);
                gspn.addImmediateTransition(transition);
            }
        }

        void GspnParser::traverseArc(xercesc::DOMNode const* const node) {
            // the first entry is false if the corresponding information was not found in the pnml file
            std::pair<bool, std::string> source(false, "");
            std::pair<bool, std::string> target(false, "");
            std::pair<bool, std::string> type(false, defaultArcType);
            std::pair<bool, uint_fast64_t> multiplicity(false, defaultMultiplicity);
            std::string id;

            // parse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("source") == 0) {
                    source.first = true;
                    source.second = XMLtoString(attr->getNodeValue());
                } else if (name.compare("target") == 0) {
                    target.first = true;
                    target.second = XMLtoString(attr->getNodeValue());
                } else if (name.compare("id") == 0) {
                    id = XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=arc): " + name + "\n");
                }
            }

            // parse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);
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

            //determine if it is an outgoing or incoming arc
            {
                auto place = gspn.getPlace(source.second);
                auto trans = gspn.getTransition(target.second);
                if (true == place.first && true == trans.first) {
                    if (!type.first) {
                        // no arc type found
                        // abport parsing
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "unknown arc type (arc=" + id + ")\n");
                    }

                    // incoming arc
                    if (type.second.compare("normal") == 0) {
                        trans.second->setInputArcMultiplicity(place.second, multiplicity.second);
                    } else if (type.second.compare("inhibition") == 0) {
                        trans.second->setInhibitionArcMultiplicity(place.second, multiplicity.second);
                    } else {
                        // unknown arc type
                        // abort parsing
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "unknown arc type (arc=" + id + ")\n");
                    }
                    return;
                }
            }
            {
                auto trans = gspn.getTransition(source.second);
                auto place = gspn.getPlace(target.second);
                if (true == place.first && true == trans.first) {
                    // outgoing arc
                    trans.second->setOutputArcMultiplicity(place.second, multiplicity.second);
                    return;
                }
            }

            // if we reach this line we could not find the corresponding place or transition
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown arc source or target (arc=" + id + ")\n");
        }

        std::string GspnParser::getName(xercesc::DOMNode *node) {
            switch (node->getNodeType()) {
                case xercesc::DOMNode::NodeType::ELEMENT_NODE: {
                    auto elementNode = (xercesc::DOMElement *) node;
                    return XMLtoString(elementNode->getTagName());
                }
                case xercesc::DOMNode::NodeType::TEXT_NODE: {
                    return XMLtoString(node->getNodeValue());
                }
                case xercesc::DOMNode::NodeType::ATTRIBUTE_NODE: {
                    return XMLtoString(node->getNodeName());
                }
                default: {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown node type \n");
                    return "";
                }
            }
        }

        uint_fast64_t GspnParser::traverseInitialMarking(xercesc::DOMNode const* const node) {
            uint_fast64_t result = defaultNumberOfInitialTokens;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);
                if (name.compare("text") == 0) {
                    result = std::stoull(getName(child->getFirstChild()));
                } else if (name.compare("value") == 0) {
                    auto value = getName(child->getFirstChild());
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

        int_fast64_t GspnParser::traverseCapacity(xercesc::DOMNode const* const node) {
            int_fast64_t result= defaultCapacity;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);
                if (name.compare("value") == 0) {
                    auto value = getName(child->getFirstChild());
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

        uint_fast64_t GspnParser::traverseMultiplicity(xercesc::DOMNode const* const node) {
            uint_fast64_t result = defaultMultiplicity;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);
                if (name.compare("value") == 0) {
                    auto value = getName(child->getFirstChild());
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


        std::string GspnParser::traverseTransitionValue(xercesc::DOMNode const* const node) {
            std::string result = defaultTransitionValue;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);
                if (name.compare("value") == 0) {
                    result = getName(child->getFirstChild());
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

        bool GspnParser::traverseTransitionType(xercesc::DOMNode const* const node) {
            bool result;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);
                if (name.compare("value") == 0) {
                    result = getName(child->getFirstChild()).compare("true") == 0 ? true : false;
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

        std::string GspnParser::traverseArcType(xercesc::DOMNode const* const node) {
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);
                if (name.compare("value") == 0) {
                    return XMLtoString(attr->getNodeValue());
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=type): " + name + "\n");
                }
            }
            return defaultArcType;
        }

        uint_fast64_t GspnParser::traversePriority(xercesc::DOMNode const* const node) {
            uint_fast64_t result = defaultPriority;
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);
                if (name.compare("text") == 0) {
                    result = std::stoull(getName(child->getFirstChild()));
                } else if (name.compare("value") == 0) {
                    auto value = getName(child->getFirstChild());
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

        std::string GspnParser::XMLtoString(const XMLCh *xmlString) {
            char* tmp = xercesc::XMLString::transcode(xmlString);
            auto result = std::string(tmp);
            delete tmp;
            return result;
        }

        void GspnParser::traverseProjectElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("version") == 0 ||
                           name.compare("name") == 0) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (name.compare("gspn") == 0) {
                    traverseGspnElement(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void GspnParser::traverseGspnElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("name") == 0) {
                    gspn.setName(XMLtoString(attr->getNodeValue()));
                } else if (name.compare("show-color-cmd") == 0 ||
                           name.compare("show-fluid-cmd") == 0) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                        "unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (name.compare("nodes") == 0) {
                    traverseNodesElement(child);
                } else if (name.compare("edges") == 0) {
                    traverseEdgesElement(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void GspnParser::traverseNodesElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                // Found node or attribute which is at the moment not handled by this parser.
                // Notify the user and continue the parsing.
                STORM_PRINT_AND_LOG("unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (name.compare("place") == 0) {
                    traversePlaceElement(child);
                } else if(name.compare("transition") == 0) {
                    traverseTransitionElement(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }

        void GspnParser::traverseEdgesElement(xercesc::DOMNode const* const node) {
            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);


                // Found node or attribute which is at the moment not handled by this parser.
                // Notify the user and continue the parsing.
                STORM_PRINT_AND_LOG("unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");

            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (name.compare("arc") == 0) {
                    traverseArcElement(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
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


        void GspnParser::traversePlaceElement(xercesc::DOMNode const* const node) {
            storm::gspn::Place place;
            place.setID(newNode);
            ++newNode;


            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("name") == 0) {
                    place.setName(XMLtoString(attr->getNodeValue()));
                } else if (name.compare("marking") == 0) {
                    place.setNumberOfInitialTokens(std::stoull(XMLtoString(attr->getNodeValue())));
                } else if (ignorePlaceAttribute(name)) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                            "unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            gspn.addPlace(place);
        }
        
        bool ignoreTransitionAttribute(std::string const& name) {
            // TODO we should probably not ignore x-servers but check that it is 0.5.
            if ((name == "label-x") || (name == "label-y") || (name == "x") || (name == "y") || (name == "nservers-x")) {
                return true;
            }
            return false;
        }

        void GspnParser::traverseTransitionElement(xercesc::DOMNode const* const node) {
            std::string transitionName;
            bool immediateTransition;
            double rate = 1.0; // The default rate in GreatSPN. 

            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("name") == 0) {
                    transitionName = XMLtoString(attr->getNodeValue());
                } else if (name.compare("type") == 0) {
                    if (XMLtoString(attr->getNodeValue()).compare("EXP") == 0) {
                        immediateTransition = false;
                    } else {
                        immediateTransition = true;
                    }
                } else if(name.compare("delay") == 0) {
                    rate = std::stod(XMLtoString(attr->getNodeValue()));
                } else if (ignoreTransitionAttribute(name)) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                            "unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }

            if (immediateTransition) {
                auto transition = storm::gspn::ImmediateTransition<double>();
                transition.setName(transitionName);
                gspn.addImmediateTransition(transition);
            } else {
                auto transition = storm::gspn::TimedTransition<double>();
                transition.setName(transitionName);
                transition.setRate(rate);
                gspn.addTimedTransition(transition);
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
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

        void GspnParser::traverseArcElement(xercesc::DOMNode const* const node) {
            std::string head = "____NOT_SET____";
            std::string tail = "____NOT_SET____";
            std::string kind = "____NOT_SET____";
            uint_fast64_t mult = 1;

            // traverse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("head") == 0) {
                    head = XMLtoString(attr->getNodeValue());
                } else if (name.compare("tail") == 0) {
                    tail = XMLtoString(attr->getNodeValue());
                } else if (name.compare("kind") == 0) {
                    kind = XMLtoString(attr->getNodeValue());
                } else if (name.compare("mult") == 0) {
                    mult = std::stoull(XMLtoString(attr->getNodeValue()));
                } else if (ignoreArcAttribute(name)) {
                    // ignore node
                } else {
                    // Found node or attribute which is at the moment not handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG(
                            "unknown attribute (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
            
            STORM_LOG_THROW(head.compare("____NOT_SET____") != 0, storm::exceptions::WrongFormatException, "Arc must have a head");
            STORM_LOG_THROW(tail.compare("____NOT_SET____") != 0, storm::exceptions::WrongFormatException, "Arc must have a tail");
            STORM_LOG_THROW(kind.compare("____NOT_SET____") != 0, storm::exceptions::WrongFormatException, "Arc must have a kind");
            

            if (kind.compare("INPUT") == 0) {
                auto transition = gspn.getTransition(head);
                if (!transition.first) {
                    std::cout << "transition not found" << std::endl;
                }
                auto place = gspn.getPlace(tail);
                if (!place.first) {
                    std::cout << "place not found" << std::endl;
                }
                transition.second->setInputArcMultiplicity(place.second, mult);
            } else if (kind.compare("INHIBITOR") == 0) {
                auto transition = gspn.getTransition(head);
                auto place = gspn.getPlace(tail);
                transition.second->setInhibitionArcMultiplicity(place.second, mult);
            } else if (kind.compare("OUTPUT") == 0) {
                auto transition = gspn.getTransition(tail);
                auto place = gspn.getPlace(head);
                transition.second->setOutputArcMultiplicity(place.second, mult);
            } else {
                // TODO error!
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else if(ignoreArcChild(name)) {
                    // ignore
                } else {
                    // Found node or attribute which is at the moment nod handled by this parser.
                    // Notify the user and continue the parsing.
                    STORM_PRINT_AND_LOG("unknown child (node=" + XMLtoString(node->getNodeName()) + "): " + name + "\n");
                }
            }
        }








    }
}

