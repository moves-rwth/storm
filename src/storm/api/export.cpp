#include "storm/api/export.h"
#include "storm/storage/jani/JaniLocationExpander.h"

namespace storm {
namespace api {

void exportJaniModelAsDot(storm::jani::Model const& model, std::string const& filename) {
    std::ofstream out;
    storm::utility::openFile(filename, out);
    model.writeDotToStream(out);
    storm::utility::closeFile(out);
}

}  // namespace api
}  // namespace storm
