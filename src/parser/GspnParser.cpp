#include "src/parser/GspnParser.h"

#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include "src/exceptions/UnexpectedException.h"
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
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, message);
                xercesc::XMLString::release(&message);
            }
            catch (const xercesc::DOMException& toCatch) {
                auto message = xercesc::XMLString::transcode(toCatch.msg);
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, message);
                xercesc::XMLString::release(&message);
            }
            catch (...) {
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Failed to parse pnml file.\n");
            }

            // build gspn by traversing the DOM object
            parser->getDocument()->normalizeDocument();
            xercesc::DOMElement* elementRoot = parser->getDocument()->getDocumentElement();
            STORM_LOG_THROW(getName(elementRoot).compare("pnml") == 0, storm::exceptions::UnexpectedException, "Failed to identify the root element.\n");
            traversePnmlElement(elementRoot);

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
                    STORM_PRINT_AND_LOG("unknown child (node=place): " + name + "\n");
                }
            }

            // build place and add it to the gspn
            storm::gspn::Place place;
            place.setName(placeName);
            if (!numberOfInitialTokens.first) {
                STORM_PRINT_AND_LOG("unknown numberOfInitialTokens (place=" + placeName + ")\n");
            }
            place.setNumberOfInitialTokens(numberOfInitialTokens.second);
            if (!capacity.first) {
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

            // parse attributes
            for (uint_fast64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
                auto attr = node->getAttributes()->item(i);
                auto name = getName(attr);

                if (name.compare("id") == 0) {
                    id = XMLtoString(attr->getNodeValue());
                } else {
                    STORM_PRINT_AND_LOG("unknown attribute (node=transition): " + name + "\n");
                }
            }

            // traverse children
            for (uint_fast64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
                auto child = node->getChildNodes()->item(i);
                auto name = getName(child);

                if (name.compare("rate") == 0) {
                    value.first =true;
                    value.second = traverseTransitionValue(child);
                } else if (name.compare("timed") == 0) {
                    timed.first = true;
                    timed.second = traverseTransitionType(child);
                } else if (std::all_of(name.begin(), name.end(), isspace)) {
                    // ignore node (contains only whitespace)
                } else if (name.compare("graphics") == 0 ||
                           name.compare("name") == 0 ||
                           name.compare("orientation") == 0) {
                    // ignore these tags
                } else {
                    STORM_PRINT_AND_LOG("unknown child (node=transition): " + name + "\n");
                }
            }

            // build transition and add it to the gspn
            if (!timed.first) {
                STORM_PRINT_AND_LOG("unknown transition type (transition=" + id + ")\n");
            }

            if (timed.second) {
                storm::gspn::TimedTransition<storm::gspn::GSPN::RateType> transition;
                if (!value.first) {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown transition rate (transition=" + id + ")\n");
                }
                transition.setRate(std::stod(value.second));
                transition.setName(id);
                gspn.addTimedTransition(transition);
            } else {
                storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType> transition;
                if (!value.first) {
                    STORM_PRINT_AND_LOG("unknown transition weight (transition=" + id + ")\n");
                }
                transition.setWeight(std::stod(value.second));
                transition.setName(id);
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
                    STORM_PRINT_AND_LOG("unknown child (node=arc): " + name + "\n");
                }
            }

            // check if all necessary information where stored in the pnml file
            if (!source.first) {
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown arc source (arc=" + id + ")\n");
            }
            if (!target.first) {
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException ,"unknown arc target (arc=" + id + ")\n");
            }
            if (!multiplicity.first) {
                STORM_PRINT_AND_LOG("unknown multiplicity (node=arc): " + id + "\n");
            }

            //determine if it is an outgoing or incoming arc
            {
                auto place = gspn.getPlace(source.second);
                auto trans = gspn.getTransition(target.second);
                if (true == place.first && true == trans.first) {
                    if (!type.first) {
                        STORM_PRINT_AND_LOG("unknown arc type (arc=" + id + ")\n");
                    }

                    // incoming arc
                    if (type.second.compare("normal") == 0) {
                        trans.second->setInputArcMultiplicity(place.second, multiplicity.second);
                    } else if (type.second.compare("inhibition") == 0) {
                        trans.second->setInhibitionArcMultiplicity(place.second, multiplicity.second);
                    } else {
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
                    STORM_PRINT_AND_LOG("unknown child (node=type): " + name + "\n");
                }
            }
            return defaultArcType;
        }

        std::string GspnParser::XMLtoString(const XMLCh *xmlString) {
            char* tmp = xercesc::XMLString::transcode(xmlString);
            auto result = std::string(tmp);
            delete tmp;
            return result;
        }
    }
}

