#include "crlBodyDiffusionTools.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <gsl/gsl_sf_bessel.h>



#include "itkDiffusionModelFittingMLECostFunction.h"
#include "itkDiffusionModelFittingSmoothedMLECostFunction.h"
//#include "itkDiffusionModelFittingCostFunction.h"
#include "itkDiffusionModelFittingCostFunctionLS.h"
#include "itkPerfusionCoeffCostFunction.h"
#include "itkADCMLECostFunction.h"
#include "itkNLOPTOptimizers.h"


#include "itkLevenbergMarquardtOptimizer.h"



const int crlBodyDiffusionTools::ADCTresh = 200;
const double crlBodyDiffusionTools::EPSILON = 0.000000000000001;
const int crlBodyDiffusionTools::EM_iters = 5;
const double crlBodyDiffusionTools::scales [4] = {1, 1000.0, 10000.0, 100.0};


class CommandIterationUpdateLevenbergMarquardt : public itk::Command
{
public:
  typedef  CommandIterationUpdateLevenbergMarquardt   Self;
  typedef  itk::Command                               Superclass;
  typedef itk::SmartPointer<Self>                     Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdateLevenbergMarquardt()
  {
    m_IterationNumber=0;
  }
public:
  typedef itk::LevenbergMarquardtOptimizer   OptimizerType;
  typedef   const OptimizerType   *          OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    std::cout << "Observer::Execute() " << std::endl;
      OptimizerPointer optimizer =
        dynamic_cast< OptimizerPointer >( object );
      if( m_FunctionEvent.CheckEvent( &event ) )
        {
        std::cout << m_IterationNumber++ << "   ";
        std::cout << optimizer->GetCachedValue() << "   ";
        std::cout << optimizer->GetCachedCurrentPosition() << std::endl;
        }


    }
private:
  unsigned long m_IterationNumber;

  itk::FunctionEvaluationIterationEvent m_FunctionEvent;
  itk::GradientEvaluationIterationEvent m_GradientEvent;
};


