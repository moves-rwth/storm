[Storm Website](https://www.stormchecker.org)
============================================

> :warning: Only push changes to the **`website/src`** branch. Any edits in **`website/build`** will be overwritten.

# How it works
On every push to `website/src`, the website is automatically build using [jekyll](https://jekyllrb.com) and GitHub Actions. The generated files are then automatically pushed to `website/build`.
Note that the whole deployment process takes a few minutes.

# Contributing
The easiest way to suggest changes and additions to the website is to fork this repository and create a pull request to `website/src`.
Of course, you can also write us an email :email: `support at stormchecker.org`


# Building the website locally
If you want to check your local changes before making them public, you have to install [jekyll and its prerequisites](https://jekyllrb.com/docs/installation/).
Make sure to be in the `storm` directory with the `website/src` branch checked out.
You can then use the provided Gemfile in the directory via:
```console
bundle install
```

To build the website (and automatically rebuild after each modification) run the following:
```console
bundle exec jekyll serve --livereload
```
If there are no errors reported, you can browse to [http://localhost:4000](http://localhost:4000) to open the website.
