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

#include "src/utility/Stopwatch.h"

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
            return checkTask.getFormula().isInFragment(storm::logic::multiObjective().setStepBoundedUntilFormulasAllowed(true));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpMultiObjectiveModelChecker<SparseMdpModelType>::checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula> const& checkTask) {
            STORM_LOG_ASSERT(this->getModel().getInitialStates().getNumberOfSetBits() == 1, "Multi Objective Model checking on model with multiple initial states is not supported.");
            storm::utility::Stopwatch swModelChecking;
            std::unique_ptr<CheckResult> result;
            
#ifdef STORM_HAVE_CARL
            storm::utility::Stopwatch swPreprocessing;
            auto preprocessorData = helper::SparseMultiObjectivePreprocessor<SparseMdpModelType>::preprocess(checkTask.getFormula(), this->getModel());
            swPreprocessing.pause();
            std::cout << std::endl;
            preprocessorData.printToStream(std::cout);
            STORM_LOG_DEBUG("Preprocessing done.");
            
            storm::utility::Stopwatch swHelper;
            auto resultData = helper::SparseMultiObjectiveHelper<SparseMdpModelType, storm::RationalNumber>::check(preprocessorData);
            swHelper.pause();
            STORM_LOG_DEBUG("Modelchecking done.");
            
            storm::utility::Stopwatch swPostprocessing;
            result = helper::SparseMultiObjectivePostprocessor<SparseMdpModelType, storm::RationalNumber>::postprocess(preprocessorData, resultData);
            swPostprocessing.pause();
            STORM_LOG_DEBUG("Postprocessing done.");
#else
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Multi objective model checking requires carl.");
#endif
            std::cout << std::endl;
            std::cout << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
            std::cout << "                                                 Multi-objective Model Checking Statistics:                                            " << std::endl;
            std::cout << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
            std::cout << std::endl;
            std::cout << "Runtimes (in seconds):" << std::endl;
            std::cout << "\t Preprocessing:    " << std::setw(8) << swPreprocessing << std::endl;
            std::cout << "\t Value Iterations: " << std::setw(8) << swHelper << std::endl;
            std::cout << "\t Postprocessing:   " << std::setw(8) << swPostprocessing << std::endl;
            std::cout << "\t Combined:         " << std::setw(8) << swModelChecking << std::endl;
            std::cout << std::endl;
            std::cout << "Performed Refinement Steps: " << resultData.refinementSteps().size() << std::endl;
            std::cout << std::endl;
            std::cout << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
            return result;
        }
        
        
                
        template class SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>>;
    }
}
