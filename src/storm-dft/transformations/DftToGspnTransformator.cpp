#include "DftToGspnTransformator.h"
#include "storm/exceptions/NotImplementedException.h"
#include <memory>

namespace storm {
    namespace transformations {
        namespace dft {

            // Prevent some magic constants
            static constexpr const uint64_t defaultPriority = 1;
            static constexpr const uint64_t defaultCapacity = 1;

            template<typename ValueType>
            DftToGspnTransformator<ValueType>::DftToGspnTransformator(storm::storage::DFT<ValueType> const &dft) : mDft(
                    dft) {
                // Intentionally left empty.
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::transform(std::map<uint64_t, uint64_t> const &priorities,
                                                              std::set<uint64_t> const &dontCareElements, bool smart,
                                                              bool mergeDCFailed) {
                this->priorities = priorities;
                this->dontCareElements = dontCareElements;
                this->smart = smart;
                this->mergedDCFailed = false;//mergeDCFailed;
                this->dontCarePriority = 1;
                builder.setGspnName("DftToGspnTransformation");

                // Translate all GSPN elements
                translateGSPNElements();

                // Create initial template
                // TODO
            }

            template<typename ValueType>
            std::map<uint64_t, uint64_t> DftToGspnTransformator<ValueType>::computePriorities() {
                std::map<uint64_t, uint64_t> priorities;
                for (std::size_t i = 0; i < mDft.nrElements(); i++) {
                    // Give all elements the same priority
                    priorities[i] = 2;
                }
                return priorities;
            }


            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::toplevelFailedPlaceId() {
                STORM_LOG_ASSERT(failedPlaces.size() > mDft.getTopLevelIndex(),
                                 "Failed place for top level element does not exist.");
                return failedPlaces.at(mDft.getTopLevelIndex());
            }

            template<typename ValueType>
            gspn::GSPN *DftToGspnTransformator<ValueType>::obtainGSPN() {
                return builder.buildGspn();
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateGSPNElements() {
                // Loop through every DFT element and create its corresponding GSPN template.
                for (std::size_t i = 0; i < mDft.nrElements(); i++) {
                    auto dftElement = mDft.getElement(i);

                    // Check which type the element is and call the corresponding translate-function.
                    switch (dftElement->type()) {
                        case storm::storage::DFTElementType::BE:
                            translateBE(std::static_pointer_cast<storm::storage::DFTBE<ValueType> const>(dftElement));
                            break;
                        case storm::storage::DFTElementType::CONSTF:
                            translateCONSTF(dftElement);
                            break;
                        case storm::storage::DFTElementType::CONSTS:
                            translateCONSTS(dftElement);
                            break;
                        case storm::storage::DFTElementType::AND:
                            translateAND(std::static_pointer_cast<storm::storage::DFTAnd<ValueType> const>(dftElement));
                            break;
                        case storm::storage::DFTElementType::OR:
                            translateOR(std::static_pointer_cast<storm::storage::DFTOr<ValueType> const>(dftElement));
                            break;
                        case storm::storage::DFTElementType::VOT:
                            translateVOT(std::static_pointer_cast<storm::storage::DFTVot<ValueType> const>(dftElement));
                            break;
                        case storm::storage::DFTElementType::PAND:
                            translatePAND(
                                    std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(dftElement),
                                    std::static_pointer_cast<storm::storage::DFTPand<ValueType> const>(
                                            dftElement)->isInclusive());
                            break;
                        case storm::storage::DFTElementType::POR:
                            translatePOR(std::static_pointer_cast<storm::storage::DFTPor<ValueType> const>(dftElement),
                                         std::static_pointer_cast<storm::storage::DFTPor<ValueType> const>(
                                                 dftElement)->isInclusive());
                            break;
                        case storm::storage::DFTElementType::SPARE:
                            translateSPARE(
                                    std::static_pointer_cast<storm::storage::DFTSpare<ValueType> const>(dftElement));
                            break;
                        case storm::storage::DFTElementType::PDEP:
                            translatePDEP(std::static_pointer_cast<storm::storage::DFTDependency<ValueType> const>(
                                    dftElement));
                            break;
                        case storm::storage::DFTElementType::SEQ:
                            translateSeq(std::static_pointer_cast<storm::storage::DFTSeq<ValueType> const>(dftElement));
                            break;
                        default:
                            STORM_LOG_ASSERT(false, "DFT type " << dftElement->type() << " unknown.");
                            break;
                    }
                }

            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateBE(
                    std::shared_ptr<storm::storage::DFTBE<ValueType> const> dftBE) {
                double xcenter = mDft.getElementLayoutInfo(dftBE->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftBE->id()).y;

                uint64_t failedPlace = addFailedPlace(dftBE, storm::gspn::LayoutInfo(xcenter + 3.0, ycenter));

                uint64_t activePlace = builder.addPlace(defaultCapacity, isActiveInitially(dftBE) ? 1 : 0,
                                                        dftBE->name() + STR_ACTIVATED);
                activePlaces.emplace(dftBE->id(), activePlace);
                builder.setPlaceLayoutInfo(activePlace, storm::gspn::LayoutInfo(xcenter - 3.0, ycenter));

                uint64_t tActive = builder.addTimedTransition(getFailPriority(dftBE), dftBE->activeFailureRate(),
                                                              dftBE->name() + "_activeFailing");
                builder.setTransitionLayoutInfo(tActive, storm::gspn::LayoutInfo(xcenter, ycenter + 3.0));
                builder.addInputArc(activePlace, tActive);
                builder.addInhibitionArc(failedPlace, tActive);
                builder.addOutputArc(tActive, activePlace);
                builder.addOutputArc(tActive, failedPlace);

                uint64_t tPassive = builder.addTimedTransition(getFailPriority(dftBE), dftBE->passiveFailureRate(),
                                                               dftBE->name() + "_passiveFailing");
                builder.setTransitionLayoutInfo(tPassive, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0));
                builder.addInhibitionArc(activePlace, tPassive);
                builder.addInhibitionArc(failedPlace, tPassive);
                builder.addOutputArc(tPassive, failedPlace);

                if (dontCareElements.count(dftBE->id()) && dftBE->id() != mDft.getTopLevelIndex()) {
                    u_int64_t tDontCare = addDontcareTransition(dftBE,
                                                                storm::gspn::LayoutInfo(xcenter + 12.0, ycenter));
                    if (!mergedDCFailed) {
                        uint64_t dontCarePlace = builder.addPlace(1, 0, dftBE->name() + STR_DONTCARE);
                        builder.setPlaceLayoutInfo(dontCarePlace,
                                                   storm::gspn::LayoutInfo(xcenter + 12.0, ycenter + 5.0));
                        builder.addInhibitionArc(dontCarePlace, tDontCare);
                        builder.addOutputArc(tDontCare, dontCarePlace);

                        builder.addInhibitionArc(dontCarePlace, tActive);
                        builder.addInhibitionArc(dontCarePlace, tPassive);

                        //Propagation for dependencies
                        if (!smart || dftBE->hasIngoingDependencies()) {
                            uint64_t dependencyPropagationPlace = builder.addPlace(1, 0,
                                                                                   dftBE->name() + "_dependency_prop");
                            dependencyPropagationPlaces.emplace(dftBE->id(), dependencyPropagationPlace);
                            builder.setPlaceLayoutInfo(dependencyPropagationPlace,
                                                       storm::gspn::LayoutInfo(xcenter + 10.0, ycenter - 5.0));
                            uint64_t tPropagationFailed = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                         dftBE->name() + "_prop_fail");
                            builder.setTransitionLayoutInfo(tPropagationFailed,
                                                            storm::gspn::LayoutInfo(xcenter + 8.0, ycenter));
                            builder.addInhibitionArc(dependencyPropagationPlace, tPropagationFailed);
                            builder.addInputArc(failedPlace, tPropagationFailed);
                            builder.addOutputArc(tPropagationFailed, failedPlace);
                            builder.addOutputArc(tPropagationFailed, dependencyPropagationPlace);
                            uint64_t tPropagationDontCare = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                           dftBE->name() +
                                                                                           "_prop_dontCare");
                            builder.setTransitionLayoutInfo(tPropagationDontCare,
                                                            storm::gspn::LayoutInfo(xcenter + 10.0, ycenter));
                            builder.addInhibitionArc(dependencyPropagationPlace, tPropagationDontCare);
                            builder.addInputArc(dependencyPropagationPlace, tPropagationDontCare);
                            builder.addOutputArc(tPropagationDontCare, dontCarePlace);
                            builder.addOutputArc(tPropagationDontCare, dependencyPropagationPlace);
                        }
                    } else {
                        builder.addInhibitionArc(failedPlace, tDontCare);
                        builder.addOutputArc(tDontCare, failedPlace);
                    }

                }

                if (!smart || dftBE->nrRestrictions() > 0) {
                    uint64_t disabledPlace = addDisabledPlace(dftBE, storm::gspn::LayoutInfo(xcenter - 9.0, ycenter));
                    builder.addInhibitionArc(disabledPlace, tActive);
                    builder.addInhibitionArc(disabledPlace, tPassive);
                }

                if (!smart || mDft.isRepresentative(dftBE->id())) {
                    uint64_t unavailablePlace = addUnavailablePlace(dftBE,
                                                                    storm::gspn::LayoutInfo(xcenter + 9.0, ycenter));
                    builder.addOutputArc(tActive, unavailablePlace);
                    builder.addOutputArc(tPassive, unavailablePlace);
                }

            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateCONSTF(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstF) {
                double xcenter = mDft.getElementLayoutInfo(dftConstF->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftConstF->id()).y;

                addFailedPlace(dftConstF, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0), true);

                if (!smart || mDft.isRepresentative(dftConstF->id())) {
                    addUnavailablePlace(dftConstF, storm::gspn::LayoutInfo(xcenter, ycenter + 3.0), false);
                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateCONSTS(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftConstS) {
                double xcenter = mDft.getElementLayoutInfo(dftConstS->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftConstS->id()).y;

                size_t capacity = 0; // It cannot contain a token, because it cannot fail.

                uint64_t failedPlace = builder.addPlace(capacity, 0, dftConstS->name() + STR_FAILED);
                assert(failedPlaces.size() == dftConstS->id());
                failedPlaces.push_back(failedPlace);
                builder.setPlaceLayoutInfo(failedPlace, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0));

                if (!smart || mDft.isRepresentative(dftConstS->id())) {
                    uint64_t unavailablePlace = builder.addPlace(capacity, 0, dftConstS->name() + "_unavail");
                    unavailablePlaces.emplace(dftConstS->id(), unavailablePlace);
                    builder.setPlaceLayoutInfo(unavailablePlace, storm::gspn::LayoutInfo(xcenter, ycenter + 3.0));
                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateAND(
                    std::shared_ptr<storm::storage::DFTAnd<ValueType> const> dftAnd) {
                double xcenter = mDft.getElementLayoutInfo(dftAnd->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftAnd->id()).y;

                uint64_t failedPlace = addFailedPlace(dftAnd, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0));

                uint64_t tFailed = builder.addImmediateTransition(getFailPriority(dftAnd), 0.0,
                                                                  dftAnd->name() + STR_FAILING);
                builder.setTransitionLayoutInfo(tFailed, storm::gspn::LayoutInfo(xcenter, ycenter + 3.0));
                builder.addInhibitionArc(failedPlace, tFailed);
                builder.addOutputArc(tFailed, failedPlace);

                if (dontCareElements.count(dftAnd->id())) {
                    if (dftAnd->id() != mDft.getTopLevelIndex()) {
                        u_int64_t tDontCare = addDontcareTransition(dftAnd,
                                                                    storm::gspn::LayoutInfo(xcenter + 16.0, ycenter));
                        if (!mergedDCFailed) {
                            uint64_t dontCarePlace = builder.addPlace(1, 0, dftAnd->name() + STR_DONTCARE);
                            builder.setPlaceLayoutInfo(dontCarePlace,
                                                       storm::gspn::LayoutInfo(xcenter + 16.0, ycenter + 4.0));
                            builder.addInhibitionArc(dontCarePlace, tDontCare);
                            builder.addOutputArc(tDontCare, dontCarePlace);
                            //Propagation
                            uint64_t propagationPlace = builder.addPlace(1, 0, dftAnd->name() + "_prop");
                            builder.setPlaceLayoutInfo(propagationPlace,
                                                       storm::gspn::LayoutInfo(xcenter + 12.0, ycenter + 8.0));
                            uint64_t tPropagationFailed = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                         dftAnd->name() + "_prop_fail");
                            builder.setTransitionLayoutInfo(tPropagationFailed,
                                                            storm::gspn::LayoutInfo(xcenter + 10.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationFailed);
                            builder.addInputArc(failedPlace, tPropagationFailed);
                            builder.addOutputArc(tPropagationFailed, failedPlace);
                            builder.addOutputArc(tPropagationFailed, propagationPlace);
                            uint64_t tPropagationDontCare = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                           dftAnd->name() +
                                                                                           "_prop_dontCare");
                            builder.setTransitionLayoutInfo(tPropagationDontCare,
                                                            storm::gspn::LayoutInfo(xcenter + 14.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationDontCare);
                            builder.addInputArc(dontCarePlace, tPropagationDontCare);
                            builder.addOutputArc(tPropagationDontCare, dontCarePlace);
                            builder.addOutputArc(tPropagationDontCare, propagationPlace);
                            for (auto const &child : dftAnd->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(propagationPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, propagationPlace);
                                }
                            }
                        } else {
                            builder.addInhibitionArc(failedPlace, tDontCare);
                            builder.addOutputArc(tDontCare, failedPlace);
                            for (auto const &child : dftAnd->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(failedPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, failedPlace);
                                }
                            }
                        }
                    } else {
                        // If AND is TLE, simple failure propagation suffices
                        for (auto const &child : dftAnd->children()) {
                            if (dontCareElements.count(child->id())) {
                                u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                builder.addInputArc(failedPlace, childDontCare);
                                builder.addOutputArc(childDontCare, failedPlace);
                            }
                        }
                    }
                }

                if (!smart || mDft.isRepresentative(dftAnd->id())) {
                    uint64_t unavailablePlace = addUnavailablePlace(dftAnd, storm::gspn::LayoutInfo(xcenter + 6.0,
                                                                                                    ycenter - 3.0));
                    builder.addOutputArc(tFailed, unavailablePlace);
                }

                for (auto const &child : dftAnd->children()) {
                    builder.addInputArc(getFailedPlace(child), tFailed);
                    builder.addOutputArc(tFailed, getFailedPlace(child));
                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateOR(
                    std::shared_ptr<storm::storage::DFTOr<ValueType> const> dftOr) {
                double xcenter = mDft.getElementLayoutInfo(dftOr->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftOr->id()).y;

                uint64_t failedPlace = addFailedPlace(dftOr, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0));

                if (dontCareElements.count(dftOr->id())) {
                    if (dftOr->id() != mDft.getTopLevelIndex()) {
                        u_int64_t tDontCare = addDontcareTransition(dftOr,
                                                                    storm::gspn::LayoutInfo(xcenter + 16.0, ycenter));
                        if (!mergedDCFailed) {
                            uint64_t dontCarePlace = builder.addPlace(1, 0, dftOr->name() + STR_DONTCARE);
                            builder.setPlaceLayoutInfo(dontCarePlace,
                                                       storm::gspn::LayoutInfo(xcenter + 16.0, ycenter + 4.0));
                            builder.addInhibitionArc(dontCarePlace, tDontCare);
                            builder.addOutputArc(tDontCare, dontCarePlace);
                            //Propagation
                            uint64_t propagationPlace = builder.addPlace(1, 0, dftOr->name() + "_prop");
                            builder.setPlaceLayoutInfo(propagationPlace,
                                                       storm::gspn::LayoutInfo(xcenter + 12.0, ycenter + 8.0));
                            uint64_t tPropagationFailed = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                         dftOr->name() + "_prop_fail");
                            builder.setTransitionLayoutInfo(tPropagationFailed,
                                                            storm::gspn::LayoutInfo(xcenter + 10.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationFailed);
                            builder.addInputArc(failedPlace, tPropagationFailed);
                            builder.addOutputArc(tPropagationFailed, failedPlace);
                            builder.addOutputArc(tPropagationFailed, propagationPlace);
                            uint64_t tPropagationDontCare = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                           dftOr->name() +
                                                                                           "_prop_dontCare");
                            builder.setTransitionLayoutInfo(tPropagationDontCare,
                                                            storm::gspn::LayoutInfo(xcenter + 14.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationDontCare);
                            builder.addInputArc(dontCarePlace, tPropagationDontCare);
                            builder.addOutputArc(tPropagationDontCare, dontCarePlace);
                            builder.addOutputArc(tPropagationDontCare, propagationPlace);
                            for (auto const &child : dftOr->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(propagationPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, propagationPlace);
                                }
                            }
                        } else {
                            builder.addInhibitionArc(failedPlace, tDontCare);
                            builder.addOutputArc(tDontCare, failedPlace);
                            for (auto const &child : dftOr->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(failedPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, failedPlace);
                                }
                            }
                        }
                    } else {
                        // If OR is TLE, simple failure propagation suffices
                        for (auto const &child : dftOr->children()) {
                            if (dontCareElements.count(child->id())) {
                                u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                builder.addInputArc(failedPlace, childDontCare);
                                builder.addOutputArc(childDontCare, failedPlace);
                            }
                        }
                    }
                }

