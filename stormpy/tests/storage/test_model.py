import stormpy
import stormpy.logic
from helpers.helper import get_example_path

class TestModel:
    def test_build_dtmc_from_prism_program(self):
        program = stormpy.parse_prism_program(get_example_path("dtmc", "die", "die.pm"))
        prop = "P=? [F \"one\"]"
        formulas = stormpy.parse_formulas_for_prism_program(prop, program)
        pair = stormpy.build_model_from_prism_program(program, formulas)
        model = pair.model()
        assert model.nr_states() == 13
        assert model.nr_transitions() == 20
        assert model.model_type() == stormpy.ModelType.DTMC
        assert not model.supports_parameters()
        assert type(model) is stormpy.SparseDtmc
 
    def test_build_parametric_dtmc_from_prism_program(self):
        program = stormpy.parse_prism_program(get_example_path("pdtmc", "brp", "brp_16_2.pm"))
        assert program.nr_modules() == 5
        assert program.model_type() == stormpy.PrismModelType.DTMC
        assert program.has_undefined_constants()
        prop = "P=? [F \"target\"]"
        formulas = stormpy.parse_formulas_for_prism_program(prop, program)
        pair = stormpy.build_parametric_model_from_prism_program(program, formulas)
        model = pair.model()
        assert model.nr_states() == 613
        assert model.nr_transitions() == 803
        assert model.model_type() == stormpy.ModelType.DTMC
        assert model.supports_parameters()
        assert model.has_parameters()
        assert type(model) is stormpy.SparseParametricDtmc
    
    def test_build_dtmc(self):
        program = stormpy.parse_prism_program(get_example_path("dtmc", "die", "die.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_model(program, formulas[0])
        assert model.nr_states() == 13
        assert model.nr_transitions() == 20
        assert model.model_type() == stormpy.ModelType.DTMC
        assert not model.supports_parameters()
        assert type(model) is stormpy.SparseDtmc
    
    def test_build_parametric_dtmc(self):
        program = stormpy.parse_prism_program(get_example_path("pdtmc", "brp", "brp_16_2.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"target\" ]", program)
        model = stormpy.build_parametric_model(program, formulas[0])
        assert model.nr_states() == 613
        assert model.nr_transitions() == 803
        assert model.model_type() == stormpy.ModelType.DTMC
        assert model.supports_parameters()
        assert model.has_parameters()
        assert type(model) is stormpy.SparseParametricDtmc
    
    def test_build_dtmc_supporting_parameters(self):
        program = stormpy.parse_prism_program(get_example_path("dtmc", "die", "die.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_parametric_model(program, formulas[0])
        assert model.nr_states() == 13
        assert model.nr_transitions() == 20
        assert model.model_type() == stormpy.ModelType.DTMC
        assert model.supports_parameters()
        assert not model.has_parameters()
        assert type(model) is stormpy.SparseParametricDtmc

    def test_label(self):
        program = stormpy.parse_prism_program(get_example_path("dtmc", "die", "die.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_model(program, formulas[0])
        labels = model.labels()
        assert len(labels) == 2
        assert "init" in labels
        assert "one" in labels
        assert "init" in model.labels_state(0)
        assert "one" in model.labels_state(7)
    
    def test_initial_states(self):
        program = stormpy.parse_prism_program(get_example_path("dtmc", "die", "die.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"one\" ]", program)
        model = stormpy.build_model(program, formulas[0])
        initial_states =  model.initial_states()
        assert len(initial_states) == 1
        assert 0 in initial_states
    
    def test_label_parametric(self):
        program = stormpy.parse_prism_program(get_example_path("pdtmc", "brp", "brp_16_2.pm"))
        formulas = stormpy.parse_formulas_for_prism_program("P=? [ F \"target\" ]", program)
        model = stormpy.build_parametric_model(program, formulas[0])
        labels = model.labels()
        assert len(labels) == 2
        assert "init" in labels
        assert "target" in labels
        assert "init" in model.labels_state(0)
        assert "target" in model.labels_state(28)
        assert "target" in model.labels_state(611)
        initial_states = model.initial_states()
        assert len(initial_states) == 1
        assert 0 in initial_states
