#include "DftTransformator.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm::dft {
namespace transformations {

template<typename ValueType>
DftTransformator<ValueType>::DftTransformator() {}

template<typename ValueType>
std::shared_ptr<storm::dft::storage::DFT<ValueType>> DftTransformator<ValueType>::transformUniqueFailedBe(storm::dft::storage::DFT<ValueType> const &dft) {
    STORM_LOG_DEBUG("Start transformation UniqueFailedBe");
    storm::dft::builder::DFTBuilder<ValueType> builder;
    // NOTE: if probabilities for constant BEs are introduced, change this to vector of tuples (name, prob)
    std::vector<std::string> failedBEs;

    for (size_t i = 0; i < dft.nrElements(); ++i) {
        std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const> element = dft.getElement(i);
        switch (element->type()) {
            case storm::dft::storage::elements::DFTElementType::BE: {
                auto be = std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType> const>(element);
                switch (be->beType()) {
                    case storm::dft::storage::elements::BEType::CONSTANT: {
                        auto beConst = std::static_pointer_cast<storm::dft::storage::elements::BEConst<ValueType> const>(element);
                        if (beConst->canFail()) {
                            STORM_LOG_TRACE("Transform " + beConst->name() + " [BE (const failed)]");
                            failedBEs.push_back(beConst->name());
                        }
                        // All original constant BEs are set to failsafe, failed BEs are later triggered by a new element
                        builder.addBasicElementConst(beConst->name(), false);
                        break;
                    }
                    case storm::dft::storage::elements::BEType::EXPONENTIAL: {
                        auto beExp = std::static_pointer_cast<storm::dft::storage::elements::BEExponential<ValueType> const>(element);
                        builder.addBasicElementExponential(beExp->name(), beExp->activeFailureRate(), beExp->dormancyFactor(), beExp->isTransient());
                        break;
                    }
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "BE type '" << be->beType() << "' not known.");
                        break;
                }
                break;
            }
            case storm::dft::storage::elements::DFTElementType::AND:
                builder.addAndGate(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::OR:
                builder.addOrGate(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::VOT: {
                auto vot = std::static_pointer_cast<storm::dft::storage::elements::DFTVot<ValueType> const>(element);
                builder.addVotingGate(vot->name(), vot->threshold(), getChildrenVector(vot));
                break;
            }
            case storm::dft::storage::elements::DFTElementType::PAND: {
                auto pand = std::static_pointer_cast<storm::dft::storage::elements::DFTPand<ValueType> const>(element);
                builder.addPandGate(pand->name(), getChildrenVector(pand), pand->isInclusive());
                break;
            }
            case storm::dft::storage::elements::DFTElementType::POR: {
                auto por = std::static_pointer_cast<storm::dft::storage::elements::DFTPor<ValueType> const>(element);
                builder.addPorGate(por->name(), getChildrenVector(por), por->isInclusive());
                break;
            }
            case storm::dft::storage::elements::DFTElementType::SPARE:
                builder.addSpareGate(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::PDEP: {
                auto dep = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType> const>(element);
                builder.addPdep(dep->name(), getChildrenVector(dep), dep->probability());
                break;
            }
            case storm::dft::storage::elements::DFTElementType::SEQ:
                builder.addSequenceEnforcer(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::MUTEX:
                builder.addMutex(element->name(), getChildrenVector(element));
                break;
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "DFT type '" << element->type() << "' not known.");
                break;
        }
    }
    // At this point the DFT is an exact copy of the original, except for all constant failure probabilities being 0

    // Introduce new constantly failed BE and FDEPs to trigger all failures
    if (!failedBEs.empty()) {
        STORM_LOG_TRACE("Add Unique_Constant_Failure [BE (const failed)]");
        builder.addBasicElementConst("Unique_Constant_Failure", true);
        failedBEs.insert(std::begin(failedBEs), "Unique_Constant_Failure");
        STORM_LOG_TRACE("Add Failure_Trigger [FDEP]");
        builder.addPdep("Failure_Trigger", failedBEs, storm::utility::one<ValueType>());
    }

    builder.setTopLevel(dft.getTopLevelElement()->name());

    STORM_LOG_DEBUG("Transformation UniqueFailedBe complete");
    return std::make_shared<storm::dft::storage::DFT<ValueType>>(builder.build());
}

template<typename ValueType>
std::shared_ptr<storm::dft::storage::DFT<ValueType>> DftTransformator<ValueType>::transformBinaryFDEPs(storm::dft::storage::DFT<ValueType> const &dft) {
    STORM_LOG_DEBUG("Start transformation BinaryFDEPs");
    storm::dft::builder::DFTBuilder<ValueType> builder;

    for (size_t i = 0; i < dft.nrElements(); ++i) {
        std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const> element = dft.getElement(i);
        switch (element->type()) {
            case storm::dft::storage::elements::DFTElementType::BE: {
                auto be = std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType> const>(element);
                switch (be->beType()) {
                    case storm::dft::storage::elements::BEType::CONSTANT: {
                        auto beConst = std::static_pointer_cast<storm::dft::storage::elements::BEConst<ValueType> const>(element);
                        builder.addBasicElementConst(beConst->name(), beConst->canFail());
                        break;
                    }
                    case storm::dft::storage::elements::BEType::EXPONENTIAL: {
                        auto beExp = std::static_pointer_cast<storm::dft::storage::elements::BEExponential<ValueType> const>(element);
                        builder.addBasicElementExponential(beExp->name(), beExp->activeFailureRate(), beExp->dormancyFactor(), beExp->isTransient());
                        break;
                    }
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "BE type '" << be->beType() << "' not known.");
                        break;
                }
                break;
            }
            case storm::dft::storage::elements::DFTElementType::AND:
                builder.addAndGate(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::OR:
                builder.addOrGate(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::VOT: {
                auto vot = std::static_pointer_cast<storm::dft::storage::elements::DFTVot<ValueType> const>(element);
                builder.addVotingGate(vot->name(), vot->threshold(), getChildrenVector(vot));
                break;
            }
            case storm::dft::storage::elements::DFTElementType::PAND: {
                auto pand = std::static_pointer_cast<storm::dft::storage::elements::DFTPand<ValueType> const>(element);
                builder.addPandGate(pand->name(), getChildrenVector(pand), pand->isInclusive());
                break;
            }
            case storm::dft::storage::elements::DFTElementType::POR: {
                auto por = std::static_pointer_cast<storm::dft::storage::elements::DFTPor<ValueType> const>(element);
                builder.addPorGate(por->name(), getChildrenVector(por), por->isInclusive());
                break;
            }
            case storm::dft::storage::elements::DFTElementType::SPARE:
                builder.addSpareGate(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::PDEP: {
                auto dep = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType> const>(element);
                auto children = getChildrenVector(dep);
                if (!storm::utility::isOne(dep->probability())) {
                    if (children.size() > 2) {
                        STORM_LOG_TRACE("Transform " + element->name() + " [PDEP]");
                        // Introduce additional element for first capturing the probabilistic dependency
                        std::string nameAdditional = dep->name() + "_additional";
                        STORM_LOG_TRACE("Add auxilliary BE " << nameAdditional);
                        builder.addBasicElementConst(nameAdditional, false);
                        STORM_LOG_TRACE("Add " << dep->name() << "_pdep [PDEP]");
                        // First consider probabilistic dependency
                        builder.addPdep(dep->name() + "_pdep", {children.front(), nameAdditional}, dep->probability());
                        // Then consider dependencies to the children if probabilistic dependency failed
                        children.erase(children.begin());
                        size_t i = 1;
                        for (auto const &child : children) {
                            std::string nameDep = dep->name() + "_" + std::to_string(i);
                            if (builder.nameInUse(nameDep)) {
                                STORM_LOG_ERROR("Element with name '" << nameDep << "' already exists.");
                            }
                            STORM_LOG_TRACE("Add " << nameDep << " [FDEP]");
                            builder.addPdep(nameDep, {dep->name() + "_additional", child}, storm::utility::one<ValueType>());
                            ++i;
                        }
                    } else {
                        builder.addPdep(dep->name(), children, dep->probability());
                    }
                } else {
                    // Add dependencies
                    for (size_t i = 1; i < children.size(); ++i) {
                        std::string nameDep;
                        if (children.size() == 2) {
                            nameDep = dep->name();
                        } else {
                            nameDep = dep->name() + "_" + std::to_string(i);
                            STORM_LOG_TRACE("Transform " + element->name() + " [FDEP]");
                            STORM_LOG_TRACE("Add " + nameDep + " [FDEP]");
                        }
                        if (builder.nameInUse(nameDep)) {
                            STORM_LOG_ERROR("Element with name '" << nameDep << "' already exists.");
                        }
                        STORM_LOG_ASSERT(storm::utility::isOne(dep->probability()) || children.size() == 2, "PDEP with multiple children supported.");
                        builder.addPdep(nameDep, {children[0], children[i]}, storm::utility::one<ValueType>());
                    }
                }
                break;
            }
            case storm::dft::storage::elements::DFTElementType::SEQ:
                builder.addSequenceEnforcer(element->name(), getChildrenVector(element));
                break;
            case storm::dft::storage::elements::DFTElementType::MUTEX:
                builder.addMutex(element->name(), getChildrenVector(element));
                break;
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "DFT type '" << element->type() << "' not known.");
                break;
        }
    }

    builder.setTopLevel(dft.getTopLevelElement()->name());

    STORM_LOG_DEBUG("Transformation BinaryFDEPs complete");
    return std::make_shared<storm::dft::storage::DFT<ValueType>>(builder.build());
}

template<typename ValueType>
std::vector<std::string> DftTransformator<ValueType>::getChildrenVector(std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const> element) {
    std::vector<std::string> res;
    if (element->isDependency()) {
        // Dependencies have to be handled separately
        auto dependency = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType> const>(element);
        res.push_back(dependency->triggerEvent()->name());
        for (auto const &depEvent : dependency->dependentEvents()) {
            res.push_back(depEvent->name());
        }
    } else {
        auto elementWithChildren = std::static_pointer_cast<storm::dft::storage::elements::DFTChildren<ValueType> const>(element);
        for (auto const &child : elementWithChildren->children()) {
            res.push_back(child->name());
        }
    }
    return res;
}

// Explicitly instantiate the class.
template class DftTransformator<double>;
template class DftTransformator<RationalFunction>;

}  // namespace transformations
}  // namespace storm::dft
