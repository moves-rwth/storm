#include "DftJsonExporter.h"

#include "storm/utility/file.h"
#include "storm/exceptions/NotSupportedException.h"

#include <algorithm>
#include <string>
#include <storm/exceptions/InvalidArgumentException.h>

namespace storm {
    namespace storage {

        template<typename ValueType>
        void DftJsonExporter<ValueType>::toFile(storm::storage::DFT<ValueType> const& dft, std::string const& filepath) {
            std::ofstream stream;
            storm::utility::openFile(filepath, stream);
            toStream(dft, stream);
            storm::utility::closeFile(stream);
        }

        template<typename ValueType>
        void DftJsonExporter<ValueType>::toStream(storm::storage::DFT<ValueType> const& dft, std::ostream& os) {
            os << translate(dft).dump(4) << std::endl;
        }

        template<typename ValueType>
        typename DftJsonExporter<ValueType>::Json DftJsonExporter<ValueType>::translate(storm::storage::DFT<ValueType> const& dft) {
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
            std::string type = storm::storage::toString(element->type());
            std::transform(type.begin(), type.end(), type.begin(), ::tolower);
            nodeData["type"] = type;

            if (element->isGate()) {
                // Set children for gate
                std::shared_ptr<DFTGate<ValueType> const> gate = std::static_pointer_cast<DFTGate<ValueType> const>(element);
                std::vector<std::string> children;
                for (DFTElementPointer const& child : gate->children()) {
                    children.push_back(std::to_string(child->id()));
                }
                nodeData["children"] = children;

                // Set threshold for voting gate
                if (element->type() == storm::storage::DFTElementType::VOT) {
                    nodeData["voting"] = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(element)->threshold();
                }
            } else if (element->isRestriction()) {
                // TODO use same method for gates and restrictions
                // Set children for restriction
                auto restriction = std::static_pointer_cast<storm::storage::DFTRestriction<ValueType> const>(element);
                std::vector<std::string> children;
                for (DFTElementPointer const& child : restriction->children()) {
                    children.push_back(std::to_string(child->id()));
                }
                nodeData["children"] = children;
            } else if (element->isDependency()) {
                // Set children for dependency
                auto dependency = std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(element);
                std::vector<std::string> children;
                children.push_back(std::to_string(dependency->triggerEvent()->id()));
                for (DFTElementPointer const& child : dependency->dependentEvents()) {
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
                std::shared_ptr<DFTBE<ValueType> const> be = std::static_pointer_cast<DFTBE<ValueType> const>(element);
                // Set BE specific data
                switch (element->type()) {
                    case storm::storage::DFTElementType::BE_EXP:
                    {
                        auto beExp = std::static_pointer_cast<BEExponential<ValueType> const>(be);
                        std::stringstream stream;
                        nodeData["distribution"] = "exp";
                        stream << beExp->activeFailureRate();
                        nodeData["rate"] = stream.str();
                        stream.str(std::string()); // Clear stringstream
                        stream << beExp->dormancyFactor();
                        nodeData["dorm"] = stream.str();
                        break;
                    }
                    case storm::storage::DFTElementType::BE_CONST:
                    {
                        auto beConst = std::static_pointer_cast<BEConst<ValueType> const>(be);
                        std::stringstream stream;
                        nodeData["distribution"] = "const";
                        nodeData["failed"] = beConst->failed();
                        break;
                    }
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "BE of type '" << be->type() << "' is not known.");
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

    }
}
