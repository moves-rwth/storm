#pragma once

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace api {
        
        void exportJaniModel(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filename) {
            auto janiSettings = storm::settings::getModule<storm::settings::modules::JaniExportSettings>();
            
            if (janiSettings.isExportAsStandardJaniSet()) {
                storm::jani::Model normalisedModel = model;
                normalisedModel.makeStandardJaniCompliant();
                storm::jani::JsonExporter::toFile(normalisedModel, properties, filename);
            } else {
                storm::jani::JsonExporter::toFile(model, properties, filename);
            }
        }
        
        void exportJaniModelAsDot(storm::jani::Model const& model, std::string const& filename) {
            std::ofstream out;
            storm::utility::openFile(filename, out);
            model.writeDotToStream(out);
            storm::utility::closeFile(out);
        }

        template <typename ValueType>
        void exportParametricResultToFile(ValueType const& result, storm::analysis::ConstraintCollector<ValueType> const& constraintCollector, std::string const& path) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot export non-parametric result.");
        }
        
        template <>
        void exportParametricResultToFile(storm::RationalFunction const& result, storm::analysis::ConstraintCollector<storm::RationalFunction> const& constraintCollector, std::string const& path) {
            std::ofstream filestream;
            storm::utility::openFile(path, filestream);
            filestream << "!Parameters: ";
            std::set<storm::RationalFunctionVariable> vars = result.gatherVariables();
            std::copy(vars.begin(), vars.end(), std::ostream_iterator<storm::RationalFunctionVariable>(filestream, "; "));
            filestream << std::endl;
            filestream << "!Result: " << result << std::endl;
            filestream << "!Well-formed Constraints: " << std::endl;
            std::vector<std::string> stringConstraints;
            std::transform(constraintCollector.getWellformedConstraints().begin(), constraintCollector.getWellformedConstraints().end(), std::back_inserter(stringConstraints), [](carl::Formula<typename storm::Polynomial::PolyType> const& c) ->  std::string { return c.toString();});
            std::copy(stringConstraints.begin(), stringConstraints.end(), std::ostream_iterator<std::string>(filestream, "\n"));
            filestream << "!Graph-preserving Constraints: " << std::endl;
            stringConstraints.clear();
            std::transform(constraintCollector.getGraphPreservingConstraints().begin(), constraintCollector.getGraphPreservingConstraints().end(), std::back_inserter(stringConstraints), [](carl::Formula<typename storm::Polynomial::PolyType> const& c) ->  std::string { return c.toString();});
            std::copy(stringConstraints.begin(), stringConstraints.end(), std::ostream_iterator<std::string>(filestream, "\n"));
            storm::utility::closeFile(filestream);
        }
        
        template <typename ValueType>
        void exportSparseModelAsDrn(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename, std::vector<std::string> const& parameterNames) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            storm::exporter::explicitExportSparseModel(stream, model, parameterNames);
            storm::utility::closeFile(stream);
        }
        
        template <typename ValueType>
        void exportSparseModelAsDot(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            model->writeDotToStream(stream);
            storm::utility::closeFile(stream);
        }
        
    }
}
