d=~/fs/s20210831_summary_work_done_w_ic
outdir=~/w/code/docker/scim/s20220204_build_docker_centos_nlopt/docker_v1/simple_copy/





# set scan directory 
d=/fileserver/external/body/serge/s20210809-ivim-cpp-build/dwi_bootstrap_v1/tests/patient2/

# set mask path 
mask=$d/mask_slice.nrrd

# set .txt file path 
txt_file=$d/bvalsFileNames_average_local.txt

# set output directory 
outdir=$d/BOBYQA_REMOVE_TEST_ONLY/

# create output directory 
mkdir $outdir

# set number of bvals 
n_bvals=8

# run locally  
cd $d
/fileserver/abd/bin/ivimFBMMRFEstimator --optMode FBM -n $n_bvals -i  $txt_file -g 0 -o $outdir -m $mask


# run via docker - interactively 
docker run -it --rm -v $d:/data centos6_scim /bin/bash 
$ cd /data
$ /scim/bin/ivimFBMMRFEstimator --optMode FBM -n 8 -i $txt_file -g 0 -o $outdir -m $mask
$ chmod -R ugo+rw /data

# run via docker - externally 
txt_file=/data/bvalsFileNames_average_local.txt
outdir=/data/BOBYQA_REMOVE_TEST_ONLY/
mask=/data/mask_slice.nrrd
docker run -it --rm -v $d:/data centos6_scim /scim/bin/ivimFBMMRFEstimator --optMode FBM -n 8 -i $txt_file -g 0 -o $outdir -m $mask


