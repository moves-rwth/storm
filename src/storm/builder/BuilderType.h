#pragma once

#include <boost/optional.hpp>
#include <vector>

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/Property.h"

namespace storm {
namespace jani {
class ModelFeatures;
}

namespace builder {
enum class BuilderType { Explicit, Dd };

storm::jani::ModelFeatures getSupportedJaniFeatures(BuilderType const& builderType);

template<typename ValueType>
bool canHandle(BuilderType const& builderType, storm::storage::SymbolicModelDescription const& modelDescription,
               boost::optional<std::vector<storm::jani::Property>> const& properties = boost::none);
}  // namespace builder
}  // namespace storm