---
title: Use Docker Container
layout: default
documentation: true
category_weight: 3
categories: [Obtain Storm]
---

<h1>Use a Docker Container</h1>

{% include includes/toc.html %}

For easy and fast access to Storm, we provide Docker containers containing Storm in different versions. The Docker containers are similar to the [Virtual machine](vm.html) but come with less overhead and offer more recent versions of Storm.

## Install Docker
To use the containers you first have to install [Docker](https://docs.docker.com/install/){:target="_blank"}.
On macOS you can use [homebrew](https://brew.sh/){:target="_blank"} to install Docker.

```console
$ brew cask install docker
```

Next you should start the Docker app and its tray icon should be visible.
If you are running Windows, you may need to reboot your machine for the Docker installation to be completed and available using the command line.

## Download Docker Image
Then you have to download the Docker image you want to use. All available images can be found on [DockerHub](https://hub.docker.com/r/movesrwth/storm/tags/){:target="_blank"}. Currently we offer the latest release and the most recent development versions of Storm. The most recent versions are built automatically each day and indicated by the suffix `travis`. Furthermore we also provide debug builds indicated by the suffix `-debug`.

Download the Storm container you want to use:

```console
$ docker pull movesrwth/storm:travis
```

## Run the Docker Image (Linux and macOS)
We want to be able to share files between the container and the host system. Therefore you should change the directory to the one you want to share, for example:

```console
$ cd ~/Desktop/data
```

The next command starts the previously downloaded image and enables file sharing with the current directory:

```console
$ docker run --mount type=bind,source="$(pwd)",target=/data -w /opt/storm/build/bin --rm -it --name storm movesrwth/storm:travis
```

After executing the command you are now within the Docker container indicated by a different prompt:

```console
root@1234xyz:/opt/storm/build/bin#
```

The file sharing directory is located at `/data` for the example directories above.
You can now continue with [testing the container](#testing-the-container).

## Run the Docker Image (Windows)
We want to be able to share files between the container and the host system. For this we need to configure Docker Desktop to enable drive sharing on a drive you want to enable it on.

We will refer to the host directory you want to direct output to as `%hostdir%` and to the virtual directory of the container as `%sharedir%`.
Any file written to `%sharedir%` within the Storm container will later be located at `%hostdir%` on your host machine.
For this demo we use the following directories, which you can export for testing:

```console
set hostdir=%HOMEDRIVE%%HOMEPATH%\Desktop\data
set sharedir=/data
```

First create the directory on the host machine. Then change the current directory to it.

```console
%HOMEDRIVE%
mkdir %hostdir%
cd %hostdir%
```

Head over to the Docker settings panel by right-clicking the Docker icon in your system tray and selecting _Settings..._.
In the _Shared Drives_ option tick the drive letters you want to make Storm available (usually `C`, as in this example), then hit _Apply_.

The next command starts the previously downloaded image and enables file sharing with the earlier set directory:

```console
> docker run --mount type=bind,source=%hostdir%,target=%sharedir% -w /opt/storm/build/bin --rm -it --name storm movesrwth/storm:travis
```

After executing the command you are now within the Docker container indicated by a different prompt:

```console
root@1234xyz:/opt/storm/build/bin#
```

The file sharing directory is located at `/data` for the example directories above.
You can now continue with [testing the container](#testing-the-container).

## Testing the Container
After running the Docker Image according to the commands above, you can start using Storm within this container:

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
