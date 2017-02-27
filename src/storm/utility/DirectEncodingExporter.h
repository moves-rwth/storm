#pragma once
#include <iostream>
#include <memory>

#include "storm/models/sparse/Model.h"

namespace storm {
    namespace exporter {

        /*!
         * Exports a sparse model into the explicit DRN format.
         *
         * @param os           Stream to export to
         * @param sparseModel  Model to export
         * @param parameters   List of parameters
         */
        template<typename ValueType>
        void explicitExportSparseModel(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel, std::vector<std::string> const& parameters);
        
    }
}
