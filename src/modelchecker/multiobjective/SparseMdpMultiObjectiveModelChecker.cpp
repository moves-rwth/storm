#include "src/modelchecker/multiobjective/SparseMdpMultiObjectiveModelChecker.h"

#include "src/utility/macros.h"
#include "src/logic/Formulas.h"
#include "src/logic/FragmentSpecification.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveModelCheckerInformation.h"
#include "src/modelchecker/multiobjective/helper/SparseMdpMultiObjectivePreprocessingHelper.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveModelCheckerHelper.h"

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
            
            helper::SparseMultiObjectiveModelCheckerInformation<SparseMdpModelType> info = helper::SparseMdpMultiObjectivePreprocessingHelper<SparseMdpModelType>::preprocess(checkTask.getFormula(), this->getModel());
            std::cout << std::endl;
            std::cout << "Preprocessed Information:" << std::endl;
            info.printInformationToStream(std::cout);
            
#ifdef STORM_HAVE_CARL
            helper::SparseMultiObjectiveModelCheckerHelper<SparseMdpModelType, storm::RationalNumber>::check(info);
#else
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Multi objective model checking currently requires carl.");
#endif
            
            std::cout << "Information after helper call: " << std::endl;
            info.printInformationToStream(std::cout);
            
            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult());
        }
        
        
                
        template class SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>>;
    }
}
