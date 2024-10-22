# SCIM 

SCIM - an efficient iterative "fusion bootstrap moves" (FBM) solver that enables precise parameter estimates for Intravoxel Incoherent Motion of DWI MRI images in spatially constraint manner.   

C++ compiled binaries are available prebuilt as a Docker image. Binaries are also available in `bin` folder and require centOS to be ran. 

### Example 

See here: [example.md](example.md)

### Available images
- 1.5T - computes IVIM via MLE cost function, with noise estimated from background regions 
- 3T - // with noise fixed at 2 
- 3Ti - // with noise input defined by user (NB this needs to be specified for every FBM iteration) 

**Images are available from**
- [dockerhub](https://hub.docker.com/r/sergeicu/scim/tags)
- [BCH google drive](https://drive.google.com/drive/folders/1i13o5E9DB0YdX5ZdaGQbfRvOb7d5fDMz?usp=sharing)  (warning: only bch users with access)
- `/fileserver/external/body/serge/scim/docker`

### Dockerhub
`docker pull sergeicu/scim:$version` 

### Binaries 
See [bin/](bin)

### Docker image info 
Docker images are based on centos6. 

Binaries in `bin` folder will run on centOS6+ without docker. 

To build new docker image see [docker/README.md](docker/README.md).   

Basic explanations of what changes were made to the original C++ code for versions (1.5T, 3T, 3Ti) see [changes_to_original_code.md](https://github.com/sergeicu/scim/blob/main/changes_to_original_code.md)


---------------

### Citation/Contact

This code is under [Apache 2.0](LICENSE.txt) licensing. \
If you use it, please cite one of the following papers:

**Reliable estimation of incoherent motion parametric maps from diffusion-weighted MRI using fusion bootstrap moves** \
Freiman, Moti and Perez-Rossello, Jeannette M and Callahan, Michael J and Voss, Stephan D and Ecklund, Kirsten and Mulkern, Robert V and Warfield, Simon K \
Medical Image Analysis 2013 \
[[link](https://pubmed.ncbi.nlm.nih.gov/23434293/) | [bibtex](bibtex.bib)]


**Spatially-constrained probability distribution model of incoherent motion (SPIM) for abdominal diffusion-weighted MRI** \
Kurugol, Sila and Freiman, Moti and Afacan, Onur and Perez-Rossello, Jeannette M and Callahan, Michael J and Warfield, Simon K \
Medical Image Analysis 2016 \
[[link](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4903917/) | [bibtex](bibtex.bib)]


**Spatially-constrained probability distribution model of incoherent motion (SPIM) in diffusion weighted MRI signals of crohn’s disease** \
Kurugol, Sila and Freiman, Moti and Afacan, Onur and Perez-Rossello, Jeannette M and Callahan, Michael J and Warfield, Simon K \
MICCAI 2014 \
[[link](https://link.springer.com/chapter/10.1007/978-3-319-13692-9_11) | [bibtex](bibtex.bib)]


**Evaluation of Motion-Compensated Spatially-Constrained IVIM (MC-SCIM) Model of Diffusion-weighted MRI for Assessment of Fibrosis in Crohn’s Disease using Surgical Histopathology Scores** \
Kurugol, Sila and Freiman, Moti and Goldsmith, Jeffrey and Didier, Ryne and Afacan, Onur and Perez-Rossello, Jeanette M and Callahan, Michael J and Bousvaros, Athos and Warfield, Simon K \
ISMRM 2017 \
[[link](https://scholar.harvard.edu/silakurugol/publications/evaluation-motion-compensated-spatially-constrained-ivim-mc-scim-model) | [bibtex](bibtex.bib)]


**Self-supervised IVIM DWI parameter estimation with a physics based forward model** \
Vasylechko, Serge Didenko and Warfield, Simon K and Afacan, Onur* and Kurugol, Sila* \
*contributed equally \
MRM 2022 \
[[link](https://onlinelibrary.wiley.com/doi/10.1002/mrm.28989) | [arxiv](https://arxiv.org) | [bibtex](bibtex.bib)]



If you have any question regarding the usage of this code, or any suggestions to improve it, you can contact us at: \
serge.vasylechko@childrens.harvard.edu
                                                                     
     
```
