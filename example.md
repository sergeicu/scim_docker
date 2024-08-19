## Using binaries 

NB: this will definitely work on centOS, and possibly some version of ubuntu (no guarantee)

```

cd example_data 

./ivimFBMMRFEstimator --optMode FBM -n 8 -i bvalsFileNames_average_local.txt -g 1 -o $PWD/test1/ -m mask_slice.nrrd -v 2

# run SCIM with bootstrap (i.e. slightly disturbed signal and compute N version of this code)
./ivimFBMinvivoSE --numOfGDIters 1 --bootstrapIterations 3 --bValuesNum 8 --threshold -1 -i bvalsFileNames_average_local.txt -o $PWD/test1/ -m mask_slice.nrrd --variance 10 
```

Input options for `ivimFBMMRFEstimator`: 
```
		TCLAP::ValueArg<int> BValuesNumArg("n","bValuesNum","bValuesNum",false,6,"int");
		cmd.add( BValuesNumArg );

		TCLAP::ValueArg<int> ThresholdArg("t","threshold","threshold",false,-1,"int");
		cmd.add( ThresholdArg );

		TCLAP::ValueArg<std::string> ImagesNamesFileNameArg("i","ImagesNamesFileName","b_value images names filename",true,"","string");
		cmd.add( ImagesNamesFileNameArg );

		TCLAP::ValueArg<std::string> outputFolderFileNameArg("o","outputFolderFileName","Output gray scale image file name",true,"","string");
		cmd.add( outputFolderFileNameArg );

		TCLAP::ValueArg<std::string> MaskImageFileNameArg("m","MaskImageFileName","MaskImageFileName",false,"","string");
		cmd.add( MaskImageFileNameArg );

		TCLAP::ValueArg<int> numOfGDItersArg("g","numOfGDIters","numOfGDIters",false,0,"int");
		cmd.add( numOfGDItersArg );

		TCLAP::ValueArg<int> BSSTDArg("","BS","bootstrap estimate of STD",false,0,"int");
		cmd.add( BSSTDArg );

		TCLAP::ValueArg<std::string> OptModeArg("","optMode","optMode",true,"","string");
		cmd.add( OptModeArg );

		TCLAP::ValueArg<double> varianceArg("v", "variance", "Variance for Rician noise (default: 2.0)", false, 2.0, "double");
		cmd.add(varianceArg);
```

Input options for `ivimFBMinvivoSE`: 
```
		TCLAP::ValueArg<int> BValuesNumArg("n","bValuesNum","how many bvalues we have",false,6,"int");
		cmd.add( BValuesNumArg );

		TCLAP::ValueArg<int> ThresholdArg("t","threshold","threshold for masking",false,-1,"int");
		cmd.add( ThresholdArg );

		TCLAP::ValueArg<std::string> ImagesNamesFileNameArg("i","ImagesNamesFileName","b_value images names filename",true,"","string");
		cmd.add( ImagesNamesFileNameArg );

		TCLAP::ValueArg<std::string> outputFolderFileNameArg("o","outputFolderFileName","Output gray scale image file name",true,"","string");
		cmd.add( outputFolderFileNameArg );

		TCLAP::ValueArg<std::string> MaskImageFileNameArg("m","MaskImageFileName","MaskImageFileName",false,"","string");
		cmd.add( MaskImageFileNameArg );

		TCLAP::ValueArg<double> varianceArg("v", "variance", "Variance for Rician noise (default: 2.0) -> more here - https://chatgpt.com/share/7b80279b-14eb-4948-8342-24a58b1b6338
        ", false, 2.0, "double");
		cmd.add(varianceArg);

		TCLAP::ValueArg<int> bsIterArg("b", "bootstrapIterations", "How many iterations of bootstrap ", false, 100, "int");
		cmd.add(bsIterArg);		

		TCLAP::ValueArg<int> numOfGDItersArg("g","numOfGDIters","how many spatial iterations of scim ",false,250,"int");
		cmd.add( numOfGDItersArg );
```


## Using docker 

### In the terminal 
```

cd example_data 

# pull docker image
version=3Ti
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
