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
#ifndef _itkIVIMMRFModelFitCICMmageFilter_txx
#define _itkIVIMMRFModelFitCICMmageFilter_txx

#include "itkIVIMMRFModelFitCICMImageFilter.h"




#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkConstNeighborhoodIterator.h"
//#include "itkDiffusionModelFittingSmoothedMLECostFunction.h"

#include "crlBodyDiffusionTools.h"



#include "itkProgressReporter.h"




namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int IVIMMRFModelFitCICMImageFilter<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMMRFModelFitCICMImageFilter<TInputImage, TOutputImage>
::FOREGROUND = 1;








template <typename TInputImage,typename TOutputImage>
IVIMMRFModelFitCICMImageFilter<TInputImage, TOutputImage>
::IVIMMRFModelFitCICMImageFilter() :
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
IVIMMRFModelFitCICMImageFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO
}


template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitCICMImageFilter<TInputImage, TOutputImage>
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
IVIMMRFModelFitCICMImageFilter<TInputImage, TOutputImage>
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


	for (samplesIter.GoToBegin(), maskIter.GoToBegin(),outIVIMIter.GoToBegin(),baseIter.GoToBegin();
		 !samplesIter.IsAtEnd();
		 ++samplesIter,++maskIter,++outIVIMIter,++baseIter)
	{

		if (maskIter.GetCenterPixel() != BACKGROUND)
		{
			itkVariableLengthVectorType S = samplesIter.GetCenterPixel();
			itkVariableLengthVectorType baseModel = baseIter.GetCenterPixel();

			crlBodyDiffusionTools::MeasureMentVectorType Sb (S.Size());
			for (size_t i=0;i<Sb.size();++i)
			{
				Sb(i)=S[i];
			}


		//	std::cout <<  "Sb : " << Sb << std::endl;
		//	std::cout <<  "baseModel : " << baseModel << std::endl;




			double cur_b0 = baseModel[0];
			double cur_perfusionCoeff = baseModel[1];
			double cur_diffusionCoeff = baseModel[2];
			double cur_perfusionFractionCoeff = baseModel[3];




			// calculate Smoothness data
			crlBodyDiffusionTools::MeasureMentVectorType neighborhood_Weights (27);
			crlBodyDiffusionTools::MeasureMentVectorType neighborhood_B0 (27);
			crlBodyDiffusionTools::MeasureMentVectorType neighborhood_Dstar (27);
			crlBodyDiffusionTools::MeasureMentVectorType neighborhood_D (27);
			crlBodyDiffusionTools::MeasureMentVectorType neighborhood_f (27);

			for (int i=0;i<27;++i)
			{
				if (maskIter.GetPixel(mask_offset[i]) != BACKGROUND)
				{

					neighborhood_B0(i)=baseIter.GetPixel(offset[i])[0];
					neighborhood_Dstar(i)=baseIter.GetPixel(offset[i])[1];
					neighborhood_D(i)=baseIter.GetPixel(offset[i])[2];
					neighborhood_f(i)=baseIter.GetPixel(offset[i])[3];

					neighborhood_Weights(i)=distanceWeights[i];
				}
				else
				{
					neighborhood_B0(i)=0;
					neighborhood_Dstar(i)=0;
					neighborhood_D(i)=0;
					neighborhood_f(i)=0;

					neighborhood_Weights(i)=0;
				}

			}

			//std::cout <<  "neighborhood_B0      : " << neighborhood_B0 << std::endl;
			//std::cout <<  "neighborhood_Dstar   : " << neighborhood_Dstar << std::endl;
			//std::cout <<  "neighborhood_D       : " << neighborhood_D << std::endl;
			//std::cout <<  "neighborhood_f       : " << neighborhood_f << std::endl;
			//std::cout <<  "neighborhood_Weights : " << neighborhood_Weights << std::endl;

			crlBodyDiffusionTools::computeSmoothedNonLinearModel (m_BValues,Sb, m_Variance, m_Weights,
																  neighborhood_D, neighborhood_Dstar, neighborhood_f, neighborhood_B0,
																  neighborhood_Weights, 1,
																  cur_b0, cur_perfusionCoeff, cur_diffusionCoeff, cur_perfusionFractionCoeff);



			itkVariableLengthVectorType resModel (4);
			resModel[0] = cur_b0;
			resModel[1] = cur_perfusionCoeff;
			resModel[2] = cur_diffusionCoeff;
			resModel[3] = cur_perfusionFractionCoeff;

			outIVIMIter.Set (resModel);

		}

	}

	progress.CompletedPixel();



}





template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitCICMImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

