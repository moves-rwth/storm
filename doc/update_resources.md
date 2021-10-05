# Update shipped third party resources

## Eigen

In Eigen, we have adapted `SparseLU` to work with scalar types that do not default construct from a double (like CLN numbers) or that do not have an operator< or std::abs

To update the Eigen version, just change the corresponding commit hash in `$STORM_DIR/resources/3rdparty/CmakeLists.txt`.
Check whether the patch located at `$STORM_DIR/resources/3rdparty/patches/eigen.patch` can be applied without issues (in particular check for changes in `Eigen/src/SparseLU/`)

In case a new patch needs to be created follow these steps:

1. Clone `https://gitlab.com/libeigen/eigen.git` to `$STORM_DIR/resources/3rdparty/` and checkout the corresponding commit
2. Checkout a new branch
3. Apply the old patch via `git apply $STORM_DIR/resources/3rdparty/patches/eigen.patch`
4. Resolve issues, make changes, and commit them
5. Create a new patch file via `git format-patch <tag> --stdout > eigen.patch`, where `<tag>` is the tag, branch or commit from step 1

## googletest / gtest

To update gtest, simply download the new sources from [here](https://github.com/google/googletest/releases) and put them to `$STORM_DIR/resources/3rdparty/googletest`.

The currently shipped version can be shown using

```console
grep GOOGLETEST_VERSION $STORM_DIR/resources/3rdparty/googletest/CMakeLists.txt
```

We add some extra code to gtest located in `$STORM_DIR/src/test/storm_gtest.h`. Note that our code might not be compatible with future versions of gtest.

## Spot

To update (shipped version of Spot), just change the url in `$STORM_DIR/resources/3rdparty/include_spot.cmake`.