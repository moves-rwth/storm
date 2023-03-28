# Change Log
All notable changes to Sylvan will be documented in this file.


## [1.7.1] - 2022-09-04
### Added
- Now complains when certain Sylvan functions are used outside a Lace worker thread.

### Changed
- Slightly streamlined `mtbdd_makenode` to generate more optimal code.
- Lace is now a dependency of Sylvan instead of included in the source code.


## [1.7.0] - 2022-09-03
### Added
- Rudimentary support for zero-suppressed decision diagrams (ZDDs). 
- Implemented irredundant sum-of-products `ISOP` that computes the ZDD cover of a BDD.

### Changed
- Another new version of Lace, using C11 atomics instead of the old abuse of the volatile keyword. This is a fairly major change and so far it even appears to improve performance a little bit.
- The name of Sylvan in CMake is now `sylvan` or `sylvan::sylvan`
- Possibly breaking change: `mtbdd_enum_first` and `mtbdd_enum_next` no longer check if `variables` covers all variables encountered in the decision diagram, allowing partial evaluation.
- Various small changes to the CMake scripts.

### Removed
- Support for Sylvan as a shared library is removed.

### Fixed
- Fixed OSX build
- Fixed warnings on modern gcc in `sha2.c`
- Fixed test and example programs that use Sylvan wrongly. Sylvan functions should always be executed from a Lace worker.
- A major performance regression introduced in version 1.6.0 has been fixed.


## [1.6.1] - 2021-06-20
### Changed
- New version of Lace with slightly better support for OSX and a few minor changes.

### Fixed
- A bug with `sylvan_set_limits` has been fixed.


## [1.6.0] - 2021-04-19
### Changed
- Major API change in Lace. This will break things!
- Lace is now started with `lace_start` and stopped with `lace_stop`. The initializing thread is no longer a Lace thread. Instead, starting Lace with N workers will simply start N threads. To temporarily halt the workers (because they use 100% CPU time), use `lace_suspend` and `lace_resume`.
- The macros `SPAWN`, `SYNC` and `CALL` only work from Lace tasks. The `LACE_ME` macro is gone. If you are not inside a Lace task, use the `RUN` macro to run a task.
- If you use `RUN` from a Lace task, this will automatically fallback to `CALL` with a little bit of overhead.
- CAREFUL: using `RUN` from an external thread is not safe w.r.t. garbage collection! If you invoke Sylvan operations from multiple external threads, then it may be possible that BDDs are not referenced (and put back into the table) in rare cases. The solution to this is to wrap each operation in a Lace task that stores the result in a properly referenced variable before leaving the Lace environment. The reason for this is that garbage collection will not run until all Lace workers cooperate.
- You can now run Sylvan from any thread. This means that in the future, it is easier to support bridges to Python and Java.


## [1.5.0] - 2019-09-05
### Added
- New tools `ldd2meddly` and `medmc` as part of research comparing the saturation algorithm in Sylvan to the one in Meddly.

### Changed
- Renamed the `mc` example program to `bccmc`.

### Fixed
- A bug in the GMP handling was fixed.


## [1.4.1] - 2018-06-14
### Changed
- We now implement twisted tabulation as the hash function for the nodes table. The old hash function is still available and the default behavior can be changed in `sylvan_table.h`.

### Removed
- Removed dependency on hwloc.


## [1.4.0] - 2017-07-12
### Added
- Function `mtbdd_cmpl` that computes the complement for MTBDDs. (0 becomes 1, non-0 becomes 0)

### Changed
- Changed file formats used by the examples to match the changes in LTSmin.
- Function `mtbdd_satcount` now does not count assignments leading to 0. Perhaps in the future we make this configurable. (Like in CuDD.)
- Slightly improved C++ support by wrapping header files in the namespace sylvan.

### Fixed
- There was a bug where Lace tasks are overwritten during SYNC, which causes problems during garbage collection. Lace reusing the bucket during SYNC is by design and is difficult to change. We fix the issue by checking during garbage collection if the stored task is still the same function, which in the worst case marks more nodes than needed.
- Band-aid patch for hashing; very similar nodes were hashing to similar positions and strides, causing early garbage collections and full tables. The patch works for now, but we need a more robust solution.

### Removed
- Removed support for HWLOC (pinning on NUMA machines). Planning to bring this back as an option, but in its current form it prevents multiple Sylvan programs from running simultaneously on the same machine.


## [1.3.3] - 2017-06-03
### Changed
- Changed file format for .bdd files in the MC example.

### Fixed
- A major bug in `lddmc_match_sat_par` has been fixed.
- A bug in the saturation algorithm in the model checking example has been fixed.
- A major bug in the hash table rehashing implementation has been fixed.


## [1.3.2] - 2017-05-23
### Added
- Now implements `lddmc_protect` and `lddmc_unprotect` for external pointer references.
- Now implements `lddmc_refs_pushptr` and `lddmc_refs_popptr` for internal pointer references

### Changed
- New version of Lace has slightly different API for manually created threads.


## [1.3.1] - 2017-05-22
### Fixed
- A bug in `mtbdd_refs_ptrs_up` caused a segfault. This has been fixed.


## [1.3.0] - 2017-05-16
### Added
- The embedded work-stealing framework now explicitly checks for stack overflows and aborts with an appropriate error message written to stderr.
- New functions `sylvan_project` and `sylvan_and_project` for BDDs, a dual of existential quantification, where instead of the variables to remove, the given set of variables are the variables to keep.
- New functions `mtbdd_refs_pushptr` and `mtbdd_refs_popptr` allow thread-specific referencing of pointers.

### Changed
- Rewritten initialization of Sylvan. Before the call to `sylvan_init_package`, table sizes must be initialized either using `sylvan_set_sizes` or with the new function `sylvan_set_limits`. This new function allows the user to set a maximum number of bytes allocated for the nodes table and for the operation cache.
- Rewritten MTBDD referencing system.
- Rewritten MTBDD map and set functions (no API change except renaming `mtbdd_map_addall` to `mtbdd_map_update` with backward compatibility)
- The lock-free unique table now uses double hashing instead of rehashing. This can improve the performance for custom leaves and improves the hash spread.

### Fixed
- A bug in `llmsset_lookup` affecting custom leaves has been fixed.


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
