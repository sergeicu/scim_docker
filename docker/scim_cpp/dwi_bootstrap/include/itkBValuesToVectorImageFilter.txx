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
#ifndef _itkBValuesToVectorImageFilter_txx
#define _itkBValuesToVectorImageFilter_txx

#include "itkBValuesToVectorImageFilter.h"
#include "itkVariableLengthVector.h"
#include "itkRicianNoiseCalculator.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"


#include "itkProgressReporter.h"




namespace itk
{

template <typename TInputImage,typename TOutputImage>
const size_t BValuesToVectorImageFilter<TInputImage, TOutputImage>
::IVIM_PARAMS_NUM = 4;


template <typename TInputImage,typename TOutputImage>
BValuesToVectorImageFilter<TInputImage, TOutputImage>
::BValuesToVectorImageFilter() :
 m_BValues (),
 m_BValuesNum (),
 m_RicianNoiseParam ()
{




}


template <typename TInputImage, typename TOutputImage>
void
BValuesToVectorImageFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}


template <typename TInputImage, typename TOutputImage>
void
BValuesToVectorImageFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData ()
{


	m_BValuesNum = m_BValues.size();





	// Get the input and output pointers
	m_SamplesImage = TOutputImage::New();
	m_SamplesImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_SamplesImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_SamplesImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_SamplesImage->SetVectorLength(m_BValuesNum);
	m_SamplesImage->Allocate();




	itkVariableLengthVectorType samplesVectorZeros(m_BValuesNum);
	samplesVectorZeros.Fill (0.0);
	m_SamplesImage->FillBuffer (samplesVectorZeros);


	m_IVIMModelImage = TOutputImage::New();
	m_IVIMModelImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_IVIMModelImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_IVIMModelImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_IVIMModelImage->SetVectorLength(IVIM_PARAMS_NUM);
	m_IVIMModelImage->Allocate();



	itkVariableLengthVectorType IVIMVectorZeros(IVIM_PARAMS_NUM);
	IVIMVectorZeros.Fill (0.0);
	m_IVIMModelImage->FillBuffer (IVIMVectorZeros);




	// cacluclate noise param
	m_RicianNoiseParam.set_size(m_BValuesNum);
	typedef itk::RicianNoiseCalculator <TInputImage> itkRicianNoiseCalculatorType;
	typename itkRicianNoiseCalculatorType::Pointer noiseVarCalc = itkRicianNoiseCalculatorType::New();

	double global_m_variance; 
	std::cout << "Please provide fixed variance value for all b-values"  << std::endl;
	std::cin >> global_m_variance;

	//double global_m_variance = 2;
	for (size_t bIdx=0;bIdx < m_BValuesNum; ++bIdx)
	{
		// modified by sv407 to remove noise calc
		//noiseVarCalc->SetImage(this->GetInput(bIdx));
		//noiseVarCalc->Compute();
		//std::cout << noiseVarCalc->GetVariance() << " " ;
		//m_RicianNoiseParam(bIdx) = noiseVarCalc->GetVariance();

		m_RicianNoiseParam(bIdx) = global_m_variance;
	}

	std::cout << "m_RicianNoiseParam parameter in itkBValuesToVectorImageFilter is hard set to:"  << std::endl;
	std::cout << global_m_variance << std::endl;
	std::cout << "variance vector is:"  << std::endl;
	std::cout << m_RicianNoiseParam << std::endl;

}

template <typename TInputImage, typename TOutputImage>
void
BValuesToVectorImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

	itkDebugMacro(<<"Actually executing");



	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

	typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;
	std::vector <itkInputImageRegionConstIteratorType> bValuesItersVec;
	for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
	{
		itkInputImageRegionConstIteratorType bIter (this->GetInput(bIdx),outputRegionForThread);
		bValuesItersVec.push_back(bIter);
	}

	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType samplesIter (m_SamplesImage,outputRegionForThread);

	for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
	{
		bValuesItersVec[bIdx].GoToBegin();
	}

	for (samplesIter.GoToBegin();
	   	 !samplesIter.IsAtEnd();
	   	 ++samplesIter)
	{
	   	// convert b-values samples to vector
		itkVariableLengthVectorType S (m_BValuesNum);
		for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
		{
			double val = bValuesItersVec[bIdx].Value();
			S[bIdx] = val;
		}

		samplesIter.Set(S);

		for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
		{
			++bValuesItersVec[bIdx];
		}
		progress.CompletedPixel();

	}

}





template <typename TInputImage, typename TOutputImage>
void
BValuesToVectorImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

