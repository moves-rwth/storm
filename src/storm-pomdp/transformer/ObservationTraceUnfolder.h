#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace pomdp {
        template<typename ValueType>
        class ObservationTraceUnfolder {

        public:
            ObservationTraceUnfolder(storm::models::sparse::Pomdp<ValueType> const& model,  std::vector<ValueType> const& risk, std::shared_ptr<storm::expressions::ExpressionManager>& exprManager);
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> transform(std::vector<uint32_t> const& observations);
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> extend(uint32_t observation);
            void reset(uint32_t observation);
        private:
            storm::models::sparse::Pomdp<ValueType> const& model;
            std::vector<ValueType> risk; // TODO reconsider holding this as a reference, but there were some strange bugs
            std::shared_ptr<storm::expressions::ExpressionManager>& exprManager;
            std::vector<storm::storage::BitVector> statesPerObservation;
            std::vector<uint32_t> traceSoFar;
            storm::expressions::Variable svvar;

        };

    }
}