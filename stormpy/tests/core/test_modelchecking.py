import stormpy
import stormpy.logic

class TestModelChecking:
    def test_state_elimination(self):
        import pycarl
        import pycarl.formula
        program = stormpy.parse_program("../examples/pdtmc/brp/brp_16_2.pm")
        prop = "P=? [F \"target\"]"
        formulas = stormpy.parse_formulas_for_program(prop, program)
        pair = stormpy.build_parametric_model_from_prism_program(program, formulas)
        model = pair.model
        assert model.nr_states() == 613
        assert model.nr_transitions() == 803
        assert model.model_type() == stormpy.ModelType.DTMC
        assert model.parametric()
        result = stormpy.perform_state_elimination(model, formulas[0])
        func = result.result_function
        one = pycarl.FactorizedPolynomial(pycarl.Rational(1))
        assert func.denominator == one
        constraints_well_formed = result.constraints_well_formed
        for constraint in constraints_well_formed:
            assert constraint.rel() == pycarl.formula.Relation.GEQ or constraint.rel() == pycarl.formula.Relation.LEQ
        constraints_graph_preserving = result.constraints_graph_preserving
        for constraint in constraints_graph_preserving:
            assert constraint.rel() == pycarl.formula.Relation.GREATER
