//
// Created by Jip Spel on 03.09.18.
//

#ifndef STORM_MONOTONICITYCHECKER_H
#define STORM_MONOTONICITYCHECKER_H

#include <map>
#include "Lattice.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/SparseMatrix.h"
#include "carl/core/Variable.h"

namespace storm {
    namespace analysis {

        template <typename ValueType>
        class MonotonicityChecker {

        public:
            /*!
             * Checks for all lattices in the map if they are monotone increasing or monotone decreasing.
             *
             * @param map The map with lattices and the assumptions made to create the lattices.
             * @param matrix The transition matrix.
             */
            void checkMonotonicity(std::map<storm::analysis::Lattice*, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>>> map, storm::storage::SparseMatrix<ValueType> matrix);

        private:
            std::map<carl::Variable, std::pair<bool, bool>> analyseMonotonicity(uint_fast64_t i, storm::analysis::Lattice* lattice, storm::storage::SparseMatrix<ValueType> matrix) ;

        };
    }
}
#endif //STORM_MONOTONICITYCHECKER_H
