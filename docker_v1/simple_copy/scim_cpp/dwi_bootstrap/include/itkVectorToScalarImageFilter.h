#ifndef __itkVectorToScalarImageFilter_h
#define __itkVectorToScalarImageFilter_h

#include "itkImageToImageFilter.h"
#include <itkVectorImage.h>
#include <itkImage.h>

#include <cstdio>



namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT VectorToScalarImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef VectorToScalarImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;





  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( VectorToScalarImageFilter, ImageToImageFilter );


  /** Type of the output image */
  typedef TOutputImage      OutputImageType;













  itkGetObjectMacro (DImage, TOutputImage);
  itkGetObjectMacro (DstarImage, TOutputImage);
  itkGetObjectMacro (b0Image, TOutputImage);
  itkGetObjectMacro (fImage, TOutputImage);




protected:
  VectorToScalarImageFilter();
  virtual ~VectorToScalarImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  //void GenerateData();


  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  VectorToScalarImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented















  typename TOutputImage::Pointer m_DImage;
  typename TOutputImage::Pointer m_DstarImage;
  typename TOutputImage::Pointer m_b0Image;
  typename TOutputImage::Pointer m_fImage;





};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVectorToScalarImageFilter.txx"
#endif

#endif

