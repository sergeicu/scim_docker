#ifndef __itkBValuesToADCImageFilter_h
#define __itkBValuesToADCImageFilter_h

#include "crlBodyDiffusionTools.h"
#include "itkImageToImageFilter.h"


#include <cstdio>



namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT BValuesToADCImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef BValuesToADCImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;



  typedef crlBodyDiffusionTools::MeasureMentVectorType MeasureMentVectorType;

  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( BValuesToADCImageFilter, ImageToImageFilter );


  /** Type of the output image */
  typedef TOutputImage      OutputImageType;

 static const int SLS;
 static const int ML;
 static const int LS;



  itkSetMacro (ComputeBootstrap, bool);
  itkSetMacro (BSscaling, int);

  itkSetMacro (BValues, MeasureMentVectorType);

  itkSetMacro (AlgMode, int);
  itkSetMacro (Ncoils, int);

  itkSetMacro (EM_Iters, size_t);
  itkSetMacro (NumberOfBootstarpIterations,int);

  itkSetObjectMacro (MaskImage, TInputImage);

  itkGetObjectMacro (DiffusionImage, TOutputImage);
  itkGetObjectMacro (PerfusionImage, TOutputImage);
  itkGetObjectMacro (PerfusionFractionImage, TOutputImage);
  itkGetObjectMacro (B0Image, TOutputImage);

  itkGetObjectMacro (DiffusionVarImage, TOutputImage);
  itkGetObjectMacro (PerfusionVarImage, TOutputImage);
  itkGetObjectMacro (PerfusionFractionVarImage, TOutputImage);
  itkGetObjectMacro (B0VarImage, TOutputImage);

  itkGetObjectMacro (DiffusionVarLoImage, TOutputImage);
  itkGetObjectMacro (PerfusionVarLoImage, TOutputImage);
  itkGetObjectMacro (PerfusionFractionVarLoImage, TOutputImage);
  itkGetObjectMacro (B0VarLoImage, TOutputImage);


  static const int BACKGROUND;
  static const int FOREGROUND;


protected:
  BValuesToADCImageFilter();
  virtual ~BValuesToADCImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  //void GenerateData();


  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  BValuesToADCImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasureMentVectorType m_BValues;
  MeasureMentVectorType m_Variance;
  size_t m_BValuesNum;
  int m_NumberOfBootstarpIterations;


  int m_AlgMode;
  size_t m_EM_Iters;
  size_t m_ADCTresh;

  bool m_ComputeBootstrap;
  int m_BSscaling;




  typename TInputImage::Pointer m_MaskImage;


  typename TOutputImage::Pointer m_DiffusionImage;
  typename TOutputImage::Pointer m_PerfusionImage;
  typename TOutputImage::Pointer m_PerfusionFractionImage;
  typename TOutputImage::Pointer m_B0Image;

  typename TOutputImage::Pointer m_DiffusionVarImage;
  typename TOutputImage::Pointer m_PerfusionVarImage;
  typename TOutputImage::Pointer m_PerfusionFractionVarImage;
  typename TOutputImage::Pointer m_B0VarImage;

  typename TOutputImage::Pointer m_DiffusionVarLoImage;
  typename TOutputImage::Pointer m_PerfusionVarLoImage;
  typename TOutputImage::Pointer m_PerfusionFractionVarLoImage;
  typename TOutputImage::Pointer m_B0VarLoImage;


  size_t m_NumOfSamplePoints;
  MeasureMentVectorType m_SamplePoints_b0;
  MeasureMentVectorType m_SamplePoints_perfusion;
  MeasureMentVectorType m_SamplePoints_diffusion;
  MeasureMentVectorType m_SamplePoints_perfusionFraction;


  int m_Ncoils;


};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBValuesToADCImageFilter.txx"
#endif

#endif

