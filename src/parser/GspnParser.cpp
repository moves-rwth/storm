#include <string>
#include <bitset>
#include <iosfwd>
#include <memory>
#include "src/parser/GspnParser.h"

storm::gspn::GSPN storm::parser::GspnParser::parse(const std::string &filename) {
    // initialize xerces
    try {
        xercesc::XMLPlatformUtils::Initialize();
    }
    catch (const xercesc::XMLException& toCatch) {
        // TODO do something
    }



    auto parser = new xercesc::XercesDOMParser();
    parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
    parser->setDoNamespaces(false);
    parser->setDoSchema(false);
    parser->setLoadExternalDTD(false);
    parser->setIncludeIgnorableWhitespace(false);
    //parser->loadGrammar(source, type);

    auto errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
    parser->setErrorHandler(errHandler);

    try {
        parser->parse(filename.c_str());
    }
    catch (const xercesc::XMLException& toCatch) {
        auto message = xercesc::XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message is: \n"
        << message << "\n";
        xercesc::XMLString::release(&message);
    }
    catch (const xercesc::DOMException& toCatch) {
        auto message = xercesc::XMLString::transcode(toCatch.msg);
        std::cout << "Exception message is: \n"
        << message << "\n";
        xercesc::XMLString::release(&message);
    }
    catch (...) {
        std::cout << "Unexpected Exception \n" ;
    }


    parser->getDocument()->normalizeDocument();
    xercesc::DOMElement* elementRoot = parser->getDocument()->getDocumentElement();
    if (getName(elementRoot).compare("pnml") != 0) {
        std::cout << "expected: pnml" << std::endl;
        std::cout << "found: " << XMLtoString(elementRoot->getTagName()) << std::endl;
        // TODO error
    }
    parsePNML(elementRoot);

    delete parser;
    delete errHandler;


    // clean up
    xercesc::XMLPlatformUtils::Terminate();

    return gspn;
}

std::string storm::parser::GspnParser::XMLtoString(const XMLCh *xmlString) {
    char* tmp = xercesc::XMLString::transcode(xmlString);
    auto result = std::string(tmp);
    delete tmp;
    return result;
}

