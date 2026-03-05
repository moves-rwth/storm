#include "storm/storage/umb/Umb.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/storage/umb/export/SparseModelToUmb.h"
#include "storm/storage/umb/export/UmbExport.h"
#include "storm/storage/umb/import/SparseModelFromUmb.h"
#include "storm/storage/umb/import/UmbImport.h"
#include "storm/storage/umb/model/UmbModel.h"

namespace storm::umb {

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<ValueType>> buildModelFromUmb(std::filesystem::path const& umbLocation, ImportOptions const& options) {
    auto umb = storm::umb::importUmb(umbLocation, options);
    STORM_LOG_INFO("Imported UMB model:\n" << umb.getModelInformation());
    return storm::umb::sparseModelFromUmb<ValueType>(umb, options);
}

std::shared_ptr<storm::models::ModelBase> buildModelFromUmb(std::filesystem::path const& umbLocation, ImportOptions const& options) {
    auto umb = storm::umb::importUmb(umbLocation, options);
    STORM_LOG_INFO("Imported UMB model:\n" << umb.getModelInformation());
    return storm::umb::sparseModelFromUmb(umb, options);
}

template<typename ValueType>
void exportModelToUmb(storm::models::sparse::Model<ValueType> const& model, std::filesystem::path const& targetLocation, ExportOptions const& options) {
    auto umb = storm::umb::sparseModelToUmb(model, options);
    umb.encodeRationals();  // Ensure rationals are encoded in their right bit representation before export
    STORM_LOG_INFO("Exporting UMB model:\n" << umb.getModelInformation());
    storm::umb::toArchive(umb, targetLocation, options);
}

template std::shared_ptr<storm::models::sparse::Model<double>> buildModelFromUmb(std::filesystem::path const&, ImportOptions const&);
template std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> buildModelFromUmb(std::filesystem::path const&, ImportOptions const&);
template std::shared_ptr<storm::models::sparse::Model<storm::Interval>> buildModelFromUmb(std::filesystem::path const&, ImportOptions const&);

template void exportModelToUmb(storm::models::sparse::Model<double> const&, std::filesystem::path const&, ExportOptions const&);
template void exportModelToUmb(storm::models::sparse::Model<storm::RationalNumber> const&, std::filesystem::path const&, ExportOptions const&);
template void exportModelToUmb(storm::models::sparse::Model<storm::Interval> const&, std::filesystem::path const&, ExportOptions const&);

}  // namespace storm::umb
