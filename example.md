## Using binaries 

NB: this will only work on centOS

```

cd example_data 

# set binary 
binary=../bin/1_5T/ivimFBMMRFEstimator

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

# run 
$binary --optMode FBM -n $n_bvals -i  $txt_file -g $g -o $outdir -m $mask


```

## Using docker 

```

cd example_data 

# pull docker image
version=1_5T
docker push sergeicu/scim:$version

# set binary
binary=/scim/ivimFBMMRFEstimator

# set data paths 
txt_file=/data/bvalsFileNames_average_docker.txt

# set mask path 
mask=/data/mask_slice.nrrd

# set output directory 
outdir=test1 && rm -rf $outdir && mkdir -p $outdir && chmod ugo+rw $outdir

# set number of bvals 
n_bvals=8

# number of FBM iterations (0 for IVIM, 400 for SCIM, 1 for toy example of SCIM) 
g=1 

# run 
docker run -it --rm -v $outdir:/data/ $binary --optMode FBM -n $n_bvals -i  $txt_file -g $g -o /data/$outdir -m $mask


```



## Notes
- `$txt_file` needs to provide relative or absolute paths
- `$output_directory` must have at least `664` permissions if using docker (use `chmod` for this) 
- `version=3.5
