#include "src/modelchecker/multiobjective/SparseMaMultiObjectiveModelChecker.h"

#include "src/utility/macros.h"
#include "src/logic/Formulas.h"
#include "src/logic/FragmentSpecification.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessor.h"
#include "src/modelchecker/multiobjective/helper/SparseMaMultiObjectiveWeightVectorChecker.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveHelper.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePostprocessor.h"

#include "src/utility/Stopwatch.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMaModelType>
        SparseMaMultiObjectiveModelChecker<SparseMaModelType>::SparseMaMultiObjectiveModelChecker(SparseMaModelType const& model) : SparseMarkovAutomatonCslModelChecker<SparseMaModelType>(model) {
            // Intentionally left empty.
        }
        
        template<typename SparseMaModelType>
        bool SparseMaMultiObjectiveModelChecker<SparseMaModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            // A formula without multi objective (sub)formulas can be handled by the base class
            if(SparseMarkovAutomatonCslModelChecker<SparseMaModelType>::canHandle(checkTask)) return true;
            //In general, each initial state requires an individual scheduler (in contrast to single objective model checking). Let's exclude this.
            if(this->getModel().getInitialStates().getNumberOfSetBits() > 1) return false;
            if(!checkTask.isOnlyInitialStatesRelevantSet()) return false;
            return checkTask.getFormula().isInFragment(storm::logic::multiObjective().setTimeAllowed(true).setTimeBoundedUntilFormulasAllowed(true));
        }
        
        template<typename SparseMaModelType>
        std::unique_ptr<CheckResult> SparseMaMultiObjectiveModelChecker<SparseMaModelType>::checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula> const& checkTask) {
            STORM_LOG_ASSERT(this->getModel().getInitialStates().getNumberOfSetBits() == 1, "Multi-objective Model checking on model with multiple initial states is not supported.");
            STORM_LOG_THROW(this->getModel().isClosed(), storm::exceptions::InvalidArgumentException, "Unable to check multi-objective formula in non-closed Markov automaton.");
            std::unique_ptr<CheckResult> result;
            
#ifdef STORM_HAVE_CARL
            storm::utility::Stopwatch swPreprocessing;
            auto preprocessorData = helper::SparseMultiObjectivePreprocessor<SparseMaModelType>::preprocess(checkTask.getFormula(), this->getModel());
            swPreprocessing.pause();
            STORM_LOG_DEBUG("Preprocessing done. Data: " << preprocessorData);
            
            storm::utility::Stopwatch swHelper;
            std::shared_ptr<helper::SparseMultiObjectiveWeightVectorChecker<SparseMaModelType>> weightVectorChecker( new helper::SparseMaMultiObjectiveWeightVectorChecker<SparseMaModelType>(preprocessorData));
            auto resultData = helper::SparseMultiObjectiveHelper<SparseMaModelType, storm::RationalNumber>::check(preprocessorData, weightVectorChecker);
            swHelper.pause();
            STORM_LOG_DEBUG("Modelchecking done.");
            
            storm::utility::Stopwatch swPostprocessing;
            result = helper::SparseMultiObjectivePostprocessor<SparseMaModelType, storm::RationalNumber>::postprocess(preprocessorData, resultData, swPreprocessing, swHelper, swPostprocessing);
            STORM_LOG_DEBUG("Postprocessing done.");
#else
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Multi-objective model checking requires carl.");
#endif
            return result;
        }
        
        
                
        template class SparseMaMultiObjectiveModelChecker<storm::models::sparse::MarkovAutomaton<double>>;
    }
}
