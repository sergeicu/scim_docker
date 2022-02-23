# build
cd ~/fs/s20220203_temp_home_down/w/code/docker/scim/s20220204_build_docker_centos_nlopt/docker_v1/simple_copy #centos6_v3
name=centos6_w_vtk_itk_nlopt
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name -f Dockerfile_centos6_dev_w_vtk_itk_nlopt .

name=centos6_crl
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name -f Dockerfile_centos6_dev .

name=centos6_scim
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name -f Dockerfile_centos6_scim .

name=scim:latest
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name -f Dockerfile .

# run 
docker run -e http_proxy=http://proxy.tch.harvard.edu:3128 -it --rm -t $name /bin/bash

# crkit 
ssh juniper
name=crkit
docker run -e http_proxy=http://proxy.tch.harvard.edu:3128 -it --rm -t $name /bin/bash

# build locally 
cd /scim && rm -rf build && mkdir build && cd build && ccmake ..

# cmake dependencies: /fileserver/fastscratch/serge/s20220203_temp_home_down/w/code/ivim/experiments/s20210730-debugging-scim-cpp-and-spim-matlab/scim-cpp/new.png
                                                     Page 1 of 1
 CMAKE_BUILD_TYPE                 Debug                                                                                                                                                                                                                                                  
 CMAKE_INSTALL_PREFIX             ../Release                                                                                                                                                                                                                                             
GSL                              /home/ch155417/moti/gsl/build/lib/libgsl.a                                                          GSLCBLAS                         /home/ch155417/moti/gsl/build/lib/libgslcblas.a                                                      GSL_INCLUDE_DIR                  /home/ch155417/moti/gsl/build/include                                                               
ITKNLOPTOPTIMIZERS_INCLUDE_DIR   /home/ch169807/moti/itkNLOPTOptimizers/src                                                         
ITK_DIR                          /opt/x86_64/pkgs/itk/3.20.0/gcc-release/lib/InsightToolkit/                                       
ITK_DIR                          /opt/x86_64/pkgs/itk/4.3.1/gcc-release/lib/cmake/ITK-4.3/
NLOPT                            /home/ch155417/moti/nlopt-2.2/bin/lib/libnlopt.a                                                  
NLOPT_DIR                        /home/ch155417/moti/nlopt-2.2                                                                     
TCLAP_INCLUDE_DIR                /home/ch169807/Software/tclap-1.2.1/include                                                       
itkNLOPTOptimizersLib            /home/ch169807/moti/itkNLOPTOptimizers/build/libITKNLOPTOptimizers.a 
VTK_DIR 
                                 /opt/x86_64/pkgs/vtk/current/lib/cmake/vtk-6.1                                                                                                                                                                 





mkdir /itkbuildenv && cd /itkbuildenv && \
    git clone -b release-3.20 https://github.com/InsightSoftwareConsortium/ITK.git ITK && \
    cd ITK && mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/opt/itk \
    -DCMAKE_INSTALL_RPATH="${CMAKE_INSTALL_PREFIX}/lib" \
    -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE \
    -DBUILD_EXAMPLES:BOOL=OFF \
    -DBUILD_TESTING:BOOL=OFF \
    -DBUILD_SHARED_LIBS:BOOL=ON \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DCMAKE_CXX_FLAGS_RELEASE:STRING="-O3 -DNDEBUG" \
    .. && \
    make -j 2 && make install 

ENV ITK_INSTALL_DIR=/opt/itk/
#ENV ITK_DIR=${ITK_INSTALL_DIR}/lib/cmake/ITK-3.20.0/
ENV ITK_DIR=${ITK_INSTALL_DIR}/lib/InsightToolkit/



