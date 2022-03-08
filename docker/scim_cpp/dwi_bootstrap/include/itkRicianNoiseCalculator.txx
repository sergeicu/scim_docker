/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRicianNoiseCalculator.txx,v $
  Language:  C++
  Date:      $Date: 2004-04-08 12:04:09 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkRicianNoiseCalculator_txx
#define _itkRicianNoiseCalculator_txx

#include "itkRicianNoiseCalculator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"

#include "gsl/gsl_sf_gamma.h"
namespace itk
{ 
    
/*
 * Constructor
 */
template<class TInputImage>
RicianNoiseCalculator<TInputImage>
::RicianNoiseCalculator() :
 m_Ncoils (1)
{
  m_Image = TInputImage::New();
  m_Variance = NumericTraits<RealType>::Zero;
  m_RegionSetByUser = false;
}


/*
 * Compute Min and Max of m_Image
 */
template<class TInputImage>
void
RicianNoiseCalculator<TInputImage>
::Compute(void)
{
  if( !m_RegionSetByUser )
  {
	  typename TInputImage::IndexType index;
	  index.Fill (0);

	  typename TInputImage::SizeType size;

	  const typename TInputImage::SizeType & orgSize = m_Image->GetLargestPossibleRegion().GetSize();


	  size[0] = orgSize[0]/10;
	  size[1] = orgSize[1]/10;
	  size[2] = orgSize[2];
	  m_Region.SetIndex (index);
	  m_Region.SetSize (size);
  }

  ImageRegionConstIterator< TInputImage >  it( m_Image, m_Region );

  // rician noise
  if (m_Ncoils == 1)
  {
	 // std::cout <<"eneter rician noise" << std::endl;
	//  std::cout << m_Region << std::endl;

	  RealType sum = 0.0;
	  size_t numOfPixels = 0;
	  for (it.GoToBegin();!it.IsAtEnd();++it )
	  {
		  sum += (it.Value()*it.Value());
		  ++numOfPixels;

		//  std::cout << numOfPixels << ": " << sum << std::endl;
	  }
	  m_Variance = vcl_sqrt(sum/(2*numOfPixels));
	//  std::cout << m_Variance << std::endl;
  }
  else if (m_Ncoils > 1)
  {
	  // nc chi-squared noise
	  RealType meanBGSignal = 0.0;
	  size_t numOfPixels = 0;
	  for (it.GoToBegin();!it.IsAtEnd();++it )
	  {
		  meanBGSignal += (it.Value()*it.Value());
		  ++numOfPixels;
	  }

	  meanBGSignal = meanBGSignal/numOfPixels;
  	  m_Variance = meanBGSignal/(vcl_sqrt(2)*(vcl_exp(gsl_sf_lngamma(m_Ncoils+1))/vcl_exp(gsl_sf_lngamma(m_Ncoils-0.5+1.0))));
  }
  else
  {
	  std::cerr << "m_Ncoils  must be >= 1" << std::endl;
	  exit (EXIT_FAILURE);
  }


}





template<class TInputImage>
void
RicianNoiseCalculator<TInputImage>
::SetRegion( const RegionType & region )
{
  m_Region = region;
  m_RegionSetByUser = true;
}


 
template<class TInputImage>
void
RicianNoiseCalculator<TInputImage>
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);


}

} // end namespace itk

#endif
