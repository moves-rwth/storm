#include "DftJsonExporter.h"

#include "storm/utility/file.h"
#include "storm/exceptions/NotImplementedException.h"

#include <algorithm>
#include <string>

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
        modernjson::json DftJsonExporter<ValueType>::translate(storm::storage::DFT<ValueType> const& dft) {
            // Nodes
            modernjson::json jsonNodes;
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                modernjson::json jsonNode = translateNode(dft.getElement(i));
                jsonNodes.push_back(jsonNode);
            }

            modernjson::json jsonDft;
            jsonDft["toplevel"] = std::to_string(dft.getTopLevelIndex());
            jsonDft["nodes"] = jsonNodes;
            return jsonDft;
        }

        template<typename ValueType>
        modernjson::json DftJsonExporter<ValueType>::translateNode(DFTElementCPointer const& element) {
            modernjson::json nodeData;
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
                // Gate dependent export
                switch (element->type()) {
                    case storm::storage::DFTElementType::VOT:
                        nodeData["voting"] = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(element)->threshold();
                        break;
                    case storm::storage::DFTElementType::PDEP:
                    {
                        ValueType probability = std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(element)->probability();
                        if (!storm::utility::isOne<ValueType>(probability)) {
                            std::stringstream stream;
                            stream << probability;
                            nodeData["prob"] = stream.str();
                        }
                        break;
                    }
                    default:
                        break;
                }
            } else if (element->isBasicElement()) {
                // Set BE specific data
                std::shared_ptr<DFTBE<ValueType> const> be = std::static_pointer_cast<DFTBE<ValueType> const>(element);
                std::stringstream stream;
                stream << be->activeFailureRate();
                nodeData["rate"] = stream.str();
                stream.str(std::string()); // Clear stringstream
                ValueType dormancy = be->passiveFailureRate() / be->activeFailureRate();
                stream << dormancy;
                nodeData["dorm"] = stream.str();
            }

            modernjson::json jsonNode;
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
