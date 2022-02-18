#ifndef __itkIVIMMRFModelFitICMImageFilter_h
#define __itkIVIMMRFModelFitICMImageFilter_h

#include "crlBodyDiffusionTools.h"
#include "itkImageToImageFilter.h"
#include "itkVariableLengthVector.h"


#include <cstdio>



namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT IVIMMRFModelFitICMImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef IVIMMRFModelFitICMImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;




  typedef crlBodyDiffusionTools::MeasureMentVectorType MeasureMentVectorType;

  typedef typename TOutputImage::RegionType OutputImageRegionType;

  typedef typename itk::Image <short,3> itkMaskImageType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( IVIMMRFModelFitICMImageFilter, ImageToImageFilter );


  typedef itk::VariableLengthVector< double > itkVariableLengthVectorType;

  /** Type of the output image */
  typedef TOutputImage      OutputImageType;






  itkSetMacro (BValues, MeasureMentVectorType);
  itkSetMacro (Variance, MeasureMentVectorType);

  itkSetObjectMacro (BaseIVIMModelImage, TOutputImage);
  itkSetObjectMacro (ProposalIVIMModelImage, TOutputImage);


  itkSetObjectMacro (MaskImage, itkMaskImageType);
  itkGetObjectMacro (OutputIVIMModelImage, TOutputImage);



  static const int BACKGROUND;
  static const int FOREGROUND;


protected:
  IVIMMRFModelFitICMImageFilter();
  virtual ~IVIMMRFModelFitICMImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  //void GenerateData();


  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  IVIMMRFModelFitICMImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasureMentVectorType m_BValues;
  MeasureMentVectorType m_Variance;
  MeasureMentVectorType m_Weights;
  size_t m_BValuesNum;
  double IVIMSquaredScales [4];






  typename itkMaskImageType::Pointer m_MaskImage;
  typename TOutputImage::Pointer m_BaseIVIMModelImage;
  typename TOutputImage::Pointer m_ProposalIVIMModelImage;
  typename TOutputImage::Pointer m_OutputIVIMModelImage;







};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIVIMMRFModelFitICMImageFilter.txx"
#endif

#endif

