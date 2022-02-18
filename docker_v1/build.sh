name=centos6.10_yum_basic
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name .

docker run --rm -it $name /bin/bash



# rename images 
docker image tag scim_test_v1 centos6.10_yum_basic
docker image rm scim_test_v1

# libs we need 
gsl
itkNLOPTOptimizers 
ITK-4.3 
nlopt-2.2
tclap-1.2.1
vtk 5.8.0 


############
# Instal libs from inside docker 
############

docker run --rm -it centos6.10_yum_basic /bin/bash

export http_proxy=http://proxy.tch.harvard.edu:3128


# gcc
yum install gcc-g++
yum install ncurses-devel



# install dependencies to build process 
yum install -y cmake wget git perk unzip 
yum install -y build-essential


# build vtk 
git clone https://gitlab.kitware.com/vtk/vtk.git
cd vtk 
git checkout v5.8.0 
mkdir build && cd build 


yum install -y cmake

cmake -DCMAKE_INSTALL_PREFIX=/opt/vtk \
-DCMAKE_INSTALL_RPATH="${CMAKE_INSTALL_PREFIX}/lib" \
-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE \
-DVTK_Group_Qt=ON \
-DVTK_QT_VERSION=5 \
-DVTK_Group_Imaging=ON \
-DVTK_Group_Views=ON \
-DModule_vtkRenderingFreeTypeFontConfig=ON \
-DVTK_WRAP_PYTHON=ON \
-DVTK_PYTHON_VERSION=2 \
-DPYTHON_EXECUTABLE=/usr/bin/python \
-DBUILD_TESTING=OFF \
-DVTK_USE_SYSTEM_LIBRARIES=ON \
-DVTK_USE_SYSTEM_LIBPROJ4=OFF \
-DVTK_USE_SYSTEM_GL2PS=OFF \
-DVTK_USE_SYSTEM_LIBHARU=OFF \
-DVTK_USE_SYSTEM_PUGIXML=OFF \
-DCMAKE_BUILD_TYPE=Release


#-DVTK_PYTHON_VERSION=3 \
#-DPYTHON_EXECUTABLE=/usr/bin/python3 \