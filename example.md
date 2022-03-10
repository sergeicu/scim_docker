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

version=1_5T
docker pull sergieu

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



In more details: 
- `$txt_file` needs to provide relative (or absolute paths) 
- `$output_directory` needs to have chmod 664 permissions at least if using docker 