# version v6.1.0
mkdir /vtkbuildenv && cd /vtkbuildenv && \
    git clone https://gitlab.kitware.com/vtk/vtk.git VTK && \
    cd VTK && \
    git checkout 82840f008383c7767c693edb82ddddab1d9f9a51 && \
    mkdir build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/opt/vtk \
      -DCMAKE_BUILD_TYPE:STRING=Release \
      -DBUILD_SHARED_LIBS:BOOL=OFF \
      -DCMAKE_INSTALL_RPATH="${CMAKE_INSTALL_PREFIX}/lib" \
          -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE \
          -DVTK_BUILD_DOCUMENTATION=OFF \
          -DVTK_BUILD_TESTING=OFF \
          -DVTK_BUILD_EXAMPLES=OFF \
          -DBUILD_USER_DEFINED_LIBS:BOOL=OFF \
          -DVTK_LEGACY_REMOVE=ON \
          -DVTK_ANDROID_BUILD=OFF \
          -DVTK_IOS_BUILD=OFF \
          -DVTK_EXTRA_COMPILER_WARNINGS=OFF \
          -DVTK_GROUP_ENABLE_Views=NO \
          -DVTK_GROUP_ENABLE_Web=NO \
          -DVTK_GROUP_ENABLE_Imaging=NO \
          -DVTK_GROUP_ENABLE_Qt=DONT_WANT \
          -DVTK_GROUP_ENABLE_Rendering=DONT_WANT \
          -DVTK_PYTHON_VERSION=3 \
          -DVTK_ENABLE_WRAPPING=ON \
          -DVTK_WRAP_PYTHON=ON \
          -DVTK_WRAP_JAVA=OFF \
          -DVTK_USE_LARGE_DATA=OFF \
          -DCMAKE_C_FLAGS=-DGLX_GLXEXT_LEGACY \
          -DCMAKE_CXX_FLAGS=-DGLX_GLXEXT_LEGACY \
          .. && \
          make -j 4 && make install
          



          
          \
          -DVTK_INCLUDE_DIR=$(python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())") \
          -DVTK_LIBRARY=$(python3 -c "import distutils.sysconfig as sysconfig; print(sysconfig.get_config_var('LIBDIR'))") \
          .. && cd .. && \
    make -j 4 && make install
    
mkdir /nloptbuildenv && cd /nloptbuildenv && \
    git clone https://github.com/stevengj/nlopt nlopt && \
    cd nlopt && \
    #git checkout eaf3ca514e30b5ee8c8f7b97b378099de1c35418 && \
    #git checkout 355a597b43c80f616252cb370fa9f7e5c997418e && \
    mkdir build && cd build && \
    cmake3 -DCMAKE_INSTALL_PREFIX=/opt/nlopt \
   -DCMAKE_INSTALL_RPATH="${CMAKE_INSTALL_PREFIX}/lib" \
   -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE \
   -DCMAKE_BUILD_TYPE:STRING=Release \
   -DBUILD_SHARED_LIBS:STRING=OFF \
   -DCMAKE_CXX_FLAGS_RELEASE:STRING="-O3 -DNDEBUG" \
    .. && \
    make -j 4 && make install

ENV ITK_INSTALL_DIR=/opt/itk/
ENV ITK_DIR=${ITK_INSTALL_DIR}/lib/cmake/ITK-3.20.0/





  
  
#export ITK_DIR=/fileserver/opt/$HWPLATFORM/pkgs/itk/3.20.0/gcc-release/lib/InsightToolkit/
#export ITK_BINARY_DIR=$ITK_DIR
#export LD_LIBRARY_PATH=$ITK_DIR:${LD_LIBRARY_PATH}  

export LD_LIBRARY_PATH=/opt/x86_64/pkgs/itk/4.3.1/gcc-release/lib:${LD_LIBRARY_PATH}  
export LIBRARY_PATH=/opt/x86_64/pkgs/itk/4.3.1/gcc-release/lib:${LIBRARY_PATH}  



# The include file search paths:
SET(CMAKE_C_TARGET_INCLUDE_PATH
  "ITKIOFactoryRegistration"
  "/opt/x86_64/pkgs/itk/4.3.1/gcc-release/include/ITK-4.3"
  ".."
  "../include"
  "/home/ch169807/moti/itkNLOPTOptimizers/src"
  "/home/ch155417/moti/gsl/build/include"
  "/home/ch155417/moti/nlopt-2.2"
  "/home/ch155417/moti/nlopt-2.2/util"
  "/home/ch155417/moti/nlopt-2.2/api"
  "/home/ch169807/Software/tclap-1.2.1/include"
  "/opt/x86_64/pkgs/vtk/5.8.0/include/vtk-5.8"
  )
