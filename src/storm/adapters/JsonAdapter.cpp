#include "storm/adapters/JsonAdapter.h"

#include <sstream>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/macros.h"

namespace storm {

template<typename ValueType, typename CallBack>
void json_for_each_number_float(storm::json<ValueType> const& j, CallBack const& f) {
    if (j.is_structured()) {
        for (auto const& ji : j) {
            json_for_each_number_float(ji, f);
        }
    } else if (j.is_number_float()) {
        f(j);
    }
}

template<typename ValueType>
void warnIfJsonExportNotAccurate(storm::json<ValueType> const& j) {
    std::stringstream message;
    uint64_t num_bad(0), num_all(0);
    json_for_each_number_float(j, [&message, &num_bad, &num_all](auto const& v_json) {
        ++num_all;
        auto const& actualValue = v_json.template get_ref<ValueType const&>();
        if (std::string v_dump = v_json.dump(); actualValue != storm::utility::convertNumber<ValueType>(v_dump)) {
            ++num_bad;
            if (num_bad == 1) {
                message << "Inaccurate JSON export: The number " << actualValue << " will be rounded to  " << v_dump << ". ";
            }
        };
    });
    STORM_LOG_WARN_COND(num_bad == 0, message.str() << "In total, " << num_bad << " of " << num_all << " numbers are inaccurate.");
}

template<typename ValueType>
std::string dumpJson(storm::json<ValueType> const& j, bool compact) {
    if constexpr (storm::NumberTraits<ValueType>::IsExact) {
        warnIfJsonExportNotAccurate(j);
    }
    if (compact) {
        return j.dump();
    } else {
        return j.dump(4);
    }
}

template std::string dumpJson(storm::json<double> const& j, bool compact = false);
template std::string dumpJson(storm::json<storm::RationalNumber> const& j, bool compact = false);

}  // namespace storm