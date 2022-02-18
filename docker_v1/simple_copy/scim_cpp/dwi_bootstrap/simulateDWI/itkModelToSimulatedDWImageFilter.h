#ifndef __itkModelToSimulatedDWImageFilter_h
#define __itkModelToSimulatedDWImageFilter_h

#include "itkImageToImageFilter.h"


#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"


namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT ModelToSimulatedDWImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef ModelToSimulatedDWImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;





  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( ModelToSimulatedDWImageFilter, ImageToImageFilter );


  /** Type of the output image */
  typedef TOutputImage      OutputImageType;





  itkSetMacro (BValue, double)
  itkSetMacro (Sigma, double)


  itkSetObjectMacro (ADCImage, TInputImage);
  itkSetObjectMacro (B0Image, TInputImage);
  itkSetObjectMacro (PERImage, TInputImage);
  itkSetObjectMacro (PER_FRACImage, TInputImage);





  static const double EPSILON;

protected:
  ModelToSimulatedDWImageFilter();
  virtual ~ModelToSimulatedDWImageFilter() {  gsl_rng_free (randGen); };
  void PrintSelf(std::ostream& os, Indent indent) const;
  void GenerateData();
  //void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  ModelToSimulatedDWImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  double m_BValue;
  double m_Sigma;





  typename TInputImage::Pointer m_ADCImage;
  typename TInputImage::Pointer m_B0Image;
  typename TInputImage::Pointer m_PERImage;
  typename TInputImage::Pointer m_PER_FRACImage;


  gsl_rng * randGen;
  const gsl_rng_type * T;



};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkModelToSimulatedDWImageFilter.txx"
#endif

#endif

