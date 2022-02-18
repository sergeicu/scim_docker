#include <iostream>
#include <string>
#include <tclap/CmdLine.h>


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkModelToTraceWImageFilter.h"


const std::string ADCFileName = "/meanADC_1.vtk";
const std::string PERFileName = "/meanPER_1.vtk";
const std::string PER_FracFileName = "/meanPER_FRAC_1.vtk";
const std::string B0_FileName = "/meanB0_1.vtk";




//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.


int main(int argc, char *argv[])
{
	double bValue = 3000;
	std::string inputModelFolderName;
	std::string outputFileName;
	std::string baseFileName;
	std::string maskFileName;



	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");

		TCLAP::ValueArg<double> BValueArg("b","bValue","bValue",true,3000,"double");
		cmd.add( BValueArg );

		TCLAP::ValueArg<std::string> inputModelFolderNameArg("i","inputModelFolderName","inputModelFolderName",true,"","string");
		cmd.add( inputModelFolderNameArg );

		TCLAP::ValueArg<std::string> maskFileNameArg("m","maskFileName","maskFileName",true,"","string");
		cmd.add( maskFileNameArg );

		TCLAP::ValueArg<std::string> baseFileNameArg("s","baseFileName","baseFileName",true,"","string");
		cmd.add( baseFileNameArg );


		TCLAP::ValueArg<std::string> outputFileNameArg("o","outputFileName","Output gray scale image file name",true,"","string");
		cmd.add( outputFileNameArg );


		// Parse the argv array.
		cmd.parse( argc, argv );

		bValue = BValueArg.getValue();
		inputModelFolderName = inputModelFolderNameArg.getValue();
		outputFileName = outputFileNameArg.getValue();
		baseFileName = baseFileNameArg.getValue();
		maskFileName = maskFileNameArg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}

	typedef itk::Image <float, 3> RealImageType;
	typedef itk::ImageFileReader <RealImageType> ImageFileReaderType;



	std::string b0_name = inputModelFolderName + B0_FileName;
	std::string ADC_name = inputModelFolderName + ADCFileName;
	std::string PER_name = inputModelFolderName + PERFileName;
	std::string PER_FRAC_name = inputModelFolderName + PER_FracFileName;

	ImageFileReaderType::Pointer b0_reader = ImageFileReaderType::New();
	ImageFileReaderType::Pointer ADC_reader = ImageFileReaderType::New();
	ImageFileReaderType::Pointer PER_reader = ImageFileReaderType::New();
	ImageFileReaderType::Pointer PER_FRAC_reader = ImageFileReaderType::New();

	ImageFileReaderType::Pointer mask_reader = ImageFileReaderType::New();
	ImageFileReaderType::Pointer base_reader = ImageFileReaderType::New();


	b0_reader->SetFileName(b0_name.c_str());
	ADC_reader->SetFileName(ADC_name.c_str());
	PER_reader->SetFileName(PER_name.c_str());
	PER_FRAC_reader->SetFileName(PER_FRAC_name.c_str());
	mask_reader->SetFileName(maskFileName.c_str());
	base_reader->SetFileName(baseFileName.c_str());

	try
	{
		b0_reader->Update();
		ADC_reader->Update();
		PER_reader->Update();
		PER_FRAC_reader->Update();
		mask_reader->Update();
		base_reader->Update();

	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
		std::cerr << excep << std::endl;
	}

	typedef itk::ModelToTraceWImageFilter <RealImageType,RealImageType> itkModelToTraceWImageFilterType;

	itkModelToTraceWImageFilterType::Pointer modelExtrapolate = itkModelToTraceWImageFilterType::New();
	modelExtrapolate->SetB0Image (b0_reader->GetOutput());
	modelExtrapolate->SetADCImage (ADC_reader->GetOutput());
	modelExtrapolate->SetPERImage (PER_reader->GetOutput());
	modelExtrapolate->SetPER_FRACImage (PER_FRAC_reader->GetOutput());

	//dummy function
	modelExtrapolate->SetInput (b0_reader->GetOutput());
	modelExtrapolate->SetBValue (bValue);
	modelExtrapolate->SetMaskImage(mask_reader->GetOutput());
	modelExtrapolate->SetBaseImage(base_reader->GetOutput());




	modelExtrapolate->Update();


	typedef itk::ImageFileWriter <RealImageType> ImageFileWriterType;
	ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
	writer->SetFileName(outputFileName.c_str());
	writer->SetInput (modelExtrapolate->GetOutput());

	try
	{
		writer->Update();
	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
	    std::cerr << excep << std::endl;
	}


	return 0;
}
