#include "src/builder/jit/StateBehaviour.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            StateBehaviour<IndexType, ValueType>::StateBehaviour() : compressed(true) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::addChoice(Choice<IndexType, ValueType>&& choice) {
                choices.emplace_back(std::move(choice));
                
            }
            
            template <typename IndexType, typename ValueType>
            Choice<IndexType, ValueType>& StateBehaviour<IndexType, ValueType>::addChoice() {
                choices.emplace_back();
                return choices.back();
            }
            
            template <typename IndexType, typename ValueType>
            typename StateBehaviour<IndexType, ValueType>::ContainerType const& StateBehaviour<IndexType, ValueType>::getChoices() const {
                return choices;
            }

            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::reduce(storm::jani::ModelType const& modelType) {
                if (choices.size() > 1) {
                    if (modelType == storm::jani::ModelType::DTMC || modelType == storm::jani::ModelType::CTMC) {
                        std::size_t totalCount = choices.size();
                        for (auto it = ++choices.begin(), ite = choices.end(); it != ite; ++it) {
                            choices.front().add(std::move(*it));
                        }
                        choices.resize(1);
                        choices.front().compress();

                        if (modelType == storm::jani::ModelType::DTMC) {
                            choices.front().divideDistribution(static_cast<ValueType>(totalCount));
                        }
                    }
                }
            }
            
            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::compress() {
                if (!compressed) {
                    for (auto& choice : choices) {
                        choice.compress();
                    }
                    compressed = true;
                }
            }
            
            template <typename IndexType, typename ValueType>
            bool StateBehaviour<IndexType, ValueType>::empty() const {
                return choices.empty();
            }
            
            template <typename IndexType, typename ValueType>
            std::size_t StateBehaviour<IndexType, ValueType>::size() const {
                return choices.size();
            }

            template <typename IndexType, typename ValueType>
            void StateBehaviour<IndexType, ValueType>::clear() {
                choices.clear();
                compressed = true;
            }
            
            template class StateBehaviour<uint32_t, double>;
            template class StateBehaviour<uint32_t, storm::RationalNumber>;
            template class StateBehaviour<uint32_t, storm::RationalFunction>;
            
        }
    }
}
