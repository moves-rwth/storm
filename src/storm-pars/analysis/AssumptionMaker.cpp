//
// Created by Jip Spel on 03.09.18.
//

#include "AssumptionMaker.h"

namespace storm {
    namespace analysis {
        template<typename ValueType>
        AssumptionMaker<ValueType>::AssumptionMaker(storm::analysis::LatticeExtender<ValueType>* latticeExtender, storm::analysis::AssumptionChecker<ValueType>* assumptionChecker, uint_fast64_t numberOfStates, bool validate) {
            this->latticeExtender = latticeExtender;
            this->numberOfStates = numberOfStates;
            this->assumptionChecker = assumptionChecker;
            this->validate = validate;
            this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>(storm::expressions::ExpressionManager());
            for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
                expressionManager->declareRationalVariable(std::to_string(i));
            }
        }

        template<typename ValueType>
        std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>
                AssumptionMaker<ValueType>::makeAssumptions(storm::analysis::Lattice *lattice, uint_fast64_t critical1,
                                                            uint_fast64_t critical2) {
            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;

            std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> emptySet;
            if (critical1 == numberOfStates || critical2 == numberOfStates) {
                result.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(lattice, emptySet));
            } else {
                storm::expressions::Variable var1 = expressionManager->getVariable(std::to_string(critical1));
                storm::expressions::Variable var2 = expressionManager->getVariable(std::to_string(critical2));

                std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions;
                auto myMap = createAssumptions(var1, var2, new Lattice(lattice), assumptions);
                result.insert(myMap.begin(), myMap.end());

                std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions2;
                myMap = createAssumptions(var2, var1, new Lattice(lattice), assumptions2);
                result.insert(myMap.begin(), myMap.end());
                if (result.size() == 0) {
                    // No assumptions could be made, all not valid based on sampling, therefore returning original lattice
                    result.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(lattice, emptySet));
                } else {
                    delete lattice;
                }
            }
            return result;
        }

        template<typename ValueType>
        std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> AssumptionMaker<ValueType>::runRecursive(storm::analysis::Lattice* lattice, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions) {
            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;
            // only the last assumption is new
            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> criticalTriple = this->latticeExtender->extendLattice(lattice, assumptions.back());

            if (validate && assumptionChecker->validated(assumptions.back())) {
                assert (assumptionChecker->valid(assumptions.back()));
                assumptions.pop_back();
            }

            if (std::get<1>(criticalTriple) == numberOfStates) {
                result.insert(std::pair<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>>(lattice, assumptions));
            } else {
                auto val1 = std::get<1>(criticalTriple);
                auto val2 = std::get<2>(criticalTriple);
                storm::expressions::Variable var1 = expressionManager->getVariable(std::to_string(val1));
                storm::expressions::Variable var2 = expressionManager->getVariable(std::to_string(val2));
                // TODO: check of in lattice de relatie niet al andersom bestaat

                assert(lattice->compare(val1, val2) == storm::analysis::Lattice::UNKNOWN);
                auto latticeCopy = new Lattice(lattice);
                std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptionsCopy = std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>(
                        assumptions);
                auto myMap = createAssumptions(var1, var2, latticeCopy, assumptionsCopy);
                result.insert(myMap.begin(), myMap.end());

                myMap = createAssumptions(var2, var1, lattice, assumptions);
                result.insert(myMap.begin(), myMap.end());
            }
            return result;
        }

        template <typename ValueType>
        std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> AssumptionMaker<ValueType>::createAssumptions(storm::expressions::Variable var1, storm::expressions::Variable var2, storm::analysis::Lattice* lattice, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions) {
            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> result;

            std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption
                = std::make_shared<storm::expressions::BinaryRelationExpression>(storm::expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(),
                        var1.getExpression().getBaseExpressionPointer(), var2.getExpression().getBaseExpressionPointer(),
                        storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual));
            if (assumptionChecker->checkOnSamples(assumption)) {
                if (validate) {
                    assumptionChecker->validateAssumption(assumption, lattice);
                    if (!assumptionChecker->validated(assumption) || assumptionChecker->valid(assumption)) {
                        assumptions.push_back(
                                std::shared_ptr<storm::expressions::BinaryRelationExpression>(assumption));
                        result = (runRecursive(lattice, assumptions));
                    }
                } else {
                    assumptions.push_back(std::shared_ptr<storm::expressions::BinaryRelationExpression>(assumption));
                    result = (runRecursive(lattice, assumptions));
                }


            } else {
                delete lattice;
            }

            return result;
        }

        template class AssumptionMaker<storm::RationalFunction>;
    }
}
