#include "storm/environment/Environment.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/storage/SchedulerChoice.h"
#include "storm/utility/macros.h"

#include "storm/modelchecker/lexicographic/lexicographicModelChecking.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
namespace modelchecker {
namespace lexicographic {

template<typename SparseModelType, typename ValueType>
helper::MDPSparseModelCheckingHelperReturnType<ValueType> check(Environment const& env, SparseModelType const& model,
                                                                CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask,
                                                                CheckFormulaCallback const& formulaChecker) {
    STORM_LOG_ASSERT(model.getInitialStates().getNumberOfSetBits() == 1,
                     "Lexicographic Model checking on model with multiple initial states is not supported.");
    storm::logic::MultiObjectiveFormula const& formula = checkTask.getFormula();

    // Define the helper that contains all functions
    helper::lexicographic::lexicographicModelCheckerHelper<SparseModelType, ValueType, true> lMC =
        helper::lexicographic::lexicographicModelCheckerHelper<SparseModelType, ValueType, true>(formula, model.getTransitionMatrix());

    // get the product of (i) the product-automaton of all subformuale, and (ii) the model
    auto res = lMC.getCompleteProductModel(model, formulaChecker);

    std::shared_ptr<storm::transformer::DAProduct<SparseModelType>> completeProductModel = res.first;
    std::vector<uint> accCond = res.second;

    // get the lexicogrpahic array for all MEC of the product-model
    std::pair<storm::storage::MaximalEndComponentDecomposition<ValueType>, std::vector<std::vector<bool>>> result =
        lMC.getLexArrays(completeProductModel, accCond);
    storm::storage::MaximalEndComponentDecomposition<ValueType> mecs = result.first;
    std::vector<std::vector<bool>> mecLexArrays = result.second;

    // solve the reachability query
    // That is: solve reachability for the lexicographic highest condition, restrict the model to optimal actions, repeat
    return lMC.lexReachability(mecs, mecLexArrays, completeProductModel, model);
}

template helper::MDPSparseModelCheckingHelperReturnType<double> check<storm::models::sparse::Mdp<double>, double>(
    Environment const& env, storm::models::sparse::Mdp<double> const& model, CheckTask<storm::logic::MultiObjectiveFormula, double> const& checkTask,
    CheckFormulaCallback const& formulaChecker);
template helper::MDPSparseModelCheckingHelperReturnType<storm::RationalNumber> check<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>(
    Environment const& env, storm::models::sparse::Mdp<storm::RationalNumber> const& model,
    CheckTask<storm::logic::MultiObjectiveFormula, storm::RationalNumber> const& checkTask, CheckFormulaCallback const& formulaChecker);
}  // namespace lexicographic
}  // namespace modelchecker
}  // namespace storm
