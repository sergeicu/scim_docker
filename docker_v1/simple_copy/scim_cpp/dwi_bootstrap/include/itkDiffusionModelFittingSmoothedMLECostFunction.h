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
#ifndef __itkDiffusionModelFittingSmoothedMLECostFunction_h
#define __itkDiffusionModelFittingSmoothedMLECostFunction_h

#include "itkSingleValuedCostFunction.h"
#include "itkExceptionObject.h"

#include "vnl/vnl_vector.h"

namespace itk
{
template <typename TMeasure>
class ITK_EXPORT DiffusionModelFittingSmoothedMLECostFunction : public SingleValuedCostFunction
{
public:

  /** Standard class typedefs. */
  typedef DiffusionModelFittingSmoothedMLECostFunction        Self;
  typedef SingleValuedCostFunction      Superclass;
  typedef SmartPointer<Self>              Pointer;
  typedef SmartPointer<const Self>        ConstPointer;

  typedef typename Superclass::MeasureType     MeasureType;
  typedef typename Superclass::DerivativeType DerivativeType;
  typedef Superclass::ParametersType 	ParametersType;

  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(DiffusionModelFittingSmoothedMLECostFunction, SingleValuedCostFunction);

  itkSetMacro (NumOfParams,int);
  itkSetMacro (nCoils,int);


  void SetObservedValues (const vnl_vector<TMeasure> & b_vals,
		                  const vnl_vector<TMeasure> & b_signals,
		                  const vnl_vector<TMeasure> & b_variance,
		                  const vnl_vector<TMeasure> & b_weights);

  void SetNeighborsCurrentParams(const vnl_vector<TMeasure> & neighbors_D,
						         const vnl_vector<TMeasure> & neighbors_Dstar,
						         const vnl_vector<TMeasure> & neighbors_f,
						         const vnl_vector<TMeasure> & neighbors_B0,
						         const vnl_vector<TMeasure> & neighbors_Weights);

  virtual MeasureType GetValue (const ParametersType & params) const;

  static const int BACKGROUND;
   static const int FOREGROUND;

   /** This method returns the derivative of the cost function corresponding
     * to the specified parameters.   */
   virtual void GetDerivative( const ParametersType & parameters,
                               DerivativeType & derivative ) const {}

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly     */
  virtual void Initialize(void) throw ( ExceptionObject ) {}
  unsigned int GetNumberOfParameters(void) const {return m_NumOfParams;}

protected:
  DiffusionModelFittingSmoothedMLECostFunction();
  virtual ~DiffusionModelFittingSmoothedMLECostFunction() {}
  void PrintSelf(std::ostream& os, Indent indent) const;


private:
  DiffusionModelFittingSmoothedMLECostFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
public:
  vnl_vector<TMeasure> _b_vals;
  vnl_vector<TMeasure> _b_signals;
  vnl_vector<TMeasure> _b_variance;
  vnl_vector<TMeasure> _b_weights;


  vnl_vector<TMeasure> _neighbors_D;
  vnl_vector<TMeasure> _neighbors_Dstar;
  vnl_vector<TMeasure> _neighbors_f;
  vnl_vector<TMeasure> _neighbors_B0;
  vnl_vector<TMeasure> _neighbors_Weights;

  int m_nCoils;

  int m_NumOfParams;

  double IVIMSquaredScales [4];

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiffusionModelFittingSmoothedMLECostFunction.txx"
#endif

#endif



