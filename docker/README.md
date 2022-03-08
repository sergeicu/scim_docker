## Versions of SCIM 

There are three distinct versions that we are interested to build: 
- SCIM 3T 
- SCIM 3Ti (interactive)
- SCIM 1.5T 

Explanation:  
Original SCIM relies on estimation of noise from the background of the image, which was available on 1.5T data. Newer scanner, 3T, yields images with zeros in the background. Hence we have two options - 1. Assume certain average value of noise 2. Set b-value interactively. 


SCIM 3T: from experiments we deviced that the average relative noise value in 3T data was 2.   




## How to fetch particular version of SCIM 

`cd docker/scim`
`git branch -a` 
`git checkout` 


### Build docker 
```
name=scim:3T
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile .

```


### Run docker interactively

```
name=scim:3T
docker run -it --rm $name /bin/bash
$ ls /scim/bin
```


### Export binaries from docker into local directory 

```
version=3T
name=scim:$version
local=~/fs/trash/scim_bin
chmod -R ugo+rw $local
docker run -it --rm -v $local:/scim_bin $name /bin/bash
$ ls /scim/bin
$ cp -R /scim/bin/* /scim_bin
$ chmod -R ugo+rw /scim_bin
$ exit
cp -R $local/* ../bin/$version
```


## Save and load docker image to and from .tar 

*binaries*
`docker save scim:3T > scim_3T.tar`
`docker load < scim_3T.tar`


*base*
`docker save scim_base:latest > scim_base_latest.tar`
`docker load < scim_base_latest.tar`



## push all files to google drive 


Libs dependencies (necessary for building the full docker image) and the .tar images are uploaded here

https://drive.google.com/drive/folders/1i13o5E9DB0YdX5ZdaGQbfRvOb7d5fDMz?usp=sharing
(warning: only bch users with access)

## Push docker images to dockerhub 

NB we ONLY want to push docker image that does not contain any source code - i.e. `scim` image and not `scim_base` image. 

`docker push scim:3T` 


