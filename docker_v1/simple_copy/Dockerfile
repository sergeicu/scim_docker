FROM centos6.10_yum_basic as base 

###############
# Local copy of dependencies 
###############

# add dependencies 
RUN mkdir -p /home/ch155417/moti/gsl/ /home/ch169807/moti/itkNLOPTOptimizers/ /home/ch155417/moti/nlopt-2.2/ /home/ch169807/Software/tclap-1.2.1 
COPY ./libs/gsl /home/ch155417/moti/gsl
COPY ./libs/itkNLOPTOptimizers /home/ch169807/moti/itkNLOPTOptimizers
COPY ./libs/nlopt-2.2 /home/ch155417/moti/nlopt-2.2
COPY ./libs/tclap-1.2.1 /home/ch169807/Software/tclap-1.2.1


# add vtk and itk 
RUN mkdir -p /opt/x86_64/pkgs/itk/4.3.1/ /opt/x86_64/pkgs/vtk/5.8.0
COPY ./libs/itk/4.3.1 /opt/x86_64/pkgs/itk/4.3.1
COPY ./libs/vtk/5.8.0 /opt/x86_64/pkgs/vtk/5.8.0


# add scim code 
RUN mkdir -p /scim
COPY ./scim_cpp/dwi_bootstrap /scim


# install cmake 
RUN mkdir -p /tmp/cmake && \
    pushd /tmp/cmake && \
    wget 'https://cmake.org/files/v3.9/cmake-3.9.1-Linux-x86_64.sh' && \
    bash cmake-3.9.1-Linux-x86_64.sh --prefix=/usr/local --exclude-subdir && \
    popd && \
    rm -rf /tmp/cmake

# build scim 
#RUN mkdir -p /scim/build && cd /scim/build && \ 
#    cmake ... <how to build with dependencies>




