/*
 * DiffusionModelFittingCostFunction.cpp
 *
 *  Created on: Jan 19, 2011
 *      Author: ch145864
 */

#include <vnl/vnl_math.h>
#include "itkDiffusionModelFittingCostFunction.h"

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
#ifndef _itkDiffusionModelFittingCostFunction_cxx
#define _itkDiffusionModelFittingCostFunction_txx

#include "itkDiffusionModelFittingCostFunctionLS.h"
#include "crlBodyDiffusionTools.h"

namespace itk
{

/** Constructor */
template <typename TMeasure>
DiffusionModelFittingCostFunction<TMeasure>
::DiffusionModelFittingCostFunction()
{

}

template <typename TMeasure>
void DiffusionModelFittingCostFunction<TMeasure>::SetObservedValues
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
typename DiffusionModelFittingCostFunction<TMeasure>::MeasureType
DiffusionModelFittingCostFunction<TMeasure>::GetValue (const ParametersType & params) const
{
	//std::cout << "params in f(): " << params << std::endl;
	MeasureType sum  = 0.0;
	double modelVal;
	for (size_t i=0;i<_b_vals.size();++i)
	{
		modelVal = ModelEvalAtbVal (_b_vals[i], params);
		//std::cout << "model eval at: " << _b_vals[i] << " is: " << modelVal << std::endl;
		sum +=  vnl_math_sqr(modelVal-_b_signals[i]);
	}
	//std::cout << "metric val: " << sum << std::endl;
	return sum;
}

template <typename TMeasure>
TMeasure DiffusionModelFittingCostFunction<TMeasure>::ModelEvalAtbVal (TMeasure b_val, const ParametersType & params) const
{
	TMeasure res = crlBodyDiffusionTools::computeModelEstimateAtBVal(params(0),params (1),params (2),params (3),b_val);



	return res;
}

/** PrintSelf */
template <typename TMeasure>
void
DiffusionModelFittingCostFunction<TMeasure>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf( os, indent );

}


} // end namespace itk

#endif

