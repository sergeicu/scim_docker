#ifndef __itkIVIMMRFMetricValue_h
#define __itkIVIMMRFMetricValue_h


#include "itkImageToImageFilter.h"
#include "itkVariableLengthVector.h"

#include <cstdio>



namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT IVIMMRFMetricValue :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef IVIMMRFMetricValue  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;




  typedef crlBodyDiffusionTools::MeasureMentVectorType MeasureMentVectorType;

  typedef typename TOutputImage::RegionType OutputImageRegionType;

  typedef typename itk::Image <short,3> itkMaskImageType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( IVIMMRFMetricValue, ImageToImageFilter );


  typedef itk::VariableLengthVector< double > itkVariableLengthVectorType;

  /** Type of the output image */
  typedef TOutputImage      OutputImageType;







  itkSetMacro (BValues, MeasureMentVectorType);
  itkSetMacro (Variance, MeasureMentVectorType);
  itkGetMacro (MetricValue, double);




  itkSetObjectMacro (SignalImage, TInputImage);
  itkSetObjectMacro (MaskImage, itkMaskImageType);



  static const int BACKGROUND;
  static const int FOREGROUND;


protected:
  IVIMMRFMetricValue();
  virtual ~IVIMMRFMetricValue() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  void GenerateData();


  //virtual void BeforeThreadedGenerateData();
  //virtual void AfterThreadedGenerateData();
  //void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  IVIMMRFMetricValue(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasureMentVectorType m_BValues;
  size_t m_BValuesNum;
  double m_MetricValue;

  MeasureMentVectorType m_Variance;
  MeasureMentVectorType m_Weights;



  typename itkMaskImageType::Pointer m_MaskImage;
  typename TOutputImage::Pointer m_IVIMModelImage;
  typename TInputImage::Pointer m_SignalImage;


  double IVIMSquaredScales[4];



};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIVIMMRFMetricValue.txx"
#endif

#endif

