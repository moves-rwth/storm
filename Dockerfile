# Base Dockerfile for using Storm
#################################
# The Docker image can be built by executing:
# docker build -t yourusername/storm .
# A different base image can be set from the commandline with:
# --build-arg BASE_IMG=<new_base_image>

# Set base image
ARG BASE_IMG=movesrwth/storm-dependencies:latest
ARG BASE_PLATFORM=linux/amd64
FROM --platform=$BASE_PLATFORM  $BASE_IMG
MAINTAINER Matthias Volk <m.volk@tue.nl>

# Specify configurations
# These configurations can be set from the commandline with:
# --build-arg <config_name>=<value>
# CMake build type
ARG build_type=Release
# Specify number of threads to use for parallel compilation
ARG no_threads=1

# Specify Storm configuration (ON/OFF)
ARG gurobi_support="ON"
ARG soplex_support="ON"
ARG spot_support="ON"
ARG developer="OFF"
ARG cln_exact="OFF"
ARG cln_ratfunc="ON"
ARG all_sanitizers="OFF"

# Specify additional CMake arguments for Storm
ARG cmake_args=""


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
RUN cmake .. -DCMAKE_BUILD_TYPE=$build_type \
             -DSTORM_PORTABLE=ON \
             -DSTORM_USE_GUROBI=$gurobi_support \
             -DSTORM_USE_SOPLEX=$soplex_support \
             -DSTORM_USE_SPOT_SYSTEM=$spot_support \
             -DSTORM_DEVELOPER=$developer \
             -DSTORM_USE_CLN_EA=$cln_exact \
             -DSTORM_USE_CLN_RF=$cln_ratfunc \
             $cmake_args

# Build external dependencies of Storm
RUN make resources -j $no_threads

# Build Storm binary
RUN make storm -j $no_threads

# Build additional binaries of Storm
# (This can be skipped or adapted depending on custom needs)
RUN make binaries -j $no_threads

# Set path
ENV PATH="/opt/storm/build/bin:$PATH"