                bool isRepresentative = mDft.isRepresentative(dftOr->id());
                uint64_t unavailablePlace = 0;
                if (!smart || isRepresentative) {
                    unavailablePlace = addUnavailablePlace(dftOr,
                                                           storm::gspn::LayoutInfo(xcenter + 6.0, ycenter - 3.0));
                }

                for (size_t i = 0; i < dftOr->nrChildren(); ++i) {
                    auto const &child = dftOr->children().at(i);
                    uint64_t tFailed = builder.addImmediateTransition(getFailPriority(dftOr), 0.0,
                                                                      dftOr->name() + STR_FAILING + std::to_string(i));
                    builder.setTransitionLayoutInfo(tFailed,
                                                    storm::gspn::LayoutInfo(xcenter - 5.0 + i * 3.0, ycenter + 3.0));
                    builder.addInhibitionArc(failedPlace, tFailed);
                    builder.addOutputArc(tFailed, failedPlace);
                    if (!smart || isRepresentative) {
                        builder.addOutputArc(tFailed, unavailablePlace);
                    }
                    builder.addInputArc(getFailedPlace(child), tFailed);
                    builder.addOutputArc(tFailed, getFailedPlace(child));
                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateVOT(
                    std::shared_ptr<storm::storage::DFTVot<ValueType> const> dftVot) {
                // TODO: finish layouting

                double xcenter = mDft.getElementLayoutInfo(dftVot->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftVot->id()).y;

                uint64_t failedPlace = addFailedPlace(dftVot, storm::gspn::LayoutInfo(xcenter, ycenter - 3.0));

                uint64_t tFailed = builder.addImmediateTransition(getFailPriority(dftVot), 0.0,
                                                                  dftVot->name() + STR_FAILING);
                builder.addOutputArc(tFailed, failedPlace);
                builder.addInhibitionArc(failedPlace, tFailed);

                if (!smart || mDft.isRepresentative(dftVot->id())) {
                    uint64_t unavailablePlace = addUnavailablePlace(dftVot, storm::gspn::LayoutInfo(xcenter + 6.0,
                                                                                                    ycenter - 3.0));
                    builder.addOutputArc(tFailed, unavailablePlace);
                }

                uint64_t collectorPlace = builder.addPlace(dftVot->nrChildren(), 0, dftVot->name() + "_collector");
                builder.setPlaceLayoutInfo(collectorPlace, storm::gspn::LayoutInfo(xcenter, ycenter));
                builder.addInputArc(collectorPlace, tFailed, dftVot->threshold());

                for (size_t i = 0; i < dftVot->nrChildren(); ++i) {
                    auto const &child = dftVot->children().at(i);
                    uint64_t childNextPlace = builder.addPlace(defaultCapacity, 1,
                                                               dftVot->name() + "_child_next" + std::to_string(i));

                    uint64_t tCollect = builder.addImmediateTransition(getFailPriority(dftVot), 0.0,
                                                                       dftVot->name() + "_child_collect" +
                                                                       std::to_string(i));
                    builder.addOutputArc(tCollect, collectorPlace);
                    builder.addInputArc(childNextPlace, tCollect);
                    builder.addInputArc(getFailedPlace(child), tCollect);
                    builder.addOutputArc(tCollect, getFailedPlace(child));
                }

                if (dontCareElements.count(dftVot->id())) {
                    if (dftVot->id() != mDft.getTopLevelIndex()) {
                        u_int64_t tDontCare = addDontcareTransition(dftVot,
                                                                    storm::gspn::LayoutInfo(xcenter + 16.0, ycenter));
                        if (!mergedDCFailed) {
                            uint64_t dontCarePlace = builder.addPlace(1, 0, dftVot->name() + STR_DONTCARE);
                            builder.setPlaceLayoutInfo(dontCarePlace,
                                                       storm::gspn::LayoutInfo(xcenter + 16.0, ycenter + 4.0));
                            builder.addInhibitionArc(dontCarePlace, tDontCare);
                            builder.addOutputArc(tDontCare, dontCarePlace);
                            //Propagation
                            uint64_t propagationPlace = builder.addPlace(1, 0, dftVot->name() + "_prop");
                            builder.setPlaceLayoutInfo(propagationPlace,
                                                       storm::gspn::LayoutInfo(xcenter + 12.0, ycenter + 8.0));
                            uint64_t tPropagationFailed = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                         dftVot->name() + "_prop_fail");
                            builder.setTransitionLayoutInfo(tPropagationFailed,
                                                            storm::gspn::LayoutInfo(xcenter + 10.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationFailed);
                            builder.addInputArc(failedPlace, tPropagationFailed);
                            builder.addOutputArc(tPropagationFailed, failedPlace);
                            builder.addOutputArc(tPropagationFailed, propagationPlace);
                            uint64_t tPropagationDontCare = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                           dftVot->name() +
                                                                                           "_prop_dontCare");
                            builder.setTransitionLayoutInfo(tPropagationDontCare,
                                                            storm::gspn::LayoutInfo(xcenter + 14.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationDontCare);
                            builder.addInputArc(dontCarePlace, tPropagationDontCare);
                            builder.addOutputArc(tPropagationDontCare, dontCarePlace);
                            builder.addOutputArc(tPropagationDontCare, propagationPlace);
                            for (auto const &child : dftVot->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(propagationPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, propagationPlace);
                                }
                            }
                        } else {
                            builder.addInhibitionArc(failedPlace, tDontCare);
                            builder.addOutputArc(tDontCare, failedPlace);
                            for (auto const &child : dftVot->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(failedPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, failedPlace);
                                }
                            }
                        }
                    } else {
                        // If VOT is TLE, simple failure propagation suffices
                        for (auto const &child : dftVot->children()) {
                            if (dontCareElements.count(child->id())) {
                                u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                builder.addInputArc(failedPlace, childDontCare);
                                builder.addOutputArc(childDontCare, failedPlace);
                            }
                        }
                    }
                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translatePAND(
                    std::shared_ptr<storm::storage::DFTPand<ValueType> const> dftPand, bool inclusive) {
                //TODO Layouting
                double xcenter = mDft.getElementLayoutInfo(dftPand->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftPand->id()).y;

                uint64_t failedPlace = addFailedPlace(dftPand, storm::gspn::LayoutInfo(xcenter + 3.0, ycenter - 3.0));

                // Set priority lower if the PAND is exclusive
                uint64_t tFailed = builder.addImmediateTransition(
                        inclusive ? getFailPriority(dftPand) : getFailPriority(dftPand) - 1, 0.0,
                        dftPand->name() + STR_FAILING);
                builder.setTransitionLayoutInfo(tFailed, storm::gspn::LayoutInfo(xcenter + 3.0, ycenter + 3.0));
                builder.addInhibitionArc(failedPlace, tFailed);
                builder.addOutputArc(tFailed, failedPlace);

                if (!smart || mDft.isRepresentative(dftPand->id())) {
                    uint64_t unavailablePlace = addUnavailablePlace(dftPand, storm::gspn::LayoutInfo(xcenter + 9.0,
                                                                                                     ycenter - 3.0));
                    builder.addOutputArc(tFailed, unavailablePlace);
                }

                uint64_t failSafePlace = builder.addPlace(defaultCapacity, 0, dftPand->name() + STR_FAILSAVE);
                builder.setPlaceLayoutInfo(failSafePlace, storm::gspn::LayoutInfo(xcenter - 3.0, ycenter - 3.0));

                builder.addInhibitionArc(failSafePlace, tFailed);

                // Transitions for failed place
                for (auto const &child : dftPand->children()) {
                    builder.addInputArc(getFailedPlace(child), tFailed);
                    builder.addOutputArc(tFailed, getFailedPlace(child));
                }
                // Transitions for fail-safe place
                for (uint64_t i = 1; i < dftPand->nrChildren(); ++i) {
                    auto const &child = dftPand->children().at(i);
                    uint64_t tFailSafe = builder.addImmediateTransition(getFailPriority(dftPand), 0.0,
                                                                        dftPand->name() + STR_FAILSAVING +
                                                                        std::to_string(i));
                    builder.setTransitionLayoutInfo(tFailSafe, storm::gspn::LayoutInfo(xcenter - 6.0 + i * 3.0,
                                                                                       ycenter + 3.0));

                    if (inclusive) {
                        builder.addInputArc(getFailedPlace(child), tFailSafe);
                        builder.addOutputArc(tFailSafe, getFailedPlace(child));
                        builder.addInhibitionArc(getFailedPlace(dftPand->children().at(i - 1)), tFailSafe);
                        builder.addOutputArc(tFailSafe, failSafePlace);
                        builder.addInhibitionArc(failSafePlace, tFailSafe);
                    } else {
                        // Delay mechanism for exclusive PAND
                        auto const &previousChild = dftPand->children().at(i - 1);
                        uint64_t delayPlace = builder.addPlace(1, 0,
                                                               dftPand->name() + "_delay_" + previousChild->name());
                        builder.setPlaceLayoutInfo(delayPlace, storm::gspn::LayoutInfo(xcenter - 5.0 + (i - 1) * 3.0,
                                                                                       ycenter + 5.0));
                        // Priority of delayTransitions needs to be lower than for failsafeTransitions
                        uint64_t tDelay = builder.addImmediateTransition(getFailPriority(dftPand) - 1, 0.0,
                                                                         child->name() + "_" + dftPand->name() +
                                                                         "_delayTransition");
                        builder.setTransitionLayoutInfo(tDelay,
                                                        storm::gspn::LayoutInfo(xcenter - 5.0 + (i - 1) * 3.0,
                                                                                ycenter + 3.0));
                        builder.addInputArc(getFailedPlace(previousChild), tDelay);
                        builder.addOutputArc(tDelay, getFailedPlace(dftPand->children().at(i - 1)));
                        builder.addOutputArc(tDelay, delayPlace);
                        builder.addInhibitionArc(delayPlace, tDelay);

                        builder.addInputArc(getFailedPlace(child), tFailSafe);
                        builder.addOutputArc(tFailSafe, getFailedPlace(child));
                        builder.addInhibitionArc(delayPlace, tFailSafe);
                        builder.addOutputArc(tFailSafe, failSafePlace);
                        builder.addInhibitionArc(failSafePlace, tFailSafe);
                    }
                }
                // Dont Care
                if (dontCareElements.count(dftPand->id())) {
                    //Propagation
                    uint64_t propagationPlace = builder.addPlace(1, 0, dftPand->name() + "_prop");
                    builder.setPlaceLayoutInfo(propagationPlace,
                                               storm::gspn::LayoutInfo(xcenter + 12.0, ycenter + 8.0));
                    uint64_t tPropagationFailed = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                 dftPand->name() + "_prop_fail");
                    builder.setTransitionLayoutInfo(tPropagationFailed,
                                                    storm::gspn::LayoutInfo(xcenter + 10.0, ycenter + 6.0));
                    uint64_t tPropagationFailsafe = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                   dftPand->name() + "_prop_failsafe");
                    builder.setTransitionLayoutInfo(tPropagationFailsafe,
                                                    storm::gspn::LayoutInfo(xcenter + 8.0, ycenter + 6.0));
                    builder.addInhibitionArc(propagationPlace, tPropagationFailed);
                    builder.addInputArc(failedPlace, tPropagationFailed);
                    builder.addOutputArc(tPropagationFailed, failedPlace);
                    builder.addOutputArc(tPropagationFailed, propagationPlace);

                    builder.addInhibitionArc(propagationPlace, tPropagationFailsafe);
                    builder.addInputArc(failSafePlace, tPropagationFailsafe);
                    builder.addOutputArc(tPropagationFailsafe, failSafePlace);
                    builder.addOutputArc(tPropagationFailsafe, propagationPlace);

                    //Connect children to propagation place
                    for (auto const &child : dftPand->children()) {
                        if (dontCareElements.count(child->id())) {
                            u_int64_t childDontCare = dontcareTransitions.at(child->id());
                            builder.addInputArc(propagationPlace, childDontCare);
                            builder.addOutputArc(childDontCare, propagationPlace);
                        }
                    }

                    if (dftPand->id() != mDft.getTopLevelIndex()) {
                        u_int64_t tDontCare = addDontcareTransition(dftPand,
                                                                    storm::gspn::LayoutInfo(xcenter + 16.0, ycenter));
                        if (!mergedDCFailed) {
                            uint64_t dontCarePlace = builder.addPlace(1, 0,
                                                                      dftPand->name() + STR_DONTCARE);
                            builder.setPlaceLayoutInfo(dontCarePlace,
                                                       storm::gspn::LayoutInfo(xcenter + 16.0, ycenter + 4.0));
                            builder.addInhibitionArc(dontCarePlace, tDontCare);
                            builder.addOutputArc(tDontCare, dontCarePlace);
                            uint64_t tPropagationDontCare = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                           dftPand->name() +
                                                                                           "_prop_dontCare");
                            builder.setTransitionLayoutInfo(tPropagationDontCare,
                                                            storm::gspn::LayoutInfo(xcenter + 14.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationDontCare);
                            builder.addInputArc(dontCarePlace, tPropagationDontCare);
                            builder.addOutputArc(tPropagationDontCare, dontCarePlace);
                            builder.addOutputArc(tPropagationDontCare, propagationPlace);

                        } else {
                            builder.addInhibitionArc(failedPlace, tDontCare);
                            builder.addOutputArc(tDontCare, failedPlace);
                        }
                    }
                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translatePOR(
                    std::shared_ptr<storm::storage::DFTPor<ValueType> const> dftPor, bool inclusive) {

                double xcenter = mDft.getElementLayoutInfo(dftPor->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftPor->id()).y;

                uint64_t delayPlace = 0;

                uint64_t failedPlace = addFailedPlace(dftPor, storm::gspn::LayoutInfo(xcenter + 3.0, ycenter - 3.0));

                // Set priority lower if the POR is exclusive
                uint64_t tFailed = builder.addImmediateTransition(
                        inclusive ? getFailPriority(dftPor) : getFailPriority(dftPor) - 1, 0.0,
                        dftPor->name() + STR_FAILING);
                builder.setTransitionLayoutInfo(tFailed, storm::gspn::LayoutInfo(xcenter + 3.0, ycenter + 3.0));
                builder.addOutputArc(tFailed, failedPlace);
                builder.addInhibitionArc(failedPlace, tFailed);

                // Arcs from first child
                builder.addInputArc(getFailedPlace(dftPor->children().front()), tFailed);
                builder.addOutputArc(tFailed, getFailedPlace(dftPor->children().front()));

                if (!smart || mDft.isRepresentative(dftPor->id())) {
                    uint64_t unavailablePlace = addUnavailablePlace(dftPor, storm::gspn::LayoutInfo(xcenter + 9.0,
                                                                                                    ycenter - 3.0));
                    builder.addOutputArc(tFailed, unavailablePlace);
                }

                uint64_t failSafePlace = builder.addPlace(defaultCapacity, 0, dftPor->name() + STR_FAILSAVE);
                builder.setPlaceLayoutInfo(failSafePlace, storm::gspn::LayoutInfo(xcenter - 3.0, ycenter - 3.0));

                builder.addInhibitionArc(failSafePlace, tFailed);

                if (!inclusive) {
                    // Setup delay mechanism if necessary
                    delayPlace = builder.addPlace(1, 0, dftPor->name() + "_delay");
                    builder.setPlaceLayoutInfo(delayPlace, storm::gspn::LayoutInfo(xcenter - 5.0,
                                                                                   ycenter + 5.0));

                    // priority of delayTransition has to be lower than other priorities
                    uint64_t tDelay = builder.addImmediateTransition(getFailPriority(dftPor) - 1, 0.0, dftPor->name() +
                                                                                                       "_delayTransition");
                    builder.setTransitionLayoutInfo(tDelay,
                                                    storm::gspn::LayoutInfo(xcenter - 5.0,
                                                                            ycenter + 3.0));

                    builder.addInputArc(getFailedPlace(dftPor->children().front()), tDelay);
                    builder.addOutputArc(tDelay, getFailedPlace(dftPor->children().front()));
                    builder.addOutputArc(tDelay, delayPlace);
                    builder.addInhibitionArc(delayPlace, tDelay);
                }

                // For all children except the first one
                for (size_t i = 1; i < dftPor->nrChildren(); ++i) {
                    auto const &child = dftPor->children().at(i);
                    uint64_t tFailSafe = builder.addImmediateTransition(getFailPriority(dftPor), 0.0,
                                                                        dftPor->name() + STR_FAILSAVING +
                                                                        std::to_string(i));
                    builder.setTransitionLayoutInfo(tFailSafe, storm::gspn::LayoutInfo(xcenter - 3.0 + i * 3.0,
                                                                                       ycenter + 3.0));

                    builder.addInputArc(getFailedPlace(child), tFailSafe);
                    builder.addOutputArc(tFailSafe, getFailedPlace(child));
                    builder.addOutputArc(tFailSafe, failSafePlace);
                    builder.addInhibitionArc(failSafePlace, tFailSafe);
                    if (inclusive) {
                        builder.addInhibitionArc(getFailedPlace(dftPor->children().front()), tFailSafe);
                    } else {
                        builder.addInhibitionArc(delayPlace, tFailSafe);
                    }
                }

                // Dont Care
                if (dontCareElements.count(dftPor->id())) {
                    //Propagation
                    uint64_t propagationPlace = builder.addPlace(1, 0, dftPor->name() + "_prop");
                    builder.setPlaceLayoutInfo(propagationPlace,
                                               storm::gspn::LayoutInfo(xcenter + 12.0, ycenter + 8.0));
                    uint64_t tPropagationFailed = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                 dftPor->name() + "_prop_fail");
                    builder.setTransitionLayoutInfo(tPropagationFailed,
                                                    storm::gspn::LayoutInfo(xcenter + 10.0, ycenter + 6.0));
                    uint64_t tPropagationFailsafe = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                   dftPor->name() + "_prop_failsafe");
                    builder.setTransitionLayoutInfo(tPropagationFailsafe,
                                                    storm::gspn::LayoutInfo(xcenter + 8.0, ycenter + 6.0));
                    builder.addInhibitionArc(propagationPlace, tPropagationFailed);
                    builder.addInputArc(failedPlace, tPropagationFailed);
                    builder.addOutputArc(tPropagationFailed, failedPlace);
                    builder.addOutputArc(tPropagationFailed, propagationPlace);

                    builder.addInhibitionArc(propagationPlace, tPropagationFailsafe);
                    builder.addInputArc(failSafePlace, tPropagationFailsafe);
                    builder.addOutputArc(tPropagationFailsafe, failSafePlace);
                    builder.addOutputArc(tPropagationFailsafe, propagationPlace);

                    //Connect children to propagation place
                    for (auto const &child : dftPor->children()) {
                        if (dontCareElements.count(child->id())) {
                            u_int64_t childDontCare = dontcareTransitions.at(child->id());
                            builder.addInputArc(propagationPlace, childDontCare);
                            builder.addOutputArc(childDontCare, propagationPlace);
                        }
                    }

                    if (dftPor->id() != mDft.getTopLevelIndex()) {
                        u_int64_t tDontCare = addDontcareTransition(dftPor,
                                                                    storm::gspn::LayoutInfo(xcenter + 16.0, ycenter));
                        if (!mergedDCFailed) {
                            uint64_t dontCarePlace = builder.addPlace(1, 0,
                                                                      dftPor->name() + STR_DONTCARE);
                            builder.setPlaceLayoutInfo(dontCarePlace,
                                                       storm::gspn::LayoutInfo(xcenter + 16.0, ycenter + 4.0));
                            builder.addInhibitionArc(dontCarePlace, tDontCare);
                            builder.addOutputArc(tDontCare, dontCarePlace);
                            uint64_t tPropagationDontCare = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                           dftPor->name() +
                                                                                           "_prop_dontCare");
                            builder.setTransitionLayoutInfo(tPropagationDontCare,
                                                            storm::gspn::LayoutInfo(xcenter + 14.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationDontCare);
                            builder.addInputArc(dontCarePlace, tPropagationDontCare);
                            builder.addOutputArc(tPropagationDontCare, dontCarePlace);
                            builder.addOutputArc(tPropagationDontCare, propagationPlace);

                        } else {
                            builder.addInhibitionArc(failedPlace, tDontCare);
                            builder.addOutputArc(tDontCare, failedPlace);
                        }
                    }
                }

            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateSPARE(
                    std::shared_ptr<storm::storage::DFTSpare<ValueType> const> dftSpare) {
                double xcenter = mDft.getElementLayoutInfo(dftSpare->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftSpare->id()).y;

                uint64_t failedPlace = addFailedPlace(dftSpare, storm::gspn::LayoutInfo(xcenter + 10.0, ycenter - 8.0));

                bool isRepresentative = mDft.isRepresentative(dftSpare->id());
                uint64_t unavailablePlace = 0;
                if (!smart || isRepresentative) {
                    unavailablePlace = addUnavailablePlace(dftSpare,
                                                           storm::gspn::LayoutInfo(xcenter + 16.0, ycenter - 8.0));
                }

                uint64_t activePlace = builder.addPlace(defaultCapacity, isActiveInitially(dftSpare) ? 1 : 0,
                                                        dftSpare->name() + STR_ACTIVATED);
                builder.setPlaceLayoutInfo(activePlace, storm::gspn::LayoutInfo(xcenter - 20.0, ycenter - 12.0));
                activePlaces.emplace(dftSpare->id(), activePlace);

                std::vector<uint64_t> tNextClaims;
                std::vector<uint64_t> tNextConsiders;
                for (size_t i = 0; i < dftSpare->nrChildren(); ++i) {
                    auto const &child = dftSpare->children().at(i);
                    // Consider next child
                    size_t considerPlace = builder.addPlace(defaultCapacity, i == 0 ? 1 : 0,
                                                            dftSpare->name() + "_consider_" + child->name());
                    builder.setPlaceLayoutInfo(considerPlace,
                                               storm::gspn::LayoutInfo(xcenter - 15.0 + i * 14.0, ycenter - 8.0));

                    if (i > 0) {
                        // Set output transition from previous next_claim
                        builder.addOutputArc(tNextClaims.back(), considerPlace);
                        // Set output transition from previous cannot_claim
                        builder.addOutputArc(tNextConsiders.back(), considerPlace);
                    }

                    // Cannot claim child
                    uint64_t tConsiderNext = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0,
                                                                            dftSpare->name() + "_cannot_claim_" +
                                                                            child->name());
                    builder.setTransitionLayoutInfo(tConsiderNext,
                                                    storm::gspn::LayoutInfo(xcenter - 7.0 + i * 14.0, ycenter - 8.0));
                    builder.addInputArc(considerPlace, tConsiderNext);
                    builder.addInputArc(unavailablePlaces.at(child->id()), tConsiderNext);
                    builder.addOutputArc(tConsiderNext, unavailablePlaces.at(child->id()));
                    tNextConsiders.push_back(tConsiderNext);

                    // Claimed child
                    size_t claimedPlace = builder.addPlace(defaultCapacity, 0,
                                                           dftSpare->name() + "_claimed_" + child->name());
                    builder.setPlaceLayoutInfo(claimedPlace,
                                               storm::gspn::LayoutInfo(xcenter - 15.0 + i * 14.0, ycenter + 5.0));
                    uint64_t tClaim = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0,
                                                                     dftSpare->name() + "_claim_" + child->name());
                    builder.setTransitionLayoutInfo(tClaim,
                                                    storm::gspn::LayoutInfo(xcenter - 15.0 + i * 14.0, ycenter));
                    builder.addInhibitionArc(unavailablePlaces.at(child->id()), tClaim);
                    builder.addInputArc(considerPlace, tClaim);
                    builder.addOutputArc(tClaim, claimedPlace);
                    builder.addOutputArc(tClaim, unavailablePlaces.at(child->id()));

