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
#ifndef _itkModelToTraceWImageFilter_txx
#define _itkModelToTraceWImageFilter_txx

#include "itkModelToTraceWImageFilter.h"



#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "crlBodyDiffusionTools.h"

#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "itkProgressReporter.h"


namespace itk
{
  


template <typename TInputImage,typename TOutputImage>
const double ModelToTraceWImageFilter<TInputImage, TOutputImage>
::EPSILON = 0.0000000001;


template <typename TInputImage,typename TOutputImage>
ModelToTraceWImageFilter<TInputImage, TOutputImage>
::ModelToTraceWImageFilter() :
 m_BValue (3000)
{

}

template <typename TInputImage, typename TOutputImage>
void
ModelToTraceWImageFilter<TInputImage, TOutputImage>
::GenerateData()
{


	gsl_rng_env_setup();
	const gsl_rng_type * m_gsl_rand_T = gsl_rng_default;
	gsl_rng * m_gsl_rand_gen = gsl_rng_alloc (m_gsl_rand_T);
	size_t  seed = time(NULL);
	gsl_rng_set (m_gsl_rand_gen, seed);



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
	itkInputImageRegionConstIteratorType BaseImageIter (m_BaseImage, m_BaseImage->GetLargestPossibleRegion());
	itkInputImageRegionConstIteratorType MaskImageIter (m_MaskImage, m_MaskImage->GetLargestPossibleRegion());



	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType outIter (OutputImage,OutputImage->GetLargestPossibleRegion());




	for (outIter.GoToBegin(),B0Iter.GoToBegin(), ADCIter.GoToBegin(),
		 PERIter.GoToBegin(), PER_FRACIter.GoToBegin(),
		 BaseImageIter.GoToBegin(),MaskImageIter.GoToBegin();
	   	 !outIter.IsAtEnd();
	   	 ++outIter,++B0Iter,++ADCIter,++PERIter,++PER_FRACIter,++BaseImageIter,++MaskImageIter)
	{
	   	//compute ADC


		double val;
		if (std::abs (MaskImageIter.Value()) < EPSILON)
		{
			val = 0.0;
		}
		else
		{
			val = crlBodyDiffusionTools::computeModelEstimateAtBVal
										(B0Iter.Value(), PERIter.Value(), ADCIter.Value(), PER_FRACIter.Value(),
										 m_BValue);


			if (std::isnan (val))
			{
				val = BaseImageIter.Value();
			}
			else if (std::isinf(val))
			{
				val = BaseImageIter.Value();
			}
			else if (val > 1000 || val < 0.0)
			{
				val = BaseImageIter.Value();
			}



		}
		/*// add rician noise
		double x = 8*gsl_ran_gaussian (m_gsl_rand_gen,1) + val;
		double y = 8*gsl_ran_gaussian (m_gsl_rand_gen,1);
		val = vcl_sqrt (x*x+y*y);
*/
		outIter.Value() = val;



	}
	this->GraftOutput (OutputImage);
}

/*
template <typename TInputImage, typename TOutputImage>
void
ModelToTraceWImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{
	std::cout << "start run" << std::endl;
	itkDebugMacro(<<"Actually executing");

	int bValuesNum = m_BValues.size();

	// Get the input and output pointers
	typename TOutputImage::Pointer outputPtr = this->GetOutput(0);

	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());


	typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;

	std::vector <itkInputImageRegionConstIteratorType> bValuesItersVec (bValuesNum);
	for (int bIdx=0;bIdx<bValuesNum;++bIdx)
	{
		itkInputImageRegionConstIteratorType bIter (this->GetInput(bIdx),outputRegionForThread);
		bValuesItersVec.push_back(bIter);
	}

	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType outIter (outputPtr,outputRegionForThread);

	itkInputImageRegionConstIteratorType maskIter (m_MaskImage,outputRegionForThread);





	for (int bIdx=0;bIdx<bValuesNum;++bIdx)
	{
		bValuesItersVec[bIdx].GoToBegin();
	}

	for (outIter.GoToBegin(),maskIter.GoToBegin();
	   	 ! outIter.IsAtEnd();
	   	 ++outIter,++maskIter)
	{
	   	//compute ADC
		double ADC = 0;
		if (maskIter.Value() != BACKGROUND)
		{
			std::vector <double> Sb (bValuesNum);
			for (int bIdx=0;bIdx<bValuesNum;++bIdx)
			{
				Sb[bIdx] = bValuesItersVec[bIdx].Value();
			}
			ADC = computeADCVal (m_BValues, Sb);
		}
		else
		{
			ADC = 0.0;
		}
		outIter.Value() = ADC;

		for (int bIdx=0;bIdx<bValuesNum;++bIdx)
		{
			bValuesItersVec[bIdx].GoToBegin();
		}
		progress.CompletedPixel();
	}
}

*/


template <typename TInputImage, typename TOutputImage>
void
ModelToTraceWImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}



} // end namespace itk

#endif

