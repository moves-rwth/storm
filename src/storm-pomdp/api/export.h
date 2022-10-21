#pragma once

#include "storm-pomdp/io/PomdpToJuliaExport.h"
#include "storm/io/file.h"

namespace storm {
namespace pomdp {
namespace api {
template<typename ValueType>
void exportSparsePomdpAsJuliaModel(std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> const& model, std::string const& filename, double discount, storm::logic::Formula const& formula) {
    std::ofstream stream;
    storm::utility::openFile(filename, stream);
    storm::pomdp::exporter::exportPomdpToJulia(stream,model,  discount, formula);
    storm::utility::closeFile(stream);
}

}
}
}