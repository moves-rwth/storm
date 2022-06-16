#pragma once
#include <iostream>
#include <map>
#include <unordered_map>

#include "storm/utility/macros.h"

#include "storm-dft/storage/DFTLayoutInfo.h"
#include "storm-dft/storage/elements/DFTElements.h"
#include "storm-dft/storage/elements/DFTRestriction.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm::storage {

// Forward declaration
template<typename ValueType>
class DFT;
}  // namespace storm::storage

namespace storm::dft {
namespace builder {

template<typename ValueType>
class DFTBuilder {
    using DFTElementPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>;
    using DFTElementCPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const>;
    using DFTElementVector = std::vector<DFTElementPointer>;
    using DFTBEPointer = std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType>>;
    using DFTBECPointer = std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>;
    using DFTChildrenCPointer = std::shared_ptr<storm::dft::storage::elements::DFTChildren<ValueType> const>;
    using DFTGatePointer = std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>>;
    using DFTGateCPointer = std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType> const>;
    using DFTGateVector = std::vector<DFTGatePointer>;
    using DFTDependencyPointer = std::shared_ptr<storm::dft::storage::elements::DFTDependency<ValueType>>;
    using DFTRestrictionPointer = std::shared_ptr<storm::dft::storage::elements::DFTRestriction<ValueType>>;

   private:
    std::size_t mNextId = 0;
    static std::size_t mUniqueOffset;  // Used to ensure unique names
    std::string mTopLevelIdentifier;
    std::unordered_map<std::string, DFTElementPointer> mElements;
    std::unordered_map<DFTElementPointer, std::vector<std::string>> mChildNames;
    std::unordered_map<DFTRestrictionPointer, std::vector<std::string>> mRestrictionChildNames;
    std::unordered_map<DFTDependencyPointer, std::vector<std::string>> mDependencyChildNames;
    std::unordered_map<std::string, storm::dft::storage::DFTLayoutInfo> mLayoutInfo;

   public:
    DFTBuilder() = default;

    /*!
     * Create BE which is constant failed or constant failsafe and add it to DFT.
     * @param name Name.
     * @param failed Whether the BE is constant failed.
     * @return True iff adding was successful.
     */
    bool addBasicElementConst(std::string const& name, bool failed);

    /*!
     * Create BE with constant (Bernoulli) distribution and add it to DFT.
     * @param name Name.
     * @param probability (Constant) probability that BE is failed.
     * @param dormancyFactor Dormancy factor in passive mode.
     * @param transient Whether the failure is transient.
     * @return True iff adding was successful.
     */
    bool addBasicElementProbability(std::string const& name, ValueType probability, ValueType dormancyFactor);

    /*!
     * Create BE with exponential distribution and add it to DFT.
     * @param name Name.
     * @param rate Failure rate governing failure distribution of BE.
     * @param dormancyFactor Dormancy factor in passive mode.
     * @param transient Whether the failure is transient.
     * @return True iff adding was successful.
     */
    bool addBasicElementExponential(std::string const& name, ValueType rate, ValueType dormancyFactor, bool transient = false);

    /*!
     * Create BE with distribution given by sample points and add it to DFT.
     * @param name Name.
     * @param activeSamples Sample points defining the unreliability at certain time points (i.e. the cumulative distribution function F(x)).
     * @return True iff adding was successful.
     */
    bool addBasicElementSamples(std::string const& name, std::map<ValueType, ValueType> const& activeSamples);

    /*!
     * Create AND-gate and add it to DFT.
     * @param name Name.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addAndGate(std::string const& name, std::vector<std::string> const& children);

    /*!
     * Create OR-gate and add it to DFT.
     * @param name Name.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addOrGate(std::string const& name, std::vector<std::string> const& children);

    /*!
     * Create VOTing-gate and add it to DFT.
     * @param name Name.
     * @param threshold Threshold specifying how many children need to fail.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addVotingGate(std::string const& name, unsigned threshold, std::vector<std::string> const& children);

    /*!
     * Create PAND-gate and add it to DFT.
     * @param name Name.
     * @param children Names of children.
     * @param inclusive Whether the failure behaviour is inclusive (simultaneous failures are allowed) or not.
     * @return True iff adding was successful.
     */
    bool addPandGate(std::string const& name, std::vector<std::string> const& children, bool inclusive = true);

