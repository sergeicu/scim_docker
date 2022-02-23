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
#ifndef _itkVectorToScalarImageFilter_txx
#define _itkVectorToScalarImageFilter_txx

#include "itkVectorToScalarImageFilter.h"
#include "itkVariableLengthVector.h"


#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"


#include "itkProgressReporter.h"




namespace itk
{




template <typename TInputImage,typename TOutputImage>
VectorToScalarImageFilter<TInputImage, TOutputImage>
::VectorToScalarImageFilter()
{




}


template <typename TInputImage, typename TOutputImage>
void
VectorToScalarImageFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}


template <typename TInputImage, typename TOutputImage>
void
VectorToScalarImageFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData ()
{






	// Get the input and output pointers
	m_DImage = TOutputImage::New();
	m_DImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_DImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_DImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_DImage->Allocate();
	m_DImage->FillBuffer (0.0);


	m_DstarImage = TOutputImage::New();
	m_DstarImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_DstarImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_DstarImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_DstarImage->Allocate();
	m_DstarImage->FillBuffer (0.0);


	m_b0Image = TOutputImage::New();
	m_b0Image->SetSpacing (this->GetInput(0)->GetSpacing());
	m_b0Image->SetOrigin (this->GetInput(0)->GetOrigin());
	m_b0Image->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_b0Image->Allocate();
	m_b0Image->FillBuffer (0.0);


	m_fImage = TOutputImage::New();
	m_fImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_fImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_fImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_fImage->Allocate();
	m_fImage->FillBuffer (0.0);




}

template <typename TInputImage, typename TOutputImage>
void
VectorToScalarImageFilter<TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{

	itkDebugMacro(<<"Actually executing");



	// support progress methods/callbacks
	ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

	typedef itk::ImageRegionConstIterator <TInputImage> itkInputImageRegionConstIteratorType;
	itkInputImageRegionConstIteratorType ivimIter (this->GetInput(),outputRegionForThread);

	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType b0Iter (m_b0Image,outputRegionForThread);
	itkOutputImageRegionIteratorType DstarIter (m_DstarImage,outputRegionForThread);
	itkOutputImageRegionIteratorType DIter (m_DImage,outputRegionForThread);
	itkOutputImageRegionIteratorType fIter (m_fImage,outputRegionForThread);


	for (ivimIter.GoToBegin(), b0Iter.GoToBegin(), DstarIter.GoToBegin(), DIter.GoToBegin(), fIter.GoToBegin();
	   	 !ivimIter.IsAtEnd();
	   	 ++ivimIter,++b0Iter,++DstarIter,++DIter,++fIter)
	{

		b0Iter.Value() = ivimIter.Get()[0];
		DstarIter.Value() = ivimIter.Get()[1];
		DIter.Value() = ivimIter.Get()[2];
		fIter.Value() = ivimIter.Get()[3];


		progress.CompletedPixel();

	}

}





template <typename TInputImage, typename TOutputImage>
void
VectorToScalarImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

