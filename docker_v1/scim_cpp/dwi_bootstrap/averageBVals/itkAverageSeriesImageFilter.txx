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
#ifndef _itkAverageSeriesImageFilter_txx
#define _itkAverageSeriesImageFilter_txx



#include "itkAverageSeriesImageFilter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"



#include "itkProgressReporter.h"


namespace itk
{
  

template <typename TInputImage,typename TOutputImage>
const int AverageSeriesImageFilter<TInputImage, TOutputImage>::GeometricAverage = 1;

template <typename TInputImage,typename TOutputImage>
const int AverageSeriesImageFilter<TInputImage, TOutputImage>::ArithmeticAverage = 0;


template <typename TInputImage,typename TOutputImage>
AverageSeriesImageFilter<TInputImage, TOutputImage>
::AverageSeriesImageFilter() :
 m_Weights ()
{

}


template <typename TInputImage, typename TOutputImage>
void
AverageSeriesImageFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData ()
{
	m_AveragedImage = TOutputImage::New();
	m_AveragedImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_AveragedImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_AveragedImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_AveragedImage->Allocate();
	m_AveragedImage->FillBuffer (0);

}

template <typename TInputImage, typename TOutputImage>
void
AverageSeriesImageFilter<TInputImage, TOutputImage>
:: ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId )
{
	int numOfInputs = m_Weights.size();

	typedef itk::ImageRegionConstIterator <TInputImage> InputImageRegionConstIteratorType;

	std::vector<InputImageRegionConstIteratorType> inputItersVec;

	for (int inputIdx=0;inputIdx<numOfInputs;++inputIdx)
	{
		InputImageRegionConstIteratorType iter (this->GetInput (inputIdx), this->GetInput (inputIdx)->GetLargestPossibleRegion());
		inputItersVec.push_back (iter);
	}







	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());




	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType aveIter (m_AveragedImage,m_AveragedImage->GetLargestPossibleRegion());


	for (int inputIdx=0;inputIdx<numOfInputs;++inputIdx)
	{
		inputItersVec[inputIdx].GoToBegin();
	}

	double ave = 0;
	double weightsSum = 0;


	if (m_AverageMode == ArithmeticAverage)
	{

		for (aveIter.GoToBegin();
			 !aveIter.IsAtEnd();
			 ++aveIter)
		{
			//compute average
			ave = 0;
			weightsSum = 0;
			for (int inputIdx=0;inputIdx<numOfInputs;++inputIdx)
			{
				ave += inputItersVec[inputIdx].Value()*m_Weights[inputIdx];
				weightsSum += m_Weights[inputIdx];

			}
			ave = ave / weightsSum;

			aveIter.Value() = static_cast <typename TOutputImage::PixelType> (ave);

			for (int inputIdx=0;inputIdx<numOfInputs;++inputIdx)
			{
				++inputItersVec[inputIdx];
			}
		}
	}
	else if (m_AverageMode == GeometricAverage)
	{
		for (aveIter.GoToBegin();
			 !aveIter.IsAtEnd();
			 ++aveIter)
		{
			//compute average
			ave = 1;

			for (int inputIdx=0;inputIdx<numOfInputs;++inputIdx)
			{
				ave *= inputItersVec[inputIdx].Value();

			}
			ave = pow (ave,double(1.0/numOfInputs));

			aveIter.Value() = static_cast <typename TOutputImage::PixelType> (ave);

			for (int inputIdx=0;inputIdx<numOfInputs;++inputIdx)
			{
				++inputItersVec[inputIdx];
			}
		}
	}
}




template <typename TInputImage, typename TOutputImage>
void
AverageSeriesImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}



} // end namespace itk

#endif