void
crlBodyDiffusionTools::computeBootstrapSE
                                (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
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
					  	  	     const int useScaling)
{


	const size_t m_NumberOfBootstarpIterations = numberOfBootstarpIterations;
	const size_t m_NumOfSamplePoints = 4;
	const size_t m_BValuesNum = bvals.size();


	MeasureMentVectorType residuals (m_BValuesNum);
	MeasureMentVectorType modelVals (m_BValuesNum);
	MeasureMentVectorType scaleVals (m_BValuesNum);

	for (size_t i=0;i<m_BValuesNum;++i)
	{
		modelVals(i) = computeModelEstimateAtBVal (b0,perfusionCoeff,diffusionCoeff,perfusionFractionCoeff,bvals(i));
		residuals(i) = Sb(i) - modelVals(i);

	//	std::cout << Sb(i) << ", " << modelVals(i) <<", " << residuals(i) <<std::endl;
	}

	// compute scented normalization factors
	// 1. generate sample points with sigma = 1




	 gsl_rng_env_setup();
	 const gsl_rng_type * m_gsl_rand_T = gsl_rng_default;
	 gsl_rng * m_gsl_rand_gen = gsl_rng_alloc (m_gsl_rand_T);


	 MeasureMentVectorType m_SamplePoints_b0 (m_NumOfSamplePoints*2);
	 MeasureMentVectorType m_SamplePoints_perfusion (m_NumOfSamplePoints*2);
	 MeasureMentVectorType m_SamplePoints_diffusion (m_NumOfSamplePoints*2);
	 MeasureMentVectorType m_SamplePoints_perfusionFraction (m_NumOfSamplePoints*2);

	/* MeasureMentVectorType bs_scales (4);
	 bs_scales[0] = 1;
	 bs_scales[1] = 1/perfusionCoeff;
	 bs_scales[2] = 1/diffusionCoeff;
	 bs_scales[3] = 1/perfusionFractionCoeff;
*/

	 for (size_t j=0;j<m_NumOfSamplePoints;++j)
	 {
	 	vnl_vector <double> point (m_NumOfSamplePoints);
	 	point.fill(0.0);
	 	/*point (j) = 1;
	 	m_SamplePoints_b0 (j) = point (0) / bs_scales[0];
	 	m_SamplePoints_perfusion (j) = point (1)/bs_scales[1];
	 	m_SamplePoints_diffusion (j) = point (2)/bs_scales[2];
		m_SamplePoints_perfusionFraction (j) = point (3)/bs_scales[3];

		point.fill(0.0);
	 	point (j) = -1;
	 	m_SamplePoints_b0 (m_NumOfSamplePoints+j) = point (0) / bs_scales[0];
	 	m_SamplePoints_perfusion (m_NumOfSamplePoints+j) = point (1)/bs_scales[1];
	 	m_SamplePoints_diffusion (m_NumOfSamplePoints+j) = point (2)/bs_scales[2];
	 	m_SamplePoints_perfusionFraction (m_NumOfSamplePoints+j) = point (3)/bs_scales[3];*/


	 	point (j) = 1;
	 	m_SamplePoints_b0 (j) = point (0) ;
	 	m_SamplePoints_perfusion (j) = point (1)*(0.001/vcl_sqrt(4.0));
	 	m_SamplePoints_diffusion (j) = point (2)*(0.0001/vcl_sqrt(4.0));
	 	m_SamplePoints_perfusionFraction (j) = point (3)*(0.01/vcl_sqrt(4.0));

	 	point.fill(0.0);
	 	point (j) = -1;
	 	m_SamplePoints_b0 (m_NumOfSamplePoints+j) = point (0) / vcl_sqrt(4.0);
	 	m_SamplePoints_perfusion (m_NumOfSamplePoints+j) = point (1)*(0.001/vcl_sqrt(4.0));
	 	m_SamplePoints_diffusion (m_NumOfSamplePoints+j) = point (2)*(0.0001/vcl_sqrt(4.0));
	 	m_SamplePoints_perfusionFraction (m_NumOfSamplePoints+j) = point (3)*(0.01/vcl_sqrt(4.0));
 	}

	//std::cout <<"sample points" << std::endl;
	//for (size_t j=0;j<2*m_NumOfSamplePoints;++j)
	//{
	//	std::cout << m_SamplePoints_b0(j) << ", " << m_SamplePoints_perfusion (j) << ", "
	//			  << m_SamplePoints_diffusion (j) << ", " << m_SamplePoints_perfusionFraction (j) << std::endl;
	//}



	// 2. propagate the points via the non-linear model to estimate the hetrosedactic variance at each point
	for (size_t i=0;i<m_BValuesNum;++i)
	{
		MeasureMentVectorType propagatedVals (m_NumOfSamplePoints*2);
		for (size_t j=0;j<m_NumOfSamplePoints*2;++j)
		{
			propagatedVals(j) = computeModelEstimateAtBVal (b0+m_SamplePoints_b0(j),
					                                        perfusionCoeff+(m_SamplePoints_perfusion(j)),
					                                        diffusionCoeff+(m_SamplePoints_diffusion(j)),
					                                        perfusionFractionCoeff+(m_SamplePoints_perfusionFraction(j)),
					                                        bvals(i));

		}



		//std::cout << "propagatedVals: " << propagatedVals << std::endl;
		scaleVals(i) = computeSTD (propagatedVals);
		//std::cout <<"std("<<i<<"): " << scaleVals(i)  << std::endl;


		//scaleVals(i) = vcl_sqrt(2);
		//std::cout <<"scaleVals("<<i<<"): " << scaleVals(i)  << std::endl;
	}




	//std::cout << "residuals.mean(): " << residuals.mean() << std::endl;
	residuals = residuals - residuals.mean();


	if (useScaling==0)
	{
		// do nothing
	}
	else if (useScaling == 1)
	{
		for (size_t i=0;i<m_BValuesNum;++i)
		{
			// std::cout <<"residuals before scale("<<i<<"): " << residuals(i)  << std::endl;
			scaleVals(i) = (1-vcl_sqrt(1/(scaleVals(i)*scaleVals(i))));
			residuals (i) =  residuals (i)*scaleVals(i);
			// std::cout <<"residuals after scale("<<i<<"): " << residuals(i)  << std::endl;
		}
	}
	else if (useScaling == 2)
	{
		for (size_t i=0;i<m_BValuesNum;++i)
		{
			// std::cout <<"residuals before scale("<<i<<"): " << residuals(i)  << std::endl;
			scaleVals(i) = vcl_sqrt((double)m_BValuesNum/double(m_BValuesNum-4));
			residuals (i) =  residuals (i)*scaleVals(i);
			// std::cout <<"residuals after scale("<<i<<"): " << residuals(i)  << std::endl;
		}
	}



	//std::cout << "residuals: " << residuals << std::endl;
	//residuals = residuals - residuals.mean();


	MeasureMentVectorType newSb (m_BValuesNum);
	MeasureMentVectorType b0_values (m_NumberOfBootstarpIterations);
	MeasureMentVectorType perfusion_values (m_NumberOfBootstarpIterations);
	MeasureMentVectorType diffusion_values (m_NumberOfBootstarpIterations);
	MeasureMentVectorType perfusionFraction_values (m_NumberOfBootstarpIterations);

	// start to generate wild samples
	//for (size_t iterIdx=0;iterIdx<1;++iterIdx)
	for (size_t iterIdx=0;iterIdx<m_NumberOfBootstarpIterations;++iterIdx)
	{
	//	std::cout <<"start bs iter: " << iterIdx << std::endl;

		for (size_t bvalIdx=0;bvalIdx<m_BValuesNum;++bvalIdx)
		{
			double newResidualWeight =   -1 + (static_cast<int> (2*gsl_rng_uniform_int (m_gsl_rand_gen,2)));
			// std::cout << "newResidualWeight: " << newResidualWeight << std::endl;
			newSb (bvalIdx) = modelVals(bvalIdx) +  newResidualWeight*residuals(bvalIdx);
		}

	//	std::cout <<"bs iter: " << iterIdx << "after residuals" << std::endl;
		double tmp_b0 = 0;
		double tmp_perfusionCoeff = 0.0;
		double tmp_diffusionCoeff = 0.0;
		double tmp_perfusionFractionCoeff = 0.0;


		// compute model

		MeasureMentVectorType tmp_weights (m_BValuesNum);
		for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
		{
			tmp_weights(bIdx) = 1/variance (bIdx);
		}

		computeLinearModel (bvals, newSb, tmp_weights,
							tmp_b0, tmp_perfusionCoeff, tmp_diffusionCoeff, tmp_perfusionFractionCoeff);

	//	std::cout <<"bs iter: " << iterIdx << "after linear model" << std::endl;
		computeNonLinearModel (bvals, newSb, variance, tmp_weights, 1,
		                       tmp_b0, tmp_perfusionCoeff, tmp_diffusionCoeff, tmp_perfusionFractionCoeff);
	//	std::cout <<"bs iter: " << iterIdx << "after non linear model" << std::endl;

		//std::cout << "newSb: " << newSb <<std::endl;
		//std::cout << "m_BValues: " << m_BValues <<std::endl;
		//std::cout << "weights: " << weights <<std::endl;

	//	std::cout << "model params: " << tmp_b0 << ", " << tmp_perfusionCoeff << ", "
	//		 	                      << tmp_diffusionCoeff << ", " << tmp_perfusionFractionCoeff << std::endl;
		b0_values(iterIdx) = tmp_b0;
		perfusion_values(iterIdx) = tmp_perfusionCoeff;
		diffusion_values(iterIdx) = tmp_diffusionCoeff;
		perfusionFraction_values(iterIdx) = tmp_perfusionFractionCoeff;


	}


//	std::cout << b0_values << "\t" << perfusion_values << "\t" << diffusion_values << "\t" << perfusionFraction_values << std::endl;


//	std::cout << "after SE" << std::endl;
//	b0 = b0_values.mean();
//	perfusionCoeff = perfusion_values.mean();
//	diffusionCoeff = diffusion_values.mean();
//	perfusionFractionCoeff = perfusionFraction_values.mean();


	double lo,hi;
	/*
	computeCI (b0_values,lo,hi);
	b0_se=hi; b0_se_lo=lo;

	computeCI (perfusion_values,lo,hi);
	perfusionCoeff_se=hi; perfusionCoeff_se_lo=lo;

	computeCI (diffusion_values,lo,hi);
	diffusionCoeff_se=hi; diffusionCoeff_se_lo=lo;

	computeCI (perfusionFraction_values,lo,hi);
	perfusionFractionCoeff_se=hi; perfusionFractionCoeff_se_lo=lo;
*/

	b0_se = b0_se_lo = computeSTD (b0_values);
	perfusionCoeff_se = perfusionCoeff_se_lo = computeSTD(perfusion_values);
	diffusionCoeff_se = diffusionCoeff_se_lo = computeSTD(diffusion_values);
	perfusionFractionCoeff_se = perfusionFractionCoeff_se_lo = computeSTD(perfusionFraction_values);

//	std::cout << b0_se << ", " << perfusionCoeff_se << ", " << diffusionCoeff_se << ", " << perfusionFractionCoeff_se <<std::endl;
//	std::cout << "after mean" << std::endl;

	gsl_rng_free(m_gsl_rand_gen);
//	std::cout << "after gsl_rng_free" << std::endl;
//	std::cout << "bstrap vars: " << b0_se << ", " << perfusionCoeff_se << ", " << diffusionCoeff_se << ", " << perfusionFractionCoeff_se << std::endl;

}

