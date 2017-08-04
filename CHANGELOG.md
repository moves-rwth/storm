Changelog
==============

This changelog lists only the most important changes. Smaller (bug)fixes as well as non-mature features are not part of the changelog.
The releases of major and minor versions contain an overview of changes since the last major/minor update.

Version 1.1.x
-------------
Long run average computation via LRA, LP based MDP model checking, parametric model checking has an own binary

### Version 1.1.0 (2017/8)

- Support for long-run average rewards on MDPs and Markov automata using a value-iteration based approach.
- Storm can now check MDPs and Markov Automata (i.e. MinMax equation systems) via Linear Programming.
- Parametric model checking is now handled in a separated library/executable called storm-pars.
- Wellformedness constraints on PMCs:
    * include constraints from rewards
    * are in smtlib2
    * fixed
    * computation of only constraints without doing model checking is now supported
- Fix for nested formulae
- JANI: Explicit engine supports custom model compositions.
- Support for parsing/building models given in the explicit input format of IMCA.
- Storm now overwrites files if asked to write files to a specific location.
- Changes in build process to accommodate for changes in carl. Also, more robust against issues with carl.
- USE_POPCNT removed in favor of FORCE_POPCNT. The popcnt instruction is used if available due to march=native, unless portable is set.
    Then, using FORCE_POPCNT enables the use of the SSE 4.2 instruction


Version 1.0.x
-------------

### Version 1.0.1 (2017/4)

- Multi-objective model checking support now fully included
- Several improvements in parameter lifting
- Several improvements in JANI parsing
- Properties can contain model variables
- Support for rational numbers/functions in decision diagrams via sylvan
- Elimination-based solvers (exact solution) for models stored as decision diagrams
- Export of version and configuration to cmake
- Improved building process

### Version 1.0.0 (2017/3)
Start of this changelog
