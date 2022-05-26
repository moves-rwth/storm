#include "DftJsonExporter.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/io/file.h"

#include <algorithm>
#include <storm/exceptions/InvalidArgumentException.h>
#include <string>

namespace storm::dft {
namespace storage {

template<typename ValueType>
void DftJsonExporter<ValueType>::toFile(storm::dft::storage::DFT<ValueType> const& dft, std::string const& filepath) {
    std::ofstream stream;
    storm::utility::openFile(filepath, stream);
    toStream(dft, stream);
    storm::utility::closeFile(stream);
}

template<typename ValueType>
void DftJsonExporter<ValueType>::toStream(storm::dft::storage::DFT<ValueType> const& dft, std::ostream& os) {
    os << translate(dft).dump(4) << '\n';
}

template<typename ValueType>
typename DftJsonExporter<ValueType>::Json DftJsonExporter<ValueType>::translate(storm::dft::storage::DFT<ValueType> const& dft) {
    // Nodes
    Json jsonNodes;
    for (size_t i = 0; i < dft.nrElements(); ++i) {
        Json jsonNode = translateNode(dft.getElement(i));
        jsonNodes.push_back(jsonNode);
    }

    Json jsonDft;
    jsonDft["toplevel"] = std::to_string(dft.getTopLevelIndex());
    jsonDft["nodes"] = jsonNodes;
    return jsonDft;
}

template<typename ValueType>
typename DftJsonExporter<ValueType>::Json DftJsonExporter<ValueType>::translateNode(DFTElementCPointer const& element) {
    Json nodeData;
    nodeData["id"] = std::to_string(element->id());
    nodeData["name"] = element->name();
    std::string type = storm::dft::storage::elements::toString(element->type());
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    nodeData["type"] = type;

    if (element->isGate()) {
        // Set children for gate
        std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType> const> gate =
            std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType> const>(element);
        std::vector<std::string> children;
        for (DFTElementPointer const& child : gate->children()) {
            children.push_back(std::to_string(child->id()));
        }
        nodeData["children"] = children;

        // Set threshold for voting gate
        if (element->type() == storm::dft::storage::elements::DFTElementType::VOT) {
            nodeData["voting"] = std::static_pointer_cast<storm::dft::storage::elements::DFTVot<ValueType> const>(element)->threshold();
        }
    } else if (element->isRestriction()) {
        // TODO use same method for gates and restrictions
        // Set children for restriction
        auto restriction = std::static_pointer_cast<storm::dft::storage::elements::DFTRestriction<ValueType> const>(element);
        std::vector<std::string> children;
        for (DFTElementPointer const& child : restriction->children()) {
            children.push_back(std::to_string(child->id()));
        }
        nodeData["children"] = children;
    } else if (element->isDependency()) {
        // Set children for dependency
        auto dependency = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType> const>(element);
        std::vector<std::string> children;
        children.push_back(std::to_string(dependency->triggerEvent()->id()));
        for (std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType>> const& child : dependency->dependentEvents()) {
            children.push_back(std::to_string(child->id()));
        }
        nodeData["children"] = children;
        if (!storm::utility::isOne<ValueType>(dependency->probability())) {
            std::stringstream stream;
            stream << dependency->probability();
            nodeData["probability"] = stream.str();
        } else {
            nodeData["type"] = "fdep";
        }
    } else if (element->isBasicElement()) {
        std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const> be =
            std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType> const>(element);
        // Set BE specific data
        switch (be->beType()) {
            case storm::dft::storage::elements::BEType::CONSTANT: {
                auto beConst = std::static_pointer_cast<storm::dft::storage::elements::BEConst<ValueType> const>(be);
                std::stringstream stream;
                nodeData["distribution"] = "const";
                nodeData["failed"] = beConst->failed();
                break;
            }
            case storm::dft::storage::elements::BEType::EXPONENTIAL: {
                auto beExp = std::static_pointer_cast<storm::dft::storage::elements::BEExponential<ValueType> const>(be);
                std::stringstream stream;
                nodeData["distribution"] = "exp";
                stream << beExp->activeFailureRate();
                nodeData["rate"] = stream.str();
                stream.str(std::string());  // Clear stringstream
                stream << beExp->dormancyFactor();
                nodeData["dorm"] = stream.str();
                break;
            }
            default:
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "BE of type '" << be->beType() << "' is not known.");
                break;
        }
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Element of type '" << element->type() << "' is not supported.");
    }

    Json jsonNode;
    jsonNode["data"] = nodeData;
    jsonNode["group"] = "nodes";
    jsonNode["classes"] = type;
    return jsonNode;
}

// Explicitly instantiate the class.
template class DftJsonExporter<double>;
template class DftJsonExporter<storm::RationalFunction>;

}  // namespace storage
}  // namespace storm::dft
