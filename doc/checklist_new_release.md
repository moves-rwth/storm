The following steps should be performed before releasing a new storm version.
Note that in most cases a simultaneous release of [carl](https://github.com/moves-rwth/carl-storm), [storm](https://github.com/moves-rwth/storm), [pycarl](https://github.com/moves-rwth/pycarl/) and [stormpy](https://github.com/moves-rwth/stormpy/) is preferred.

1. Update `CHANGELOG.md`:
   * To get all the commits from an author since the last tag execute:
   ```console
   git log last_tag..HEAD --author "author_name"
   ```
   * Set release month

2. Update used carl version:
   * Update `STORM_CARL_GIT_TAG` in `CMakeLists.txt`
   * Update `carl_tag` in `Dockerfile` and `.github/workflows/Dockerfile.archlinux`
   * Maybe update `CARL_MINVERSION` in `resources/3rdparty/CMakeLists.txt`

3. Check that storm builds without errors and all tests are successful:
   * [Github Actions](https://github.com/moves-rwth/storm/actions/) should run successfully.

4. Set new storm version:
   * Set new storm version in the root `CMakeList.cmake`

5. Set new tag in Git (assuming that the new version is X.Y.Z and that the remote "origin" is the github repo):
   ```console
   git tag -a X.Y.Z -m "Storm version X.Y.Z"
   git push origin X.Y.Z
   ```
   The new tag should now be visible on [GitHub](https://github.com/moves-rwth/storm/tags).

6. Use the [CI](https://github.com/moves-rwth/storm/actions/workflows/release_docker.yml) on the tag, provide the version `X.Y.Z` as tag and automatically create the [Docker containers](https://hub.docker.com/r/movesrwth/storm) for the new version.

7. [Add new release](https://github.com/moves-rwth/storm/releases/new) in GitHub.

8. Update `stable` branch:

   ```console
   git checkout stable
   git rebase master
   git push origin stable
   ```
   Note: Rebasing might fail if `stable` is ahead of `master` (e.g. because of merge commits). In this case we can do:
    ```console
   git checkout stable
   git reset --hard master
   git push --force origin stable
   ```

9. Use the [CI](https://github.com/moves-rwth/storm/actions/workflows/release_docker.yml) on the `stable` branch, provide the tag 'stable' and automatically create the [Docker containers](https://hub.docker.com/r/movesrwth/storm).

10. Update [Homebrew formula](https://github.com/moves-rwth/homebrew-storm).

11. Announce new storm version on [website](http://www.stormchecker.org/news.html).
