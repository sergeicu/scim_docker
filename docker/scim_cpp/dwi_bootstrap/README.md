
C++ source code for SCIM IVIM. 

### Described by: 
- Taimouri et al (2015). Spatially constrained incoherent motion method improves diffusion-weighted MRI signal decay analysis in the liver and spleen. https://aapm.onlinelibrary.wiley.com/doi/abs/10.1118/1.4915495
- Taimouri et al (2014). Spatially Constrained Incoherent Motion (SCIM) Model Improves Quantitative Diffusion-Weighted MRI Analysis of Crohn’s Disease Patients. [https://link.springer.com/chapter/10.1007/978-3-642-41083-3_2](https://link.springer.com/chapter/10.1007/978-3-642-41083-3_2)


### Compatible machines at CRL: 
Currently, the build tools are compatible only with `yonat` machine at CRL 


### Dependencies
Quick screen of *working* ccmake compile dependencies and paths mounted on `yonat`: 

 CMAKE_BUILD_TYPE                 Debug                                                                                                                                                                           
 CMAKE_INSTALL_PREFIX             ../Release                                                                                                                                                                      
 GSL                              /home/ch155417/moti/gsl/build/lib/libgsl.a                                                                                                                                      
 GSLCBLAS                         /home/ch155417/moti/gsl/build/lib/libgslcblas.a                                                                                                                                 
 GSL_INCLUDE_DIR                  /home/ch155417/moti/gsl/build/include                                                                                                                                           
 ITKNLOPTOPTIMIZERS_INCLUDE_DIR   /home/ch169807/moti/itkNLOPTOptimizers/src                                                                                                                                      
 ITK_DIR                          /opt/x86_64/pkgs/itk/3.20.0/gcc-release/lib/InsightToolkit                                                                                                                      
 NLOPT                            /home/ch155417/moti/nlopt-2.2/bin/lib/libnlopt.a                                                                                                                                
 NLOPT_DIR                        /home/ch155417/moti/nlopt-2.2                                                                                                                                                   
 TCLAP_INCLUDE_DIR                /home/ch169807/Software/tclap-1.2.1/include                                                                                                                                     
 VTK_DIR                          /opt/x86_64/pkgs/vtk/current/lib/cmake/vtk-6.1                                                                                                                                  
 itkNLOPTOptimizersLib            /home/ch169807/moti/itkNLOPTOptimizers/build/libITKNLOPTOptimizers.a
