#include "storm-pomdp/analysis/QualitativeAnalysis.h"

#include "storm/utility/macros.h"
#include "storm/utility/graph.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace analysis {

        template<typename ValueType>
        QualitativeAnalysis<ValueType>::QualitativeAnalysis(storm::models::sparse::Pomdp<ValueType> const& pomdp) : pomdp(pomdp) {
            // Intentionally left empty
        }
    
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProb0(storm::logic::ProbabilityOperatorFormula const& formula) const {
            return analyseProb0or1(formula, true);
        }
        
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProb1(storm::logic::ProbabilityOperatorFormula const& formula) const {
            return analyseProb0or1(formula, false);
        }

        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProbSmaller1(storm::logic::ProbabilityOperatorFormula const &formula) const {
            STORM_LOG_THROW(formula.hasOptimalityType() || formula.hasBound(), storm::exceptions::InvalidPropertyException, "The formula " << formula << " does not specify whether to minimize or maximize.");
            bool minimizes = (formula.hasOptimalityType() && storm::solver::minimize(formula.getOptimalityType())) || (formula.hasBound() && storm::logic::isLowerBound(formula.getBound().comparisonType));
            STORM_LOG_THROW(!minimizes,storm::exceptions::NotImplementedException, "This operation is only supported when maximizing");
            std::shared_ptr<storm::logic::Formula const> subformula = formula.getSubformula().asSharedPointer();
            std::shared_ptr<storm::logic::UntilFormula> untilSubformula;
            // If necessary, convert the subformula to a more general case
            if (subformula->isEventuallyFormula()) {
                untilSubformula = std::make_shared<storm::logic::UntilFormula>(storm::logic::Formula::getTrueFormula(), subformula->asEventuallyFormula().getSubformula().asSharedPointer());
            } else if(subformula->isUntilFormula()) {
                untilSubformula = std::make_shared<storm::logic::UntilFormula>(subformula->asUntilFormula());
            }
            // The vector is sound, but not necessarily complete!
            return ~storm::utility::graph::performProb1E(pomdp.getTransitionMatrix(), pomdp.getTransitionMatrix().getRowGroupIndices(), pomdp.getBackwardTransitions(), checkPropositionalFormula(untilSubformula->getLeftSubformula()), checkPropositionalFormula(untilSubformula->getRightSubformula()));
        }
        
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProb0or1(storm::logic::ProbabilityOperatorFormula const& formula, bool prob0) const {
            // check whether the property is minimizing or maximizing
            STORM_LOG_THROW(pomdp.isCanonic(), storm::exceptions::IllegalArgumentException, "POMDP needs to be canonic");
            STORM_LOG_THROW(formula.hasOptimalityType() || formula.hasBound(), storm::exceptions::InvalidPropertyException, "The formula " << formula << " does not specify whether to minimize or maximize.");
            bool minimizes = (formula.hasOptimalityType() && storm::solver::minimize(formula.getOptimalityType())) || (formula.hasBound() && storm::logic::isLowerBound(formula.getBound().comparisonType));
            
            std::shared_ptr<storm::logic::Formula const> subformula = formula.getSubformula().asSharedPointer();
            // If necessary, convert the subformula to a more general case
            if (subformula->isEventuallyFormula()) {
                subformula = std::make_shared<storm::logic::UntilFormula>(storm::logic::Formula::getTrueFormula(), subformula->asEventuallyFormula().getSubformula().asSharedPointer());
            }
            
            if (subformula->isUntilFormula()) {
                if (minimizes && prob0) {
                    return analyseProb0Min(subformula->asUntilFormula());
                } else if (minimizes && !prob0){
                    return analyseProb1Min(subformula->asUntilFormula());
                } else if (!minimizes && prob0){
                    return analyseProb0Max(subformula->asUntilFormula());
                } else if (!minimizes && !prob0){
                    return analyseProb1Max(subformula->asUntilFormula());
                }
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Prob0or1 analysis is not supported for the property " << formula);
        }
    
    
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProb0Max(storm::logic::UntilFormula const& formula) const {
            return storm::utility::graph::performProb0A(pomdp.getBackwardTransitions(), checkPropositionalFormula(formula.getLeftSubformula()), checkPropositionalFormula(formula.getRightSubformula()));
        }
        
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProb0Min(storm::logic::UntilFormula const& formula) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Prob0 analysis is currently not implemented for minimizing properties.");
        }
        
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProb1Max(storm::logic::UntilFormula const& formula) const {
            // We consider the states that satisfy the formula with prob.1 under arbitrary schedulers as goal states.
            storm::storage::BitVector goalStates = storm::utility::graph::performProb1A(pomdp.getTransitionMatrix(), pomdp.getTransitionMatrix().getRowGroupIndices(), pomdp.getBackwardTransitions(), checkPropositionalFormula(formula.getLeftSubformula()), checkPropositionalFormula(formula.getRightSubformula()));
            STORM_LOG_TRACE("Prob1A states according to MDP: " << goalStates);
            // Now find a set of observations such that there is a memoryless scheduler inducing prob. 1 for each state whose observation is in the set.
            return goalStates;
            
        }
        
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::analyseProb1Min(storm::logic::UntilFormula const& formula) const {
            return storm::utility::graph::performProb1A(pomdp.getTransitionMatrix(), pomdp.getTransitionMatrix().getRowGroupIndices(), pomdp.getBackwardTransitions(), checkPropositionalFormula(formula.getLeftSubformula()), checkPropositionalFormula(formula.getRightSubformula()));
        }
    
        template<typename ValueType>
        storm::storage::BitVector QualitativeAnalysis<ValueType>::checkPropositionalFormula(storm::logic::Formula const& propositionalFormula) const {
            storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(pomdp);
            STORM_LOG_THROW(mc.canHandle(propositionalFormula), storm::exceptions::InvalidPropertyException, "Propositional model checker can not handle formula " << propositionalFormula);
            return mc.check(propositionalFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
        }


        template class QualitativeAnalysis<storm::RationalNumber>;

        template
        class QualitativeAnalysis<double>;
    }
}