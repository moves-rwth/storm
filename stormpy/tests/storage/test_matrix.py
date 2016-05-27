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
            assert e.value() == 0.5 or e.value() == 0 or (e.value() == 1 and e.column() > 6)
    
    def test_change_sparse_matrix(self):
        model = stormpy.parse_explicit_model("../examples/dtmc/die/die.tra", "../examples/dtmc/die/die.lab")
        matrix = model.transition_matrix()
        for e in matrix:
            assert e.value() == 0.5 or e.value() == 0 or e.value() == 1
        i = 0
        for e in matrix:
            e.set_value(i)
            i += 0.1
        i = 0
        for e in matrix:
            assert e.value() == i
            i += 0.1
    
    def test_change_sparse_matrix_modelchecking(self):
        import stormpy.logic
        model = stormpy.parse_explicit_model("../examples/dtmc/die/die.tra", "../examples/dtmc/die/die.lab")
        matrix = model.transition_matrix()
        # Check matrix
        for e in matrix:
            assert e.value() == 0.5 or e.value() == 0 or e.value() == 1
        # First model checking
        formulas = stormpy.parse_formulas("P=? [ F \"one\" ]")
        result = stormpy.model_checking(model, formulas[0])
        assert result == 0.16666666666666663
        
        # Change probabilities
        i = 0
        for e in matrix:
            if e.value() == 0.5:
                if i % 2 == 0:
                    e.set_value(0.3)
                else:
                    e.set_value(0.7)
                i += 1
        for e in matrix:
            assert e.value() == 0.3 or e.value() == 0.7 or e.value() == 1 or e.value() == 0
        # Second model checking
        result = stormpy.model_checking(model, formulas[0])
        assert result == 0.06923076923076932
        
        # Change probabilities again
        for state in stormpy.state.State(0, model):
            for action in state.actions():
                for transition in action.transitions():
                    if transition.value() == 0.3:
                        transition.set_value(0.8)
                    elif transition.value() == 0.7:
                        transition.set_value(0.2)
        # Third model checking
        result = stormpy.model_checking(model, formulas[0])
        assert result == 0.3555555555555556 or result == 0.3555555555555557
    
    def test_change_parametric_sparse_matrix_modelchecking(self):
        import stormpy.logic
        import pycarl
        program = stormpy.parse_prism_program("../examples/pdtmc/brp/brp_16_2.pm")
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"target\" ]", program)
        model = stormpy.build_parametric_model(program, formulas[0])
        matrix = model.transition_matrix()
        # Check matrix
        one_pol = pycarl.Rational(1)
        one_pol = pycarl.FactorizedPolynomial(one_pol)
        one = pycarl.FactorizedRationalFunction(one_pol, one_pol)
        for e in matrix:
            assert e.value() == one or len(e.value().gather_variables()) > 0
        # First model checking
        result = stormpy.model_checking(model, formulas[0])
        assert len(result.result_function().gather_variables()) > 0
        
        # Change probabilities
        two_pol = pycarl.Rational(2)
        two_pol = pycarl.FactorizedPolynomial(two_pol)
        new_val = pycarl.FactorizedRationalFunction(one_pol, two_pol)
        for e in matrix:
            if len(e.value().gather_variables()) > 0:
                e.set_value(new_val)
        for e in matrix:
            assert e.value() == new_val or e.value() == one
        # Second model checking
        result = stormpy.model_checking(model, formulas[0])
        assert len(result.result_function().gather_variables()) == 0
