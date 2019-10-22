---
title: Getting Started
navigation_weight: 2
layout: default
---

## Obtaining Storm

To be able to run Storm, you need to obtain it and run it on your system.
Currently, you can choose whether you want to [install](documentation/installation/installation.html) it, try an up-to-date [Docker image](documentation/installation/installation.html#docker) or in a [virtual machine](documentation/vm/vm.html) image we provide.

## Model Types

After you have obtained Storm, you need to make sure that your input model has the right form. That is, on a fundamental level, you need to ensure that your input model falls into one of the [model types](documentation/usage/models.html) supported by Storm.

## Input Languages

If your model indeed does, then the next thing is to have the model available in an [input language](documentation/usage/languages.html) of Storm. If you don't have such a model yet, you need to first model the system you are interested in or transcribe it from a different input format.

## Properties

However, the input model is only "half" of the input you need to provide, the other "half" being the property you want to verify. Please consult our [guide to properties](documentation/usage/properties.html) for details on how to specify them.

## Running Storm

Finally, if both the input model as well as the property are captured in an appropriate format, then you are ready to run Storm! Our [guide](documentation/usage/running-storm.html) illustrates how you can do so. Since the calls (and even the binaries) you need to invoke depend on the input language for each of the input languages, the guide shows how to run Storm depending on the input you have.
