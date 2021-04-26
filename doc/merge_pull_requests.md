The following steps should be performed to integrate pull requests from Github.

0. After a pull request is opened, some automatic build checks should be performed by Github Actions.
   Failures of these checks should be fixed.

1. Manually review the pull request on Github and suggest improvements if necessary.
   In particular make sure:
   * No unnecessary files were committed (for example build artefacts, etc.)
   * No remains from the development are present (for example debug output, hackish workarounds, etc.)
   * ...

2. Integrate the pull request via Github, preferably by *rebase and merge*.

3. Optional (if not done already): add the Github repository as another remote for your local copy of the internal repository:
   ```console
   git remote add github https://github.com/moves-rwth/storm.git
   ```
   
4. Fetch the current Github master:
   ```console
   git fetch github
   ```

5. Make sure to be on the (internal) master:
   ```console
   git checkout master
   ```

6. Rebase the changes of Github onto the (internal) master:
   ```console
   git rebase github/master
   ```

7. Check that Storm builds successfully and everything works as expected.

8. Push the changes into the internal repository:
   ```console
   git push origin
   ```
