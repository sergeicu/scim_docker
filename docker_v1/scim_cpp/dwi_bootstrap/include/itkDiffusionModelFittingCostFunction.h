/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkPointSetToPointSetMetric.h,v $
  Language:  C++
  Date:      $Date: 2003-11-08 17:58:32 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkDiffusionModelFittingCostFunction_h
#define __itkDiffusionModelFittingCostFunction_h

#include "itkMultipleValuedCostFunction.h"
#include "itkExceptionObject.h"

#include "vnl/vnl_vector.h"

#include "itkArray.h"
#include "itkArray2D.h"












namespace itk
{
class ITK_EXPORT DiffusionModelFittingCostFunction : public MultipleValuedCostFunction
{
public:

  /** Standard class typedefs. */
  typedef DiffusionModelFittingCostFunction        Self;
  typedef SingleValuedCostFunction      Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  typedef Superclass::MeasureType     MeasureType;
  typedef Superclass::DerivativeType DerivativeType;
  typedef Superclass::ParametersType 	ParametersType;



  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(DiffusionModelFittingCostFunction, MultipleValuedCostFunction);

  itkSetMacro (NumOfParams,int);
  itkGetMacro (NumOfParams,int);

  itkSetMacro (Dimension,int);
  itkGetMacro (Dimension,int);

  void SetObservedValues (const vnl_vector<double> & b_vals,
 		                  const vnl_vector<double> & b_signals,
 		                  const vnl_vector<double> & b_variance,
 		                  const vnl_vector<double> & b_weights);

  itk::Array<double> GetValue (const ParametersType & params) const;


  unsigned int 	GetNumberOfParameters (void) const { return m_NumOfParams;}

  unsigned int 	GetNumberOfValues (void) const { return m_Dimension;}

   /** This method returns the derivative of the cost function corresponding
     * to the specified parameters.   */
   virtual void GetDerivative( const itk::Array<double> & parameters,
		   itk::Array2D<double> & derivative ) const {}

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize(void) throw ( ExceptionObject ) {}

protected:
  DiffusionModelFittingCostFunction();
  virtual ~DiffusionModelFittingCostFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const;


private:
  DiffusionModelFittingCostFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  vnl_vector<double> _b_vals;
  vnl_vector<double> _b_signals;
  vnl_vector<double> _b_variance;
  vnl_vector<double> _b_weights;

  double ModelEvalAtbVal (double b_val, const ParametersType & params) const;





  int m_NumOfParams;
  int m_Dimension;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiffusionModelFittingCostFunction.txx"
#endif

#endif