    /*!
     * Create POR-gate and add it to DFT.
     * @param name Name.
     * @param children Names of children.
     * @param inclusive Whether the failure behaviour is inclusive (simultaneous failures are allowed) or not.
     * @return True iff adding was successful.
     */
    bool addPorGate(std::string const& name, std::vector<std::string> const& children, bool inclusive = true);

    /*!
     * Create SPARE-gate and add it to DFT.
     * @param name Name.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addSpareGate(std::string const& name, std::vector<std::string> const& children);

    /*!
     * Create sequence enforcer (SEQ) and add it to DFT.
     * @param name Name.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addSequenceEnforcer(std::string const& name, std::vector<std::string> const& children);

    /*!
     * Create mutual exclusion-gate (MUTEX) and add it to DFT.
     * @param name Name.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addMutex(std::string const& name, std::vector<std::string> const& children);

    /*!
     * Create (probabilistic) dependency (PDEP) and add it to DFT.
     * @param name Name.
     * @param children Names of children. First child is the trigger event, all remaining children are the dependent events.
     * @param probability Probability of forwarding the failure. Probability 1 corresponds to an FDEP.
     * @return True iff adding was successful.
     */
    bool addPdep(std::string const& name, std::vector<std::string> const& children, ValueType probability);

    void addLayoutInfo(std::string const& name, double x, double y) {
        if (!nameInUse(name)) {
            STORM_LOG_ERROR("Element with name '" << name << "' not found.");
        }
        mLayoutInfo[name] = storm::dft::storage::DFTLayoutInfo(x, y);
    }

    bool setTopLevel(std::string const& tle) {
        mTopLevelIdentifier = tle;
        return nameInUse(tle);
    }

    /**
     * Check whether the name is already used.
     * @param name Element name.
     * @return True iff name is already in use.
     */
    bool nameInUse(std::string const& name) {
        return mElements.find(name) != mElements.end();
    }

    std::string getUniqueName(std::string name);

    storm::dft::storage::DFT<ValueType> build();

    /**
     * Copy element and insert it again in the builder.
     *
     * @param element Element to copy.
     */
    void copyElement(DFTElementCPointer element);

    /**
     * Copy BE and insert it again in the builder.i
     *
     * @param be BE to copy.
     */
    void copyBE(DFTBECPointer be);

    /**
     * Copy gate with given children and insert it again in the builder. The current children of the element
     * are discarded.
     *
     * @param gate Gate to copy.
     * @param children New children of copied element.
     */
    void copyGate(DFTGateCPointer gate, std::vector<std::string> const& children);

   private:
    unsigned computeRank(DFTElementPointer const& elem);

    /*!
     * Add given element to DFT.
     * @param element DFT element.
     * @return True iff adding was successful.
     */
    bool addElement(DFTElementPointer element);

    /*!
     * Add given gate to DFT.
     * @param gate Gate.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addGate(DFTGatePointer gate, std::vector<std::string> const& children);

    /*!
     * Add given dependency to DFT.
     * @param dependency Dependency.
     * @parama children Names of children.
     * @return True iff adding was successful.
     */
    bool addDependency(DFTDependencyPointer dependency, std::vector<std::string> const& children);

    /*!
     * Add given restriction to DFT.
     * @param restriction Restriction.
     * @param children Names of children.
     * @return True iff adding was successful.
     */
    bool addRestriction(DFTRestrictionPointer restriction, std::vector<std::string> const& children);

    enum class topoSortColour { WHITE, BLACK, GREY };

    void topoVisit(DFTElementPointer const& n, std::map<DFTElementPointer, topoSortColour, storm::dft::storage::OrderElementsById<ValueType>>& visited,
                   DFTElementVector& L);

    DFTElementVector topoSort();
};

}  // namespace builder
}  // namespace storm::dft
