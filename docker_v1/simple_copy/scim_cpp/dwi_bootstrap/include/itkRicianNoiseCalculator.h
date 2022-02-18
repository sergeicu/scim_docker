/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkRicianNoiseCalculator.h,v $
  Language:  C++
  Date:      $Date: 2005-01-21 20:20:27 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkRicianNoiseCalculator_h
#define __itkRicianNoiseCalculator_h

#include "itkObject.h"
#include "itkObjectFactory.h"

namespace itk
{

/** This calculator computes the Rician noise variance in an MRI Image
 * using a rayleigh estimator using the corner of the image as background
 * It is templated over input image type.
 *
 * \ingroup Operators
 */
template <class TInputImage>            
class ITK_EXPORT RicianNoiseCalculator : public Object
{
public:
  /** Standard class typedefs. */
  typedef RicianNoiseCalculator Self;
  typedef Object  Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(RicianNoiseCalculator, Object);

  /** Type definition for the input image. */
  typedef TInputImage  ImageType;

  /** Pointer type for the image. */
  typedef typename TInputImage::Pointer  ImagePointer;
  
  /** Const Pointer type for the image. */
  typedef typename TInputImage::ConstPointer ImageConstPointer;

  /** Type definition for the input image pixel type. */
  typedef typename TInputImage::PixelType PixelType;
  
  /** Type definition for the input image index type. */
  typedef typename TInputImage::IndexType IndexType;
  
  /** Type definition for the input image region type. */
  typedef typename TInputImage::RegionType RegionType;
  

  typedef double RealType;

  /** Set the input image. */
  itkSetConstObjectMacro(Image,ImageType);


  /** Compute the minimum and maximum values of intensity of the input image. */
  void Compute(void);

  /** Return the minimum intensity value. */
  itkGetMacro(Variance,RealType);

  itkSetMacro(Ncoils,int);
  

  /** Set the region over which the values will be computed */
  void SetRegion( const RegionType & region );

protected:
  RicianNoiseCalculator();
  virtual ~RicianNoiseCalculator() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

private:
  RicianNoiseCalculator(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  RealType             m_Variance;
  ImageConstPointer    m_Image;
  int 				   m_Ncoils;

  RegionType           m_Region;
  bool                 m_RegionSetByUser;
};

} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkRicianNoiseCalculator.txx"
#endif

#endif /* __itkRicianNoiseCalculator_h */
