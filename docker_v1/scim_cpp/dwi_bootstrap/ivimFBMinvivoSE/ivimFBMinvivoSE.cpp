#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

#include <itkVectorImage.h>
#include <itkBValuesToVectorImageFilter.h>
#include <itkIVIMModelFitFilter.h>
#include "itkIVIMBootstrapProposalFilter.h"
#include "itkIVIMBootstrapSignalFilter.h"
#include "itkIVIMMRFModelFitICMImageFilter.h"
#include "itkIVIMMRFModelFitFBMImageFilter.h"
#include "itkIVIMMRFMetricValue.h"
#include <itkVectorToScalarImageFilter.h>
#include "itkIVIMMRFModelFitCICMImageFilter.h"

#include "BvaluesParser.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
//#include "itkBValuesToADCImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"


const std::string ICMCADCFileName = "/icmcADC_";
const std::string ICMCPERFileName = "/icmcPER_";
const std::string ICMCPER_FracFileName = "/icmcPER_FRAC_";
const std::string ICMCB0_FileName = "/icmcB0_";

const std::string IVIMADCFileName = "/ivimADC_";
const std::string IVIMPERFileName = "/ivimPER_";
const std::string IVIMPER_FracFileName = "/ivimPER_FRAC_";
const std::string IVIMB0_FileName = "/ivimB0_";

const std::string FBMADCFileName = "/fbmADC_";
const std::string FBMPERFileName = "/fbmPER_";
const std::string FBMPER_FracFileName = "/fbmPER_FRAC_";
const std::string FBMB0_FileName = "/fbmB0_";

