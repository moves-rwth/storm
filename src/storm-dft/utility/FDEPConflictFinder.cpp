#include "FDEPConflictFinder.h"

namespace storm {
    namespace dft {
        namespace utility {

            template<>
            std::vector<std::pair<uint64_t, uint64_t>> FDEPConflictFinder<double>::getDependencyConflicts(storm::storage::DFT<double> const& dft, bool useSMT,
                                                                                                          uint_fast64_t timeout) {

                std::shared_ptr<storm::modelchecker::DFTASFChecker> smtChecker = nullptr;
                if (useSMT) {
                    storm::modelchecker::DFTASFChecker checker(dft);
                    smtChecker = std::make_shared<storm::modelchecker::DFTASFChecker>(checker);
                    smtChecker->toSolver();
                }

                std::vector<std::pair<uint64_t, uint64_t>> res;
                uint64_t dep1Index;
                uint64_t dep2Index;
                for (size_t i = 0; i < dft.getDependencies().size(); ++i) {
                    dep1Index = dft.getDependencies().at(i);
                    for (size_t j = i + 1; j < dft.getDependencies().size(); ++j) {
                        dep2Index = dft.getDependencies().at(j);
                        if (dft.getDynamicBehavior()[dep1Index] && dft.getDynamicBehavior()[dep2Index]) {
                            if (useSMT) { // if an SMT solver is to be used
                                if (dft.getDependency(dep1Index)->triggerEvent() == dft.getDependency(dep2Index)->triggerEvent()) {
                                    STORM_LOG_TRACE("Conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name() << ": Same trigger");
                                    res.emplace_back(std::pair<uint64_t, uint64_t>(dep1Index, dep2Index));
                                } else {
                                    switch (smtChecker->checkDependencyConflict(dep1Index, dep2Index, timeout)) {
                                        case storm::solver::SmtSolver::CheckResult::Sat:
                                            STORM_LOG_TRACE("Conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name());
                                            res.emplace_back(std::pair<uint64_t, uint64_t>(dep1Index, dep2Index));
                                            break;
                                        case storm::solver::SmtSolver::CheckResult::Unknown:
                                            STORM_LOG_TRACE("Unknown: Conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name());
                                            res.emplace_back(std::pair<uint64_t, uint64_t>(dep1Index, dep2Index));
                                            break;
                                        default:
                                            STORM_LOG_TRACE("No conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name());
                                            break;
                                    }
                                }
                            } else {
                                STORM_LOG_TRACE("Conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name());
                                res.emplace_back(std::pair<uint64_t, uint64_t>(dep1Index, dep2Index));
                            }
                        } else {
                            STORM_LOG_TRACE("Static behavior: No conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name());
                            break;
                        }
                    }
                }
                return res;
            }

            template<>
            std::vector<std::pair<uint64_t, uint64_t>> FDEPConflictFinder<storm::RationalFunction>::getDependencyConflicts(storm::storage::DFT<storm::RationalFunction> const& dft,
                                                                                                                           bool useSMT, uint_fast64_t timeout) {
                if (useSMT) {
                    STORM_LOG_WARN("SMT encoding for rational functions is not supported");
                }

                std::vector<std::pair<uint64_t, uint64_t>> res;
                uint64_t dep1Index;
                uint64_t dep2Index;
                for (size_t i = 0; i < dft.getDependencies().size(); ++i) {
                    dep1Index = dft.getDependencies().at(i);
                    for (size_t j = i + 1; j < dft.getDependencies().size(); ++j) {
                        dep2Index = dft.getDependencies().at(j);
                        if (dft.getDynamicBehavior()[dep1Index] && dft.getDynamicBehavior()[dep2Index]) {
                            STORM_LOG_TRACE("Conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name());
                            res.emplace_back(std::pair<uint64_t, uint64_t>(dep1Index, dep2Index));
                        } else {
                            STORM_LOG_TRACE("Static behavior: No conflict between " << dft.getElement(dep1Index)->name() << " and " << dft.getElement(dep2Index)->name());
                            break;
                        }
                    }
                }
                return res;
            }

            template
            class FDEPConflictFinder<double>;

            template
            class FDEPConflictFinder<storm::RationalFunction>;
        }
    }
}
