/*
 * DiffusionModelFittingSmoothedMLECostFunction.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: ch145864
 */
#include <cmath>
#include <vnl/vnl_math.h>
#include <gsl/gsl_sf_bessel.h>
#include "itkDiffusionModelFittingSmoothedMLECostFunction.h"

#include <iostream>


/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPointSetToPointSetMetric.txx,v $
  Language:  C++
  Date:      $Date: 2003-11-08 17:58:32 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkDiffusionModelFittingSmoothedMLECostFunction_cxx
#define _itkDiffusionModelFittingSmoothedMLECostFunction_txx

#include "itkDiffusionModelFittingSmoothedMLECostFunction.h"
#include "crlBodyDiffusionTools.h"

namespace itk
{

template <typename TMeasure>
const int DiffusionModelFittingSmoothedMLECostFunction<TMeasure>
::BACKGROUND = 0;

template <typename TMeasure>
const int DiffusionModelFittingSmoothedMLECostFunction<TMeasure>
::FOREGROUND = 1;



/** Constructor */
template <typename TMeasure>
DiffusionModelFittingSmoothedMLECostFunction<TMeasure>
::DiffusionModelFittingSmoothedMLECostFunction() :
 _b_vals (),
 _b_signals (),
 _b_variance (),
 _b_weights (),
 _neighbors_D (),
 _neighbors_Dstar (),
 _neighbors_f (),
 _neighbors_B0 (),
 _neighbors_Weights (),
 m_NumOfParams (4),
 m_nCoils (1)
{

	IVIMSquaredScales[0] = 1.0;
	IVIMSquaredScales[1] = 0.001;
	IVIMSquaredScales[2] = 0.0001;
	IVIMSquaredScales[3] = 0.01;

}


template <typename TMeasure>
void DiffusionModelFittingSmoothedMLECostFunction<TMeasure>::SetNeighborsCurrentParams
                 (const vnl_vector<TMeasure> & neighbors_D,
				  const vnl_vector<TMeasure> & neighbors_Dstar,
				  const vnl_vector<TMeasure> & neighbors_f,
				  const vnl_vector<TMeasure> & neighbors_B0,
				  const vnl_vector<TMeasure> & neighbors_Weights)
{
	_neighbors_D = neighbors_D;
	_neighbors_Dstar = neighbors_Dstar;
	_neighbors_f = neighbors_f;
	_neighbors_B0 = neighbors_B0;
	_neighbors_Weights = neighbors_Weights;

}


template <typename TMeasure>
void DiffusionModelFittingSmoothedMLECostFunction<TMeasure>::SetObservedValues
				(const vnl_vector<TMeasure> & b_vals,
				 const vnl_vector<TMeasure> & b_signals,
				 const vnl_vector<TMeasure> & b_variance,
				 const vnl_vector<TMeasure> & b_weights)
{
	_b_vals = b_vals;
	_b_signals = b_signals;
	_b_variance = b_variance;
	_b_weights = b_weights;
}

template <typename TMeasure>
typename DiffusionModelFittingSmoothedMLECostFunction<TMeasure>::MeasureType
DiffusionModelFittingSmoothedMLECostFunction<TMeasure>::GetValue (const ParametersType & params) const
{
	//likelihood term
	TMeasure s0  = params(0);
	TMeasure PerCoeff = params (1);
	TMeasure DiffCoeff = params (2);
	TMeasure PerFracCoeff = params (3);

	TMeasure likelihoodVal = 0;



	if (m_nCoils == 1)
		// rician model
		{
			//std::cout << "params in f(): " << params << std::endl;

			//std::cout << "weights: " << _b_weights << std::endl;
			MeasureType sum1  = 0.0, sum2 = 0.0;
			for (size_t i=0;i<_b_vals.size();++i)
			{

				TMeasure M = _b_signals(i);
				TMeasure S = crlBodyDiffusionTools::computeModelEstimateAtBVal(s0,PerCoeff,DiffCoeff,PerFracCoeff, _b_vals(i));

				TMeasure Var = _b_variance(i)*_b_variance(i);

				//std::cout  << "M,S,Var: " << M << ", " << S << ", " << Var << std::endl;

				TMeasure val1 = log (gsl_sf_bessel_I0_scaled((S*M)/Var)) +  ((S*M)/Var);
				TMeasure val2 = (S*S)/(2*Var);

				//std::cout << "vals: " << val1 << ", " << val2 << std::endl;

				sum1 +=   _b_weights(i)*val1;
				sum2 +=  _b_weights(i)*val2;
			}
			likelihoodVal = sum1 - sum2;
		}
		else if (m_nCoils > 1)
		{
			// ncx2 noise model
			MeasureType sum1  = 0.0, sum2 = 0.0, sum3 = 0.0;

			for (size_t i=0;i<_b_vals.size();++i)
			{

				TMeasure M = _b_signals(i);
				TMeasure S = crlBodyDiffusionTools::computeModelEstimateAtBVal(s0,PerCoeff,DiffCoeff,PerFracCoeff, _b_vals(i));

				TMeasure Var = _b_variance(i)*_b_variance(i);

				//std::cout  << "M,S,Var: " << M << ", " << S << ", " << Var << std::endl;

				if (S < 1e-10)
				{
					continue;
				}
				else
				{
					TMeasure val1 = log (gsl_sf_bessel_In_scaled(m_nCoils-1,(S*M)/Var)) +  ((S*M)/Var);
					TMeasure val2 = (S*S)/(2*Var);
					TMeasure val3 = (m_nCoils-1)*log(M/S);
					//std::cout << "vals: " << val1 << ", " << val2 << std::endl;

					sum1 += _b_weights(i)*val1;
					sum2 +=  _b_weights(i)*val2;
					sum3 += _b_weights(i)*val3;
				}
				//sum1 +=   val1;
				//sum2 +=  val2;
			}
			likelihoodVal = sum1 - sum2 + sum3;
			//std::cout << "likelihoodVal: " << likelihoodVal << std::endl;
		}
		else
		{
			std::cerr << "nCoils must be >= 1" << std::endl;
			exit (EXIT_FAILURE);
		}



		// smoothness term
		TMeasure smoothnessVal = 0;

		for (int i=0;i<27;++i)
		{
			double neighbor_b0 = _neighbors_B0(i);
			double neighbor_perfusionCoeff =  _neighbors_Dstar(i);
			double neighbor_diffusionCoeff =  _neighbors_D(i);
			double neighbor_perfusionFractionCoeff =  _neighbors_f(i);


			smoothnessVal += _neighbors_Weights[i]*fabs(neighbor_b0-s0)/IVIMSquaredScales[0];
			smoothnessVal += _neighbors_Weights[i]*fabs(neighbor_perfusionCoeff-PerCoeff)/IVIMSquaredScales[1];
			smoothnessVal += _neighbors_Weights[i]*fabs(neighbor_diffusionCoeff-DiffCoeff)/IVIMSquaredScales[2];
			smoothnessVal += _neighbors_Weights[i]*fabs(neighbor_perfusionFractionCoeff-PerFracCoeff)/IVIMSquaredScales[3];


		}
		//std::cout << "likelihoodVal: " << sum1 << " - " << sum2 << " = " << likelihoodVal << std::endl;
		return  (-likelihoodVal+0.001*smoothnessVal/26);

}



/** PrintSelf */
template <typename TMeasure>
void
DiffusionModelFittingSmoothedMLECostFunction<TMeasure>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

}


} // end namespace itk

#endif

