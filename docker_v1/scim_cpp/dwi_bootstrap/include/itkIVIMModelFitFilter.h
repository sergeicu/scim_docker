#ifndef __itkIVIMModelFitFilter_h
#define __itkIVIMModelFitFilter_h

#include "crlBodyDiffusionTools.h"
#include "itkImageToImageFilter.h"
#include "itkVariableLengthVector.h"

#include <cstdio>



namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT IVIMModelFitFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef IVIMModelFitFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;




  typedef crlBodyDiffusionTools::MeasureMentVectorType MeasureMentVectorType;

  typedef typename TOutputImage::RegionType OutputImageRegionType;

  typedef typename itk::Image <short,3> itkMaskImageType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( IVIMModelFitFilter, ImageToImageFilter );


  typedef itk::VariableLengthVector< double > itkVariableLengthVectorType;

  /** Type of the output image */
  typedef TOutputImage      OutputImageType;

  static const int SLS;
  static const int ML;
  static const int LS;





  itkSetMacro (BValues, MeasureMentVectorType);
  itkSetMacro (Variance, MeasureMentVectorType);

  itkSetMacro (AlgMode, int);
  itkSetMacro (BSflag, bool);

  itkSetObjectMacro (MaskImage, itkMaskImageType);
  itkGetObjectMacro (IVIMModelImage, TOutputImage);
  itkGetObjectMacro (IVIMModelBSImage, TOutputImage);


  static const int BACKGROUND;
  static const int FOREGROUND;


protected:
  IVIMModelFitFilter();
  virtual ~IVIMModelFitFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  //void GenerateData();


  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  IVIMModelFitFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasureMentVectorType m_BValues;
  MeasureMentVectorType m_Variance;
  size_t m_BValuesNum;


  int m_AlgMode;
  bool m_BSflag;
  size_t m_ADCTresh;


  typename itkMaskImageType::Pointer m_MaskImage;
  typename TOutputImage::Pointer m_IVIMModelImage;
  typename TOutputImage::Pointer m_IVIMModelBSImage;






};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIVIMModelFitFilter.txx"
#endif

#endif

