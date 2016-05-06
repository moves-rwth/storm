import stormpy

class TestParse:
    def test_parse_program(self):
        program = stormpy.parse_program("../examples/dtmc/die/die.pm")
        assert program.nr_modules() == 1
        assert program.model_type() == stormpy.PrismModelType.DTMC
        assert not program.has_undefined_constants()
    
    def test_parse_formula(self):
        prop = "P=? [F \"one\"]"
        formulas = stormpy.parse_formulas(prop)
        assert len(formulas) == 1
        assert str(formulas[0]) == prop
    
    def test_build_model_from_prism_program(self):
        program = stormpy.parse_program("../examples/dtmc/die/die.pm")
        prop = "P=? [F \"one\"]"
        formulas = stormpy.parse_formulas_for_program(prop, program)
        pair = stormpy.build_model_from_prism_program(program, formulas)
        model = pair.model
        assert model.nr_states() == 13
        assert model.nr_transitions() == 20
        assert model.model_type() == stormpy.ModelType.DTMC
        assert not model.parametric()
    
    def test_build_model(self):
        program = stormpy.parse_program("../examples/dtmc/die/die.pm")
        formulas = stormpy.parse_formulas_for_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_model(program, formulas[0])
        assert model.nr_states() == 13
        assert model.nr_transitions() == 20
        assert model.model_type() == stormpy.ModelType.DTMC
        assert model.parametric()
