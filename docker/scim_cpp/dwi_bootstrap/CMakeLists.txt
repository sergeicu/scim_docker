PROJECT (DWI_BOOTSTRAP)
SET (PROJECT_HOME ..)



cmake_minimum_required (VERSION 2.8) 

#---------------
#
#For using ITK  you need to add

if(NOT CMAKE_BUILD_TYPE)
  #set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE) 

set(CMAKE_INSTALL_PREFIX "${PROJECT_HOME}/Release" CACHE STRING "CMAKE_INSTALL_PREFIX: Install path prefix, prepended onto install directories." FORCE)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_HOME}/bin)


#include_directories( ~/include/;${PROJECT_HOME}/src/itkNLOPTOptimizers;${PROJECT_HOME}/include)

include_directories( ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include) 

#SET(CMAKE_BUILD_TYPE "Debug")
 

#ADD_SUBDIRECTORY ( ${PROJECT_HOME}/src/itkNLOPTOptimizers )

  

FIND_LIBRARY(GSL gsl ~/lib/)
FIND_LIBRARY(GSLCBLAS gslcblas ~/lib/)
FIND_LIBRARY(itkNLOPTOptimizersLib ITKNLOPTOptimizers ~/lib/)
FIND_LIBRARY(NLOPT nlopt ~/lib)


### itkNLOPTOptimizers
#SET(itkNLOPTOptimizers_INCLUDE_DIR /home/ch155417/moti/itkNLOPTOptimizers/src)
FIND_PATH(ITKNLOPTOPTIMIZERS_INCLUDE_DIR itkNLOPTOptimizers.h)
INCLUDE_DIRECTORIES(${ITKNLOPTOPTIMIZERS_INCLUDE_DIR})

FIND_PATH(GSL_INCLUDE_DIR gsl_include_dir)
INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIR})

FIND_PATH(NLOPT_DIR nlopt_dir)
INCLUDE_DIRECTORIES(${NLOPT_DIR} ${NLOPT_DIR}/util ${NLOPT_DIR}/api)

#SET(GSL_INCLUDE_DIR /home/ch169807/moti/gsl/build/include)
#INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIR})

#SET(NLOPT_INCLUDE_DIR /home/ch169807/moti/nlopt-2.2)
#INCLUDE_DIRECTORIES(${NLOPT_INCLUDE_DIR} ${NLOPT_INCLUDE_DIR}/util ${NLOPT_INCLUDE_DIR}/api)

FIND_PATH(TCLAP_INCLUDE_DIR tclap/CmdLine.h)
INCLUDE_DIRECTORIES(${TCLAP_INCLUDE_DIR})
####










FIND_PACKAGE(ITK)
IF(ITK_FOUND)
   INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
   MESSAGE(FATAL_ERROR
           "Cannot build InsightApplications without ITK.  Please set ITK_DIR.")
ENDIF(ITK_FOUND)

#---------------


#For using VTK you need to add

FIND_PACKAGE(VTK)
IF (VTK_FOUND)
  INCLUDE (${VTK_USE_FILE})
ELSE(VTK_FOUND)
   MESSAGE(FATAL_ERROR
          "Cannot build InsightApplications without VTK.  Please set VTK_DIR.")
ENDIF(VTK_FOUND)







SET (ITK_LIBS
     ITKBasicFilters
     ITKAlgorithms
     ITKCommon
     ITKIO
     ITKNumerics
     ITKNrrdIO
     itkpng
     itktiff
     itkgdcm
     itkjpeg8
     itkjpeg12
     itkjpeg16
     ITKMetaIO
     itkzlib
     ITKDICOMParser
     ITKEXPAT
     itkvnl_inst
     itkvnl_algo
     itkvnl
     itkvcl
     itksys
)



SET ( VTK_LIBS vtkalglib vtkCommon vtkDICOMParser
               vtkexoIIc vtkexpat vtkFiltering vtkfreetype 
               vtkftgl vtkGenericFiltering vtkGraphics
               vtkRendering vtksqlite vtksys vtktiff vtkverdict
               vtkVolumeRendering vtkWidgets vtkzlib
               vtkHybrid vtkImaging vtkIO vtkjpeg
               vtkNetCDF vtkpng vtkmetaio
)  



