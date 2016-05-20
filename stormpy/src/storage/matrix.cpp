#include "matrix.h"

#include "src/storage/SparseMatrix.h"

typedef storm::storage::SparseMatrix<double>::index_type entry_index;

void define_sparse_matrix(py::module& m) {

    py::class_<storm::storage::MatrixEntry<entry_index, double>>(m, "SparseMatrixEntry", "Entry of sparse matrix")
        .def("__str__", [](storm::storage::MatrixEntry<entry_index, double> const& entry) {
                std::stringstream stream;
                stream << entry;
                return stream.str();
            })
        .def_property("val", &storm::storage::MatrixEntry<entry_index, double>::getValue, &storm::storage::MatrixEntry<entry_index, double>::setValue, "Value")
        .def_property_readonly("column", &storm::storage::MatrixEntry<entry_index, double>::getColumn, "Column")
    ;
 
    py::class_<storm::storage::SparseMatrix<double>>(m, "SparseMatrix", "Sparse matrix")
        //.def("__str__", &storm::logic::Formula::toString)
        .def("__iter__", [](storm::storage::SparseMatrix<double> const& matrix) {
                return py::make_iterator(matrix.begin(), matrix.end());
            }, py::keep_alive<0, 1>() /* Essential: keep object alive while iterator exists */)
        .def("nr_rows", &storm::storage::SparseMatrix<double>::getRowCount, "Number of rows")
        .def("nr_columns", &storm::storage::SparseMatrix<double>::getColumnCount, "Number of columns")
        .def("nr_entries", &storm::storage::SparseMatrix<double>::getEntryCount, "Number of non-zero entries")
    ;

}
