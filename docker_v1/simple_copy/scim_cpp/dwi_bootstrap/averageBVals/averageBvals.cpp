#include <iostream>
#include <string>
#include <cstring>
#include <tclap/CmdLine.h>


#include "BvaluesParser.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkAverageSeriesImageFilter.h"
#include "itkRicianNoiseCalculator.h"

#include <vector>


int main(int argc, char *argv[])
{


	int bValuesNum = 6;
	std::string ImagesNamesFileName;
	std::string outputFolderFileName;
	std::string averageMode="";

	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");
		TCLAP::ValueArg<int> BValuesNumArg("n","bValuesNum","bValuesNum",false,6,"int");
		cmd.add( BValuesNumArg );

		TCLAP::ValueArg<std::string> ImagesNamesFileNameArg("i","ImagesNamesFileName","b_value images names filename",true,"","string");
		cmd.add( ImagesNamesFileNameArg );

		TCLAP::ValueArg<std::string> outputFolderFileNameArg("o","outputFolderFileName","Output gray scale image file name",true,"","string");
		cmd.add( outputFolderFileNameArg );

		TCLAP::ValueArg<std::string> averageModeArg("m","averageMode","averageMode",true,"","string");
		cmd.add( averageModeArg );

		// Parse the argv array.
		cmd.parse( argc, argv );

		bValuesNum = BValuesNumArg.getValue();
		ImagesNamesFileName = ImagesNamesFileNameArg.getValue();
		outputFolderFileName = outputFolderFileNameArg.getValue();
		averageMode = averageModeArg.getValue();

		}
		catch (TCLAP::ArgException &e)  // catch any exceptions
		{
			std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		}

		BvaluesParser parser (bValuesNum, ImagesNamesFileName);



		typedef itk::Image <short, 3> ImageType;
		typedef itk::ImageFileReader <ImageType> ImageFileReaderType;
		typedef itk::ImageFileWriter <ImageType> ImageFileWriterType;

		typedef itk::SmoothingRecursiveGaussianImageFilter <ImageType> itkSmoothingRecursiveGaussianImageFilterType;

		typedef itk::AverageSeriesImageFilter<ImageType,ImageType> itkAverageSeriesImageFilterType;
		typedef itk::RicianNoiseCalculator <ImageType> itkRicianNoiseCalculatorType;


		std::vector <ImageType::Pointer> bImages;
		std::vector <int> uniquebValues (bValuesNum);
		std::vector <int>  bValues (bValuesNum);
		std::vector <double> weights (bValuesNum);

		for (size_t i=0;i<parser.getBValues().size();++i)
		{
			bValues[i] = parser.getBValues()[i];
			uniquebValues[i]  = bValues[i];
		}

		std::vector<int>::iterator uniqueIt;
		// using default comparison:
		uniqueIt = unique (uniquebValues.begin(), uniquebValues.end());
		uniquebValues.resize( uniqueIt - uniquebValues.begin() );


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
			weights[bIdx] = 1/noiseVarCalc->GetVariance();;

		}

	  	// run averaging
	  	for (std::vector<int>::const_iterator it = uniquebValues.begin();
	  		 it != uniquebValues.end(); ++it)
	  	{
	  		int bval = *it;

	  		itkAverageSeriesImageFilterType::Pointer averageFilter = itkAverageSeriesImageFilterType::New();
	  		if (averageMode=="geometric")
	  		{
	  			averageFilter->SetAverageMode(itkAverageSeriesImageFilterType::GeometricAverage);
	  		}
	  		else if (averageMode=="arithmetic")
	  		{
	  			averageFilter->SetAverageMode(itkAverageSeriesImageFilterType::ArithmeticAverage);
	  		}
	  		else
	  		{
	  			std::cerr << "error: unknown average mode. Possibilites are: geometric | arithmetic" << std::endl;
	  			exit (EXIT_FAILURE);
	  		}

	  		int counter = 0;
	  		for (int bIdx=0;bIdx<bValuesNum;++bIdx)
	  		{
	  			if (bValues[bIdx] == bval)
	  			{
	  				averageFilter->SetInput(counter,bImages[bIdx]);
	  				averageFilter->AddWeight(weights[bIdx]);
	  				counter++;
	  			}
	  		}
	  		averageFilter->Update();


	  		//build filename

	  		char outputfilenameBase[255];
	  		sprintf (outputfilenameBase, "/b%d_averaged.vtk", bval);

	  		std::string outputFileName = outputFolderFileName;
	  		outputFileName.append(outputfilenameBase);

	  		ImageFileWriterType::Pointer averageWriter = ImageFileWriterType::New();
	  		averageWriter->SetFileName(outputFileName.c_str());
	  		averageWriter->SetInput(averageFilter->GetOutput());


	  		try
	  		{
	  			averageWriter->Update();
  			}
  			catch( itk::ExceptionObject & excep )
  			{
  				std::cerr << "Exception catched !" << std::endl;
  			    std::cerr << excep << std::endl;
  			}

	  	}







	return 0;
}
