#pragma once

#include "storm/models/sparse/ItemLabeling.h"

namespace storm {
namespace models {
namespace sparse {

/*!
 * This class manages the labeling of the choice space with a number of (atomic) labels.
 */
class ChoiceLabeling : public ItemLabeling {
   public:
    /*!
     * Constructs an empty labeling for the given number of choices.
     *
     * @param choiceCount The number of choices for which this labeling can hold the labels.
     */
    ChoiceLabeling(uint_fast64_t choiceCount = 0);

    ChoiceLabeling(ChoiceLabeling const& other) = default;
    ChoiceLabeling(ItemLabeling const& other);
    ChoiceLabeling(ItemLabeling const&& other);
    ChoiceLabeling& operator=(ChoiceLabeling const& other) = default;

    virtual bool isChoiceLabeling() const override;

    /*!
     * Checks whether the two labelings are equal.
     *
     * @param other The labeling with which the current one is compared.
     * @return True iff the labelings are equal.
     */
    bool operator==(ChoiceLabeling const& other) const;

    /*!
     * Retrieves the sub labeling that represents the same labeling as the current one for all selected choices.
     *
     * @param choices The selected set of choices.
     */
    ChoiceLabeling getSubLabeling(storm::storage::BitVector const& choices) const;

    /*!
     * Retrieves the set of labels attached to the given choice.
     *
     * @param choice The choice for which to retrieve the labels.
     * @return The labels attached to the given choice.
     */
    std::set<std::string> getLabelsOfChoice(uint64_t choice) const;

    /*!
     * Adds a label to a given choice.
     *
     * @param label The name of the label to add.
     * @param choice The index of the choice to label.
     */
    void addLabelToChoice(std::string const& label, uint64_t choice);

    /*!
     * Checks whether a given choice is labeled with the given label.
     *
     * @param label The name of the label.
     * @param choice The index of the choice to check.
     * @return True if the node is labeled with the label, false otherwise.
     */
    bool getChoiceHasLabel(std::string const& label, uint64_t choice) const;

    /*!
     * Returns the labeling of choices associated with the given label.
     *
     * @param label The name of the label.
     * @return A bit vector that represents the labeling of the choices with the given label.
     */
    storm::storage::BitVector const& getChoices(std::string const& label) const;

    /*!
     * Sets the labeling of choices associated with the given label.
     *
     * @param label The name of the label.
     * @param labeling A bit vector that represents the set of choices that will get this label.
     */
    void setChoices(std::string const& label, storage::BitVector const& labeling);

    /*!
     * Sets the labeling of choices associated with the given label.
     *
     * @param label The name of the label.
     * @param labeling A bit vector that represents the set of choices that will get this label.
     */
    void setChoices(std::string const& label, storage::BitVector&& labeling);

    friend std::ostream& operator<<(std::ostream& out, ChoiceLabeling const& labeling);
};

}  // namespace sparse
}  // namespace models
}  // namespace storm