# Base Dockerfile for using Storm
#################################
# The Docker image can be built by executing:
# docker build -t yourusername/storm .

FROM movesrwth/storm-basesystem:latest
MAINTAINER Matthias Volk <m.volk@utwente.nl>

# Specify number of threads to use for parallel compilation
# This number can be set from the commandline with:
# --build-arg no_threads=<value>
ARG no_threads=1


# Build Carl
############
# Explicitly build the Carl library
# This is needed when using pycarl/stormpy later on
WORKDIR /opt/

# Obtain Carl from public repository
RUN git clone https://github.com/moves-rwth/carl-storm.git carl

# Switch to build directory
RUN mkdir -p /opt/carl/build
WORKDIR /opt/carl/build

# Configure Carl
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON -DTHREAD_SAFE=ON

# Build Carl library
RUN make lib_carl -j $no_threads



# Build Storm
#############
RUN mkdir /opt/storm
WORKDIR /opt/storm

# Copy the content of the current local Storm repository into the Docker image
COPY . .

# Switch to build directory
RUN mkdir -p /opt/storm/build
WORKDIR /opt/storm/build

# Configure Storm
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DSTORM_DEVELOPER=OFF -DSTORM_LOG_DISABLE_DEBUG=ON -DSTORM_PORTABLE=ON -DSTORM_USE_SPOT_SHIPPED=ON

# Build external dependencies of Storm
RUN make resources -j $no_threads

# Build Storm binary
RUN make storm -j $no_threads

# Build additional binaries of Storm
# (This can be skipped or adapted dependending on custom needs)
RUN make binaries -j $no_threads

# Set path
ENV PATH="/opt/storm/build/bin:$PATH"
