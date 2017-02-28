---
title: Manual Configuration
layout: default
documentation: true
category_weight: 3
categories: [Installation]
---

For advanced users and people developing under Storm, we recommend to install some dependencies by hand, and configuring Storm for improved usability.
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

