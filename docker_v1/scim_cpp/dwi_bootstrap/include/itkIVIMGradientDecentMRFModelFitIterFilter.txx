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
#ifndef _itkIVIMGradientDecentMRFModelFitIterFilter_txx
#define _itkIVIMGradientDecentMRFModelFitIterFilter_txx

#include "itkIVIMGradientDecentMRFModelFitIterFilter.h"




#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkConstNeighborhoodIterator.h"





#include "itkProgressReporter.h"




namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int IVIMGradientDecentMRFModelFitIterFilter<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMGradientDecentMRFModelFitIterFilter<TInputImage, TOutputImage>
::FOREGROUND = 1;






template <typename TInputImage,typename TOutputImage>
IVIMGradientDecentMRFModelFitIterFilter<TInputImage, TOutputImage>
::IVIMGradientDecentMRFModelFitIterFilter() :
 m_BValues (),
 m_BValuesNum ()
{

}


template <typename TInputImage, typename TOutputImage>
void
IVIMGradientDecentMRFModelFitIterFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}


template <typename TInputImage, typename TOutputImage>
void
IVIMGradientDecentMRFModelFitIterFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData ()
{
	m_BValuesNum = m_BValues.size();

	// compute variances:




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

}

template <typename TInputImage, typename TOutputImage>
void
IVIMGradientDecentMRFModelFitIterFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

	itkDebugMacro(<<"Actually executing");



	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

	typedef itk::ConstNeighborhoodIterator< TInputImage > itkConstNeighborhoodIteratorType;
	typename itkConstNeighborhoodIteratorType::RadiusType radius;
	radius.Fill(1);
	itkConstNeighborhoodIteratorType inputModelIter( radius, this->GetInput(),outputRegionForThread  );

	//typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;

	itkConstNeighborhoodIteratorType samplesIter( radius, m_SignalImage,outputRegionForThread  );


	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType outIVIMIter (m_IVIMModelImage,outputRegionForThread);

	typedef itk::ConstNeighborhoodIterator< itkMaskImageType > itkConstNeighborhoodMaskIteratorType;
	typename itkConstNeighborhoodMaskIteratorType::RadiusType mask_radius;
	mask_radius.Fill(1);
	itkConstNeighborhoodMaskIteratorType maskIter( radius, m_MaskImage,outputRegionForThread  );


	typename itkConstNeighborhoodIteratorType::OffsetType offset[27];
	typename itkConstNeighborhoodMaskIteratorType::OffsetType mask_offset[27];

	int offsetIndex = 0;
	for (int i=0;i<3;i++)
	{
	   	for (int j=0;j<3;j++)
		{
			for (int k=0;k<3;k++)
			{
				offset[offsetIndex][0] = i-1;
				offset[offsetIndex][1] = j-1;
				offset[offsetIndex][2] = k-1;

				mask_offset[offsetIndex][0] = i-1;
				mask_offset[offsetIndex][1] = j-1;
				mask_offset[offsetIndex][2] = k-1;

				offsetIndex++;
			}
		}
	}




	for (samplesIter.GoToBegin(), maskIter.GoToBegin(),outIVIMIter.GoToBegin(),inputModelIter.GoToBegin();
	   	 !samplesIter.IsAtEnd();
	   	 ++samplesIter,++maskIter,++outIVIMIter,++inputModelIter)
	{

		if (maskIter.GetCenterPixel() != BACKGROUND)
		{
			itkVariableLengthVectorType S = samplesIter.GetCenterPixel();
			itkVariableLengthVectorType curModel = inputModelIter.GetCenterPixel();
			double b0 = curModel[0];
			double perfusionCoeff = curModel[1];
			double diffusionCoeff = curModel[2];
			double perfusionFractionCoeff = curModel[3];

			itkVariableLengthVectorType M (m_BValuesNum);

			double partial_s0 = 0;
			double partial_f = 0;
			double partial_D = 0;
			double partial_Dstar = 0;

			// compute gradient for data term

			for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
			{

				double partial_s0_at_bval = 0;
				double partial_f_at_bval = 0;
				double partial_D_at_bval = 0;
				double partial_Dstar_at_bval = 0;

				//compute model projected signal
				M[bIdx] = crlBodyDiffusionTools::computeModelEstimateAtBVal
												(b0,perfusionCoeff,diffusionCoeff,perfusionFractionCoeff,
												 m_BValues(bIdx));


				//compute partial Mi/partial s0
				crlBodyDiffusionTools::computeModelPartialDerAtBval (b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff,
																	 m_BValues(bIdx),
																	 partial_s0_at_bval, partial_Dstar_at_bval, partial_D_at_bval, partial_f_at_bval);


				partial_s0 += 2*(M[bIdx]-S[bIdx] )*partial_s0_at_bval;
				partial_f += 2*(M[bIdx]-S[bIdx])*partial_f_at_bval;
				partial_Dstar += 2*(M[bIdx]-S[bIdx])*partial_Dstar_at_bval;
				partial_D += 2*(M[bIdx]-S[bIdx])*partial_D_at_bval;

				//std::cout << partial_s0 << ", " << partial_f << ", " << partial_Dstar << ", " << partial_D <<std::endl;


			}
			//std::cout << "end data term" << std::endl;

			// compute gradient for smoothness term
			for (int i=0;i<27;++i)
			{
				if (maskIter.GetPixel(mask_offset[i]) != BACKGROUND)
				{
					itkVariableLengthVectorType curNeighborModel = inputModelIter.GetPixel(offset[i]);
					//itkVariableLengthVectorType curNeighborSignal = samplesIter.GetPixel(offset[i]);


					double neighbor_b0 = curNeighborModel[0];
					double neighbor_perfusionCoeff = curNeighborModel[1];
					double neighbor_diffusionCoeff = curNeighborModel[2];
					double neighbor_perfusionFractionCoeff = curNeighborModel[3];

					partial_s0 += 0.01*(1/26.0)*(2*(b0 - neighbor_b0)/1.0);
					partial_Dstar += 0.01*(1/26.0)*(2*(perfusionCoeff - neighbor_perfusionCoeff)/(0.00001*0.00001));
					partial_D += 0.01*(1/26.0)*(2*(diffusionCoeff - neighbor_diffusionCoeff)/(0.000001*0.000001));
					partial_f += 0.01*(1/26.0)*(2*(perfusionFractionCoeff - neighbor_perfusionFractionCoeff)/(0.001*0.001));
					/*

					//itkVariableLengthVectorType neighbor_M (m_BValuesNum);
					for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
					{

						//compute model projected signal at neighbor
						//neighbor_M[bIdx] = crlBodyDiffusionTools::computeModelEstimateAtBVal
						//								(neighbor_b0,neighbor_perfusionCoeff,neighbor_diffusionCoeff,neighbor_perfusionFractionCoeff,
						//								 m_BValues(bIdx));



						double partial_s0_at_bval = 0;
						double partial_f_at_bval = 0;
						double partial_D_at_bval = 0;
						double partial_Dstar_at_bval = 0;

						crlBodyDiffusionTools::computeModelPartialDerAtBval (b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff,
																			 m_BValues(bIdx),
																			 partial_s0_at_bval, partial_Dstar_at_bval, partial_D_at_bval, partial_f_at_bval);

						partial_s0 += 2*(M[bIdx]-curNeighborSignal[bIdx] )*partial_s0_at_bval;
						partial_f += 2*(M[bIdx]-curNeighborSignal[bIdx])*partial_f_at_bval;
						partial_Dstar += 2*(M[bIdx]-curNeighborSignal[bIdx])*partial_Dstar_at_bval;
						partial_D += 2*(M[bIdx]-curNeighborSignal[bIdx])*partial_D_at_bval;

						//std::cout << partial_s0 << ", " << partial_f << ", " << partial_Dstar << ", " << partial_D <<std::endl;

						//compute partial Mi/partial s0
						//crlBodyDiffusionTools::computeModelPartialDerAtBval (b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff,
						//													 m_BValues(bIdx),
						//													 partial_s0_at_bval, partial_Dstar_at_bval, partial_D_at_bval, partial_f_at_bval);


//						double neighbor_partial_s0_at_bval = 0;
//						double neighbor_partial_f_at_bval = 0;
//						double neighbor_partial_D_at_bval = 0;
//						double neighbor_partial_Dstar_at_bval = 0;

						//compute partial Mi/partial s0 at neighbor
//						crlBodyDiffusionTools::computeModelPartialDerAtBval (neighbor_b0,neighbor_perfusionCoeff,neighbor_diffusionCoeff,neighbor_perfusionFractionCoeff,
//																			 m_BValues(bIdx),
//																			 neighbor_partial_s0_at_bval, neighbor_partial_Dstar_at_bval, neighbor_partial_D_at_bval, neighbor_partial_f_at_bval);



						//partial_s0 += 2*(b0 - neighbor_b0)
						//partial_f += 2*(M[bIdx] - neighbor_M[bIdx])*(partial_f_at_bval-neighbor_partial_f_at_bval);
						//partial_Dstar += 2*(M[bIdx] - neighbor_M[bIdx])*(partial_Dstar_at_bval-neighbor_partial_Dstar_at_bval);
						//partial_D += 2*(M[bIdx] - neighbor_M[bIdx])*(partial_D_at_bval-neighbor_partial_D_at_bval);



					}
					*/


				}
			}

			typename TOutputImage::PixelType IVIMParmas (4);
			IVIMParmas[0] = b0 - m_LearningRate*partial_s0*1.0;
			IVIMParmas[1] = perfusionCoeff - m_LearningRate*partial_Dstar;
			IVIMParmas[2] = diffusionCoeff - m_LearningRate*partial_D;
			IVIMParmas[3] = perfusionFractionCoeff - m_LearningRate*partial_f;

			outIVIMIter.Set(IVIMParmas);
		}
		else
		{
			outIVIMIter.Set(inputModelIter.GetCenterPixel());
		}




		progress.CompletedPixel();

	}

}





template <typename TInputImage, typename TOutputImage>
void
IVIMGradientDecentMRFModelFitIterFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

