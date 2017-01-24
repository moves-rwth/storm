---
title: Configuration Guide
layout: default
documentation: true
categories: [Installation]
---

For advanced users and people developping under storm, we recommend to install some dependencies by hand, and configuring storm for improved usability.
This document collects these steps.

# TOC
 {:.no_toc}
- list
{:toc}

# Install dependencies by hand

## Carl

Obtain [carl](https://github.com/smtrat/carl): 

```shell
git clone https://github.com/smtrat/carl
cd carl
mkdir build
cd build
cmake -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON ..
```


## Boost

# CMake Options

## Debug Flags

