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
#ifndef _itkIVIMBootstrapSignalFilter_txx
#define _itkIVIMBootstrapSignalFilter_txx

#include "itkIVIMBootstrapSignalFilter.h"




#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"






#include "itkProgressReporter.h"




namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::FOREGROUND = 1;





template <typename TInputImage,typename TOutputImage>
const int IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::UNWEIGHTED = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::FIXED_WEIGHTS = 1;

template <typename TInputImage,typename TOutputImage>
const int IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::UNSCENTED_WEIGHTS = 2;

template <typename TInputImage,typename TOutputImage>
IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::IVIMBootstrapSignalFilter() :
 m_BValues (),
 m_Variance (),
 m_BValuesNum (),
 m_BootstrapMode (FIXED_WEIGHTS),
 m_ADCTresh (200)
{

	gsl_rng_env_setup();
	const gsl_rng_type * m_gsl_rand_T = gsl_rng_default;
	m_gsl_rand_gen = gsl_rng_alloc (m_gsl_rand_T);

	/*
	for (int i=0;i<10;++i)
	{
		double newResidualWeight =   -1 + (static_cast<int> (2*gsl_rng_uniform_int (m_gsl_rand_gen,2)));
		std::cout << "newResidualWeight: " << newResidualWeight << std::endl;
	}*/
}


template <typename TInputImage, typename TOutputImage>
void
IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}


template <typename TInputImage, typename TOutputImage>
void
IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData ()
{
	m_BValuesNum = m_BValues.size();

	// compute variances:
	//m_Variance.set_size(m_BValuesNum);



	if (m_MaskImage.IsNull())
	{
		m_MaskImage = itkMaskImageType::New();
		m_MaskImage->SetSpacing (this->GetInput(0)->GetSpacing());
		m_MaskImage->SetOrigin (this->GetInput(0)->GetOrigin());
		m_MaskImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
		m_MaskImage->Allocate();
		m_MaskImage->FillBuffer (FOREGROUND);
	}




	// Get the input and output pointers
	m_BSSignalProposalImage = TOutputImage::New();
	m_BSSignalProposalImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_BSSignalProposalImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_BSSignalProposalImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_BSSignalProposalImage->SetVectorLength(m_BValuesNum);
	m_BSSignalProposalImage->Allocate();

	itkVariableLengthVectorType SingalVectorZeros(m_BValuesNum);
	SingalVectorZeros.Fill (0.0);
	m_BSSignalProposalImage->FillBuffer (SingalVectorZeros);

}

template <typename TInputImage, typename TOutputImage>
void
IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

	itkDebugMacro(<<"Actually executing");



	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

	typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;
	itkInputImageRegionConstIteratorType samplesIter (this->GetInput(),outputRegionForThread);


	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType BSProposalIter (m_BSSignalProposalImage,outputRegionForThread);
	itkOutputImageRegionIteratorType initialIVIMModelIter (m_InitialIVIMModelImage,outputRegionForThread);


	typedef itk::ImageRegionConstIterator <itkMaskImageType> itkMaskImageRegionConstIteratorType;
	itkMaskImageRegionConstIteratorType maskIter (m_MaskImage,outputRegionForThread);



	for (samplesIter.GoToBegin(), maskIter.GoToBegin(),initialIVIMModelIter.GoToBegin(),BSProposalIter.GoToBegin();
	   	 !samplesIter.IsAtEnd();
	   	 ++samplesIter,++maskIter,++initialIVIMModelIter,++BSProposalIter)
	{
	   	//compute ADC


		if (maskIter.Value() != BACKGROUND)
		{
			const typename TOutputImage::PixelType & modelIVIMParams = initialIVIMModelIter.Get();



			double model_b0=modelIVIMParams[0] ;
			double model_perfusionCoeff=modelIVIMParams[1];
			double model_diffusionCoeff=modelIVIMParams[2];
			double model_perfusionFractionCoeff=modelIVIMParams[3];


			//std::cout << "model params: " <<  modelIVIMParams << std::endl;
			MeasureMentVectorType Sb (m_BValuesNum);
			MeasureMentVectorType weights (m_BValuesNum);


			for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
			{
				double val = samplesIter.Get()[bIdx];
				if (val < crlBodyDiffusionTools::EPSILON)
				{
					val = crlBodyDiffusionTools::EPSILON;
				}
				Sb(bIdx) = val;
				weights(bIdx) = 1/m_Variance (bIdx);
			}

		//	std::cout << Sb << std::endl;
		//	std::cout << weights << std::endl;


			MeasureMentVectorType residuals (m_BValuesNum);
			MeasureMentVectorType modelVals (m_BValuesNum);
			MeasureMentVectorType scaleVals (m_BValuesNum);

			for (size_t i=0;i<m_BValuesNum;++i)
			{
				modelVals(i) = crlBodyDiffusionTools::computeModelEstimateAtBVal (model_b0,model_perfusionCoeff,model_diffusionCoeff,model_perfusionFractionCoeff,m_BValues(i));
				residuals(i) = Sb(i) - modelVals(i);
			}

		//	std::cout << modelVals << std::endl;
		//	std::cout << residuals << std::endl;


			//std::cout << "residuals.mean(): " << residuals.mean() << std::endl;
			residuals = residuals - residuals.mean();
		//	std::cout << residuals << std::endl;


			if (m_BootstrapMode==UNWEIGHTED)
			{
				// do nothing
			}
			else if (m_BootstrapMode == FIXED_WEIGHTS)
			{
				for (size_t i=0;i<m_BValuesNum;++i)
				{
					// std::cout <<"residuals before scale("<<i<<"): " << residuals(i)  << std::endl;
					scaleVals(i) = vcl_sqrt((double)m_BValuesNum/double(m_BValuesNum-4));
					residuals (i) =  residuals (i)*scaleVals(i);
					// std::cout <<"residuals after scale("<<i<<"): " << residuals(i)  << std::endl;
				}
			}
			else if (m_BootstrapMode == UNSCENTED_WEIGHTS)
			{
				// TODO
				/*
				for (size_t i=0;i<m_BValuesNum;++i)
				{
					// std::cout <<"residuals before scale("<<i<<"): " << residuals(i)  << std::endl;
					scaleVals(i) = (1-vcl_sqrt(1/(scaleVals(i)*scaleVals(i))));
					residuals (i) =  residuals (i)*scaleVals(i);
					// std::cout <<"residuals after scale("<<i<<"): " << residuals(i)  << std::endl;
				}*/
			}



			//std::cout << "residuals: " << residuals << std::endl;
			residuals = residuals - residuals.mean();


			itkVariableLengthVectorType  newSb (m_BValuesNum);

			// Generate wild BOOSTRAP samples

			//	std::cout <<"start bs iter: " << iterIdx << std::endl;

			for (size_t bvalIdx=0;bvalIdx<m_BValuesNum;++bvalIdx)
			{
				double newResidualWeight =   -1 + (static_cast<int> (2*gsl_rng_uniform_int (m_gsl_rand_gen,2)));
				// std::cout << "newResidualWeight: " << newResidualWeight << std::endl;
				newSb[bvalIdx] = modelVals(bvalIdx) +  newResidualWeight*residuals(bvalIdx);
			}


			BSProposalIter.Set (newSb);

			progress.CompletedPixel();
		}

	}

}





template <typename TInputImage, typename TOutputImage>
void
IVIMBootstrapSignalFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif
