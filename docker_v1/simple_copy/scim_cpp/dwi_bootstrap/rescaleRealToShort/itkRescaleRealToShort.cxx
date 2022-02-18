#include <iostream>
#include <tclap/CmdLine.h>


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMultiplyImageFilter.h"

int main(int argc, char *argv[])
{

	std::string inputImageFileName;
	std::string outputImageFileName;
	int scaleFactor;

	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");

		TCLAP::ValueArg<std::string> inputImageFilenameArg("i","inputImageFileName","Input gray scale image file name",true,"file.nii.gz","string");
		cmd.add( inputImageFilenameArg );

		TCLAP::ValueArg<std::string> outputImageFilenameArg("o","outputImageFileName","Output gray scale image file name",true,"file.nii.gz","string");
		cmd.add( outputImageFilenameArg );

		TCLAP::ValueArg<int> scaleFactorArg("s","scaleFactor","scaleFactor",true, 1,"int");
		cmd.add( scaleFactorArg );



		// Parse the argv array.
		cmd.parse( argc, argv );

		inputImageFileName = inputImageFilenameArg.getValue();
		outputImageFileName = outputImageFilenameArg.getValue();
		scaleFactor = scaleFactorArg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	// Get the value parsed by each arg.




	typedef itk::Image <short, 3> ImageType;
	typedef itk::Image <float, 3> RealImageType;
	typedef itk::ImageFileReader <RealImageType> ImageFileReaderType;
	ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
	reader->SetFileName(inputImageFileName.c_str());


	try
	{
		reader->Update();
	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
	    std::cerr << excep << std::endl;
	}

	ImageType::Pointer scaleImage = ImageType::New();
	scaleImage->SetSpacing (reader->GetOutput()->GetSpacing());
	scaleImage->SetRegions (reader->GetOutput()->GetLargestPossibleRegion());
	scaleImage->SetOrigin (reader->GetOutput()->GetOrigin());
	scaleImage->Allocate();
	scaleImage->FillBuffer (scaleFactor);







	typedef itk::MultiplyImageFilter <RealImageType,ImageType,ImageType> itkMultiplyImageFilterType;
	itkMultiplyImageFilterType::Pointer multiplyFilter = itkMultiplyImageFilterType::New();
	multiplyFilter->SetInput1 (reader->GetOutput());
	multiplyFilter->SetInput2  (scaleImage);

	multiplyFilter->Update();


	typedef itk::ImageFileWriter <ImageType> ImageFileWriterType;
	ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
	writer->SetFileName(outputImageFileName.c_str());
	writer->SetInput (multiplyFilter->GetOutput());
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