double
crlBodyDiffusionTools::computeModelEstimateAtBVal
                                    (double b0,
		                             double perfusionCoeff,
		                             double diffusionCoeff,
		                             double perfusionFractionCoeff,
		                             double bval)
 {
	  double res = b0*(perfusionFractionCoeff*vcl_exp(-bval*(perfusionCoeff+diffusionCoeff)) +
		               (1- perfusionFractionCoeff)*vcl_exp(-bval*(diffusionCoeff)));
	  return res;
 }



double crlBodyDiffusionTools::computeNonLinearModelGenetic
							   (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
								const MeasureMentVectorType &variance,
								const MeasureMentVectorType &weights,
								double & b0,
								double & perfusionCoeff,
								double & diffusionCoeff,
								double & perfusionFractionCoeff)
{

//	std::cout << "enter genetic opt" << std::endl;
	 gsl_rng_env_setup();
	 const gsl_rng_type * m_gsl_rand_T = gsl_rng_default;
	 gsl_rng * m_gsl_rand_gen = gsl_rng_alloc (m_gsl_rand_T);





	itk::NLOPTOptimizers::ParametersType t_params (4);
	t_params(0) = b0;
	t_params(1) = perfusionCoeff;
	t_params(2) = diffusionCoeff;
	t_params(3) = perfusionFractionCoeff;



	double ml_val = computeNonLinearModel (bvals,Sb, variance, weights,1,
						t_params(0),t_params(1), t_params(2), t_params(3));

	b0 = t_params(0);
	perfusionCoeff = t_params(1);
	diffusionCoeff = t_params(2);
	perfusionFractionCoeff = t_params(3);
	double max_val = ml_val;

//	std::cout << "t_params: " << t_params[0] << ", " << t_params[1] <<", " << t_params[2] << ", " << t_params[3] << std::endl;
//	std::cout << "initial ml_val:  " << max_val<< std::endl;

	for (int i=1;i<10;++i)
	{
		// generate random init
		itk::NLOPTOptimizers::ParametersType i_params (4);
		i_params[0] = t_params[0];
		i_params[1] = t_params[1];
		i_params[2] = t_params[2];
		i_params[3] = t_params[3];


		for (size_t j=0;j<i_params.size();++j)
		{
			double newResidual =   -1 + (static_cast<int> (2*gsl_rng_uniform_int (m_gsl_rand_gen,2)));
	//		std::cout << "newResidual: " << newResidual<< std::endl;
			i_params(j) = i_params(j) + 0.20*newResidual*i_params(j);
		}

	//	std::cout << "i_params 1: " << i_params[0] << ", " << i_params[1] <<", " << i_params[2] << ", " << i_params[3] << std::endl;


		ml_val = computeNonLinearModel (bvals,Sb, variance, weights,1,
											 i_params(0),i_params(1), i_params(2), i_params(3));

	//	std::cout << "i_params 2: " << i_params[0] << ", " << i_params[1] <<", " << i_params[2] << ", " << i_params[3] << std::endl;
	//	std::cout << "after opt ml_val:  " << ml_val << std::endl;

	//	std::cout << "i_params 2: " << i_params[0] << ", " << i_params[1] <<", " << i_params[2] << ", " << i_params[3] << std::endl;


		if (ml_val > max_val)
		{
			b0 = i_params(0);
			perfusionCoeff = i_params(1);
			diffusionCoeff = i_params(2);
			perfusionFractionCoeff = i_params(3);
			max_val = ml_val;
	//		std::cout << "change vals" << std::endl;
		}

	}

	//std::cout <<"end global opt" << std::endl;




//	std::cout <<"end finding max" << std::endl;
	gsl_rng_free(m_gsl_rand_gen);

	return max_val;





}

double
crlBodyDiffusionTools::computeSmoothedNonLinearModel (const MeasureMentVectorType & bvals, const MeasureMentVectorType & Sb,
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
												 double & perfusionFractionCoeff)
{
	typedef itk::DiffusionModelFittingSmoothedMLECostFunction<double> itkDiffusionModelFittingCostFunctionType;
	itkDiffusionModelFittingCostFunctionType::Pointer  modelCostFunc = itkDiffusionModelFittingCostFunctionType::New();

	modelCostFunc->SetObservedValues(bvals,Sb,variance, weights);
	modelCostFunc->SetNeighborsCurrentParams (neighbors_D,neighbors_Dstar,neighbors_f,neighbors_B0,neighbors_weights);
	modelCostFunc->SetnCoils (nCoils);

	itk::NLOPTOptimizers::ParametersType initParams (4);
	initParams (0) = b0;
	initParams (1) = perfusionCoeff;
	initParams (2) =  diffusionCoeff;
	initParams (3) = perfusionFractionCoeff;

	modelCostFunc->SetNumOfParams(initParams.size());
	modelCostFunc->Initialize();




	itk::NLOPTOptimizers::Pointer optimizer = itk::NLOPTOptimizers::New();
	optimizer->SetCostFunction(modelCostFunc);

		// Set the parameters
	optimizer->SetMaxEval(100);
	optimizer->SetAlgorithm(itk::NLOPTOptimizers::NLOPT_LN_BOBYQA);

		// Set the initial position
	optimizer->SetInitialPosition(initParams);

	itk::NLOPTOptimizers::ScalesType opt_scales (initParams.size());
	opt_scales [0] = scales[0];
	opt_scales [1] = scales[1];
	opt_scales [2] = scales[2];
	opt_scales [3] = scales[3];
	optimizer->SetScales( opt_scales);

	itk::NLOPTOptimizers::ParametersType LowerBoundParams (initParams.size());
	LowerBoundParams (0) = std::max(initParams (0)*0.25,0.0);
	LowerBoundParams (1) = std::max(initParams (1)*0.25,0.0);
	LowerBoundParams (2) = std::max(initParams (2)*0.25,0.0);
	LowerBoundParams (3) = std::max(initParams (3)*0.25,0.0);



	itk::NLOPTOptimizers::ParametersType UpperBoundParams (initParams.size());
	UpperBoundParams (0) = std::min(initParams (0)*1.75,1000.0);
	UpperBoundParams (1) = std::min(initParams (1)*1.75,1.0);
	UpperBoundParams (2) = std::min(initParams (2)*1.75,1.0);
	UpperBoundParams (3) = std::min(initParams (3)*1.75,1.0);




	optimizer->SetLowerBoundParameters(LowerBoundParams);
	optimizer->SetUpperBoundParameters(UpperBoundParams);


	optimizer->SetMaximize(false);
	optimizer->StartOptimization();

		//std::cout<<"Return code "<<(int)optimizer->GetErrorCode();
		//std::cout<<" = "<<optimizer->GetErrorCodeDescription()<<std::endl;
		//std::cout<<"Position = "<<optimizer->GetCurrentPosition()<<std::endl;
	if (optimizer->isSuccessful() )
	{

		const itk::NLOPTOptimizers::ParametersType & params = optimizer->GetCurrentPosition ();
		b0 = params (0);
		perfusionCoeff = params (1);
		diffusionCoeff = params (2);
		perfusionFractionCoeff = params (3);
		return optimizer->GetValue();
	}
	else
	{
		return -1;

	}
}

