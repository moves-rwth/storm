#include <string>
#include <memory>
#include <iosfwd>
#include <bitset>
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
    std::cout << "pnml" << std::endl;
    for (uint64_t i = 0; i < element->getChildNodes()->getLength(); ++i) {
        auto child = element->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("net") == 0) {
            parseNet(child);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // TODO remove after adding DTD
        } else {
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parseNet(xercesc::DOMNode* node) {
    std::cout << "net" << std::endl;
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
            // TODO remove after adding DTD
        } else if (name.compare("name") == 0) {
            // ignore these tags
        } else {
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parsePage(xercesc::DOMNode *node) {
    std::cout << "page" << std::endl;
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
            // TODO remove after adding DTD
        } else {
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parsePlace(xercesc::DOMNode *node) {
    std::cout << "place" << std::endl;
    uint64_t place;
    for (uint64_t i = 0; i < node->getAttributes()->getLength(); ++i) {
        auto attr = node->getAttributes()->item(i);
        auto name = getName(attr);
        if (name.compare("id") == 0) {
            place = addNewPlace(XMLtoString(attr->getNodeValue()));
        } else {
            std::cout << "unkown attr.: " << name << std::endl;
        }
    }

    //redo
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("initialMarking") == 0) {
            auto tokens = parseInitialMarking(child);
            gspn.setInitialTokens(place, tokens);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // TODO remove after adding DTD
        } else if (name.compare("name") == 0 ||
                  name.compare("graphics") == 0) {
            // ignore these tags
        } else {
            std::cout << "unkown child: " << name << std::endl;
        }
    }
}

void storm::parser::GspnParser::parseTransition(xercesc::DOMNode *node) {
    //std::cout << "transition" << std::endl; // TODO
    bool timed = false;
    //value for the rate

}

void storm::parser::GspnParser::parseArc(xercesc::DOMNode *node) {
    //std::cout << "arc" << std::endl; // TODO
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
    std::cout << "initialMarking" << std::endl;
    uint64_t result= 0;
    for (uint64_t i = 0; i < node->getChildNodes()->getLength(); ++i) {
        auto child = node->getChildNodes()->item(i);
        auto name = getName(child);
        if (name.compare("text") == 0) {
            result = std::stoull(getName(child->getFirstChild()));
        } else if (name.compare("value") == 0) {
            auto value = getName(child->getFirstChild());
            value.substr(std::string("Default,").length()-1);
            result = std::stoull(value);
        } else if (std::all_of(name.begin(), name.end(), isspace)) {
            // TODO remove after adding DTD
        } else if (name.compare("graphics") == 0) {
            // ignore these tags
        } else {
            std::cout << "unkown child: " << name << std::endl;
        }
    }
    return result;
}
