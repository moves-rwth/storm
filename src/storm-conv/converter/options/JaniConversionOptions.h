#pragma once

#include <boost/optional.hpp>
#include <string>
#include <vector>

#include "storm-conv/settings/modules/JaniExportSettings.h"
#include "storm/storage/jani/ModelFeatures.h"

namespace storm {
namespace converter {

struct JaniConversionOptions {
    JaniConversionOptions();
    JaniConversionOptions(storm::settings::modules::JaniExportSettings const& settings);

    /// (Automaton,Variable)-pairs that will be transformed to location variables of the respective automaton.
    std::vector<std::pair<std::string, std::string>> locationVariables;

    /// If set, the model might have transient assignments to the edges
    bool edgeAssignments;

    /// If set, the model is transformed into a single automaton
    bool flatten;

    /// If set, constants in expressions are substituted with their definition
    bool substituteConstants;

    /// If set, variables will be made local wherever possible
    bool localVars;

    /// If set, variables will be made global wherever possible
    bool globalVars;

    /// If given, the model will get this name
    boost::optional<std::string> modelName;

    /// Only these model features are allowed in the output
    storm::jani::ModelFeatures allowedModelFeatures;

    /// Add constants that are defined in the properties to the model
    bool addPropertyConstants;

    /// If set, local and global variables that are (a) not assigned to some value and (b) have a known initial value are replaced by constants.
    bool replaceUnassignedVariablesWithConstants;

    /// If set, attempts to simplify the system composition
    bool simplifyComposition;

    /// If set, attempts to perform location elimination of states to reduce the state space of the final model
    bool locationElimination;

    /// Controls which locations are eliminated if location elimination is enabled
    uint64_t locationEliminationLocationHeuristic;

    // Controls which edges are eliminated if location elimination is enabled
    uint64_t locationEliminationEdgeHeuristic;
};
}  // namespace converter
}  // namespace storm
