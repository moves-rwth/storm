#pragma once

#include "storm/settings/SettingsManager.h"

#include "storm/utility/DirectEncodingExporter.h"
#include "storm/utility/DDEncodingExporter.h"
#include "storm/utility/file.h"
#include "storm/utility/macros.h"
#include "storm/storage/Scheduler.h"

namespace storm {
    
    namespace jani {
        class Model;
    }
    
    namespace api {
        
        void exportJaniModelAsDot(storm::jani::Model const& model, std::string const& filename);

        template <typename ValueType>
        void exportSparseModelAsDrn(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename, std::vector<std::string> const& parameterNames = {}, bool allowPlaceholders=true) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            storm::exporter::DirectEncodingOptions options;
            options.allowPlaceholders = allowPlaceholders;
            storm::exporter::explicitExportSparseModel(stream, model, parameterNames, options);
            storm::utility::closeFile(stream);
        }

        template<storm::dd::DdType Type, typename ValueType>
        void exportSparseModelAsDrdd(std::shared_ptr<storm::models::symbolic::Model<Type,ValueType>> const& model, std::string const& filename) {
            storm::exporter::explicitExportSymbolicModel(filename, model);
        }
        
        template <typename ValueType>
        void exportSparseModelAsDot(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename, size_t maxWidth = 30) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            model->writeDotToStream(stream, maxWidth);
            storm::utility::closeFile(stream);
        }

        template<storm::dd::DdType Type, typename ValueType>
        void exportSymbolicModelAsDot(std::shared_ptr<storm::models::symbolic::Model<Type,ValueType>> const& model, std::string const& filename) {
            model->writeDotToFile(filename);
        }
        
        template <typename ValueType>
        void exportScheduler(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::storage::Scheduler<ValueType> const& scheduler, std::string const& filename) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            std::string jsonFileExtension = ".json";
            if (filename.size() > 4 && std::equal(jsonFileExtension.rbegin(), jsonFileExtension.rend(), filename.rbegin())) {
                scheduler.printJsonToStream(stream, model);
            } else {
                scheduler.printToStream(stream, model);
            }
            storm::utility::closeFile(stream);
        }
        
    }
}
