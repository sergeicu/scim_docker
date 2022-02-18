# Objective: build vtk from source


# source: 
# https://github.com/lukin0110/docker-vtk-python/blob/master/Dockerfile


# steps to build: 
# - download vtk and unpackage
# - download cmake 
# - make sure your gcc environment matches cmake 
# - install build-essential libgl1-mesa-dev 
# - build tcl 
# - build tk 
# - 


name=vtk
docker build --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t $name .
docker run --rm -it $name /bin/bash

# now go and identify gcc file 
export http_proxy=http://proxy.tch.harvard.edu:3128

wget --no-check-certificate http://www.vtk.org/files/release/7.0/VTK-7.0.0.tar.gz && tar -zxvf VTK-7.0.0.tar.gz





# build essentials 
#https://unix.stackexchange.com/questions/16422/cant-install-build-essential-on-centos/63068
yum groupinstall 'Development Tools'