double
crlBodyDiffusionTools::computeNonLinearModel
                           (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
							const MeasureMentVectorType &variance,
							const MeasureMentVectorType &weights,
							const int nCoils,
			                double & b0,
			                double & perfusionCoeff,
			                double & diffusionCoeff,
			                double & perfusionFractionCoeff)
{


	typedef itk::DiffusionModelFittingMLECostFunction<double> itkDiffusionModelFittingCostFunctionType;
	itkDiffusionModelFittingCostFunctionType::Pointer  modelCostFunc = itkDiffusionModelFittingCostFunctionType::New();

	modelCostFunc->SetObservedValues(bvals,Sb,variance, weights);
	modelCostFunc->SetnCoils (nCoils);

	itk::NLOPTOptimizers::ParametersType initParams (4);
	initParams (0) = b0;
	initParams (1) = perfusionCoeff;
	initParams (2) =  diffusionCoeff;
	initParams (3) = perfusionFractionCoeff;

	modelCostFunc->SetNumOfParams(initParams.size());
	modelCostFunc->Initialize();




	itk::NLOPTOptimizers::Pointer optimizer = itk::NLOPTOptimizers::New();
	optimizer->SetCostFunction(modelCostFunc);

	// Set the parameters
	optimizer->SetMaxEval(200);

	// Set the initial position
	optimizer->SetInitialPosition(initParams);

	itk::NLOPTOptimizers::ScalesType opt_scales (initParams.size());
	opt_scales [0] = scales[0];
	opt_scales [1] = scales[1];
	opt_scales [2] = scales[2];
	opt_scales [3] = scales[3];
	optimizer->SetScales( opt_scales);

	itk::NLOPTOptimizers::ParametersType LowerBoundParams (initParams.size());
	LowerBoundParams (0) = std::max(initParams (0)*0.25,0.0);
	LowerBoundParams (1) = std::max(initParams (1)*0.25,0.0);
	LowerBoundParams (2) = std::max(initParams (2)*0.25,0.0);
	LowerBoundParams (3) = std::max(initParams (3)*0.25,0.0);



	itk::NLOPTOptimizers::ParametersType UpperBoundParams (initParams.size());
	UpperBoundParams (0) = std::min(initParams (0)*1.75,1000.0);
	UpperBoundParams (1) = std::min(initParams (1)*1.75,1.0);
	UpperBoundParams (2) = std::min(initParams (2)*1.75,1.0);
	UpperBoundParams (3) = std::min(initParams (3)*1.75,1.0);




	optimizer->SetLowerBoundParameters(LowerBoundParams);
	optimizer->SetUpperBoundParameters(UpperBoundParams);


	optimizer->SetMaximize(true);
	optimizer->SetAlgorithm(itk::NLOPTOptimizers::NLOPT_LN_BOBYQA);

	optimizer->StartOptimization();

	//std::cout<<"Return code "<<(int)optimizer->GetErrorCode();
	//std::cout<<" = "<<optimizer->GetErrorCodeDescription()<<std::endl;
	//std::cout<<"Position = "<<optimizer->GetCurrentPosition()<<std::endl;

	const itk::NLOPTOptimizers::ParametersType & params = optimizer->GetCurrentPosition ();
	b0 = params (0);
	perfusionCoeff = params (1);
	diffusionCoeff = params (2);
	perfusionFractionCoeff = params (3);

	return optimizer->GetValue();

}


double crlBodyDiffusionTools::computeNonLinearModelEM (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
									   const MeasureMentVectorType &variance,
									   const MeasureMentVectorType &weights,
									   double & b0,
									   double & perfusionCoeff,
									   double & diffusionCoeff,
									   double & perfusionFractionCoeff)
{
	MeasureMentVectorType int_weights = weights;

	for (int i=0; i< 15; ++i)
	{
		computeNonLinearModelLS(bvals,Sb,variance, int_weights,b0,perfusionCoeff,diffusionCoeff,perfusionFractionCoeff);
		computeWeights(bvals,Sb,variance, int_weights,b0,perfusionCoeff,diffusionCoeff,perfusionFractionCoeff );
		std::cout << "EM Iter " << i << ": " << b0 << ", " << perfusionCoeff<< ", " <<diffusionCoeff<< ", " <<perfusionFractionCoeff << std::endl;
		std::cout << "EM weights Iter " << i << int_weights << std::endl;
	}




}


