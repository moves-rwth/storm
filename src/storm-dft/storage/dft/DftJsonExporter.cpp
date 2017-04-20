#include "DftJsonExporter.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"

#include <algorithm>
#include <string>

namespace storm {
    namespace storage {

        template<typename ValueType>
        size_t DftJsonExporter<ValueType>::currentId = 0;

        template<typename ValueType>
        void DftJsonExporter<ValueType>::toFile(storm::storage::DFT<ValueType> const& dft, std::string const& filepath) {
            std::ofstream ofs;
            ofs.open(filepath, std::ofstream::out);
            if(ofs.is_open()) {
                toStream(dft, ofs);
                ofs.close();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Cannot open " << filepath);
            }
        }

        template<typename ValueType>
        void DftJsonExporter<ValueType>::toStream(storm::storage::DFT<ValueType> const& dft, std::ostream& os) {
            os << translate(dft).dump(4) << std::endl;
        }

        template<typename ValueType>
        modernjson::json DftJsonExporter<ValueType>::translate(storm::storage::DFT<ValueType> const& dft) {
            modernjson::json jsonDft;
            currentId = 0;

            // Nodes
            modernjson::json jsonNodes;
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                modernjson::json jsonNode = translateNode(dft.getElement(i));
                jsonDft.push_back(jsonNode);
            }

            // Edges
            modernjson::json jsonEdges;
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                if (dft.isGate(i)) {
                    std::shared_ptr<DFTGate<ValueType> const> gate = dft.getGate(i);
                    for (size_t index = 0; index < gate->nrChildren(); ++index) {
                        DFTElementPointer child = gate->children()[index];
                        modernjson::json jsonEdge = translateEdge(gate, child, index);
                        jsonDft.push_back(jsonEdge);
                    }
                }
            }

            return jsonDft;
        }

        template<typename ValueType>
        modernjson::json DftJsonExporter<ValueType>::translateNode(DFTElementCPointer const& element) {
            modernjson::json nodeData;
            nodeData["id"] = element->id();
            ++currentId;
            STORM_LOG_ASSERT(element->id() == currentId-1, "Ids do not correspond");
            nodeData["name"] = element->name();

            if (element->isGate()) {
                // Set children for gate
                std::shared_ptr<DFTGate<ValueType> const> gate = std::static_pointer_cast<DFTGate<ValueType> const>(element);
                std::vector<size_t> children;
                for (DFTElementPointer const& child : gate->children()) {
                    children.push_back(child->id());
                }
                nodeData["children"] = children;
            } else if (element->isBasicElement()) {
                // Set rates for BE
                std::shared_ptr<DFTBE<ValueType> const> be = std::static_pointer_cast<DFTBE<ValueType> const>(element);
                std::stringstream stream;
                stream << be->activeFailureRate();
                nodeData["rate"] = stream.str();
                stream.clear();
                stream << (be->passiveFailureRate() / be->activeFailureRate());
                nodeData["dorm"] = stream.str();
            }

            modernjson::json jsonNode;
            jsonNode["data"] = nodeData;

            jsonNode["group"] = "nodes";
            std::string type = storm::storage::toString(element->type());
            std::transform(type.begin(), type.end(), type.begin(), ::tolower);
            jsonNode["classes"] = type;
            return jsonNode;
        }

        template<typename ValueType>
        modernjson::json DftJsonExporter<ValueType>::translateEdge(std::shared_ptr<DFTGate<ValueType> const> const& gate, DFTElementCPointer const& child, size_t index) {
            modernjson::json nodeData;
            std::stringstream stream;
            stream << gate->id() << "e" << child->id();
            nodeData["id"] = stream.str();
            ++currentId;
            nodeData["source"] = gate->id();
            nodeData["target"] = child->id();
            nodeData["index"] = index;

            modernjson::json jsonNode;
            jsonNode["data"] = nodeData;

            jsonNode["group"] = "edges";
            jsonNode["classes"] = "";
            return jsonNode;
        }

        // Explicitly instantiate the class.
        template class DftJsonExporter<double>;

#ifdef STORM_HAVE_CARL
        template class DftJsonExporter<storm::RationalFunction>;
#endif

    }
}
