#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

#include "BvaluesParser.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBValuesToADCImageFilter.h"
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


	int bValuesNum = 6;
	std::string ImagesNamesFileName;
	std::string MaskImageFileName;
	std::string outputFolderFileName;
	std::string expType;
	int bsScaling = 0;


	int Threshold  = 0;

	int numOfBSIters = 1000;

	int nCoils = 1;

	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");

		TCLAP::ValueArg<int> BValuesNumArg("n","bValuesNum","bValuesNum",false,6,"int");
		cmd.add( BValuesNumArg );

		TCLAP::ValueArg<int> ThresholdArg("t","threshold","threshold",false,0,"int");
		cmd.add( ThresholdArg );

		TCLAP::ValueArg<int> BSItersArg("","bsIters","bsIters",false,1000,"int");
		cmd.add( BSItersArg );

		TCLAP::ValueArg<int> nCoilsArg("","nCoils","nCoils",false,1,"int");
		cmd.add( nCoilsArg );

		TCLAP::ValueArg<std::string> ImagesNamesFileNameArg("i","ImagesNamesFileName","b_value images names filename",true,"","string");
		cmd.add( ImagesNamesFileNameArg );

		TCLAP::ValueArg<std::string> MaskImageFileNameArg("m","MaskImageFileName","MaskImageFileName",false,"","string");
		cmd.add( MaskImageFileNameArg );

		TCLAP::ValueArg<std::string> outputFolderFileNameArg("o","outputFolderFileName","Output gray scale image file name",true,"","string");
		cmd.add( outputFolderFileNameArg );


		TCLAP::ValueArg<std::string> ExpTypeArg("","expType","expType",true,"","string");
		cmd.add( ExpTypeArg );

		TCLAP::ValueArg<int> BSScalingArg("","bsScaling","bsScaling",true,0,"int");
		cmd.add( BSScalingArg );

		// Parse the argv array.
		cmd.parse( argc, argv );

		bValuesNum = BValuesNumArg.getValue();
		ImagesNamesFileName = ImagesNamesFileNameArg.getValue();
		MaskImageFileName = MaskImageFileNameArg.getValue();
		outputFolderFileName = outputFolderFileNameArg.getValue();
		Threshold = ThresholdArg.getValue();
		expType = ExpTypeArg.getValue();
		numOfBSIters = BSItersArg.getValue();
		bsScaling = BSScalingArg.getValue();
		nCoils = nCoilsArg.getValue();

		std::cout <<"expType: " << expType << std::endl;



	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	BvaluesParser parser (bValuesNum, ImagesNamesFileName);



	typedef itk::Image <short, 3> ImageType;
	typedef itk::Image <float, 3> RealImageType;
	typedef itk::ImageFileReader <ImageType> ImageFileReaderType;
	typedef itk::SmoothingRecursiveGaussianImageFilter <ImageType> itkSmoothingRecursiveGaussianImageFilterType;

	typedef itk::BValuesToADCImageFilter <ImageType,RealImageType> itkBValuesToADCImageFilterType;


	std::vector <ImageType::Pointer> bImages;


	itkBValuesToADCImageFilterType::Pointer modelFit = itkBValuesToADCImageFilterType::New();
	modelFit->SetNcoils (nCoils);
	if (expType=="LS")
	{
		//std::cout << "should set LS(0)" << std::endl;
		modelFit->SetAlgMode(itkBValuesToADCImageFilterType::LS);
		modelFit->SetComputeBootstrap(false);
	}
	else if (expType=="ML")
	{
		//std::cout << "should set ML(1)" << std::endl;
		modelFit->SetAlgMode(itkBValuesToADCImageFilterType::ML);
		modelFit->SetComputeBootstrap(false);
	}
	else if (expType=="SLS")
	{
		//std::cout << "should set EM(2)" << std::endl;
		modelFit->SetAlgMode(itkBValuesToADCImageFilterType::SLS);
		modelFit->SetComputeBootstrap(false);

	}
	else if (expType=="BS")
	{
		//std::cout << "should set LS(0)" << std::endl;
		modelFit->SetAlgMode(itkBValuesToADCImageFilterType::ML);
		modelFit->SetComputeBootstrap(true);
		modelFit->SetNumberOfBootstarpIterations( numOfBSIters);
		modelFit->SetBSscaling (bsScaling);
	}
	else
	{
		std::cerr << "error: " << "Undefined exp type." << std::endl;
		exit (EXIT_FAILURE);
	}



	itkBValuesToADCImageFilterType::MeasureMentVectorType bValues (parser.getBValues().size());
	itkBValuesToADCImageFilterType::MeasureMentVectorType varianceVec (parser.getBValues().size());

	for (size_t i=0;i<parser.getBValues().size();++i)
	{
		bValues(i) = parser.getBValues()[i];
		std::cout << bValues(i) << " ";
	}
	std::cout << std::endl;


	modelFit->SetBValues (bValues);

	/*
	ImageType::IndexType index;
	index[0] = 39;
	index[1] = 56;
	index[2] = 27;

	ImageType::SizeType size;
	size[0] = 1;
	size[1] = 1;
	size[2] = 1;

	ImageType::RegionType region;
	region.SetIndex(index);
	region.SetSize(size);
*/
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


		bImages.push_back(smoother->GetOutput());


		modelFit->SetInput (bIdx,bImages[bIdx]);
	}
	std::cout << std::endl;


	if (!MaskImageFileName.empty())
	{
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
		modelFit->SetMaskImage (maskReader->GetOutput());
	}
	else if (Threshold !=0)
	{
		typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> itkBinaryThresholdImageFilterType;
		itkBinaryThresholdImageFilterType::Pointer thresh = itkBinaryThresholdImageFilterType::New();

	 	thresh->SetInput (bImages[0]); //assume that the first is b0 image

		thresh->SetLowerThreshold(Threshold);
		thresh->SetOutsideValue(itkBValuesToADCImageFilterType::BACKGROUND);
		thresh->SetInsideValue(itkBValuesToADCImageFilterType::FOREGROUND);
		thresh->Update();
		modelFit->SetMaskImage (thresh->GetOutput());
	}

	modelFit->Update();


	std::string outputADCImageFileName = outputFolderFileName + ADCFileName;
	std::string outputPERImageFileName = outputFolderFileName + PERFileName;
	std::string outputPerFracImageFileName = outputFolderFileName + PER_FracFileName;
	std::string outputB0_ImageFileName = outputFolderFileName + B0_FileName;

	char numOfBSItersStr [255];
	sprintf (numOfBSItersStr, "%d", numOfBSIters);
	std::string outputADCVarImageFileName = outputFolderFileName + ADCVarFileName + numOfBSItersStr + ".vtk";
	std::string outputPERVarImageFileName = outputFolderFileName + PERVarFileName + numOfBSItersStr + ".vtk";
	std::string outputPerFracVarImageFileName = outputFolderFileName + PER_FracVarFileName + numOfBSItersStr + ".vtk";
	std::string outputB0Var_ImageFileName = outputFolderFileName + B0Var_FileName + numOfBSItersStr + ".vtk";

	std::string outputADCVarLoImageFileName = outputFolderFileName + ADCVarLoFileName + numOfBSItersStr + ".vtk";
	std::string outputPERVarLoImageFileName = outputFolderFileName + PERVarLoFileName + numOfBSItersStr + ".vtk";
	std::string outputPerFracVarLoImageFileName = outputFolderFileName + PER_FracVarLoFileName + numOfBSItersStr + ".vtk";
	std::string outputB0VarLo_ImageFileName = outputFolderFileName + B0VarLo_FileName + numOfBSItersStr + ".vtk";



	std::string outputMeanADCImageFileName = outputFolderFileName + ADCFileName + numOfBSItersStr + ".vtk";
	std::string outputMeanPERImageFileName = outputFolderFileName + PERFileName + numOfBSItersStr + ".vtk";
	std::string outputMeanPerFracImageFileName = outputFolderFileName + PER_FracFileName + numOfBSItersStr + ".vtk";
	std::string outputMeanB0_ImageFileName = outputFolderFileName + B0_FileName + numOfBSItersStr + ".vtk";

	typedef itk::ImageFileWriter <RealImageType> ImageFileWriterType;
	ImageFileWriterType::Pointer diffWriter = ImageFileWriterType::New();
	diffWriter->SetFileName(outputMeanADCImageFileName.c_str());
	diffWriter->SetInput (modelFit->GetDiffusionImage());

	ImageFileWriterType::Pointer perWriter = ImageFileWriterType::New();
	perWriter->SetFileName(outputMeanPERImageFileName.c_str());
	perWriter->SetInput (modelFit->GetPerfusionImage());

	ImageFileWriterType::Pointer fracWriter = ImageFileWriterType::New();
	fracWriter->SetFileName(outputMeanPerFracImageFileName.c_str());
	fracWriter->SetInput (modelFit->GetPerfusionFractionImage());

	ImageFileWriterType::Pointer b0Writer = ImageFileWriterType::New();
	b0Writer->SetFileName(outputMeanB0_ImageFileName.c_str());
	b0Writer->SetInput (modelFit->GetB0Image());



	typedef itk::ImageFileWriter <RealImageType> ImageFileWriterType;
	ImageFileWriterType::Pointer diffVarWriter = ImageFileWriterType::New();
	diffVarWriter->SetFileName(outputADCVarImageFileName.c_str());
	diffVarWriter->SetInput (modelFit->GetDiffusionVarImage());

	ImageFileWriterType::Pointer perVarWriter = ImageFileWriterType::New();
	perVarWriter->SetFileName(outputPERVarImageFileName.c_str());
	perVarWriter->SetInput (modelFit->GetPerfusionVarImage());

	ImageFileWriterType::Pointer fracVarWriter = ImageFileWriterType::New();
	fracVarWriter->SetFileName(outputPerFracVarImageFileName.c_str());
	fracVarWriter->SetInput (modelFit->GetPerfusionFractionVarImage());

	ImageFileWriterType::Pointer b0VarWriter = ImageFileWriterType::New();
	b0VarWriter->SetFileName(outputB0Var_ImageFileName.c_str());
	b0VarWriter->SetInput (modelFit->GetB0VarImage());



	ImageFileWriterType::Pointer diffVarLoWriter = ImageFileWriterType::New();
	diffVarLoWriter->SetFileName(outputADCVarLoImageFileName.c_str());
	diffVarLoWriter->SetInput (modelFit->GetDiffusionVarLoImage());

	ImageFileWriterType::Pointer perVarLoWriter = ImageFileWriterType::New();
	perVarLoWriter->SetFileName(outputPERVarLoImageFileName.c_str());
	perVarLoWriter->SetInput (modelFit->GetPerfusionVarLoImage());

	ImageFileWriterType::Pointer fracVarLoWriter = ImageFileWriterType::New();
	fracVarLoWriter->SetFileName(outputPerFracVarLoImageFileName.c_str());
	fracVarLoWriter->SetInput (modelFit->GetPerfusionFractionVarLoImage());

	ImageFileWriterType::Pointer b0VarLoWriter = ImageFileWriterType::New();
	b0VarLoWriter->SetFileName(outputB0VarLo_ImageFileName.c_str());
	b0VarLoWriter->SetInput (modelFit->GetB0VarLoImage());



	try
	{
		diffWriter->Update();
		perWriter->Update();
		fracWriter->Update();
		b0Writer->Update();

		diffVarWriter->Update();
		perVarWriter->Update();
		fracVarWriter->Update();
		b0VarWriter->Update();

		diffVarLoWriter->Update();
		perVarLoWriter->Update();
		fracVarLoWriter->Update();
		b0VarLoWriter->Update();


	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
	    std::cerr << excep << std::endl;
	}




	return 0;
}
