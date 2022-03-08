#include <iostream>
#include <string>
#include <tclap/CmdLine.h>


#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>

#include <cstdio>


#include <fstream>

#include "crlBodyDiffusionTools.h"


typedef crlBodyDiffusionTools::MeasureMentVectorType MeasureMentVectorType;


void computeLinearADC  ( const MeasureMentVectorType & bvals, const MeasureMentVectorType &signal,
		                 double &b0, double &adc);

void computeTwoPointsADC  ( const MeasureMentVectorType & bvals, const MeasureMentVectorType &signal,
						    double &b0, double &adc );




#include "crlBodyDiffusionTools.h"


int main(int argc, char *argv[])
{

	int bValuesNum = 8;
	std::string InputFileName;


	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");
		TCLAP::ValueArg<int> BValuesNumArg("n","bValuesNum","bValuesNum",false,6,"int");
		cmd.add( BValuesNumArg );

		TCLAP::ValueArg<std::string> InputFileNameArg("i","InputFileName","InputFileName",true,"","string");
		cmd.add( InputFileNameArg );


		// Parse the argv array.
		cmd.parse( argc, argv );

		bValuesNum = BValuesNumArg.getValue();
		InputFileName = InputFileNameArg.getValue();




		}
		catch (TCLAP::ArgException &e)  // catch any exceptions
		{
			std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		}



		crlBodyDiffusionTools::MeasureMentVectorType bvals (bValuesNum);
		crlBodyDiffusionTools::MeasureMentVectorType Sb (bValuesNum);
		crlBodyDiffusionTools::MeasureMentVectorType weights (bValuesNum);
		crlBodyDiffusionTools::MeasureMentVectorType variance (bValuesNum);

		crlBodyDiffusionTools::MeasureMentVectorType bvals0_600 (2);
		crlBodyDiffusionTools::MeasureMentVectorType Sb_0_600 (2);

		crlBodyDiffusionTools::MeasureMentVectorType bvals0_50_800 (3);
		crlBodyDiffusionTools::MeasureMentVectorType Sb_0_50_800 (3);


		std::fstream inFile;
		inFile.open (InputFileName.c_str(), std::ios::in);


		for (size_t bIdx=0;bIdx<bValuesNum;++bIdx)
		{
			double bval, s;
			inFile >> bval >> s;

			bvals(bIdx) = bval;
			Sb (bIdx) = s;
			weights (bIdx) = 1;
			variance (bIdx) = 1;

			if (bval <=5)
			{
				bvals0_600(0)=bval;
				Sb_0_600(0)=s;

				bvals0_50_800(0)=bval;
				Sb_0_50_800(0)=s;

			}
			if (bval==600)
			{
				bvals0_600(1)=bval;
				Sb_0_600(1)=s;
			}
			if (bval==50)
			{
				bvals0_50_800(1)=bval;
				Sb_0_50_800(1)=s;
			}
			if (bval==800)
			{
				bvals0_50_800(2)=bval;
				Sb_0_50_800(2)=s;
			}

			//std::cout << bvals(bIdx) << ": " << Sb (bIdx) << std::endl;
		}

		inFile.close();

		weights.normalize();




		// compute ADCall model
		double ADCall=0, B0all=0;
		computeLinearADC (bvals,  Sb, B0all, ADCall );
		std::cout << B0all << "\t " << ADCall <<std::endl;


/*
		// compute ADC0_600 model
		double ADC_0_600=0, B0_0_600=0;
		computeLinearADC (bvals0_600,  Sb_0_600, B0_0_600, ADC_0_600 );
		std::cout << B0_0_600 << "\t " << ADC_0_600 <<std::endl;

		// compute ADC0_50_800 model
		double ADC_0_50_800=0, B0_0_50_800=0;
		computeLinearADC (bvals0_50_800,  Sb_0_50_800, B0_0_50_800, ADC_0_50_800 );
		std::cout << B0_0_50_800 << "\t " << ADC_0_50_800 <<std::endl;


*/


		double IVIM_B0 = 0,IVIM_Dstar = 0, IVIM_D = 0,IVIM_f = 0;


	// compute linear model
	crlBodyDiffusionTools::computeLinearModel (bvals,Sb,weights,IVIM_B0,IVIM_Dstar,IVIM_D,IVIM_f);
	crlBodyDiffusionTools::computeNonLinearModelLS (bvals,Sb,variance,weights,IVIM_B0,IVIM_Dstar,IVIM_D,IVIM_f);

	std::cout << IVIM_B0 << "\t" << IVIM_Dstar << "\t" << IVIM_D << "\t" << IVIM_f << std::endl;








	return 0;
}




void computeLinearADC  ( const MeasureMentVectorType & bvals, const MeasureMentVectorType &signal,
						 double &b0, double &adc )
{
	//****************** FOR 3 or more independent bvalues (siemenes) *********************

	//linear model
	//std::cout << "bval: " << bvals << std::endl;
	//std::cout << "signal: " << signal << std::endl;


	vnl_matrix <double> A (bvals.size(),2) ;
	vnl_vector <double> b (bvals.size());

	for (size_t i=0;i<bvals.size();++i)
	{
		A(i,0) = -bvals[i]; A(i,1) = 1;
		b(i) = vcl_log (signal[i]);
	}






	//std::cout << A << std::endl;
	//std::cout << b << std::endl;




	vnl_svd <double> lsq_svd_solver (A);
	vnl_vector<double> x = lsq_svd_solver.solve(b);

	if (!std::isnan (x(0)) &&
		!std::isinf (x(0)) &&
		!std::isnan (x(1)) &&
		!std::isinf (x(1))		)
	{

		adc = x(0);
		b0 = vcl_exp(x(1));
	}
	else
	{
		adc = 0 ;
		b0 = 0;
	}

}



//****************** FOR 2 bvalues (ge) *********************
void computeTwoPointsADC  ( const MeasureMentVectorType & bvals, const MeasureMentVectorType &signal,
						    double &b0, double &adc )

{

			//	std::cout << bvals[0] <<": " << signal[0] << ", " << bvals[1] << ": " <<signal[1] <<std::endl;

				// assume that bvals[0] = 0 and bvals[1] = bvalue
				b0 = signal[0];

				double diff = vcl_log(signal[1]) - vcl_log(signal[0]);
				double slope = diff/(bvals[0]-bvals[1]);
				adc = slope;


				//std::cout <<"diff: " << diff<< ", " << "adc: " << adc << std::endl;





}

