---
title: Manual Configuration
layout: default
documentation: true
category_weight: 3
categories: [Installation]
---

{::options parse_block_html="true" /}
<div class="panel panel-default">
<div class="panel-heading">
### Table of contents
{:.no_toc}
- list
{:toc}
</div>
</div>


Designed for users that need particular features and people developing under Storm, this guide will detail how perform a manual configuration of the build process.

# Manually installing dependencies

## Carl
{:.no_toc}

Storm makes use of [CArL](https://github.com/smtrat/carl)

```shell
git clone https://github.com/smtrat/carl
cd carl
mkdir build
cd build
cmake -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON ..
```


## Boost
{:.no_toc}

# CMake Options

## Debug Flags
{:.no_toc}

