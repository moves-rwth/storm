#include "storm/models/sparse/Pomdp.h"

namespace storm {
namespace pomdp {
/**
 * Observation-trace unrolling to allow model checking for monitoring.
 * This approach is outlined in  Junges, Hazem, Seshia  -- Runtime Monitoring for Markov Decision Processes
 * @tparam ValueType ValueType for probabilities
 */
template<typename ValueType>
class ObservationTraceUnfolder {
   public:
    /**
     * Initialize
     * @param model the MDP with state-based observations
     * @param risk the state risk
     * @param exprManager an Expression Manager
     */
    ObservationTraceUnfolder(storm::models::sparse::Pomdp<ValueType> const& model, std::vector<ValueType> const& risk,
                             std::shared_ptr<storm::expressions::ExpressionManager>& exprManager);
    /**
     * Transform in one shot
     * @param observations
     * @return
     */
    std::shared_ptr<storm::models::sparse::Mdp<ValueType>> transform(std::vector<uint32_t> const& observations);
    /**
     * Transform incrementaly
     * @param observation
     * @return
     */
    std::shared_ptr<storm::models::sparse::Mdp<ValueType>> extend(uint32_t observation);
    /**
     * When using the incremental approach, reset the observations made so far.
     * @param observation The new initial observation
     */
    void reset(uint32_t observation);

   private:
    storm::models::sparse::Pomdp<ValueType> const& model;
    std::vector<ValueType> risk;  // TODO reconsider holding this as a reference, but there were some strange bugs
    std::shared_ptr<storm::expressions::ExpressionManager>& exprManager;
    std::vector<storm::storage::BitVector> statesPerObservation;
    std::vector<uint32_t> traceSoFar;
    storm::expressions::Variable svvar;
};

}  // namespace pomdp
}  // namespace storm