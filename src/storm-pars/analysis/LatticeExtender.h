//
// Created by Jip Spel on 28.08.18.
//

#ifndef STORM_LATTICEEXTENDER_H
#define STORM_LATTICEEXTENDER_H

#include <storm/logic/Formula.h>
#include "storm/models/sparse/Dtmc.h"
#include "storm-pars/analysis/Lattice.h"
#include "storm/api/storm.h"




namespace storm {
    namespace analysis {


        template<typename ValueType>
        class LatticeExtender {

        public:
            LatticeExtender(std::shared_ptr<storm::models::sparse::Model<ValueType>> model);

            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas);

            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> extendLattice(storm::analysis::Lattice* lattice, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions);

        private:
            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;

            std::map<uint_fast64_t, storm::storage::BitVector> stateMap;

            std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> extendLattice(storm::analysis::Lattice* lattice);

        };
    }
}

#endif //STORM_LATTICEEXTENDER_H
