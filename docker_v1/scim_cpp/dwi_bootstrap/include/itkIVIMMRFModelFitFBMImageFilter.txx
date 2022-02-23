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
#ifndef _itkIVIMMRFModelFitFBMImageFilter_txx
#define _itkIVIMMRFModelFitFBMImageFilter_txx

#include "itkIVIMMRFModelFitFBMImageFilter.h"




#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodIterator.h"





#include "itkProgressReporter.h"

#include <cstdlib>
#include <ctime>

#include "QPBO/QPBO.h"


namespace itk
{
  
template <typename TInputImage,typename TOutputImage>
const int IVIMMRFModelFitFBMImageFilter<TInputImage, TOutputImage>
::BACKGROUND = 0;

template <typename TInputImage,typename TOutputImage>
const int IVIMMRFModelFitFBMImageFilter<TInputImage, TOutputImage>
::FOREGROUND = 1;







template <typename TInputImage,typename TOutputImage>
IVIMMRFModelFitFBMImageFilter<TInputImage, TOutputImage>
::IVIMMRFModelFitFBMImageFilter() :
 m_BValues (),
 m_Variance (),
 m_Weights (),
 m_BValuesNum ()
{
	IVIMSquaredScales[0] = 1.0;
	IVIMSquaredScales[1] = 0.001;
	IVIMSquaredScales[2] = 0.0001;
	IVIMSquaredScales[3] = 0.01;

	// random seed initializer for QPBO optimization
	srand(time(NULL));

}


/*
template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitFBMImageFilter<TInputImage, TOutputImage>
::AfterThreadedGenerateData ()
{
	// TODO

}
*/
/*
template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitFBMImageFilter<TInputImage, TOutputImage>
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
*/
template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitFBMImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
	// construct all required data

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


	// indices image for graph building
	IndicesImageType::Pointer indicesImage = IndicesImageType::New();
	indicesImage->SetOrigin (this->GetInput()->GetOrigin());
	indicesImage->SetSpacing (this->GetInput()->GetSpacing());
	indicesImage->SetRegions (this->GetInput()->GetLargestPossibleRegion());
	indicesImage->Allocate();




	// compute variances:
	//m_Variance.set_size(m_BValuesNum);
	m_Weights.set_size(m_BValuesNum);
	for (size_t bIdx=0;bIdx<m_BValuesNum;++bIdx)
	{
		m_Weights(bIdx) = 1/m_Variance (bIdx);
	}



	// actually compute FBM



	typedef itk::ConstNeighborhoodIterator< TInputImage > itkConstNeighborhoodSamplesIteratorType;
	typename itkConstNeighborhoodSamplesIteratorType::RadiusType samples_radius;
	samples_radius.Fill(1);
	itkConstNeighborhoodSamplesIteratorType samplesIter( samples_radius, this->GetInput(),this->GetInput()->GetLargestPossibleRegion()  );



	typedef itk::ConstNeighborhoodIterator< TOutputImage > itkConstNeighborhoodModelIteratorType;
	typename itkConstNeighborhoodModelIteratorType::RadiusType model_radius;
	model_radius.Fill(1);
	itkConstNeighborhoodModelIteratorType baseIter( model_radius, m_BaseIVIMModelImage,m_BaseIVIMModelImage->GetLargestPossibleRegion()  );
	itkConstNeighborhoodModelIteratorType proposalIter( model_radius, m_ProposalIVIMModelImage,m_ProposalIVIMModelImage->GetLargestPossibleRegion()  );


	typedef itk::ImageRegionIterator <TOutputImage> itkOutputImageRegionIteratorType;
	itkOutputImageRegionIteratorType outIVIMIter (m_OutputIVIMModelImage,m_OutputIVIMModelImage->GetLargestPossibleRegion());

	typedef itk::ImageRegionIterator <itkMaskImageType> itkMaskImageRegionIteratorType;
	itkMaskImageRegionIteratorType maskIter (m_MaskImage, m_MaskImage->GetLargestPossibleRegion());

	typedef itk::NeighborhoodIterator< IndicesImageType > itkNeighborhoodIndicesIteratorType;
	typename itkNeighborhoodIndicesIteratorType::RadiusType indices_radius;
	indices_radius.Fill(1);
	itkNeighborhoodIndicesIteratorType indicesIter( indices_radius, indicesImage,indicesImage->GetLargestPossibleRegion()  );


	const typename TInputImage::SpacingType & spacing = this->GetInput()->GetSpacing();


