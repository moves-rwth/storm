

#include "DFTBuilder.h"

#include "DFT.h"
#include <algorithm>
#include "OrderDFTElementsById.h"
#include <src/utility/macros.h>
#include <src/exceptions/NotSupportedException.h>
#include "../../exceptions/WrongFormatException.h"


namespace storm {
    namespace storage {

        template<typename ValueType>
        DFT<ValueType> DFTBuilder<ValueType>::build() {
            for(auto& elem : mChildNames) {
                for(auto const& child : elem.second) {
                    std::shared_ptr<DFTGate> gate = std::static_pointer_cast<DFTGate>(elem.first);
                    gate->pushBackChild(mElements[child]);
                    mElements[child]->addParent(gate);
                }
            }

            // Sort elements topologically

            // compute rank
            for (auto& elem : mElements) {
                computeRank(elem.second);
            }

            std::vector<std::shared_ptr<DFTElement>> elems = topoSort();
            size_t id = 0;
            for(std::shared_ptr<DFTElement> e : elems) {
                e->setId(id++);
            }
            return DFT<ValueType>(elems, mElements[topLevelIdentifier]);
        }

        template<typename ValueType>
        unsigned DFTBuilder<ValueType>::computeRank(std::shared_ptr<DFTElement> const& elem) {
            if(elem->rank() == -1) {
                if(elem->nrChildren() == 0) {
                    elem->setRank(0);
                } else {
                    std::shared_ptr<DFTGate> gate = std::static_pointer_cast<DFTGate>(elem);
                    unsigned maxrnk = 0;
                    unsigned newrnk = 0;

                    for (std::shared_ptr<DFTElement> const &child : gate->children()) {
                        newrnk = computeRank(child);
                        if (newrnk > maxrnk) {
                            maxrnk = newrnk;
                        }
                    }
                    elem->setRank(maxrnk + 1);
                }
            }

            return elem->rank();
        }

        template<typename ValueType>
        bool DFTBuilder<ValueType>::addStandardGate(std::string const& name, std::vector<std::string> const& children, DFTElementTypes tp) {
            assert(children.size() > 0);
            if(mElements.count(name) != 0) {
                // Element with that name already exists.
                return false;
            }
            std::shared_ptr<DFTElement> element;
            switch(tp) {
                case DFTElementTypes::AND:
                    element = std::make_shared<DFTAnd>(mNextId++, name);
                    break;
                case DFTElementTypes::OR:
                    element = std::make_shared<DFTOr>(mNextId++, name);
                    break;
                case DFTElementTypes::PAND:
                    element = std::make_shared<DFTPand>(mNextId++, name);
                    break;
                case DFTElementTypes::POR:
                    element = std::make_shared<DFTPor>(mNextId++, name);
                    break;
                case DFTElementTypes::SPARE:
                   element = std::make_shared<DFTSpare>(mNextId++, name);
                   break;
                case DFTElementTypes::BE:
                case DFTElementTypes::VOT:
                    // Handled separately
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type handled separately.");
                case DFTElementTypes::CONSTF:
                case DFTElementTypes::CONSTS:
                case DFTElementTypes::FDEP:
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type not supported.");
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type not known.");
            }
            mElements[name] = element;
            mChildNames[element] = children;
            return true;
        }

        template<typename ValueType>
        void DFTBuilder<ValueType>::topoVisit(std::shared_ptr<DFTElement> const& n, std::map<std::shared_ptr<DFTElement>, topoSortColour>& visited, std::vector<std::shared_ptr<DFTElement>>& L) {
            if(visited[n] == topoSortColour::GREY) {
                throw storm::exceptions::WrongFormatException("DFT is cyclic");
            } else if(visited[n] == topoSortColour::WHITE) {
                if(n->isGate()) {
                    visited[n] = topoSortColour::GREY;
                    for(std::shared_ptr<DFTElement> const& c : std::static_pointer_cast<DFTGate>(n)->children()) {
                        topoVisit(c, visited, L);
                    }
                }
                visited[n] = topoSortColour::BLACK;
                L.push_back(n);
            }
        }

        template<typename ValueType>
        std::vector<std::shared_ptr<DFTElement>> DFTBuilder<ValueType>::topoSort() {
            std::map<std::shared_ptr<DFTElement>, topoSortColour> visited;
            for(auto const& e : mElements) {
                visited.insert(std::make_pair(e.second, topoSortColour::WHITE)); 
            }

            std::vector<std::shared_ptr<DFTElement>> L;
            for(auto const& e : visited) {
                topoVisit(e.first, visited, L);
            }
            //std::reverse(L.begin(), L.end()); 
            return L;
        }

        // Explicitly instantiate the class.
        template class DFTBuilder<double>;

#ifdef STORM_HAVE_CARL
        template class DFTBuilder<RationalFunction>;
#endif

    }
}
            
