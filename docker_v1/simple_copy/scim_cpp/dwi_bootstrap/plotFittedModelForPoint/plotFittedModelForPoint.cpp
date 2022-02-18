#include <iostream>
#include <string>
#include <tclap/CmdLine.h>

#include "BvaluesParser.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "vnl/vnl_vector.h"

#include <fstream>

const std::string ADCFileName = "/meanADC_1000.vtk";
const std::string PERFileName = "/meanPER_1000.vtk";
const std::string PER_FracFileName = "/meanPER_FRAC_1000.vtk";
const std::string B0_FileName = "/meanB0_1000.vtk";




//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.


int main(int argc, char *argv[])
{
	int bValuesNum = 6;
	std::string ImagesNamesFileName;
	std::string modelFolderFileName;
	std::string outputFileName;

	int x,y,z;

	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");

		TCLAP::ValueArg<int> BValuesNumArg("n","bValuesNum","bValuesNum",false,6,"int");
		cmd.add( BValuesNumArg );

		TCLAP::ValueArg<std::string> ImagesNamesFileNameArg("i","ImagesNamesFileName","b_value images names filename",true,"","string");
		cmd.add( ImagesNamesFileNameArg );

		TCLAP::ValueArg<std::string> modelFolderFileNameArg("m","modelFolderFileName","modelFolderFileName",true,"","string");
		cmd.add( modelFolderFileNameArg );

		TCLAP::ValueArg<std::string> outputFileNameArg("o","outputFileName","outputFileName",true,"","string");
		cmd.add( outputFileNameArg );

		TCLAP::ValueArg<int> xCoordArg("x","xCoord","xCoord",true,5,"int");
		cmd.add( xCoordArg );

		TCLAP::ValueArg<int> yCoordArg("y","yCoord","yCoord",true,5,"int");
		cmd.add( yCoordArg );

		TCLAP::ValueArg<int> zCoordArg("z","zCoord","zCoord",true,5,"int");
		cmd.add( zCoordArg );


		// Parse the argv array.
		cmd.parse( argc, argv );

		bValuesNum = BValuesNumArg.getValue();
		ImagesNamesFileName = ImagesNamesFileNameArg.getValue();
		modelFolderFileName = modelFolderFileNameArg.getValue();
		outputFileName = outputFileNameArg.getValue();
		x = xCoordArg.getValue();
		y = yCoordArg.getValue();
		z = zCoordArg.getValue();




	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}


	typedef itk::Image <short, 3> ImageType;
	typedef itk::Image <float, 3> RealImageType;
	typedef itk::ImageFileReader <ImageType> ImageFileReaderType;
	typedef itk::ImageFileReader <RealImageType> RealImageFileReaderType;


	std::string adc_filename = modelFolderFileName + ADCFileName;
	std::string b0_filename = modelFolderFileName + B0_FileName;
	std::string PER_filename = modelFolderFileName + PERFileName;
	std::string PER_FRAC_filename = modelFolderFileName + PER_FracFileName;



	RealImageFileReaderType::Pointer adcReader = RealImageFileReaderType::New();
	adcReader->SetFileName (adc_filename.c_str());

	RealImageFileReaderType::Pointer b0Reader = RealImageFileReaderType::New();
	b0Reader->SetFileName (b0_filename.c_str());

	RealImageFileReaderType::Pointer perReader = RealImageFileReaderType::New();
	perReader->SetFileName (PER_filename.c_str());

	RealImageFileReaderType::Pointer perFracReader = RealImageFileReaderType::New();
	perFracReader->SetFileName (PER_FRAC_filename.c_str());


	try
	{
		adcReader->Update();
		b0Reader->Update();
		perReader->Update();
		perFracReader->Update();

	}
	catch( itk::ExceptionObject & excep )
	{
		std::cerr << "Exception catched !" << std::endl;
		std::cerr << excep << std::endl;
	}

	RealImageType::IndexType index;
	index[0] = x;
	index[1] = y;
	index[2] = z;


	double adcVal = adcReader->GetOutput()->GetPixel(index);
	double b0Val = b0Reader->GetOutput()->GetPixel(index);
	double perVal = perReader->GetOutput()->GetPixel(index);
	double perFracVal = perFracReader->GetOutput()->GetPixel(index);

	std::cout << "model params:" << b0Val << ", " << perVal << ", " << adcVal << ", " << perFracVal <<std::endl;

	std::vector <ImageType::Pointer> bImages;


	BvaluesParser parser (bValuesNum, ImagesNamesFileName);

	vnl_vector<double> bValues (parser.getBValues().size());
	vnl_vector<double> measurementVec (parser.getBValues().size());
	vnl_vector<double> modelVec (parser.getBValues().size());

	for (size_t i=0;i<parser.getBValues().size();++i)
	{
		bValues(i) = parser.getBValues()[i];
		ImageFileReaderType::Pointer reader = ImageFileReaderType::New();
		reader->SetFileName(parser.getBValueImageName(i).c_str());
		try
		{
			reader->Update();
		}
		catch( itk::ExceptionObject & excep )
		{
			std::cerr << "Exception catched !" << std::endl;
			std::cerr << excep << std::endl;
		}
		measurementVec(i) = reader->GetOutput()->GetPixel(index);
		modelVec(i) = b0Val*
				            (perFracVal*vcl_exp(-bValues(i)*perVal) +
				             (1-perFracVal)*vcl_exp(-bValues(i)*(perVal+adcVal)));
	}

	std::fstream outFile;
	outFile.open (outputFileName.c_str(), std::ios::out);
	for (size_t i=0;i<parser.getBValues().size();++i)
	{
		outFile << bValues(i) << "\t" << measurementVec(i) << "\t" <<  modelVec(i) << std::endl;
	}
	outFile.close();



	return 0;
}
