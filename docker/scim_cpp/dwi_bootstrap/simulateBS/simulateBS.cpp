#include <iostream>
#include <fstream>
#include <string>
#include <tclap/CmdLine.h>


#include <vector>

#include "crlBodyDiffusionTools.h"




int main(int argc, char *argv[])
{


	//crlBodyDiffusionTools::test();

	int bValuesNum = 6;
	std::string orgParamsFileName;
	std::string bvalsFileName;
	std::string outputFileName;
	double noiseSigma = 0.0;
	int NumOfSimulations = 100;


	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");


		TCLAP::ValueArg<std::string> bvalsFileNameArg("i","bvalsFileName","b_values filename",true,"","string");
		cmd.add( bvalsFileNameArg );

		TCLAP::ValueArg<std::string> orgParamsFileNameArg("p","orgParamsFileName","orgParamsFileName",true,"","string");
		cmd.add( orgParamsFileNameArg );


		TCLAP::ValueArg<std::string> outputFileNameArg("o","outputFileName","outputFileName",true,"","string");
		cmd.add( outputFileNameArg );

		TCLAP::ValueArg<double> SigmaArg("s","sigma","sigma",true,5.0,"double");
		cmd.add( SigmaArg );

		TCLAP::ValueArg<int> NumOfSimulationsArg("n","numOfSim","numOfSim",true,100,"int");
		cmd.add( NumOfSimulationsArg );



		// Parse the argv array.
		cmd.parse( argc, argv );

		bvalsFileName = bvalsFileNameArg.getValue();
		orgParamsFileName = orgParamsFileNameArg.getValue();
		outputFileName = outputFileNameArg.getValue();
		noiseSigma = SigmaArg.getValue();
		NumOfSimulations = NumOfSimulationsArg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}


	std::fstream bvalsFile;
	bvalsFile.open (bvalsFileName.c_str(), std::ios::in);

	bvalsFile >> bValuesNum;
	crlBodyDiffusionTools::MeasureMentVectorType bvals (bValuesNum);
	for  (int i=0;i<bValuesNum;++i)
	{
		bvalsFile >> bvals[i];
	}
	bvalsFile.close();

	// std::cout << bvals << std::endl;




	double _b0, _perfusionCoeff, _diffusionCoeff, _perfusionFractionCoeff;
	std::fstream paramsFile;
	paramsFile.open (orgParamsFileName.c_str(), std::ios::in);
	paramsFile >> _b0 >> _perfusionCoeff >> _diffusionCoeff >> _perfusionFractionCoeff;
	paramsFile.close();

	std::fstream outputFile;
	outputFile.open (outputFileName.c_str(), std::ios::out);

	//outputFile <<  _b0 << ", " << _perfusionCoeff << ", " << _diffusionCoeff << ", " << _perfusionFractionCoeff << std::endl;

	crlBodyDiffusionTools::MeasureMentVectorType orgSb (bvals.size());

	for (size_t i=0;i<bvals.size();++i)
	{
		orgSb (i) = crlBodyDiffusionTools::computeModelEstimateAtBVal
		                                    (_b0,
				                             _perfusionCoeff,
				                             _diffusionCoeff,
				                             _perfusionFractionCoeff,
				                             bvals(i));
	}
	//std::cout << "org signal: " << orgSb << std::endl;

	double Variance = crlBodyDiffusionTools::estimateSimulatedNoiseVariance (noiseSigma);
	//std::cout << "Variance: " << Variance << std::endl;






	double b0_sls,perfusionCoeff_sls,diffusionCoeff_sls,perfusionFractionCoeff_sls;
	double b0_ml,perfusionCoeff_ml,diffusionCoeff_ml,perfusionFractionCoeff_ml;


	crlBodyDiffusionTools::MeasureMentVectorType Dvals (NumOfSimulations);
	crlBodyDiffusionTools::MeasureMentVectorType DSTARvals (NumOfSimulations);
	crlBodyDiffusionTools::MeasureMentVectorType B0vals (NumOfSimulations);
	crlBodyDiffusionTools::MeasureMentVectorType fvals (NumOfSimulations);

	//for (int simulationIdx=0;simulationIdx<1;++simulationIdx)
	for (int simulationIdx=0;simulationIdx<NumOfSimulations;++simulationIdx)
	{

		//std::cout << "simulationIdx: " << simulationIdx << std::endl;

		crlBodyDiffusionTools::MeasureMentVectorType Sb (bValuesNum);
		//Sb = orgSb;
		crlBodyDiffusionTools::computeSimulatedSignal(bvals, noiseSigma,
															  _b0, _perfusionCoeff, _diffusionCoeff, _perfusionFractionCoeff,
															  Sb);

	//	std::cout << "simulationIdx: " << simulationIdx <<  " after simulated signal" << std::endl;

		crlBodyDiffusionTools::MeasureMentVectorType weights (bValuesNum);
		weights.fill(1.0);
		weights.normalize();

		crlBodyDiffusionTools::MeasureMentVectorType variance (bValuesNum);
		variance.fill(Variance);



		crlBodyDiffusionTools::computeLinearModel(bvals,Sb,weights,b0_sls,perfusionCoeff_sls,diffusionCoeff_sls,perfusionFractionCoeff_sls, 200);

	//	std::cout << "simulationIdx: " << simulationIdx <<  " after linear model" << std::endl;
		b0_ml = b0_sls;
		perfusionCoeff_ml = perfusionCoeff_sls;
		diffusionCoeff_ml = diffusionCoeff_sls;
		perfusionFractionCoeff_ml = perfusionFractionCoeff_sls;


		crlBodyDiffusionTools::computeNonLinearModel(bvals,Sb,variance, weights,1,b0_ml,perfusionCoeff_ml,diffusionCoeff_ml,perfusionFractionCoeff_ml);
		//outputFile << b0_ml << ", " << perfusionCoeff_ml << ", " << diffusionCoeff_ml << ", " << perfusionFractionCoeff_ml << std::endl;
		B0vals [simulationIdx] = b0_ml;
		DSTARvals [simulationIdx] = perfusionCoeff_ml;
		Dvals [simulationIdx] = diffusionCoeff_ml;
		fvals [simulationIdx] = perfusionFractionCoeff_ml;

	//	std::cout << "simulationIdx: " << simulationIdx <<  " after non linear model" << std::endl;

		//for figure purpose only
	//	if (simulationIdx == 500)
		if (simulationIdx % 1000 == 0)
		{
			double b0_se, dstar_se,d_se,f_se,b0_se_lo, dstar_se_lo,d_se_lo,f_se_lo;

			double b0_t = b0_ml;
			double dstar_t = perfusionCoeff_ml;
			double d_t = diffusionCoeff_ml;
			double f_t = perfusionFractionCoeff_ml;

			outputFile << "0\t" << b0_ml << "\t" << perfusionCoeff_ml << "\t" << diffusionCoeff_ml << "\t" << perfusionFractionCoeff_ml << std::endl;

	//		std::cout << "simulationIdx: " << simulationIdx <<  " enter SE 1" << std::endl;
			crlBodyDiffusionTools::computeBootstrapSE (bvals,Sb,variance, weights,b0_t,dstar_t,d_t,f_t,b0_se, dstar_se,d_se,f_se,b0_se_lo, dstar_se_lo,d_se_lo,f_se_lo,1000, false);
			outputFile << "1\t" << b0_t << "\t" << dstar_t << "\t" << d_t << "\t" << f_t << std::endl;
			outputFile << "2\t" << b0_se << "\t" << dstar_se << "\t" << d_se << "\t" << f_se << std::endl;


			b0_t = b0_ml;
			dstar_t = perfusionCoeff_ml;
			d_t = diffusionCoeff_ml;
			f_t = perfusionFractionCoeff_ml;

	//		std::cout << "simulationIdx: " << simulationIdx <<  " enter SE 2" << std::endl;
			crlBodyDiffusionTools::computeBootstrapSE (bvals,Sb,variance, weights,b0_t,dstar_t,d_t,f_t,b0_se, dstar_se,d_se,f_se,b0_se_lo, dstar_se_lo,d_se_lo,f_se_lo,1000, true);
			outputFile << "3\t" << b0_t << "\t" << dstar_t << "\t" << d_t << "\t" << f_t << std::endl;
			outputFile << "4\t" << b0_se << "\t" << dstar_se << "\t" << d_se << "\t" << f_se << std::endl;

	//		std::cout << "simulationIdx: " << simulationIdx <<  " end SE 2" << std::endl;

		}

	}


	//std::cout << B0vals << "\t" << DSTARvals << "\t" << Dvals << "\t" << fvals << std::endl;

	outputFile << "6\t" << B0vals.mean() << "\t" << DSTARvals.mean() << "\t" << Dvals.mean() << "\t" << fvals.mean() << std::endl;
	outputFile << "7\t" << crlBodyDiffusionTools::getSE (B0vals) << "\t" << crlBodyDiffusionTools::getSE (DSTARvals) << "\t" << crlBodyDiffusionTools::getSE (Dvals) << "\t" << crlBodyDiffusionTools::getSE (fvals) << std::endl;


	outputFile.close();














	return 0;
}

