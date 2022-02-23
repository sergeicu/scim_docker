#ifndef _crlBodyFiffusionTools_h_
#define _crlBodyFiffusionTools_h_


#include <vnl/vnl_vector.h>
#include <gsl/gsl_sf_bessel.h>

#include "itkVariableLengthVector.h"

class crlBodyDiffusionTools
{
public:

	typedef vnl_vector <double> MeasureMentVectorType;





	static const int ADCTresh;
	static const double EPSILON;
	static const int EM_iters;
	static const double scales [4];



	static double computeRicianLikelihood (const MeasureMentVectorType & bvals,
										   const MeasureMentVectorType & Sb,
										   const MeasureMentVectorType & variance,
										   const MeasureMentVectorType & weights,
										   const double & B0,const double & PER, const double & ADC, const double & PER_FRAC);

	static void computeModelPartialDerAtBval (double b0,
										        double perfusionCoeff,
												double diffusionCoeff,
												double perfusionFractionCoeff,
												double bval,
												double & partial_s0,
												double & partial_Dstar,
												double & partial_D,
												double & partial_f
												);


	static double computeModelEstimateAtBVal (double b0,
		                                      double perfusionCoeff,
		                                      double diffusionCoeff,
		                                      double perfusionFractionCoeff,
		                                      double bval);

	static void computeLinearModel(const MeasureMentVectorType & bvals,
								   const MeasureMentVectorType & Sb,
								   const MeasureMentVectorType & weights,
								   double & B0,double & PER, double & ADC, double & PER_FRAC,
								   const int ADC_Tresh=ADCTresh);

	static void computeUniExpModel(const MeasureMentVectorType & bvals,
								   const MeasureMentVectorType & Sb,
								   const MeasureMentVectorType & weights,
								   double & B0, double & ADC,
								   const int ADC_Tresh=ADCTresh);

	static void computeUniExpModelML  ( const MeasureMentVectorType & bvals, const MeasureMentVectorType &signal,
			                            const MeasureMentVectorType & variance,
		                                double &B0, double &ADC,const int ADC_Tresh=ADCTresh );



	static void computeWeights (const MeasureMentVectorType &bvals,
								const MeasureMentVectorType & Sb,
								const MeasureMentVectorType & variance,
								MeasureMentVectorType &weights,
								const double & b0,
								const double & perfusionCoeff,
								const double & diffusionCoeff,
								const double & perfusionFractionCoeff);

	static double RicePDF (double v, double x,double sigma);
	static double GaussianPDF (double v, double x,double sigma);

	static double computeNonLinearModel (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
									   const MeasureMentVectorType &variance,
									   const MeasureMentVectorType &weights,
									   const int nCoils,
									   double & b0,
									   double & perfusionCoeff,
									   double & diffusionCoeff,
									   double & perfusionFractionCoeff);

	static double computeSmoothedNonLinearModel (const MeasureMentVectorType & bvals, const MeasureMentVectorType & Sb,
										         const MeasureMentVectorType & variance,
												 const MeasureMentVectorType & weights,
												 const MeasureMentVectorType & neighbors_D,
												 const MeasureMentVectorType & neighbors_Dstar,
												 const MeasureMentVectorType & neighbors_f,
												 const MeasureMentVectorType & neighbors_B0,
												 const MeasureMentVectorType & neighbors_weights,
												 const int nCoils,
												 double & b0,
												 double & perfusionCoeff,
												 double & diffusionCoeff,
												 double & perfusionFractionCoeff);

	static double computeNonLinearModelEM (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
										   const MeasureMentVectorType &variance,
										   const MeasureMentVectorType &weights,
										   double & b0,
										   double & perfusionCoeff,
										   double & diffusionCoeff,
										   double & perfusionFractionCoeff);

	static double computeNonLinearModelGenetic
								   (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
									const MeasureMentVectorType &variance,
									const MeasureMentVectorType &weights,
									double & b0,
									double & perfusionCoeff,
									double & diffusionCoeff,
									double & perfusionFractionCoeff);

	static double computeNonLinearModelLS (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
										   const MeasureMentVectorType &variance,
										   const MeasureMentVectorType &weights,
										   double & b0,
										   double & perfusionCoeff,
										   double & diffusionCoeff,
										   double & perfusionFractionCoeff);

	static double computeNonLinearModelLSGenetic
									   (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
										const MeasureMentVectorType &variance,
										const MeasureMentVectorType &weights,
										double & b0,
										double & perfusionCoeff,
										double & diffusionCoeff,
										double & perfusionFractionCoeff);


	static double estimateSimulatedNoiseVariance (const double & noiseSigma);


	static void test ();

	static void
	computeGlobalNonLinearModel
	                       	   (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
								const MeasureMentVectorType &variance,
								const MeasureMentVectorType &weights,
				                double & b0,
				                double & perfusionCoeff,
				                double & diffusionCoeff,
				                double & perfusionFractionCoeff);


	static void
	computeBootstrapSE              (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
									 const MeasureMentVectorType &variance,const MeasureMentVectorType &weights,
									 double & b0,
									 double & perfusionCoeff,
									 double & diffusionCoeff,
									 double & perfusionFractionCoeff,
									 double & b0_se,
									 double & perfusionCoeff_se,
									 double & diffusionCoeff_se,
									 double & perfusionFractionCoeff_se,
									 double & b0_se_lo,
									 double & perfusionCoeff_se_lo,
									 double & diffusionCoeff_se_lo,
									 double & perfusionFractionCoeff_se_lo,
									 const size_t numberOfBootstarpIterations,
									 const int useScaling);




	static void
	computeSimulatedSignal (const MeasureMentVectorType &bvals,
	                        const double & sigma,
	                        const double & b0,
	                        const double & perfusionCoeff,
			                const double & diffusionCoeff,
			                const double & perfusionFractionCoeff,
			                MeasureMentVectorType & Sb);

	static double
	computeSTD (const MeasureMentVectorType & vec);

	static void computeCI (const MeasureMentVectorType & v, double &lo, double &hi);

	static double getSE (const MeasureMentVectorType & v);
};


#endif // _crlBodyFiffusionTools_h_
