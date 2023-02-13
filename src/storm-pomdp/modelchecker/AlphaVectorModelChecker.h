#include "logic/Formula.h"
#include "storm-pomdp/builder/BeliefMdpExplorer.h"
#include "storm-pomdp/parser/AlphaVectorPolicyParser.h"
#include "storm-pomdp/storage/BeliefManager.h"
#include "utility/ConstantsComparator.h"

namespace storm {
namespace pomdp {
namespace modelchecker {
    template<typename PomdpModelType, typename BeliefValueType = typename PomdpModelType::ValueType, typename BeliefMDPType = typename PomdpModelType::ValueType>
    class AlphaVectorModelChecker {
       public:
            typedef storm::storage::BeliefManager<PomdpModelType, BeliefValueType> BeliefManagerType;
            typedef storm::builder::BeliefMdpExplorer<PomdpModelType, BeliefValueType> ExplorerType;
            typedef typename PomdpModelType::ValueType PomdpValueType;

            AlphaVectorModelChecker(std::shared_ptr<PomdpModelType> pomdp, storm::pomdp::storage::AlphaVectorPolicy<BeliefValueType> alphaVectorPolicy);

            //TODO return result structure
            void check(storm::logic::Formula const& formula);

       private:

            PomdpModelType const& pomdp() const;

            bool buildMarkovChain(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& beliefExplorer, std::vector<typename PomdpModelType::ValueType> const &cutoffVec);

            std::string getBestActionInBelief(uint64_t beliefId, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer);

            std::shared_ptr<PomdpModelType> inputPomdp;
            std::shared_ptr<PomdpModelType> preprocessedPomdp;

            storm::pomdp::storage::AlphaVectorPolicy<BeliefValueType> inputPolicy;

            storm::utility::ConstantsComparator<BeliefValueType> beliefTypeCC;
            storm::utility::ConstantsComparator<typename PomdpModelType::ValueType> valueTypeCC;

            double prec;
    };

}
}
}
