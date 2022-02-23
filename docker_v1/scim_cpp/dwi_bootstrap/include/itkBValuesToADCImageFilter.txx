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
#ifndef _itkBValuesToADCImageFilter_txx
#define _itkBValuesToADCImageFilter_txx

#include "itkBValuesToADCImageFilter.h"

#include "itkRicianNoiseCalculator.h"


#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#include "itkMedianImageFilter.h"




#include "itkProgressReporter.h"




namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int BValuesToADCImageFilter<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int BValuesToADCImageFilter<TInputImage, TOutputImage>
::FOREGROUND = 1;





template <typename TInputImage,typename TOutputImage>
const int BValuesToADCImageFilter<TInputImage, TOutputImage>
::SLS = 0;

template <typename TInputImage,typename TOutputImage>
const int BValuesToADCImageFilter<TInputImage, TOutputImage>
::ML = 1;

template <typename TInputImage,typename TOutputImage>
const int BValuesToADCImageFilter<TInputImage, TOutputImage>
::LS = 2;

template <typename TInputImage,typename TOutputImage>
BValuesToADCImageFilter<TInputImage, TOutputImage>
::BValuesToADCImageFilter() :
 m_BValues (),
 m_Variance (),
 m_BValuesNum (),
 m_NumberOfBootstarpIterations (1000),
 m_AlgMode (LS),
 m_EM_Iters (5),
 m_ADCTresh (200),
 m_ComputeBootstrap (false),
 m_NumOfSamplePoints (4),
 m_SamplePoints_b0 (m_NumOfSamplePoints*2),
 m_SamplePoints_perfusion (m_NumOfSamplePoints*2),
 m_SamplePoints_diffusion (m_NumOfSamplePoints*2),
 m_SamplePoints_perfusionFraction (m_NumOfSamplePoints*2),
 m_Ncoils (1)
{



	 for (size_t j=0;j<m_NumOfSamplePoints;++j)
	 {
	 	vnl_vector <double> point (m_NumOfSamplePoints);
	 	point.fill(0.0);
	 	point (j) = 1;
	 	m_SamplePoints_b0 (j) = point (0);
	 	m_SamplePoints_perfusion (j) = point (1);
	 	m_SamplePoints_diffusion (j) = point (2);
	 	m_SamplePoints_perfusionFraction (j) = point (3);

 		point.fill(0.0);
 		point (j) = -1;
 		m_SamplePoints_b0 (m_NumOfSamplePoints+j) = point (0);
 		m_SamplePoints_perfusion (m_NumOfSamplePoints+j) = point (1);
 		m_SamplePoints_diffusion (m_NumOfSamplePoints+j) = point (2);
 		m_SamplePoints_perfusionFraction (m_NumOfSamplePoints+j) = point (3);




 	}

}


template <typename TInputImage, typename TOutputImage>
void
BValuesToADCImageFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}


