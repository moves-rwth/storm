Changelog
==============

This changelog lists only the most important changes. Smaller (bug)fixes as well as non-mature features are not part of the changelog.
The releases of major and minor versions contain an overview of changes since the last major/minor update.


Version 1.0.x
-------------------

### Version 1.0.2 (2017/5)

- Fix for nested formulae
- JANI: Explicit engine supports custom model compositions.
- Storm now overwrites files if asked to write files to a specific location
- Changes in build process to accommodate for changes in carl. Also, more robust against issues with carl
- Wellformedness constraints on PMCs:
    * include constraints from rewards
    * are in smtlib2
- USE_POPCNT removed in favor of FORCE_POPCNT. The popcnt instruction is used if available due to march=native, unless portable is set.
    Then, using FORCE_POPCNT enables the use of the SSE 4.2 instruction
- Parametric model checking is now handled in a separated library/executable called storm-pars
- Support for long-run average rewwards on Markov automata using the value-iteration based approach by Butkova et al. (TACAS 17)

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
