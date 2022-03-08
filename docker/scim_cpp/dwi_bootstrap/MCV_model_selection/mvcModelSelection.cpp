#include <iostream>
#include <fstream>
#include <string>
#include <tclap/CmdLine.h>
#include <vector>

#include "crlBodyDiffusionTools.h"


struct CaseData
{
	std::vector<int> bvals;
	std::vector<double> signals;


	CaseData (){}
	CaseData (const CaseData& t) {bvals = t.bvals;  signals=t.signals;}
};


//  The following section of code implements a Command observer
//  that will monitor the evolution of the registration process.


int main(int argc, char *argv[])
{



	std::string casesFileNames;



	try
	{
		TCLAP::CmdLine cmd("Written by: Moti Freiman (moti.freiman@childrens.harvard.edu)", ' ', "0.1");



		TCLAP::ValueArg<std::string> casesFileNamesArg("i","casesFileNames","casesFileNames",true,"","string");
		cmd.add( casesFileNamesArg );

		// Parse the argv array.
		cmd.parse( argc, argv );

		casesFileNames = casesFileNamesArg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}


	std::vector <int> caseIdxVec;

	std::fstream casesListFile;
	casesListFile.open (casesFileNames.c_str(), std::ios::in);

	while (true)
	{
	    int caseIdx;
	    casesListFile >> caseIdx;
	    if( casesListFile.eof() ) break;
	    caseIdxVec.push_back (caseIdx);
	}
	casesListFile.close();


	int numOfCases=caseIdxVec.size();

	//std::cout << "numOfCases: " << numOfCases << std::endl;

	std::vector<CaseData> caseDataVec;

	for (int i=0;i<numOfCases;++i)
	{
		CaseData tmpCase;
		char caseFilename [255];
		sprintf (caseFilename, "case%d.txt",  caseIdxVec[i]);
		//std::cout << "caseFilename: " << caseFilename << std::endl;
		std::fstream caseFile;
		caseFile.open (caseFilename, std::ios::in);
		while (true)
		{
		    int tmpBval;
		    double tmpSignal;
		    caseFile >> tmpBval >> tmpSignal;

		    if( caseFile.eof() ) break;
		    tmpCase.bvals.push_back (tmpBval);
		    tmpCase.signals.push_back (tmpSignal);


		}
		caseFile.close();

		caseDataVec.push_back (tmpCase);


	}



	for (int testIdx=0;testIdx<numOfCases;++testIdx)
	{
		std::vector<int> trainBvals;
		std::vector<double> trainSignals;

		std::vector<int> testBvals;
		std::vector<double> testSignals;



		for (int caseIdx=0;caseIdx<numOfCases;++caseIdx)
		{
			if (caseIdx==testIdx)
			{
				//add to test

				for (size_t i=0;i<caseDataVec[caseIdx].bvals.size();++i)
				{
					testBvals.push_back(caseDataVec[caseIdx].bvals[i]);
					testSignals.push_back(caseDataVec[caseIdx].signals[i]);


				}


			}
			else
			{
				//add to training

				for (size_t i=0;i<caseDataVec[caseIdx].bvals.size();++i)
				{
					trainBvals.push_back(caseDataVec[caseIdx].bvals[i]);
					trainSignals.push_back(caseDataVec[caseIdx].signals[i]);

				}
			}
		}





		int trainSize = trainSignals.size();
		int testSize = testSignals.size();



	//	std::cout <<"trainSize: " << trainSize << std::endl;
	//	std::cout <<"testSize: " << testSize << std::endl;

		crlBodyDiffusionTools::MeasureMentVectorType trainBvalsVec (trainSize);
		crlBodyDiffusionTools::MeasureMentVectorType trainSignalsVec (trainSize);

		for (int i=0;i<trainSize;++i)
		{
			trainBvalsVec(i) = trainBvals[i];
			trainSignalsVec(i) = trainSignals[i];
		}





		// fit ivim model to the data
		double ivim_s0=0,ivim_dstar=0, ivim_d=0,ivim_f=0;
		crlBodyDiffusionTools::MeasureMentVectorType weights (trainSize);
		weights.fill (1.0);
		crlBodyDiffusionTools::MeasureMentVectorType variance (trainSize);
		variance.fill (2.0);

		crlBodyDiffusionTools::computeLinearModel   (trainBvalsVec, trainSignalsVec, weights,
													 ivim_s0,ivim_dstar,ivim_d,ivim_f, 200);
		crlBodyDiffusionTools::computeNonLinearModelLS   (trainBvalsVec, trainSignalsVec, weights, variance,
															 ivim_s0,ivim_dstar,ivim_d,ivim_f);
		crlBodyDiffusionTools::computeNonLinearModel   (trainBvalsVec, trainSignalsVec, weights, variance,1,
																	 ivim_s0,ivim_dstar,ivim_d,ivim_f);
		//std::cout << ivim_s0 << ", " << ivim_dstar<< ", " << ivim_d<< ", " << ivim_f <<std::endl;


		// fit adc model to the data
		double adc_s0=0, adc_d = 0;
		crlBodyDiffusionTools::computeUniExpModel(trainBvalsVec, trainSignalsVec, weights,
												  adc_s0, adc_d, -1);
		//std::cout << adc_s0 << ", " << adc_d<< std::endl;

		double ADC_RSS=0, IVIM_RSS=0;
		for (int i=0;i<testSize;++i)
		{

			double bval = testBvals[i];
			double ivim_model_val = (ivim_f*(vcl_exp(-bval*(ivim_d+ivim_dstar)))+(1-ivim_f)*(vcl_exp(-bval*ivim_d)));
			double adc_model_val = (vcl_exp(-bval*adc_d));

			double normalizedTestSignal=testSignals[i]/testSignals[0];
			ADC_RSS += (adc_model_val-normalizedTestSignal)*(adc_model_val-normalizedTestSignal);
			IVIM_RSS += (ivim_model_val-normalizedTestSignal)*(ivim_model_val-normalizedTestSignal);
		}

		ADC_RSS = ADC_RSS / testSize;
		IVIM_RSS = IVIM_RSS / testSize;

		std::cout << vcl_sqrt(ADC_RSS) << "\t " << vcl_sqrt(IVIM_RSS) << std::endl;
	}



	return 0;
}
