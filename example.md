## Using binaries 

NB: this will only work on centOS

```

cd example_data 

# set binary 
binary=../bin/3Ti/ivimFBMMRFEstimator

# set data paths 
txt_file=bvalsFileNames_average_local.txt

# set mask path 
mask=mask_slice.nrrd

# set output directory 
outdir=$PWD/test1/ && rm -rf $outdir && mkdir -p $outdir

# set number of bvals 
n_bvals=8

# number of FBM iterations (0 for IVIM, 400 for SCIM, 1 for toy example of SCIM) 
g=1 

# set variance (if running 3Ti model) -> see this for more details -> https://chatgpt.com/share/7b80279b-14eb-4948-8342-24a58b1b6338
variance=2

# run 
$binary --optMode FBM -n $n_bvals -i  $txt_file -g $g -o $outdir -m $mask -v $variance 


```

## Using docker 

### In the terminal 
```

cd example_data 

# pull docker image
version=1_5T
image=sergeicu/scim:$version
docker pull $image

# set binary
binary=/scim/bin/ivimFBMMRFEstimator

# set data paths 
txt_file=/data/bvalsFileNames_average_docker.txt

# set mask path 
mask=/data/mask_slice.nrrd

# set output directory 
outdir=test2 && rm -rf $outdir && mkdir -p $outdir && chmod ugo+rw $outdir

# set number of bvals 
n_bvals=8

# number of FBM iterations (0 for IVIM, 400 for SCIM, 1 for toy example of SCIM) 
g=1 

# run (single command)
docker run -it --rm -v $PWD:/data/ $image $binary --optMode FBM -n $n_bvals -i  $txt_file -g $g -o /data/$outdir -m $mask && chmod -R ugo+rw /data/

```

### Inside docker 
```

cd example_data 

# pull docker image
version=1_5T
image=sergeicu/scim:$version
docker pull $image

# enter docker 
docker run -it --rm -v $PWD:/data/ $image /bin/bash 

# set binary
binary=/scim/bin/ivimFBMMRFEstimator

# set data paths 
txt_file=/data/bvalsFileNames_average_docker.txt

# set mask path 
mask=/data/mask_slice.nrrd

# set output directory 
outdir=test2 && rm -rf /data/$outdir && mkdir -p /data/$outdir && chmod ugo+rw /data/$outdir

# set number of bvals 
n_bvals=8

# number of FBM iterations (0 for IVIM, 400 for SCIM, 1 for toy example of SCIM) 
g=1 

# run 
$binary --optMode FBM -n $n_bvals -i  $txt_file -g $g -o /data/$outdir -m $mask

# set permissions 
chmod -R ugo+rw /data/

# exit docker 
exit

```



## Notes
- `$txt_file` needs to provide relative or absolute paths
- `$output_directory` must have at least `664` permissions if using docker (use `chmod` for this) 
- `chmod -R ugo+rw` is necessary command inside docker else you won't be able to delete / move your data
