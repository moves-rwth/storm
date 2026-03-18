#pragma once

#include <bit>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <ranges>
#include <span>

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Variable.h"
#include "storm/storage/umb/model/StringEncoding.h"
#include "storm/storage/umb/model/ValuationDescription.h"
#include "storm/storage/umb/model/ValueEncoding.h"
#include "storm/utility/bitoperations.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm::umb {

class Valuations {
   public:
    Valuations(std::vector<ValuationClassDescription> const& descriptions, std::vector<char> valuations, std::vector<uint64_t> stringMapping,
               std::vector<char> strings, std::optional<std::vector<uint32_t>> classes = {},
               std::vector<std::shared_ptr<storm::expressions::ExpressionManager>> expressionManagers = {})
        : valuations(std::move(valuations)), stringMapping(std::move(stringMapping)), strings(std::move(strings)) {
        if (expressionManagers.empty()) {
            expressionManagers.emplace_back(std::make_shared<storm::expressions::ExpressionManager>());
        }
        STORM_LOG_ASSERT(descriptions.size() == expressionManagers.size() || expressionManagers.size() == 1,
                         "Mismatch between number of descriptions and expression managers.");
        for (uint64_t i = 0; i < descriptions.size(); ++i) {
            // We either have a separate manager for each class or all classes share the same manager.
            // In the latter case, variable (names,types) pairs need to be unique among all possible classes
            auto& manager = expressionManagers.size() == 1 ? *expressionManagers.front() : *expressionManagers[i];
            variableClasses.push_back(createVariablesInformation(manager, descriptions[i]));
        }
        if (classes.has_value()) {
            STORM_LOG_ASSERT(classes->size() == descriptions.size(), "Mismatch between number of descriptions and class mapping.");
            uniqueSizeInBytes = std::numeric_limits<uint64_t>::max();  // not unique
            this->classes = {std::move(*classes), std::vector<uint64_t>{1, 0}};
            std::vector<uint64_t> classSizesInBytes;
            for (auto const& descr : descriptions) {
                classSizesInBytes.push_back(descr.sizeInBits() / 8);
            }
            uint64_t pos = 0;
            this->classes->toValuationsMapping.reserve(this->classes->toClassMapping.size() + 1);
            for (uint64_t entity = 0; entity < this->classes->toClassMapping.size(); ++entity) {
                pos += classSizesInBytes[this->classes->toClassMapping[entity]];
                this->classes->toValuationsMapping.push_back(pos);
            }
            STORM_LOG_ASSERT(valuations.size() == pos, "Valuation data size does not match class mapping.");
        } else {
            STORM_LOG_ASSERT(descriptions.size() == 1, "Valuation descriptions must be unique if no class mapping is given.");
            uniqueSizeInBytes = descriptions.front().sizeInBits() / 8;
            STORM_LOG_ASSERT(valuations.size() % uniqueSizeInBytes == 0, "Valuation data size is not a multiple of the unique valuation size.");
        }
    }

    Valuations(std::vector<ValuationClassDescription> descriptions, std::vector<char> valuations, std::optional<std::vector<uint32_t>> classes = {},
               std::vector<std::shared_ptr<storm::expressions::ExpressionManager>> expressionManagers = {})
        : Valuations(std::move(descriptions), std::move(valuations), {}, {}, std::move(classes), std::move(expressionManagers)) {
        // Intentionally empty
    }

    uint64_t size() const {
        if (classes) {
            return classes->toClassMapping.size();
        } else {
            return valuations.size() / uniqueSizeInBytes;
        }
    }

    uint64_t numStrings() const {
        return stringMapping.size() > 0 ? stringMapping.size() - 1 : 0;
    }

    void read(uint64_t entity, auto const& callback) const {
        for (auto const& varInfo : info(entity).variables) {
            read(entity, varInfo, callback);
        }
    }

    void read(auto const& callback) const {
        for (uint64_t entity = 0; entity < size(); ++entity) {
            read(entity, callback);
        }
    }

   private:
    using Integer = storm::NumberTraits<storm::RationalNumber>::IntegerType;

    // Variable information
    struct VariableInformation {
        storm::expressions::Variable const expressionVariable;
        ValuationClassDescription::Variable const description;
        uint64_t const bitOffset;  // The first bit holding the variable's data within the valuation.
        // If the variable is optional, the optional bit is located at bitOffset - 1
        bool const fits64Bit;
    };
    struct VariablesInformation {
        std::vector<VariableInformation> variables;
        std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
    };
    std::vector<VariablesInformation> variableClasses;

    // Classes information
    struct ClassData {
        std::vector<uint32_t> toClassMapping;
        std::vector<uint64_t> toValuationsMapping;
    };
    std::optional<ClassData> classes;  // present iff there are multiple classes
    uint64_t uniqueSizeInBytes;        // if there is only a single class, this is the size of the valuation in bytes. 2^64-1 iff classes are present.

