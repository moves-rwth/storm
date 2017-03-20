# Change Log
All notable changes to Sylvan will be documented in this file.

## [Unreleased]
### Added
- The embedded work-stealing framework now explicitly checks for stack overflows and aborts with an appropriate error message written to stderr.
- New functions `sylvan_project` and `sylvan_and_project` for BDDs, a dual of existential quantification, where instead of the variables to remove, the given set of variables are the variables to keep.

### Changed
- Rewritten initialization of Sylvan. Before the call to `sylvan_init_package`, table sizes must be initialized either using `sylvan_set_sizes` or with the new function `sylvan_set_limits`. This new function allows the user to set a maximum number of bytes allocated for the nodes table and for the operation cache.

## [1.2.0] - 2017-02-03
### Added
- Added documentation in the docs directory using Sphinx. Some documentation is removed from the README.md file.

### Changed
- The custom terminal/leaf API is slightly modified. The `read_binary_cb` has a different signature to remove the dependency upon MTBDD functionality.
- The custom terminal/leaf API functions have been renamed and moved to a separate file.
- Lace has been updated with a new version. The new version has rewritten the hardware locality code that pins worker threads and memory.

### Fixed
- A bug in `mtbdd_reader_readbinary` has been fixed.

## [1.1.2] - 2017-01-11
### Fixed
- The pkg-config file is slightly improved.
- A critical bug in `sylvan_collect` has been fixed.

## [1.1.1] - 2017-01-10
### Fixed
- The pkg-config file now includes hwloc as a requirement

## [1.1.0] - 2017-01-09
### Added
- This CHANGELOG file.
- Custom leaves can now implement custom callbacks for writing/reading to/from files.
- Implemented GMP leaf writing/reading to/from file.
- Method `mtbdd_eval_compose` for proper function composition (after partial evaluation).
- Method `mtbdd_enum_par_*` for parallel path enumeration.
- LDD methods `relprod` and `relprev` now support action labels (meta 5).
- Examples program `ldd2bdd` now converts LDD transition systems to BDDs transition systems.
- Methods `cache_get6` and `cache_put6` for operation cache entries that require two buckets.
- File `sylvan.pc` for pkg-config.

### Changed
- The API to register a custom MTBDD leaf now requires multiple calls, which is better design for future extensions.
- When rehashing during garbage collection fails (due to finite length probe sequences), Sylvan now increases the probe sequence length instead of aborting with an error message. However, Sylvan will probably still abort due to the table being full, since this error is typically triggered when garbage collection does not remove many dead nodes.

### Fixed
- Methods `mtbdd_enum_all_*` fixed and rewritten.

### Removed
- We no longer use both autoconf makefiles and CMake. Instead, we removed the autoconf files and rely solely on CMake now.