                    // Claim next
                    uint64_t tClaimNext = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0,
                                                                         dftSpare->name() + "_next_claim_" +
                                                                         std::to_string(i));
                    builder.setTransitionLayoutInfo(tClaimNext,
                                                    storm::gspn::LayoutInfo(xcenter - 7.0 + i * 14.0, ycenter + 5.0));
                    builder.addInputArc(claimedPlace, tClaimNext);
                    builder.addInputArc(getFailedPlace(child), tClaimNext);
                    builder.addOutputArc(tClaimNext, getFailedPlace(child));
                    tNextClaims.push_back(tClaimNext);

                    // Activate all elements in spare module
                    uint64_t l = 0;
                    for (uint64_t k : mDft.module(child->id())) {
                        uint64_t tActivate = builder.addImmediateTransition(getFailPriority(dftSpare), 0.0,
                                                                            dftSpare->name() + "_activate_" +
                                                                            std::to_string(i) + "_" +
                                                                            std::to_string(k));
                        builder.setTransitionLayoutInfo(tActivate, storm::gspn::LayoutInfo(xcenter - 18.0 + (i + l) * 3,
                                                                                           ycenter - 12.0));
                        builder.addInhibitionArc(activePlaces.at(k), tActivate);
                        builder.addInputArc(claimedPlace, tActivate);
                        builder.addInputArc(activePlace, tActivate);
                        builder.addOutputArc(tActivate, claimedPlace);
                        builder.addOutputArc(tActivate, activePlace);
                        builder.addOutputArc(tActivate, activePlaces.at(k));
                        ++l;
                    }
                }

