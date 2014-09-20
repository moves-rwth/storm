#include "src/settings/ArgumentTypeInferationHelper.h"

namespace storm {
    namespace settings {
        template <typename T>
        ArgumentType ArgumentTypeInferation::inferToEnumType() {
            LOG_THROW(false, storm::exceptions::InternalTypeErrorException, "Unable to infer type of argument.");
        }
        
        template <>
        ArgumentType ArgumentTypeInferation::inferToEnumType<std::string>() {
            return ArgumentType::String;
        }
        
        template <>
        ArgumentType ArgumentTypeInferation::inferToEnumType<int_fast64_t>() {
            return ArgumentType::Integer;
        }
        
        template <>
        ArgumentType ArgumentTypeInferation::inferToEnumType<uint_fast64_t>() {
            return ArgumentType::UnsignedInteger;
        }
        
        template <>
        ArgumentType ArgumentTypeInferation::inferToEnumType<double>() {
            return ArgumentType::Double;
        }
        
        template <>
        ArgumentType ArgumentTypeInferation::inferToEnumType<bool>() {
            return ArgumentType::Boolean;
        }
        
        template <typename T>
        std::string const& ArgumentTypeInferation::inferToString(ArgumentType const& argumentType, T const& value) {
            LOG_THROW(false, storm::exceptions::InternalTypeErrorException, "Unable to infer string from non-string argument value.");
        }
        
        template <>
        std::string const& ArgumentTypeInferation::inferToString<std::string>(ArgumentType const& argumentType, std::string const& value) {
            LOG_THROW(argumentType == ArgumentType::String, storm::exceptions::InternalTypeErrorException, "Unable to infer string from non-string argument.");
            return value;
        }
        
        template <typename T>
        int_fast64_t ArgumentTypeInferation::inferToInteger(ArgumentType const& argumentType, T const& value) {
            LOG_THROW(false, storm::exceptions::InternalTypeErrorException, "Unable to infer integer from non-integer argument value.");
        }
        
        template <>
        int_fast64_t ArgumentTypeInferation::inferToInteger<int_fast64_t>(ArgumentType const& argumentType, int_fast64_t const& value) {
            LOG_THROW(argumentType == ArgumentType::Integer, storm::exceptions::InternalTypeErrorException, "Unable to infer integer from non-integer argument.");
            return value;
        }
        
        template <typename T>
        uint_fast64_t ArgumentTypeInferation::inferToUnsignedInteger(ArgumentType const& argumentType, T const& value) {
            LOG_THROW(false, storm::exceptions::InternalTypeErrorException, "Unable to infer unsigned integer from non-unsigned argument value.");
        }
        
        template <>
        uint_fast64_t ArgumentTypeInferation::inferToUnsignedInteger<uint_fast64_t>(ArgumentType const& argumentType, uint_fast64_t const& value) {
            LOG_THROW(argumentType == ArgumentType::UnsignedInteger, storm::exceptions::InternalTypeErrorException, "Unable to infer integer from non-integer argument.");
            return value;
        }
        
        template <typename T>
        double ArgumentTypeInferation::inferToDouble(ArgumentType const& argumentType, T const& value) {
            LOG_THROW(false, storm::exceptions::InternalTypeErrorException, "Unable to infer double from non-double argument value.");
        }
        
        template <>
        double ArgumentTypeInferation::inferToDouble<double>(ArgumentType const& argumentType, double const& value) {
            LOG_THROW(argumentType == ArgumentType::UnsignedInteger, storm::exceptions::InternalTypeErrorException, "Unable to infer double from non-double argument.");
            return value;
        }
        
        template <typename T>
        bool ArgumentTypeInferation::inferToBoolean(ArgumentType const& argumentType, T const& value) {
            LOG_THROW(false, storm::exceptions::InternalTypeErrorException, "Unable to infer boolean from non-boolean argument value.");
        }
        
        template <>
        bool ArgumentTypeInferation::inferToBoolean<bool>(ArgumentType const& argumentType, bool const& value) {
            LOG_THROW(argumentType == ArgumentType::Boolean, storm::exceptions::InternalTypeErrorException, "Unable to infer boolean from non-boolean argument.");
            return value;
        }
    }
}