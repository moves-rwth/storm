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
                    switch (element->type()) {
                        case storm::storage::DFTElementType::BE_EXP: {
                            STORM_LOG_DEBUG("Transform " + element->name() + " [BE (exp)]");
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
                                STORM_LOG_DEBUG("Transform " + element->name() + " [BE (const failed)]");
                                failedBEs.push_back(be_const->name());
                            } else {
                                STORM_LOG_DEBUG("Transform " + element->name() + " [BE (const failsafe)]");
                            }
                            // All original constant BEs are set to failsafe, failed BEs are later triggered by a new element
                            builder.addBasicElementConst(be_const->name(), false);
                            break;
                        }
                        case storm::storage::DFTElementType::AND:
                            STORM_LOG_DEBUG("Transform " + element->name() + " [AND]");
                            builder.addAndElement(element->name(), getChildrenVector(element));
                            break;
                        case storm::storage::DFTElementType::OR:
                            STORM_LOG_DEBUG("Transform " + element->name() + " [OR]");
                            builder.addOrElement(element->name(), getChildrenVector(element));
                            break;
                        case storm::storage::DFTElementType::VOT: {
                            STORM_LOG_DEBUG("Transform " + element->name() + " [VOT]");
                            auto vot = std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(element);
                            builder.addVotElement(vot->name(), vot->threshold(), getChildrenVector(vot));
                            break;
                        }
                        case storm::storage::DFTElementType::PAND: {
                            STORM_LOG_DEBUG("Transform " + element->name() + " [PAND]");
                            auto pand = std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(element);
                            builder.addPandElement(pand->name(), getChildrenVector(pand), pand->isInclusive());
                            break;
                        }
                        case storm::storage::DFTElementType::POR: {
                            STORM_LOG_DEBUG("Transform " + element->name() + " [POR]");
                            auto por = std::static_pointer_cast<storm::storage::DFTPor<ValueType> const>(element);
                            builder.addPandElement(por->name(), getChildrenVector(por), por->isInclusive());
                            break;
                        }
                        case storm::storage::DFTElementType::SPARE:
                            STORM_LOG_DEBUG("Transform " + element->name() + " [SPARE]");
                            builder.addSpareElement(element->name(), getChildrenVector(element));
                            break;
                        case storm::storage::DFTElementType::PDEP: {
                            auto dep = std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(
                                    element);
                            if (dep->isFDEP()) {
                                STORM_LOG_DEBUG("Transform " + element->name() + " [FDEP]");
                            } else {
                                STORM_LOG_DEBUG("Transform " + element->name() + " [PDEP]");
                            }
                            builder.addDepElement(dep->name(), getChildrenVector(element), dep->probability());
                            break;
                        }
                        case storm::storage::DFTElementType::SEQ:
                            STORM_LOG_DEBUG("Transform " + element->name() + " [SEQ]");
                            builder.addSequenceEnforcer(element->name(), getChildrenVector(element));
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException,
                                            "DFT type '" << element->type() << "' not known.");
                            break;
                    }

                }
                // At this point the DFT is an exact copy of the original, except for all constant failure probabilities being 0
                if (!failedBEs.empty()) {
                    builder.addBasicElementConst("Unique_Constant_Failure", true);
                    failedBEs.insert(std::begin(failedBEs), "Unique_Constant_Failure");
                    builder.addDepElement("Failure_Trigger", failedBEs, storm::utility::one<ValueType>());
                }

                builder.setTopLevel(mDft.getTopLevelGate()->name());

                STORM_LOG_DEBUG("Transformation complete!");
                return builder.build();
            }

            template<typename ValueType>
            std::vector<std::string> DftTransformator<ValueType>::getChildrenVector(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> element) {
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
