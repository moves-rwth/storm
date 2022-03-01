#include "storm/io/file.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/analysis/GraphConditions.h"

namespace storm {
    namespace api {

        template <typename ValueType>
        void exportParametricResultToFile(boost::optional<ValueType> , storm::analysis::ConstraintCollector<ValueType> const& constraintCollector, std::string const& path) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot export non-parametric result.");
        }

        template <>
        inline void exportParametricResultToFile(boost::optional<storm::RationalFunction> result, storm::analysis::ConstraintCollector<storm::RationalFunction> const& constraintCollector, std::string const& path) {
            std::ofstream filestream;
            storm::utility::openFile(path, filestream);
            filestream << "$Parameters: ";
            auto const& vars = constraintCollector.getVariables();
            std::copy(vars.begin(), vars.end(), std::ostream_iterator<storm::RationalFunctionVariable>(filestream, "; "));
            filestream << '\n';
            if(result) {
                filestream << "$Result: " << result->toString(false, true) << '\n';
            }
            filestream << "$Well-formed Constraints: \n";
            std::vector<std::string> stringConstraints;
            std::transform(constraintCollector.getWellformedConstraints().begin(), constraintCollector.getWellformedConstraints().end(), std::back_inserter(stringConstraints), [](carl::Formula<typename storm::Polynomial::PolyType> const& c) ->  std::string { return c.toString();});
            std::copy(stringConstraints.begin(), stringConstraints.end(), std::ostream_iterator<std::string>(filestream, "\n"));
            filestream << "$Graph-preserving Constraints: \n";
            stringConstraints.clear();
            std::transform(constraintCollector.getGraphPreservingConstraints().begin(), constraintCollector.getGraphPreservingConstraints().end(), std::back_inserter(stringConstraints), [](carl::Formula<typename storm::Polynomial::PolyType> const& c) ->  std::string { return c.toString();});
            std::copy(stringConstraints.begin(), stringConstraints.end(), std::ostream_iterator<std::string>(filestream, "\n"));
            storm::utility::closeFile(filestream);
        }
    }
}
