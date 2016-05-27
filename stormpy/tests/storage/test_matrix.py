import stormpy

class TestMatrix:
    def test_sparse_matrix(self):
        model = stormpy.parse_explicit_model("../examples/dtmc/die/die.tra", "../examples/dtmc/die/die.lab")
        matrix = model.transition_matrix()
        assert type(matrix) is stormpy.storage.SparseMatrix
        assert matrix.nr_rows() == model.nr_states()
        assert matrix.nr_columns() == model.nr_states()
        assert matrix.nr_entries() == 27 #model.nr_transitions()
        for e in matrix:
            assert e.val() == 0.5 or e.val() == 0 or (e.val() == 1 and e.column() > 6)
    
    def test_change_sparse_matrix(self):
        model = stormpy.parse_explicit_model("../examples/dtmc/die/die.tra", "../examples/dtmc/die/die.lab")
        matrix = model.transition_matrix()
        for e in matrix:
            assert e.val() == 0.5 or e.val() == 0 or e.val() == 1
        i = 0
        for e in matrix:
            e.set_val(i)
            i += 0.1
        i = 0
        for e in matrix:
            assert e.val() == i
            i += 0.1
    
    def test_change_sparse_matrix_modelchecking(self):
        import stormpy.logic
        model = stormpy.parse_explicit_model("../examples/dtmc/die/die.tra", "../examples/dtmc/die/die.lab")
        matrix = model.transition_matrix()
        # Check matrix
        for e in matrix:
            assert e.val() == 0.5 or e.val() == 0 or e.val() == 1
        # First model checking
        formulas = stormpy.parse_formulas("P=? [ F \"one\" ]")
        result = stormpy.model_checking(model, formulas[0])
        assert result == 0.16666666666666663
        
        # Change probabilities
        i = 0
        for e in matrix:
            if e.val() == 0.5:
                if i % 2 == 0:
                    e.set_val(0.3)
                else:
                    e.set_val(0.7)
                i += 1
        for e in matrix:
            assert e.val() == 0.3 or e.val() == 0.7 or e.val() == 1 or e.val() == 0
        # Second model checking
        result = stormpy.model_checking(model, formulas[0])
        assert result == 0.06923076923076932
        
        # Change probabilities again
        for state in stormpy.state.State(0, model):
            for action in state.actions():
                for transition in action.transitions():
                    if transition.val() == 0.3:
                        transition.set_val(0.8)
                    elif transition.val() == 0.7:
                        transition.set_val(0.2)
        # Third model checking
        result = stormpy.model_checking(model, formulas[0])
        assert result == 0.3555555555555556 or result == 0.3555555555555557
