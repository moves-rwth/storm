# Developer information

The following contains some general guidelines for developers.


## Structure
- Storm consists of the core library `lib/libstorm` resulting from the source code in `src/storm`.
- Several additional libraries `lib/libstorm-xyz` provide additional features (parametric models, POMPD, DFT, etc.) and are built from the corresponding code in `src/storm-xyz`.
- For each library, a corresponding binary `/bin/storm-xyz` is built from the source in `src/storm-xyz-cli`.
- Functionality is accompanied by tests (`src/test`) whenever possible.
  The complete test suite can be executed by `make test` and individual tests can be executed via the corresponding binaries `bin/test-xyz`.
- Storm is heavily templated.
  In particular, it features the template argument `ValueType` representing the underlying number type.
  The most commonly used types are `double`, `storm::RationalNumber` and `storm::RationalFunction`.


## Coding conventions
- Code should be formatted according to the given rules set by clang-format.
  Proper formatting can be ensured by executing `make format`.
  For more information see [PR#175](https://github.com/moves-rwth/storm/pull/175).
- We use Doxygen for documentation, see [storm-doc](https://moves-rwth.github.io/storm-doc/).
  Code blocks should be documented with:
  ```
  /*!
   * ... 
   * @param ... 
   * @return ... 
   */
  ```
- We provide custom macros for output and logging.
  The use of `std::cout` should be avoided and instead, macros such as `STORM_LOG_DEBUG`, `STORM_LOG_INFO` or `STORM_PRINT_AND_LOG` should be used.
- For line breaks, we use `'\n'` instead of `std::endl` to avoid unnecessary flushing.
  See [PR 178](https://github.com/moves-rwth/storm/pull/178) for details.


## Contributing
- Check that all tests run successfully: `make test`.
- Check that the code is properly formatted: `make format`.
  There is also a CI job which can provide automated code formatting.
- New code should be submitted by opening a [pull request](https://github.com/moves-rwth/storm/pulls).
  Our continuous integration automatically checks that the code in the PR is properly formatted and all tests run successfully. 