double crlBodyDiffusionTools::computeNonLinearModelLSGenetic
							   (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
								const MeasureMentVectorType &variance,
								const MeasureMentVectorType &weights,
								double & b0,
								double & perfusionCoeff,
								double & diffusionCoeff,
								double & perfusionFractionCoeff)
{

	//	std::cout << "enter genetic opt" << std::endl;
		 gsl_rng_env_setup();
		 const gsl_rng_type * m_gsl_rand_T = gsl_rng_default;
		 gsl_rng * m_gsl_rand_gen = gsl_rng_alloc (m_gsl_rand_T);





		itk::NLOPTOptimizers::ParametersType t_params (4);
		t_params(0) = b0;
		t_params(1) = perfusionCoeff;
		t_params(2) = diffusionCoeff;
		t_params(3) = perfusionFractionCoeff;



		double ls_val = computeNonLinearModelLS (bvals,Sb, variance, weights,
							t_params(0),t_params(1), t_params(2), t_params(3));

		b0 = t_params(0);
		perfusionCoeff = t_params(1);
		diffusionCoeff = t_params(2);
		perfusionFractionCoeff = t_params(3);
		double min_val = ls_val;

		//std::cout << "t_params: " << t_params[0] << ", " << t_params[1] <<", " << t_params[2] << ", " << t_params[3] << std::endl;
	//	std::cout << "initial ls_val:  " << min_val<< std::endl;

		for (int i=1;i<10;++i)
		{
			// generate random init
			itk::NLOPTOptimizers::ParametersType i_params (4);
			i_params[0] = t_params[0];
			i_params[1] = t_params[1];
			i_params[2] = t_params[2];
			i_params[3] = t_params[3];


			for (int j=0;j<4;++j)
			{
				double newResidual =   -1 + (static_cast<int> (2*gsl_rng_uniform_int (m_gsl_rand_gen,2)));

				i_params(j) = i_params(j) + 0.2*newResidual*i_params(j);
			}

		//	std::cout << "i_params 1: " << i_params[0] << ", " << i_params[1] <<", " << i_params[2] << ", " << i_params[3] << std::endl;


			ls_val = computeNonLinearModelLS (bvals,Sb, variance, weights,
												 i_params(0),i_params(1), i_params(2), i_params(3));

	//		std::cout << "i_params 2: " << i_params[0] << ", " << i_params[1] <<", " << i_params[2] << ", " << i_params[3] << std::endl;
	//		std::cout << "after opt ml_val:  " << ls_val<< std::endl;

		//	std::cout << "i_params 2: " << i_params[0] << ", " << i_params[1] <<", " << i_params[2] << ", " << i_params[3] << std::endl;


			if (ls_val < min_val)
			{
				b0 = i_params(0);
				perfusionCoeff = i_params(1);
				diffusionCoeff = i_params(2);
				perfusionFractionCoeff = i_params(3);
				min_val = ls_val;
		//		std::cout << "change vals" << std::endl;
			}

		}

		//std::cout <<"end global opt" << std::endl;




	//	std::cout <<"end finding max" << std::endl;
		gsl_rng_free(m_gsl_rand_gen);

		return min_val;






}
double
crlBodyDiffusionTools::computeNonLinearModelLS
                           (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
							const MeasureMentVectorType &variance,
							const MeasureMentVectorType &weights,
			                double & b0,
			                double & perfusionCoeff,
			                double & diffusionCoeff,
			                double & perfusionFractionCoeff)
{

/*
	typedef itk::DiffusionModelFittingCostFunction itkDiffusionModelFittingCostFunctionType;
	itkDiffusionModelFittingCostFunctionType::Pointer  modelCostFunc = itkDiffusionModelFittingCostFunctionType::New();





	modelCostFunc->SetObservedValues(bvals,Sb,variance, weights);


	typedef itk::LevenbergMarquardtOptimizer itkOptimizerType;
	itkOptimizerType::ParametersType initParams (4);
	initParams (0) = b0;
	initParams (1) = perfusionCoeff;
	initParams (2) =  diffusionCoeff;
	initParams (3) = perfusionFractionCoeff;


	modelCostFunc->SetNumOfParams(initParams.size());
	modelCostFunc->Initialize();



	itkOptimizerType::Pointer optimizer = itkOptimizerType::New();
	optimizer->SetCostFunction(modelCostFunc);
	optimizer->SetInitialPosition( initParams );

	itkOptimizerType::ScalesType opt_scales (initParams.size());
	opt_scales [0] = scales[0];
	opt_scales [1] = scales[1];
	opt_scales [2] = scales[2];
	opt_scales [3] = scales[3];
	optimizer->SetScales( opt_scales);


	optimizer->SetDebug(true);


	  optimizer->SetNumberOfIterations(200);

	  optimizer->UseCostFunctionGradientOff();
	optimizer->StartOptimization();


	itkOptimizerType::ParametersType params = optimizer->GetCurrentPosition ();
	b0 = params (0);
	perfusionCoeff = params (1);
	diffusionCoeff = params (2);
	perfusionFractionCoeff = params (3);



	return optimizer->GetValue().sum();
			//modelCostFunc->GetValue(params).sum();



*/
	typedef itk::DiffusionModelFittingCostFunction <double> itkDiffusionModelFittingCostFunctionType;
	itkDiffusionModelFittingCostFunctionType::Pointer  modelCostFunc = itkDiffusionModelFittingCostFunctionType::New();





		modelCostFunc->SetObservedValues(bvals,Sb,variance, weights);


		itk::NLOPTOptimizers::ParametersType initParams (4);
		initParams (0) = b0;
		initParams (1) = perfusionCoeff;
		initParams (2) =  diffusionCoeff;
		initParams (3) = perfusionFractionCoeff;


		modelCostFunc->SetNumOfParams(initParams.size());
		modelCostFunc->Initialize();

		itk::NLOPTOptimizers::Pointer optimizer = itk::NLOPTOptimizers::New();
			optimizer->SetCostFunction(modelCostFunc);

			// Set the parameters
			optimizer->SetAlgorithm(itk::NLOPTOptimizers::NLOPT_LN_BOBYQA);

			// Set the initial position
			optimizer->SetInitialPosition(initParams);

			itk::NLOPTOptimizers::ScalesType opt_scales (initParams.size());
			opt_scales [0] = scales[0];
			opt_scales [1] = scales[1];
			opt_scales [2] = scales[2];
			opt_scales [3] = scales[3];
			optimizer->SetScales( opt_scales);

			itk::NLOPTOptimizers::ParametersType LowerBoundParams (initParams.size());
			LowerBoundParams (0) = std::max(initParams (0)*0.25,0.0);
			LowerBoundParams (1) = std::max(initParams (1)*0.25,0.0);
			LowerBoundParams (2) = std::max(initParams (2)*0.25,0.0);
			LowerBoundParams (3) = std::max(initParams (3)*0.25,0.0);



			itk::NLOPTOptimizers::ParametersType UpperBoundParams (initParams.size());
			UpperBoundParams (0) = std::min(initParams (0)*1.75,1000.0);
			UpperBoundParams (1) = std::min(initParams (1)*1.75,1.0);
			UpperBoundParams (2) = std::min(initParams (2)*1.75,1.0);
			UpperBoundParams (3) = std::min(initParams (3)*1.75,1.0);



			optimizer->SetMaxEval(200);
			optimizer->SetLowerBoundParameters(LowerBoundParams);
			optimizer->SetUpperBoundParameters(UpperBoundParams);


			optimizer->SetMaximize(false);
			optimizer->StartOptimization();


		//	std::cout<<"Return code "<<(int)optimizer->GetErrorCode();
		//	std::cout<<" = "<<optimizer->GetErrorCodeDescription()<<std::endl;
		//	std::cout<<"Position = "<<optimizer->GetCurrentPosition()<<std::endl;
		//	std::cout<<"Value = "<<optimizer->GetCurrentCost()<<std::endl;
		//	std::cout<<"Time = "<<optimizer->GetTimeForOptimization()<<" seconds"<<std::endl<<std::endl;

			const itk::NLOPTOptimizers::ParametersType & params = optimizer->GetCurrentPosition ();
			b0 = params (0);
			perfusionCoeff = params (1);
			diffusionCoeff = params (2);
			perfusionFractionCoeff = params (3);

			return optimizer->GetCurrentCost();



}



void
crlBodyDiffusionTools::computeWeights
                    (const MeasureMentVectorType &bvals,
		             const MeasureMentVectorType & Sb,
		             const MeasureMentVectorType & variance,
					 MeasureMentVectorType &weights,
		             const double & b0,
			         const double & perfusionCoeff,
			         const double & diffusionCoeff,
			         const double & perfusionFractionCoeff)
{


	for (size_t i=0;i<bvals.size();++i)
	{


		double modelVal = computeModelEstimateAtBVal (b0, perfusionCoeff,
		                                              diffusionCoeff,
		                                              perfusionFractionCoeff,
		                                              bvals(i));

		//double f_x = RicePDF (modelVal,Sb(i),variance(i));
		double f_x = GaussianPDF (modelVal,Sb(i),variance(i));
		weights(i) = f_x;

	}
	//std::cout << "weights: " << weights << std::endl;
	weights.normalize();
	//std::cout << "normalized weights: " << weights << std::endl;

}

