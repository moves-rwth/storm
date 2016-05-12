from . import core
from .core import *

core.set_up("")

def build_model(program, formulae):
    intermediate = core._build_model(program, formulae)
    
    if intermediate.parametric():
         raise RuntimeError("Model should be non-parametric")
    else:
         if intermediate.model_type() == ModelType.DTMC:
             return intermediate.as_dtmc()
         elif intermediate.model_type() == ModelType.MDP:
             return intermediate.as_mdp()
         else:
             raise RuntimeError("Not supported non-parametric model constructed")

def build_parametric_model(program, formulae):
    intermediate = core._build_parametric_model(program, formulae)
    
    if intermediate.parametric():
         if intermediate.model_type() == ModelType.DTMC:
             return intermediate.as_pdtmc()
         elif intermediate.model_type() == ModelType.MDP:
             return intermediate.as_pmdp()
         else:
             raise RuntimeError("Not supported parametric model constructed")
    else:
        raise RuntimeError("Model should be parametric")

def perform_bisimulation(model, formula, bisimulation_type):
    if model.parametric():
        return core._perform_parametric_bisimulation(model, formula, bisimulation_type)
    else:
        return core._perform_bisimulation(model, formula, bisimulation_type)