template <typename TInputImage, typename TOutputImage>
void
BValuesToADCImageFilter<TInputImage, TOutputImage>
::BeforeThreadedGenerateData ()
{
	m_BValuesNum = m_BValues.size();

	// compute variances:
	m_Variance.set_size(m_BValuesNum);

	typedef itk::RicianNoiseCalculator <TInputImage> itkRicianNoiseCalculatorType;
	typename itkRicianNoiseCalculatorType::Pointer noiseVarCalc = itkRicianNoiseCalculatorType::New();
	noiseVarCalc->SetNcoils(m_Ncoils);
	for (size_t bIdx=0;bIdx < m_BValuesNum; ++bIdx)
	{
		noiseVarCalc->SetImage(this->GetInput(bIdx));
		noiseVarCalc->Compute();
		std::cout << noiseVarCalc->GetVariance() << " " ;
		m_Variance(bIdx) = noiseVarCalc->GetVariance();
	}
	std::cout << std::endl;

	if (m_MaskImage.IsNull())
	{
		m_MaskImage = TInputImage::New();
		m_MaskImage->SetSpacing (this->GetInput(0)->GetSpacing());
		m_MaskImage->SetOrigin (this->GetInput(0)->GetOrigin());
		m_MaskImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
		m_MaskImage->Allocate();
		m_MaskImage->FillBuffer (FOREGROUND);
	}




	// Get the input and output pointers
	m_DiffusionImage = TOutputImage::New();
	m_DiffusionImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_DiffusionImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_DiffusionImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_DiffusionImage->Allocate();
	m_DiffusionImage->FillBuffer (0.0);

	m_PerfusionImage = TOutputImage::New();
	m_PerfusionImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_PerfusionImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_PerfusionImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_PerfusionImage->Allocate();
	m_PerfusionImage->FillBuffer (0.0);

	m_PerfusionFractionImage = TOutputImage::New();
	m_PerfusionFractionImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_PerfusionFractionImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_PerfusionFractionImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_PerfusionFractionImage->Allocate();
	m_PerfusionFractionImage->FillBuffer (0.0);

	m_B0Image = TOutputImage::New();
	m_B0Image->SetSpacing (this->GetInput(0)->GetSpacing());
	m_B0Image->SetOrigin (this->GetInput(0)->GetOrigin());
	m_B0Image->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_B0Image->Allocate();
	m_B0Image->FillBuffer (0.0);


	// Get the input and output pointers
	m_DiffusionVarImage = TOutputImage::New();
	m_DiffusionVarImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_DiffusionVarImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_DiffusionVarImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_DiffusionVarImage->Allocate();
	m_DiffusionVarImage->FillBuffer (0.0);

	m_PerfusionVarImage = TOutputImage::New();
	m_PerfusionVarImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_PerfusionVarImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_PerfusionVarImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_PerfusionVarImage->Allocate();
	m_PerfusionVarImage->FillBuffer (0.0);

	m_PerfusionFractionVarImage = TOutputImage::New();
	m_PerfusionFractionVarImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_PerfusionFractionVarImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_PerfusionFractionVarImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_PerfusionFractionVarImage->Allocate();
	m_PerfusionFractionVarImage->FillBuffer (0.0);

	m_B0VarImage = TOutputImage::New();
	m_B0VarImage->SetSpacing (this->GetInput(0)->GetSpacing());
	m_B0VarImage->SetOrigin (this->GetInput(0)->GetOrigin());
	m_B0VarImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
	m_B0VarImage->Allocate();
	m_B0VarImage->FillBuffer (0.0);



	// Get the input and output pointers
		m_DiffusionVarLoImage = TOutputImage::New();
		m_DiffusionVarLoImage->SetSpacing (this->GetInput(0)->GetSpacing());
		m_DiffusionVarLoImage->SetOrigin (this->GetInput(0)->GetOrigin());
		m_DiffusionVarLoImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
		m_DiffusionVarLoImage->Allocate();
		m_DiffusionVarLoImage->FillBuffer (0.0);

		m_PerfusionVarLoImage = TOutputImage::New();
		m_PerfusionVarLoImage->SetSpacing (this->GetInput(0)->GetSpacing());
		m_PerfusionVarLoImage->SetOrigin (this->GetInput(0)->GetOrigin());
		m_PerfusionVarLoImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
		m_PerfusionVarLoImage->Allocate();
		m_PerfusionVarLoImage->FillBuffer (0.0);

		m_PerfusionFractionVarLoImage = TOutputImage::New();
		m_PerfusionFractionVarLoImage->SetSpacing (this->GetInput(0)->GetSpacing());
		m_PerfusionFractionVarLoImage->SetOrigin (this->GetInput(0)->GetOrigin());
		m_PerfusionFractionVarLoImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
		m_PerfusionFractionVarLoImage->Allocate();
		m_PerfusionFractionVarLoImage->FillBuffer (0.0);

		m_B0VarLoImage = TOutputImage::New();
		m_B0VarLoImage->SetSpacing (this->GetInput(0)->GetSpacing());
		m_B0VarLoImage->SetOrigin (this->GetInput(0)->GetOrigin());
		m_B0VarLoImage->SetRegions (this->GetInput(0)->GetLargestPossibleRegion());
		m_B0VarLoImage->Allocate();
		m_B0VarLoImage->FillBuffer (0.0);

}