double crlBodyDiffusionTools::estimateSimulatedNoiseVariance (const double & noiseSigma)
{
	 const int numOfSamples = 10000;
	 gsl_rng_env_setup();
	 const gsl_rng_type * m_gsl_rand_T = gsl_rng_default;
	 gsl_rng * m_gsl_rand_gen = gsl_rng_alloc (m_gsl_rand_T);
	 size_t  seed = time(NULL);
	 gsl_rng_set (m_gsl_rand_gen, seed);


	 double sum = 0.0;







	for (int i=0;i<numOfSamples;++i)
	{
		// add noise
		 double x = gsl_ran_gaussian (m_gsl_rand_gen,noiseSigma);
		 double y = gsl_ran_gaussian (m_gsl_rand_gen,noiseSigma);

		 sum +=  (x*x+y*y);
	}





	gsl_rng_free(m_gsl_rand_gen);

	return vcl_sqrt (sum/(2*numOfSamples));



}

void crlBodyDiffusionTools::test(){
	std::cout.precision(35);
	std::cout << "gsl_sf_bessel_I0(8): " << gsl_sf_bessel_I0(8) << std::endl;
	std::cout << "gsl_sf_bessel_I0_scaled(8): " << gsl_sf_bessel_I0_scaled(8) << std::endl;
	std::cout << "gsl_sf_bessel_I0_scaled(8)*exp(8): " << gsl_sf_bessel_I0_scaled(8)*vcl_exp(8) << std::endl;
}


double
crlBodyDiffusionTools::RicePDF (double v, double x,double sigma)
 {

	// verified in matlab on small numbers
	double Var = sigma*sigma;
	double f_x1 = vcl_log (x/Var)  + (-(x*x+v*v)/(2*Var));
	double f_x2 = vcl_log (gsl_sf_bessel_I0_scaled((x*v)/Var)) + (x*v/Var);
	return vcl_exp (f_x1+f_x2);
 }


double
crlBodyDiffusionTools::GaussianPDF (double v, double x,double sigma)
 {

	// verified in matlab on small numbers
	double Var = sigma*sigma;
	double f= (1/(2*3.1415*Var))*vcl_exp(-((x-v)*(x-v)/(2*Var)));
	return f;
 }


void crlBodyDiffusionTools::computeUniExpModel(const MeasureMentVectorType & bvals,
								   const MeasureMentVectorType & Sb,
								   const MeasureMentVectorType & weights,
								   double & B0, double & ADC,
								   const int ADC_Tresh)
{

	MeasureMentVectorType linearWeights (weights.size());
		for (size_t i=0;i<bvals.size();++i)
			{
				if (bvals[i] < ADC_Tresh)
				{
					linearWeights(i) = 0;
				}
				else
				{
					linearWeights(i) = weights(i);
				}
			}

			linearWeights.normalize();


			//initial fit of ADC and intercept (b0) from higher bvalues

			//count relevant b_vals
			vnl_vector<double> b(bvals.size()); //current protocol
			vnl_matrix <double> A (bvals.size(),2);

			for (size_t i=0;i<bvals.size();++i)
			{
				if (Sb[i] < EPSILON)
				{
					b(i) = 0;
				}
				else
				{
					b(i) = log(Sb[i])*linearWeights(i);
				}
				A(i,1) = -bvals[i]*linearWeights(i);
				A(i,0) = 1.0*linearWeights(i);

			}



			vnl_svd <double> lsq_svd_solver (A);
			vnl_vector<double> x = lsq_svd_solver.solve(b);
			if (!lsq_svd_solver.valid())
			{
				std::cout << "SVD Failed" <<std::endl;
				exit (EXIT_FAILURE);
			}





			B0 = exp(std::min (x(0),6.5));
			ADC = std::max (x(1),0.0);


}


