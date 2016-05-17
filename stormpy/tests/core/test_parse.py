import stormpy
import stormpy.logic

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
