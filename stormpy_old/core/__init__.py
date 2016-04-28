from stormpy.core._core import *
        
def _build_model(program, formulae):
    intermediate = _core.build_model(program, formulae)
    if intermediate.parametric:
        if intermediate.model_type == ModelType.DTMC:
            return intermediate.as_pdtmc()
        elif intermediate.model_type == ModelType.MDP:
            return intermediate.as_pmdp()
        else:
            raise RuntimeError("Not supported parametric model constructed")
    else:
        if intermediate.model_type == ModelType.DTMC:
            return intermediate.as_pdtmc()
        elif intermediate.model_type == ModelType.MDP:
            return intermediate.as_pmdp()
        else:
            raise RuntimeError("Not supported non-parametric model constructed")

build_model = _build_model