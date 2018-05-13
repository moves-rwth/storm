Changelog
==============

This changelog lists only the most important changes. Smaller (bug)fixes as well as non-mature features are not part of the changelog.
The releases of major and minor versions contain an overview of changes since the last major/minor update.

Version 1.2.x
-------------

### Version 1.2.2 (to be released)
- `storm-dft`: improvements in Galileo parser
- `storm-dft`: test cases for DFT analysis
- Sound value iteration (SVI) for DTMCs and MDPs
- Topological solver for linear equation systems and MinMax equation systems.


### Version 1.2.1 (2018/02)
- Multi-dimensional reward bounded reachability properties for DTMCs.
- `storm-dft`: transformation of DFTs to GSPNs
- Several bug fixes

### Version 1.2.0 (2017/12)
- C++ api changes: Building model takes `BuilderOptions` instead of extended list of Booleans, does not depend on settings anymore.
- `storm-cli-utilities` now contains cli related stuff, instead of `storm-lib`
- Symbolic (MT/BDD) bisimulation
- Fixed issue related to variable names that can not be used in Exprtk.
- DRN parser improved
- LP-based MDP model checking
- Sound (interval) value iteration
- Support for Multi-objective multi-dimensional reward bounded reachability properties for MDPs.
- RationalSearch method to solve equation systems exactly
- WalkerChae method for solving linear equation systems with guaranteed convergence
- Performance improvements for sparse model building
- Performance improvements for conditional properties on MDPs
- Automatically convert MA without probabilistic states into CTMC
- Fixed implemention of Fox and Glynn' algorithm
- `storm-pars`: support for welldefinedness constraints in mdps.
- `storm-dft`: split DFT settings into IO settings and fault tree settings
- `storm-dft`: removed obsolete explicit model builder for DFTs
- Features for developers:
	* Solvers can now expose requirements
	* unbounded reachability and reachability rewards now correctly respect solver requirements
	* Environment variables (such as the solver precisions) can now be handled more flexible
	* changes to Matrix-Vector operation interfaces, in particular fixed some issues with the use Intel TBB


Version 1.1.x
-------------

### Version 1.1.0 (2017/8)
- Support for long-run average rewards on MDPs and Markov automata using a value-iteration based approach.
- Storm can now check MDPs and Markov Automata (i.e. MinMax equation systems) via Linear Programming.
- Parametric model checking is now handled in a separated library/executable called `storm-pars`.
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
- `USE_POPCNT` removed in favor of `FORCE_POPCNT`. The popcnt instruction is used if available due to `march=native`, unless portable is set.
  Then, using `FORCE_POPCNT` enables the use of the SSE 4.2 instruction


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
