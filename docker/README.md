## Versions of SCIM 

There are three distinct versions that we are interested to build: 
- SCIM 3T 
- SCIM 3Ti (interactive)
- SCIM 1.5T 

Explanation:  
Original SCIM relies on estimation of noise from the background of the image, which was available on 1.5T data. Newer scanner, 3T, yields images with zeros in the background. Hence we have two options - 1. Assume certain average value of noise 2. Set b-value interactively. 


SCIM 3T: from experiments we deviced that the average relative noise value in 3T data was 2.   






## How to fetch particular version of SCIM 

*go into scim directory* 
`cd docker/scim`

*1.5T - original Moti code* 
`git switch main `
`git checkout dd34a018ab22cc62586e15f6e3d22ade40469fa3`

*3Ti - interactive*
`git switch variance_equals_2`
`git checkout a783a1a6a1affb53137a99466e465ce7404385b1`


*3T - variance hard set to 2* 
`git switch variance_equals_2`
`git checkout b1ac18fc8745c9090e41d82fe65193af1f33435f`

NB: 3Ti will unfortunately request noise level to be entered at EVERY iteration of FBM. Further work is required on the code base to make this into a global parameter. 

### Build docker 
```
name=scim:3T
name=scim:1_5T
bch_proxy=http://proxy.tch.harvard.edu:3128
docker build --no-cache --build-arg http_proxy=$bch_proxy -t $name -f Dockerfile .

```


### Run docker interactively

```
name=scim:3T
docker run -it --rm $name /bin/bash
$ ls /scim/bin
```


### Export binaries from docker into local directory 

```
# outside docker 
username=sergeicu
version=1_5T
name=$username/scim:$version
local=~/fs/trash/scim_bin
rm -rf $local 
mkdir -p $local
chmod -R ugo+rw $local
docker run -it --rm -v $local:/scim_bin $name /bin/bash

# inside docker 
ls /scim/bin
cp -R /scim/bin/* /scim_bin
chmod -R ugo+rw /scim_bin
exit

# outside docker 
cp -R $local/* ../bin/$version
```


## Save and load docker image to and from .tar 

*scim version*
`docker save scim:3T > scim_3T.tar`
`docker load < scim_3T.tar`


*scim_base*
`docker save scim_base:latest > scim_base_latest.tar`
`docker load < scim_base_latest.tar`



## push all files to google drive 

Libs dependencies (necessary for building the full docker image) and the .tar images are uploaded here

https://drive.google.com/drive/folders/1i13o5E9DB0YdX5ZdaGQbfRvOb7d5fDMz?usp=sharing
(warning: only bch users with access)

## Push docker images to dockerhub 

NB we ONLY want to push docker image that does not contain any source code - i.e. `scim:<version>` image and not `scim_base` image. 

`docker login` 
`docker push $name` 


