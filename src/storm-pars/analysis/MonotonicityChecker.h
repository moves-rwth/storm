//
// Created by Jip Spel on 03.09.18.
//

#ifndef STORM_MONOTONICITYCHECKER_H
#define STORM_MONOTONICITYCHECKER_H

#include <map>
#include "Lattice.h"
#include "LatticeExtender.h"
#include "AssumptionMaker.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"

#include "carl/core/Variable.h"
#include "storm/models/ModelBase.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/Formula.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
    namespace analysis {

        template <typename ValueType>
        class MonotonicityChecker {

        public:
            MonotonicityChecker(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, bool validate);
            /*!
             * Checks for all lattices in the map if they are monotone increasing or monotone decreasing.
             *
             * @param map The map with lattices and the assumptions made to create the lattices.
             * @param matrix The transition matrix.
             * @return TODO
             */
            std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> checkMonotonicity(std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix);

            /*!
             * TODO
             * @param model
             * @param formulas
             * @param validate
             * @return
             */
            std::map<storm::analysis::Lattice*, std::map<carl::Variable, std::pair<bool, bool>>> checkMonotonicity();

            /*!
             * TODO
             * @param lattice
             * @param matrix
             * @return
             */
            bool somewhereMonotonicity(storm::analysis::Lattice* lattice) ;

        private:
            //TODO: variabele type
            std::map<carl::Variable, std::pair<bool, bool>> analyseMonotonicity(uint_fast64_t i, storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) ;

            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> createLattice();

            std::map<carl::Variable, std::pair<bool, bool>> checkOnSamples(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model, uint_fast64_t numberOfSamples);

            std::map<carl::Variable, std::pair<bool, bool>> checkOnSamples(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> model, uint_fast64_t numberOfSamples);

            std::map<storm::analysis::Lattice*, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> extendLatticeWithAssumptions(storm::analysis::Lattice* lattice, storm::analysis::AssumptionMaker<ValueType>* assumptionMaker, uint_fast64_t val1, uint_fast64_t val2, std::vector<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions);

            std::shared_ptr<storm::models::ModelBase> model;

            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;

            bool validate;

            std::map<carl::Variable, std::pair<bool, bool>> resultCheckOnSamples;

            storm::analysis::LatticeExtender<ValueType> *extender;
        };
    }
}
#endif //STORM_MONOTONICITYCHECKER_H
