# Base Dockerfile for using Storm

# Copy this Dockerfile into the local directory containing the content which should be available within the Docker image
# and adapt the Dockerfile
# The Docker image can be built by executing:
#docker build -f Dockerfile -t custom_docker_repo/image_name:custom_tag .

# Ideally one can just use one of the base images of Storm
# by using one of the following lines:
#FROM movesrwth/storm:ci-release
#FROM movesrwth/storm:1.6.4

# If a default image cannot be used, one has to manually build Storm as follows
FROM movesrwth/carl:master14
MAINTAINER Matthias Volk <m.volk@utwente.nl>

# Build Storm
#############
WORKDIR /opt
# Obtain Storm from some repository
RUN git clone https://github.com/moves-rwth/storm.git
RUN mkdir -p /opt/storm/build
WORKDIR /opt/storm/build
# If needed, checkout a specific branch
#RUN git checkout specific_branch
# Configure Storm
RUN cmake .. "-DCMAKE_BUILD_TYPE=Release -DSTORM_DEVELOPER=OFF -DSTORM_LOG_DISABLE_DEBUG=ON -DSTORM_PORTABLE=ON"
# Build binaries of Storm using a single thread
RUN make binaries -j 1
ENV PATH="/opt/storm/build/bin:$PATH"


# Build additional libraries/script/etc.
########################################
RUN mkdir /opt/custom_dir
WORKDIR /opt/custom_dir
# Copy the content of the current local directory into the Docker image
COPY . .
# Perform the necessary build steps
#RUN xyz...