const std::string ICMDADCFileName = "/icmdADC_";
const std::string ICMDPERFileName = "/icmdPER_";
const std::string ICMDPER_FracFileName = "/icmdPER_FRAC_";
const std::string ICMDB0_FileName = "/icmdB0_";


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


	int Threshold = -1;





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


		// Parse the argv array.
		cmd.parse( argc, argv );

		bValuesNum = BValuesNumArg.getValue();
		ImagesNamesFileName = ImagesNamesFileNameArg.getValue();
		outputFolderFileName = outputFolderFileNameArg.getValue();
		Threshold = ThresholdArg.getValue();
		MaskImageFileName = MaskImageFileNameArg.getValue();

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

		/*itkSmoothingRecursiveGaussianImageFilterType::Pointer smoother = itkSmoothingRecursiveGaussianImageFilterType::New();
		smoother->SetSigma (reader->GetOutput()->GetSpacing()[0]/2.0);
		smoother->SetInput (reader->GetOutput());
		smoother->Update();
*/

		//std::cout << smoother->GetOutput()->GetPixel(index) << " ";
		bvalues2vector->SetInput (bIdx,reader->GetOutput());
	}




	bvalues2vector->Update();



	typedef itk::IVIMModelFitFilter <itkVectorImageType,itkVectorImageType> itkIVIMModelFitType;
	itkIVIMModelFitType::Pointer modelFit = itkIVIMModelFitType::New();
	modelFit->SetInput (bvalues2vector->GetSamplesImage());
	modelFit->SetVariance (bvalues2vector->GetRicianNoiseParam());
	//std::cout << bvalues2vector->GetRicianNoiseParam() << std::endl;
	modelFit->SetBValues (bValues);
	modelFit->SetAlgMode (itkIVIMModelFitType::ML);

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
	itkVectorImageType::Pointer baseModel=modelFit->GetIVIMModelImage();

	typedef itk::VectorToScalarImageFilter <itkVectorImageType, RealImageType> itkVectorToScalarImageFilterType;

	typedef itk::IVIMBootstrapSignalFilter <itkVectorImageType,itkVectorImageType> itkIVIMBootstrapSignalFilterType;
	itkIVIMBootstrapSignalFilterType::Pointer bsSignalFilter = itkIVIMBootstrapSignalFilterType::New();
	bsSignalFilter->SetInput (bvalues2vector->GetSamplesImage());
	bsSignalFilter->SetVariance (bvalues2vector->GetRicianNoiseParam());
	bsSignalFilter->SetBValues (bValues);
	bsSignalFilter->SetInitialIVIMModelImage (baseModel);
	bsSignalFilter->SetBootstrapMode (itkIVIMBootstrapSignalFilterType::UNWEIGHTED);
	bsSignalFilter->SetMaskImage (maskImage);

	for (int bsIter=0;bsIter<100;++bsIter)//400
	{


		// the first is to initiate the filter
		bsSignalFilter->SetBootstrapMode (itkIVIMBootstrapSignalFilterType::UNWEIGHTED);
		bsSignalFilter->SetBootstrapMode (itkIVIMBootstrapSignalFilterType::FIXED_WEIGHTS);
		bsSignalFilter->SetBootstrapMode (itkIVIMBootstrapSignalFilterType::UNWEIGHTED);

		//bsProposalFilter->SetInitialIVIMModelImage (curModel);
		bsSignalFilter->Update();



		//IVIM estimate
		std::cout << "bsIter: " << bsIter << std::endl;

		itkIVIMModelFitType::Pointer bs_modelFit = itkIVIMModelFitType::New();
		bs_modelFit->SetInput (bsSignalFilter->GetBSSignalProposalImage());
		bs_modelFit->SetVariance (bvalues2vector->GetRicianNoiseParam());
		bs_modelFit->SetBValues (bValues);
		bs_modelFit->SetAlgMode (itkIVIMModelFitType::ML);
		bs_modelFit->SetMaskImage (maskImage);
		bs_modelFit->Update();

		itkVectorImageType::Pointer bs_ivimModel=bs_modelFit->GetIVIMModelImage();



		itkVectorToScalarImageFilterType::Pointer vector2scalar = itkVectorToScalarImageFilterType::New();
		vector2scalar->SetInput (bs_ivimModel);
		vector2scalar->Update();


		char bsIdxStr [255];
		sprintf (bsIdxStr,"%d",bsIter);

		std::string outputIVIMADCImageFileName = outputFolderFileName + IVIMADCFileName + bsIdxStr + ".vtk";
		std::string outputIVIMPERImageFileName = outputFolderFileName + IVIMPERFileName + bsIdxStr + ".vtk";
		std::string outputIVIMPerFracImageFileName = outputFolderFileName + IVIMPER_FracFileName + bsIdxStr + ".vtk";
		std::string outputIVIMB0_ImageFileName = outputFolderFileName + IVIMB0_FileName + bsIdxStr + ".vtk";


		typedef itk::ImageFileWriter <RealImageType> ImageFileWriterType;
		ImageFileWriterType::Pointer IVIMdiffWriter = ImageFileWriterType::New();
		IVIMdiffWriter->SetFileName(outputIVIMADCImageFileName.c_str());
		IVIMdiffWriter->SetInput (vector2scalar->GetDImage());

		ImageFileWriterType::Pointer IVIMperWriter = ImageFileWriterType::New();
		IVIMperWriter->SetFileName(outputIVIMPERImageFileName.c_str());
		IVIMperWriter->SetInput (vector2scalar->GetDstarImage());

		ImageFileWriterType::Pointer IVIMfracWriter = ImageFileWriterType::New();
		IVIMfracWriter->SetFileName(outputIVIMPerFracImageFileName.c_str());
		IVIMfracWriter->SetInput (vector2scalar->GetfImage());

		ImageFileWriterType::Pointer IVIMb0Writer = ImageFileWriterType::New();
		IVIMb0Writer->SetFileName(outputIVIMB0_ImageFileName.c_str());
		IVIMb0Writer->SetInput (vector2scalar->Getb0Image());



		try
		{
			IVIMdiffWriter->Update();
			IVIMperWriter->Update();
			IVIMfracWriter->Update();
			IVIMb0Writer->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
		    std::cerr << excep << std::endl;
		}

		std::cout << "*************** end IVIM **************" << std::endl;
		int numOfIters=250;

/*
		itkVectorImageType::Pointer icmc_curModel=bs_ivimModel;

		typedef itk::IVIMMRFModelFitCICMImageFilter <itkVectorImageType,itkVectorImageType> itkIVIMMRFModelFitCICMImageFilterType;
		typedef itk::IVIMMRFMetricValue <itkVectorImageType,itkVectorImageType> itkIVIMMRFMetricValueType;

		int numOfIters=250;

		for (int i=0;i<numOfIters;++i)
		{
			itkIVIMMRFModelFitCICMImageFilterType::Pointer mrfFitCICM = itkIVIMMRFModelFitCICMImageFilterType::New();
			mrfFitCICM->SetInput (bvalues2vector->GetSamplesImage());
			mrfFitCICM->SetVariance (bvalues2vector->GetRicianNoiseParam());
			mrfFitCICM->SetBValues (bValues);
			mrfFitCICM->SetMaskImage (maskImage);
			mrfFitCICM->SetBaseIVIMModelImage (icmc_curModel);
			mrfFitCICM->Update();

			icmc_curModel = mrfFitCICM->GetOutputIVIMModelImage();
		}

		itkVectorToScalarImageFilterType::Pointer vector2scalar_icmc = itkVectorToScalarImageFilterType::New();
		vector2scalar_icmc->SetInput (icmc_curModel);
		vector2scalar_icmc->Update();


		std::string outputICMCADCImageFileName = outputFolderFileName + ICMCADCFileName + bsIdxStr + ".vtk";
		std::string outputICMCPERImageFileName = outputFolderFileName + ICMCPERFileName + bsIdxStr + ".vtk";
		std::string outputICMCPerFracImageFileName = outputFolderFileName + ICMCPER_FracFileName + bsIdxStr + ".vtk";
		std::string outputICMCB0_ImageFileName = outputFolderFileName + ICMCB0_FileName + bsIdxStr + ".vtk";


		ImageFileWriterType::Pointer ICMCdiffWriter = ImageFileWriterType::New();
		ICMCdiffWriter->SetFileName(outputICMCADCImageFileName.c_str());
		ICMCdiffWriter->SetInput (vector2scalar_icmc->GetDImage());

		ImageFileWriterType::Pointer ICMCperWriter = ImageFileWriterType::New();
		ICMCperWriter->SetFileName(outputICMCPERImageFileName.c_str());
		ICMCperWriter->SetInput (vector2scalar_icmc->GetDstarImage());

		ImageFileWriterType::Pointer ICMCfracWriter = ImageFileWriterType::New();
		ICMCfracWriter->SetFileName(outputICMCPerFracImageFileName.c_str());
		ICMCfracWriter->SetInput (vector2scalar_icmc->GetfImage());

		ImageFileWriterType::Pointer ICMCb0Writer = ImageFileWriterType::New();
		ICMCb0Writer->SetFileName(outputICMCB0_ImageFileName.c_str());
		ICMCb0Writer->SetInput (vector2scalar_icmc->Getb0Image());


		try
		{
			ICMCdiffWriter->Update();
			ICMCperWriter->Update();
			ICMCfracWriter->Update();
			ICMCb0Writer->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
		    std::cerr << excep << std::endl;
		}

		std::cout << "*************** end ICMC **************" << std::endl;
*/











		//start FBM and

		typedef itk::IVIMMRFMetricValue <itkVectorImageType,itkVectorImageType> itkIVIMMRFMetricValueType;
		typedef itk::IVIMMRFModelFitICMImageFilter <itkVectorImageType,itkVectorImageType> itkIVIMMRFModelFitICMImageFilterType;
		typedef itk::IVIMMRFModelFitFBMImageFilter <itkVectorImageType,itkVectorImageType> itkIVIMMRFModelFitFBMImageFilterType;


		typedef itk::IVIMBootstrapProposalFilter <itkVectorImageType,itkVectorImageType> itkIVIMBootstrapProposalFilterType;
		itkIVIMBootstrapProposalFilterType::Pointer bsProposalFilterInnerFBM = itkIVIMBootstrapProposalFilterType::New();
		bsProposalFilterInnerFBM->SetInput (bsSignalFilter->GetBSSignalProposalImage());
		bsProposalFilterInnerFBM->SetVariance (bvalues2vector->GetRicianNoiseParam());
		bsProposalFilterInnerFBM->SetBValues (bValues);
		bsProposalFilterInnerFBM->SetInitialIVIMModelImage (bs_ivimModel);
		bsProposalFilterInnerFBM->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);
		bsProposalFilterInnerFBM->SetMaskImage (maskImage);

		itkVectorImageType::Pointer curModelFBM=bs_ivimModel;


		for (int i=0;i<numOfIters;++i)
		{



			// the first is to initiate the filter
			bsProposalFilterInnerFBM->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);
			bsProposalFilterInnerFBM->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::FIXED_WEIGHTS);
			bsProposalFilterInnerFBM->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);

			bsProposalFilterInnerFBM->SetInitialIVIMModelImage (curModelFBM);
			bsProposalFilterInnerFBM->Update();



			itkIVIMMRFModelFitFBMImageFilterType::Pointer mrfFitFBM = itkIVIMMRFModelFitFBMImageFilterType::New();
			mrfFitFBM->SetInput (bsSignalFilter->GetBSSignalProposalImage());
			mrfFitFBM->SetVariance (bvalues2vector->GetRicianNoiseParam());
			mrfFitFBM->SetBValues (bValues);
			mrfFitFBM->SetMaskImage (maskImage);
			mrfFitFBM->SetBaseIVIMModelImage (curModelFBM);
			mrfFitFBM->SetProposalIVIMModelImage (bsProposalFilterInnerFBM->GetBSIVIMProposalImage());
			mrfFitFBM->Update();
			curModelFBM=mrfFitFBM->GetOutputIVIMModelImage();





		}





		typedef itk::VectorToScalarImageFilter <itkVectorImageType, RealImageType> itkVectorToScalarImageFilterType;
		itkVectorToScalarImageFilterType::Pointer FBM_vector2scalar = itkVectorToScalarImageFilterType::New();
		FBM_vector2scalar->SetInput (curModelFBM);
		FBM_vector2scalar->Update();


		std::string outputFBMADCImageFileName = outputFolderFileName + FBMADCFileName + bsIdxStr + ".vtk";
		std::string outputFBMPERImageFileName = outputFolderFileName + FBMPERFileName + bsIdxStr + ".vtk";
		std::string outputFBMPerFracImageFileName = outputFolderFileName + FBMPER_FracFileName + bsIdxStr + ".vtk";
		std::string outputFBMB0_ImageFileName = outputFolderFileName + FBMB0_FileName + bsIdxStr + ".vtk";


		typedef itk::ImageFileWriter <RealImageType> ImageFileWriterType;
		ImageFileWriterType::Pointer FBMdiffWriter = ImageFileWriterType::New();
		FBMdiffWriter->SetFileName(outputFBMADCImageFileName.c_str());
		FBMdiffWriter->SetInput (FBM_vector2scalar->GetDImage());

		ImageFileWriterType::Pointer FBMperWriter = ImageFileWriterType::New();
		FBMperWriter->SetFileName(outputFBMPERImageFileName.c_str());
		FBMperWriter->SetInput (FBM_vector2scalar->GetDstarImage());

		ImageFileWriterType::Pointer FBMfracWriter = ImageFileWriterType::New();
		FBMfracWriter->SetFileName(outputFBMPerFracImageFileName.c_str());
		FBMfracWriter->SetInput (FBM_vector2scalar->GetfImage());

		ImageFileWriterType::Pointer FBMb0Writer = ImageFileWriterType::New();
		FBMb0Writer->SetFileName(outputFBMB0_ImageFileName.c_str());
		FBMb0Writer->SetInput (FBM_vector2scalar->Getb0Image());



		try
		{
			FBMdiffWriter->Update();
			FBMperWriter->Update();
			FBMfracWriter->Update();
			FBMb0Writer->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
		    std::cerr << excep << std::endl;
		}



		std::cout << "*************** end FBM **************" << std::endl;


