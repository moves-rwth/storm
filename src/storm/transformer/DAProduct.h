#pragma

#include "storm/automata/AcceptanceCondition.h"
#include "storm/transformer/Product.h"
#include <memory>

namespace storm {
    namespace transformer {

        template <typename Model>
        class DAProduct : public Product<Model> {
        public:
            typedef std::shared_ptr<DAProduct<Model>> ptr;

            DAProduct(Product<Model>&& product, storm::automata::AcceptanceCondition::ptr acceptance)
                : Product<Model>(std::move(product)), acceptance(acceptance) {
                // Intentionally left blank
            }

            storm::automata::AcceptanceCondition::ptr getAcceptance() {
                return acceptance;
            }
        private:
            storm::automata::AcceptanceCondition::ptr acceptance;
        };
    }
}
