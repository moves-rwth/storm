The following steps should be performed before releasing a new storm version.
Note that in most case a simultaneous release of [carl](https://github.com/smtrat/carl), [storm](https://github.com/moves-rwth/storm), [pycarl](https://github.com/moves-rwth/pycarl/) and [stormpy](https://github.com/moves-rwth/stormpy/) is preferred.

1. Update `CHANGELOG.md`
   To get all the commits from an author since the last tag execute:
   ```console
   git log last_tag..HEAD --author "author_name"
   ```

2. Update used carl version:
  * Update `GIT_TAG` in `resources/3rdparty/carl/CMakeLists.txt`
  * Maybe update `CARL_MINVERSION` in `resources/3rdparty/CMakeLists.txt`

3. Check that storm builds without errors and all tests are successful
   * [Travis](https://travis-ci.org/moves-rwth/storm) should run successfully

4. Set new storm version:
   * Set new storm version in `version.cmake`

5. Set new tag in git
   ```console
   git tag -a new_version
   git push origin new_version
   ```
   Next we push the tag to GitHub. This step requires the GitHub repo to to be configured as a remote.
   ```console
   git remote add github https://github.com/moves-rwth/storm.git
   git push github new_version
   ```
   The new tag should now be visible on [GitHub](https://github.com/moves-rwth/storm/tags)

6. [Add new release](https://github.com/moves-rwth/storm/releases/new) in GitHub

7. Update [Homebrew formula](https://github.com/moves-rwth/homebrew-storm)

8. Announce new storm version on [website](http://www.stormchecker.org/news.html)