/*
		itkVectorImageType::Pointer curModelICMD=bs_ivimModel;
		itkIVIMBootstrapProposalFilterType::Pointer bsProposalFilterInnerICMD = itkIVIMBootstrapProposalFilterType::New();
		bsProposalFilterInnerICMD->SetInput (bsSignalFilter->GetBSSignalProposalImage());
		bsProposalFilterInnerICMD->SetVariance (bvalues2vector->GetRicianNoiseParam());
		bsProposalFilterInnerICMD->SetBValues (bValues);
		bsProposalFilterInnerICMD->SetInitialIVIMModelImage (bs_ivimModel);
		bsProposalFilterInnerICMD->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);
		bsProposalFilterInnerICMD->SetMaskImage (maskImage);


		for (int i=0;i<numOfIters;++i)
		{

			// the first is to initiate the filter
			bsProposalFilterInnerICMD->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);
			bsProposalFilterInnerICMD->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::FIXED_WEIGHTS);
			bsProposalFilterInnerICMD->SetBootstrapMode (itkIVIMBootstrapProposalFilterType::UNWEIGHTED);
			bsProposalFilterInnerICMD->SetInitialIVIMModelImage (curModelICMD);
			bsProposalFilterInnerICMD->Update();


			itkIVIMMRFModelFitICMImageFilterType::Pointer mrfFitICM = itkIVIMMRFModelFitICMImageFilterType::New();
			mrfFitICM->SetInput (bvalues2vector->GetSamplesImage());
			mrfFitICM->SetVariance (bvalues2vector->GetRicianNoiseParam());
			mrfFitICM->SetBValues (bValues);
			mrfFitICM->SetMaskImage (maskImage);
			mrfFitICM->SetBaseIVIMModelImage (curModelICMD);
			mrfFitICM->SetProposalIVIMModelImage (bsProposalFilterInnerICMD->GetBSIVIMProposalImage());
			mrfFitICM->Update();
			curModelICMD=mrfFitICM->GetOutputIVIMModelImage();
		}

		itkVectorToScalarImageFilterType::Pointer 	ICMD_vector2scalar = itkVectorToScalarImageFilterType::New();
		ICMD_vector2scalar->SetInput (curModelICMD);
		ICMD_vector2scalar->Update();


		std::string outputICMDADCImageFileName = outputFolderFileName + ICMDADCFileName + bsIdxStr + ".vtk";
		std::string outputICMDPERImageFileName = outputFolderFileName + ICMDPERFileName + bsIdxStr + ".vtk";
		std::string outputICMDPerFracImageFileName = outputFolderFileName + ICMDPER_FracFileName + bsIdxStr + ".vtk";
		std::string outputICMDB0_ImageFileName = outputFolderFileName + ICMDB0_FileName + bsIdxStr + ".vtk";


		ImageFileWriterType::Pointer ICMDdiffWriter = ImageFileWriterType::New();
		ICMDdiffWriter->SetFileName(outputICMDADCImageFileName.c_str());
		ICMDdiffWriter->SetInput (ICMD_vector2scalar->GetDImage());

		ImageFileWriterType::Pointer ICMDperWriter = ImageFileWriterType::New();
		ICMDperWriter->SetFileName(outputICMDPERImageFileName.c_str());
		ICMDperWriter->SetInput (ICMD_vector2scalar->GetDstarImage());

		ImageFileWriterType::Pointer ICMDfracWriter = ImageFileWriterType::New();
		ICMDfracWriter->SetFileName(outputICMDPerFracImageFileName.c_str());
		ICMDfracWriter->SetInput (ICMD_vector2scalar->GetfImage());

		ImageFileWriterType::Pointer ICMDb0Writer = ImageFileWriterType::New();
		ICMDb0Writer->SetFileName(outputICMDB0_ImageFileName.c_str());
		ICMDb0Writer->SetInput (ICMD_vector2scalar->Getb0Image());



		try
		{
			ICMDdiffWriter->Update();
			ICMDperWriter->Update();
			ICMDfracWriter->Update();
			ICMDb0Writer->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
		    std::cerr << excep << std::endl;
		}


		std::cout << "*************** end ICMD**************" << std::endl;


*/









	}

	return 0;
}