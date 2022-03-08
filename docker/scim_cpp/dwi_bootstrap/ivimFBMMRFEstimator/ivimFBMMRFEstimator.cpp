#include <iostream>
#include <sstream>
#include <string>
#include <tclap/CmdLine.h>

#include <itkVectorImage.h>
#include <itkBValuesToVectorImageFilter.h>
#include <itkIVIMModelFitFilter.h>
#include "itkIVIMBootstrapProposalFilter.h"
#include "itkIVIMMRFModelFitICMImageFilter.h"
#include "itkIVIMMRFModelFitFBMImageFilter.h"
#include "itkIVIMMRFMetricValue.h"
#include <itkVectorToScalarImageFilter.h>


#include "BvaluesParser.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
//#include "itkBValuesToADCImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"



const std::string ADCFileName = "/meanADC_";
const std::string PERFileName = "/meanPER_";
const std::string PER_FracFileName = "/meanPER_FRAC_";
const std::string B0_FileName = "/meanB0_";
const std::string ADCVarFileName = "/ADC_SE_";
const std::string PERVarFileName = "/PER_SE_";
const std::string PER_FracVarFileName = "/PER_FRAC_SE_";
const std::string B0Var_FileName = "/B0_SE_";
const std::string ADCVarLoFileName = "/ADC_SE_LO_";
const std::string PERVarLoFileName = "/PER_SE_LO_";
const std::string PER_FracVarLoFileName = "/PER_FRAC_SE_LO_";
const std::string B0VarLo_FileName = "/B0_SE_LO_";


//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.


