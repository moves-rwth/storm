

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
                DFTGatePointer gate = std::static_pointer_cast<DFTGate<ValueType>>(elem.first);
                for(auto const& child : elem.second) {
                    gate->pushBackChild(mElements[child]);
                    mElements[child]->addParent(gate);
                }
            }

            // Sort elements topologically
            // compute rank
            for (auto& elem : mElements) {
                computeRank(elem.second);
            }

            DFTElementVector elems = topoSort();
            size_t id = 0;
            for(DFTElementPointer e : elems) {
                e->setId(id++);
            }
            return DFT<ValueType>(elems, mElements[topLevelIdentifier]);
        }

        template<typename ValueType>
        unsigned DFTBuilder<ValueType>::computeRank(DFTElementPointer const& elem) {
            if(elem->rank() == -1) {
                if(elem->nrChildren() == 0) {
                    elem->setRank(0);
                } else {
                    DFTGatePointer gate = std::static_pointer_cast<DFTGate<ValueType>>(elem);
                    unsigned maxrnk = 0;
                    unsigned newrnk = 0;

                    for (DFTElementPointer const &child : gate->children()) {
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
            DFTElementPointer element;
            switch(tp) {
                case DFTElementTypes::AND:
                    element = std::make_shared<DFTAnd<ValueType>>(mNextId++, name);
                    break;
                case DFTElementTypes::OR:
                    element = std::make_shared<DFTOr<ValueType>>(mNextId++, name);
                    break;
                case DFTElementTypes::PAND:
                    element = std::make_shared<DFTPand<ValueType>>(mNextId++, name);
                    break;
                case DFTElementTypes::POR:
                    element = std::make_shared<DFTPor<ValueType>>(mNextId++, name);
                    break;
                case DFTElementTypes::SPARE:
                   element = std::make_shared<DFTSpare<ValueType>>(mNextId++, name);
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
        void DFTBuilder<ValueType>::topoVisit(DFTElementPointer const& n, std::map<DFTElementPointer, topoSortColour>& visited, DFTElementVector& L) {
            if(visited[n] == topoSortColour::GREY) {
                throw storm::exceptions::WrongFormatException("DFT is cyclic");
            } else if(visited[n] == topoSortColour::WHITE) {
                if(n->isGate()) {
                    visited[n] = topoSortColour::GREY;
                    for(DFTElementPointer const& c : std::static_pointer_cast<DFTGate<ValueType>>(n)->children()) {
                        topoVisit(c, visited, L);
                    }
                }
                visited[n] = topoSortColour::BLACK;
                L.push_back(n);
            }
        }

        // TODO Matthias: use typedefs
        template<typename ValueType>
        std::vector<std::shared_ptr<DFTElement<ValueType>>> DFTBuilder<ValueType>::topoSort() {
            std::map<std::shared_ptr<DFTElement<ValueType>>, topoSortColour> visited;
            for(auto const& e : mElements) {
                visited.insert(std::make_pair(e.second, topoSortColour::WHITE)); 
            }

            std::vector<std::shared_ptr<DFTElement<ValueType>>> L;
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
            
