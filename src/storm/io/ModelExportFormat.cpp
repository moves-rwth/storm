#include "ModelExportFormat.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace exporter {

ModelExportFormat getModelExportFormatFromString(std::string const& input) {
    if (input == "dot") {
        return ModelExportFormat::Dot;
    } else if (input == "drdd") {
        return ModelExportFormat::Drdd;
    } else if (input == "drn") {
        return ModelExportFormat::Drn;
    } else if (input == "json") {
        return ModelExportFormat::Json;
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The model export format '" << input << "' does not match any known format.");
}

std::string toString(ModelExportFormat const& input) {
    switch (input) {
        case ModelExportFormat::Dot:
            return "dot";
        case ModelExportFormat::Drdd:
            return "drdd";
        case ModelExportFormat::Drn:
            return "drn";
        case ModelExportFormat::Json:
            return "json";
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Unhandled model export format.");
}

ModelExportFormat getModelExportFormatFromFileExtension(std::string const& filename) {
    auto pos = filename.find_last_of('.');
    STORM_LOG_THROW(pos != std::string::npos, storm::exceptions::InvalidArgumentException,
                    "Couldn't detect a file extension from input filename '" << filename << "'.");
    ++pos;
    return getModelExportFormatFromString(filename.substr(pos));
}

}  // namespace exporter
}  // namespace storm
