/*
 * PerfusionCoeffCostFunction.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: ch145864
 */

#include <vnl/vnl_math.h>
#include "itkPerfusionCoeffCostFunction.h"

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
#ifndef _itkPerfusionCoeffCostFunction_cxx
#define _itkPerfusionCoeffCostFunction_txx

#include "itkPerfusionCoeffCostFunction.h"
#include "crlBodyDiffusionTools.h"

namespace itk
{
template <typename TMeasure>
const int PerfusionCoeffCostFunction<TMeasure>
::NumOfParams = 1;

/** Constructor */
template <typename TMeasure>
PerfusionCoeffCostFunction<TMeasure>
::PerfusionCoeffCostFunction():
  _b_vals (),
 _b_signals (),
 _b_weights (),
 _b0 (0.0), _adc(0.0), _f(0.0)
{

}

template <typename TMeasure>
void PerfusionCoeffCostFunction<TMeasure>::SetObservedValues
						 (const vnl_vector<TMeasure> & b_vals,
		                  const vnl_vector<TMeasure> & b_signal,
		                  const vnl_vector<TMeasure> & b_weights)
{
	_b_vals = b_vals;
	_b_signals = b_signal;
	_b_weights = b_weights;
}

template <typename TMeasure>
typename PerfusionCoeffCostFunction<TMeasure>::MeasureType
PerfusionCoeffCostFunction<TMeasure>::GetValue (const ParametersType & params) const
{
	//std::cout << "params in f(): " << params << std::endl;
	MeasureType sum  = 0.0;
	double modelVal;
	for (size_t bvalIdx=0;bvalIdx<_b_vals.size();++bvalIdx)
	{
		modelVal = ModelEvalAtbVal (_b_vals[bvalIdx], params);
		//std::cout << "model eval at: " << _b_vals[bvalIdx] << " is: " << modelVal << std::endl;
		double diff = modelVal-_b_signals[bvalIdx];
		diff = diff*diff;
		sum +=  _b_weights(bvalIdx)*diff;
	}
//	std::cout << "metric val: " << sum << std::endl;
	return sum;
}

template <typename TMeasure>
TMeasure PerfusionCoeffCostFunction<TMeasure>::ModelEvalAtbVal (TMeasure b_val, const ParametersType & params) const
{
	TMeasure res = crlBodyDiffusionTools::computeModelEstimateAtBVal(_b0,params (0),_adc,_f,b_val);
	return res;
}

/** PrintSelf */
template <typename TMeasure>
void
PerfusionCoeffCostFunction<TMeasure>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

}


} // end namespace itk

#endif

