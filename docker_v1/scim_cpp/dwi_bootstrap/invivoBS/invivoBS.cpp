#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

#include <cstdlib>
#include <ctime>


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkSmoothingRecursiveGaussianImageFilter.h"

#include "crlBodyDiffusionTools.h"
#include "itkBValuesToADCImageFilter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkBinaryThresholdImageFilter.h"

const std::string ADC_SE_FileName = "/ADC_SE.vtk";
const std::string PER_SE_FileName = "/PER_SE.vtk";

const std::string PER_FRAC_SE_FileName = "/PER_FRAC_SE.vtk";



const std::string ADC_SE_LO_FileName = "/ADC_SE_LO.vtk";
const std::string PER_SE_LO_FileName = "/PER_SE_LO.vtk";

const std::string PER_FRAC_SE_LO_FileName = "/PER_FRAC_SE_LO.vtk";


const std::string ADCFileName = "/meanADC.vtk";
const std::string PERFileName = "/meanPER.vtk";
const std::string PER_FracFileName = "/meanPER_FRAC.vtk";

//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.


int main(int argc, char *argv[])
{
	// random initialization
	srand((unsigned)time(0));

	std::string DirsFileName;
	std::string ImagesNamesFileName;
	std::string MaskImageFileName;

	std::string outputFolderFileName;


	int numOfBSIters = 1000;

	int Threshold  = 0;

	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");



		TCLAP::ValueArg<int> BSItersArg("","bsIters","bsIters",false,1000,"int");
		cmd.add( BSItersArg );

		TCLAP::ValueArg<std::string> DirsFileNameArg("d","DirsNamesFileName","DirsNamesFileName",true,"","string");
		cmd.add( DirsFileNameArg );

		TCLAP::ValueArg<std::string> ImagesNamesFileNameArg("i","ImagesNamesFileName","b_value images names filename",true,"","string");
		cmd.add( ImagesNamesFileNameArg );

		TCLAP::ValueArg<std::string> MaskImageFileNameArg("m","MaskImageFileName","MaskImageFileName",false,"","string");
		cmd.add( MaskImageFileNameArg );

		TCLAP::ValueArg<std::string> outputFolderFileNameArg("o","outputFolderFileName","Output gray scale image file name",true,"","string");
		cmd.add( outputFolderFileNameArg );

		TCLAP::ValueArg<int> ThresholdArg("t","threshold","threshold",false,0,"int");
		cmd.add( ThresholdArg );


		// Parse the argv array.
		cmd.parse( argc, argv );


		DirsFileName = DirsFileNameArg.getValue();
		ImagesNamesFileName = ImagesNamesFileNameArg.getValue();
		MaskImageFileName = MaskImageFileNameArg.getValue();
		outputFolderFileName = outputFolderFileNameArg.getValue();
		numOfBSIters = BSItersArg.getValue();
		Threshold = ThresholdArg.getValue();





	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}





	typedef itk::Image <short, 3> ImageType;
	typedef itk::Image <float, 3> RealImageType;
	typedef itk::ImageFileReader <ImageType> ImageFileReaderType;
	typedef itk::Image <crlBodyDiffusionTools::MeasureMentVectorType, 3> tmpBSImageType;
	typedef itk::SmoothingRecursiveGaussianImageFilter <ImageType> itkSmoothingRecursiveGaussianImageFilterType;

	typedef itk::ImageRegionConstIterator <RealImageType> RealImageRegionConstIteratorType;
	typedef itk::ImageRegionIterator <RealImageType> RealImageRegionIteratorType;


	typedef itk::ImageRegionConstIterator <tmpBSImageType> tmpBSImageRegionConstIteratorType;
	typedef itk::ImageRegionIterator <tmpBSImageType> tmpBSImageRegionIteratorType;


	typedef itk::BValuesToADCImageFilter <ImageType,RealImageType> itkBValuesToADCImageFilterType;




	std::fstream dirsFile;
	dirsFile.open( DirsFileName.c_str(), std::ios::in );

	int numOfDirs;
	dirsFile >> numOfDirs;

	std::cout << "numOfDirs: " << numOfDirs << std::endl;


	ImageType::Pointer orgDataMatrix [4][34];

	std::fstream dataSetNamesFile;
	dataSetNamesFile.open (ImagesNamesFileName.c_str(), std::ios::in);

	int bvalsNum=34;
	//dataSetNamesFile >> bvalsNum;

	//std::cout << "bvalsNum: " << bvalsNum << std::endl;



	crlBodyDiffusionTools::MeasureMentVectorType bvals  (bvalsNum);
	std::vector<std::string> b_filenames (bvalsNum);

	for (int bIdx=0;bIdx<bvalsNum;++bIdx)
	{
		dataSetNamesFile >> bvals[bIdx] >> b_filenames [bIdx];

		//std::cout << bvals[bIdx] << " - " << b_filenames [bIdx] << std::endl;
	}


	for (int expIdx=0;expIdx<numOfDirs;++expIdx)
	{

		std::string curDir;
		dirsFile>>curDir;



		for (int bIdx=0;bIdx<bvalsNum;++bIdx)
		{
			std::string tmpFilename = curDir + "/" + b_filenames [bIdx];
			//std::cout << tmpFilename << std::endl;


			ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
			reader->SetFileName(tmpFilename.c_str());
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

			ImageType::Pointer tmpPointer = smoother->GetOutput();
			tmpPointer->DisconnectPipeline();




			orgDataMatrix[expIdx][bIdx] = tmpPointer;



		}



	}
	dirsFile.close();


	ImageType::Pointer maskImage;

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
			maskImage= maskReader->GetOutput();

		}
		else if (Threshold !=0)
		{
			typedef itk::BinaryThresholdImageFilter<ImageType,ImageType> itkBinaryThresholdImageFilterType;
			itkBinaryThresholdImageFilterType::Pointer thresh = itkBinaryThresholdImageFilterType::New();

		 	thresh->SetInput (orgDataMatrix[0][0] ); //assume that the first is b0 image

			thresh->SetLowerThreshold(Threshold);
			thresh->SetOutsideValue(itkBValuesToADCImageFilterType::BACKGROUND);
			thresh->SetInsideValue(itkBValuesToADCImageFilterType::FOREGROUND);
			thresh->Update();
			maskImage=thresh->GetOutput();
		}



	crlBodyDiffusionTools::MeasureMentVectorType zeroVec (numOfBSIters);
	zeroVec.fill (0.0);

	tmpBSImageType::Pointer bsSamples_D = tmpBSImageType::New();
	bsSamples_D->SetSpacing (maskImage->GetSpacing());
	bsSamples_D->SetOrigin (maskImage->GetOrigin());
	bsSamples_D->SetRegions(maskImage->GetLargestPossibleRegion());
	bsSamples_D->Allocate();
	bsSamples_D->FillBuffer(zeroVec);

	tmpBSImageType::Pointer bsSamples_Dstar = tmpBSImageType::New();
	bsSamples_Dstar->SetSpacing (maskImage->GetSpacing());
	bsSamples_Dstar->SetOrigin (maskImage->GetOrigin());
	bsSamples_Dstar->SetRegions(maskImage->GetLargestPossibleRegion());
	bsSamples_Dstar->Allocate();
	bsSamples_Dstar->FillBuffer(zeroVec);

	tmpBSImageType::Pointer bsSamples_f = tmpBSImageType::New();
	bsSamples_f->SetSpacing (maskImage->GetSpacing());
	bsSamples_f->SetOrigin (maskImage->GetOrigin());
	bsSamples_f->SetRegions(maskImage->GetLargestPossibleRegion());
	bsSamples_f->Allocate();
	bsSamples_f->FillBuffer(zeroVec);


	tmpBSImageRegionIteratorType bsSamples_D_Iter (bsSamples_D,bsSamples_D->GetLargestPossibleRegion());
	tmpBSImageRegionIteratorType bsSamples_Dstar_Iter (bsSamples_Dstar,bsSamples_Dstar->GetLargestPossibleRegion());
	tmpBSImageRegionIteratorType bsSamples_f_Iter (bsSamples_f,bsSamples_f->GetLargestPossibleRegion());



	// keep out put accumlators for D,DSTAR, AND f
