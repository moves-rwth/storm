#pragma once
#include <iostream>
#include <memory>

#include "storm/models/sparse/Model.h"

namespace storm {
    namespace exporter {
        
        template<typename ValueType>
        void explicitExportSparseModel(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel, std::vector<std::string> const& parameters);
        
        
    }
}
