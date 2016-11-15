import pycarl
import stormpy
import stormpy.logic

class TestFormulas:

    def test_probability_formula(self):
        prop = "P=? [F \"one\"]"
        formulas = stormpy.parse_formulas(prop)
        formula = formulas[0]
        assert type(formula) == stormpy.logic.ProbabilityOperator
        assert len(formulas) == 1
        assert str(formula) == prop

    def test_reward_formula(self):
        prop = "R=? [F \"one\"]"
        formulas = stormpy.parse_formulas(prop)
        formula = formulas[0]
        assert type(formula) == stormpy.logic.RewardOperator
        assert len(formulas) == 1
        assert str(formula) == "R[exp]=? [F \"one\"]"

    def test_formula_list(self):
        formulas = []
        prop = "=? [F \"one\"]"
        forms = stormpy.parse_formulas("P" + prop)
        formulas.append(forms[0])
        forms = stormpy.parse_formulas("R" + prop)
        formulas.append(forms[0])
        assert len(formulas) == 2
        assert str(formulas[0]) == "P" + prop
        assert str(formulas[1]) == "R[exp]" + prop

    def test_bounds(self):
        prop = "P=? [F \"one\"]"
        formula = stormpy.parse_formulas(prop)[0]
        assert not formula.has_bound
        prop = "P<0.4 [F \"one\"]"
        formula = stormpy.parse_formulas(prop)[0]
        assert formula.has_bound
        assert formula.threshold == pycarl.Rational("0.4")
        assert formula.comparison_type == stormpy.logic.ComparisonType.LESS

    def test_set_bounds(self):
        prop = "P<0.4 [F \"one\"]"
        formula = stormpy.parse_formulas(prop)[0]
        formula.threshold = pycarl.Rational("0.2")
        formula.comparison_type = stormpy.logic.ComparisonType.GEQ
        assert formula.threshold == pycarl.Rational("0.2")
        assert formula.comparison_type == stormpy.logic.ComparisonType.GEQ
        assert str(formula) == "P>=1/5 [F \"one\"]"
