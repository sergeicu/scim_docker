FROM centos:centos6.10 AS build

LABEL maintainer="serge.vasylechko@childrens.harvard.edu"
LABEL vendor="QUIN"

# add yum repos and update to correct paths  - https://www.getpagespeed.com/server-setup/how-to-fix-yum-after-centos-6-went-eol
RUN curl https://www.getpagespeed.com/files/centos6-eol.repo --output /etc/yum.repos.d/CentOS-Base.repo && \
    curl https://www.getpagespeed.com/files/centos6-epel-eol.repo --output /etc/yum.repos.d/epel.repo && \
    curl https://www.getpagespeed.com/files/centos6-scl-eol.repo --output /etc/yum.repos.d/CentOS-SCLo-scl.repo && \
    curl https://www.getpagespeed.com/files/centos6-scl-rh-eol.repo --output /etc/yum.repos.d/CentOS-SCLo-scl-rh.repo && \
    sed -i 's%^mirrorlist%#mirrorlist%g' /etc/yum.repos.d/* && \
    sed -i 's%#baseurl=http://mirror.centos.org%baseurl=https://vault.centos.org%g' /etc/yum.repos.d/* && \
    sed -i 's,http://vault.centos.org,https://vault.centos.org,g' /etc/yum.repos.d/* 

############################
# Install yum dependencies 
############################

# update and install basic build tools 
RUN yum update -y yum-utils \
                    epel-release \
                    wget \
                    git \
                    unzip && \
                    yum install -y --nogpgcheck python34 python34-devel && \
                    yum makecache fast && yum clean all 


###############
# Copy dependencies as pre-compiled binaries: vtk, nlopt, tclap 
###############

# Why not build from source? 
    # Following multiple attempts to build a working vtk from source on centos6 (nearly extinct), amongst others, 
    # we felt it may be faster / easier to reuse precompiled binaries directly. 
    # Remember that our end goal was to compile SCIM from source, and not its dependencies. 
    # All of these binaries are available inside of this docker image, as well as in the relevant github repo 
    # In the worst case scenario - you can build them from source. See versions of each dependency below

# add vtk and itk 
RUN mkdir -p /opt/x86_64/pkgs/itk/4.3.1/ /opt/x86_64/pkgs/vtk/5.8.0
COPY ./libs/itk/3.20.0 /opt/x86_64/pkgs/itk/3.20.0
COPY ./libs/vtk/6.1.0 /opt/x86_64/pkgs/vtk/current


# add nlopt, tclap, gsl
RUN mkdir -p /home/ch155417/moti/gsl/ /home/ch169807/moti/itkNLOPTOptimizers/ /home/ch155417/moti/nlopt-2.2/ /home/ch169807/Software/tclap-1.2.1 
COPY ./libs/nlopt-2.2 /home/ch155417/moti/nlopt-2.2
COPY ./libs/itkNLOPTOptimizers /home/ch169807/moti/itkNLOPTOptimizers
COPY ./libs/tclap-1.2.1 /home/ch169807/Software/tclap-1.2.1
COPY ./libs/gsl /home/ch155417/moti/gsl

###############
# Add scim code
###############

# add scim code 
RUN mkdir -p /scim
COPY ./scim_cpp/dwi_bootstrap /scim


###############
# Compile scim
###############

# fake flag - change it to force docker to recompile from here (as long as you are not running --no-cache during build)
ARG REBUILD=change_this_string_to_force_rebuild_from_this_line

# Set dependency paths 
ENV GSL=/home/ch155417/moti/gsl/build/lib/libgsl.a
ENV GSLCBLAS=/home/ch155417/moti/gsl/build/lib/libgslcblas.a
ENV GSL_INCLUDE_DIR=/home/ch155417/moti/gsl/build/include
ENV ITKNLOPTOPTIMIZERS_INCLUDE_DIR=/home/ch169807/moti/itkNLOPTOptimizers/src
ENV NLOPT=/home/ch155417/moti/nlopt-2.2/bin/lib/libnlopt.a
ENV NLOPT_DIR=/home/ch155417/moti/nlopt-2.2
ENV TCLAP_INCLUDE_DIR=/home/ch169807/Software/tclap-1.2.1/include
ENV itkNLOPTOptimizersLib=/home/ch169807/moti/itkNLOPTOptimizers/build/libITKNLOPTOptimizers.a
ENV VTK_DIR=/opt/x86_64/pkgs/vtk/current/lib/cmake/vtk-6.1                                                                                             
ENV ITK_DIR=/opt/x86_64/pkgs/itk/3.20.0/gcc-release/lib/InsightToolkit/

# Compile 
RUN mkdir -p /scim/build && cd /scim/build && \
    cmake -DGSL=${GSL} -DGSL_INCLUDE_DIR=${GSL} \
    -DGSLCBLAS=${GSLCBLAS} \
    -DGSL_INCLUDE_DIR=${GSL_INCLUDE_DIR} \
    -DITKNLOPTOPTIMIZERS_INCLUDE_DIR=${ITKNLOPTOPTIMIZERS_INCLUDE_DIR} \
    -DITK_DIR=${ITK_DIR} \
    -DNLOPT=${NLOPT} \
    -DNLOPT_DIR:PATH="${NLOPT_DIR}" \
    -DTCLAP_INCLUDE_DIR:PATH=$TCLAP_INCLUDE_DIR \
    -DitkNLOPTOptimizersLib:PATH=$itkNLOPTOptimizersLib \
    -DVTK_DIR:PATH="${VTK_DIR}" .. && \
    make -j 4 && make install 

###############
# Compress the image
###############

# Summary: we copy only the compiled scim binaries, and environment dependencies into the final image

FROM centos:centos6.10 as scim
COPY --from=build /scim/bin /scim/bin
COPY --from=build /opt/x86_64/pkgs/vtk/current/ /opt/vtk
COPY --from=build /opt/x86_64/pkgs/itk/3.20.0/gcc-release/ /opt/itk 


# Publisher info
ARG BUILD_DATE
ARG VERSION
LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name="SCIM" \
      org.label-schema.description="CRKit - tools for computational radiology" \
      org.label-schema.url="http://quin.med.harvard.edu" \
      org.label-schema.version=$VERSION \
      org.label-schema.schema-version="1.0"


# Set VTK and ITK dependency paths 
ENV VTK_INSTALL_DIR=/opt/vtk
ENV ITK_INSTALL_DIR=/opt/itk
ENV LD_LIBRARY_PATH=${VTK_INSTALL_DIR}/lib:${ITK_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}

#docker run --rm -e LD_LIBRARY_PATH=<CORRECT_PATH> docker_image <command_input_args>

#docker run --rm -it docker_image /bin/bash 
#$ LD_LIBRARY_PATH=<CORRECT_PATH>
#$ command_rags...


# Add binaries to path 
ENV PATH=/scim/bin:${PATH}

# User instructions
ENV msg="\nList of available binaries in /scim/bin\n"
CMD echo $msg; find /scim/bin/ -type f ; echo $msg

# set work dir 
WORKDIR /data
