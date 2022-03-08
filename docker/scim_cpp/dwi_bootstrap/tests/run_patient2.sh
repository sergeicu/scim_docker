# using hashtag in .sh scripts is a comment. It's equivalent to '%' in matlab 

# default
echo "default bin is /fileserver/abd/bin/ivimFBMMRFEstimator" 

# set path to binary to run 
binary=$2 #/fileserver/abd/bin/ivimFBMMRFEstimator
outdir=$1

# set scan directory 
d=/fileserver/external/body/serge/s20210809-ivim-cpp-build/dwi_bootstrap_v1/patient2/

# set mask path 
mask=$d/mask_slice.nrrd

# set .txt file path 
txt_file=$d/bvalsFileNames_average.txt

# set output directory 
outdir=$d/BOBYQA_${outdir}

# create output directory 
mkdir $outdir

# set number of bvals 
n_bvals=8

# run 
$binary --optMode FBM -n $n_bvals -i  $txt_file -g 0 -o $outdir -m $mask
