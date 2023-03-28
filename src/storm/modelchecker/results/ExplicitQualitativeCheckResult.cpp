#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace modelchecker {
ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult() : truthValues(map_type()) {
    // Intentionally left empty.
}

ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(map_type const& map) : truthValues(map) {
    // Intentionally left empty.
}

ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(map_type&& map) : truthValues(map) {
    // Intentionally left empty.
}

ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(storm::storage::sparse::state_type state, bool value) : truthValues(map_type()) {
    boost::get<map_type>(truthValues)[state] = value;
}

ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(storm::storage::BitVector const& truthValues) : truthValues(truthValues) {
    // Intentionally left empty.
}

ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(storm::storage::BitVector&& truthValues) : truthValues(std::move(truthValues)) {
    // Intentionally left empty.
}

ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(boost::variant<vector_type, map_type> const& truthValues) : truthValues(truthValues) {
    // Intentionally left empty.
}

ExplicitQualitativeCheckResult::ExplicitQualitativeCheckResult(boost::variant<vector_type, map_type>&& truthValues) : truthValues(std::move(truthValues)) {
    // Intentionally left empty.
}

std::unique_ptr<CheckResult> ExplicitQualitativeCheckResult::clone() const {
    return std::make_unique<ExplicitQualitativeCheckResult>(this->truthValues);
}

void ExplicitQualitativeCheckResult::performLogicalOperation(ExplicitQualitativeCheckResult& first, QualitativeCheckResult const& second, bool logicalAnd) {
    STORM_LOG_THROW(second.isExplicitQualitativeCheckResult(), storm::exceptions::InvalidOperationException,
                    "Cannot perform logical 'and' on check results of incompatible type.");
    STORM_LOG_THROW(first.isResultForAllStates() == second.isResultForAllStates(), storm::exceptions::InvalidOperationException,
                    "Cannot perform logical 'and' on check results of incompatible type.");
    ExplicitQualitativeCheckResult const& secondCheckResult = static_cast<ExplicitQualitativeCheckResult const&>(second);
    if (first.isResultForAllStates()) {
        if (logicalAnd) {
            boost::get<vector_type>(first.truthValues) &= boost::get<vector_type>(secondCheckResult.truthValues);
        } else {
            boost::get<vector_type>(first.truthValues) |= boost::get<vector_type>(secondCheckResult.truthValues);
        }
    } else {
        std::function<bool(bool, bool)> function = logicalAnd ? std::function<bool(bool, bool)>([](bool a, bool b) { return a && b; })
                                                              : std::function<bool(bool, bool)>([](bool a, bool b) { return a || b; });

        map_type& map1 = boost::get<map_type>(first.truthValues);
        map_type const& map2 = boost::get<map_type>(secondCheckResult.truthValues);
        for (auto& element1 : map1) {
            auto const& keyValuePair = map2.find(element1.first);
            STORM_LOG_THROW(keyValuePair != map2.end(), storm::exceptions::InvalidOperationException,
                            "Cannot perform logical 'and' on check results of incompatible type.");
            element1.second = function(element1.second, keyValuePair->second);
        }

        // Double-check that there are no entries in map2 that the current result does not have.
        for (auto const& element2 : map2) {
            auto const& keyValuePair = map1.find(element2.first);
            STORM_LOG_THROW(keyValuePair != map1.end(), storm::exceptions::InvalidOperationException,
                            "Cannot perform logical 'and' on check results of incompatible type.");
        }
    }
}

QualitativeCheckResult& ExplicitQualitativeCheckResult::operator&=(QualitativeCheckResult const& other) {
    performLogicalOperation(*this, other, true);
    return *this;
}

QualitativeCheckResult& ExplicitQualitativeCheckResult::operator|=(QualitativeCheckResult const& other) {
    performLogicalOperation(*this, other, false);
    return *this;
}

bool ExplicitQualitativeCheckResult::existsTrue() const {
    if (this->isResultForAllStates()) {
        return !boost::get<vector_type>(truthValues).empty();
    } else {
        for (auto& element : boost::get<map_type>(truthValues)) {
            if (element.second) {
                return true;
            }
        }
        return false;
    }
}
bool ExplicitQualitativeCheckResult::forallTrue() const {
    if (this->isResultForAllStates()) {
        return boost::get<vector_type>(truthValues).full();
    } else {
        for (auto& element : boost::get<map_type>(truthValues)) {
            if (!element.second) {
                return false;
            }
        }
        return true;
    }
}

uint64_t ExplicitQualitativeCheckResult::count() const {
    if (this->isResultForAllStates()) {
        return boost::get<vector_type>(truthValues).getNumberOfSetBits();
    } else {
        uint64_t result = 0;
        for (auto& element : boost::get<map_type>(truthValues)) {
            if (element.second) {
                ++result;
            }
        }
        return result;
    }
}

bool ExplicitQualitativeCheckResult::operator[](storm::storage::sparse::state_type state) const {
    if (this->isResultForAllStates()) {
        return boost::get<vector_type>(truthValues).get(state);
    } else {
        map_type const& map = boost::get<map_type>(truthValues);
        auto const& keyValuePair = map.find(state);
        STORM_LOG_THROW(keyValuePair != map.end(), storm::exceptions::InvalidOperationException, "Unknown key '" << state << "'.");
        return keyValuePair->second;
    }
}

ExplicitQualitativeCheckResult::vector_type const& ExplicitQualitativeCheckResult::getTruthValuesVector() const {
    return boost::get<vector_type>(truthValues);
}