int main(int argc, char *argv[])
{



	const unsigned int ImageDimension = 3;


    typedef itk::VectorImage <double, ImageDimension> itkVectorImageType;







	int bValuesNum = 6;
	std::string ImagesNamesFileName;
	std::string outputFolderFileName;
	std::string MaskImageFileName;
	std::string optMode;

	int Threshold = -1;

	int numOfIters = 0;
	bool BS;


	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");

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

		// Parse the argv array.
		cmd.parse( argc, argv );

		bValuesNum = BValuesNumArg.getValue();
		ImagesNamesFileName = ImagesNamesFileNameArg.getValue();
		outputFolderFileName = outputFolderFileNameArg.getValue();
		Threshold = ThresholdArg.getValue();
		MaskImageFileName = MaskImageFileNameArg.getValue();
		numOfIters = numOfGDItersArg.getValue();
		optMode = OptModeArg.getValue();
		BS = (bool) BSSTDArg.getValue();
	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	BvaluesParser parser (bValuesNum, ImagesNamesFileName);



	typedef itk::Image <short, ImageDimension> ImageType;
	typedef itk::Image <float, ImageDimension> RealImageType;
	typedef itk::ImageFileReader <ImageType> ImageFileReaderType;
	typedef itk::SmoothingRecursiveGaussianImageFilter <ImageType> itkSmoothingRecursiveGaussianImageFilterType;

	typedef itk::BValuesToVectorImageFilter <ImageType,itkVectorImageType> itkBValuesToVectorImageFilterType;

	itkBValuesToVectorImageFilterType::Pointer bvalues2vector = itkBValuesToVectorImageFilterType::New();

	crlBodyDiffusionTools::MeasureMentVectorType bValues (parser.getBValues().size());


	int bZeroIdx = 0;
	for (size_t i=0;i<parser.getBValues().size();++i)
	{
		bValues(i) = parser.getBValues()[i];
		if (bValues(i) < 10)
		{
			bZeroIdx = i;
		}
		//std::cout << bValues(i) << " ";
	}
	//std::cout << std::endl;


	bvalues2vector->SetBValues (bValues);


	for (int bIdx=0;bIdx<bValuesNum;++bIdx)
	{
		ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
		reader->SetFileName(parser.getBValueImageName(bIdx).c_str());

		try
		{
			reader->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
			std::cerr << excep << std::endl;
		}

		itkSmoothingRecursiveGaussianImageFilterType::Pointer smoother = itkSmoothingRecursiveGaussianImageFilterType::New();
		smoother->SetSigma (reader->GetOutput()->GetSpacing()[0]/2.0);
		smoother->SetInput (reader->GetOutput());
		smoother->Update();


		//std::cout << smoother->GetOutput()->GetPixel(index) << " ";
		bvalues2vector->SetInput (bIdx,smoother->GetOutput());
	}




	bvalues2vector->Update();



	typedef itk::IVIMModelFitFilter <itkVectorImageType,itkVectorImageType> itkIVIMModelFitType;
	itkIVIMModelFitType::Pointer modelFit = itkIVIMModelFitType::New();
	modelFit->SetInput (bvalues2vector->GetSamplesImage());
	modelFit->SetVariance (bvalues2vector->GetRicianNoiseParam());
	//std::cout << bvalues2vector->GetRicianNoiseParam() << std::endl;
	modelFit->SetBValues (bValues);
	modelFit->SetAlgMode (itkIVIMModelFitType::ML);

	modelFit->SetBSflag(BS);
	std::cout << std::endl;

	// TODO: add threshold part
	ImageType::Pointer maskImage = ImageType::New();

	if (Threshold==-1)
	{
		std::cout << "enter readin mask" << std::endl;
		ImageFileReaderType::Pointer maskReader = ImageFileReaderType::New();
		maskReader->SetFileName(MaskImageFileName.c_str());
		try
		{
			maskReader->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
			std::cerr << excep << std::endl;
		}
		maskImage = maskReader->GetOutput();
	}
	else if (Threshold !=-1)
	{
		std::cout << "enter threshold mask: " << Threshold << std::endl;
		std::cout << "base name: " << parser.getBValueImageName(bZeroIdx).c_str() << std::endl;
		ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
		reader->SetFileName(parser.getBValueImageName(bZeroIdx).c_str());
		try
		{
			reader->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
			std::cerr << excep << std::endl;
		}

		typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> itkBinaryThresholdImageFilterType;
		itkBinaryThresholdImageFilterType::Pointer thresh = itkBinaryThresholdImageFilterType::New();

	 	thresh->SetInput (reader->GetOutput()); //assume that the first is b0 image

		thresh->SetLowerThreshold(Threshold);
		thresh->SetOutsideValue(0);
		thresh->SetInsideValue(1);
		thresh->Update();
		maskImage = thresh->GetOutput();


	}

/*
	ImageFileReaderType::Pointer maskReader = ImageFileReaderType::New();
	maskReader->SetFileName(MaskImageFileName.c_str());
	try
	{
		maskReader->Update();
	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
		std::cerr << excep << std::endl;
	}
	*/
	modelFit->SetMaskImage (maskImage);




	// model initialization
	modelFit->Update();

	std::cout << "after IVIM fitting" << std::endl;
	itkVectorImageType::Pointer curModel=modelFit->GetIVIMModelImage();

	typedef itk::IVIMBootstrapProposalFilter <itkVectorImageType,itkVectorImageType> itkIVIMBootstrapProposalFilterType;
	itkIVIMBootstrapProposalFilterType::Pointer bsProposalFilter = itkIVIMBootstrapProposalFilterType::New();
	bsProposalFilter->SetInput (bvalues2vector->GetSamplesImage());
	bsProposalFilter->SetVariance (bvalues2vector->GetRicianNoiseParam());
	bsProposalFilter->SetBValues (bValues);
	bsProposalFilter->SetInitialIVIMModelImage (modelFit->GetIVIMModelImage());
	bsProposalFilter->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);
	bsProposalFilter->SetMaskImage (maskImage);

	typedef itk::IVIMMRFMetricValue <itkVectorImageType,itkVectorImageType> itkIVIMMRFMetricValueType;
	typedef itk::IVIMMRFModelFitICMImageFilter <itkVectorImageType,itkVectorImageType> itkIVIMMRFModelFitICMImageFilterType;
	typedef itk::IVIMMRFModelFitFBMImageFilter <itkVectorImageType,itkVectorImageType> itkIVIMMRFModelFitFBMImageFilterType;

	typedef itk::ImageFileWriter <RealImageType> ImageFileWriterType;
	for (int i=0;i<numOfIters;++i)
	{
/*
		typedef itk::VectorToScalarImageFilter <itkVectorImageType, RealImageType> itkVectorToScalarImageFilterType;
		itkVectorToScalarImageFilterType::Pointer vector2scalarInner = itkVectorToScalarImageFilterType::New();
		vector2scalarInner->SetInput (curModel);
		vector2scalarInner->Update();
		std::ostringstream ss;
		ss << i;
		std::string iterNum = ss.str();
		std::string outputPerFracImageFileNameInner = outputFolderFileName + PER_FracFileName + "_iter_" + iterNum + ".vtk";
		ImageFileWriterType::Pointer fracWriterInner = ImageFileWriterType::New();
		fracWriterInner->SetFileName(outputPerFracImageFileNameInner.c_str());
		fracWriterInner->SetInput (vector2scalarInner->GetfImage());

		try
			{
				fracWriterInner->Update();
			}
			catch( itk::ExceptionObject & excep )
			{
				std::cerr << "Exception catched !" << std::endl;
			    std::cerr << excep << std::endl;
			}

*/


		itkIVIMMRFMetricValueType::Pointer metricVal = itkIVIMMRFMetricValueType::New();
		metricVal->SetInput (curModel);
		metricVal->SetBValues (bValues);
		metricVal->SetVariance (bvalues2vector->GetRicianNoiseParam());
		metricVal->SetSignalImage (bvalues2vector->GetSamplesImage());
		metricVal->SetMaskImage (maskImage);
		metricVal->Update();
		std::cout << "iter " << i << ": " << metricVal->GetMetricValue() << std::endl;


		// the first is to initiate the filter
		bsProposalFilter->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);
		bsProposalFilter->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::FIXED_WEIGHTS);
		//bsProposalFilter->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);

		//bsProposalFilter->SetInitialIVIMModelImage (curModel);
		bsProposalFilter->Update();

		// mrf optimization
		if (optMode == "ICM")
		{
			itkIVIMMRFModelFitICMImageFilterType::Pointer mrfFitICM = itkIVIMMRFModelFitICMImageFilterType::New();
			mrfFitICM->SetInput (bvalues2vector->GetSamplesImage());
			mrfFitICM->SetVariance (bvalues2vector->GetRicianNoiseParam());
			mrfFitICM->SetBValues (bValues);
			mrfFitICM->SetMaskImage (maskImage);
			mrfFitICM->SetBaseIVIMModelImage (curModel);
			mrfFitICM->SetProposalIVIMModelImage (bsProposalFilter->GetBSIVIMProposalImage());
			mrfFitICM->Update();
			curModel=mrfFitICM->GetOutputIVIMModelImage();


		}
		else if (optMode == "FBM")
		{
			//TODO
			itkIVIMMRFModelFitFBMImageFilterType::Pointer mrfFitFBM = itkIVIMMRFModelFitFBMImageFilterType::New();
			mrfFitFBM->SetInput (bvalues2vector->GetSamplesImage());
			mrfFitFBM->SetVariance (bvalues2vector->GetRicianNoiseParam());
			mrfFitFBM->SetBValues (bValues);
			mrfFitFBM->SetMaskImage (maskImage);
			mrfFitFBM->SetBaseIVIMModelImage (curModel);
			mrfFitFBM->SetProposalIVIMModelImage (bsProposalFilter->GetBSIVIMProposalImage());
			mrfFitFBM->Update();
			curModel=mrfFitFBM->GetOutputIVIMModelImage();
		}
		else
		{
			std::cerr << "Unknown optimization mode. Use ICM or FBM only." << std::endl;
			exit (EXIT_FAILURE);
		}
	}





	itkIVIMMRFMetricValueType::Pointer finalMetricVal = itkIVIMMRFMetricValueType::New();
	finalMetricVal->SetInput (curModel);
	finalMetricVal->SetBValues (bValues);
	finalMetricVal->SetVariance (bvalues2vector->GetRicianNoiseParam());
	finalMetricVal->SetSignalImage (bvalues2vector->GetSamplesImage());
	finalMetricVal->SetMaskImage (maskImage);
	finalMetricVal->Update();
	std::cout << "final metric val: " << finalMetricVal->GetMetricValue() << std::endl;





	typedef itk::VectorToScalarImageFilter <itkVectorImageType, RealImageType> itkVectorToScalarImageFilterType;
	itkVectorToScalarImageFilterType::Pointer vector2scalar = itkVectorToScalarImageFilterType::New();
	vector2scalar->SetInput (curModel);
	vector2scalar->Update();












	std::string outputADCImageFileName = outputFolderFileName + ADCFileName + "1.vtk";
	std::string outputPERImageFileName = outputFolderFileName + PERFileName + "1.vtk";
	std::string outputPerFracImageFileName = outputFolderFileName + PER_FracFileName + "1.vtk";
	std::string outputB0_ImageFileName = outputFolderFileName + B0_FileName + "1.vtk";






	ImageFileWriterType::Pointer diffWriter = ImageFileWriterType::New();
	diffWriter->SetFileName(outputADCImageFileName.c_str());
	diffWriter->SetInput (vector2scalar->GetDImage());

	ImageFileWriterType::Pointer perWriter = ImageFileWriterType::New();
	perWriter->SetFileName(outputPERImageFileName.c_str());
	perWriter->SetInput (vector2scalar->GetDstarImage());

	ImageFileWriterType::Pointer fracWriter = ImageFileWriterType::New();
	fracWriter->SetFileName(outputPerFracImageFileName.c_str());
	fracWriter->SetInput (vector2scalar->GetfImage());

	ImageFileWriterType::Pointer b0Writer = ImageFileWriterType::New();
	b0Writer->SetFileName(outputB0_ImageFileName.c_str());
	b0Writer->SetInput (vector2scalar->Getb0Image());





	try
	{
		diffWriter->Update();
		perWriter->Update();
		fracWriter->Update();
		b0Writer->Update();




	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
	    std::cerr << excep << std::endl;
	}


	if (BS)
	{

		itkVectorToScalarImageFilterType::Pointer vector2scalarBS = itkVectorToScalarImageFilterType::New();
		vector2scalarBS->SetInput (modelFit->GetIVIMModelBSImage());
		vector2scalarBS->Update();


		std::string outputADCImageFileNameBS = outputFolderFileName + ADCFileName + "BS.vtk";
		std::string outputPERImageFileNameBS = outputFolderFileName + PERFileName + "BS.vtk";
		std::string outputPerFracImageFileNameBS = outputFolderFileName + PER_FracFileName + "BS.vtk";
		std::string outputB0_ImageFileNameBS = outputFolderFileName + B0_FileName + "BS.vtk";


		ImageFileWriterType::Pointer diffWriterBS = ImageFileWriterType::New();
		diffWriterBS->SetFileName(outputADCImageFileNameBS.c_str());
		diffWriterBS->SetInput (vector2scalarBS->GetDImage());

		ImageFileWriterType::Pointer perWriterBS = ImageFileWriterType::New();
		perWriterBS->SetFileName(outputPERImageFileNameBS.c_str());
		perWriterBS->SetInput (vector2scalarBS->GetDstarImage());

		ImageFileWriterType::Pointer fracWriterBS = ImageFileWriterType::New();
		fracWriterBS->SetFileName(outputPerFracImageFileNameBS.c_str());
		fracWriterBS->SetInput (vector2scalarBS->GetfImage());

		ImageFileWriterType::Pointer b0WriterBS = ImageFileWriterType::New();
		b0WriterBS->SetFileName(outputB0_ImageFileNameBS.c_str());
		b0WriterBS->SetInput (vector2scalarBS->Getb0Image());





		try
		{
			diffWriterBS->Update();
			perWriterBS->Update();
			fracWriterBS->Update();
			b0WriterBS->Update();


		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
		    std::cerr << excep << std::endl;
		}
	}



	return 0;
}
