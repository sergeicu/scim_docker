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

	// outputFile <<  _b0 << ", " << _perfusionCoeff << ", " << _diffusionCoeff << ", " << _perfusionFractionCoeff << std::endl;
	std::cout << "org params: " <<  _b0 << ", " << _perfusionCoeff << ", " << _diffusionCoeff << ", " << _perfusionFractionCoeff << std::endl;

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
	std::cout << "org signal: " << orgSb << std::endl;

	double Variance = crlBodyDiffusionTools::estimateSimulatedNoiseVariance (noiseSigma);
	std::cout << "Variance: " << Variance << std::endl;






		double b0_sls,perfusionCoeff_sls,diffusionCoeff_sls,perfusionFractionCoeff_sls;
		double b0_ls,perfusionCoeff_ls,diffusionCoeff_ls,perfusionFractionCoeff_ls;
		double b0_ml,perfusionCoeff_ml,diffusionCoeff_ml,perfusionFractionCoeff_ml;
		double b0_em,perfusionCoeff_em,diffusionCoeff_em,perfusionFractionCoeff_em;

		for (int simulationIdx=0;simulationIdx<NumOfSimulations;++simulationIdx)
		{
			crlBodyDiffusionTools::MeasureMentVectorType Sb (bValuesNum);
			//Sb = orgSb;
			crlBodyDiffusionTools::computeSimulatedSignal(bvals, noiseSigma,
																  _b0, _perfusionCoeff, _diffusionCoeff, _perfusionFractionCoeff,
																  Sb);

			std::cout << "Curropted Signal: " << Sb << std::endl;

			crlBodyDiffusionTools::MeasureMentVectorType weights (bValuesNum);
			weights.fill(1.0);
			weights.normalize();

			crlBodyDiffusionTools::MeasureMentVectorType variance (bValuesNum);
			variance.fill(Variance);



			crlBodyDiffusionTools::computeLinearModel(bvals,Sb,weights,b0_sls,perfusionCoeff_sls,diffusionCoeff_sls,perfusionFractionCoeff_sls, 200);

			//outputFile << b0_sls << ", " << perfusionCoeff_sls << ", " << diffusionCoeff_sls << ", " << perfusionFractionCoeff_sls << std::endl;
			//b0_ls = b0_sls;
			//perfusionCoeff_ls = perfusionCoeff_sls;
			//diffusionCoeff_ls = diffusionCoeff_sls;
			//perfusionFractionCoeff_ls = perfusionFractionCoeff_sls;

			//crlBodyDiffusionTools::computeWeights(bvals,Sb,variance, weights,b0_ls,perfusionCoeff_ls,diffusionCoeff_ls,perfusionFractionCoeff_ls);

			//crlBodyDiffusionTools::computeNonLinearModelLS(bvals,Sb,variance, weights,b0_ls,perfusionCoeff_ls,diffusionCoeff_ls,perfusionFractionCoeff_ls);
			//outputFile << b0_ls << ", " << perfusionCoeff_ls << ", " << diffusionCoeff_ls << ", " << perfusionFractionCoeff_ls << std::endl;





			b0_ml = b0_sls;
			perfusionCoeff_ml = perfusionCoeff_sls;
			diffusionCoeff_ml = diffusionCoeff_sls;
			perfusionFractionCoeff_ml = perfusionFractionCoeff_sls;
     		//crlBodyDiffusionTools::computeWeights(bvals,Sb,variance, weights,b0_ml,perfusionCoeff_ml,diffusionCoeff_ml,perfusionFractionCoeff_ml);

			crlBodyDiffusionTools::computeNonLinearModelLS(bvals,Sb,variance, weights,b0_ml,perfusionCoeff_ml,diffusionCoeff_ml,perfusionFractionCoeff_ml);
			//outputFile << "variance: " << variance << std::endl;
			//outputFile << "weights: " << weights << std::endl;

			//outputFile << b0_ml << ", " << perfusionCoeff_ml << ", " << diffusionCoeff_ml << ", " << perfusionFractionCoeff_ml << std::endl;
			std::cout << "ML: " << b0_ml << ", " << perfusionCoeff_ml << ", " << diffusionCoeff_ml << ", " << perfusionFractionCoeff_ml << std::endl;
			std::cout << "ML weights: " << weights << std::endl;


			b0_em = b0_sls;
			perfusionCoeff_em = perfusionCoeff_sls;
			diffusionCoeff_em = diffusionCoeff_sls;
			perfusionFractionCoeff_em = perfusionFractionCoeff_sls;

			crlBodyDiffusionTools::computeNonLinearModelEM(bvals,Sb,variance, weights,b0_em,perfusionCoeff_em,diffusionCoeff_em,perfusionFractionCoeff_em);
			//outputFile << "variance: " << variance << std::endl;
			//outputFile << "weights: " << weights << std::endl;

			//outputFile << b0_em << ", " << perfusionCoeff_em << ", " << diffusionCoeff_em << ", " << perfusionFractionCoeff_em << std::endl;
			std::cout << "EM: " << b0_em << ", " << perfusionCoeff_em << ", " << diffusionCoeff_em << ", " << perfusionFractionCoeff_em << std::endl;
			std::cout << "ML weights: " << weights << std::endl;



		}

		outputFile.close();
/*








*/






	return 0;
}
