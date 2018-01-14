#include "GspnJsonExporter.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"

#include <algorithm>
#include <string>

namespace storm {
    namespace gspn {

        size_t GspnJsonExporter::currentId = 0;

        void GspnJsonExporter::toStream(storm::gspn::GSPN const& gspn, std::ostream& os) {
            os << translate(gspn).dump(4) << std::endl;
        }

        modernjson::json GspnJsonExporter::translate(storm::gspn::GSPN const& gspn) {
            modernjson::json jsonGspn;
            currentId = 0;
            return jsonGspn;
        }
    }
}
