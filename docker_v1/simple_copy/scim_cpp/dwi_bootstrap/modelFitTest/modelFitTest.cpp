#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

#include "vnl/vnl_vector.h"
#include <cstdio>


#include <fstream>







#include "BvaluesParser.h"
#include "itkRicianNoiseCalculator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"


#include "crlBodyDiffusionTools.h"


int main(int argc, char *argv[])
{




	int bValuesNum = 6;
	int numberOfBootstrapEstimates = 1000;
	std::string ImagesNamesFileName;
	std::string IndicesFileName;
	std::string outputFolderFileName;
	std::string expType;

	int x,y,z;



	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");
		TCLAP::ValueArg<int> BValuesNumArg("n","bValuesNum","bValuesNum",false,6,"int");
		cmd.add( BValuesNumArg );

		TCLAP::ValueArg<std::string> ImagesNamesFileNameArg("i","ImagesNamesFileName","b_value images names filename",true,"","string");
		cmd.add( ImagesNamesFileNameArg );

		TCLAP::ValueArg<std::string> outputFolderFileNameArg("o","outputFolderFileName","Output gray scale image file name",true,"","string");
		cmd.add( outputFolderFileNameArg );

		TCLAP::ValueArg<int> XIndexArg("x","XIndex","XIndex",true,6,"int");
		cmd.add( XIndexArg );

		TCLAP::ValueArg<int> YIndexArg("y","YIndex","YIndex",true,6,"int");
		cmd.add( YIndexArg );

		TCLAP::ValueArg<int> ZIndexArg("z","ZIndex","ZIndex",true,6,"int");
		cmd.add( ZIndexArg );




		// Parse the argv array.
		cmd.parse( argc, argv );

		bValuesNum = BValuesNumArg.getValue();
		ImagesNamesFileName = ImagesNamesFileNameArg.getValue();
		outputFolderFileName = outputFolderFileNameArg.getValue();
		x = XIndexArg.getValue();
		y = YIndexArg.getValue();
		z = ZIndexArg.getValue();




		}
		catch (TCLAP::ArgException &e)  // catch any exceptions
		{
			std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		}

		BvaluesParser parser (bValuesNum, ImagesNamesFileName);



		typedef itk::Image <float, 3> ImageType;
		typedef itk::ImageFileReader <ImageType> ImageFileReaderType;
		typedef itk::SmoothingRecursiveGaussianImageFilter <ImageType> itkSmoothingRecursiveGaussianImageFilterType;
		typedef itk::RicianNoiseCalculator <ImageType> itkRicianNoiseCalculatorType;

		crlBodyDiffusionTools::MeasureMentVectorType bvals (bValuesNum);
		crlBodyDiffusionTools::MeasureMentVectorType Sb (bValuesNum);
		crlBodyDiffusionTools::MeasureMentVectorType weights (bValuesNum);
		crlBodyDiffusionTools::MeasureMentVectorType variance (bValuesNum);


		std::vector <ImageType::Pointer> bImages;

		for (size_t bIdx=0;bIdx<bValuesNum;++bIdx)
		{
			bvals(bIdx) = parser.getBValues()[bIdx];
			std::cout << bvals(bIdx) << " ";
		}
		std::cout << std::endl;

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

			bImages.push_back(smoother->GetOutput());


			itkRicianNoiseCalculatorType::Pointer noiseVarCalc = itkRicianNoiseCalculatorType::New();

			noiseVarCalc->SetImage(smoother->GetOutput());
			noiseVarCalc->Compute();
			variance(bIdx) = noiseVarCalc->GetVariance();
			weights(bIdx) = 1/ variance(bIdx);




		}




		ImageType::IndexType index;
		index[0] = x;
		index[1] = y;
		index[2] = z;


		for (size_t i=0;i<bValuesNum;++i)
		{
			Sb(i) = bImages[i]->GetPixel(index);
		}




	std::fstream dataOut;
	std::string outputDataFileName = outputFolderFileName;
	outputDataFileName+="/modelFitData.txt";
	dataOut.open (outputDataFileName.c_str(),std::ios::out);
	for (size_t i=0;i<bvals.size();++i)
	{
		dataOut << bvals(i) << "\t" << Sb(i) << "\t" << weights(i) << "\t" << variance(i) << std::endl;
	}
	dataOut.close();


	std::string outputModelFileName = outputFolderFileName;
	outputModelFileName+="/modelFitParams.txt";

	std::fstream modelOut;
    modelOut.open (outputModelFileName.c_str(),std::ios::out);
	double B0 = 0,PER = 0,ADC = 0,PER_FRAC = 0;

