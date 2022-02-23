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
#ifndef _itkIVIMMRFModelFitIICMmageFilter_txx
#define _itkIVIMMRFModelFitIICMmageFilter_txx

#include "itkIVIMMRFModelFitICMImageFilter.h"




#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkDiffusionModelFittingSmoothedMLECostFunction.h"

#include "crlBodyDiffusionTools.h"



#include "itkProgressReporter.h"




namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int IVIMMRFModelFitICMImageFilter<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMMRFModelFitICMImageFilter<TInputImage, TOutputImage>
::FOREGROUND = 1;








template <typename TInputImage,typename TOutputImage>
IVIMMRFModelFitICMImageFilter<TInputImage, TOutputImage>
::IVIMMRFModelFitICMImageFilter() :
 m_BValues (),
 m_Variance (),
 m_Weights (),
 m_BValuesNum ()
{

	IVIMSquaredScales[0] = 1.0;
	IVIMSquaredScales[1] = 0.001;
	IVIMSquaredScales[2] = 0.0001;
	IVIMSquaredScales[3] = 0.01;
}


template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitICMImageFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}


template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitICMImageFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData ()
{
	m_BValuesNum = m_BValues.size();




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
	m_OutputIVIMModelImage = TOutputImage::New();
	m_OutputIVIMModelImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_OutputIVIMModelImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_OutputIVIMModelImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_OutputIVIMModelImage->SetVectorLength(4);
	m_OutputIVIMModelImage->Allocate();

	itkVariableLengthVectorType IVIMVectorZeros(4);
	IVIMVectorZeros.Fill (0.0);
	m_OutputIVIMModelImage->FillBuffer (IVIMVectorZeros);


	// compute variances:
	//m_Variance.set_size(m_BValuesNum);
	m_Weights.set_size(m_BValuesNum);
	for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
	{
		m_Weights(bIdx) = 1/m_Variance (bIdx);
	}

}

template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitICMImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

	itkDebugMacro(<<"Actually executing");



	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());



	typedef itk::ConstNeighborhoodIterator< TInputImage > itkConstNeighborhoodSamplesIteratorType;
	typename itkConstNeighborhoodSamplesIteratorType::RadiusType samples_radius;
	samples_radius.Fill(1);
	itkConstNeighborhoodSamplesIteratorType samplesIter( samples_radius, this->GetInput(),outputRegionForThread  );



	typedef itk::ConstNeighborhoodIterator< TOutputImage > itkConstNeighborhoodModelIteratorType;
	typename itkConstNeighborhoodModelIteratorType::RadiusType model_radius;
	model_radius.Fill(1);
	itkConstNeighborhoodModelIteratorType baseIter( model_radius, m_BaseIVIMModelImage,outputRegionForThread  );
	itkConstNeighborhoodModelIteratorType proposalIter( model_radius, m_ProposalIVIMModelImage,m_ProposalIVIMModelImage->GetLargestPossibleRegion()  );


	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType outIVIMIter (m_OutputIVIMModelImage,outputRegionForThread);

	typedef itk::ConstNeighborhoodIterator< itkMaskImageType > itkConstNeighborhoodMaskIteratorType;
	typename itkConstNeighborhoodMaskIteratorType::RadiusType mask_radius;
	mask_radius.Fill(1);
	itkConstNeighborhoodMaskIteratorType maskIter( mask_radius, m_MaskImage,outputRegionForThread  );


	const typename TInputImage::SpacingType & spacing = this->GetInput()->GetSpacing();

	typename itkConstNeighborhoodModelIteratorType::OffsetType offset[27];
	typename itkConstNeighborhoodMaskIteratorType::OffsetType mask_offset[27];
	double distanceWeights[27];

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

				if((k-1) == 0)
				{
					distanceWeights[offsetIndex] = vcl_sqrt(((i-1)*spacing[0])*((i-1)*spacing[0]) +
															((j-1)*spacing[1])*((j-1)*spacing[1]));
				}
				else
				{
					distanceWeights[offsetIndex] = 0;
				}

				offsetIndex++;
			}
		}
	}


	for (samplesIter.GoToBegin(), maskIter.GoToBegin(),outIVIMIter.GoToBegin(),baseIter.GoToBegin(),proposalIter.GoToBegin();
		 !samplesIter.IsAtEnd();
		 ++samplesIter,++maskIter,++outIVIMIter,++baseIter,++proposalIter)
	{

		if (maskIter.GetCenterPixel() != BACKGROUND)
		{
			itkVariableLengthVectorType S = samplesIter.GetCenterPixel();
			itkVariableLengthVectorType baseModel = baseIter.GetCenterPixel();
			itkVariableLengthVectorType proposalModel = proposalIter.GetCenterPixel();


			crlBodyDiffusionTools::MeasureMentVectorType Sb (S.Size());
			for (size_t i=0;i<Sb.size();++i)
			{
				Sb(i)=S[i];
			}



			double cur_b0 = baseModel[0];
			double cur_perfusionCoeff = baseModel[1];
			double cur_diffusionCoeff = baseModel[2];
			double cur_perfusionFractionCoeff = baseModel[3];


			double baseLikelihoodTerm = crlBodyDiffusionTools::computeRicianLikelihood (m_BValues, Sb, m_Variance,
															                          m_Weights,
															                          cur_b0, cur_perfusionCoeff,
															                          cur_diffusionCoeff,cur_perfusionFractionCoeff);


			double proposal_b0 = proposalModel[0];
			double proposal_perfusionCoeff = proposalModel[1];
			double proposal_diffusionCoeff = proposalModel[2];
			double proposal_perfusionFractionCoeff = proposalModel[3];


			double proposalLikelihoodTerm = crlBodyDiffusionTools::computeRicianLikelihood (m_BValues, Sb, m_Variance,
																					   m_Weights,
																					   proposal_b0, proposal_perfusionCoeff,
																					   proposal_diffusionCoeff,proposal_perfusionFractionCoeff);





			// calculate Smoothness data



			double baseSmoothness = 0.0;
			double proposalSmoothness = 0.0;

			for (int i=0;i<27;++i)
			{
				if (maskIter.GetPixel(mask_offset[i]) != BACKGROUND)
				{
					itkVariableLengthVectorType baseNeighborModel = baseIter.GetPixel(offset[i]);
					for (int paramIdx=0;paramIdx<4;++paramIdx)
					{
						// L1 - robust penalty
						baseSmoothness += distanceWeights[i]*fabs(baseModel[paramIdx]-baseNeighborModel[paramIdx])/
																		IVIMSquaredScales[paramIdx];
						proposalSmoothness += distanceWeights[i]*fabs(proposalModel[paramIdx]-baseNeighborModel[paramIdx])/
																	   IVIMSquaredScales[paramIdx];
					}
				}
			}

			double baseEnergy = -baseLikelihoodTerm + 0.001*baseSmoothness/26.0;
			double proposalEnergy = -proposalLikelihoodTerm + 0.001*proposalSmoothness/26.0;




			itkVariableLengthVectorType resModel (4);

			if (baseEnergy < proposalEnergy)
			{
				resModel[0] = baseModel[0];
				resModel[1] = baseModel[1];
				resModel[2] = baseModel[2];
				resModel[3] = baseModel[3];
			}
			else
			{
				resModel[0] = proposalModel[0];
				resModel[1] = proposalModel[1];
				resModel[2] = proposalModel[2];
				resModel[3] = proposalModel[3];
			}
			outIVIMIter.Set (resModel);

		}

	}

	progress.CompletedPixel();



}





template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitICMImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

