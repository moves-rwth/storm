

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
                    auto itFind = mElements.find(child);
                    if (itFind != mElements.end()) {
                        // Child found
                        DFTElementPointer childElement = itFind->second;
                        assert(!childElement->isDependency());
                        gate->pushBackChild(childElement);
                        childElement->addParent(gate);
                    } else {
                        // Child not found -> find first dependent event to assure that child is dependency
                        auto itFind = mElements.find(child + "_1");
                        assert(itFind != mElements.end());
                        assert(itFind->second->isDependency());
                        STORM_LOG_TRACE("Ignore functional dependency " << child << " in gate " << gate->name());
                    }
                }
            }

            // Initialize dependencies
            for (auto& dependency : mDependencies) {
                DFTGatePointer triggerEvent = std::static_pointer_cast<DFTGate<ValueType>>(mElements[dependency->nameTrigger()]);
                assert(mElements[dependency->nameDependent()]->isBasicElement());
                std::shared_ptr<DFTBE<ValueType>> dependentEvent = std::static_pointer_cast<DFTBE<ValueType>>(mElements[dependency->nameDependent()]);
                dependency->initialize(triggerEvent, dependentEvent);
                triggerEvent->addOutgoingDependency(dependency);
                dependentEvent->addIngoingDependency(dependency);
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
                if(elem->nrChildren() == 0 || elem->isDependency()) {
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
        bool DFTBuilder<ValueType>::addStandardGate(std::string const& name, std::vector<std::string> const& children, DFTElementType tp) {
            assert(children.size() > 0);
            if(mElements.count(name) != 0) {
                // Element with that name already exists.
                return false;
            }
            DFTElementPointer element;
            switch(tp) {
                case DFTElementType::AND:
                    element = std::make_shared<DFTAnd<ValueType>>(mNextId++, name);
                    break;
                case DFTElementType::OR:
                    element = std::make_shared<DFTOr<ValueType>>(mNextId++, name);
                    break;
                case DFTElementType::PAND:
                    element = std::make_shared<DFTPand<ValueType>>(mNextId++, name);
                    break;
                case DFTElementType::POR:
                    element = std::make_shared<DFTPor<ValueType>>(mNextId++, name);
                    break;
                case DFTElementType::SPARE:
                   element = std::make_shared<DFTSpare<ValueType>>(mNextId++, name);
                   break;
                case DFTElementType::BE:
                case DFTElementType::VOT:
                    // Handled separately
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type handled separately.");
                case DFTElementType::CONSTF:
                case DFTElementType::CONSTS:
                case DFTElementType::PDEP:
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type not supported.");
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type not known.");
            }
            mElements[name] = element;
            mChildNames[element] = children;
            return true;
        }

        template<typename ValueType>
        void DFTBuilder<ValueType>::topoVisit(DFTElementPointer const& n, std::map<DFTElementPointer, topoSortColour, OrderElementsById<ValueType>>& visited, DFTElementVector& L) {
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
            std::map<DFTElementPointer, topoSortColour, OrderElementsById<ValueType>> visited;
            for(auto const& e : mElements) {
                visited.insert(std::make_pair(e.second, topoSortColour::WHITE)); 
            }

            DFTElementVector L;
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
            
