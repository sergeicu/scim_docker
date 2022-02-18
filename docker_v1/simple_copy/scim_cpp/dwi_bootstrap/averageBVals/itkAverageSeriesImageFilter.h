#ifndef __itkAverageSeriesImageFilter_h
#define __itkAverageSeriesImageFilter_h

#include "itkImageToImageFilter.h"

#include <vector>


namespace itk
{

template <typename TInputImage, typename TOutputImage>
class ITK_EXPORT AverageSeriesImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef AverageSeriesImageFilter  Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;


  static const int GeometricAverage;
  static const int ArithmeticAverage;


  typedef typename TOutputImage::RegionType OutputImageRegionType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Type macro that defines a name for this class */
  itkTypeMacro( AverageSeriesImageFilter, ImageToImageFilter );


  /** Type of the output image */
  typedef TOutputImage      OutputImageType;



  void AddWeight (double w) {m_Weights.push_back (w);}

  itkSetMacro( AverageMode, int );
  itkGetMacro( AverageMode, int );






protected:
  AverageSeriesImageFilter();
  virtual ~AverageSeriesImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  //void GenerateData();
  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData() {this->GraftOutput (m_AveragedImage);}
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, int threadId );

private:  
  AverageSeriesImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  std::vector <double> m_Weights;
  typename TOutputImage::Pointer m_AveragedImage;
  int m_AverageMode;


};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAverageSeriesImageFilter.txx"
#endif

#endif

