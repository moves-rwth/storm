#include "storm/storage/dd/cudd/InternalCuddDdManager.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CuddSettings.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm {
namespace dd {

InternalDdManager<DdType::CUDD>::InternalDdManager() : cuddManager(), reorderingTechnique(CUDD_REORDER_NONE), numberOfDdVariables(0) {
    this->cuddManager.SetMaxMemory(
        static_cast<unsigned long>(storm::settings::getModule<storm::settings::modules::CuddSettings>().getMaximalMemory() * 1024ul * 1024ul));

    auto const& settings = storm::settings::getModule<storm::settings::modules::CuddSettings>();
    this->cuddManager.SetEpsilon(settings.getConstantPrecision());

    // Now set the selected reordering technique.
    storm::settings::modules::CuddSettings::ReorderingTechnique reorderingTechniqueAsSetting = settings.getReorderingTechnique();
    switch (reorderingTechniqueAsSetting) {
        case storm::settings::modules::CuddSettings::ReorderingTechnique::None:
            this->reorderingTechnique = CUDD_REORDER_NONE;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Random:
            this->reorderingTechnique = CUDD_REORDER_RANDOM;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::RandomPivot:
            this->reorderingTechnique = CUDD_REORDER_RANDOM_PIVOT;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Sift:
            this->reorderingTechnique = CUDD_REORDER_SIFT;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::SiftConv:
            this->reorderingTechnique = CUDD_REORDER_SIFT_CONVERGE;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::SymmetricSift:
            this->reorderingTechnique = CUDD_REORDER_SYMM_SIFT;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::SymmetricSiftConv:
            this->reorderingTechnique = CUDD_REORDER_SYMM_SIFT_CONV;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::GroupSift:
            this->reorderingTechnique = CUDD_REORDER_GROUP_SIFT;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::GroupSiftConv:
            this->reorderingTechnique = CUDD_REORDER_GROUP_SIFT_CONV;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Win2:
            this->reorderingTechnique = CUDD_REORDER_WINDOW2;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Win2Conv:
            this->reorderingTechnique = CUDD_REORDER_WINDOW2_CONV;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Win3:
            this->reorderingTechnique = CUDD_REORDER_WINDOW3;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Win3Conv:
            this->reorderingTechnique = CUDD_REORDER_WINDOW3_CONV;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Win4:
            this->reorderingTechnique = CUDD_REORDER_WINDOW4;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Win4Conv:
            this->reorderingTechnique = CUDD_REORDER_WINDOW4_CONV;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Annealing:
            this->reorderingTechnique = CUDD_REORDER_ANNEALING;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Genetic:
            this->reorderingTechnique = CUDD_REORDER_GENETIC;
            break;
        case storm::settings::modules::CuddSettings::ReorderingTechnique::Exact:
            this->reorderingTechnique = CUDD_REORDER_EXACT;
            break;
    }

    this->allowDynamicReordering(settings.isReorderingEnabled());
}

InternalDdManager<DdType::CUDD>::~InternalDdManager() {
    // Intentionally left empty.
}

InternalBdd<DdType::CUDD> InternalDdManager<DdType::CUDD>::getBddOne() const {
    return InternalBdd<DdType::CUDD>(this, cuddManager.bddOne());
}

template<typename ValueType>
InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getAddOne() const {
    return InternalAdd<DdType::CUDD, ValueType>(this, cuddManager.addOne());
}

InternalBdd<DdType::CUDD> InternalDdManager<DdType::CUDD>::getBddZero() const {
    return InternalBdd<DdType::CUDD>(this, cuddManager.bddZero());
}

InternalBdd<DdType::CUDD> InternalDdManager<DdType::CUDD>::getBddEncodingLessOrEqualThan(uint64_t bound, InternalBdd<DdType::CUDD> const& cube,
                                                                                         uint64_t numberOfDdVariables) const {
    return InternalBdd<DdType::CUDD>(this, cudd::BDD(cuddManager, this->getBddEncodingLessOrEqualThanRec(0, (1ull << numberOfDdVariables) - 1, bound,
                                                                                                         cube.getCuddDdNode(), numberOfDdVariables)));
}

DdNodePtr InternalDdManager<DdType::CUDD>::getBddEncodingLessOrEqualThanRec(uint64_t minimalValue, uint64_t maximalValue, uint64_t bound, DdNodePtr cube,
                                                                            uint64_t remainingDdVariables) const {
    if (maximalValue <= bound) {
        return Cudd_ReadOne(cuddManager.getManager());
    } else if (minimalValue > bound) {
        return Cudd_ReadLogicZero(cuddManager.getManager());
    }

    STORM_LOG_ASSERT(remainingDdVariables > 0, "Expected more remaining DD variables.");
    STORM_LOG_ASSERT(!Cudd_IsConstant(cube), "Expected non-constant cube.");
    uint64_t newRemainingDdVariables = remainingDdVariables - 1;
    DdNodePtr elseResult =
        getBddEncodingLessOrEqualThanRec(minimalValue, maximalValue & ~(1ull << newRemainingDdVariables), bound, Cudd_T(cube), newRemainingDdVariables);
    Cudd_Ref(elseResult);
    DdNodePtr thenResult =
        getBddEncodingLessOrEqualThanRec(minimalValue | (1ull << newRemainingDdVariables), maximalValue, bound, Cudd_T(cube), newRemainingDdVariables);
    Cudd_Ref(thenResult);
    STORM_LOG_ASSERT(thenResult != elseResult, "Expected different results.");

    bool complemented = Cudd_IsComplement(thenResult);
    DdNodePtr result =
        cuddUniqueInter(cuddManager.getManager(), Cudd_NodeReadIndex(cube), Cudd_Regular(thenResult), complemented ? Cudd_Not(elseResult) : elseResult);
    if (complemented) {
        result = Cudd_Not(result);
    }
    Cudd_Deref(thenResult);
    Cudd_Deref(elseResult);
    return result;
}

template<typename ValueType>
InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getAddZero() const {
    return InternalAdd<DdType::CUDD, ValueType>(this, cuddManager.addZero());
}

template<typename ValueType>
InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getAddUndefined() const {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Undefined values are not supported by CUDD.");
}

template<typename ValueType>
InternalAdd<DdType::CUDD, ValueType> InternalDdManager<DdType::CUDD>::getConstant(ValueType const& value) const {
    return InternalAdd<DdType::CUDD, ValueType>(this, cuddManager.constant(value));
}

template<>
InternalAdd<DdType::CUDD, storm::RationalNumber> InternalDdManager<DdType::CUDD>::getConstant(storm::RationalNumber const& value) const {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Operation not supported.");
}

std::vector<InternalBdd<DdType::CUDD>> InternalDdManager<DdType::CUDD>::createDdVariables(uint64_t numberOfLayers,
                                                                                          boost::optional<uint_fast64_t> const& position) {
    std::vector<InternalBdd<DdType::CUDD>> result;

    if (position) {
        for (uint64_t layer = 0; layer < numberOfLayers; ++layer) {
            result.emplace_back(InternalBdd<DdType::CUDD>(this, cuddManager.bddNewVarAtLevel(position.get() + layer)));
        }
    } else {
        for (uint64_t layer = 0; layer < numberOfLayers; ++layer) {
            result.emplace_back(InternalBdd<DdType::CUDD>(this, cuddManager.bddVar()));
        }
    }

    // Connect the variables so they are not 'torn apart' by dynamic reordering.
    // Note that MTR_FIXED preserves the order of the layers. While this is not always necessary to preserve,
    // (for example) the hybrid engine relies on this connection, so we choose MTR_FIXED instead of MTR_DEFAULT.
    cuddManager.MakeTreeNode(result.front().getIndex(), numberOfLayers, MTR_FIXED);

    // Keep track of the number of variables.
    numberOfDdVariables += numberOfLayers;

    return result;
}

bool InternalDdManager<DdType::CUDD>::supportsOrderedInsertion() const {
    return true;
}

void InternalDdManager<DdType::CUDD>::allowDynamicReordering(bool value) {
    if (value) {
        this->getCuddManager().AutodynEnable(this->reorderingTechnique);
    } else {
        this->getCuddManager().AutodynDisable();
    }
}

bool InternalDdManager<DdType::CUDD>::isDynamicReorderingAllowed() const {
    Cudd_ReorderingType type;
    return this->getCuddManager().ReorderingStatus(&type);
}

void InternalDdManager<DdType::CUDD>::triggerReordering() {
    this->getCuddManager().ReduceHeap(this->reorderingTechnique, 0);
}

void InternalDdManager<DdType::CUDD>::debugCheck() const {
    this->getCuddManager().CheckKeys();
    this->getCuddManager().DebugCheck();
}

void InternalDdManager<DdType::CUDD>::execute(std::function<void()> const& f) const {
    f();
}

cudd::Cudd& InternalDdManager<DdType::CUDD>::getCuddManager() {
    return cuddManager;
}

cudd::Cudd const& InternalDdManager<DdType::CUDD>::getCuddManager() const {
    return cuddManager;
}

uint_fast64_t InternalDdManager<DdType::CUDD>::getNumberOfDdVariables() const {
    return numberOfDdVariables;
}

template InternalAdd<DdType::CUDD, double> InternalDdManager<DdType::CUDD>::getAddOne() const;
template InternalAdd<DdType::CUDD, uint_fast64_t> InternalDdManager<DdType::CUDD>::getAddOne() const;
template InternalAdd<DdType::CUDD, storm::RationalNumber> InternalDdManager<DdType::CUDD>::getAddOne() const;

template InternalAdd<DdType::CUDD, double> InternalDdManager<DdType::CUDD>::getAddZero() const;
template InternalAdd<DdType::CUDD, uint_fast64_t> InternalDdManager<DdType::CUDD>::getAddZero() const;
template InternalAdd<DdType::CUDD, storm::RationalNumber> InternalDdManager<DdType::CUDD>::getAddZero() const;

template InternalAdd<DdType::CUDD, double> InternalDdManager<DdType::CUDD>::getConstant(double const& value) const;
template InternalAdd<DdType::CUDD, uint_fast64_t> InternalDdManager<DdType::CUDD>::getConstant(uint_fast64_t const& value) const;
}  // namespace dd
}  // namespace storm
