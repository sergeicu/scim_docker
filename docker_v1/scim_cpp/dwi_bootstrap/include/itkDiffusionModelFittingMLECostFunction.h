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
#ifndef __itkDiffusionModelFittingMLECostFunction_h
#define __itkDiffusionModelFittingMLECostFunction_h

#include "itkSingleValuedCostFunction.h"
#include "itkExceptionObject.h"

#include "vnl/vnl_vector.h"

namespace itk
{
template <typename TMeasure>
class ITK_EXPORT DiffusionModelFittingMLECostFunction : public SingleValuedCostFunction
{
public:

  /** Standard class typedefs. */
  typedef DiffusionModelFittingMLECostFunction        Self;
  typedef SingleValuedCostFunction      Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  typedef typename Superclass::MeasureType     MeasureType;
  typedef typename Superclass::DerivativeType DerivativeType;
  typedef Superclass::ParametersType 	ParametersType;

  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(DiffusionModelFittingMLECostFunction, SingleValuedCostFunction);

  itkSetMacro (NumOfParams,int);
  itkSetMacro (nCoils,int);


  void SetObservedValues (const vnl_vector<TMeasure> & b_vals,
		                  const vnl_vector<TMeasure> & b_signals,
		                  const vnl_vector<TMeasure> & b_variance,
		                  const vnl_vector<TMeasure> & b_weights);
  virtual MeasureType GetValue (const ParametersType & params) const;



   /** This method returns the derivative of the cost function corresponding
     * to the specified parameters.   */
   virtual void GetDerivative( const ParametersType & parameters,
                               DerivativeType & derivative ) const {}

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize(void) throw ( ExceptionObject ) {}
  unsigned int GetNumberOfParameters(void) const {return m_NumOfParams;}

protected:
  DiffusionModelFittingMLECostFunction();
  virtual ~DiffusionModelFittingMLECostFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const;


private:
  DiffusionModelFittingMLECostFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
public:
  vnl_vector<TMeasure> _b_vals;
  vnl_vector<TMeasure> _b_signals;
  vnl_vector<TMeasure> _b_variance;
  vnl_vector<TMeasure> _b_weights;

  int m_nCoils;

  int m_NumOfParams;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiffusionModelFittingMLECostFunction.txx"
#endif

#endif


