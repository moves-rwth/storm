#include "DftTransformator.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace transformations {
        namespace dft {
            template<typename ValueType>
            DftTransformator<ValueType>::DftTransformator(storm::storage::DFT<ValueType> const &dft) : mDft(dft) {}

            template<typename ValueType>
            storm::storage::DFT<ValueType> DftTransformator<ValueType>::transformUniqueFailedBe() {
                storm::builder::DFTBuilder<ValueType> builder = storm::builder::DFTBuilder<ValueType>(true, false);
                // NOTE: if probabilities for constant BEs are introduced, change this to vector of tuples (name, prob)
                std::vector<std::string> failedBEs;

                for (size_t i = 0; i < mDft.nrElements(); ++i) {
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = mDft.getElement(i);
                    STORM_LOG_DEBUG("Transform " + element->name());
                    switch (element->type()) {
                        case storm::storage::DFTElementType::BE_EXP: {
                            auto be_exp = std::static_pointer_cast<storm::storage::BEExponential<ValueType> const>(
                                    element);
                            builder.addBasicElementExponential(be_exp->name(), be_exp->activeFailureRate(),
                                                               be_exp->dormancyFactor());
                            break;
                        }
                        case storm::storage::DFTElementType::BE_CONST: {
                            auto be_const = std::static_pointer_cast<storm::storage::BEExponential<ValueType> const>(
                                    element);
                            if (be_const->canFail()) {
                                failedBEs.push_back(be_const->name());
                            }
                            // All original constant BEs are set to failsafe, failed BEs are later triggered by a new element
                            builder.addBasicElementConst(be_const->name(), false);
                            break;
                        }
                        case storm::storage::DFTElementType::AND:
                            builder.addAndElement(element->name(), getChildrenVector(element));
                            break;
                        case storm::storage::DFTElementType::OR:
                            builder.addOrElement(element->name(), getChildrenVector(element));
                            break;
                        case storm::storage::DFTElementType::VOT: {
                            auto vot = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(element);
                            builder.addVotElement(vot->name(), vot->threshold(), getChildrenVector(vot));
                            break;
                        }
                        case storm::storage::DFTElementType::PAND: {
                            auto pand = std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(element);
                            builder.addPandElement(pand->name(), getChildrenVector(pand), pand->isInclusive());
                            break;
                        }
                        case storm::storage::DFTElementType::POR: {
                            auto por = std::static_pointer_cast<storm::storage::DFTPor<ValueType> const>(element);
                            builder.addPandElement(por->name(), getChildrenVector(por), por->isInclusive());
                            break;
                        }
                        case storm::storage::DFTElementType::SPARE:
                            builder.addSpareElement(element->name(), getChildrenVector(element));
                            break;
                        case storm::storage::DFTElementType::PDEP: {
                            auto dep = std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(
                                    element);
                            builder.addDepElement(dep->name(), getChildrenVector(element), dep->probability());
                            break;
                        }
                        case storm::storage::DFTElementType::SEQ:
                            builder.addSequenceEnforcer(element->name(), getChildrenVector(element));
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                                            "DFT type '" << element->type() << "' not known.");
                            break;
                    }

                }
                // At this point the DFT is an exact copy of the original, except for all constant failure probabilities being 0

                builder.setTopLevel(mDft.getTopLevelGate()->name());

                STORM_LOG_DEBUG("Transformation complete!");
                return builder.build();
            }

            template<typename ValueType>
            std::vector<std::string> DftTransformator<ValueType>::getChildrenVector(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> element) {
                STORM_LOG_DEBUG("Get children for " + element->name());
                std::vector<std::string> res;
                if (element->isDependency()) {
                    // Dependencies have to be handled separately
                    auto dependency = std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(element);
                    res.push_back(dependency->triggerEvent()->name());
                    for (auto const &depEvent : dependency->dependentEvents()) {
                        res.push_back(depEvent->name());
                    }
                } else {
                    auto elementWithChildren = std::static_pointer_cast<storm::storage::DFTChildren<ValueType> const>(
                            element);
                    for (auto const &child : elementWithChildren->children()) {
                        res.push_back(child->name());
                        STORM_LOG_DEBUG("Got child " + child->name());
                    }
                }
                return res;
            }

            // Explicitly instantiate the class.
            template
            class DftTransformator<double>;

#ifdef STORM_HAVE_CARL

            template
            class DftTransformator<RationalFunction>;

#endif
        }
    }
}
