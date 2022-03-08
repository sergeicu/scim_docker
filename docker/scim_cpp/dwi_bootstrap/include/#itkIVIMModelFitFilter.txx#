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
#ifndef _itkIVIMModelFitFilter_txx
#define _itkIVIMModelFitFilter_txx

#include "itkIVIMModelFitFilter.h"




#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"






#include "itkProgressReporter.h"




namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int IVIMModelFitFilter<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMModelFitFilter<TInputImage, TOutputImage>
::FOREGROUND = 1;





template <typename TInputImage,typename TOutputImage>
const int IVIMModelFitFilter<TInputImage, TOutputImage>
::SLS = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMModelFitFilter<TInputImage, TOutputImage>
::ML = 1;

template <typename TInputImage,typename TOutputImage>
const int IVIMModelFitFilter<TInputImage, TOutputImage>
::LS = 2;

template <typename TInputImage,typename TOutputImage>
IVIMModelFitFilter<TInputImage, TOutputImage>
::IVIMModelFitFilter() :
 m_BValues (),
 m_Variance (),
 m_BValuesNum (),
 m_AlgMode (LS),
 m_ADCTresh (200),
 m_BSflag (false)
{

}


template <typename TInputImage, typename TOutputImage>
void
IVIMModelFitFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}


template <typename TInputImage, typename TOutputImage>
void
IVIMModelFitFilter<TInputImage, TOutputImage>
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
	m_IVIMModelImage = TOutputImage::New();
	m_IVIMModelImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_IVIMModelImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_IVIMModelImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_IVIMModelImage->SetVectorLength(4);
	m_IVIMModelImage->Allocate();

	itkVariableLengthVectorType IVIMVectorZeros(4);
	IVIMVectorZeros.Fill (0.0);
	m_IVIMModelImage->FillBuffer (IVIMVectorZeros);

	// Get the input and output pointers
	m_IVIMModelBSImage = TOutputImage::New();
	m_IVIMModelBSImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_IVIMModelBSImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_IVIMModelBSImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_IVIMModelBSImage->SetVectorLength(4);
	m_IVIMModelBSImage->Allocate();
	m_IVIMModelBSImage->FillBuffer (IVIMVectorZeros);



}

template <typename TInputImage, typename TOutputImage>
void
IVIMModelFitFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

	itkDebugMacro(<<"Actually executing");



	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

	typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;
	itkInputImageRegionConstIteratorType samplesIter (this->GetInput(),outputRegionForThread);


	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType IVIMIter (m_IVIMModelImage,outputRegionForThread);
	itkOutputImageRegionIteratorType IVIMBSIter (m_IVIMModelBSImage,outputRegionForThread);

	typedef itk::ImageRegionConstIterator <itkMaskImageType> itkMaskImageRegionConstIteratorType;
	itkMaskImageRegionConstIteratorType maskIter (m_MaskImage,outputRegionForThread);



	for (samplesIter.GoToBegin(), maskIter.GoToBegin(),IVIMIter.GoToBegin(),IVIMBSIter.GoToBegin();
	   	 !samplesIter.IsAtEnd();
	   	 ++samplesIter,++maskIter,++IVIMIter,++IVIMBSIter)
	{
	   	//compute ADC
		double b0=0;
		double perfusionCoeff=0;
		double diffusionCoeff=0;
		double perfusionFractionCoeff=0;

		double b0STD=0,
		       perfusionCoeffSTD=0,
		       diffusionCoeffSTD=0,
		       perfusionFractionCoeffSTD=0;
		double b0STD_lo=0,
			   perfusionCoeffSTD_lo=0,
			   diffusionCoeffSTD_lo=0,
			   perfusionFractionCoeffSTD_lo=0;


		if (maskIter.Value() != BACKGROUND)
		{
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

/*
			crlBodyDiffusionTools::computeGlobalNonLinearModel
								                      (m_BValues, Sb, m_Variance,weights,
													   b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);
			*/
			//weights.normalize();

			crlBodyDiffusionTools::computeLinearModel
			                   (m_BValues, Sb, weights,
					            b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);


			if (m_AlgMode > SLS)
			{

				if( m_AlgMode == ML)
				{

				/*	crlBodyDiffusionTools::computeWeights
		 			               (m_BValues, Sb, m_Variance, weights,
									b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);*/

					crlBodyDiffusionTools::computeNonLinearModel
					                      (m_BValues, Sb, m_Variance,weights,1,
										   b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);
				}
				else if ( m_AlgMode == LS)
				{
				/*	crlBodyDiffusionTools::computeWeights
					 	               (m_BValues, Sb, m_Variance, weights,
										b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);*/
					crlBodyDiffusionTools::computeNonLinearModelLS
				                      (m_BValues, Sb, m_Variance,weights,
           								   b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);
									// std::cout << "start EM" << std::endl;
				}

			}
			typename TOutputImage::PixelType IVIMParmas (4);
			IVIMParmas[0] = b0;
			IVIMParmas[1] = perfusionCoeff;
			IVIMParmas[2] = diffusionCoeff;
			IVIMParmas[3] = perfusionFractionCoeff;
			IVIMIter.Set (IVIMParmas);

			//std::cout << b0 << ", " << perfusionCoeff << ", " << diffusionCoeff << ", " << perfusionFractionCoeff << std::endl;
			// CALCUALTE MODEL CERTAINTY:
			if (m_BSflag)
			{
				crlBodyDiffusionTools::computeBootstrapSE
				                                (m_BValues, Sb,
											     m_Variance, weights,
									  	  	  	 b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff,
									  	  	  	 b0STD,
									  	  	  	 perfusionCoeffSTD,
									  	  	  	 diffusionCoeffSTD,
									  	  	  	 perfusionFractionCoeffSTD,
									  	  	  	 b0STD_lo,
									  	  	  	 perfusionCoeffSTD_lo,
									  	  	 	 diffusionCoeffSTD_lo,
									  	  	 	 perfusionFractionCoeffSTD_lo,
									  	  	     200,
									  	  	     0);
				typename TOutputImage::PixelType IVIMBSParmas (4);
				IVIMBSParmas[0] = b0STD;
				IVIMBSParmas[1] = perfusionCoeffSTD;
				IVIMBSParmas[2] = diffusionCoeffSTD;
				IVIMBSParmas[3] = perfusionFractionCoeffSTD;

				for (int i=0;i<4;++i)
				{
					if (IVIMParmas[i] > 0.0000001)
					{
						IVIMBSParmas[i] = IVIMBSParmas[i]/IVIMParmas[i];
					}
					else
					{
						IVIMBSParmas[i] = 0;
					}
				}

				//std::cout << IVIMBSParmas[0] << ", " << IVIMBSParmas[1] << ", " << IVIMBSParmas[2] << ", " << IVIMBSParmas[3] << std::endl;

				IVIMBSIter.Set (IVIMBSParmas);

			}
		} //END MASK
		progress.CompletedPixel();

	}

}





template <typename TInputImage, typename TOutputImage>
void
IVIMModelFitFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

