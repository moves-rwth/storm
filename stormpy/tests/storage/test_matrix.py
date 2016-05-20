import stormpy
import stormpy.storage

class TestMatrix:
    def test_sparse_matrix(self):
        model = stormpy.parse_explicit_model("../examples/dtmc/die/die.tra", "../examples/dtmc/die/die.lab")
        matrix = model.transition_matrix()
        assert type(matrix) is stormpy.storage.SparseMatrix
        assert matrix.nr_rows() == model.nr_states()
        assert matrix.nr_columns() == model.nr_states()
        assert matrix.nr_entries() == 27 #model.nr_transitions()
        for e in matrix:
            assert e.val == 0.5 or e.val == 0 or (e.val == 1 and e.column > 6)
    
    def test_change_sparse_matrix(self):
        model = stormpy.parse_explicit_model("../examples/dtmc/die/die.tra", "../examples/dtmc/die/die.lab")
        matrix = model.transition_matrix()
        for e in matrix:
            assert e.val == 0.5 or e.val == 0 or e.val == 1
        i = 0
        for e in matrix:
            e.val = i
            i += 0.1
        i = 0
        for e in matrix:
            assert e.val == i
            i += 0.1