template <typename TInputImage, typename TOutputImage>
void
BValuesToADCImageFilter<TInputImage, TOutputImage>
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
	itkOutputImageRegionIteratorType difIter (m_DiffusionImage,outputRegionForThread);
	itkOutputImageRegionIteratorType perIter (m_PerfusionImage,outputRegionForThread);
	itkOutputImageRegionIteratorType fracIter (m_PerfusionFractionImage,outputRegionForThread);
	itkOutputImageRegionIteratorType b0Iter (m_B0Image, outputRegionForThread);

	itkOutputImageRegionIteratorType difVarIter (m_DiffusionVarImage,outputRegionForThread);
	itkOutputImageRegionIteratorType perVarIter (m_PerfusionVarImage,outputRegionForThread);
	itkOutputImageRegionIteratorType fracVarIter (m_PerfusionFractionVarImage,outputRegionForThread);
	itkOutputImageRegionIteratorType b0VarIter (m_B0VarImage, outputRegionForThread);

	itkOutputImageRegionIteratorType difVarLoIter (m_DiffusionVarLoImage,outputRegionForThread);
	itkOutputImageRegionIteratorType perVarLoIter (m_PerfusionVarLoImage,outputRegionForThread);
	itkOutputImageRegionIteratorType fracVarLoIter (m_PerfusionFractionVarLoImage,outputRegionForThread);
	itkOutputImageRegionIteratorType b0VarLoIter (m_B0VarLoImage, outputRegionForThread);

	itkInputImageRegionConstIteratorType maskIter (m_MaskImage,outputRegionForThread);





	double b0 = 0;
	double perfusionCoeff = 0;
	double diffusionCoeff = 0;
	double perfusionFractionCoeff = 0;

	double b0Var = 0;
	double perfusionCoeffVar = 0;
	double diffusionCoeffVar = 0;
	double perfusionFractionCoeffVar = 0;

	double b0Var_lo = 0;
	double perfusionCoeffVar_lo = 0;
	double diffusionCoeffVar_lo = 0;
	double perfusionFractionCoeffVar_lo = 0;

	for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
	{
		bValuesItersVec[bIdx].GoToBegin();
	}

	for (difIter.GoToBegin(),perIter.GoToBegin(), fracIter.GoToBegin(),b0Iter.GoToBegin(),
		 difVarIter.GoToBegin(),perVarIter.GoToBegin(), fracVarIter.GoToBegin(),b0VarIter.GoToBegin(),
		 difVarLoIter.GoToBegin(),perVarLoIter.GoToBegin(), fracVarLoIter.GoToBegin(),b0VarLoIter.GoToBegin(),
		 maskIter.GoToBegin();
	   	 !difIter.IsAtEnd();
	   	 ++difIter,++perIter,++fracIter,++b0Iter,
	   	 ++difVarIter,++perVarIter,++fracVarIter,++b0VarIter,
	   	++difVarLoIter,++perVarLoIter,++fracVarLoIter,++b0VarLoIter,
	   	 ++maskIter)
	{
	   	//compute ADC

		if (maskIter.Value() != BACKGROUND)
		{
			MeasureMentVectorType Sb (m_BValuesNum);
			MeasureMentVectorType weights (m_BValuesNum);
			for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
			{
				double val = bValuesItersVec[bIdx].Value();
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
			//std::cout << "start LS" << std::endl;
			crlBodyDiffusionTools::computeLinearModel
			                   (m_BValues, Sb, weights,
					            b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);


			int EM_iters = m_EM_Iters;

			if (m_AlgMode > SLS)
			{

				if( m_AlgMode == ML)
				{

				//	std::cout <<"enter ml" << std::endl;
				/*	crlBodyDiffusionTools::computeWeights
		 			               (m_BValues, Sb, m_Variance, weights,
									b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);*/

					crlBodyDiffusionTools::computeNonLinearModel
					                      (m_BValues, Sb, m_Variance,weights, m_Ncoils,
										   b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);

					if (m_ComputeBootstrap)
					{
						crlBodyDiffusionTools::computeBootstrapSE
							                       (m_BValues, Sb, m_Variance, weights,
									 	 	 	 	b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff,
						       					    b0Var, perfusionCoeffVar, diffusionCoeffVar, perfusionFractionCoeffVar,
						       					    b0Var_lo, perfusionCoeffVar_lo, diffusionCoeffVar_lo, perfusionFractionCoeffVar_lo,
						       					    m_NumberOfBootstarpIterations,m_BSscaling);
					}
					else
					{
						diffusionCoeffVar = 0;
						perfusionCoeffVar = 0;
						perfusionFractionCoeffVar = 0;
						b0Var = 0;

						diffusionCoeffVar_lo = 0;
						perfusionCoeffVar_lo = 0;
						perfusionFractionCoeffVar_lo = 0;
						b0Var_lo = 0;
					}

					// std::cout << "start EM" << std::endl;
				}
				else if ( m_AlgMode == LS)
				{
				//	std::cout << "enter ls" << std::endl;
				/*	crlBodyDiffusionTools::computeWeights
					 	               (m_BValues, Sb, m_Variance, weights,
										b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);*/
					crlBodyDiffusionTools::computeNonLinearModelLS
				                      (m_BValues, Sb, m_Variance,weights,
           								   b0, perfusionCoeff, diffusionCoeff, perfusionFractionCoeff);
									// std::cout << "start EM" << std::endl;
				}

			}

		}
		else
		{
			diffusionCoeff = 0;
			perfusionCoeff = 0;
			perfusionFractionCoeff = 0;
			b0 = 0;

			diffusionCoeffVar = 0;
			perfusionCoeffVar = 0;
			perfusionFractionCoeffVar = 0;
			b0Var = 0;

			diffusionCoeffVar_lo = 0;
			perfusionCoeffVar_lo = 0;
			perfusionFractionCoeffVar_lo = 0;
			b0Var_lo = 0;
		}


		b0Iter.Value() = b0;
		difIter.Value() = diffusionCoeff;
		perIter.Value() = perfusionCoeff;
		fracIter.Value() = perfusionFractionCoeff;



		b0VarIter.Value() = b0Var;
		difVarIter.Value() = diffusionCoeffVar;
		perVarIter.Value() = perfusionCoeffVar;
		fracVarIter.Value() = perfusionFractionCoeffVar;


		b0VarLoIter.Value() = b0Var_lo;
		difVarLoIter.Value() = diffusionCoeffVar_lo;
		perVarLoIter.Value() = perfusionCoeffVar_lo;
		fracVarLoIter.Value() = perfusionFractionCoeffVar_lo;



		for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
		{
			++bValuesItersVec[bIdx];
		}
		progress.CompletedPixel();

	}

}





template <typename TInputImage, typename TOutputImage>
void
BValuesToADCImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

