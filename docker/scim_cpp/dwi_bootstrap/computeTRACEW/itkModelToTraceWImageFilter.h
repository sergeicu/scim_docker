#ifndef __itkModelToTraceWImageFilter_h
#define __itkModelToTraceWImageFilter_h

#include "itkImageToImageFilter.h"




namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT ModelToTraceWImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ModelToTraceWImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;





  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( ModelToTraceWImageFilter, ImageToImageFilter );


  /** Type of the output image */
  typedef TOutputImage      OutputImageType;





  itkSetMacro (BValue, double)


  itkSetObjectMacro (ADCImage, TInputImage);
  itkSetObjectMacro (B0Image, TInputImage);
  itkSetObjectMacro (PERImage, TInputImage);
  itkSetObjectMacro (PER_FRACImage, TInputImage);
  itkSetObjectMacro (BaseImage, TInputImage);
  itkSetObjectMacro (MaskImage, TInputImage);





  static const double EPSILON;

protected:
  ModelToTraceWImageFilter();
  virtual ~ModelToTraceWImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  void GenerateData();
  //void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  ModelToTraceWImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  double m_BValue;





  typename TInputImage::Pointer m_ADCImage;
  typename TInputImage::Pointer m_B0Image;
  typename TInputImage::Pointer m_PERImage;
  typename TInputImage::Pointer m_PER_FRACImage;
  typename TInputImage::Pointer m_BaseImage;
  typename TInputImage::Pointer m_MaskImage;






};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkModelToTraceWImageFilter.txx"
#endif

#endif

