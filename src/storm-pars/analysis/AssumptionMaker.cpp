//
// Created by Jip Spel on 03.09.18.
//

#include "AssumptionMaker.h"

namespace storm {
    namespace analysis {
        template<typename ValueType>
        AssumptionMaker<ValueType>::AssumptionMaker(storm::analysis::LatticeExtender<ValueType>* latticeExtender, uint_fast64_t numberOfStates) {
            this->latticeExtender = latticeExtender;
            this->numberOfStates = numberOfStates;
            this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
            for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
                expressionManager->declareIntegerVariable(std::to_string(i));
                expressionManager->declareFreshIntegerVariable();
            }
        }


        template<typename ValueType>
        std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>
                AssumptionMaker<ValueType>::startMakingAssumptions(storm::analysis::Lattice* lattice, uint_fast64_t critical1, uint_fast64_t critical2) {

            std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;

            std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> emptySet;
            if (critical1 == numberOfStates || critical2 == numberOfStates) {
                result.insert(std::pair<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(lattice, emptySet));
            } else {
                storm::expressions::Variable var1 = expressionManager->getVariable(std::to_string(critical1));
                storm::expressions::Variable var2 = expressionManager->getVariable(std::to_string(critical2));
                std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions;

                auto myMap = createAssumptions(var1, var2, lattice, assumptions);
                result.insert(myMap.begin(), myMap.end());

                myMap = createAssumptions(var2, var1, lattice, assumptions);
                result.insert(myMap.begin(), myMap.end());
            }
            return result;
        }

        template<typename ValueType>
        std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> AssumptionMaker<ValueType>::runRecursive(storm::analysis::Lattice* lattice, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions) {
            std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;
            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> criticalPair = this->latticeExtender->extendLattice(lattice, assumptions);

            if (std::get<1>(criticalPair) == numberOfStates) {
                result.insert(std::pair<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(lattice, assumptions));
            } else {
                storm::expressions::Variable var1 = expressionManager->getVariable(std::to_string(std::get<1>(criticalPair)));
                storm::expressions::Variable var2 = expressionManager->getVariable(std::to_string(std::get<2>(criticalPair)));

                auto myMap = createAssumptions(var1, var2, lattice, assumptions);
                result.insert(myMap.begin(), myMap.end());
                myMap = createAssumptions(var2, var1, lattice, assumptions);
                result.insert(myMap.begin(), myMap.end());
            }

            return result;
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> AssumptionMaker<ValueType>::createAssumptions(storm::expressions::Variable var1, storm::expressions::Variable var2, storm::analysis::Lattice* lattice, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions) {
            std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions1 = std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>(assumptions);
            std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption1
                = std::make_shared<storm::expressions::BinaryRelationExpression>(storm::expressions::BinaryRelationExpression(*expressionManager, var1.getType(),
                        var1.getExpression().getBaseExpressionPointer(), var2.getExpression().getBaseExpressionPointer(),
                        storm::expressions::BinaryRelationExpression::RelationType::Greater));
            assumptions1.insert(assumption1);
            auto lattice1 = new Lattice(lattice);
            return (runRecursive(lattice1, assumptions1));
        }

        template class AssumptionMaker<storm::RationalFunction>;
    }
}


// Een map met daarin een pointer naar de lattic en een set met de geldende assumptions voor die lattice