void storm::parser::GspnParser::parsePNML(xercesc::DOMElement *element) {
    for (uint64_t i = 0; i < element->getChildNodes()->getLength(); ++i) {
        auto child = element->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("net") == 0) {
            parseNet(child);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else {
            std::cout << "pnml" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parseNet(xercesc::DOMNode* node) {
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("page") == 0) {
            parsePage(child);
        } else if (name.compare("place") == 0) {
            parsePlace(child);
        } else if (name.compare("transition") == 0) {
            parseTransition(child);
        } else if (name.compare("arc") == 0) {
            parseArc(child);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else if (name.compare("name") == 0 ||
                   name.compare("token") == 0) {
            // ignore these tags
        } else {
            std::cout << "net" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parsePage(xercesc::DOMNode *node) {
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("place") == 0) {
            parsePlace(child);
        } else if (name.compare("transition") == 0) {
            parseTransition(child);
        } else if (name.compare("arc") == 0) {
            parseArc(child);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else {
            std::cout << "page" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parsePlace(xercesc::DOMNode *node) {
    uint64_t place;
    for (uint64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
        auto attr = node->getAttributes()->item(i);
        auto name = getName(attr);
        if (name.compare("id") == 0) {
            place = addNewPlace(XMLtoString(attr->getNodeValue()));
        } else {
            std::cout << "place" << std::endl;
            std::cout << "unkown attr.: " << name << std::endl;
        }
    }
    
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("initialMarking") == 0) {
            auto tokens = parseInitialMarking(child);

            std::cout << "place: " << place << "; tokens: " << tokens << std::endl;
            //TODO search bug
            gspn.setNumberOfPlaces(gspn.getNumberOfPlaces()+1);
            //gspn.setInitialTokens(place, tokens);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else if (name.compare("name") == 0 ||
                   name.compare("graphics") == 0) {
            // ignore these tags
        } else {
            std::cout << "place" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parseTransition(xercesc::DOMNode *node) {
    bool timed = false;
    std::string rate, id;

    for (uint64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
        auto attr = node->getAttributes()->item(i);
        auto name = getName(attr);
        if (name.compare("id") == 0) {
            id = XMLtoString(attr->getNodeValue());
        } else {
            std::cout << "transition" << std::endl;
            std::cout << "unkown attr.: " << name << std::endl;
        }
    }

    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("rate") == 0) {
            rate = parseRate(child);
        } else if (name.compare("timed") == 0) {
            timed = parseTimed(child);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else if (name.compare("graphics") == 0 ||
                   name.compare("name") == 0 ||
                   name.compare("orientation") == 0) {
            // ignore these tags
        } else {
            std::cout << "transition" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }

    if (timed) {
        auto transition = std::make_shared<storm::gspn::TimedTransition<storm::gspn::GSPN::RateType>>();
        transition->setRate(std::stoull(rate));
        gspn.addTimedTransition(transition);
        this->stringToTransition[id] = transition;
    } else {
        auto transition = std::make_shared<storm::gspn::ImmediateTransition<storm::gspn::GSPN::WeightType>>();
        transition->setWeight(std::stoull(rate));
        gspn.addImmediateTransition(transition);
        this->stringToTransition[id] = transition;
    }
}

void storm::parser::GspnParser::parseArc(xercesc::DOMNode *node) {
    std::string source, target, type;
    uint64_t cardinality;

    for (uint64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
        auto attr = node->getAttributes()->item(i);
        auto name = getName(attr);
        if (name.compare("source") == 0) {
            source = XMLtoString(attr->getNodeValue());
        } else if (name.compare("target") == 0) {
            target = XMLtoString(attr->getNodeValue());
        } else if (name.compare("id") == 0) {
            // ignore these tags
        } else {
            std::cout << "arc" << std::endl;
            std::cout << "unkown attr.: " << name << std::endl;
        }
    }

    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("type") == 0) {
            type = parseType(child);
        } else if(name.compare("inscription") == 0) {
            cardinality = parseCapacity(child);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else if (name.compare("graphics") == 0 ||
                   name.compare("arcpath") == 0) {
            // ignore these tags
        } else {
            std::cout << "arc" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
    //determine if it is an outgoing or incoming arc
    {
        auto it1 = stringToState.find(source);
        auto it2 = stringToTransition.find(target);
        if (it1 != stringToState.end() && it2 != stringToTransition.end()) {
            // incoming arc
            if (type.compare("normal") == 0) {
                auto transition = stringToTransition[target];
                transition->setInputArcCardinality(stringToState[source], cardinality);
            } else {
                std::cout << "arc" << std::endl;
                std::cout << "unkown type: " << type << std::endl;
            }
            return;
        }
    }
    {
        auto it1 = stringToTransition.find(source);
        auto it2 = stringToState.find(target);
        if (it1 != stringToTransition.end() && it2 != stringToState.end()) {
            // outgoing arc
            if (type.compare("normal") == 0) {
                auto transition = stringToTransition[source];
                transition->setOutputArcCardinality(stringToState[target], cardinality);
            } else {
                std::cout << "arc" << std::endl;
                std::cout << "unkown type: " << type << std::endl;
            }
            return;
        }
    }
    std::cout << "found an arc with no correpsonding transition" << std::endl;
}

std::string storm::parser::GspnParser::getName(xercesc::DOMNode *node) {
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
            std::cout << "unknown NodeType: " << node->getNodeType() << std::endl;
            return "";
        }
    }
}

uint64_t storm::parser::GspnParser::addNewPlace(std::string id) {
    auto place = newNode;
    ++newNode;
    stringToState[id] = place;// TODO check whether the id is already in the map
    return place;
}

uint64_t storm::parser::GspnParser::parseInitialMarking(xercesc::DOMNode *node) {
    uint64_t result= 0;
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
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
            std::cout << "initialMarking" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
    return result;
}

std::string storm::parser::GspnParser::parseRate(xercesc::DOMNode *node) {
    std::string result = "";
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("value") == 0) {
            result = getName(child->getFirstChild());
        } else  if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else {
            std::cout << "rate" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
    return result;
}

bool storm::parser::GspnParser::parseTimed(xercesc::DOMNode *node) {
    bool result;
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("value") == 0) {
            result = getName(child->getFirstChild()).compare("true") ? true : false;
        } else  if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else {
            std::cout << "timed" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
    return result;
}

std::string storm::parser::GspnParser::parseType(xercesc::DOMNode *node) {
    for (uint64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
        auto attr = node->getAttributes()->item(i);
        auto name = getName(attr);
        if (name.compare("value") == 0) {
            return XMLtoString(attr->getNodeValue());
        } else {
            std::cout << "type" << std::endl;
            std::cout << "unkown attr.: " << name << std::endl;
        }
    }
    return "";
}

uint64_t storm::parser::GspnParser::parseCapacity(xercesc::DOMNode *node) {
    uint64_t result= 0;
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("value") == 0) {
            auto value = getName(child->getFirstChild());
            value = value.substr(std::string("Default,").length());
            result = std::stoull(value);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // ignore node (contains only whitespace)
        } else if (name.compare("graphics") == 0) {
            // ignore these tags
        } else {
            std::cout << "capacity" << std::endl;
            std::cout << "unkown child: " << name << std::endl;
        }
    }
    return result;
}
