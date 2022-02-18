#ifndef __itkBValuesToVectorImageFilter_h
#define __itkBValuesToVectorImageFilter_h

#include "crlBodyDiffusionTools.h"
#include "itkImageToImageFilter.h"
#include <itkVectorImage.h>

#include <cstdio>



namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT BValuesToVectorImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef BValuesToVectorImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;


  typedef itk::VariableLengthVector< double > itkVariableLengthVectorType;

  typedef crlBodyDiffusionTools::MeasureMentVectorType MeasureMentVectorType;

  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( BValuesToVectorImageFilter, ImageToImageFilter );


  /** Type of the output image */
  typedef TOutputImage      OutputImageType;






  itkSetMacro (BValues, MeasureMentVectorType);







  itkGetObjectMacro (IVIMModelImage, TOutputImage);
  itkGetObjectMacro (SamplesImage, TOutputImage);
  itkGetMacro (RicianNoiseParam,MeasureMentVectorType);



protected:
  BValuesToVectorImageFilter();
  virtual ~BValuesToVectorImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  //void GenerateData();


  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  BValuesToVectorImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasureMentVectorType m_BValues;
  size_t m_BValuesNum;
  MeasureMentVectorType m_RicianNoiseParam;











  typename TOutputImage::Pointer m_SamplesImage;
  typename TOutputImage::Pointer m_IVIMModelImage;

  static const size_t IVIM_PARAMS_NUM;



};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBValuesToVectorImageFilter.txx"
#endif

#endif

