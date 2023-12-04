### Step 1: Build OR Download scim_base image

You need to obtain a base image (`scim_base`) before building the `scim` image.     
This base image contains centos6, as well as all the necessary dependencies for building scim code. 

A. DOWNLOAD  
You can download this image here:   
`docker pull sergeicu/scim_base:latest`   
or here   
`docker image load < /fileserver/external/body/serge/scim/docker/scim_base_latest.tar`  

B. BUILD 
Alternatively, you can build your own image from scratch (~1-2hours):   

```
username=sergeicu
name=sergeicu/scim_base:latest
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --no-cache --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile_scim_base .

```

Note: `scim_base` does not contain any scim source code.   
Note2: if you are building `scim_base` image from scratch - you will need to download additional files. These are ITK/VTK/NLOPT/GSL/TCLAP dependencies for CENTOS6 build process. The reason these are not provided in this git repository is because of a large size of these files (~800M). These files, named aptly as `libs` folder, must be present when building scim_base image. Download it first from [BCH Google Drive](https://drive.google.com/drive/folders/1i13o5E9DB0YdX5ZdaGQbfRvOb7d5fDMz?usp=sharing) or from `/fileserver/external/body/serge/scim/docker`

### Step 2: Download SCIM source code 

In order to build scim docker image, you will need to download our source code (not publically available, unlinke this repository).  
You will need to place this source code in 'docker/scim' folder. 

It is available here: 

https://github.com/quin-med-harvard-edu/scim
https://github.com/sergeicu/scim

Important: please read additional information about which scim source code commit you should use at the bottom of this readme. 

### Step 3: Build scim docker image


```
version=3T
username=sergeicu
name=sergeicu/scim:$version
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --no-cache --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile .

```
NB you must run `docker build` from inside 'docker' subfolder. 

### [optional] Export scim binaries locally 

If you would like to run the scim binaries directly on your own CENTOS6 or CENTOS7 machines, without having to call 'docker' - you can export them manually as presented below.  
NB: this step assumes that you have the correct VTK/ITK dependencies available on your local machine (note this is presently working for the following CRL/QUIN machines - boreas/zephyr/auster/carlsen, however it may be broken any time if the system updates, and so we recommend to use docker directly). 

This is how we copied the binaries locally as shown in [bin](https://github.com/sergeicu/scim_docker/tree/main/bin) folder

```
# on your local machine Terminal
username=sergeicu
version=1_5T
name=$username/scim:$version
localfolder=<local_folder_to_place_binaries>
rm -rf $local 
mkdir -p $local
chmod -R ugo+rw $local
docker run -it --rm -v $localfolder:/scim_bin $name /bin/bash

# run these commands inside docker container
ls /scim/bin
cp -R /scim/bin/* /scim_bin
chmod -R ugo+rw /scim_bin
exit

```


## [optional] Save and load docker images to and from .tar 

If you would like to save a local copy of your pre-build docker image (and prevent the accidental removal of your compiled docker image that you worked so hard to make - when somebody runs `docker prune -a` on your machine) - do this: 

`docker save scim:3T > scim_3T.tar`
`docker load < scim_3T.tar`


## [optional] Push docker images to dockerhub 
Make your docker image available to anyone with a single line of code.  

NB we ONLY push docker images that do not contain any source code

`docker login`   
`docker push $name`   


## IMPORTANT: About different versions of SCIM 

There are three distinct versions that we are interested to build: 
- SCIM 3T 
- SCIM 3Ti (interactive)
- SCIM 1.5T 
   
Original SCIM relies on estimation of noise from the background of the image.   
Unfortunately, newer 3T scanner yields images with zeros in the background. Hence we have two options  
- 1. Assume certain average value of noise (scim:3T)
- 2. Set noise interactively (scim:3Ti) 


From empirical experiments on 3T prisma scanner, we devices that for current protocol the noise is ~`2`. Hence, scim:3T version has fixed noise of `2` 

## Important: How to fetch particular version of SCIM (warning: may be updated later) 

Our scim source code is available in a separate private repository (not free to public), which is linked here as a submodule. 
https://github.com/sergeicu/scim/branches   
https://github.com/quin-med-harvard-edu/scim/branches   

Different branches and commits in this repository correspond to different version of the code that we had build.   
Current version are fetched from these commits / branches: 


*go into scim/ directory*   
`cd docker/scim`   


*1.5T - original code*    
`git switch 1_5T `   


*3Ti - interactive*   
`git switch 3Ti`   

*3T - variance hard set to 2*    
`git switch 3T`  


TODO: Current version of 3Ti will unfortunately request noise level to be entered for EVERY iteration of FBM. This is a bug. Further work is required to make this a global parameter. 


Important: BEFORE you build your scim docker image - make sure that you checkout the correct version of scim source code as shown above. 

