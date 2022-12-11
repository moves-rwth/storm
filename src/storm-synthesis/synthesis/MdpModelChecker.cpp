#include "storm-synthesis/synthesis/MdpModelChecker.h"

#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace synthesis {

        template<typename ValueType>
        std::shared_ptr<storm::modelchecker::CheckResult> verifyMdp(
            storm::Environment const& env,
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const& mdp,
            storm::logic::Formula const& formula,
            bool produce_schedulers
        ) {
            storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> task(formula);
            task.setProduceSchedulers(produce_schedulers);
            storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
            return modelchecker.check(env, task);
        }

        template std::shared_ptr<storm::modelchecker::CheckResult> verifyMdp<double>(
            storm::Environment const& env,
            std::shared_ptr<storm::models::sparse::Mdp<double>> const& mdp,
            storm::logic::Formula const& formula,
            bool produce_schedulers
        );

    } 
}
