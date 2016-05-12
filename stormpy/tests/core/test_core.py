import stormpy

class TestCore:
    def test_init(self):
        stormpy.set_up("")
    
    def test_pycarl(self):
        import pycarl
        import pycarl.formula
        import pycarl.parse
