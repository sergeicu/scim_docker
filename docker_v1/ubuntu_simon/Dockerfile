FROM ubuntu:20.04 AS build
ENV DEBIAN_FRONTEND="noninteractive"


# A word about HTTP_PROXY
# On systems that need to access a proxy to download packages, the build
# should be called with a build-arg that passes in the proxy to use.
#  A symptom that this is needed is that apt-get cannot access packages.
# This is not needed if building on a system that does not  use a proxy.
#
# To set the proxy variable from the build environment:
# docker build --build-arg HTTP_PROXY .
#

LABEL maintainer="warfield@crl.med.harvard.edu"
LABEL vendor="Computational Radiology Laboratory"

RUN cd $GITHUB_WORKSPACE && mkdir crkitenv && echo `pwd` && cd crkitenv && echo `pwd`

# Update the ubuntu.
RUN apt-get -y update && \
    apt-get -y upgrade && apt-get install -y apt-utils

# FIX THE MISSING LOCALE in ubuntu
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y locales \
    && sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen \
    && dpkg-reconfigure --frontend=noninteractive locales \
    && update-locale LANG=en_US.UTF-8

ENV LANG=en_US.UTF-8 
ENV LC_ALL=en_US.UTF-8

# Install the prerequisite software
RUN apt-get install -y build-essential \
                       zlib1g-dev \
                       libncurses5-dev \
                       libgdbm-dev \
                       libnss3-dev \
                       libssl-dev \
                       libreadline-dev \
                       libffi-dev \
                       libxcb-xinerama0 \
                       libxcb-xinerama0-dev \
                       wget \
                       git \
                       perl \
                       python3-dev \
                       python3-pip \
                       libproj-dev \
                       gperf bison flex \
                       unzip \
                       cmake \
                       libgoogle-glog-dev libgflags-dev \
                       libatlas-base-dev \
                       libeigen3-dev \
                       libsuitesparse-dev \
                       qt5-default \
                       libtbb-dev \
                       libfreetype6-dev \
                       libfontconfig-dev \
                       libdouble-conversion-dev \
                       liblz4-dev liblzma-dev \
                       libnetcdf-dev libnetcdf-cxx-legacy-dev \
                       libogg-dev \
                       libtheora-dev \
                       libpng-dev \
                       libjpeg-dev \
                       libtiff-dev \
                       libjsoncpp-dev \
                       libexpat1-dev \
                       libglew-dev \
                       libhdf5-dev \
                       libqt5x11extras5-dev \
                       libqt5opengl5-dev \
                       qtbase5-dev \
                       qttools5-dev \
                       libxt-dev libxml2-dev \
                       libsqlite3-dev \
                       libswscale-dev \
    && rm -rf /var/lib/apt/lists/*




