#pragma once

#include <unordered_map>

#include "storm-gspn/builder/JaniGSPNBuilder.h"
#include "storm-gspn/storage/gspn/GSPN.h"
#include "storm/storage/jani/Model.h"

namespace storm {
namespace api {

/**
 *    Builds JANI model from GSPN.
 */
storm::jani::Model* buildJani(storm::gspn::GSPN const& gspn);

void handleGSPNExportSettings(
    storm::gspn::GSPN const& gspn, std::function<std::vector<storm::jani::Property>(storm::builder::JaniGSPNBuilder const&)> const& janiProperyGetter =
                                       [](storm::builder::JaniGSPNBuilder const&) { return std::vector<storm::jani::Property>(); });

std::unordered_map<std::string, uint64_t> parseCapacitiesList(std::string const& filename, storm::gspn::GSPN const& gspn);

}  // namespace api
}  // namespace storm
