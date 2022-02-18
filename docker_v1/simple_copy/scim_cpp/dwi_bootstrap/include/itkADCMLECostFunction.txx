/*
 * DiffusionModelFittingMLECostFunction.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: ch145864
 */
#include <cmath>
#include <vnl/vnl_math.h>
#include <gsl/gsl_sf_bessel.h>
#include "itkADCMLECostFunction.h"

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
#ifndef _itkADCMLECostFunction_txx
#define _itkADCMLECostFunction_txx

#include "itkADCMLECostFunction.h"

namespace itk
{

/** Constructor */
template <typename TMeasure>
ADCMLECostFunction<TMeasure>
::ADCMLECostFunction()
{

}

template <typename TMeasure>
void ADCMLECostFunction<TMeasure>::SetObservedValues
				(const MeasurementVecType & b_vals,
				 const MeasurementVecType & b_signals,
				 const MeasurementVecType & b_variance)
{
	_b_vals = b_vals;
	_b_signals = b_signals;
	_b_variance = b_variance;
}

template <typename TMeasure>
typename ADCMLECostFunction<TMeasure>::MeasureType
ADCMLECostFunction<TMeasure>::GetValue (const ParametersType & params) const
{
	//std::cout << "params in f(): " << params << std::endl;
	MeasureType sum1  = 0.0, sum2 = 0.0;


	TMeasure s0  = params(0);
	TMeasure DiffCoeff = params (1);



	for (size_t i=0;i<_b_vals.size();++i)
	{
		if (_b_vals(i) < m_MinBVal)
		{
			continue;
		}
		else
		{
			TMeasure M = _b_signals(i);
			TMeasure S = s0*vcl_exp (-_b_vals(i)*(DiffCoeff));
			TMeasure Var = _b_variance(i)*_b_variance(i);


			TMeasure val1 = log (gsl_sf_bessel_I0_scaled((S*M)/Var))  + ((S*M)/Var);
			TMeasure val2 = (S*S)/(2*Var);

		//std::cout << "vals: " << val1 << ", " << val2 << std::endl;
			sum1 +=   val1;
			sum2 +=  val2;
		}

	}
	TMeasure likelihoodVal = sum1 - sum2;

	//std::cout << "after metric comp: [" <<s0 << ", " << DiffCoeff << "]: " << likelihoodVal << std::endl;
	return  (likelihoodVal);
}



/** PrintSelf */
template <typename TMeasure>
void
ADCMLECostFunction<TMeasure>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

}


} // end namespace itk

#endif

