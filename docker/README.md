### Build / fetch scim_base image

You need to  obtain a base image before building the scim docker.     
This base image contains centos6, as well as all the necessary dependencies for building scim code. 

You can fetch this image here:   
`docker pull sergeicu/scim_base:latest`   
or here   
`docker image load < /fileserver/external/body/serge/scim/docker/scim_base_latest.tar`  

Alternatively, you can build the image from scratch (~1-2hours):   

```
username=sergeicu
name=sergeicu/scim_base:latest
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --no-cache --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile_scim_base .

```

Note: `scim_base` does not contain any scim source code.   
Note2: `scim_base` requires `libs` folder to be present in local directory. Download it first from [BCH Google Drive](https://drive.google.com/drive/folders/1i13o5E9DB0YdX5ZdaGQbfRvOb7d5fDMz?usp=sharing) or from `/fileserver/external/body/serge/scim/docker`

### Build scim docker image
```
version=3T
username=sergeicu
name=sergeicu/scim:$version
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --no-cache --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile .

```

### Export scim binaries locally

This is how we made the binaries available in [bin](https://github.com/sergeicu/scim_docker/tree/main/bin) folder

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


## Save and load docker images to and from .tar 

`docker save scim:3T > scim_3T.tar`
`docker load < scim_3T.tar`


## push all files to google drive 

`libs` folder (referenced in Dockerfile) contains dependencies for compiling scim. 
These are not shared on github as their size is very large. Instead, we made these available in .tar [here](https://drive.google.com/drive/folders/1i13o5E9DB0YdX5ZdaGQbfRvOb7d5fDMz?usp=sharing) (warning: only bch users with access) and `/fileserver/external/body/serge/scim/docker`


## Push docker images to dockerhub 
NB we ONLY push docker images that do not contain any source code

`docker login`   
`docker push $name`   


## About different versions of SCIM 

There are three distinct versions that we are interested to build: 
- SCIM 3T 
- SCIM 3Ti (interactive)
- SCIM 1.5T 
   
Original SCIM relies on estimation of noise from the background of the image.   
Unfortunately, newer 3T scanner yields images with zeros in the background. Hence we have two options  
- 1.Assume certain average value of noise (scim:3T)
- 2.Set noise interactively (scim:3Ti) 


From empirical experiments on 3T prisma scanner, we devices that for current protocol the noise is ~`2`. Hence, scim:3T version has fixed noise of `2` 

## How to fetch particular version of SCIM (warning: may be updated later) 

Our scim code is available in a separate repository, which is linked here as a submodule. 
https://github.com/sergeicu/scim/branches   

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
