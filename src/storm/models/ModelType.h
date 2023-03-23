#ifndef STORM_MODELS_MODELTYPE_H_
#define STORM_MODELS_MODELTYPE_H_

#include <iosfwd>

namespace storm {
namespace models {
// All supported model types.
enum class ModelType { Dtmc, Ctmc, Mdp, MarkovAutomaton, S2pg, Pomdp, Smg };

ModelType getModelType(std::string const& type);

std::ostream& operator<<(std::ostream& os, ModelType const& type);
}  // namespace models
}  // namespace storm

#endif /* STORM_MODELS_MODELTYPE_H_ */
