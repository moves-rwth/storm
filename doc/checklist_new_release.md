The following steps should be performed before releasing a new Storm version.
Note that in most cases a simultaneous release of [carl-storm](https://github.com/moves-rwth/carl-storm), [Storm](https://github.com/moves-rwth/storm) and [stormpy](https://github.com/moves-rwth/stormpy/) is preferred.

1. Update `CHANGELOG.md`:
   * Set release month
   * Set major changes.
     Use the [automatically generated release notes](https://docs.github.com/en/repositories/releasing-projects-on-github/automatically-generated-release-notes).
     Alternatively, get all the commits since the `last_tag` by executing:
     ```console
     git log last_tag..HEAD
     ```

2. Update used carl version:
   This should be automatically handled by a CI workflow triggered by new releases in carl-storm.
   To manually update the carl version, update the following locations:
   * `STORM_CARL_GIT_TAG` in `CMakeLists.txt`
   * `carl_tag` in `Dockerfile`, `.github/workflows/Dockerfile.archlinux` and `.github/workflows/Dockerfile.release`
   * Maybe update `CARL_MINVERSION` in `resources/3rdparty/CMakeLists.txt`

3. Update the versions of the dependencies in the [storm-dependencies Dockerfile](https://github.com/moves-rwth/docker-storm/blob/main/storm-dependencies/Dockerfile).
   Use the [CI of docker-storm](https://github.com/moves-rwth/docker-storm/actions/workflows/dependencies.yml) to create new Docker images.

4. Check that Storm [CI](https://github.com/moves-rwth/storm/actions/) builds without errors and all tests are successful:

5. Set new Storm version in `CMakeList.cmake`.

6. (The tag can also automatically be set in the next step when creating the release on Github.)
   Set the new tag in Git, use the flag `-s` to sign the tag.
   ```console
   git tag -a X.Y.Z -m "Storm version X.Y.Z" -s
   git push origin X.Y.Z
   ```
   The new tag should now be visible on [GitHub](https://github.com/moves-rwth/storm/tags).

7. [Add new release](https://github.com/moves-rwth/storm/releases/new) on GitHub.
   Create a new tag or use the tag created in the previous step.
   Finishing the release automatically triggers a CI workflow which also
   * updates the `stable` branch
   * creates new Docker containers for both the tag and `stable` branch
   * triggers a PR in stormpy to update the Storm version

8. Update [Homebrew formula](https://github.com/moves-rwth/homebrew-storm).

9. Announce new storm version on [website](http://www.stormchecker.org/news.html).
