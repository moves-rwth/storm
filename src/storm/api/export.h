#pragma once

#include "storm/settings/SettingsManager.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/io/DDEncodingExporter.h"
#include "storm/io/DirectEncodingExporter.h"
#include "storm/io/file.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/Scheduler.h"
#include "storm/utility/macros.h"

namespace storm {

namespace jani {
class Model;
}

namespace api {

void exportJaniModelAsDot(storm::jani::Model const& model, std::string const& filename);

template<typename ValueType>
void exportSparseModelAsDrn(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename,
                            std::vector<std::string> const& parameterNames = {}, bool allowPlaceholders = true) {
    std::ofstream stream;
    storm::utility::openFile(filename, stream);
    storm::exporter::DirectEncodingOptions options;
    options.allowPlaceholders = allowPlaceholders;
    storm::exporter::explicitExportSparseModel(stream, model, parameterNames, options);
    storm::utility::closeFile(stream);
}

template<storm::dd::DdType Type, typename ValueType>
void exportSymbolicModelAsDrdd(std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> const& model, std::string const& filename) {
    storm::exporter::explicitExportSymbolicModel(filename, model);
}

template<typename ValueType>
void exportSparseModelAsDot(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename, size_t maxWidth = 30) {
    std::ofstream stream;
    storm::utility::openFile(filename, stream);
    model->writeDotToStream(stream, maxWidth);
    storm::utility::closeFile(stream);
}

template<typename ValueType>
void exportSparseModelAsJson(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::string const& filename) {
    std::ofstream stream;
    storm::utility::openFile(filename, stream);
    model->writeJsonToStream(stream);
    storm::utility::closeFile(stream);
}

template<storm::dd::DdType Type, typename ValueType>
void exportSymbolicModelAsDot(std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> const& model, std::string const& filename) {
    model->writeDotToFile(filename);
}

template<typename ValueType>
void exportScheduler(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::storage::Scheduler<ValueType> const& scheduler,
                     std::string const& filename) {
    std::ofstream stream;
    storm::utility::openFile(filename, stream);
    std::string jsonFileExtension = ".json";
    if (filename.size() > 4 && std::equal(jsonFileExtension.rbegin(), jsonFileExtension.rend(), filename.rbegin())) {
        scheduler.printJsonToStream(stream, model, false, true);
    } else {
        scheduler.printToStream(stream, model, false, true);
    }
    storm::utility::closeFile(stream);
}

template<typename ValueType>
inline void exportCheckResultToJson(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model,
                                    std::unique_ptr<storm::modelchecker::CheckResult> const& checkResult, std::string const& filename) {
    std::ofstream stream;
    storm::utility::openFile(filename, stream);
    if (checkResult->isExplicitQualitativeCheckResult()) {
        stream << checkResult->asExplicitQualitativeCheckResult().toJson(model->getOptionalStateValuations(), model->getStateLabeling()).dump(4);
    } else {
        STORM_LOG_THROW(checkResult->isExplicitQuantitativeCheckResult(), storm::exceptions::NotSupportedException,
                        "Export of check results is only supported for explicit check results (e.g. in the sparse engine)");
        stream << checkResult->template asExplicitQuantitativeCheckResult<ValueType>()
                      .toJson(model->getOptionalStateValuations(), model->getStateLabeling())
                      .dump(4);
    }
    storm::utility::closeFile(stream);
}

template<>
inline void exportCheckResultToJson<storm::RationalFunction>(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> const&,
                                                             std::unique_ptr<storm::modelchecker::CheckResult> const&, std::string const&) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Export of check results is not supported for rational functions. ");
}

}  // namespace api
}  // namespace storm