                // Set arcs to failed
                builder.addOutputArc(tNextConsiders.back(), failedPlace);
                builder.addOutputArc(tNextClaims.back(), failedPlace);

                // Don't Care Mechanism
                if (dontCareElements.count(dftSpare->id())) {
                    if (dftSpare->id() != mDft.getTopLevelIndex()) {
                        u_int64_t tDontCare = addDontcareTransition(dftSpare,
                                                                    storm::gspn::LayoutInfo(xcenter + 16.0, ycenter));
                        if (!mergedDCFailed) {
                            uint64_t dontCarePlace = builder.addPlace(1, 0,
                                                                      dftSpare->name() + STR_DONTCARE);
                            builder.setPlaceLayoutInfo(dontCarePlace,
                                                       storm::gspn::LayoutInfo(xcenter + 16.0, ycenter + 4.0));
                            builder.addInhibitionArc(dontCarePlace, tDontCare);
                            builder.addOutputArc(tDontCare, dontCarePlace);
                            //Propagation
                            uint64_t propagationPlace = builder.addPlace(1, 0,
                                                                         dftSpare->name() + "_prop");
                            builder.setPlaceLayoutInfo(propagationPlace,
                                                       storm::gspn::LayoutInfo(xcenter + 12.0, ycenter + 8.0));
                            uint64_t tPropagationFailed = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                         dftSpare->name() +
                                                                                         "_prop_fail");
                            builder.setTransitionLayoutInfo(tPropagationFailed,
                                                            storm::gspn::LayoutInfo(xcenter + 10.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationFailed);
                            builder.addInputArc(failedPlace, tPropagationFailed);
                            builder.addOutputArc(tPropagationFailed, failedPlace);
                            builder.addOutputArc(tPropagationFailed, propagationPlace);
                            uint64_t tPropagationDontCare = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                                           dftSpare->name() +
                                                                                           "_prop_dontCare");
                            builder.setTransitionLayoutInfo(tPropagationDontCare,
                                                            storm::gspn::LayoutInfo(xcenter + 14.0, ycenter + 6.0));
                            builder.addInhibitionArc(propagationPlace, tPropagationDontCare);
                            builder.addInputArc(dontCarePlace, tPropagationDontCare);
                            builder.addOutputArc(tPropagationDontCare, dontCarePlace);
                            builder.addOutputArc(tPropagationDontCare, propagationPlace);
                            for (auto const &child : dftSpare->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(propagationPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, propagationPlace);
                                }
                            }
                        } else {
                            builder.addInhibitionArc(failedPlace, tDontCare);
                            builder.addOutputArc(tDontCare, failedPlace);
                            for (auto const &child : dftSpare->children()) {
                                if (dontCareElements.count(child->id())) {
                                    u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                    builder.addInputArc(failedPlace, childDontCare);
                                    builder.addOutputArc(childDontCare, failedPlace);
                                }
                            }
                        }
                    } else {
                        // If SPARE is TLE, simple failure propagation suffices
                        for (auto const &child : dftSpare->children()) {
                            if (dontCareElements.count(child->id())) {
                                u_int64_t childDontCare = dontcareTransitions.at(child->id());
                                builder.addInputArc(failedPlace, childDontCare);
                                builder.addOutputArc(childDontCare, failedPlace);
                            }
                        }
                    }
                }

                if (!smart || isRepresentative) {
                    builder.addOutputArc(tNextConsiders.back(), unavailablePlace);
                    builder.addOutputArc(tNextClaims.back(), unavailablePlace);
                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translatePDEP(
                    std::shared_ptr<storm::storage::DFTDependency<ValueType> const> dftDependency) {
                double xcenter = mDft.getElementLayoutInfo(dftDependency->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftDependency->id()).y;

                uint64_t failedPlace = 0;
                if (!smart) {
                    failedPlace = addFailedPlace(dftDependency, storm::gspn::LayoutInfo(xcenter + 10.0, ycenter - 8.0));
                    addUnavailablePlace(dftDependency, storm::gspn::LayoutInfo(xcenter + 16.0, ycenter - 8.0));
                }

                uint64_t forwardPlace = 0;
                if (dftDependency->probability() < 1.0) {
                    // PDEP
                    forwardPlace = builder.addPlace(defaultCapacity, 0, dftDependency->name() + "_forward");
                    builder.setPlaceLayoutInfo(forwardPlace, storm::gspn::LayoutInfo(xcenter + 1.0, ycenter + 2.0));

                    uint64_t coinPlace = builder.addPlace(defaultCapacity, 1, dftDependency->name() + "_coin");
                    builder.setPlaceLayoutInfo(coinPlace, storm::gspn::LayoutInfo(xcenter - 5.0, ycenter + 2.0));

                    uint64_t tStartFlip = builder.addImmediateTransition(getFailPriority(dftDependency), 0.0,
                                                                         dftDependency->name() + "_start_flip");
                    builder.addInputArc(coinPlace, tStartFlip);
                    builder.addInputArc(getFailedPlace(dftDependency->triggerEvent()), tStartFlip);
                    builder.addOutputArc(tStartFlip, getFailedPlace(dftDependency->triggerEvent()));

                    uint64_t flipPlace = builder.addPlace(defaultCapacity, 0, dftDependency->name() + "_flip");
                    builder.setPlaceLayoutInfo(flipPlace, storm::gspn::LayoutInfo(xcenter - 2.0, ycenter + 2.0));
                    builder.addOutputArc(tStartFlip, flipPlace);

                    uint64_t tWinFlip = builder.addImmediateTransition(getFailPriority(dftDependency) + 1,
                                                                       dftDependency->probability(),
                                                                       "_win_flip");
                    builder.addInputArc(flipPlace, tWinFlip);
                    builder.addOutputArc(tWinFlip, forwardPlace);

                    uint64_t tLooseFlip = builder.addImmediateTransition(getFailPriority(dftDependency) + 1,
                                                                         storm::utility::one<ValueType>() -
                                                                         dftDependency->probability(), "_loose_flip");
                    builder.addInputArc(flipPlace, tLooseFlip);
                } else {
                    // FDEP
                    forwardPlace = getFailedPlace(dftDependency->triggerEvent());
                }

                for (auto const &child : dftDependency->dependentEvents()) {
                    uint64_t tForwardFailure = builder.addImmediateTransition(getFailPriority(dftDependency), 0.0,
                                                                              dftDependency->name() + "_propagate_" +
                                                                              child->name());
                    builder.addInputArc(forwardPlace, tForwardFailure);
                    builder.addOutputArc(tForwardFailure, forwardPlace);
                    builder.addOutputArc(tForwardFailure, getFailedPlace(child));
                    builder.addInhibitionArc(getFailedPlace(child), tForwardFailure);
                    if (!smart || child->nrRestrictions() > 0) {
                        builder.addInhibitionArc(disabledPlaces.at(child->id()), tForwardFailure);
                    }
                    if (!smart || mDft.isRepresentative(child->id())) {
                        builder.addOutputArc(tForwardFailure, unavailablePlaces.at(child->id()));
                    }
                }

                // Don't Care
                if (dontCareElements.count(dftDependency->id())) {
                    u_int64_t tDontCare = addDontcareTransition(dftDependency,
                                                                storm::gspn::LayoutInfo(xcenter + 3.0, ycenter));
                    if (!mergedDCFailed) {
                        u_int64_t dontCarePlace = builder.addPlace(1, 0,
                                                                   dftDependency->name() + STR_DONTCARE);
                        builder.setPlaceLayoutInfo(dontCarePlace, storm::gspn::LayoutInfo(xcenter + 4.0, ycenter));
                        builder.addInhibitionArc(dontCarePlace, tDontCare);
                        builder.addOutputArc(tDontCare, dontCarePlace);
                        // Add the arcs for the dependent events
                        for (auto const &dependentEvent : dftDependency->dependentEvents()) {
                            if (dontCareElements.count(dependentEvent->id())) {
                                u_int64_t dependentEventPropagation = dependencyPropagationPlaces.at(
                                        dependentEvent->id());
                                builder.addInputArc(dependentEventPropagation, tDontCare);
                                builder.addOutputArc(tDontCare, dependentEventPropagation);
                            }
                        }
                        // Add the arcs for the trigger
                        uint64_t triggerDontCare = dontcareTransitions.at(dftDependency->triggerEvent()->id());
                        builder.addInputArc(dontCarePlace, triggerDontCare);
                        builder.addOutputArc(triggerDontCare, dontCarePlace);
                    } else {
                        if (failedPlace == 0) {
                            failedPlace = addFailedPlace(dftDependency,
                                                         storm::gspn::LayoutInfo(xcenter + 4.0, ycenter));
                        }
                        builder.addInhibitionArc(failedPlace, tDontCare);
                        builder.addOutputArc(tDontCare, failedPlace);

                        // Add the arcs for the dependent events
                        for (auto const &dependentEvent : dftDependency->dependentEvents()) {
                            if (dontCareElements.count(dependentEvent->id())) {
                                u_int64_t dependentEventFailed = failedPlaces.at(dependentEvent->id());
                                builder.addInputArc(dependentEventFailed, tDontCare);
                                builder.addOutputArc(tDontCare, dependentEventFailed);
                            }
                        }
                        // Add the arcs for the trigger
                        uint64_t triggerDontCare = dontcareTransitions.at(dftDependency->triggerEvent()->id());
                        builder.addInputArc(failedPlace, triggerDontCare);
                        builder.addOutputArc(triggerDontCare, failedPlace);
                    }

                }
            }

            template<typename ValueType>
            void DftToGspnTransformator<ValueType>::translateSeq(
                    std::shared_ptr<storm::storage::DFTSeq<ValueType> const> dftSeq) {
                STORM_LOG_THROW(dftSeq->allChildrenBEs(), storm::exceptions::NotImplementedException,
                                "Sequence enforcers with gates as children are currently not supported");
                double xcenter = mDft.getElementLayoutInfo(dftSeq->id()).x;
                double ycenter = mDft.getElementLayoutInfo(dftSeq->id()).y;

                if (!smart) {
                    addFailedPlace(dftSeq, storm::gspn::LayoutInfo(xcenter + 10.0, ycenter - 8.0));
                    addUnavailablePlace(dftSeq, storm::gspn::LayoutInfo(xcenter + 16.0, ycenter - 8.0));
                }

                uint64_t tEnable = 0;
                uint64_t nextPlace = 0;
                for (size_t i = 0; i < dftSeq->nrChildren(); ++i) {
                    auto const &child = dftSeq->children().at(i);

                    nextPlace = builder.addPlace(defaultCapacity, i == 0 ? 1 : 0,
                                                 dftSeq->name() + "_next_" + child->name());
                    builder.setPlaceLayoutInfo(nextPlace,
                                               storm::gspn::LayoutInfo(xcenter - 5.0 + i * 3.0, ycenter - 3.0));

                    if (i > 0) {
                        builder.addOutputArc(tEnable, nextPlace);
                    }
                    tEnable = builder.addImmediateTransition(getFailPriority(dftSeq), 0.0,
                                                             dftSeq->name() + "_unblock_" + child->name());
                    builder.setTransitionLayoutInfo(tEnable,
                                                    storm::gspn::LayoutInfo(xcenter - 5.0 + i * 3.0, ycenter + 3.0));
                    builder.addInputArc(nextPlace, tEnable);
                    builder.addInputArc(disabledPlaces.at(child->id()), tEnable);
                    if (i > 0) {
                        builder.addInputArc(getFailedPlace(dftSeq->children().at(i - 1)), tEnable);
                    }
                }

            }

            template<typename ValueType>
            uint64_t
            DftToGspnTransformator<ValueType>::addFailedPlace(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement,
                    storm::gspn::LayoutInfo const &layoutInfo,
                    bool initialFailed) {
                uint64_t failedPlace = builder.addPlace(defaultCapacity, initialFailed ? 1 : 0,
                                                        dftElement->name() + STR_FAILED);
                assert(failedPlaces.size() == dftElement->id());
                failedPlaces.push_back(failedPlace);
                builder.setPlaceLayoutInfo(failedPlace, layoutInfo);
                return failedPlace;
            }


            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::addUnavailablePlace(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement,
                    storm::gspn::LayoutInfo const &layoutInfo, bool initialAvailable) {
                uint64_t unavailablePlace = builder.addPlace(defaultCapacity, initialAvailable ? 0 : 1,
                                                             dftElement->name() + "_unavail");
                unavailablePlaces.emplace(dftElement->id(), unavailablePlace);
                builder.setPlaceLayoutInfo(unavailablePlace, layoutInfo);
                return unavailablePlace;
            }

            template<typename ValueType>
            uint64_t
            DftToGspnTransformator<ValueType>::addDisabledPlace(
                    std::shared_ptr<const storm::storage::DFTBE<ValueType> > dftBe,
                    storm::gspn::LayoutInfo const &layoutInfo) {
                uint64_t disabledPlace = builder.addPlace(dftBe->nrRestrictions(), dftBe->nrRestrictions(),
                                                          dftBe->name() + "_dabled");
                disabledPlaces.emplace(dftBe->id(), disabledPlace);
                builder.setPlaceLayoutInfo(disabledPlace, layoutInfo);
                return disabledPlace;
            }

            template<typename ValueType>
            uint64_t
            DftToGspnTransformator<ValueType>::addDontcareTransition(
                    std::shared_ptr<const storm::storage::DFTElement<ValueType> > dftElement,
                    storm::gspn::LayoutInfo const &layoutInfo) {
                uint64_t dontcareTransition = builder.addImmediateTransition(dontCarePriority, 0.0,
                                                                             dftElement->name() + STR_DONTCARE +
                                                                             "_transition");
                dontcareTransitions.emplace(dftElement->id(), dontcareTransition);
                builder.setTransitionLayoutInfo(dontcareTransition, layoutInfo);
                return dontcareTransition;
            }

            template<typename ValueType>
            bool DftToGspnTransformator<ValueType>::isActiveInitially(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement) {
                // If element is in the top module, return true.
                return !mDft.hasRepresentant(dftElement->id());
            }

            template<typename ValueType>
            uint64_t DftToGspnTransformator<ValueType>::getFailPriority(
                    std::shared_ptr<storm::storage::DFTElement<ValueType> const> dftElement) {
                // Return the value given in the field
                return priorities.at(dftElement->id());
            }


            // Explicitly instantiate the class.
            template
            class DftToGspnTransformator<double>;

#ifdef STORM_HAVE_CARL
            // template class DftToGspnTransformator<storm::RationalFunction>;
#endif

        } // namespace dft
    } // namespace transformations
} // namespace storm


