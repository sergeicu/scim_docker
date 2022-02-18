docker build --no-cache  --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t nvidia-test-hard -f Dockerfile_hard .

docker build --no-cache  --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t nvidia-test-simple -f Dockerfile_simple .


docker run --rm -it nvidia-test-simple /bin/bash
docker run --rm -it --gpus all nvidia-test-simple /bin/bash


# version 1 
https://nvidia.github.io/nvidia-docker/





# How to build a GPU compatible DOCKER image on centos7 

# Compiled from: 
# source: https://nvidia.github.io/nvidia-docker/
# source: https://www.cloudsavvyit.com/14942/how-to-use-an-nvidia-gpu-with-docker-containers/

# updated packages 
distribution=$(. /etc/os-release;echo $ID$VERSION_ID)
curl -s -L https://nvidia.github.io/nvidia-docker/$distribution/nvidia-docker.repo | \
  sudo tee /etc/yum.repos.d/nvidia-docker.repo
sudo yum-config-manager --enable libnvidia-container-experimental
sudo yum-config-manager --enable nvidia-container-runtime-experimental
sudo yum-config-manager --disable libnvidia-container-experimental
sudo yum-config-manager --disable nvidia-container-runtime-experimental


# install docker repo 
yum install -y nvidia-docker2

# restart docker 
sudo systemctl restart docker

# create the following Dockerfile. let's call 
mkdir -p test && cd test 
echo "FROM nvidia/cuda:10.2-base" >> Dockerfile_simple
echo "CMD nvidia-smi" >> Dockerfile_simple # https://towardsdatascience.com/how-to-properly-use-the-gpu-within-a-docker-container-4c699c78c6d1


# build the docker 
docker build --no-cache  --build-arg http_proxy=http://proxy.tch.harvard.edu:3128 -t nvidia-test-simple -f Dockerfile_simple .

# test docker 
docker run -it --gpus all nvidia-test-simple nvidia-smi && echo "location of nvidia-smi" && which nvidia-smi 

