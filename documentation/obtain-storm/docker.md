---
title: Use Docker Container
layout: default
documentation: true
category_weight: 3
categories: [Obtain Storm]
---

# Use a Docker Container

For easy and fast access to Storm, we provide Docker containers containing Storm in different versions. The Docker containers are similar to the [Virtual machine](vm.html) but come with less overhead and offer more recent versions of Storm.

## Install Docker
To use the containers you first have to install [Docker](https://docs.docker.com/install/){:target="_blank"}.
On macOS you can use [homebrew](https://brew.sh/){:target="_blank"} to install Docker.

```console
$ brew cask install docker
```

Next you should start the Docker app and its tray icon should be visible.

## Download Docker Image
Then you have to download the Docker image you want to use. All available images can be found on [DockerHub](https://hub.docker.com/r/movesrwth/storm/tags/){:target="_blank"}. Currently we offer the latest release and the most recent development versions of Storm. The most recent versions are built automatically each day and indicated by the suffix `travis`. Furthermore we also provide debug builds indicated by the suffix `-debug`.

Download the Storm container you want to use:

```console
$ docker pull movesrwth/storm:travis
```

## Run the Docker Image
We want to be able to share files between the container and the host system. Therefore you should change the directory to the one you want to share, for example:

```console
$ cd ~/Desktop/data
```

The next command starts the previously downloaded image and enables the file sharing with the current directory:

```console
$ docker run --mount type=bind,source="$(pwd)",target=/data -w /opt/storm/build/bin --rm -it --name storm movesrwth/storm:travis
```

After executing the command you are now within the Docker container indicated by a different prompt:

```console
root@1234xyz:/opt/storm/build/bin#
```

The file sharing directory is located at `/data`.
Now you can start using Storm within this container:

```console
$ ./storm --version
```

To try out file sharing execute:

```console
$ ./storm --prism ../../resources/examples/testfiles/dtmc/die.pm --io:exportexplicit /data/die.drn
```

Afterwards there should be a file named `die.drn` in the shared directory now.

In the end exit the container. A clean-up is performed automatically.

```console
$ exit
```
