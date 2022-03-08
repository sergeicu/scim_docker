import os 
import argparse
import glob 


import numpy as np 
import nibabel as nb 


def get_bval_paths(b0_path, bvals):
    bval_paths = [b0_path.replace("b0","b"+str(i)) for i in bvals]
    
    assert all([os.path.exists(i) for i in bval_paths])
    
    return bval_paths
  
def get_region(im, mask):
    
    assert im.shape == mask.shape 
    
    
    im_v = im[mask!=0] 
    assert im_v.ndim == 1

    return im_v 
    
    
def loadim(path):
    
    # load images 
    if path.endswith(".nii.gz"): 
        im = nb.load(path).get_fdata()
    elif path.endswith(".nrrd"):
        im, _ = nrrd.read(path)
    
    return im

def calculate_rician(v):
    
    # converted from C++ from: /fileserver/external/body/serge/s20210809-ivim-cpp-build/dwi_bootstrap_v1/include/itkRicianNoiseCalculator.txx
    # for single coil 
    
    """C++ form 
          RealType sum = 0.0;
          size_t numOfPixels = 0;
          for (it.GoToBegin();!it.IsAtEnd();++it )
          {
              sum += (it.Value()*it.Value());
              ++numOfPixels;

            //  std::cout << numOfPixels << ": " << sum << std::endl;
          }
          m_Variance = vcl_sqrt(sum/(2*numOfPixels));
        //  std::cout << m_Variance << std::endl;

    """    
    sum_of_squares = np.sum(np.square(v))
    #from IPython import embed; embed()
        
    num_of_pixels = len(v)
    
    variance = np.sqrt(sum_of_squares/(2*num_of_pixels))
    
    
    debug = True
    if debug: 
    
        print("Last pixel's value")
        print(v[-1])
        print("sum of squares")
        print(sum_of_squares)
        print("num of pixels")
        print(num_of_pixels)
        
    
    
    return variance 
    
    
    
    
    
    
    
    return noise

def load_args():
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--b0',type=str, help='full path to b0 image')
    parser.add_argument('--bvals', type=int,nargs='+',default=[0, 50, 100, 200, 400, 600, 800], help='bvalues to use')    
    parser.add_argument('--mask', type=str, default=None,help='specify full path to mask image. If not specified, the corner of the image will be used (default)')
    parser.add_argument('--debug', action="store_true", help='load default bvalue images and default mask')               
    args = parser.parse_args()
            
    
    return args 

if __name__ == '__main__':

    args = load_args()

    # debug 
    if args.debug:
        d="/fileserver/fastscratch/ipek/patient6/volumes/"
        args.b0 = d + "b0_averaged.nii.gz"
        args.mask = d + "mask_example.nii.gz"
        args.bvals = [0, 50, 100, 200, 400, 600, 800]
        

    # init
    b0_path = args.b0
    assert os.path.exists(b0_path)
    bvals = args.bvals

    # load mask 
    if args.mask is not None:
        assert os.path.isfile(args.mask)
        mask_path = args.mask
        assert os.path.exists(mask_path)
        mask = loadim(mask_path)
    else:
        # get corner of b0 image 
        b0 = loadim(b0_path)
        x,y,z = b0.shape
        mask = np.zeros_like(b0)
        mask[0:x//10, 0:y//10, :] = 1
        #from IPython import embed; embed()
        #mask[-x//10:, -y//10:, :] = 1

    # get bvalue paths 
    bval_paths = get_bval_paths(b0_path, bvals)
    
    # load b0 images 
    ims = []
    for bval_path in bval_paths: 
        ims.append(loadim(bval_path))
        
    # mask over the region from which rician noise is calculated
    ims_v = []
    for im in ims:
        ims_v.append(get_region(im, mask))

    
    # calculate rician noise 
    rician = []
    for v in ims_v:
        rician.append(calculate_rician(v))


    # convert to a list of strings with 2dp
    rician_2dp = [round(i, 2) for i in rician]

    
    # convert into string of numbers 
    cmd = ' '.join([str(i) for i in rician_2dp])
    
    # print to two decimal places
    print("Rician noise:")
    print(cmd)
  