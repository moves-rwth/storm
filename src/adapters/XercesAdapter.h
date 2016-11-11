#pragma once


#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLString.hpp>


#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>



namespace storm {
    namespace adapters {
        inline std::string XMLtoString(const XMLCh *xmlString) {
            char* tmp = xercesc::XMLString::transcode(xmlString);
            auto result = std::string(tmp);
            delete tmp;
            return result;
        }
        
        inline std::string getName(xercesc::DOMNode const* node) {
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
                    assert(false);
                    return "";
                }
            }
        }
    }
}