	double distanceWeights [27];
	typename itkConstNeighborhoodModelIteratorType::OffsetType offset[27];
	typename itkNeighborhoodIndicesIteratorType::OffsetType indices_offset[27];

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
				indices_offset[offsetIndex][0] = i-1;
				indices_offset[offsetIndex][1] = j-1;
				indices_offset[offsetIndex][2] = k-1;

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
				if( (k-1) == 0)
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

	//build graph



	int num_of_nodes = 0;

	for (maskIter.GoToBegin(),indicesIter.GoToBegin(); !maskIter.IsAtEnd(); ++maskIter,++indicesIter)
	{
		if (maskIter.Value() != BACKGROUND )
		{
			indicesIter.SetCenterPixel(num_of_nodes);
	        ++num_of_nodes;
		}
		else
		{
			indicesIter.SetCenterPixel(-1);
		}
	}
	int num_of_edges = (num_of_nodes)*26;



	// build the graph:
	QPBO<float>* qpbo_graph;

	qpbo_graph = new QPBO<float>(num_of_nodes, num_of_edges); // max number of nodes & edges
	qpbo_graph->AddNode(num_of_nodes);




	for (samplesIter.GoToBegin(), indicesIter.GoToBegin(),baseIter.GoToBegin(),proposalIter.GoToBegin();
		 !samplesIter.IsAtEnd();
		 ++samplesIter,++indicesIter,++baseIter,++proposalIter)
	{

		if (indicesIter.GetCenterPixel() > -1)
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

			double proposal_b0 = proposalModel[0];
			double proposal_perfusionCoeff = proposalModel[1];
			double proposal_diffusionCoeff = proposalModel[2];
			double proposal_perfusionFractionCoeff = proposalModel[3];



			//std::cout << "S: " << S << std::endl;
			//std::cout << "Sb: " << Sb << std::endl;
			//std::cout << "m_BValues: " << m_BValues << std::endl;
			//std::cout << "m_Variance: " << m_Variance << std::endl;
			//std::cout << "m_Weights: " << m_Weights << std::endl;
			//std::cout << "baseModel: " << baseModel << std::endl;
			//std::cout << "proposalModel: " << proposalModel << std::endl;

			// calculate Likelihood term

			double baseLikelihoodTerm = crlBodyDiffusionTools::computeRicianLikelihood (m_BValues, Sb, m_Variance,
												                          m_Weights,
												                          cur_b0, cur_perfusionCoeff,
												                          cur_diffusionCoeff,cur_perfusionFractionCoeff);

			double proposalLikelihoodTerm = crlBodyDiffusionTools::computeRicianLikelihood (m_BValues, Sb, m_Variance,
																			   m_Weights,
																			   proposal_b0, proposal_perfusionCoeff,
																			   proposal_diffusionCoeff,proposal_perfusionFractionCoeff);



			qpbo_graph->AddUnaryTerm(indicesIter.GetCenterPixel(), -baseLikelihoodTerm, -proposalLikelihoodTerm);

			//std::cout << "Unary terms: 0-" <<  baseLikelihoodTerm << "; 1-" << proposalLikelihoodTerm <<std::endl;
			// calculate Smoothness term
			for (int i=0;i<27;++i)
			{
				if (indicesIter.GetPixel(indices_offset[i]) > -1 &&
					indicesIter.GetPixel(indices_offset[i]) != indicesIter.GetCenterPixel())
				{
					itkVariableLengthVectorType baseNeighborModel = baseIter.GetPixel(offset[i]);
					itkVariableLengthVectorType proposalNeighborModel = proposalIter.GetPixel(offset[i]);

					double base2baseSmoothness = 0.0;
					double base2proposalSmoothness = 0.0;
					double proposal2baseSmoothness = 0.0;
					double proposal2proposalSmoothness = 0.0;


					for (int paramIdx=0;paramIdx<4;++paramIdx)
					{
						// L1 - robust penalty
						base2baseSmoothness += fabs(baseModel[paramIdx]-baseNeighborModel[paramIdx])/
													IVIMSquaredScales[paramIdx];

						base2proposalSmoothness += fabs(baseModel[paramIdx]-proposalNeighborModel[paramIdx])/
												   IVIMSquaredScales[paramIdx];

						proposal2baseSmoothness += fabs(proposalModel[paramIdx]-baseNeighborModel[paramIdx])/
												   IVIMSquaredScales[paramIdx];

						proposal2proposalSmoothness += fabs(proposalModel[paramIdx]-proposalNeighborModel[paramIdx])/
													   IVIMSquaredScales[paramIdx];

/*
						std::cout << "base 2 base binary terms " << paramIdx << ": "
								  << fabs(baseModel[paramIdx]-baseNeighborModel[paramIdx])/ IVIMSquaredScales[paramIdx] << std::endl;

						std::cout << "base 2 proposal binary terms " << paramIdx << ": "
								  << fabs(baseModel[paramIdx]-proposalNeighborModel[paramIdx])/ IVIMSquaredScales[paramIdx] << std::endl;

						std::cout << "proposal 2 base binary terms " << paramIdx << ": "
								  << fabs(proposalModel[paramIdx]-baseNeighborModel[paramIdx])/ IVIMSquaredScales[paramIdx] << std::endl;

						std::cout << "proposal 2 proposal binary terms " << paramIdx << ": "
								  << fabs(proposalModel[paramIdx]-proposalNeighborModel[paramIdx])/ IVIMSquaredScales[paramIdx] << std::endl;
*/

						// L2 - gaussian model
						/*
						base2baseSmoothness += (baseModel[paramIdx]-baseNeighborModel[paramIdx])*
											   (baseModel[paramIdx]-baseNeighborModel[paramIdx])/
											   IVIMSquaredScales[paramIdx];

						base2proposalSmoothness += (baseModel[paramIdx]-proposalNeighborModel[paramIdx])*
												   (baseModel[paramIdx]-proposalNeighborModel[paramIdx])/
												   IVIMSquaredScales[paramIdx];

						proposal2baseSmoothness += (proposalModel[paramIdx]-baseNeighborModel[paramIdx])*
												   (proposalModel[paramIdx]-baseNeighborModel[paramIdx])/
												   IVIMSquaredScales[paramIdx];

						proposal2proposalSmoothness += (proposalModel[paramIdx]-proposalNeighborModel[paramIdx])*
													   (proposalModel[paramIdx]-proposalNeighborModel[paramIdx])/
													   IVIMSquaredScales[paramIdx];

						*/
					}

					qpbo_graph->AddPairwiseTerm(indicesIter.GetCenterPixel(),
							                    indicesIter.GetPixel(indices_offset[i]),
							                    0.01*base2baseSmoothness*distanceWeights[i]/26.0,
							                    0.01*base2proposalSmoothness*distanceWeights[i]/26.0,
							                    0.01*proposal2baseSmoothness*distanceWeights[i]/26.0,
							                    0.01*proposal2proposalSmoothness*distanceWeights[i]/26.0);
				/*	std::cout << "base 2 base binary terms : " <<  base2baseSmoothness*distanceWeights[i]/26.0 << std::endl;
					std::cout << "base 2 proposal binary terms : " <<  base2proposalSmoothness*distanceWeights[i]/26.0 << std::endl;
					std::cout << "proposal 2 base binary terms : " <<  proposal2baseSmoothness*distanceWeights[i]/26.0 << std::endl;
					std::cout << "proposal 2 proposal binary terms : " <<  proposal2proposalSmoothness*distanceWeights[i]/26.0 << std::endl;
*/



				}
			}


			//	std::cout << cur_energy << ", " << proposal_energy << std::endl;


		}

	}
	qpbo_graph->MergeParallelEdges();
	qpbo_graph->Solve();


	for (int i=0;i<100;++i)
	{
		qpbo_graph->Improve();
	}
	qpbo_graph->ComputeWeakPersistencies();



	for (outIVIMIter.GoToBegin(), indicesIter.GoToBegin(),baseIter.GoToBegin(),proposalIter.GoToBegin();
		 !outIVIMIter.IsAtEnd();
		 ++outIVIMIter, ++indicesIter,++baseIter,++proposalIter)
	{
		if (indicesIter.GetCenterPixel() > -1)
		{
			itkVariableLengthVectorType baseModel = baseIter.GetCenterPixel();
			itkVariableLengthVectorType proposalModel = proposalIter.GetCenterPixel();

			int label = qpbo_graph->GetLabel(indicesIter.GetCenterPixel());
			if (label == 0)
			{
				outIVIMIter.Set(baseModel);
			}
			else if (label == 1)
			{
				outIVIMIter.Set(proposalModel);
			}
			else // unlabeled node
			{
				outIVIMIter.Set(baseModel);
				std::cout << "unlabled node" << std::endl;
			}
		}
	}

	delete qpbo_graph;

}





template <typename TInputImage, typename TOutputImage>
void
IVIMMRFModelFitFBMImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << "TODO " << std::endl;
}




} // end namespace itk

#endif

