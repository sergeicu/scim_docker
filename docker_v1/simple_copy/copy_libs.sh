# copy the binaries 
target=~/w/code/docker/scim/s20220204_build_docker_centos_nlopt/docker_v1/simple_copy/libs/
cp -R /home/ch155417/moti/gsl/ $target
cp -R /home/ch169807/moti/itkNLOPTOptimizers/ $target
cp -R /home/ch155417/moti/nlopt-2.2/ $target
cp -R /home/ch169807/Software/tclap-1.2.1 $target



# copy x86_64 binaries - 
mkdir -p $target/itk $target/vtk
cp -R /opt/x86_64/pkgs/itk/4.3.1/ $target/itk
cp -R /opt/x86_64/pkgs/itk/3.20.0/ $target/itk
cp -R /opt/x86_64/pkgs/vtk/5.8.0 $target/vtk
cp -R /opt/x86_64/pkgs/vtk/6.1.0/ $target/vtk

# copy scim code 
cp -R /fileserver/external/body/IVIM_code/FBMcode/dwi_bootstrap $target/../scim_cpp

# sila's bashrc 
~/fs/serge/s20220203_temp_home_down/w/code/ivim/experiments/s20210730-debugging-scim-cpp-and-spim-matlab/scim-cpp/bashrc_sila_20210730


# main build 
/fileserver/fastscratch/serge/s20210809-ivim-cpp-build

# instructions for that build 
/fileserver/fastscratch/serge/s20220203_temp_home_down/w/code/ivim/experiments/s20210730-debugging-scim-cpp-and-spim-matlab/scim-cpp/s20210730-log.sh



###################
# Create a docker image for centos6 and copy files above to it to the exact same directories 
###################
name=scim_test1
# base Dockerfile is here - fileserver/fastscratch/serge/s20220203_temp_home_down/w/code/docker/scim/s20220204_build_docker_centos_nlopt/docker_v1/Dockerfile
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name . 
docker build --no-cache  --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t ${name}_remove -f Dockerfile_test . 
docker run --rm -it $name /bin/bash
docker run --rm -it ${name}_remove /bin/bash


# try the build process inside docker 
docker run --rm -it $name /bin/bash
cd /scim/ && mkdir build && cd build 
ccmake .. 


##################
# install cmake and make 
##################

####
# check how to install cmake inside docker 
####


docker run --rm -it -e http_proxy=http://proxy.tch.harvard.edu:3128 centos6.10_yum_basic /bin/bash # export http_proxy=http://proxy.tch.harvard.edu:3128
yum info cmake
yum info make
yum install -y make 
yum install -y cmake 

# how to run cmake within docker
CMakeLists.txt

RUN mkdir vtksrc && cd vtksrc && \
    git clone https://gitlab.kitware.com/vtk/vtk.git && \
    cd vtk && \
    git checkout v9.0.3 && \
    mkdir build && \
    cd build && \
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
    .. && \
    make -j 4 && make install && \
    cd /usr/src && \
    rm -rf /usr/src/vtksrc
    
    
    
# add repos list 
https://www.getpagespeed.com/server-setup/how-to-fix-yum-after-centos-6-went-eol


# 
docker run --rm -it -e http_proxy=http://proxy.tch.harvard.edu:3128 centos:centos6.10 /bin/bash 