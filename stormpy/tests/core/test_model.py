import stormpy
import stormpy.logic

class TestModel:
    def test_build_dtmc_from_prism_program(self):
        stormpy.set_up("")
        program = stormpy.parse_program("../examples/dtmc/die/die.pm")
        prop = "P=? [F \"one\"]"
        formulas = stormpy.parse_formulas_for_program(prop, program)
        pair = stormpy.build_model_from_prism_program(program, formulas)
        model = pair.model
        assert model.nr_states() == 13
        assert model.nr_transitions() == 20
        assert model.model_type() == stormpy.ModelType.DTMC
        assert not model.parametric()
        assert type(model) is stormpy.SparseDtmc
 
    def test_build_parametric_dtmc_from_prism_program(self):
        program = stormpy.parse_program("../examples/pdtmc/brp/brp_16_2.pm")
        assert program.nr_modules() == 5
        assert program.model_type() == stormpy.PrismModelType.DTMC
        assert program.has_undefined_constants()
        prop = "P=? [F \"target\"]"
        formulas = stormpy.parse_formulas_for_program(prop, program)
        pair = stormpy.build_parametric_model_from_prism_program(program, formulas)
        model = pair.model
        assert model.nr_states() == 613
        assert model.nr_transitions() == 803
        assert model.model_type() == stormpy.ModelType.DTMC
        assert model.parametric()
        assert type(model) is stormpy.SparseParametricDtmc
    
    def test_build_dtmc(self):
        program = stormpy.parse_program("../examples/dtmc/die/die.pm")
        formulas = stormpy.parse_formulas_for_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_model(program, formulas[0])
        assert model.nr_states() == 13
        assert model.nr_transitions() == 20
        assert model.model_type() == stormpy.ModelType.DTMC
        assert not model.parametric()
        assert type(model) is stormpy.SparseDtmc
    
    def test_build_parametric_dtmc(self):
        program = stormpy.parse_program("../examples/pdtmc/brp/brp_16_2.pm")
        formulas = stormpy.parse_formulas_for_program("P=? [ F \"target\" ]", program)
        model = stormpy.build_parametric_model(program, formulas[0])
        assert model.nr_states() == 613
        assert model.nr_transitions() == 803
        assert model.model_type() == stormpy.ModelType.DTMC
        assert model.parametric()
        assert type(model) is stormpy.SparseParametricDtmc
