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
#ifndef _itkIVIMMRFMetricValue_txx
#define _itkIVIMMRFMetricValue_txx

#include "itkIVIMMRFMetricValue.h"




#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkConstNeighborhoodIterator.h"





#include "itkProgressReporter.h"




namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int IVIMMRFMetricValue<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMMRFMetricValue<TInputImage, TOutputImage>
::FOREGROUND = 1;






template <typename TInputImage,typename TOutputImage>
IVIMMRFMetricValue<TInputImage, TOutputImage>
::IVIMMRFMetricValue() :
 m_BValues (),
 m_BValuesNum (),
 m_MetricValue (0.0),
 m_Variance (),
 m_Weights ()

 {

	IVIMSquaredScales[0] = 1.0;
	IVIMSquaredScales[1] = 0.001;
	IVIMSquaredScales[2] = 0.0001;
	IVIMSquaredScales[3] = 0.01;

}







template <typename TInputImage, typename TOutputImage>
void
IVIMMRFMetricValue<TInputImage, TOutputImage>
::GenerateData()
{
	m_MetricValue = 0.0;

	m_BValuesNum = m_BValues.size();

	m_Weights.set_size(m_BValuesNum);
	for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
	{
		m_Weights(bIdx) = 1/m_Variance (bIdx);
	}


	const typename TInputImage::SpacingType & spacing = this->GetInput()->GetSpacing();

	typedef itk::ConstNeighborhoodIterator< TInputImage > itkConstNeighborhoodIteratorType;
	typename itkConstNeighborhoodIteratorType::RadiusType radius;
	radius.Fill(1);
	itkConstNeighborhoodIteratorType inputModelIter( radius, this->GetInput(),this->GetInput()->GetLargestPossibleRegion()  );

	//typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;
	//itkInputImageRegionConstIteratorType samplesIter (m_SignalImage,this->GetInput()->GetLargestPossibleRegion());
	itkConstNeighborhoodIteratorType samplesIter( radius, m_SignalImage,this->GetInput()->GetLargestPossibleRegion()  );



	typedef itk::ConstNeighborhoodIterator< itkMaskImageType > itkConstNeighborhoodMaskIteratorType;
	typename itkConstNeighborhoodMaskIteratorType::RadiusType mask_radius;
	mask_radius.Fill(1);
	itkConstNeighborhoodMaskIteratorType maskIter( radius, m_MaskImage,this->GetInput()->GetLargestPossibleRegion()  );


	typename itkConstNeighborhoodIteratorType::OffsetType offset[27];
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

				/*
				double dist = vcl_sqrt(((i-1)*spacing[0])*((i-1)*spacing[0]) +
									    ((j-1)*spacing[1])*((j-1)*spacing[1]) +
									    ((k-1)*spacing[2])*((k-1)*spacing[2]) );
				if (dist > 0.000001)
				{
					distanceWeights[offsetIndex] = 1.0/dist;
				}
				else
				{
					distanceWeights[offsetIndex] = 0;
				}*/
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




	for (samplesIter.GoToBegin(), maskIter.GoToBegin(),inputModelIter.GoToBegin();
	   	 !samplesIter.IsAtEnd();
	   	 ++samplesIter,++maskIter,++inputModelIter)
	{

		if (maskIter.GetCenterPixel() != BACKGROUND)
		{
			itkVariableLengthVectorType S = samplesIter.GetCenterPixel();
			itkVariableLengthVectorType curModel = inputModelIter.GetCenterPixel();
			double b0 = curModel[0];
			double perfusionCoeff = curModel[1];
			double diffusionCoeff = curModel[2];
			double perfusionFractionCoeff = curModel[3];


			//itkVariableLengthVectorType M (m_BValuesNum);



			// compute gradient for data term
			/*
			for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
			{


				//compute model projected signal
				M[bIdx] = crlBodyDiffusionTools::computeModelEstimateAtBVal
												(b0,perfusionCoeff,diffusionCoeff,perfusionFractionCoeff,
												 m_BValues(bIdx));




				//m_MetricValue += (S[bIdx] - M[bIdx])*(S[bIdx] - M[bIdx]);

			}*/

			crlBodyDiffusionTools::MeasureMentVectorType Sb (S.Size());
			for (size_t i=0;i<Sb.size();++i)
			{
				Sb(i)=S[i];
			}

			double likelihoodTerm = crlBodyDiffusionTools::computeRicianLikelihood (m_BValues, Sb, m_Variance,
																					m_Weights,
																					b0, perfusionCoeff,
																					diffusionCoeff,perfusionFractionCoeff);
			double smoothnessTerm = 0.0;

			// compute gradient for smoothness term
			for (int i=0;i<27;++i)
			{
				if (maskIter.GetPixel(mask_offset[i]) != BACKGROUND)
				{
					itkVariableLengthVectorType curNeighborModel = inputModelIter.GetPixel(offset[i]);

					double neighbor_b0 = curNeighborModel[0];
					double neighbor_perfusionCoeff = curNeighborModel[1];
					double neighbor_diffusionCoeff = curNeighborModel[2];
					double neighbor_perfusionFractionCoeff = curNeighborModel[3];


					// L2
					/*
					smoothnessTerm += distanceWeights[i]*(b0 - neighbor_b0)*(b0 - neighbor_b0)/IVIMSquaredScales[0];
					smoothnessTerm += distanceWeights[i]*(perfusionCoeff - neighbor_perfusionCoeff)*(perfusionCoeff - neighbor_perfusionCoeff)/IVIMSquaredScales[1];
					smoothnessTerm += distanceWeights[i]*(diffusionCoeff - neighbor_diffusionCoeff)*(diffusionCoeff - neighbor_diffusionCoeff)/IVIMSquaredScales[2];
					smoothnessTerm += distanceWeights[i]*(perfusionFractionCoeff - neighbor_perfusionFractionCoeff)*(perfusionFractionCoeff - neighbor_perfusionFractionCoeff)/IVIMSquaredScales[3];
					*/

					smoothnessTerm += distanceWeights[i]*fabs(b0 - neighbor_b0)/IVIMSquaredScales[0];
					smoothnessTerm += distanceWeights[i]*fabs(perfusionCoeff - neighbor_perfusionCoeff)/IVIMSquaredScales[1];
					smoothnessTerm += distanceWeights[i]*fabs(diffusionCoeff - neighbor_diffusionCoeff)/IVIMSquaredScales[2];
					smoothnessTerm += distanceWeights[i]*fabs(perfusionFractionCoeff - neighbor_perfusionFractionCoeff)/IVIMSquaredScales[3];

				}
			}
			m_MetricValue += -likelihoodTerm + 0.01*smoothnessTerm/26.0;


		}







	}

}





template <typename TInputImage, typename TOutputImage>
void
IVIMMRFMetricValue<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