/*
	//compute uniexp model
	int adc_min [] ={5,50,100,200};

	for (int i=0;i<4;++i)
	{
		int min_adc = adc_min[i];
		crlBodyDiffusionTools::computeUniExpModel (bvals,Sb,weights,B0, ADC,min_adc);
		modelOut << "uni fit ls:  " <<  min_adc << "\t" << B0 << "\t" << "\t" << ADC << std::endl;
	}

	for (int i=0;i<4;++i)
	{
		int min_adc = adc_min[i];
		crlBodyDiffusionTools::computeUniExpModel (bvals,Sb,weights,B0, ADC,min_adc);
		crlBodyDiffusionTools::computeUniExpModelML (bvals,Sb,variance,B0, ADC,min_adc);
		modelOut << "uni fit ml:  " <<  min_adc << "\t" << B0 << "\t" << "\t" << ADC << std::endl;
	}

*/
// compute linear model
	crlBodyDiffusionTools::computeLinearModel (bvals,Sb,weights,B0,PER,ADC,PER_FRAC);

	modelOut << "SLS fit: " <<  B0 << "\t" << PER << "\t" << ADC << "\t" << PER_FRAC << std::endl;

	crlBodyDiffusionTools::computeNonLinearModelLS (bvals,Sb,variance, weights,B0,PER,ADC,PER_FRAC);
	modelOut << "LS fit: " << B0 << "\t" << PER << "\t" << ADC << "\t" << PER_FRAC << std::endl;




	crlBodyDiffusionTools::computeLinearModel (bvals,Sb,weights,B0,PER,ADC,PER_FRAC);
	crlBodyDiffusionTools::computeNonLinearModel (bvals,Sb,variance,weights,1,B0,PER,ADC,PER_FRAC);

	modelOut << "ML fit: " << B0 << "\t" << PER << "\t" << ADC << "\t" << PER_FRAC << std::endl;





	/*
	crlBodyDiffusionTools::computeGlobalNonLinearModel (bvals,Sb,variance,weights,B0,PER,ADC,PER_FRAC);

	modelOut << "Global ML fit: " << B0 << "\t" << PER << "\t" << ADC << "\t" << PER_FRAC << std::endl;


	for (int emIterIdx = 0; emIterIdx< crlBodyDiffusionTools::EM_iters; ++emIterIdx)
	{
		crlBodyDiffusionTools::computeWeights (bvals,Sb,variance,
					                           weights,
					                           B0,PER,ADC,PER_FRAC);

		crlBodyDiffusionTools::computeNonLinearModel (bvals,Sb,variance,weights,B0,PER,ADC,PER_FRAC);

	}

	modelOut << B0 << "\t" << PER << "\t" << ADC << "\t" << PER_FRAC << std::endl;
*/

/*

	double B0Var = 0;
	double PERVar = 0;
	double ADCVar = 0;
	double PER_FRACVar = 0;

	crlBodyDiffusionTools::computeBootstrapVarianceEstimate(bvals, Sb, variance, weights,
						  	  	  	                        B0,PER,ADC,PER_FRAC,
						  	  	  	                        B0Var, PERVar,
						  	  	  	                        ADCVar, PER_FRACVar,
						  	  	  	                        numberOfBootstrapEstimates);



	//std::fstream varOut;
	//std::string outputVarFileName = outputFolderFileName;
	//outputVarFileName+="/modelFitBS.txt";
	//varOut.open (outputVarFileName.c_str(),std::ios::out);






	// read MC average and std values:

	std::string mcFileNames [8];
	mcFileNames[0] = "../B0Mean_mc.vtk";
	mcFileNames[1] = "../PERMean_mc.vtk";
	mcFileNames[2] = "../ADCMean_mc.vtk";
	mcFileNames[3] = "../PER_FRACMean_mc.vtk";
	mcFileNames[4] = "../B0Var_mc.vtk";
	mcFileNames[5] = "../PERVar_mc.vtk";
	mcFileNames[6] = "../ADCVar_mc.vtk";
	mcFileNames[7] = "../PER_FRACVar_mc.vtk";

	crlBodyDiffusionTools::MeasureMentVectorType mcVals (8);

	for (int i=0;i<8;++i)
	{
		ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
		reader->SetFileName(mcFileNames[i].c_str());
		try
		{
			reader->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
			std::cerr << excep << std::endl;
		}
		mcVals [i] = reader->GetOutput()->GetPixel(index);


	}

	modelOut << "bs mean: " << B0 << "\t" << PER << "\t" << ADC << "\t" << PER_FRAC << std::endl;
	modelOut << "Truth mean: " << mcVals[0]  << "\t" << mcVals[1] << "\t" << mcVals[2] << "\t" << mcVals[3] << std::endl;

	modelOut << std::endl << std::endl;

	modelOut << "std: " << B0Var << "\t" << PERVar << "\t" << ADCVar << "\t" << PER_FRACVar << std::endl;
	modelOut << "Truth std: " << mcVals[4] << "\t" << mcVals[5] << "\t" << mcVals[6] << "\t" << mcVals[7] << std::endl;







	// varOut.close();

	 */
	modelOut.close();

	return 0;
}


