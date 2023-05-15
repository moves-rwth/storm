#pragma once

#include <limits>
#include <optional>
#include <type_traits>

#include "storm/solver/OptimizationDirection.h"

namespace storm::utility {

/*!
 * Stores and manages an extremal (maximal or minimal) value
 */
template<storm::OptimizationDirection Dir, typename ValueType>
class Extremum {
   public:
    Extremum() = default;
    Extremum(ValueType const& value);
    Extremum(ValueType&& value);
    Extremum(Extremum const&) = default;
    Extremum(Extremum&&) = default;
    Extremum& operator=(Extremum const&) = default;
    Extremum& operator=(Extremum&&) = default;
    ~Extremum() = default;

    /*!
     * Sets the extremum to the given value
     * @return a reference to this
     */
    Extremum& operator=(ValueType const& value);

    /*!
     * Sets the extremum to the given value
     * @return a reference to this
     */
    Extremum& operator=(ValueType&& value);

    /*!
     * @param value
     * @return True if the provided value is strictly better (larger if we maximize; smaller if we minimize) than the stored value
     */
    bool better(ValueType const& value) const;

    /*!
     * Updates the stored value, if the given extremal value is better.
     * @param other
     * @return true if the extremum value of this changed
     */
    bool operator&=(Extremum const& other);

    /*!
     * Updates the stored value, if the given extremal value is better.
     * @param other
     * @return true if the extremum value of this changed
     */
    bool operator&=(Extremum&& other);

    /*!
     * Updates the stored value, if the given value is better.
     * @param other
     * @return true if the extremum value of this changed
     */
    bool operator&=(ValueType const& value);

    /*!
     * Updates the stored value, if the given value is better.
     * @param other
     * @return true if the extremum value of this changed
     */
    bool operator&=(ValueType&& value);

    /*!
     * @return true if this does not store any value (representing the extremum over an empty set)
     */
    bool empty() const;

    /*!
     * @pre this is not empty
     * @return the stored extremal value
     */
    ValueType const& operator*() const;

    /*!
     * @pre this is not empty
     * @return the stored extremal value
     */
    ValueType& operator*();

    /*!
     * @return the stored extremal value as an optional. Returns std::nullopt if this is empty
     */
    std::optional<ValueType> getOptionalValue() const;

    /*!
     * Forgets the extremal value so that this represents the extremum over an empty set.
     */
    void reset();

   private:
    /// indicates whether ValueType supports +/- infinity. If this is true we can use those values to encode an extremum over an empty set
    static bool const SupportsInfinity = std::numeric_limits<ValueType>::is_iec559;

    /// Data for the case that ValueType does not have infinity
    struct DefaultData {
        ValueType value;
        bool empty{true};
    };

    /// Data for the case that ValueType has infinity
    struct DataInfinity {
        ValueType constexpr baseValue() const {
            if constexpr (storm::solver::minimize(Dir)) {
                return std::numeric_limits<ValueType>::infinity();
            } else {
                static_assert(storm::solver::maximize(Dir));
                return -std::numeric_limits<ValueType>::infinity();
            }
        }
        ValueType value{baseValue()};
    };

    /// Data
    std::conditional_t<SupportsInfinity, DataInfinity, DefaultData> data;
};

template<typename ValueType>
using Maximum = Extremum<storm::OptimizationDirection::Maximize, ValueType>;
template<typename ValueType>
using Minimum = Extremum<storm::OptimizationDirection::Minimize, ValueType>;

}  // namespace storm::utility