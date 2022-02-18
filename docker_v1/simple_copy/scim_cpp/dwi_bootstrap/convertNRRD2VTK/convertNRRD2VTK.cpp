#include <iostream>
#include <tclap/CmdLine.h>


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"

int main(int argc, char *argv[])
{

	std::string inputImageFileName;
	std::string outputImageFileName;
	int	cropHalf = 0;

	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");

		TCLAP::ValueArg<std::string> inputImageFilenameArg("i","inputImageFileName","Input gray scale image file name",true,"file.nii.gz","string");
		cmd.add( inputImageFilenameArg );

		TCLAP::ValueArg<std::string> outputImageFilenameArg("o","outputImageFileName","Output gray scale image file name",true,"file.nii.gz","string");
		cmd.add( outputImageFilenameArg );


		TCLAP::ValueArg<int> cropHalfArg("c","cropHalf","cropHalf",true,1,"int");
		cmd.add( cropHalfArg );

		// Parse the argv array.
		cmd.parse( argc, argv );

		inputImageFileName = inputImageFilenameArg.getValue();
		outputImageFileName = outputImageFilenameArg.getValue();
		cropHalf = cropHalfArg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	// Get the value parsed by each arg.





	typedef itk::Image <short, 3> ImageType;
	typedef itk::ImageFileReader <ImageType> ImageFileReaderType;
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


	ImageType::Pointer output;

	if (cropHalf == 0)
	{
		output = reader->GetOutput();
	}
	else
	{

		// extract 45 slices only
		ImageType::RegionType orgRegion = reader->GetOutput()->GetLargestPossibleRegion();

		ImageType::RegionType resRegion;
		resRegion.SetIndex(orgRegion.GetIndex());

		//std::cout <<orgRegion.GetIndex()[0] << " " << orgRegion.GetIndex()[1] << " " << orgRegion.GetIndex()[2] << std::endl;

		ImageType::SizeType size;
		size[0] = orgRegion.GetSize()[0];
		size[1] = orgRegion.GetSize()[1];
		size[2] = orgRegion.GetSize()[2]/2.0;
		resRegion.SetSize (size);
		//std::cout <<size[0] << " " << size[1] << " " << size[2] << std::endl;


		typedef itk::RegionOfInterestImageFilter <ImageType,ImageType> itkExtractImageFilterType;
		itkExtractImageFilterType::Pointer extract = itkExtractImageFilterType::New();
		extract->SetInput (reader->GetOutput());
		extract->SetRegionOfInterest(resRegion);
		extract->Update();
		output = extract->GetOutput();

	}

	typedef itk::ImageFileWriter <ImageType> ImageFileWriterType;
	ImageFileWriterType::Pointer writer = ImageFileWriterType::New();
	writer->SetFileName(outputImageFileName.c_str());
	writer->SetInput (output);
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
