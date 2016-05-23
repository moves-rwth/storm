"""
It looks like you want to know about 'stormpy'.

                        _.-;:q=._ 
                      .' j=""^k;:\. 
                     ; .F       ";`Y
                    ,;.J_        ;'j
                  ,-;"^7F       : .F           _________________
                 ,-'-_<.        ;gj. _.,---""''               .'
                ;  _,._`\.     : `T"5,                       ; 
                : `?8w7 `J  ,-'" -^q. `                     ;  
                 \;._ _,=' ;   n58L Y.                     .' 
                   F;";  .' k_ `^'  j'                     ;  
                   J;:: ;     "y:-='                      ;   
                    L;;==      |:;   jT\                  ;
                    L;:;J      J:L  7:;'       _         ; 
                    I;|:.L     |:k J:.' ,  '       .     ;
                    |;J:.|     ;.I F.:      .           : 
                   ;J;:L::     |.| |.J  , '   `    ;    ; 
                 .' J:`J.`.    :.J |. L .    ;         ; 
                ;    L :k:`._ ,',j J; |  ` ,        ; ; 
              .'     I :`=.:."_".'  L J             `.'
            .'       |.:  `"-=-'    |.J              ; 
        _.-'         `: :           ;:;           _ ; 
    _.-'"             J: :         /.;'       ;    ; 
  ='_                  k;.\.    _.;:Y'     ,     .' 
     `"---..__          `Y;."-=';:='     ,      .'
              `""--..__   `"==="'    -        .' 
                       ``""---...__    itz .-' 
                                   ``""---'
"""

from . import core
from .core import *
from . import storage
from .storage import *

core.set_up("")

def build_model(program, formulae):
    intermediate = core._build_model(program, formulae)
    assert not intermediate.supports_parameters()
    if intermediate.model_type() == ModelType.DTMC:
        return intermediate.as_dtmc()
    elif intermediate.model_type() == ModelType.MDP:
        return intermediate.as_mdp()
    else:
        raise RuntimeError("Not supported non-parametric model constructed")

def build_parametric_model(program, formulae):
    intermediate = core._build_parametric_model(program, formulae)
    assert intermediate.supports_parameters()
    if intermediate.model_type() == ModelType.DTMC:
        return intermediate.as_pdtmc()
    elif intermediate.model_type() == ModelType.MDP:
        return intermediate.as_pmdp()
    else:
        raise RuntimeError("Not supported parametric model constructed")

def perform_bisimulation(model, formula, bisimulation_type):
    if model.supports_parameters():
        return core._perform_parametric_bisimulation(model, formula, bisimulation_type)
    else:
        return core._perform_bisimulation(model, formula, bisimulation_type)

def model_checking(model, formula):
    if model.supports_parameters():
        return core._parametric_model_checking(model, formula)
    else:
        return core._model_checking(model, formula)