ExplicitQualitativeCheckResult::map_type const& ExplicitQualitativeCheckResult::getTruthValuesMap() const {
    return boost::get<map_type>(truthValues);
}

void ExplicitQualitativeCheckResult::complement() {
    if (this->isResultForAllStates()) {
        boost::get<vector_type>(truthValues).complement();
    } else {
        for (auto& element : boost::get<map_type>(truthValues)) {
            element.second = !element.second;
        }
    }
}

bool ExplicitQualitativeCheckResult::isExplicit() const {
    return true;
}

bool ExplicitQualitativeCheckResult::isResultForAllStates() const {
    return truthValues.which() == 0;
}

bool ExplicitQualitativeCheckResult::isExplicitQualitativeCheckResult() const {
    return true;
}

std::ostream& ExplicitQualitativeCheckResult::writeToStream(std::ostream& out) const {
    if (this->isResultForAllStates()) {
        vector_type const& vector = boost::get<vector_type>(truthValues);
        bool allTrue = vector.full();
        bool allFalse = !allTrue && vector.empty();
        if (allTrue) {
            out << "{true}";
        } else if (allFalse) {
            out << "{false}";
        } else {
            out << "{true, false}";
        }
    } else {
        std::ios::fmtflags oldflags(std::cout.flags());
        out << std::boolalpha;

        map_type const& map = boost::get<map_type>(truthValues);
        if (map.size() == 1) {
            out << map.begin()->second;
        } else {
            bool allTrue = true;
            bool allFalse = true;
            for (auto const& entry : map) {
                if (entry.second) {
                    allFalse = false;
                } else {
                    allTrue = false;
                }
            }
            if (allTrue) {
                out << "{true}";
            } else if (allFalse) {
                out << "{false}";
            } else {
                out << "{true, false}";
            }
        }

        std::cout.flags(oldflags);
    }
    return out;
}

void ExplicitQualitativeCheckResult::filter(QualitativeCheckResult const& filter) {
    STORM_LOG_THROW(filter.isExplicitQualitativeCheckResult(), storm::exceptions::InvalidOperationException,
                    "Cannot filter explicit check result with non-explicit filter.");
    STORM_LOG_THROW(filter.isResultForAllStates(), storm::exceptions::InvalidOperationException, "Cannot filter check result with non-complete filter.");
    ExplicitQualitativeCheckResult const& explicitFilter = filter.asExplicitQualitativeCheckResult();
    vector_type const& filterTruthValues = explicitFilter.getTruthValuesVector();

    if (this->isResultForAllStates()) {
        map_type newMap;
        for (auto element : filterTruthValues) {
            newMap.emplace(element, this->getTruthValuesVector().get(element));
        }
        this->truthValues = newMap;
    } else {
        map_type const& map = boost::get<map_type>(truthValues);

        map_type newMap;
        for (auto const& element : map) {
            if (filterTruthValues.get(element.first)) {
                newMap.insert(element);
            }
        }

        STORM_LOG_THROW(newMap.size() == filterTruthValues.getNumberOfSetBits(), storm::exceptions::InvalidOperationException,
                        "The check result fails to contain some results referred to by the filter.");

        this->truthValues = newMap;
    }
}

template<typename JsonRationalType>
void insertJsonEntry(storm::json<JsonRationalType>& json, uint64_t const& id, bool value,
                     std::optional<storm::storage::sparse::StateValuations> const& stateValuations = std::nullopt,
                     std::optional<storm::models::sparse::StateLabeling> const& stateLabels = std::nullopt) {
    storm::json<JsonRationalType> entry;
    if (stateValuations) {
        entry["s"] = stateValuations->template toJson<JsonRationalType>(id);
    } else {
        entry["s"] = id;
    }
    entry["v"] = value;
    if (stateLabels) {
        auto labs = stateLabels->getLabelsOfState(id);
        entry["l"] = labs;
    }
    json.push_back(std::move(entry));
}

template<typename JsonRationalType>
storm::json<JsonRationalType> ExplicitQualitativeCheckResult::toJson(std::optional<storm::storage::sparse::StateValuations> const& stateValuations,
                                                                     std::optional<storm::models::sparse::StateLabeling> const& stateLabels) const {
    storm::json<JsonRationalType> result;
    if (this->isResultForAllStates()) {
        vector_type const& valuesAsVector = boost::get<vector_type>(truthValues);
        for (uint64_t state = 0; state < valuesAsVector.size(); ++state) {
            insertJsonEntry(result, state, valuesAsVector.get(state), stateValuations, stateLabels);
        }
    } else {
        map_type const& valuesAsMap = boost::get<map_type>(truthValues);
        for (auto const& stateValue : valuesAsMap) {
            insertJsonEntry(result, stateValue.first, stateValue.second, stateValuations, stateLabels);
        }
    }
    return result;
}

template storm::json<double> ExplicitQualitativeCheckResult::toJson<double>(std::optional<storm::storage::sparse::StateValuations> const&,
                                                                            std::optional<storm::models::sparse::StateLabeling> const&) const;
template storm::json<storm::RationalNumber> ExplicitQualitativeCheckResult::toJson<storm::RationalNumber>(
    std::optional<storm::storage::sparse::StateValuations> const&, std::optional<storm::models::sparse::StateLabeling> const&) const;

}  // namespace modelchecker
}  // namespace storm
