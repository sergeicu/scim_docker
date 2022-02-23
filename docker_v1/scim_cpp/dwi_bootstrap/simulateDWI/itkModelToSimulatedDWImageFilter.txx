/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRecursiveGaussianImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2006-03-19 04:36:56 $
  Version:   $Revision: 1.38 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkModelToSimulatedDWImageFilter_txx
#define _itkModelToSimulatedDWImageFilter_txx

#include "itkModelToSimulatedDWImageFilter.h"



#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"


#include <cmath>
#include <cstdlib>

#include "itkProgressReporter.h"


namespace itk
{
  


template <typename TInputImage,typename TOutputImage>
const double ModelToSimulatedDWImageFilter<TInputImage, TOutputImage>
::EPSILON = 0.0000000001;


template <typename TInputImage,typename TOutputImage>
ModelToSimulatedDWImageFilter<TInputImage, TOutputImage>
::ModelToSimulatedDWImageFilter() :
 m_BValue (3000)
{


	//gsl_rng_env_setup();
	T = gsl_rng_default;
	randGen = gsl_rng_alloc (T);
	size_t  seed = time(NULL);
	std::cout << "seed: " << seed << std::endl;
	gsl_rng_set (randGen, seed);

}

template <typename TInputImage, typename TOutputImage>
void
ModelToSimulatedDWImageFilter<TInputImage, TOutputImage>
::GenerateData()
{






	typename TOutputImage::Pointer OutputImage = TOutputImage::New();
	OutputImage->SetSpacing (this->GetInput(0)->GetSpacing());
	OutputImage->SetOrigin (this->GetInput(0)->GetOrigin());
	OutputImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	OutputImage->Allocate();
	OutputImage->FillBuffer (0.0);




	// support progress methods/callbacks
	//ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());


	typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;

	itkInputImageRegionConstIteratorType ADCIter (m_ADCImage, m_ADCImage->GetLargestPossibleRegion());
	itkInputImageRegionConstIteratorType B0Iter (m_B0Image, m_B0Image->GetLargestPossibleRegion());
	itkInputImageRegionConstIteratorType PERIter (m_PERImage, m_PERImage->GetLargestPossibleRegion());
	itkInputImageRegionConstIteratorType PER_FRACIter (m_PER_FRACImage, m_PER_FRACImage->GetLargestPossibleRegion());



	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType outIter (OutputImage,OutputImage->GetLargestPossibleRegion());




	for (outIter.GoToBegin(),B0Iter.GoToBegin(), ADCIter.GoToBegin(),PERIter.GoToBegin(),PER_FRACIter.GoToBegin();
	   	 !outIter.IsAtEnd();
	   	 ++outIter,++B0Iter,++ADCIter,++PERIter,++PER_FRACIter)
	{
	   	//compute noise free signal


		double val = B0Iter.Value()*(PER_FRACIter.Value()*exp(-m_BValue*PERIter.Value()) + (1-PER_FRACIter.Value())*exp(-m_BValue*ADCIter.Value()));
		if (std::isnan (val))
		{
			val = 0.0;
		}
		else if (std::isinf(val))
		{
			val = 0.0;
		}
		else if (val > 1000 || val < 0.0)
		{
			val = 0.0;
		}

		// add noise
		double x = m_Sigma*gsl_ran_gaussian (randGen,1.0) + val;
		double y = m_Sigma*gsl_ran_gaussian (randGen,1.0);


		outIter.Value() = vcl_sqrt (x*x+y*y);



	}


	this->GraftOutput (OutputImage);



}


template <typename TInputImage, typename TOutputImage>
void
ModelToSimulatedDWImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}



} // end namespace itk

#endif