    // Data
    std::vector<char> valuations;
    std::vector<uint64_t> stringMapping;
    std::vector<char> strings;

    /*!
     * Return true if the variable representation and potential offset addition all fit inside a standard 64 bit number representation
     * @return
     */
    static bool fits64Bit(ValuationClassDescription::Variable const& varDesc) {
        using enum storm::umb::Type;
        if (varDesc.type.bitSize() > 64) {
            return false;
        }
        switch (varDesc.type.type) {
            case Bool:
                return true;
            case Uint: {
                if (varDesc.offset.value_or(0) == 0) {
                    return true;
                }
                // check if adding the (non-zero) offset still fits into 64 bits
                uint64_t const maxValue = varDesc.upper.has_value()        ? static_cast<uint64_t>(varDesc.upper.value())
                                          : (varDesc.type.bitSize() == 64) ? std::numeric_limits<uint64_t>::max()
                                                                           : (static_cast<uint64_t>(1) << varDesc.type.bitSize()) - 1;
                // the minValue equals the offset (given as int64_t and therefore always fits into 64 bits
                if (varDesc.offset.value() < 0) {
                    // maxValue + offset = maxValue - (-offset) must fit into output type int64_t
                    return maxValue - static_cast<uint64_t>(-varDesc.offset.value()) <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
                } else {  // positive offset
                    // maxValue + offset must fit into output type uint64_t
                    return maxValue <= std::numeric_limits<uint64_t>::max() - static_cast<uint64_t>(varDesc.offset.value());
                }
            }
            case Int: {
                if (varDesc.offset.value_or(0) == 0) {
                    return true;
                }
                // check if adding the (non-zero) offset still fits into 64 bits
                int64_t const maxValue = varDesc.upper.value_or(varDesc.type.bitSize() == 64 ? std::numeric_limits<int64_t>::max()
                                                                                             : (static_cast<int64_t>(1) << (varDesc.type.bitSize() - 1)) - 1);
                int64_t const minValue = varDesc.lower.value_or(varDesc.type.bitSize() == 64 ? std::numeric_limits<int64_t>::min()
                                                                                             : -(static_cast<int64_t>(1) << (varDesc.type.bitSize() - 1)));
                if (varDesc.offset.value() < 0) {
                    // minValue + offset must fit into output type int64_t
                    return minValue >= std::numeric_limits<int64_t>::min() - varDesc.offset.value();
                } else {  // positive offset
                    // maxValue + offset must fit into output type int64_t
                    return maxValue <= std::numeric_limits<int64_t>::max() - varDesc.offset.value();
                }
            }
            case Double:
                return true;  // double values are always stored as 64 bit IEEE 754 values
            case Rational:
                return false;  //
            case String:
                return true;  // string indices are always stored as uint64_t
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                "Valuations for variable type '" << varDesc.type.toString() << "' are not supported.");
        }
    }

    static VariablesInformation createVariablesInformation(storm::expressions::ExpressionManager& expressionManager,
                                                           ValuationClassDescription const& description) {
        VariablesInformation result{.variables = {}, .expressionManager = expressionManager.shared_from_this()};
        uint64_t currentOffset = 0;
        for (auto const& varVariant : description.variables) {
            if (std::holds_alternative<ValuationClassDescription::Variable>(varVariant)) {
                auto const& varDesc = std::get<ValuationClassDescription::Variable>(varVariant);
                storm::expressions::Type variableType;
                using enum storm::umb::Type;
                switch (varDesc.type.type) {
                    case Bool:
                        variableType = expressionManager.getBooleanType();
                        break;
                    case Uint:
                    case Int:
                        variableType = expressionManager.getIntegerType();
                        break;
                    case Double:
                    case Rational:
                        variableType = expressionManager.getRationalType();
                        break;
                    case String:
                        variableType = expressionManager.getStringType();
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                        "Valuations for variable type '" << varDesc.type.toString() << "' are not supported.");
                }
                if (varDesc.isOptional.value_or(false)) {
                    ++currentOffset;  // optional variables have a preceding presence bit
                }
                result.variables.emplace_back(VariableInformation{.expressionVariable = expressionManager.declareOrGetVariable(varDesc.name, variableType),
                                                                  .description = varDesc,
                                                                  .bitOffset = currentOffset,
                                                                  .fits64Bit = fits64Bit(varDesc)});
                currentOffset += result.variables.back().description.type.bitSize();
            } else {
                auto const& padding = std::get<ValuationClassDescription::Padding>(varVariant);
                currentOffset += padding.padding;
            }
        }
        STORM_LOG_ASSERT(currentOffset == description.sizeInBits(), "Computed size does not match description size.");
        STORM_LOG_ASSERT(currentOffset % 8 == 0, "Invalid valuation description detected: size in bits must be a multiple of 8.");
        return result;
    }

    VariablesInformation const& info(uint64_t entity) const {
        STORM_LOG_ASSERT(entity < size(), "Entity index out of bounds: " << entity << " >= " << size() << ".");
        if (classes) {
            return variableClasses[classes->toClassMapping[entity]];
        } else {
            return variableClasses.front();
        }
    }

    std::span<char const> bytes(uint64_t entity) const {
        STORM_LOG_ASSERT(entity < size(), "Entity index out of bounds: " << entity << " >= " << size() << ".");
        if (classes) {
            auto const start = classes->toValuationsMapping[entity];
            auto const end = classes->toValuationsMapping[entity + 1];
            return std::span<char const>(&valuations[start], end - start);
        } else {
            auto const start = entity * uniqueSizeInBytes;
            return std::span<char const>(&valuations[start], uniqueSizeInBytes);
        }
    }

    std::span<char> bytes(uint64_t entity) {
        if (classes) {
            auto const start = classes->toValuationsMapping[entity];
            auto const end = classes->toValuationsMapping[entity + 1];
            return std::span<char>(&valuations[start], end - start);
        } else {
            auto const start = entity * uniqueSizeInBytes;
            return std::span<char>(&valuations[start], uniqueSizeInBytes);
        }
    }

    uint64_t readUint64(std::span<char const> bytes, uint64_t const bitOffset, uint64_t const bitSize) const {
        STORM_LOG_ASSERT(bitOffset < bytes.size() * 8, "Variable offset exceeds valuation size.");
        STORM_LOG_ASSERT(bitSize <= 64, "Invalid bit range.");
        auto const firstByte = bitOffset / 8;
        auto const bitOffsetWithinByte = bitOffset % 8;
        auto const numBytes = (bitOffsetWithinByte + bitSize + 7) / 8;
        STORM_LOG_ASSERT(numBytes <= 9, "Invalid number of bytes computed: " << numBytes);
        uint64_t result;
        // set the first (up to) 8 bytes
        std::memcpy(&result, &bytes[firstByte], std::min<uint64_t>(numBytes, 8ull));
        result >>= bitOffsetWithinByte;
        // if necessary, set the most significant bits by reading a 9th byte
        if (numBytes == 9ull) {
            uint64_t upperBits = std::bit_cast<uint8_t>(bytes[firstByte + 8]);
            upperBits <<= (64 - bitOffsetWithinByte);
            result |= upperBits;
        }
        // Set irrelevant bits to zero
        if (bitSize < 64) {
            uint64_t const relevantBitMask = (1ull << bitSize) - 1;
            result &= relevantBitMask;
        }
        return result;
    }

    template<bool Signed>
    Integer readInteger(std::span<char const> bytes, uint64_t const bitOffset, uint64_t const bitSize) const {
        auto const num64BitChunks = (bitSize + 63) / 64;
        auto chunksView =
            std::ranges::iota_view(0ull, num64BitChunks) | std::ranges::views::transform([this, &bytes, &bitOffset, &bitSize](auto i) -> uint64_t {
                return readUint64(bytes, bitOffset + i * 64, std::min<uint64_t>(64, bitSize - i * 64));
            });
        Integer result = ValueEncoding::decodeArbitraryPrecisionInteger<false>(chunksView);
        if constexpr (Signed) {
            // Check if this number is supposed to be negative
            if (result >= storm::utility::pow<Integer>(2, bitSize - 1)) {
                return result - storm::utility::pow<Integer>(2, bitSize);
            }
        }
        return result;
    }

    template<typename... AllowedTypes>
    void read(uint64_t entity, VariableInformation const& varInfo, auto const& callback) const {
        bool constexpr allowAny = (sizeof...(AllowedTypes) == 0);
        bool constexpr allowNullopt = allowAny || std::disjunction_v<std::is_same<std::nullopt_t, AllowedTypes>...>;
        bool constexpr allowBool = allowAny || std::disjunction_v<std::is_same<bool, AllowedTypes>...>;
        bool constexpr allowUint64 = allowAny || std::disjunction_v<std::is_same<uint64_t, AllowedTypes>...>;
        bool constexpr allowInt64 = allowAny || std::disjunction_v<std::is_same<int64_t, AllowedTypes>...>;
        bool constexpr allowDouble = allowAny || std::disjunction_v<std::is_same<double, AllowedTypes>...>;
        bool constexpr allowRational = allowAny || std::disjunction_v<std::is_same<storm::RationalNumber, AllowedTypes>...>;
        bool constexpr allowInteger = allowAny || std::disjunction_v<std::is_same<Integer, AllowedTypes>...>;
        bool constexpr allowString = allowAny || std::disjunction_v<std::is_same<std::string_view, AllowedTypes>...>;

        if (varInfo.description.isOptional.value_or(false)) {
            if constexpr (allowNullopt) {
                callback(entity, varInfo.expressionVariable, std::nullopt);
            }
        }
        auto const bitSize = varInfo.description.type.bitSize();
        using enum storm::umb::Type;
        if (varInfo.fits64Bit) {
            STORM_LOG_ASSERT(bitSize <= 64, "Invalid bit size for 64 bit fast path.");
            uint64_t rawContent = readUint64(bytes(entity), varInfo.bitOffset, bitSize);
            switch (varInfo.description.type.type) {
                case Bool:
                    if constexpr (allowBool) {
                        callback(entity, varInfo.expressionVariable, rawContent != 0);
                        return;
                    }
                case Uint:
                    if (int64_t offset = varInfo.description.offset.value_or(0); offset < 0) {
                        // negative offset, output type is int64_t
                        if constexpr (allowInt64) {
                            callback(entity, varInfo.expressionVariable, static_cast<int64_t>(rawContent) + offset);
                            return;
                        }
                    } else {
                        // non-negative offset, output type is uint64_t
                        uint64_t value = rawContent + offset;
                        if constexpr (allowUint64) {
                            callback(entity, varInfo.expressionVariable, value);
                            return;
                        } else if constexpr (allowInt64) {
                            if (value <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
                                callback(entity, varInfo.expressionVariable, static_cast<int64_t>(value));
                                return;
                            }
                        }
                    }
                case Int:
                    if constexpr (allowInt64) {
                        uint64_t const mostSignificantBitMask = 1ull << (bitSize - 1);
                        if (rawContent & mostSignificantBitMask) {
                            // Negative value: Two's complement (e.g. 1111...1101 is -3)
                            callback(entity, varInfo.expressionVariable, -static_cast<int64_t>(~rawContent & (mostSignificantBitMask - 1)) - 1);
                            return;
                        } else {
                            // Positive value
                            callback(entity, varInfo.expressionVariable, static_cast<int64_t>(rawContent));
                            return;
                        }
                    }
                case Double:
                    if constexpr (allowDouble) {
                        callback(entity, varInfo.expressionVariable, static_cast<double>(std::bit_cast<double>(rawContent)));
                        return;
                    }
                case Rational:
                    // Reaching this part should not be possible as varInfo.fits64Bit would be false
                    STORM_LOG_ASSERT(false, "Handling of rational values in 64 bit fast path is not implemented.");
                case String:
                    if constexpr (allowString) {
                        callback(entity, varInfo.expressionVariable, stringVectorView(strings, stringMapping)[rawContent]);
                        return;
                    }
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                    "Valuations for variable type '" << varInfo.description.type.toString() << "' are not supported.");
            }
        }
        // reaching this point means that we could not handle the value in the fast path
        switch (varInfo.description.type.type) {
            case Bool:
                if constexpr (allowBool) {
                    // Bools could be encoded with more than 64 bits (which is not reasonable, but possible..)
                    callback(entity, varInfo.expressionVariable, readInteger<false>(bytes(entity), varInfo.bitOffset, bitSize) != Integer(0));
                    return;
                }
            case Uint:
            case Int:
                if constexpr (allowInteger) {
                    Integer value = varInfo.description.type.type == Int ? readInteger<true>(bytes(entity), varInfo.bitOffset, bitSize)
                                                                         : readInteger<false>(bytes(entity), varInfo.bitOffset, bitSize);
                    value += storm::utility::convertNumber<Integer>(varInfo.description.offset.value_or(0));
                    callback(entity, varInfo.expressionVariable, value);
                    return;
                }
            case Double:
                // Reaching this part should not be possible as varInfo.fits64Bit would be true
                STORM_LOG_ASSERT(false, "double variables with more than 64 bits are not compliant.");
            case Rational:
                if constexpr (allowRational) {
                    STORM_LOG_ASSERT(bitSize % 2 == 0, "Rational number bit size must be even.");
                    uint64_t b = bitSize / 2;
                    storm::RationalNumber numerator = readInteger<true>(bytes(entity), varInfo.bitOffset, b);
                    storm::RationalNumber denominator = readInteger<false>(bytes(entity), varInfo.bitOffset + b, b);
                    callback(entity, varInfo.expressionVariable, storm::RationalNumber(numerator / denominator));
                    return;
                }
            case String: {
                // Reaching this part should not be possible as varInfo.fits64Bit would be true
                STORM_LOG_ASSERT(false, "String variables with more than 64 bits are not compliant.");
            }
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                                "Valuations for variable type '" << varInfo.description.type.toString() << "' are not supported.");
        }

        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException,
                        "Variable " << varInfo.description.name << " of type " << varInfo.description.type.toString() << " is not handled.");
    }
};
}  // namespace storm::umb