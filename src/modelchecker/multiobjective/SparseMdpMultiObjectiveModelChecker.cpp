#include "src/modelchecker/multiobjective/SparseMdpMultiObjectiveModelChecker.h"

#include "src/utility/macros.h"
#include "src/logic/Formulas.h"
#include "src/logic/FragmentSpecification.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessor.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveHelper.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePostprocessor.h"

#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMdpModelType>
        SparseMdpMultiObjectiveModelChecker<SparseMdpModelType>::SparseMdpMultiObjectiveModelChecker(SparseMdpModelType const& model) : SparseMdpPrctlModelChecker<SparseMdpModelType>(model) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        bool SparseMdpMultiObjectiveModelChecker<SparseMdpModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            // A formula without multi objective (sub)formulas can be handled by the base class
            if(SparseMdpPrctlModelChecker<SparseMdpModelType>::canHandle(checkTask)) return true;
            //In general, each initial state requires an individual scheduler (in contrast to single objective model checking). Let's exclude this.
            if(this->getModel().getInitialStates().getNumberOfSetBits() > 1) return false;
            if(!checkTask.isOnlyInitialStatesRelevantSet()) return false;
            return checkTask.getFormula().isInFragment(storm::logic::multiObjective());
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpMultiObjectiveModelChecker<SparseMdpModelType>::checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula> const& checkTask) {
            STORM_LOG_ASSERT(this->getModel().getInitialStates().getNumberOfSetBits() == 1, "Multi Objective Model checking on model with multiple initial states is not supported.");
            std::unique_ptr<CheckResult> result;
            
#ifdef STORM_HAVE_CARL
            auto preprocessorData = helper::SparseMultiObjectivePreprocessor<SparseMdpModelType>::preprocess(checkTask.getFormula(), this->getModel());
            std::cout << std::endl;
            std::cout << "Preprocessing done." << std::endl;
            preprocessorData.printToStream(std::cout);
            auto resultData = helper::SparseMultiObjectiveHelper<SparseMdpModelType, storm::RationalNumber>::check(preprocessorData);
            std::cout << "Modelchecking done." << std::endl;
            result = helper::SparseMultiObjectivePostprocessor<SparseMdpModelType, storm::RationalNumber>::postprocess(preprocessorData, resultData);
            std::cout << "Postprocessing done." << std::endl;
#else
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Multi objective model checking requires carl.");
#endif
            return result;
        }
        
        
                
        template class SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>>;
    }
}
