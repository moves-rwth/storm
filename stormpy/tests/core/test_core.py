
class TestCore:
    def test_init(self):
        import stormpy
    
    def test_pycarl(self):
        import stormpy
        import pycarl
        import pycarl.formula
        import pycarl.parse
        pol1 = pycarl.FactorizedPolynomial(32)
        pol2 = pycarl.FactorizedPolynomial(2)
        rat = pycarl.FactorizedRationalFunction(pol1, pol2)
        assert str(rat) == "16"