/*
	RealImageType::Pointer D_Mk = RealImageType::New();
	D_Mk->SetSpacing (maskImage->GetSpacing());
	D_Mk->SetOrigin (maskImage->GetOrigin());
	D_Mk->SetRegions(maskImage->GetLargestPossibleRegion());
	D_Mk->Allocate();
	D_Mk->FillBuffer(0.0);

	RealImageType::Pointer D_Qk = RealImageType::New();
	D_Qk->SetSpacing (maskImage->GetSpacing());
	D_Qk->SetOrigin (maskImage->GetOrigin());
	D_Qk->SetRegions(maskImage->GetLargestPossibleRegion());
	D_Qk->Allocate();
	D_Qk->FillBuffer(0.0);


	RealImageType::Pointer DSTAR_Mk = RealImageType::New();
	DSTAR_Mk->SetSpacing (maskImage->GetSpacing());
	DSTAR_Mk->SetOrigin (maskImage->GetOrigin());
	DSTAR_Mk->SetRegions(maskImage->GetLargestPossibleRegion());
	DSTAR_Mk->Allocate();
	DSTAR_Mk->FillBuffer(0.0);

	RealImageType::Pointer DSTAR_Qk = RealImageType::New();
	DSTAR_Qk->SetSpacing (maskImage->GetSpacing());
	DSTAR_Qk->SetOrigin (maskImage->GetOrigin());
	DSTAR_Qk->SetRegions(maskImage->GetLargestPossibleRegion());
	DSTAR_Qk->Allocate();
	DSTAR_Qk->FillBuffer(0.0);



	RealImageType::Pointer f_Mk = RealImageType::New();
	f_Mk->SetSpacing (maskImage->GetSpacing());
	f_Mk->SetOrigin (maskImage->GetOrigin());
	f_Mk->SetRegions(maskImage->GetLargestPossibleRegion());
	f_Mk->Allocate();
	f_Mk->FillBuffer(0.0);

	RealImageType::Pointer f_Qk = RealImageType::New();
	f_Qk->SetSpacing (maskImage->GetSpacing());
	f_Qk->SetOrigin (maskImage->GetOrigin());
	f_Qk->SetRegions(maskImage->GetLargestPossibleRegion());
	f_Qk->Allocate();
	f_Qk->FillBuffer(0.0);


	RealImageRegionIteratorType D_Mk_Iter (D_Mk,D_Mk->GetLargestPossibleRegion());
	RealImageRegionIteratorType D_Qk_Iter (D_Qk,D_Qk->GetLargestPossibleRegion());

	RealImageRegionIteratorType DSTAR_Mk_Iter (DSTAR_Mk,DSTAR_Mk->GetLargestPossibleRegion());
	RealImageRegionIteratorType DSTAR_Qk_Iter (DSTAR_Qk,DSTAR_Qk->GetLargestPossibleRegion());

	RealImageRegionIteratorType f_Mk_Iter (f_Mk,f_Mk->GetLargestPossibleRegion());
	RealImageRegionIteratorType f_Qk_Iter (f_Qk,f_Qk->GetLargestPossibleRegion());

*/

	int k = 0;
	for (int bsIdx=0;bsIdx < numOfBSIters; ++bsIdx)
	{

		// generate new dataset for exp
		itkBValuesToADCImageFilterType::Pointer modelFit = itkBValuesToADCImageFilterType::New();
		modelFit->SetAlgMode(itkBValuesToADCImageFilterType::ML);
		modelFit->SetComputeBootstrap(false);

		//loop from 1 to 34
		//		random number from 1 to 4
		// 		put image from the random seq to cur vector
		//end loop
		for (int bvalIdx=0;bvalIdx<bvalsNum;++bvalIdx)
		{

			int random_dir = (int) (double(numOfDirs) * (rand() / (RAND_MAX + 1.0)));
			//std::cout << bvalIdx << ": " << random_dir << " -" << orgDataMatrix[random_dir][bvalIdx]->GetOrigin()[2] << std::endl;
			modelFit->SetInput (bvalIdx,orgDataMatrix[random_dir][bvalIdx]);
		}

		modelFit->SetMaskImage (maskImage);
		modelFit->SetBValues(bvals);
		modelFit->Update();



		// accumulating data

		RealImageType::Pointer dstar_image = modelFit->GetPerfusionImage();
		RealImageType::Pointer d_image = modelFit->GetDiffusionImage();
		RealImageType::Pointer f_image = modelFit->GetPerfusionFractionImage();


		RealImageRegionConstIteratorType d_image_iter (d_image,d_image->GetLargestPossibleRegion());
		RealImageRegionConstIteratorType dstar_image_iter (dstar_image,dstar_image->GetLargestPossibleRegion());
		RealImageRegionConstIteratorType f_image_iter (f_image,f_image->GetLargestPossibleRegion());


		/*
		if (bsIdx == 0) // first BS iteration
		{
			k = 1;

			D_Qk->FillBuffer(0.0);
			DSTAR_Qk->FillBuffer(0.0);
			f_Qk->FillBuffer(0.0);

			for (D_Mk_Iter.GoToBegin(), DSTAR_Mk_Iter.GoToBegin(),f_Mk_Iter.GoToBegin(),
				 d_image_iter.GoToBegin(), dstar_image_iter.GoToBegin(), f_image_iter.GoToBegin();
				 !D_Mk_Iter.IsAtEnd();
				 ++D_Mk_Iter, ++DSTAR_Mk_Iter,++f_Mk_Iter,
				 ++d_image_iter, ++dstar_image_iter, ++f_image_iter)
			{
				D_Mk_Iter.Value() = d_image_iter.Value();
				DSTAR_Mk_Iter.Value() = dstar_image_iter.Value();
				f_Mk_Iter.Value() = f_image_iter.Value();

			}

		}
		else
		{
			k=k+1;

			for (D_Mk_Iter.GoToBegin(), DSTAR_Mk_Iter.GoToBegin(),f_Mk_Iter.GoToBegin(),
				 D_Qk_Iter.GoToBegin(), DSTAR_Qk_Iter.GoToBegin(),f_Qk_Iter.GoToBegin(),
				 d_image_iter.GoToBegin(), dstar_image_iter.GoToBegin(), f_image_iter.GoToBegin();
				 !D_Mk_Iter.IsAtEnd();
				 ++D_Mk_Iter, ++DSTAR_Mk_Iter,++f_Mk_Iter,
				 ++D_Qk_Iter, ++DSTAR_Qk_Iter,++f_Qk_Iter,
				 ++d_image_iter, ++dstar_image_iter, ++f_image_iter)
				{



					double D_Xk = d_image_iter.Value ();
					double D_d = D_Xk-D_Mk_Iter.Value(); // is actually xk - Mk-1,
							                             // as Mk was not yet updated
					D_Qk_Iter.Value ()  =  D_Qk_Iter.Value() + (k-1)*D_d*D_d/k;
					D_Mk_Iter.Value () = D_Mk_Iter.Value () + D_d/k;


					double DSTAR_Xk = dstar_image_iter.Value ();
					double DSTAR_d = DSTAR_Xk-DSTAR_Mk_Iter.Value(); // is actually xk - Mk-1,
														             // as Mk was not yet updated
					DSTAR_Qk_Iter.Value ()  =  DSTAR_Qk_Iter.Value() + (k-1)*DSTAR_d*DSTAR_d/k;
					DSTAR_Mk_Iter.Value () = DSTAR_Mk_Iter.Value () + DSTAR_d/k;


					double f_Xk = f_image_iter.Value ();
					double f_d = f_Xk-f_Mk_Iter.Value(); // is actually xk - Mk-1,
																			             // as Mk was not yet updated
					f_Qk_Iter.Value ()  =  f_Qk_Iter.Value() + (k-1)*f_d*f_d/k;
					f_Mk_Iter.Value () = f_Mk_Iter.Value() + f_d/k;


				}



		}


		std::cout << "bs iteration: " << k <<  std::endl;


	}


*/

		for (bsSamples_D_Iter.GoToBegin(), bsSamples_Dstar_Iter.GoToBegin(),bsSamples_f_Iter.GoToBegin(),
			 d_image_iter.GoToBegin(), dstar_image_iter.GoToBegin(), f_image_iter.GoToBegin();
			 !bsSamples_D_Iter.IsAtEnd();
			 ++bsSamples_D_Iter, ++bsSamples_Dstar_Iter,++bsSamples_f_Iter,
			 ++d_image_iter, ++dstar_image_iter, ++f_image_iter)
			{
				double D = d_image_iter.Value ();
				double DSTAR = dstar_image_iter.Value ();
				double f = f_image_iter.Value ();

				bsSamples_D_Iter.Value()[bsIdx]=D;
				bsSamples_Dstar_Iter.Value()[bsIdx]=DSTAR;
				bsSamples_f_Iter.Value()[bsIdx]=f;
			}
		std::cout << "bs iteration: " << bsIdx <<  std::endl;
	} // end bs iters

	RealImageType::Pointer D = RealImageType::New();
	D->SetSpacing (maskImage->GetSpacing());
	D->SetOrigin (maskImage->GetOrigin());
	D->SetRegions(maskImage->GetLargestPossibleRegion());
	D->Allocate();
	D->FillBuffer(0.0);

	RealImageType::Pointer D_LO = RealImageType::New();
	D_LO->SetSpacing (maskImage->GetSpacing());
	D_LO->SetOrigin (maskImage->GetOrigin());
	D_LO->SetRegions(maskImage->GetLargestPossibleRegion());
	D_LO->Allocate();
	D_LO->FillBuffer(0.0);

	RealImageType::Pointer D_HI = RealImageType::New();
	D_HI->SetSpacing (maskImage->GetSpacing());
	D_HI->SetOrigin (maskImage->GetOrigin());
	D_HI->SetRegions(maskImage->GetLargestPossibleRegion());
	D_HI->Allocate();
	D_HI->FillBuffer(0.0);


	RealImageType::Pointer Dstar = RealImageType::New();
	Dstar->SetSpacing (maskImage->GetSpacing());
	Dstar->SetOrigin (maskImage->GetOrigin());
	Dstar->SetRegions(maskImage->GetLargestPossibleRegion());
	Dstar->Allocate();
	Dstar->FillBuffer(0.0);

	RealImageType::Pointer Dstar_LO = RealImageType::New();
	Dstar_LO->SetSpacing (maskImage->GetSpacing());
	Dstar_LO->SetOrigin (maskImage->GetOrigin());
	Dstar_LO->SetRegions(maskImage->GetLargestPossibleRegion());
	Dstar_LO->Allocate();
	Dstar_LO->FillBuffer(0.0);

	RealImageType::Pointer Dstar_HI = RealImageType::New();
	Dstar_HI->SetSpacing (maskImage->GetSpacing());
	Dstar_HI->SetOrigin (maskImage->GetOrigin());
	Dstar_HI->SetRegions(maskImage->GetLargestPossibleRegion());
	Dstar_HI->Allocate();
	D_HI->FillBuffer(0.0);

	RealImageType::Pointer f = RealImageType::New();
	f->SetSpacing (maskImage->GetSpacing());
	f->SetOrigin (maskImage->GetOrigin());
	f->SetRegions(maskImage->GetLargestPossibleRegion());
	f->Allocate();
	f->FillBuffer(0.0);

	RealImageType::Pointer f_LO = RealImageType::New();
	f_LO->SetSpacing (maskImage->GetSpacing());
	f_LO->SetOrigin (maskImage->GetOrigin());
	f_LO->SetRegions(maskImage->GetLargestPossibleRegion());
	f_LO->Allocate();
	f_LO->FillBuffer(0.0);

	RealImageType::Pointer f_HI = RealImageType::New();
	f_HI->SetSpacing (maskImage->GetSpacing());
	f_HI->SetOrigin (maskImage->GetOrigin());
	f_HI->SetRegions(maskImage->GetLargestPossibleRegion());
	f_HI->Allocate();
	f_HI->FillBuffer(0.0);



	RealImageRegionIteratorType D_Iter (D,D->GetLargestPossibleRegion());
	RealImageRegionIteratorType Dstar_Iter (Dstar,Dstar->GetLargestPossibleRegion());
	RealImageRegionIteratorType f_Iter (f,f->GetLargestPossibleRegion());

	RealImageRegionIteratorType D_LO_Iter (D_LO,D_LO->GetLargestPossibleRegion());
	RealImageRegionIteratorType D_HI_Iter (D_HI,D_HI->GetLargestPossibleRegion());

	RealImageRegionIteratorType Dstar_LO_Iter (Dstar_LO,Dstar_LO->GetLargestPossibleRegion());
	RealImageRegionIteratorType Dstar_HI_Iter (Dstar_HI,Dstar_HI->GetLargestPossibleRegion());

	RealImageRegionIteratorType f_LO_Iter (f_LO,f_LO->GetLargestPossibleRegion());
	RealImageRegionIteratorType f_HI_Iter (f_HI,f_HI->GetLargestPossibleRegion());


	for (D_Iter.GoToBegin(), Dstar_Iter.GoToBegin(),f_Iter.GoToBegin(),
		 D_LO_Iter.GoToBegin(), Dstar_LO_Iter.GoToBegin(),f_LO_Iter.GoToBegin(),
		 D_HI_Iter.GoToBegin(), Dstar_HI_Iter.GoToBegin(),f_HI_Iter.GoToBegin(),
		 bsSamples_D_Iter.GoToBegin(), bsSamples_Dstar_Iter.GoToBegin(),bsSamples_f_Iter.GoToBegin();
		 !D_LO_Iter.IsAtEnd();
		 ++D_LO_Iter, ++Dstar_LO_Iter,++f_LO_Iter,
		 ++D_HI_Iter, ++Dstar_HI_Iter,++f_HI_Iter,
		 ++D_Iter, ++Dstar_Iter,++f_Iter,
		 ++bsSamples_D_Iter, ++bsSamples_Dstar_Iter,++bsSamples_f_Iter
		 )
		{

			double D_LO, D_HI, Dstar_LO, Dstar_HI, f_LO, f_HI;

			crlBodyDiffusionTools::computeCI (bsSamples_D_Iter.Value(), D_LO, D_HI);
			crlBodyDiffusionTools::computeCI (bsSamples_Dstar_Iter.Value(), Dstar_LO, Dstar_HI);
			crlBodyDiffusionTools::computeCI (bsSamples_f_Iter.Value(), f_LO, f_HI);

			D_LO_Iter.Value() = D_LO;
			Dstar_LO_Iter.Value() = D_LO;
			f_LO_Iter.Value() = f_LO;
			D_HI_Iter.Value() = D_HI;
			Dstar_HI_Iter.Value() = Dstar_HI;
			f_HI_Iter.Value() = f_HI;
			D_Iter.Value() = bsSamples_D_Iter.Value().mean();
			Dstar_Iter.Value() = bsSamples_Dstar_Iter.Value().mean();
			f_Iter.Value() = bsSamples_f_Iter.Value().mean();



		}


	//extract final output from Accumulators to mean val and sd
	/*
	for (D_Qk_Iter.GoToBegin(), DSTAR_Qk_Iter.GoToBegin(),f_Qk_Iter.GoToBegin();
		!D_Qk_Iter.IsAtEnd();
		++D_Qk_Iter, ++DSTAR_Qk_Iter,++f_Qk_Iter)
	{

		D_Qk_Iter.Value() = vcl_sqrt(D_Qk_Iter.Value()/(k-1));
		DSTAR_Qk_Iter.Value() = vcl_sqrt(DSTAR_Qk_Iter.Value()/(k-1));
		f_Qk_Iter.Value() = vcl_sqrt(f_Qk_Iter.Value()/(k-1));
	}*/


	//write output
	std::string outputD_SE_ImageFileName = outputFolderFileName + "/" + ADC_SE_FileName;
	std::string outputDSTAR_SE_ImageFileName = outputFolderFileName + "/" + PER_SE_FileName;
	std::string outputf_SE_ImageFileName = outputFolderFileName + "/" + PER_FRAC_SE_FileName;

	std::string outputD_SE_LO_ImageFileName = outputFolderFileName + "/" + ADC_SE_LO_FileName;
	std::string outputDSTAR_SE_LO_ImageFileName = outputFolderFileName + "/" + PER_SE_LO_FileName;
	std::string outputf_SE_LO_ImageFileName = outputFolderFileName + "/" + PER_FRAC_SE_LO_FileName;


	std::string outputDImageFileName = outputFolderFileName + "/" + ADCFileName;
	std::string outputDSTARImageFileName = outputFolderFileName + "/" + PERFileName;
	std::string outputfImageFileName = outputFolderFileName + "/" + PER_FracFileName;

	typedef itk::ImageFileWriter <RealImageType> ImageFileWriterType;
	ImageFileWriterType::Pointer diff_SE_Writer = ImageFileWriterType::New();
	diff_SE_Writer->SetFileName(outputD_SE_ImageFileName.c_str());
	diff_SE_Writer->SetInput (D_HI);

	ImageFileWriterType::Pointer per_SE_Writer = ImageFileWriterType::New();
	per_SE_Writer->SetFileName(outputDSTAR_SE_ImageFileName.c_str());
	per_SE_Writer->SetInput (Dstar_HI);

	ImageFileWriterType::Pointer frac_SE_Writer = ImageFileWriterType::New();
	frac_SE_Writer->SetFileName(outputf_SE_ImageFileName.c_str());
	frac_SE_Writer->SetInput (f_HI);


	ImageFileWriterType::Pointer diff_SE_LO_Writer = ImageFileWriterType::New();
	diff_SE_LO_Writer->SetFileName(outputD_SE_LO_ImageFileName.c_str());
	diff_SE_LO_Writer->SetInput (D_LO);

	ImageFileWriterType::Pointer per_SE_LO_Writer = ImageFileWriterType::New();
	per_SE_LO_Writer->SetFileName(outputDSTAR_SE_LO_ImageFileName.c_str());
	per_SE_LO_Writer->SetInput (Dstar_LO);

	ImageFileWriterType::Pointer frac_SE_LO_Writer = ImageFileWriterType::New();
	frac_SE_LO_Writer->SetFileName(outputf_SE_LO_ImageFileName.c_str());
	frac_SE_LO_Writer->SetInput (f_LO);



	ImageFileWriterType::Pointer diffWriter = ImageFileWriterType::New();
	diffWriter->SetFileName(outputDImageFileName.c_str());
	diffWriter->SetInput (D);

	ImageFileWriterType::Pointer perWriter = ImageFileWriterType::New();
	perWriter->SetFileName(outputDSTARImageFileName.c_str());
	perWriter->SetInput (Dstar);

	ImageFileWriterType::Pointer fracWriter = ImageFileWriterType::New();
	fracWriter->SetFileName(outputfImageFileName.c_str());
	fracWriter->SetInput (f);



	try
	{
		diffWriter->Update();
		perWriter->Update();
		fracWriter->Update();

		diff_SE_Writer->Update();
		per_SE_Writer->Update();
		frac_SE_Writer->Update();


		diff_SE_LO_Writer->Update();
		per_SE_LO_Writer->Update();
		frac_SE_LO_Writer->Update();

	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
		std::cerr << excep << std::endl;
	}

	return 0;
}
