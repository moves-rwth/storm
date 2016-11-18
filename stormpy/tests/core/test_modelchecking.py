import stormpy
import stormpy.logic
from helpers.helper import get_example_path

class TestModelChecking:
    def test_model_checking_dtmc(self):
        program = stormpy.parse_prism_program(get_example_path("dtmc", "die.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_model(program, formulas[0])
        assert model.nr_states == 13
        assert model.nr_transitions == 20
        result = stormpy.model_checking(model, formulas[0])
        assert result == 0.16666666666666663
    
    def test_model_checking_all_dtmc(self):
        program = stormpy.parse_prism_program(get_example_path("dtmc", "die.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_model(program, formulas[0])
        assert model.nr_states == 13
        assert model.nr_transitions == 20
        results = stormpy.model_checking_all(model, formulas[0])
        results_orig = [0.16666666666666663, 0.3333333333333333, 0, 0.6666666666666666, 0, 0, 0, 1, 0, 0, 0, 0, 0]
        assert results == results_orig
    
    def test_parametric_state_elimination(self):
        import pycarl
        import pycarl.formula
        program = stormpy.parse_prism_program(get_example_path("pdtmc", "brp16_2.pm"))
        prop = "P=? [F s=5]"
        formulas = stormpy.parse_formulas_for_prism_program(prop, program)
        model = stormpy.build_parametric_model_from_prism_program(program, formulas)
        assert model.nr_states == 613
        assert model.nr_transitions == 803
        assert model.model_type == stormpy.ModelType.DTMC
        assert model.has_parameters
        result = stormpy.model_checking(model, formulas[0])
        func = result.result_function
        one = pycarl.FactorizedPolynomial(pycarl.Rational(1))
        assert func.denominator == one
        constraints_well_formed = result.constraints_well_formed
        for constraint in constraints_well_formed:
            assert constraint.rel() == pycarl.formula.Relation.GEQ or constraint.rel() == pycarl.formula.Relation.LEQ
        constraints_graph_preserving = result.constraints_graph_preserving
        for constraint in constraints_graph_preserving:
            assert constraint.rel() == pycarl.formula.Relation.GREATER
