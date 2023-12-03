#pragma once

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "storm/models/sparse/ChoiceLabeling.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/PlayerIndex.h"
#include "storm/storage/prism/Program.h"
#include "storm/storage/sparse/PrismChoiceOrigins.h"
#include "storm/storage/sparse/StateValuations.h"

namespace storm {
namespace builder {

/*!
 * This class collects information regarding the states and choices during model building.
 * It is expected that the provided indices are in ascending order
 */
class StateAndChoiceInformationBuilder {
   public:
    StateAndChoiceInformationBuilder();

    void setBuildChoiceLabels(bool value);
    bool isBuildChoiceLabels() const;
    void addChoiceLabel(std::string const& label, uint_fast64_t choiceIndex);
    storm::models::sparse::ChoiceLabeling buildChoiceLabeling(uint_fast64_t totalNumberOfChoices);

    void setBuildChoiceOrigins(bool value);
    bool isBuildChoiceOrigins() const;
    void addChoiceOriginData(std::any const& originData, uint_fast64_t choiceIndex);
    std::vector<std::any> buildDataOfChoiceOrigins(uint_fast64_t totalNumberOfChoices);

    void setBuildStatePlayerIndications(bool value);
    bool isBuildStatePlayerIndications() const;
    /*!
     * @note: skipped choiceIndices get assigned the invalid player index.
     */
    void addStatePlayerIndication(storm::storage::PlayerIndex player, uint_fast64_t stateIndex);
    bool hasStatePlayerIndicationBeenSet(storm::storage::PlayerIndex expectedPlayer, uint_fast64_t stateIndex) const;
    std::vector<storm::storage::PlayerIndex> buildStatePlayerIndications(uint_fast64_t totalNumberOfStates);

    void setBuildMarkovianStates(bool value);
    bool isBuildMarkovianStates() const;
    void addMarkovianState(uint_fast64_t markovianStateIndex);
    storm::storage::BitVector buildMarkovianStates(uint_fast64_t totalNumberOfStates);

    void setBuildStateValuations(bool value);
    bool isBuildStateValuations() const;
    storm::storage::sparse::StateValuationsBuilder& stateValuationsBuilder();

   private:
    bool _buildChoiceLabels;
    std::unordered_map<std::string, storm::storage::BitVector> _choiceLabels;

    bool _buildChoiceOrigins;
    std::vector<std::any> _dataOfChoiceOrigins;

    bool _buildStatePlayerIndications;
    std::vector<storm::storage::PlayerIndex> _statePlayerIndications;

    bool _buildMarkovianStates;
    storm::storage::BitVector _markovianStates;

    bool _buildStateValuations;
    storm::storage::sparse::StateValuationsBuilder _stateValuationsBuilder;
};
}  // namespace builder
}  // namespace storm
