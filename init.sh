# Goal: build basic docker image based on centos6 with simon's tools and compile IVIM code within it. 

# pull basic centos image 
docker pull centos:centos6.10


# setup 
cd /fileserver/Rad-Warfield-e2/Groups/Imp-Recons/serge/code/ivim_motion/experiments/s20220204_build_docker_centos_nlopt/docker_v1/Dockerfile


# build docker 
name=scim_test_v1
docker build --no-cache -t $name .

# go into docker and build from there 
docker run -it --rm $name /bin/bash

# check things 
cat /etc/os-release
uname -r 
lsb_release -a
