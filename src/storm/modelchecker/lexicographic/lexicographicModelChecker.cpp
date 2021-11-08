//
// Created by steffi on 05.11.21.
//
#include "storm/modelchecker/lexicographic/lexicographicModelChecker.h"
#include "storm/models/sparse/Mdp.h"

namespace storm {
    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType>
        void lexicographic::lexicographicModelChecker<SparseModelType>::foo() {
            return;
        }

        template<typename SparseModelType>
        int lexicographicModelChecker<SparseModelType>::getCompleteProductModel(const SparseModelType& model, const logic::MultiObjectiveFormula& formula) {
            return 0;
        }

        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
        }
    }
}
