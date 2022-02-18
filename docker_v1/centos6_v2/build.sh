name=centos6.10_yum_basic
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name .
docker run --rm -it $name /bin/bash


export http_proxy=http://proxy.tch.harvard.edu:3128

# download vtk 
git clone https://gitlab.kitware.com/vtk/vtk.git
cd vtk 
git checkout v5.8.0 
mkdir build && cd build 


# install wget 
yum install -y wget 


# install tar 
yum install -y tar 

# download vtk dependencies: cmake 
wget http://www.cmake.org/files/v2.8/cmake-2.8.12.2.tar.gz \
    && tar xzf cmake-2.8.12.2.tar.gz
    
    
cd cmake-2.8.12.2 \
    && ./configure --prefix=/usr/local \
    && make \
    && make install
    
    
# update 
yum update 

    
# download gcc (hopefully the correct version)
wget -O /etc/yum.repos.d/slc6-devtoolset.repo http://linuxsoft.cern.ch/cern/devtoolset/slc6-devtoolset.repo
yum install devtoolset-2-gcc-c++