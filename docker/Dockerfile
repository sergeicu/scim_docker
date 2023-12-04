FROM sergeicu/scim_base:latest as scim_base

LABEL maintainer="serge.vasylechko@childrens.harvard.edu"
LABEL vendor="QUIN"


###############
# Add UPDATED scim code
###############

# add scim code 
RUN mkdir -p /scim
COPY ./scim /scim


###############
# Compile scim
###############

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
RUN rm -rf /scim/build && mkdir -p /scim/build && cd /scim/build && \
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
COPY --from=scim_base /scim/bin /scim/bin
COPY --from=scim_base /opt/x86_64/pkgs/vtk/current/ /opt/vtk
COPY --from=scim_base /opt/x86_64/pkgs/itk/3.20.0/gcc-release/ /opt/itk 


# Publisher info
ARG BUILD_DATE
ARG VERSION
LABEL org.label-schema.build-date=$BUILD_DATE \
      org.label-schema.name="SCIM" \
      org.label-schema.description="SCIM" \
      org.label-schema.url="http://quin.med.harvard.edu" \
      org.label-schema.version=$VERSION \
      org.label-schema.schema-version="1.0"


# Set VTK and ITK dependency paths 
ENV VTK_INSTALL_DIR=/opt/vtk
ENV ITK_INSTALL_DIR=/opt/itk
ENV LD_LIBRARY_PATH=${VTK_INSTALL_DIR}/lib:${ITK_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}

# Add binaries to path 
ENV PATH=/scim/bin:${PATH}

# User instructions
ENV msg="\nList of available binaries in /scim/bin\n"
CMD echo $msg; find /scim/bin/ -type f ; echo $msg

# set work dir 
WORKDIR /data

