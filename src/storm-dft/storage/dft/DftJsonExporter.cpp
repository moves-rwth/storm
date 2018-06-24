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
            modernjson::json jsonDft;
            jsonDft["toplevel"] = dft.getTopLevelIndex();

            // Parameters
            modernjson::json jsonParameters;
            jsonDft["parameters"] = jsonParameters;

            // Nodes
            modernjson::json jsonNodes;
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                modernjson::json jsonNode = translateNode(dft.getElement(i));
                jsonNodes.push_back(jsonNode);
            }
            jsonDft["nodes"] = jsonNodes;

            return jsonDft;
        }

        template<typename ValueType>
        modernjson::json DftJsonExporter<ValueType>::translateNode(DFTElementCPointer const& element) {
            modernjson::json nodeData;
            nodeData["id"] = element->id();
            nodeData["name"] = element->name();
            std::string type = storm::storage::toString(element->type());
            std::transform(type.begin(), type.end(), type.begin(), ::tolower);
            nodeData["type"] = type;

            if (element->isGate()) {
                // Set children for gate
                std::shared_ptr<DFTGate<ValueType> const> gate = std::static_pointer_cast<DFTGate<ValueType> const>(element);
                std::vector<size_t> children;
                for (DFTElementPointer const& child : gate->children()) {
                    children.push_back(child->id());
                }
                nodeData["children"] = children;
                // Set gate specific data
                switch (element->type()) {
                    case storm::storage::DFTElementType::VOT:
                        nodeData["voting"] = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(element)->threshold();
                        break;
                    default:
                        break;
                }
            } else if (element->isBasicElement()) {
                // Set BE specific data
                std::shared_ptr<DFTBE<ValueType> const> be = std::static_pointer_cast<DFTBE<ValueType> const>(element);
                nodeData["rate"] = storm::utility::to_string(be->activeFailureRate());
                nodeData["dorm"] = storm::utility::to_string(be->passiveFailureRate() / be->activeFailureRate());
                nodeData["repair"] = 0;
                nodeData["transient"] = be->isTransient();
            } else if (element->isDependency()) {
                std::shared_ptr<DFTDependency<ValueType> const> dependency = std::static_pointer_cast<DFTDependency<ValueType> const>(element);
                std::vector<size_t> children = { dependency->triggerEvent()->id() };
                for (DFTElementPointer const& child : dependency->dependentEvents()) {
                    children.push_back(child->id());
                }
                nodeData["children"] = children;
                nodeData["prob"] = storm::utility::to_string(dependency->probability());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Type name: " << type << "  not supported for export.");
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
