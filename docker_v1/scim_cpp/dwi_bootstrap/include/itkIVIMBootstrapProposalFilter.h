#ifndef __itkIVIMBootstrapProposalFilter_h
#define __itkIVIMBootstrapProposalFilter_h

#include "crlBodyDiffusionTools.h"
#include "itkImageToImageFilter.h"
#include "itkVariableLengthVector.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <cstdio>



namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT IVIMBootstrapProposalFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef IVIMBootstrapProposalFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;




  typedef crlBodyDiffusionTools::MeasureMentVectorType MeasureMentVectorType;

  typedef typename TOutputImage::RegionType OutputImageRegionType;

  typedef typename itk::Image <short,3> itkMaskImageType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( IVIMBootstrapProposalFilter, ImageToImageFilter );


  typedef itk::VariableLengthVector< double > itkVariableLengthVectorType;

  /** Type of the output image */
  typedef TOutputImage      OutputImageType;

  static const int UNWEIGHTED;
  static const int FIXED_WEIGHTS;
  static const int UNSCENTED_WEIGHTS;





  itkSetMacro (BValues, MeasureMentVectorType);
  itkSetMacro (Variance, MeasureMentVectorType);
  itkSetObjectMacro (InitialIVIMModelImage, TOutputImage);


  itkSetMacro (BootstrapMode, int);

  itkSetObjectMacro (MaskImage, itkMaskImageType);
  itkGetObjectMacro (BSIVIMProposalImage, TOutputImage);



  static const int BACKGROUND;
  static const int FOREGROUND;


protected:
  IVIMBootstrapProposalFilter();
  virtual ~IVIMBootstrapProposalFilter() {gsl_rng_free(m_gsl_rand_gen);};
  void PrintSelf(std::ostream& os, Indent indent) const;

  //void GenerateData();


  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  IVIMBootstrapProposalFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  MeasureMentVectorType m_BValues;
  MeasureMentVectorType m_Variance;
  size_t m_BValuesNum;


  int m_BootstrapMode;
  size_t m_ADCTresh;


  typename itkMaskImageType::Pointer m_MaskImage;
  typename TOutputImage::Pointer m_InitialIVIMModelImage;
  typename TOutputImage::Pointer m_BSIVIMProposalImage;

  gsl_rng * m_gsl_rand_gen;





};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIVIMBootstrapProposalFilter.txx"
#endif

#endif