void crlBodyDiffusionTools::computeLinearModel
                       (const MeasureMentVectorType & bvals,
						const MeasureMentVectorType & Sb,
						const MeasureMentVectorType & weights,
						double & B0,double & PER, double & ADC, double & PER_FRAC,
						const int ADC_Tresh)
{


	//std::cout << bvals << std::endl;
	//std::cout << weights << std::endl;
	//std::cout << Sb << std::endl;
	//std::cout << "enter linear model" << std::endl;
	MeasureMentVectorType linearWeights (weights.size());
	for (size_t i=0;i<bvals.size();++i)
		{

		if (bvals[i] < ADC_Tresh)
			{
				linearWeights(i) = 0;
			}
			else
			{
				linearWeights(i) = weights(i);
			}
		}


		linearWeights.normalize();


		//initial fit of ADC and intercept (b0) from higher bvalues

		//count relevant b_vals
		vnl_vector<double> b(bvals.size()); //current protocol
		vnl_matrix <double> A (bvals.size(),2);

		for (size_t i=0;i<bvals.size();++i)
		{
			if (Sb[i] < EPSILON)
			{
				b(i) = 0;
			}
			else
			{
				b(i) = log(Sb[i])*linearWeights(i);
			}
			A(i,1) = -bvals[i]*linearWeights(i);
			A(i,0) = 1.0*linearWeights(i);

		}




		vnl_svd <double> lsq_svd_solver (A);
		vnl_vector<double> x = lsq_svd_solver.solve(b);
		if (!lsq_svd_solver.valid())
		{
			std::cout << "SVD Failed" <<std::endl;
			exit (EXIT_FAILURE);
		}



	//	std::cout << "after adc" << std::endl;


		double initial_B0 = exp(std::min (x(0),6.5));
		double initial_ADC = std::max (x(1),0.0);


		double modelB5 = initial_B0*exp(bvals.min_value()*initial_ADC);
		//std::cout << "bp & adc: " << initial_B0 <<", " << initial_ADC <<std::endl;


		// from "Comparison of Biexponential and Monoexponential Model of
		//       Diffusion Weighted Imaging in Evaluation of Renal Lesions
		//       Preliminary Experience", Investigative Radiology 2010.

		//std::cout << modelB5 << " - " << Sb[0] << ": "<< initial_ADC << std::endl;


		double meanSb0 = 0.0;
		double weightsSum = 0.0;
		//std::cout <<"b0vals: " ;
		for (size_t i=0;i<bvals.size();++i)
		{
			if (bvals[i] ==  bvals.min_value())
			{

				meanSb0 += Sb[i]*weights(i);
				weightsSum += weights(i);
			}
		}

		meanSb0 = meanSb0 / weightsSum;





		double initial_f =  (meanSb0-modelB5)/meanSb0;
		//std::cout << initial_f << std::endl;
		double initial_PER, initial_full_B0;
		if (initial_f < 0 )
		{
			initial_f = 0;
			initial_PER = 0;
			initial_full_B0 = initial_B0;
		}

		else
		{


//			std::cout << "enter F" << std::endl;

			// svd initial est. of PER part
			MeasureMentVectorType fullWeights (weights.size());
			fullWeights = weights;




			// compute perfusion effect
			typedef itk::PerfusionCoeffCostFunction<double> itkPerfusionCoeffCostFunctionType;
			itkPerfusionCoeffCostFunctionType::Pointer  modelCostFunc = itkPerfusionCoeffCostFunctionType::New();
			//modelCostFunc->SetNumOfParams(1);

				//std::cout << "weights0 " << weights << std::endl;

				modelCostFunc->SetObservedValues(bvals,Sb, weights);

				modelCostFunc->SetFixedParams (meanSb0,initial_ADC,initial_f);


				//itkDiffusionModelFittingCostFunctionType::ParametersType params (4);
				itk::NLOPTOptimizers::ParametersType params (1);
				params (0) = 0.0; //arbitrary choice - to check



				modelCostFunc->Initialize();


				//std::cout << "initial cost: " << modelCostFunc->GetValue(params) << std::endl;

				itk::NLOPTOptimizers::Pointer optimizer = itk::NLOPTOptimizers::New();
				optimizer->SetCostFunction(modelCostFunc);

				itk::NLOPTOptimizers::ParametersType LowerBoundParams (1);
				LowerBoundParams (0) = 0;


				itk::NLOPTOptimizers::ParametersType UpperBoundParams (1);
				UpperBoundParams (0) = 0.1;




				optimizer->SetLowerBoundParameters(LowerBoundParams);
				optimizer->SetUpperBoundParameters(UpperBoundParams);


				// Set the parameters

				optimizer->SetAlgorithm(itk::NLOPTOptimizers::NLOPT_LN_BOBYQA);
				optimizer->SetInitialPosition(params);
				itk::NLOPTOptimizers::ScalesType opt_scales (params.size());
				opt_scales [0] = 1000;


				optimizer->SetScales( opt_scales);
				optimizer->SetMaximize(false);





				optimizer->SetDebug(false);
				optimizer->SetMaxEval(100);

				optimizer->StartOptimization();
		//		std::cout << "after dstar optimization" << std::endl;

				if (optimizer->isSuccessful() )
				{
					//std::cout << "before: " << params [0] << ", " << params[1] << ", " << params[2] << ", " << params[3] <<std::endl;

					params = optimizer->GetCurrentPosition ();
				//	std::cout <<"**************************** " <<  params (0) << " *************************" << std::endl;
					initial_PER = params (0);


					//std::cout << "after: " << params [0] << ", " << params[1] << ", " << params[2] << ", " << params[3] <<std::endl;
					//std::cout << optimizer->GetStopConditionDescription() << std::endl;
					//std::cout << optimizer->GetStopVal() << std::endl;

					//std::cout << "cost: " << modelCostFunc->GetValue(params) << std::endl;



				}
				else
				{
					std::cout <<"per optimizer failed" << std::endl;
					initial_PER = 0;
				}






		}


		//estimate b0 value from the model


		B0 = meanSb0;
		ADC = initial_ADC;
		PER = initial_PER;
		PER_FRAC = initial_f;

	//	std::cout << B0 << "\t" << PER << "\t" << ADC << "\t" << PER_FRAC << std::endl;


}

double
crlBodyDiffusionTools::computeSTD (const MeasureMentVectorType & v)
{
	double meanVal = v.mean();

	double std = 0;
	for (size_t i=0;i<v.size();++i)
	{
		std += ((v[i]-meanVal)*(v[i]-meanVal));
	}
	std = vcl_sqrt (std/(v.size()-1));

	return std;
}

void
crlBodyDiffusionTools::computeCI (const MeasureMentVectorType & v, double &lo, double &hi)
{
	int numOfItems=v.size();
	std::vector <double> tmp (v.begin(),v.end());
	std::sort(tmp.begin(), tmp.end());
	int lo_index = (int) floor(numOfItems*0.05);
	int hi_index = (int) floor(numOfItems*0.95);
	lo=tmp [lo_index];
	hi=tmp [hi_index];

}


void
crlBodyDiffusionTools::computeGlobalNonLinearModel
                       	   (const MeasureMentVectorType &bvals, const MeasureMentVectorType & Sb,
							const MeasureMentVectorType &variance,
							const MeasureMentVectorType &weights,
			                double & b0,
			                double & perfusionCoeff,
			                double & diffusionCoeff,
			                double & perfusionFractionCoeff)
{
	std::vector <int> minBforADC (6);
	minBforADC[0] = 5;
	minBforADC[1] = 50;
	minBforADC[2] = 100;
	minBforADC[3] = 200;
	minBforADC[4] = 260;
	minBforADC[5] = 400;

	double global_cost = -100000.0;

	double _b0 = b0;
	double _perfusionCoeff=perfusionCoeff;
	double _diffusionCoeff=diffusionCoeff;
	double _perfusionFractionCoeff=perfusionFractionCoeff;

	//std::cout << "weights: " << weights << std::endl;


	for (int i=0;i<6;++i)
	{
		MeasureMentVectorType _weights (weights);

		//std::cout << "_weights: " << _weights << std::endl;

		computeLinearModel (bvals, Sb, _weights,
							_b0, _perfusionCoeff, _diffusionCoeff, _perfusionFractionCoeff,
							minBforADC[i]);

		//std::cout << "init with " << minBforADC[i] << ": " <<
		//				     _b0  << ", " << _perfusionCoeff << ", " << _diffusionCoeff << ", " << _perfusionFractionCoeff << std::endl;

		computeWeights     (bvals, Sb, variance, _weights,
							_b0, _perfusionCoeff, _diffusionCoeff, _perfusionFractionCoeff);



		double tmp_cost = computeNonLinearModel
		                      (bvals, Sb, variance, _weights,1,
		  					   _b0, _perfusionCoeff, _diffusionCoeff, _perfusionFractionCoeff);

		//std::cout << tmp_cost << ": " <<
		//		     _b0  << ", " << _perfusionCoeff << ", " << _diffusionCoeff << ", " << _perfusionFractionCoeff << std::endl;
		if (tmp_cost > global_cost)
		{
			global_cost = tmp_cost;
			b0 = _b0;
			perfusionCoeff=_perfusionCoeff;
			diffusionCoeff=_diffusionCoeff;
			perfusionFractionCoeff=_perfusionFractionCoeff;
		}




	}


}