############### convert NRRD2VTK ###########################3

SET(convertNRRD2VTK_SRCS
  ./convertNRRD2VTK/convertNRRD2VTK.cpp
  
)

SET( convertNRRD2VTK_LIB
     convertNRRD2VTK
     ${ITK_LIBS}
)

ADD_EXECUTABLE(convertNRRD2VTK ${convertNRRD2VTK_SRCS} )
TARGET_LINK_LIBRARIES( ${convertNRRD2VTK_LIB})

SET(OutPuts ${convertNRRD2VTK})

############### modelFitTest ###########################3

SET(modelFitTest_SRCS
  ./modelFitTest/modelFitTest.cpp
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
 
 
  
  
)

SET( modelFitTest_LIB
     modelFitTest
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(modelFitTest ${modelFitTest_SRCS} )
TARGET_LINK_LIBRARIES( ${modelFitTest_LIB})

SET(OutPuts ${OutPuts}  ${modelFitTest})


############### computeADC ###########################3

SET(computeADC_SRCS
  ./computeADC/computeADC.cpp
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
 
  
  
)

SET( computeADC_LIB 
     computeADC
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(computeADC ${computeADC_SRCS} )
TARGET_LINK_LIBRARIES( ${computeADC_LIB})




############### computeADC ###########################3

SET(computeTRACEW_SRCS
  ./computeTRACEW/computeTRACEW.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 

 
  
  
)
 
SET( computeTRACEW_LIB
     computeTRACEW
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
     
)

ADD_EXECUTABLE(computeTRACEW ${computeTRACEW_SRCS} )
TARGET_LINK_LIBRARIES( ${computeTRACEW_LIB})





############### computeADC ###########################3

SET(averageBVals_SRCS
  ./averageBVals/averageBvals.cpp
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
  
)

SET( averageBVals_LIB
     averageBVals
     ${GSL}
     ${GSLCBLAS}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(averageBVals ${averageBVals_SRCS} )
TARGET_LINK_LIBRARIES( ${averageBVals_LIB})


############### rescaleRealToShort ###########################3

SET(rescaleRealToShort_SRCS
  ./rescaleRealToShort/itkRescaleRealToShort.cxx
  
)

SET( rescaleRealToShort_LIB
     rescaleRealToShort
     ${ITK_LIBS}
)

ADD_EXECUTABLE(rescaleRealToShort ${rescaleRealToShort_SRCS} )
TARGET_LINK_LIBRARIES( ${rescaleRealToShort_LIB})


############### plotFittedModelForPoint ###########################3

SET(plotFittedModelForPoint_SRCS
  ./plotFittedModelForPoint/plotFittedModelForPoint.cpp
  ./plotFittedModelForPoint/BvaluesParser.h
  ./plotFittedModelForPoint/BvaluesParser.cpp
 
  
  
)

SET( plotFittedModelForPoint_LIB
     plotFittedModelForPoint
     ${ITK_LIBS}
)

ADD_EXECUTABLE(plotFittedModelForPoint ${plotFittedModelForPoint_SRCS} )
TARGET_LINK_LIBRARIES( ${plotFittedModelForPoint_LIB})





############### simulateDWI ###########################3

SET(simulateDWI_SRCS
  ./simulateDWI/simulateDWI.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
 
  
  
)

SET( simulateDWI_LIB
     simulateDWI
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(simulateDWI ${simulateDWI_SRCS} )
TARGET_LINK_LIBRARIES( ${simulateDWI_LIB})





############### simulateBS ###########################3

SET(simulateBS_SRCS
  ./simulateBS/simulateBS.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
 
  
  
)

SET( simulateBS_LIB 
     simulateBS
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(simulateBS ${simulateBS_SRCS} )
TARGET_LINK_LIBRARIES( ${simulateBS_LIB})





############### invivoBS ###########################3

SET(invivoBS_SRCS
  ./invivoBS/invivoBS.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
 
  
  
)

SET( invivoBS_LIB
     invivoBS
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(invivoBS ${invivoBS_SRCS} )
TARGET_LINK_LIBRARIES( ${invivoBS_LIB})




############### modelFitToSamples ###########################3

SET(modelFitToSamples_SRCS
  ./modelFitToSamples/modelFitToSamples
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
 
 
  
  
)

SET( modelFitToSamples_LIB
     modelFitToSamples
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(modelFitToSamples ${modelFitToSamples_SRCS} )
TARGET_LINK_LIBRARIES( ${modelFitToSamples_LIB})






############### ivimMRFEstimator ###########################3

SET(ivimMRFEstimator_SRCS
  ./ivimMRFEstimator/ivimMRFEstimator.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
 
 
  
  
)

SET( ivimMRFEstimator_LIB
     ivimMRFEstimator
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(ivimMRFEstimator ${ivimMRFEstimator_SRCS} )
TARGET_LINK_LIBRARIES( ${ivimMRFEstimator_LIB})

############### ivimFBMMRFEstimator ###########################3

SET(ivimFBMMRFEstimator_SRCS
  ./ivimFBMMRFEstimator/ivimFBMMRFEstimator.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
  ./include/QPBO/QPBO.cpp
  ./include/QPBO/QPBO_extra.cpp
  ./include/QPBO/QPBO_maxflow.cpp
  ./include/QPBO/QPBO_postprocessing.cpp
 
 
  
  
)

SET( ivimFBMMRFEstimator_LIB
     ivimFBMMRFEstimator
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(ivimFBMMRFEstimator ${ivimFBMMRFEstimator_SRCS} )
TARGET_LINK_LIBRARIES( ${ivimFBMMRFEstimator_LIB})


############### ivimFBMinvivoSE ###########################3

SET(ivimFBMinvivoSE_SRCS
  ./ivimFBMinvivoSE/ivimFBMinvivoSE.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
  ./include/QPBO/QPBO.cpp
  ./include/QPBO/QPBO_extra.cpp
  ./include/QPBO/QPBO_maxflow.cpp
  ./include/QPBO/QPBO_postprocessing.cpp
 
 
  
  
)

SET( ivimFBMinvivoSE_LIB
     ivimFBMinvivoSE
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(ivimFBMinvivoSE ${ivimFBMinvivoSE_SRCS} )
TARGET_LINK_LIBRARIES( ${ivimFBMinvivoSE_LIB})




############### tmpTest ###########################3

SET(tmpTest_SRCS
  ./tmpTest/tmpTest.cpp
  ./include/QPBO/QPBO.cpp
  ./include/QPBO/QPBO_extra.cpp
  ./include/QPBO/QPBO_maxflow.cpp
  ./include/QPBO/QPBO_postprocessing.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
 
 
  
  
)






############### MCV model selection ###########################3

SET(MCV_MODEL_SELECTION_SRCS
  ./MCV_model_selection/mvcModelSelection.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
  
)

SET( MCV_MODEL_SELECTION_LIB
     mvcModelSelection
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(mvcModelSelection ${MCV_MODEL_SELECTION_SRCS} )
TARGET_LINK_LIBRARIES( ${MCV_MODEL_SELECTION_LIB})











SET( tmpTest_LIB
     tmpTest
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(tmpTest ${tmpTest_SRCS} )
TARGET_LINK_LIBRARIES( ${tmpTest_LIB})




############### ivimCICMMRFEstimator ###########################3

SET(ivimCICMMRFEstimator_SRCS
  ./ivimCICMMRFEstimator/ivimCICMMRFEstimator.cpp
  ./include/crlBodyDiffusionTools.h
  ./include/crlBodyDiffusionTools.cpp 
  ./include/BvaluesParser.h
  ./include/BvaluesParser.cpp
  
 
  
  
)

SET( ivimCICMMRFEstimator_LIB
     ivimCICMMRFEstimator
     ${GSL}
     ${GSLCBLAS}
     ${itkNLOPTOptimizersLib}
     ${NLOPT}
     ${ITK_LIBS}
)

ADD_EXECUTABLE(ivimCICMMRFEstimator ${ivimCICMMRFEstimator_SRCS} )
TARGET_LINK_LIBRARIES( ${ivimCICMMRFEstimator_LIB})



#################INSTALL RULES#########################

INSTALL( TARGETS ${OutPuts}
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib/static)