void
crlBodyDiffusionTools::computeSimulatedSignal (const MeasureMentVectorType &bvals,
			                                   const double & sigma,
			                                   const double & b0,
							                   const double & perfusionCoeff,
							                   const double & diffusionCoeff,
							                   const double & perfusionFractionCoeff,
							                   MeasureMentVectorType & Sb)
{
	 gsl_rng_env_setup();
	 const gsl_rng_type * m_gsl_rand_T = gsl_rng_default;
	 gsl_rng * m_gsl_rand_gen = gsl_rng_alloc (m_gsl_rand_T);
	 size_t  seed = time(NULL);
	 gsl_rng_set (m_gsl_rand_gen, seed);



	 for (size_t i=0;i<bvals.size();++i)
	 {
		 double val = computeModelEstimateAtBVal (b0,perfusionCoeff,diffusionCoeff,perfusionFractionCoeff, bvals[i]);

		 // add rician noise
		 double x = sigma*gsl_ran_gaussian (m_gsl_rand_gen,1) + val;
		 double y = sigma*gsl_ran_gaussian (m_gsl_rand_gen,1);
		 Sb[i] = vcl_sqrt (x*x+y*y);

		// add Gaussian noise
		// double noise = gsl_ran_gaussian (m_gsl_rand_gen,sigma);
		// val = noise + val;
		// Sb[i] = val;

	 }



	 gsl_rng_free(m_gsl_rand_gen);


}


void crlBodyDiffusionTools::computeUniExpModelML  ( const MeasureMentVectorType & bvals, const MeasureMentVectorType &signal,
			                                        const MeasureMentVectorType & variance,
		                                            double &B0, double &ADC, const int ADC_Tresh)
{

	typedef itk::ADCMLECostFunction<double> itkADCMLECostFunctionType;
	itkADCMLECostFunctionType::Pointer  modelCostFunc = itkADCMLECostFunctionType::New();
	modelCostFunc->SetObservedValues(bvals,signal,variance);
	modelCostFunc->SetMinBVal (ADC_Tresh);

	//itkDiffusionModelFittingCostFunctionType::ParametersType params (4);
	itk::NLOPTOptimizers::ParametersType params (2);
	params (0) = B0;
	params (1) =  ADC;



	modelCostFunc->SetNumOfParams(params.size());
	itk::NLOPTOptimizers::Pointer optimizer = itk::NLOPTOptimizers::New();
	optimizer->SetCostFunction(modelCostFunc);

	itk::NLOPTOptimizers::ParametersType LowerBoundParams (2);
	LowerBoundParams (0) = B0*0.5;
	LowerBoundParams (1) = ADC*0.5;


	itk::NLOPTOptimizers::ParametersType UpperBoundParams (2);
	UpperBoundParams (0) = std::min(B0*1.5,1000.0);
	UpperBoundParams (1) = std::min(ADC*1.5,0.01);

	//optimizer->SetLowerBoundParameters(LowerBoundParams);
	//optimizer->SetUpperBoundParameters(UpperBoundParams);


	// Set the parameters
	optimizer->SetMaxEval(100);
	optimizer->SetAlgorithm(itk::NLOPTOptimizers::NLOPT_LN_BOBYQA);
	optimizer->SetInitialPosition(params);
	itk::NLOPTOptimizers::ScalesType opt_scales (params.size());
	opt_scales [0] = 1;
	opt_scales [1] = 1000;


	optimizer->SetScales( opt_scales);
	optimizer->SetMaximize(true);







	optimizer->StartOptimization();



	if (optimizer->isSuccessful() )
	{
		//std::cout << "before: " << params [0] << ", " << params[1] << ", " << params[2] << ", " << params[3] <<std::endl;


		params = optimizer->GetCurrentPosition ();
		B0 = params (0);
		ADC = params (1);

		//std::cout << "after: " << params [0] << ", " << params[1] << ", " << params[2] << ", " << params[3] <<std::endl;

	}
	else
	{
		std::cout << optimizer->GetErrorCodeDescription() << std::endl;
	}

}

double crlBodyDiffusionTools::getSE (const MeasureMentVectorType & v)
{
	double meanVal = v.mean();

	double std = 0;
	for (size_t i=0;i<v.size();++i)
	{
		std += ((v[i]-meanVal)*(v[i]-meanVal));
	}
	std = vcl_sqrt (std/(v.size()-1));

	double SE = std/vcl_sqrt(v.size());
	return SE;
}




void crlBodyDiffusionTools::computeModelPartialDerAtBval (double b0,
										  double perfusionCoeff,
										  double diffusionCoeff,
										  double perfusionFractionCoeff,
										  double bval,
										  double & partial_s0,
										  double & partial_Dstar,
										  double & partial_D,
										  double & partial_f
									      )
{
	partial_s0 = perfusionFractionCoeff*vcl_exp(-bval*(perfusionCoeff+diffusionCoeff)) +
			     (1-perfusionFractionCoeff)*vcl_exp(-bval*diffusionCoeff);
	partial_f = b0*(vcl_exp(-bval*(perfusionCoeff+diffusionCoeff)) - vcl_exp(-bval*diffusionCoeff));
	partial_Dstar = -bval*b0*perfusionFractionCoeff*vcl_exp(-bval*(perfusionCoeff+diffusionCoeff));
	partial_D = -bval*b0*(perfusionFractionCoeff*vcl_exp(-bval*(perfusionCoeff+diffusionCoeff)) +
			     (1-perfusionFractionCoeff)*vcl_exp(-bval*diffusionCoeff));
}

double crlBodyDiffusionTools::computeRicianLikelihood (const MeasureMentVectorType & bvals,
													   const MeasureMentVectorType & Sb,
													   const MeasureMentVectorType & variance,
													   const MeasureMentVectorType & weights,
													   const double & B0,const double & PER, const double & ADC, const double & PER_FRAC)
{
	double sum1  = 0.0, sum2 = 0.0;

	for (size_t i=0;i<bvals.size();++i)
	{

		double M = Sb(i);
		double S = crlBodyDiffusionTools::computeModelEstimateAtBVal(B0,PER,ADC,PER_FRAC, bvals(i));

		double Var = variance(i)*variance(i);

		//std::cout  << "M,S,Var: " << M << ", " << S << ", " << Var << std::endl;

		double val1 = log (gsl_sf_bessel_I0_scaled((S*M)/Var)) +  ((S*M)/Var);
		double val2 = (S*S)/(2*Var);

		//std::cout << "vals: " << val1 << ", " << val2 << std::endl;

		sum1 +=   weights(i)*val1;
		sum2 +=  weights(i)*val2;
	}

	double likelihoodVal = sum1 - sum2;

	//std::cout << "likelihoodVal: " << sum1 << " - " << sum2 << " = " << likelihoodVal << std::endl;
	return  (likelihoodVal);


}



