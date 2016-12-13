//=================================================================================================
/*!
//  \file blazetest/mathtest/smatdmatmult/OperationTest.h
//  \brief Header file for the sparse matrix/dense matrix multiplication operation test
//
//  Copyright (C) 2013 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. You can redistribute it and/or modify it under
//  the terms of the New (Revised) BSD License. Redistribution and use in source and binary
//  forms, with or without modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//     of conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//  3. Neither the names of the Blaze development group nor the names of its contributors
//     may be used to endorse or promote products derived from this software without specific
//     prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
*/
//=================================================================================================

#ifndef _BLAZETEST_MATHTEST_SMATDMATMULT_OPERATIONTEST_H_
#define _BLAZETEST_MATHTEST_SMATDMATMULT_OPERATIONTEST_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <blaze/math/Aliases.h>
#include <blaze/math/CompressedMatrix.h>
#include <blaze/math/constraints/Computation.h>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/constraints/MatMatMultExpr.h>
#include <blaze/math/constraints/SparseMatrix.h>
#include <blaze/math/constraints/StorageOrder.h>
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/Functors.h>
#include <blaze/math/shims/Equal.h>
#include <blaze/math/shims/IsDefault.h>
#include <blaze/math/traits/MultExprTrait.h>
#include <blaze/math/traits/MultTrait.h>
#include <blaze/math/typetraits/IsResizable.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/math/typetraits/IsSquare.h>
#include <blaze/math/typetraits/LowType.h>
#include <blaze/math/typetraits/UnderlyingBuiltin.h>
#include <blaze/math/typetraits/UnderlyingNumeric.h>
#include <blaze/math/Views.h>
#include <blaze/util/constraints/Numeric.h>
#include <blaze/util/constraints/SameType.h>
#include <blaze/util/FalseType.h>
#include <blaze/util/mpl/If.h>
#include <blaze/util/mpl/Or.h>
#include <blaze/util/TrueType.h>
#include <blaze/util/typetraits/IsSame.h>
#include <blazetest/system/LAPACK.h>
#include <blazetest/system/MathTest.h>
#include <blazetest/mathtest/Creator.h>
#include <blazetest/mathtest/IsEqual.h>
#include <blazetest/mathtest/MatchAdaptor.h>
#include <blazetest/mathtest/MatchSymmetry.h>
#include <blazetest/mathtest/RandomMaximum.h>
#include <blazetest/mathtest/RandomMinimum.h>


namespace blazetest {

namespace mathtest {

namespace smatdmatmult {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Auxiliary class template for the sparse matrix/dense matrix multiplication operation test.
//
// This class template represents one particular matrix multiplication test between two matrices
// of a particular type. The two template arguments \a MT1 and \a MT2 represent the types of the
// left-hand side and right-hand side matrix, respectively.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
class OperationTest
{
 private:
   //**Type definitions****************************************************************************
   typedef blaze::ElementType_<MT1>  ET1;  //!< Element type 1
   typedef blaze::ElementType_<MT2>  ET2;  //!< Element type 2

   typedef blaze::OppositeType_<MT1>    OMT1;   //!< Matrix type 1 with opposite storage order
   typedef blaze::OppositeType_<MT2>    OMT2;   //!< Matrix type 2 with opposite storage order
   typedef blaze::TransposeType_<MT1>   TMT1;   //!< Transpose matrix type 1
   typedef blaze::TransposeType_<MT2>   TMT2;   //!< Transpose matrix type 2
   typedef blaze::TransposeType_<OMT1>  TOMT1;  //!< Transpose matrix type 1 with opposite storage order
   typedef blaze::TransposeType_<OMT2>  TOMT2;  //!< Transpose matrix type 2 with opposite storage order

   //! Dense result type
   typedef blaze::MultTrait_<MT1,MT2>  DRE;

   typedef blaze::ElementType_<DRE>     DET;    //!< Element type of the dense result
   typedef blaze::OppositeType_<DRE>    ODRE;   //!< Dense result type with opposite storage order
   typedef blaze::TransposeType_<DRE>   TDRE;   //!< Transpose dense result type
   typedef blaze::TransposeType_<ODRE>  TODRE;  //!< Transpose dense result type with opposite storage order

   //! Sparse result type
   typedef MatchAdaptor_< DRE, blaze::CompressedMatrix<DET,false> >  SRE;

   typedef blaze::ElementType_<SRE>     SET;    //!< Element type of the sparse result
   typedef blaze::OppositeType_<SRE>    OSRE;   //!< Sparse result type with opposite storage order
   typedef blaze::TransposeType_<SRE>   TSRE;   //!< Transpose sparse result type
   typedef blaze::TransposeType_<OSRE>  TOSRE;  //!< Transpose sparse result type with opposite storage order

   typedef blaze::DynamicMatrix<ET1,false>  RT1;  //!< Reference type 1
   typedef blaze::DynamicMatrix<ET2,false>  RT2;  //!< Reference type 2

   //! Reference result type
   typedef MatchSymmetry_< DRE, blaze::MultTrait_<RT1,RT2> >  RRE;

   //! Type of the matrix/matrix multiplication expression
   typedef blaze::MultExprTrait_<MT1,MT2>  MatMatMultExprType;

   //! Type of the matrix/transpose matrix multiplication expression
   typedef blaze::MultExprTrait_<MT1,OMT2>  MatTMatMultExprType;

   //! Type of the transpose matrix/matrix multiplication expression
   typedef blaze::MultExprTrait_<OMT1,MT2>  TMatMatMultExprType;

   //! Type of the transpose matrix/transpose matrix multiplication expression
   typedef blaze::MultExprTrait_<OMT1,OMT2>  TMatTMatMultExprType;
   //**********************************************************************************************

 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit OperationTest( const Creator<MT1>& creator1, const Creator<MT2>& creator2 );
   // No explicitly declared copy constructor.
   //@}
   //**********************************************************************************************

   //**Destructor**********************************************************************************
   // No explicitly declared destructor.
   //**********************************************************************************************

 private:
   //**Test functions******************************************************************************
   /*!\name Test functions */
   //@{
                          void testInitialStatus     ();
                          void testAssignment        ();
                          void testElementAccess     ();
                          void testBasicOperation    ();
                          void testNegatedOperation  ();
   template< typename T > void testScaledOperation   ( T scalar );
                          void testTransOperation    ();
                          void testCTransOperation   ();
                          void testAbsOperation      ();
                          void testConjOperation     ();
                          void testRealOperation     ();
                          void testImagOperation     ();
                          void testInvOperation      ();
                          void testEvalOperation     ();
                          void testSerialOperation   ();
                          void testDeclSymOperation  ( blaze::TrueType  );
                          void testDeclSymOperation  ( blaze::FalseType );
                          void testDeclHermOperation ( blaze::TrueType  );
                          void testDeclHermOperation ( blaze::FalseType );
                          void testDeclLowOperation  ( blaze::TrueType  );
                          void testDeclLowOperation  ( blaze::FalseType );
                          void testDeclUppOperation  ( blaze::TrueType  );
                          void testDeclUppOperation  ( blaze::FalseType );
                          void testDeclDiagOperation ( blaze::TrueType  );
                          void testDeclDiagOperation ( blaze::FalseType );
                          void testSubmatrixOperation();
                          void testRowOperation      ();
                          void testColumnOperation   ();

   template< typename OP > void testCustomOperation( OP op, const std::string& name );
   //@}
   //**********************************************************************************************

   //**Error detection functions*******************************************************************
   /*!\name Error detection functions */
   //@{
   template< typename LT, typename RT > void checkResults();
   template< typename LT, typename RT > void checkTransposeResults();
   //@}
   //**********************************************************************************************

   //**Utility functions***************************************************************************
   /*!\name Utility functions */
   //@{
   void initResults();
   void initTransposeResults();
   template< typename LT, typename RT > void convertException( const std::exception& ex );
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   MT1   lhs_;     //!< The left-hand side sparse matrix.
   MT2   rhs_;     //!< The right-hand side dense matrix.
   OMT1  olhs_;    //!< The left-hand side sparse matrix with opposite storage order.
   OMT2  orhs_;    //!< The right-hand side dense matrix with opposite storage order.
   DRE   dres_;    //!< The dense result matrix.
   SRE   sres_;    //!< The sparse result matrix.
   ODRE  odres_;   //!< The dense result matrix with opposite storage order.
   OSRE  osres_;   //!< The sparse result matrix with opposite storage order.
   TDRE  tdres_;   //!< The transpose dense result matrix.
   TSRE  tsres_;   //!< The transpose sparse result matrix.
   TODRE todres_;  //!< The transpose dense result matrix with opposite storage order.
   TOSRE tosres_;  //!< The transpose sparse result matrix with opposite storage order.
   RT1   reflhs_;  //!< The reference left-hand side matrix.
   RT2   refrhs_;  //!< The reference right-hand side matrix.
   RRE   refres_;  //!< The reference result.

   std::string test_;   //!< Label of the currently performed test.
   std::string error_;  //!< Description of the current error type.
   //@}
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( MT1   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( MT2   );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( OMT1  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( OMT2  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TMT1  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TMT2  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TOMT1 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TOMT2 );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( RT1   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( RT2   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( RRE   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( DRE   );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( SRE   );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( ODRE  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( OSRE  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TDRE  );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TSRE  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE ( TODRE );
   BLAZE_CONSTRAINT_MUST_BE_SPARSE_MATRIX_TYPE( TOSRE );

   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( MT1   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( MT2   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OMT1  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OMT2  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TMT1  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TMT2  );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TOMT1 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TOMT2 );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( RT1   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( RT2   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( DRE   );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( SRE   );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( ODRE  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( OSRE  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TDRE  );
   BLAZE_CONSTRAINT_MUST_BE_COLUMN_MAJOR_MATRIX_TYPE( TSRE  );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TODRE );
   BLAZE_CONSTRAINT_MUST_BE_ROW_MAJOR_MATRIX_TYPE   ( TOSRE );

   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET1, blaze::ElementType_<OMT1>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET2, blaze::ElementType_<OMT2>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET1, blaze::ElementType_<TMT1>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET2, blaze::ElementType_<TMT2>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET1, blaze::ElementType_<TOMT1>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( ET2, blaze::ElementType_<TOMT2>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( DET, blaze::ElementType_<DRE>    );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( DET, blaze::ElementType_<ODRE>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( DET, blaze::ElementType_<TDRE>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( DET, blaze::ElementType_<TODRE>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( DET, blaze::ElementType_<SRE>    );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( SET, blaze::ElementType_<SRE>    );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( SET, blaze::ElementType_<OSRE>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( SET, blaze::ElementType_<TSRE>   );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( SET, blaze::ElementType_<TOSRE>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( SET, blaze::ElementType_<DRE>    );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT1, blaze::OppositeType_<OMT1>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT2, blaze::OppositeType_<OMT2>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT1, blaze::TransposeType_<TMT1> );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT2, blaze::TransposeType_<TMT2> );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( DRE, blaze::OppositeType_<ODRE>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( DRE, blaze::TransposeType_<TDRE> );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( SRE, blaze::OppositeType_<OSRE>  );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( SRE, blaze::TransposeType_<TSRE> );

   BLAZE_CONSTRAINT_MUST_BE_MATMATMULTEXPR_TYPE( MatMatMultExprType   );
   BLAZE_CONSTRAINT_MUST_BE_MATMATMULTEXPR_TYPE( MatTMatMultExprType  );
   BLAZE_CONSTRAINT_MUST_BE_MATMATMULTEXPR_TYPE( TMatMatMultExprType  );
   BLAZE_CONSTRAINT_MUST_BE_MATMATMULTEXPR_TYPE( TMatTMatMultExprType );

   BLAZE_CONSTRAINT_MUST_BE_COMPUTATION_TYPE( MatMatMultExprType   );
   BLAZE_CONSTRAINT_MUST_BE_COMPUTATION_TYPE( MatTMatMultExprType  );
   BLAZE_CONSTRAINT_MUST_BE_COMPUTATION_TYPE( TMatMatMultExprType  );
   BLAZE_CONSTRAINT_MUST_BE_COMPUTATION_TYPE( TMatTMatMultExprType );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  CONSTRUCTORS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Constructor for the sparse matrix/dense matrix multiplication operation test.
//
// \param creator1 The creator for the left-hand side sparse matrix of the matrix multiplication.
// \param creator2 The creator for the right-hand side dense matrix of the matrix multiplication.
// \exception std::runtime_error Operation error detected.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
OperationTest<MT1,MT2>::OperationTest( const Creator<MT1>& creator1, const Creator<MT2>& creator2 )
   : lhs_( creator1() )  // The left-hand side sparse matrix
   , rhs_( creator2() )  // The right-hand side dense matrix
   , olhs_( lhs_ )       // The left-hand side sparse matrix with opposite storage order
   , orhs_( rhs_ )       // The right-hand side dense matrix with opposite storage order
   , dres_()             // The dense result matrix
   , sres_()             // The sparse result matrix
   , odres_()            // The dense result matrix with opposite storage order
   , osres_()            // The sparse result matrix with opposite storage order
   , tdres_()            // The transpose dense result matrix
   , tsres_()            // The transpose sparse result matrix
   , todres_()           // The transpose dense result matrix with opposite storage order
   , tosres_()           // The transpose sparse result matrix with opposite storage order
   , reflhs_( lhs_ )     // The reference left-hand side matrix
   , refrhs_( rhs_ )     // The reference right-hand side matrix
   , refres_()           // The reference result
   , test_()             // Label of the currently performed test
   , error_()            // Description of the current error type
{
   typedef blaze::UnderlyingNumeric_<DET>  Scalar;

   testInitialStatus();
   testAssignment();
   testElementAccess();
   testBasicOperation();
   testNegatedOperation();
   testScaledOperation( 2 );
   testScaledOperation( 2UL );
   testScaledOperation( 2.0F );
   testScaledOperation( 2.0 );
   testScaledOperation( Scalar( 2 ) );
   testTransOperation();
   testCTransOperation();
   testAbsOperation();
   testConjOperation();
   testRealOperation();
   testImagOperation();
   testInvOperation();
   testEvalOperation();
   testSerialOperation();
   testDeclSymOperation( blaze::IsSame< blaze::LowType_<ET1,ET2>, ET1 >() );
   testDeclHermOperation( blaze::IsSame< blaze::LowType_<ET1,ET2>, ET1 >() );
   testDeclLowOperation( blaze::Or< blaze::IsSquare<DRE>, blaze::IsResizable<DRE> >() );
   testDeclUppOperation( blaze::Or< blaze::IsSquare<DRE>, blaze::IsResizable<DRE> >() );
   testDeclDiagOperation( blaze::Or< blaze::IsSquare<DRE>, blaze::IsResizable<DRE> >() );
   testSubmatrixOperation();
   testRowOperation();
   testColumnOperation();
}
//*************************************************************************************************




//=================================================================================================
//
//  TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Tests on the initial status of the matrices.
//
// \return void
// \exception std::runtime_error Initialization error detected.
//
// This function runs tests on the initial status of the matrices. In case any initialization
// error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testInitialStatus()
{
   //=====================================================================================
   // Performing initial tests with the row-major types
   //=====================================================================================

   // Checking the number of rows of the left-hand side operand
   if( lhs_.rows() != reflhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side row-major sparse operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Detected number of rows = " << lhs_.rows() << "\n"
          << "   Expected number of rows = " << reflhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the left-hand side operand
   if( lhs_.columns() != reflhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side row-major sparse operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Detected number of columns = " << lhs_.columns() << "\n"
          << "   Expected number of columns = " << reflhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of rows of the right-hand side operand
   if( rhs_.rows() != refrhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side row-major dense operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Detected number of rows = " << rhs_.rows() << "\n"
          << "   Expected number of rows = " << refrhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the right-hand side operand
   if( rhs_.columns() != refrhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side row-major dense operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Detected number of columns = " << rhs_.columns() << "\n"
          << "   Expected number of columns = " << refrhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the left-hand side operand
   if( !isEqual( lhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of left-hand side row-major sparse operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Current initialization:\n" << lhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the right-hand side operand
   if( !isEqual( rhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of right-hand side row-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Current initialization:\n" << rhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }


   //=====================================================================================
   // Performing initial tests with the column-major types
   //=====================================================================================

   // Checking the number of rows of the left-hand side operand
   if( olhs_.rows() != reflhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side column-major dense operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Detected number of rows = " << olhs_.rows() << "\n"
          << "   Expected number of rows = " << reflhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the left-hand side operand
   if( olhs_.columns() != reflhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of left-hand side column-major dense operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Detected number of columns = " << olhs_.columns() << "\n"
          << "   Expected number of columns = " << reflhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of rows of the right-hand side operand
   if( orhs_.rows() != refrhs_.rows() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side column-major dense operand\n"
          << " Error: Invalid number of rows\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Detected number of rows = " << orhs_.rows() << "\n"
          << "   Expected number of rows = " << refrhs_.rows() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the number of columns of the right-hand side operand
   if( orhs_.columns() != refrhs_.columns() ) {
      std::ostringstream oss;
      oss << " Test: Initial size comparison of right-hand side column-major dense operand\n"
          << " Error: Invalid number of columns\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Detected number of columns = " << orhs_.columns() << "\n"
          << "   Expected number of columns = " << refrhs_.columns() << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the left-hand side operand
   if( !isEqual( olhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of left-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Current initialization:\n" << olhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   // Checking the initialization of the right-hand side operand
   if( !isEqual( orhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Initial test of initialization of right-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Current initialization:\n" << orhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the matrix assignment.
//
// \return void
// \exception std::runtime_error Assignment error detected.
//
// This function tests the matrix assignment. In case any error is detected, a
// \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testAssignment()
{
   //=====================================================================================
   // // Performing an assignment with the row-major types
   //=====================================================================================

   try {
      lhs_ = reflhs_;
      rhs_ = refrhs_;
   }
   catch( std::exception& ex ) {
      std::ostringstream oss;
      oss << " Test: Assignment with the row-major types\n"
          << " Error: Failed assignment\n"
          << " Details:\n"
          << "   Left-hand side row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Right-hand side row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( lhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of left-hand side row-major sparse operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Current initialization:\n" << lhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( rhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of right-hand side row-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n"
          << "   Current initialization:\n" << rhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }


   //=====================================================================================
   // Performing an assignment with the column-major types
   //=====================================================================================

   try {
      olhs_ = reflhs_;
      orhs_ = refrhs_;
   }
   catch( std::exception& ex ) {
      std::ostringstream oss;
      oss << " Test: Assignment with the column-major types\n"
          << " Error: Failed assignment\n"
          << " Details:\n"
          << "   Left-hand side column-major dense matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Right-hand side column-major dense matrix type:\n"
          << "     "  << typeid( OMT2 ).name() << "\n"
          << "   Error message: " << ex.what() << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( olhs_, reflhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of left-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Current initialization:\n" << olhs_ << "\n"
          << "   Expected initialization:\n" << reflhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( orhs_, refrhs_ ) ) {
      std::ostringstream oss;
      oss << " Test: Checking the assignment result of right-hand side column-major dense operand\n"
          << " Error: Invalid matrix initialization\n"
          << " Details:\n"
          << "   Column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n"
          << "   Current initialization:\n" << orhs_ << "\n"
          << "   Expected initialization:\n" << refrhs_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the matrix element access.
//
// \return void
// \exception std::runtime_error Element access error detected.
//
// This function tests the element access via the subscript operator. In case any
// error is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testElementAccess()
{
   using blaze::equal;


   //=====================================================================================
   // Testing the element access with two row-major matrices
   //=====================================================================================

   if( lhs_.rows() > 0UL && rhs_.columns() > 0UL )
   {
      const size_t m( lhs_.rows()    - 1UL );
      const size_t n( rhs_.columns() - 1UL );

      if( !equal( ( lhs_ * rhs_ )(m,n), ( reflhs_ * refrhs_ )(m,n) ) ||
          !equal( ( lhs_ * rhs_ ).at(m,n), ( reflhs_ * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( lhs_ * eval( rhs_ ) )(m,n), ( reflhs_ * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( lhs_ * eval( rhs_ ) ).at(m,n), ( reflhs_ * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * rhs_ )(m,n), ( eval( reflhs_ ) * refrhs_ )(m,n) ) ||
          !equal( ( eval( lhs_ ) * rhs_ ).at(m,n), ( eval( reflhs_ ) * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * eval( rhs_ ) )(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( eval( lhs_ ) * eval( rhs_ ) ).at(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }

   try {
      ( lhs_ * rhs_ ).at( 0UL, rhs_.columns() );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Right-hand side row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}

   try {
      ( lhs_ * rhs_ ).at( lhs_.rows(), 0UL );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Right-hand side row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}


   //=====================================================================================
   // Testing the element access with a row-major matrix and a column-major matrix
   //=====================================================================================

   if( lhs_.rows() > 0UL && orhs_.columns() > 0UL )
   {
      const size_t m( lhs_.rows()     - 1UL );
      const size_t n( orhs_.columns() - 1UL );

      if( !equal( ( lhs_ * orhs_ )(m,n), ( reflhs_ * refrhs_ )(m,n) ) ||
          !equal( ( lhs_ * orhs_ ).at(m,n), ( reflhs_ * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( lhs_ * eval( orhs_ ) )(m,n), ( reflhs_ * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( lhs_ * eval( orhs_ ) ).at(m,n), ( reflhs_ * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * orhs_ )(m,n), ( eval( reflhs_ ) * refrhs_ )(m,n) ) ||
          !equal( ( eval( lhs_ ) * orhs_ ).at(m,n), ( eval( reflhs_ ) * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( lhs_ ) * eval( orhs_ ) )(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( eval( lhs_ ) * eval( orhs_ ) ).at(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side row-major sparse matrix type:\n"
             << "     " << typeid( MT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }

   try {
      ( lhs_ * orhs_ ).at( 0UL, orhs_.columns() );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Right-hand side column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}

   try {
      ( lhs_ * orhs_ ).at( lhs_.rows(), 0UL );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side row-major sparse matrix type:\n"
          << "     " << typeid( MT1 ).name() << "\n"
          << "   Right-hand side column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}


   //=====================================================================================
   // Testing the element access with a column-major matrix and a row-major matrix
   //=====================================================================================

   if( olhs_.rows() > 0UL && rhs_.columns() > 0UL )
   {
      const size_t m( olhs_.rows()   - 1UL );
      const size_t n( rhs_.columns() - 1UL );

      if( !equal( ( olhs_ * rhs_ )(m,n), ( reflhs_ * refrhs_ )(m,n) ) ||
          !equal( ( olhs_ * rhs_ ).at(m,n), ( reflhs_ * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( olhs_ * eval( rhs_ ) )(m,n), ( reflhs_ * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( olhs_ * eval( rhs_ ) ).at(m,n), ( reflhs_ * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * rhs_ )(m,n), ( eval( reflhs_ ) * refrhs_ )(m,n) ) ||
          !equal( ( eval( olhs_ ) * rhs_ ).at(m,n), ( eval( reflhs_ ) * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * eval( rhs_ ) )(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( eval( olhs_ ) * eval( rhs_ ) ).at(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side row-major dense matrix type:\n"
             << "     " << typeid( MT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }

   try {
      ( olhs_ * rhs_ ).at( 0UL, rhs_.columns() );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Right-hand side row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}

   try {
      ( olhs_ * rhs_ ).at( olhs_.rows(), 0UL );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Right-hand side row-major dense matrix type:\n"
          << "     " << typeid( MT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}


   //=====================================================================================
   // Testing the element access with two column-major matrices
   //=====================================================================================

   if( olhs_.rows() > 0UL && orhs_.columns() > 0UL )
   {
      const size_t m( olhs_.rows()    - 1UL );
      const size_t n( orhs_.columns() - 1UL );

      if( !equal( ( olhs_ * orhs_ )(m,n), ( reflhs_ * refrhs_ )(m,n) ) ||
          !equal( ( olhs_ * orhs_ ).at(m,n), ( reflhs_ * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major sparse matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( olhs_ * eval( orhs_ ) )(m,n), ( reflhs_ * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( olhs_ * eval( orhs_ ) ).at(m,n), ( reflhs_ * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of right evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major sparse matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * orhs_ )(m,n), ( eval( reflhs_ ) * refrhs_ )(m,n) ) ||
          !equal( ( eval( olhs_ ) * orhs_ ).at(m,n), ( eval( reflhs_ ) * refrhs_ ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of left evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major sparse matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }

      if( !equal( ( eval( olhs_ ) * eval( orhs_ ) )(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) )(m,n) ) ||
          !equal( ( eval( olhs_ ) * eval( orhs_ ) ).at(m,n), ( eval( reflhs_ ) * eval( refrhs_ ) ).at(m,n) ) ) {
         std::ostringstream oss;
         oss << " Test : Element access of fully evaluated multiplication expression\n"
             << " Error: Unequal resulting elements at element (" << m << "," << n << ") detected\n"
             << " Details:\n"
             << "   Left-hand side column-major sparse matrix type:\n"
             << "     " << typeid( OMT1 ).name() << "\n"
             << "   Right-hand side column-major dense matrix type:\n"
             << "     " << typeid( OMT2 ).name() << "\n";
         throw std::runtime_error( oss.str() );
      }
   }

   try {
      ( olhs_ * orhs_ ).at( 0UL, orhs_.columns() );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Right-hand side column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}

   try {
      ( olhs_ * orhs_ ).at( olhs_.rows(), 0UL );

      std::ostringstream oss;
      oss << " Test : Checked element access of multiplication expression\n"
          << " Error: Out-of-bound access succeeded\n"
          << " Details:\n"
          << "   Left-hand side column-major sparse matrix type:\n"
          << "     " << typeid( OMT1 ).name() << "\n"
          << "   Right-hand side column-major dense matrix type:\n"
          << "     " << typeid( OMT2 ).name() << "\n";
      throw std::runtime_error( oss.str() );
   }
   catch( std::out_of_range& ) {}
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the plain sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the plain matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testBasicOperation()
{
#if BLAZETEST_MATHTEST_TEST_BASIC_OPERATION
   if( BLAZETEST_MATHTEST_TEST_BASIC_OPERATION > 1 )
   {
      //=====================================================================================
      // Multiplication
      //=====================================================================================

      // Multiplication with the given matrices
      {
         test_  = "Multiplication with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = lhs_ * rhs_;
            odres_  = lhs_ * rhs_;
            sres_   = lhs_ * rhs_;
            osres_  = lhs_ * rhs_;
            refres_ = reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = lhs_ * orhs_;
            odres_  = lhs_ * orhs_;
            sres_   = lhs_ * orhs_;
            osres_  = lhs_ * orhs_;
            refres_ = reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = olhs_ * rhs_;
            odres_  = olhs_ * rhs_;
            sres_   = olhs_ * rhs_;
            osres_  = olhs_ * rhs_;
            refres_ = reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = olhs_ * orhs_;
            odres_  = olhs_ * orhs_;
            sres_   = olhs_ * orhs_;
            osres_  = olhs_ * orhs_;
            refres_ = reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Multiplication with evaluated matrices
      {
         test_  = "Multiplication with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = eval( lhs_ ) * eval( rhs_ );
            odres_  = eval( lhs_ ) * eval( rhs_ );
            sres_   = eval( lhs_ ) * eval( rhs_ );
            osres_  = eval( lhs_ ) * eval( rhs_ );
            refres_ = eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = eval( lhs_ ) * eval( orhs_ );
            odres_  = eval( lhs_ ) * eval( orhs_ );
            sres_   = eval( lhs_ ) * eval( orhs_ );
            osres_  = eval( lhs_ ) * eval( orhs_ );
            refres_ = eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = eval( olhs_ ) * eval( rhs_ );
            odres_  = eval( olhs_ ) * eval( rhs_ );
            sres_   = eval( olhs_ ) * eval( rhs_ );
            osres_  = eval( olhs_ ) * eval( rhs_ );
            refres_ = eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = eval( olhs_ ) * eval( orhs_ );
            odres_  = eval( olhs_ ) * eval( orhs_ );
            sres_   = eval( olhs_ ) * eval( orhs_ );
            osres_  = eval( olhs_ ) * eval( orhs_ );
            refres_ = eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Multiplication with addition assignment
      //=====================================================================================

      // Multiplication with addition assignment with the given matrices
      {
         test_  = "Multiplication with addition assignment with the given matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += lhs_ * rhs_;
            odres_  += lhs_ * rhs_;
            sres_   += lhs_ * rhs_;
            osres_  += lhs_ * rhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += lhs_ * orhs_;
            odres_  += lhs_ * orhs_;
            sres_   += lhs_ * orhs_;
            osres_  += lhs_ * orhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += olhs_ * rhs_;
            odres_  += olhs_ * rhs_;
            sres_   += olhs_ * rhs_;
            osres_  += olhs_ * rhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += olhs_ * orhs_;
            odres_  += olhs_ * orhs_;
            sres_   += olhs_ * orhs_;
            osres_  += olhs_ * orhs_;
            refres_ += reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Multiplication with addition assignment with evaluated matrices
      {
         test_  = "Multiplication with addition assignment with evaluated matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += eval( lhs_ ) * eval( rhs_ );
            odres_  += eval( lhs_ ) * eval( rhs_ );
            sres_   += eval( lhs_ ) * eval( rhs_ );
            osres_  += eval( lhs_ ) * eval( rhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += eval( lhs_ ) * eval( orhs_ );
            odres_  += eval( lhs_ ) * eval( orhs_ );
            sres_   += eval( lhs_ ) * eval( orhs_ );
            osres_  += eval( lhs_ ) * eval( orhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += eval( olhs_ ) * eval( rhs_ );
            odres_  += eval( olhs_ ) * eval( rhs_ );
            sres_   += eval( olhs_ ) * eval( rhs_ );
            osres_  += eval( olhs_ ) * eval( rhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += eval( olhs_ ) * eval( orhs_ );
            odres_  += eval( olhs_ ) * eval( orhs_ );
            sres_   += eval( olhs_ ) * eval( orhs_ );
            osres_  += eval( olhs_ ) * eval( orhs_ );
            refres_ += eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Multiplication with subtraction assignment with the given matrices
      //=====================================================================================

      // Multiplication with subtraction assignment with the given matrices
      {
         test_  = "Multiplication with subtraction assignment with the given matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= lhs_ * rhs_;
            odres_  -= lhs_ * rhs_;
            sres_   -= lhs_ * rhs_;
            osres_  -= lhs_ * rhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= lhs_ * orhs_;
            odres_  -= lhs_ * orhs_;
            sres_   -= lhs_ * orhs_;
            osres_  -= lhs_ * orhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= olhs_ * rhs_;
            odres_  -= olhs_ * rhs_;
            sres_   -= olhs_ * rhs_;
            osres_  -= olhs_ * rhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= olhs_ * orhs_;
            odres_  -= olhs_ * orhs_;
            sres_   -= olhs_ * orhs_;
            osres_  -= olhs_ * orhs_;
            refres_ -= reflhs_ * refrhs_;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= eval( lhs_ ) * eval( rhs_ );
            odres_  -= eval( lhs_ ) * eval( rhs_ );
            sres_   -= eval( lhs_ ) * eval( rhs_ );
            osres_  -= eval( lhs_ ) * eval( rhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= eval( lhs_ ) * eval( orhs_ );
            odres_  -= eval( lhs_ ) * eval( orhs_ );
            sres_   -= eval( lhs_ ) * eval( orhs_ );
            osres_  -= eval( lhs_ ) * eval( orhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= eval( olhs_ ) * eval( rhs_ );
            odres_  -= eval( olhs_ ) * eval( rhs_ );
            sres_   -= eval( olhs_ ) * eval( rhs_ );
            osres_  -= eval( olhs_ ) * eval( rhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= eval( olhs_ ) * eval( orhs_ );
            odres_  -= eval( olhs_ ) * eval( orhs_ );
            sres_   -= eval( olhs_ ) * eval( orhs_ );
            osres_  -= eval( olhs_ ) * eval( orhs_ );
            refres_ -= eval( reflhs_ ) * eval( refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the negated sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the negated matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testNegatedOperation()
{
#if BLAZETEST_MATHTEST_TEST_NEGATED_OPERATION
   if( BLAZETEST_MATHTEST_TEST_NEGATED_OPERATION > 1 )
   {
      //=====================================================================================
      // Negated multiplication
      //=====================================================================================

      // Negated multiplication with the given matrices
      {
         test_  = "Negated multiplication with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = -( lhs_ * rhs_ );
            odres_  = -( lhs_ * rhs_ );
            sres_   = -( lhs_ * rhs_ );
            osres_  = -( lhs_ * rhs_ );
            refres_ = -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = -( lhs_ * orhs_ );
            odres_  = -( lhs_ * orhs_ );
            sres_   = -( lhs_ * orhs_ );
            osres_  = -( lhs_ * orhs_ );
            refres_ = -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = -( olhs_ * rhs_ );
            odres_  = -( olhs_ * rhs_ );
            sres_   = -( olhs_ * rhs_ );
            osres_  = -( olhs_ * rhs_ );
            refres_ = -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = -( olhs_ * orhs_ );
            odres_  = -( olhs_ * orhs_ );
            sres_   = -( olhs_ * orhs_ );
            osres_  = -( olhs_ * orhs_ );
            refres_ = -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Negated multiplication with evaluated matrices
      {
         test_  = "Negated multiplication with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = -( eval( lhs_ ) * eval( rhs_ ) );
            odres_  = -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   = -( eval( lhs_ ) * eval( rhs_ ) );
            osres_  = -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ = -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = -( eval( lhs_ ) * eval( orhs_ ) );
            odres_  = -( eval( lhs_ ) * eval( orhs_ ) );
            sres_   = -( eval( lhs_ ) * eval( orhs_ ) );
            osres_  = -( eval( lhs_ ) * eval( orhs_ ) );
            refres_ = -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = -( eval( olhs_ ) * eval( rhs_ ) );
            odres_  = -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   = -( eval( olhs_ ) * eval( rhs_ ) );
            osres_  = -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ = -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = -( eval( olhs_ ) * eval( orhs_ ) );
            odres_  = -( eval( olhs_ ) * eval( orhs_ ) );
            sres_   = -( eval( olhs_ ) * eval( orhs_ ) );
            osres_  = -( eval( olhs_ ) * eval( orhs_ ) );
            refres_ = -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Negated multiplication with addition assignment
      //=====================================================================================

      // Negated multiplication with addition assignment with the given matrices
      {
         test_  = "Negated multiplication with addition assignment with the given matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += -( lhs_ * rhs_ );
            odres_  += -( lhs_ * rhs_ );
            sres_   += -( lhs_ * rhs_ );
            osres_  += -( lhs_ * rhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += -( lhs_ * orhs_ );
            odres_  += -( lhs_ * orhs_ );
            sres_   += -( lhs_ * orhs_ );
            osres_  += -( lhs_ * orhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += -( olhs_ * rhs_ );
            odres_  += -( olhs_ * rhs_ );
            sres_   += -( olhs_ * rhs_ );
            osres_  += -( olhs_ * rhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += -( olhs_ * orhs_ );
            odres_  += -( olhs_ * orhs_ );
            sres_   += -( olhs_ * orhs_ );
            osres_  += -( olhs_ * orhs_ );
            refres_ += -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Negated multiplication with addition assignment with the given matrices
      {
         test_  = "Negated multiplication with addition assignment with evaluated matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += -( eval( lhs_ ) * eval( rhs_ ) );
            odres_  += -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += -( eval( lhs_ ) * eval( rhs_ ) );
            osres_  += -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += -( eval( lhs_ ) * eval( orhs_ ) );
            odres_  += -( eval( lhs_ ) * eval( orhs_ ) );
            sres_   += -( eval( lhs_ ) * eval( orhs_ ) );
            osres_  += -( eval( lhs_ ) * eval( orhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += -( eval( olhs_ ) * eval( rhs_ ) );
            odres_  += -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += -( eval( olhs_ ) * eval( rhs_ ) );
            osres_  += -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += -( eval( olhs_ ) * eval( orhs_ ) );
            odres_  += -( eval( olhs_ ) * eval( orhs_ ) );
            sres_   += -( eval( olhs_ ) * eval( orhs_ ) );
            osres_  += -( eval( olhs_ ) * eval( orhs_ ) );
            refres_ += -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Negated multiplication with subtraction assignment
      //=====================================================================================

      // Negated multiplication with subtraction assignment with the given matrices
      {
         test_  = "Negated multiplication with subtraction assignment with the given matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= -( lhs_ * rhs_ );
            odres_  -= -( lhs_ * rhs_ );
            sres_   -= -( lhs_ * rhs_ );
            osres_  -= -( lhs_ * rhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= -( lhs_ * orhs_ );
            odres_  -= -( lhs_ * orhs_ );
            sres_   -= -( lhs_ * orhs_ );
            osres_  -= -( lhs_ * orhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= -( olhs_ * rhs_ );
            odres_  -= -( olhs_ * rhs_ );
            sres_   -= -( olhs_ * rhs_ );
            osres_  -= -( olhs_ * rhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= -( olhs_ * orhs_ );
            odres_  -= -( olhs_ * orhs_ );
            sres_   -= -( olhs_ * orhs_ );
            osres_  -= -( olhs_ * orhs_ );
            refres_ -= -( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Negated multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Negated multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= -( eval( lhs_ ) * eval( rhs_ ) );
            odres_  -= -( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= -( eval( lhs_ ) * eval( rhs_ ) );
            osres_  -= -( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= -( eval( lhs_ ) * eval( orhs_ ) );
            odres_  -= -( eval( lhs_ ) * eval( orhs_ ) );
            sres_   -= -( eval( lhs_ ) * eval( orhs_ ) );
            osres_  -= -( eval( lhs_ ) * eval( orhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= -( eval( olhs_ ) * eval( rhs_ ) );
            odres_  -= -( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= -( eval( olhs_ ) * eval( rhs_ ) );
            osres_  -= -( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= -( eval( olhs_ ) * eval( orhs_ ) );
            odres_  -= -( eval( olhs_ ) * eval( orhs_ ) );
            sres_   -= -( eval( olhs_ ) * eval( orhs_ ) );
            osres_  -= -( eval( olhs_ ) * eval( orhs_ ) );
            refres_ -= -( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the scaled sparse matrix/dense matrix multiplication.
//
// \param scalar The scalar value.
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the scaled matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename T >    // Type of the scalar
void OperationTest<MT1,MT2>::testScaledOperation( T scalar )
{
   BLAZE_CONSTRAINT_MUST_BE_NUMERIC_TYPE( T );

   if( scalar == T(0) )
      throw std::invalid_argument( "Invalid scalar parameter" );


#if BLAZETEST_MATHTEST_TEST_SCALED_OPERATION
   if( BLAZETEST_MATHTEST_TEST_SCALED_OPERATION > 1 )
   {
      //=====================================================================================
      // Self-scaling (M*=s)
      //=====================================================================================

      // Self-scaling (M*=s)
      {
         test_ = "Self-scaling (M*=s)";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = dres_;
            sres_   = dres_;
            osres_  = dres_;
            refres_ = dres_;

            dres_   *= scalar;
            odres_  *= scalar;
            sres_   *= scalar;
            osres_  *= scalar;
            refres_ *= scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed self-scaling operation\n"
                << " Details:\n"
                << "   Scalar = " << scalar << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();
      }


      //=====================================================================================
      // Self-scaling (M=M*s)
      //=====================================================================================

      // Self-scaling (M=M*s)
      {
         test_ = "Self-scaling (M=M*s)";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = dres_;
            sres_   = dres_;
            osres_  = dres_;
            refres_ = dres_;

            dres_   = dres_   * scalar;
            odres_  = odres_  * scalar;
            sres_   = sres_   * scalar;
            osres_  = osres_  * scalar;
            refres_ = refres_ * scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed self-scaling operation\n"
                << " Details:\n"
                << "   Scalar = " << scalar << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();
      }


      //=====================================================================================
      // Self-scaling (M=s*M)
      //=====================================================================================

      // Self-scaling (M=s*M)
      {
         test_ = "Self-scaling (M=s*M)";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = dres_;
            sres_   = dres_;
            osres_  = dres_;
            refres_ = dres_;

            dres_   = scalar * dres_;
            odres_  = scalar * odres_;
            sres_   = scalar * sres_;
            osres_  = scalar * osres_;
            refres_ = scalar * refres_;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed self-scaling operation\n"
                << " Details:\n"
                << "   Scalar = " << scalar << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();
      }


      //=====================================================================================
      // Self-scaling (M/=s)
      //=====================================================================================

      // Self-scaling (M/=s)
      {
         test_ = "Self-scaling (M/=s)";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = dres_;
            sres_   = dres_;
            osres_  = dres_;
            refres_ = dres_;

            dres_   /= scalar;
            odres_  /= scalar;
            sres_   /= scalar;
            osres_  /= scalar;
            refres_ /= scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed self-scaling operation\n"
                << " Details:\n"
                << "   Scalar = " << scalar << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();
      }


      //=====================================================================================
      // Self-scaling (M=M/s)
      //=====================================================================================

      // Self-scaling (M=M/s)
      {
         test_ = "Self-scaling (M=M/s)";

         try {
            dres_   = lhs_ * rhs_;
            odres_  = dres_;
            sres_   = dres_;
            osres_  = dres_;
            refres_ = dres_;

            dres_   = dres_   / scalar;
            odres_  = odres_  / scalar;
            sres_   = sres_   / scalar;
            osres_  = osres_  / scalar;
            refres_ = refres_ / scalar;
         }
         catch( std::exception& ex ) {
            std::ostringstream oss;
            oss << " Test : " << test_ << "\n"
                << " Error: Failed self-scaling operation\n"
                << " Details:\n"
                << "   Scalar = " << scalar << "\n"
                << "   Error message: " << ex.what() << "\n";
            throw std::runtime_error( oss.str() );
         }

         checkResults<MT1,MT2>();
      }


      //=====================================================================================
      // Scaled multiplication (s*OP)
      //=====================================================================================

      // Scaled multiplication with the given matrices
      {
         test_  = "Scaled multiplication with the given matrices (s*OP)";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = scalar * ( lhs_ * rhs_ );
            odres_  = scalar * ( lhs_ * rhs_ );
            sres_   = scalar * ( lhs_ * rhs_ );
            osres_  = scalar * ( lhs_ * rhs_ );
            refres_ = scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = scalar * ( lhs_ * orhs_ );
            odres_  = scalar * ( lhs_ * orhs_ );
            sres_   = scalar * ( lhs_ * orhs_ );
            osres_  = scalar * ( lhs_ * orhs_ );
            refres_ = scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = scalar * ( olhs_ * rhs_ );
            odres_  = scalar * ( olhs_ * rhs_ );
            sres_   = scalar * ( olhs_ * rhs_ );
            osres_  = scalar * ( olhs_ * rhs_ );
            refres_ = scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = scalar * ( olhs_ * orhs_ );
            odres_  = scalar * ( olhs_ * orhs_ );
            sres_   = scalar * ( olhs_ * orhs_ );
            osres_  = scalar * ( olhs_ * orhs_ );
            refres_ = scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with evaluated matrices
      {
         test_  = "Scaled multiplication with evaluated matrices (s*OP)";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            odres_  = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            osres_  = scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ = scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            odres_  = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            sres_   = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            osres_  = scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            refres_ = scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            odres_  = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            osres_  = scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ = scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            odres_  = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            sres_   = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            osres_  = scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            refres_ = scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication (OP*s)
      //=====================================================================================

      // Scaled multiplication with the given matrices
      {
         test_  = "Scaled multiplication with the given matrices (OP*s)";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = ( lhs_ * rhs_ ) * scalar;
            odres_  = ( lhs_ * rhs_ ) * scalar;
            sres_   = ( lhs_ * rhs_ ) * scalar;
            osres_  = ( lhs_ * rhs_ ) * scalar;
            refres_ = ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = ( lhs_ * orhs_ ) * scalar;
            odres_  = ( lhs_ * orhs_ ) * scalar;
            sres_   = ( lhs_ * orhs_ ) * scalar;
            osres_  = ( lhs_ * orhs_ ) * scalar;
            refres_ = ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = ( olhs_ * rhs_ ) * scalar;
            odres_  = ( olhs_ * rhs_ ) * scalar;
            sres_   = ( olhs_ * rhs_ ) * scalar;
            osres_  = ( olhs_ * rhs_ ) * scalar;
            refres_ = ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = ( olhs_ * orhs_ ) * scalar;
            odres_  = ( olhs_ * orhs_ ) * scalar;
            sres_   = ( olhs_ * orhs_ ) * scalar;
            osres_  = ( olhs_ * orhs_ ) * scalar;
            refres_ = ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with evaluated matrices
      {
         test_  = "Scaled multiplication with evaluated matrices (OP*s)";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  = ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  = ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  = ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  = ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication (OP/s)
      //=====================================================================================

      // Scaled multiplication with the given matrices
      {
         test_  = "Scaled multiplication with the given matrices (OP/s)";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = ( lhs_ * rhs_ ) / scalar;
            odres_  = ( lhs_ * rhs_ ) / scalar;
            sres_   = ( lhs_ * rhs_ ) / scalar;
            osres_  = ( lhs_ * rhs_ ) / scalar;
            refres_ = ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = ( lhs_ * orhs_ ) / scalar;
            odres_  = ( lhs_ * orhs_ ) / scalar;
            sres_   = ( lhs_ * orhs_ ) / scalar;
            osres_  = ( lhs_ * orhs_ ) / scalar;
            refres_ = ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = ( olhs_ * rhs_ ) / scalar;
            odres_  = ( olhs_ * rhs_ ) / scalar;
            sres_   = ( olhs_ * rhs_ ) / scalar;
            osres_  = ( olhs_ * rhs_ ) / scalar;
            refres_ = ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = ( olhs_ * orhs_ ) / scalar;
            odres_  = ( olhs_ * orhs_ ) / scalar;
            sres_   = ( olhs_ * orhs_ ) / scalar;
            osres_  = ( olhs_ * orhs_ ) / scalar;
            refres_ = ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with evaluated matrices
      {
         test_  = "Scaled multiplication with evaluated matrices (OP/s)";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  = ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  = ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  = ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  = ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ = ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (s*OP)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrices
      {
         test_  = "Scaled multiplication with addition assignment with the given matrices (s*OP)";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += scalar * ( lhs_ * rhs_ );
            odres_  += scalar * ( lhs_ * rhs_ );
            sres_   += scalar * ( lhs_ * rhs_ );
            osres_  += scalar * ( lhs_ * rhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += scalar * ( lhs_ * orhs_ );
            odres_  += scalar * ( lhs_ * orhs_ );
            sres_   += scalar * ( lhs_ * orhs_ );
            osres_  += scalar * ( lhs_ * orhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += scalar * ( olhs_ * rhs_ );
            odres_  += scalar * ( olhs_ * rhs_ );
            sres_   += scalar * ( olhs_ * rhs_ );
            osres_  += scalar * ( olhs_ * rhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += scalar * ( olhs_ * orhs_ );
            odres_  += scalar * ( olhs_ * orhs_ );
            sres_   += scalar * ( olhs_ * orhs_ );
            osres_  += scalar * ( olhs_ * orhs_ );
            refres_ += scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with addition assignment with evaluated matrices
      {
         test_  = "Scaled multiplication with addition assignment with evaluated matrices (s*OP)";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            odres_  += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            osres_  += scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            odres_  += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            sres_   += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            osres_  += scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            odres_  += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            osres_  += scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            odres_  += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            sres_   += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            osres_  += scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            refres_ += scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (OP*s)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrices
      {
         test_  = "Scaled multiplication with addition assignment with the given matrices (OP*s)";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += ( lhs_ * rhs_ ) * scalar;
            odres_  += ( lhs_ * rhs_ ) * scalar;
            sres_   += ( lhs_ * rhs_ ) * scalar;
            osres_  += ( lhs_ * rhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += ( lhs_ * orhs_ ) * scalar;
            odres_  += ( lhs_ * orhs_ ) * scalar;
            sres_   += ( lhs_ * orhs_ ) * scalar;
            osres_  += ( lhs_ * orhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += ( olhs_ * rhs_ ) * scalar;
            odres_  += ( olhs_ * rhs_ ) * scalar;
            sres_   += ( olhs_ * rhs_ ) * scalar;
            osres_  += ( olhs_ * rhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += ( olhs_ * orhs_ ) * scalar;
            odres_  += ( olhs_ * orhs_ ) * scalar;
            sres_   += ( olhs_ * orhs_ ) * scalar;
            osres_  += ( olhs_ * orhs_ ) * scalar;
            refres_ += ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with addition assignment with evaluated matrices
      {
         test_  = "Scaled multiplication with addition assignment with evaluated matrices (OP*s)";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  += ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  += ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  += ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  += ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with addition assignment (OP/s)
      //=====================================================================================

      // Scaled multiplication with addition assignment with the given matrices
      {
         test_  = "Scaled multiplication with addition assignment with the given matrices (OP/s)";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += ( lhs_ * rhs_ ) / scalar;
            odres_  += ( lhs_ * rhs_ ) / scalar;
            sres_   += ( lhs_ * rhs_ ) / scalar;
            osres_  += ( lhs_ * rhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += ( lhs_ * orhs_ ) / scalar;
            odres_  += ( lhs_ * orhs_ ) / scalar;
            sres_   += ( lhs_ * orhs_ ) / scalar;
            osres_  += ( lhs_ * orhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += ( olhs_ * rhs_ ) / scalar;
            odres_  += ( olhs_ * rhs_ ) / scalar;
            sres_   += ( olhs_ * rhs_ ) / scalar;
            osres_  += ( olhs_ * rhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += ( olhs_ * orhs_ ) / scalar;
            odres_  += ( olhs_ * orhs_ ) / scalar;
            sres_   += ( olhs_ * orhs_ ) / scalar;
            osres_  += ( olhs_ * orhs_ ) / scalar;
            refres_ += ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with addition assignment with evaluated matrices
      {
         test_  = "Scaled multiplication with addition assignment with evaluated matrices (OP/s)";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            dres_   += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  += ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  += ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  += ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  += ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ += ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (s*OP)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrices
      {
         test_  = "Scaled multiplication with subtraction assignment with the given matrices (s*OP)";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= scalar * ( lhs_ * rhs_ );
            odres_  -= scalar * ( lhs_ * rhs_ );
            sres_   -= scalar * ( lhs_ * rhs_ );
            osres_  -= scalar * ( lhs_ * rhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= scalar * ( lhs_ * orhs_ );
            odres_  -= scalar * ( lhs_ * orhs_ );
            sres_   -= scalar * ( lhs_ * orhs_ );
            osres_  -= scalar * ( lhs_ * orhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= scalar * ( olhs_ * rhs_ );
            odres_  -= scalar * ( olhs_ * rhs_ );
            sres_   -= scalar * ( olhs_ * rhs_ );
            osres_  -= scalar * ( olhs_ * rhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= scalar * ( olhs_ * orhs_ );
            odres_  -= scalar * ( olhs_ * orhs_ );
            sres_   -= scalar * ( olhs_ * orhs_ );
            osres_  -= scalar * ( olhs_ * orhs_ );
            refres_ -= scalar * ( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Scaled multiplication with subtraction assignment with evaluated matrices (s*OP)";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            odres_  -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            sres_   -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            osres_  -= scalar * ( eval( lhs_ ) * eval( rhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            odres_  -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            sres_   -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            osres_  -= scalar * ( eval( lhs_ ) * eval( orhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            odres_  -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            sres_   -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            osres_  -= scalar * ( eval( olhs_ ) * eval( rhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            odres_  -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            sres_   -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            osres_  -= scalar * ( eval( olhs_ ) * eval( orhs_ ) );
            refres_ -= scalar * ( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (OP*s)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrices
      {
         test_  = "Scaled multiplication with subtraction assignment with the given matrices (OP*s)";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= ( lhs_ * rhs_ ) * scalar;
            odres_  -= ( lhs_ * rhs_ ) * scalar;
            sres_   -= ( lhs_ * rhs_ ) * scalar;
            osres_  -= ( lhs_ * rhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= ( lhs_ * orhs_ ) * scalar;
            odres_  -= ( lhs_ * orhs_ ) * scalar;
            sres_   -= ( lhs_ * orhs_ ) * scalar;
            osres_  -= ( lhs_ * orhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= ( olhs_ * rhs_ ) * scalar;
            odres_  -= ( olhs_ * rhs_ ) * scalar;
            sres_   -= ( olhs_ * rhs_ ) * scalar;
            osres_  -= ( olhs_ * rhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= ( olhs_ * orhs_ ) * scalar;
            odres_  -= ( olhs_ * orhs_ ) * scalar;
            sres_   -= ( olhs_ * orhs_ ) * scalar;
            osres_  -= ( olhs_ * orhs_ ) * scalar;
            refres_ -= ( reflhs_ * refrhs_ ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Scaled multiplication with subtraction assignment with evaluated matrices (OP*s)";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            odres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            sres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            osres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            odres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            sres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            osres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) * scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) * scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Scaled multiplication with subtraction assignment (OP/s)
      //=====================================================================================

      // Scaled multiplication with subtraction assignment with the given matrices
      {
         test_  = "Scaled multiplication with subtraction assignment with the given matrices (OP/s)";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= ( lhs_ * rhs_ ) / scalar;
            odres_  -= ( lhs_ * rhs_ ) / scalar;
            sres_   -= ( lhs_ * rhs_ ) / scalar;
            osres_  -= ( lhs_ * rhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= ( lhs_ * orhs_ ) / scalar;
            odres_  -= ( lhs_ * orhs_ ) / scalar;
            sres_   -= ( lhs_ * orhs_ ) / scalar;
            osres_  -= ( lhs_ * orhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= ( olhs_ * rhs_ ) / scalar;
            odres_  -= ( olhs_ * rhs_ ) / scalar;
            sres_   -= ( olhs_ * rhs_ ) / scalar;
            osres_  -= ( olhs_ * rhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= ( olhs_ * orhs_ ) / scalar;
            odres_  -= ( olhs_ * orhs_ ) / scalar;
            sres_   -= ( olhs_ * orhs_ ) / scalar;
            osres_  -= ( olhs_ * orhs_ ) / scalar;
            refres_ -= ( reflhs_ * refrhs_ ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Scaled multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Scaled multiplication with subtraction assignment with evaluated matrices (OP/s)";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            dres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  -= ( eval( lhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  -= ( eval( lhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            odres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            sres_   -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            osres_  -= ( eval( olhs_ ) * eval( rhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            odres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            sres_   -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            osres_  -= ( eval( olhs_ ) * eval( orhs_ ) ) / scalar;
            refres_ -= ( eval( reflhs_ ) * eval( refrhs_ ) ) / scalar;
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the transpose sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the transpose matrix multiplication with plain assignment. In case
// any error resulting from the multiplication or the subsequent assignment is detected, a
// \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testTransOperation()
{
#if BLAZETEST_MATHTEST_TEST_TRANS_OPERATION
   if( BLAZETEST_MATHTEST_TEST_TRANS_OPERATION > 1 )
   {
      //=====================================================================================
      // Transpose multiplication
      //=====================================================================================

      // Transpose multiplication with the given matrices
      {
         test_  = "Transpose multiplication with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initTransposeResults();
            tdres_  = trans( lhs_ * rhs_ );
            todres_ = trans( lhs_ * rhs_ );
            tsres_  = trans( lhs_ * rhs_ );
            tosres_ = trans( lhs_ * rhs_ );
            refres_ = trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkTransposeResults<MT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = trans( lhs_ * orhs_ );
            todres_ = trans( lhs_ * orhs_ );
            tsres_  = trans( lhs_ * orhs_ );
            tosres_ = trans( lhs_ * orhs_ );
            refres_ = trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkTransposeResults<MT1,OMT2>();

         try {
            initTransposeResults();
            tdres_  = trans( olhs_ * rhs_ );
            todres_ = trans( olhs_ * rhs_ );
            tsres_  = trans( olhs_ * rhs_ );
            tosres_ = trans( olhs_ * rhs_ );
            refres_ = trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkTransposeResults<OMT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = trans( olhs_ * orhs_ );
            todres_ = trans( olhs_ * orhs_ );
            tsres_  = trans( olhs_ * orhs_ );
            tosres_ = trans( olhs_ * orhs_ );
            refres_ = trans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkTransposeResults<OMT1,OMT2>();
      }

      // Transpose multiplication with evaluated matrices
      {
         test_  = "Transpose multiplication with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initTransposeResults();
            tdres_  = trans( eval( lhs_ ) * eval( rhs_ ) );
            todres_ = trans( eval( lhs_ ) * eval( rhs_ ) );
            tsres_  = trans( eval( lhs_ ) * eval( rhs_ ) );
            tosres_ = trans( eval( lhs_ ) * eval( rhs_ ) );
            refres_ = trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkTransposeResults<MT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = trans( eval( lhs_ ) * eval( orhs_ ) );
            todres_ = trans( eval( lhs_ ) * eval( orhs_ ) );
            tsres_  = trans( eval( lhs_ ) * eval( orhs_ ) );
            tosres_ = trans( eval( lhs_ ) * eval( orhs_ ) );
            refres_ = trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkTransposeResults<MT1,OMT2>();

         try {
            initTransposeResults();
            tdres_  = trans( eval( olhs_ ) * eval( rhs_ ) );
            todres_ = trans( eval( olhs_ ) * eval( rhs_ ) );
            tsres_  = trans( eval( olhs_ ) * eval( rhs_ ) );
            tosres_ = trans( eval( olhs_ ) * eval( rhs_ ) );
            refres_ = trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkTransposeResults<OMT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = trans( eval( olhs_ ) * eval( orhs_ ) );
            todres_ = trans( eval( olhs_ ) * eval( orhs_ ) );
            tsres_  = trans( eval( olhs_ ) * eval( orhs_ ) );
            tosres_ = trans( eval( olhs_ ) * eval( orhs_ ) );
            refres_ = trans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkTransposeResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the conjugate transpose sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the conjugate transpose matrix multiplication with plain assignment. In
// case any error resulting from the multiplication or the subsequent assignment is detected,
// a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testCTransOperation()
{
#if BLAZETEST_MATHTEST_TEST_CTRANS_OPERATION
   if( BLAZETEST_MATHTEST_TEST_CTRANS_OPERATION > 1 )
   {
      //=====================================================================================
      // Conjugate transpose multiplication
      //=====================================================================================

      // Conjugate transpose multiplication with the given matrices
      {
         test_  = "Conjugate transpose multiplication with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initTransposeResults();
            tdres_  = ctrans( lhs_ * rhs_ );
            todres_ = ctrans( lhs_ * rhs_ );
            tsres_  = ctrans( lhs_ * rhs_ );
            tosres_ = ctrans( lhs_ * rhs_ );
            refres_ = ctrans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkTransposeResults<MT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = ctrans( lhs_ * orhs_ );
            todres_ = ctrans( lhs_ * orhs_ );
            tsres_  = ctrans( lhs_ * orhs_ );
            tosres_ = ctrans( lhs_ * orhs_ );
            refres_ = ctrans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkTransposeResults<MT1,OMT2>();

         try {
            initTransposeResults();
            tdres_  = ctrans( olhs_ * rhs_ );
            todres_ = ctrans( olhs_ * rhs_ );
            tsres_  = ctrans( olhs_ * rhs_ );
            tosres_ = ctrans( olhs_ * rhs_ );
            refres_ = ctrans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkTransposeResults<OMT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = ctrans( olhs_ * orhs_ );
            todres_ = ctrans( olhs_ * orhs_ );
            tsres_  = ctrans( olhs_ * orhs_ );
            tosres_ = ctrans( olhs_ * orhs_ );
            refres_ = ctrans( reflhs_ * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkTransposeResults<OMT1,OMT2>();
      }

      // Conjugate transpose multiplication with evaluated matrices
      {
         test_  = "Conjugate transpose multiplication with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initTransposeResults();
            tdres_  = ctrans( eval( lhs_ ) * eval( rhs_ ) );
            todres_ = ctrans( eval( lhs_ ) * eval( rhs_ ) );
            tsres_  = ctrans( eval( lhs_ ) * eval( rhs_ ) );
            tosres_ = ctrans( eval( lhs_ ) * eval( rhs_ ) );
            refres_ = ctrans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkTransposeResults<MT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = ctrans( eval( lhs_ ) * eval( orhs_ ) );
            todres_ = ctrans( eval( lhs_ ) * eval( orhs_ ) );
            tsres_  = ctrans( eval( lhs_ ) * eval( orhs_ ) );
            tosres_ = ctrans( eval( lhs_ ) * eval( orhs_ ) );
            refres_ = ctrans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkTransposeResults<MT1,OMT2>();

         try {
            initTransposeResults();
            tdres_  = ctrans( eval( olhs_ ) * eval( rhs_ ) );
            todres_ = ctrans( eval( olhs_ ) * eval( rhs_ ) );
            tsres_  = ctrans( eval( olhs_ ) * eval( rhs_ ) );
            tosres_ = ctrans( eval( olhs_ ) * eval( rhs_ ) );
            refres_ = ctrans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkTransposeResults<OMT1,MT2>();

         try {
            initTransposeResults();
            tdres_  = ctrans( eval( olhs_ ) * eval( orhs_ ) );
            todres_ = ctrans( eval( olhs_ ) * eval( orhs_ ) );
            tsres_  = ctrans( eval( olhs_ ) * eval( orhs_ ) );
            tosres_ = ctrans( eval( olhs_ ) * eval( orhs_ ) );
            refres_ = ctrans( eval( reflhs_ ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkTransposeResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the abs sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the abs matrix multiplication with plain assignment, addition assignment,
// and subtraction assignment. In case any error resulting from the multiplication or the
// subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testAbsOperation()
{
#if BLAZETEST_MATHTEST_TEST_ABS_OPERATION
   if( BLAZETEST_MATHTEST_TEST_ABS_OPERATION > 1 )
   {
      testCustomOperation( blaze::Abs(), "abs" );
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the conjugate sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the conjugate matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testConjOperation()
{
#if BLAZETEST_MATHTEST_TEST_CONJ_OPERATION
   if( BLAZETEST_MATHTEST_TEST_CONJ_OPERATION > 1 )
   {
      testCustomOperation( blaze::Conj(), "conj" );
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the \a real sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the \a real matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testRealOperation()
{
#if BLAZETEST_MATHTEST_TEST_REAL_OPERATION
   if( BLAZETEST_MATHTEST_TEST_REAL_OPERATION > 1 )
   {
      testCustomOperation( blaze::Real(), "real" );
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the \a imag sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the \a imag matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testImagOperation()
{
#if BLAZETEST_MATHTEST_TEST_IMAG_OPERATION
   if( BLAZETEST_MATHTEST_TEST_IMAG_OPERATION > 1 )
   {
      testCustomOperation( blaze::Imag(), "imag" );
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the \a inv sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the \a inv matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testInvOperation()
{
#if BLAZETEST_MATHTEST_TEST_INV_OPERATION && BLAZETEST_MATHTEST_LAPACK_MODE
   if( BLAZETEST_MATHTEST_TEST_INV_OPERATION > 1 )
   {
      if( !isSquare( lhs_ * rhs_ ) || blaze::isDefault( det( lhs_ * rhs_ ) ) )
         return;

      testCustomOperation( blaze::Inv(), "inv" );
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the evaluated sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the evaluated matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testEvalOperation()
{
#if BLAZETEST_MATHTEST_TEST_EVAL_OPERATION
   if( BLAZETEST_MATHTEST_TEST_EVAL_OPERATION > 1 )
   {
      testCustomOperation( blaze::Eval(), "eval" );
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the serialized sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the serialized matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testSerialOperation()
{
#if BLAZETEST_MATHTEST_TEST_SERIAL_OPERATION
   if( BLAZETEST_MATHTEST_TEST_SERIAL_OPERATION > 1 )
   {
      testCustomOperation( blaze::Serial(), "serial" );
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the symmetric sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the symmetric matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment in the case that the left-hand side element type
// is the lower data type. In case any error resulting from the multiplication or the
// subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclSymOperation( blaze::TrueType )
{
#if BLAZETEST_MATHTEST_TEST_DECLSYM_OPERATION
   if( BLAZETEST_MATHTEST_TEST_DECLSYM_OPERATION > 1 )
   {
      if( lhs_.rows() != rhs_.columns() )
         return;


      //=====================================================================================
      // Test-specific setup
      //=====================================================================================

      MT2  rhs;
      OMT2 orhs;
      RT2  refrhs;

      try {
         rhs    = trans( lhs_ );
         orhs   = trans( lhs_ );
         refrhs = trans( lhs_ );
      }
      catch( std::exception& ) {
         return;
      }


      //=====================================================================================
      // Declsym multiplication
      //=====================================================================================

      // Declsym multiplication with the given matrices
      {
         test_  = "Declsym multiplication the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declsym( lhs_ * rhs );
            odres_  = declsym( lhs_ * rhs );
            sres_   = declsym( lhs_ * rhs );
            osres_  = declsym( lhs_ * rhs );
            refres_ = declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declsym( lhs_ * orhs );
            odres_  = declsym( lhs_ * orhs );
            sres_   = declsym( lhs_ * orhs );
            osres_  = declsym( lhs_ * orhs );
            refres_ = declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declsym( olhs_ * rhs );
            odres_  = declsym( olhs_ * rhs );
            sres_   = declsym( olhs_ * rhs );
            osres_  = declsym( olhs_ * rhs );
            refres_ = declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declsym( olhs_ * orhs );
            odres_  = declsym( olhs_ * orhs );
            sres_   = declsym( olhs_ * orhs );
            osres_  = declsym( olhs_ * orhs );
            refres_ = declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declsym multiplication with evaluated matrices
      {
         test_  = "Declsym multiplication with evaluated left-hand side matrix";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declsym( eval( lhs_ ) * eval( rhs ) );
            odres_  = declsym( eval( lhs_ ) * eval( rhs ) );
            sres_   = declsym( eval( lhs_ ) * eval( rhs ) );
            osres_  = declsym( eval( lhs_ ) * eval( rhs ) );
            refres_ = declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declsym( eval( lhs_ ) * eval( orhs ) );
            odres_  = declsym( eval( lhs_ ) * eval( orhs ) );
            sres_   = declsym( eval( lhs_ ) * eval( orhs ) );
            osres_  = declsym( eval( lhs_ ) * eval( orhs ) );
            refres_ = declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declsym( eval( olhs_ ) * eval( rhs ) );
            odres_  = declsym( eval( olhs_ ) * eval( rhs ) );
            sres_   = declsym( eval( olhs_ ) * eval( rhs ) );
            osres_  = declsym( eval( olhs_ ) * eval( rhs ) );
            refres_ = declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declsym( eval( olhs_ ) * eval( orhs ) );
            odres_  = declsym( eval( olhs_ ) * eval( orhs ) );
            sres_   = declsym( eval( olhs_ ) * eval( orhs ) );
            osres_  = declsym( eval( olhs_ ) * eval( orhs ) );
            refres_ = declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declsym multiplication with addition assignment
      //=====================================================================================

      // Declsym multiplication with addition assignment with the given matrices
      {
         test_  = "Declsym multiplication with addition assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declsym( lhs_ * rhs );
            odres_  += declsym( lhs_ * rhs );
            sres_   += declsym( lhs_ * rhs );
            osres_  += declsym( lhs_ * rhs );
            refres_ += declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declsym( lhs_ * orhs );
            odres_  += declsym( lhs_ * orhs );
            sres_   += declsym( lhs_ * orhs );
            osres_  += declsym( lhs_ * orhs );
            refres_ += declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declsym( olhs_ * rhs );
            odres_  += declsym( olhs_ * rhs );
            sres_   += declsym( olhs_ * rhs );
            osres_  += declsym( olhs_ * rhs );
            refres_ += declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declsym( olhs_ * orhs );
            odres_  += declsym( olhs_ * orhs );
            sres_   += declsym( olhs_ * orhs );
            osres_  += declsym( olhs_ * orhs );
            refres_ += declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declsym multiplication with addition assignment with evaluated matrices
      {
         test_  = "Declsym multiplication with addition assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declsym( eval( lhs_ ) * eval( rhs ) );
            odres_  += declsym( eval( lhs_ ) * eval( rhs ) );
            sres_   += declsym( eval( lhs_ ) * eval( rhs ) );
            osres_  += declsym( eval( lhs_ ) * eval( rhs ) );
            refres_ += declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declsym( eval( lhs_ ) * eval( orhs ) );
            odres_  += declsym( eval( lhs_ ) * eval( orhs ) );
            sres_   += declsym( eval( lhs_ ) * eval( orhs ) );
            osres_  += declsym( eval( lhs_ ) * eval( orhs ) );
            refres_ += declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declsym( eval( olhs_ ) * eval( rhs ) );
            odres_  += declsym( eval( olhs_ ) * eval( rhs ) );
            sres_   += declsym( eval( olhs_ ) * eval( rhs ) );
            osres_  += declsym( eval( olhs_ ) * eval( rhs ) );
            refres_ += declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declsym( eval( olhs_ ) * eval( orhs ) );
            odres_  += declsym( eval( olhs_ ) * eval( orhs ) );
            sres_   += declsym( eval( olhs_ ) * eval( orhs ) );
            osres_  += declsym( eval( olhs_ ) * eval( orhs ) );
            refres_ += declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declsym multiplication with subtraction assignment
      //=====================================================================================

      // Declsym multiplication with subtraction assignment with the given matrices
      {
         test_  = "Declsym multiplication with subtraction assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declsym( lhs_ * rhs );
            odres_  -= declsym( lhs_ * rhs );
            sres_   -= declsym( lhs_ * rhs );
            osres_  -= declsym( lhs_ * rhs );
            refres_ -= declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( lhs_ * orhs );
            odres_  -= declsym( lhs_ * orhs );
            sres_   -= declsym( lhs_ * orhs );
            osres_  -= declsym( lhs_ * orhs );
            refres_ -= declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declsym( olhs_ * rhs );
            odres_  -= declsym( olhs_ * rhs );
            sres_   -= declsym( olhs_ * rhs );
            osres_  -= declsym( olhs_ * rhs );
            refres_ -= declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( olhs_ * orhs );
            odres_  -= declsym( olhs_ * orhs );
            sres_   -= declsym( olhs_ * orhs );
            osres_  -= declsym( olhs_ * orhs );
            refres_ -= declsym( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declsym multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Declsym multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declsym( eval( lhs_ ) * eval( rhs ) );
            odres_  -= declsym( eval( lhs_ ) * eval( rhs ) );
            sres_   -= declsym( eval( lhs_ ) * eval( rhs ) );
            osres_  -= declsym( eval( lhs_ ) * eval( rhs ) );
            refres_ -= declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( eval( lhs_ ) * eval( orhs ) );
            odres_  -= declsym( eval( lhs_ ) * eval( orhs ) );
            sres_   -= declsym( eval( lhs_ ) * eval( orhs ) );
            osres_  -= declsym( eval( lhs_ ) * eval( orhs ) );
            refres_ -= declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declsym( eval( olhs_ ) * eval( rhs ) );
            odres_  -= declsym( eval( olhs_ ) * eval( rhs ) );
            sres_   -= declsym( eval( olhs_ ) * eval( rhs ) );
            osres_  -= declsym( eval( olhs_ ) * eval( rhs ) );
            refres_ -= declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( eval( olhs_ ) * eval( orhs ) );
            odres_  -= declsym( eval( olhs_ ) * eval( orhs ) );
            sres_   -= declsym( eval( olhs_ ) * eval( orhs ) );
            osres_  -= declsym( eval( olhs_ ) * eval( orhs ) );
            refres_ -= declsym( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the symmetric sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the symmetric matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment in the case that the right-hand side element type
// is the lower data type. In case any error resulting from the multiplication or the
// subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclSymOperation( blaze::FalseType )
{
#if BLAZETEST_MATHTEST_TEST_DECLSYM_OPERATION
   if( BLAZETEST_MATHTEST_TEST_DECLSYM_OPERATION > 1 )
   {
      if( lhs_.rows() != rhs_.columns() )
         return;


      //=====================================================================================
      // Test-specific setup
      //=====================================================================================

      MT1  lhs;
      OMT1 olhs;
      RT1  reflhs;

      try {
         lhs    = trans( rhs_ );
         olhs   = trans( rhs_ );
         reflhs = trans( rhs_ );
      }
      catch( std::exception& ) {
         return;
      }


      //=====================================================================================
      // Declsym multiplication
      //=====================================================================================

      // Declsym multiplication with the given matrices
      {
         test_  = "Declsym multiplication the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declsym( lhs * rhs_ );
            odres_  = declsym( lhs * rhs_ );
            sres_   = declsym( lhs * rhs_ );
            osres_  = declsym( lhs * rhs_ );
            refres_ = declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declsym( lhs * orhs_ );
            odres_  = declsym( lhs * orhs_ );
            sres_   = declsym( lhs * orhs_ );
            osres_  = declsym( lhs * orhs_ );
            refres_ = declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declsym( olhs * rhs_ );
            odres_  = declsym( olhs * rhs_ );
            sres_   = declsym( olhs * rhs_ );
            osres_  = declsym( olhs * rhs_ );
            refres_ = declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declsym( olhs * orhs_ );
            odres_  = declsym( olhs * orhs_ );
            sres_   = declsym( olhs * orhs_ );
            osres_  = declsym( olhs * orhs_ );
            refres_ = declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declsym multiplication with evaluated matrices
      {
         test_  = "Declsym multiplication with evaluated left-hand side matrix";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declsym( eval( lhs ) * eval( rhs_ ) );
            odres_  = declsym( eval( lhs ) * eval( rhs_ ) );
            sres_   = declsym( eval( lhs ) * eval( rhs_ ) );
            osres_  = declsym( eval( lhs ) * eval( rhs_ ) );
            refres_ = declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declsym( eval( lhs ) * eval( orhs_ ) );
            odres_  = declsym( eval( lhs ) * eval( orhs_ ) );
            sres_   = declsym( eval( lhs ) * eval( orhs_ ) );
            osres_  = declsym( eval( lhs ) * eval( orhs_ ) );
            refres_ = declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declsym( eval( olhs ) * eval( rhs_ ) );
            odres_  = declsym( eval( olhs ) * eval( rhs_ ) );
            sres_   = declsym( eval( olhs ) * eval( rhs_ ) );
            osres_  = declsym( eval( olhs ) * eval( rhs_ ) );
            refres_ = declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declsym( eval( olhs ) * eval( orhs_ ) );
            odres_  = declsym( eval( olhs ) * eval( orhs_ ) );
            sres_   = declsym( eval( olhs ) * eval( orhs_ ) );
            osres_  = declsym( eval( olhs ) * eval( orhs_ ) );
            refres_ = declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declsym multiplication with addition assignment
      //=====================================================================================

      // Declsym multiplication with addition assignment with the given matrices
      {
         test_  = "Declsym multiplication with addition assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declsym( lhs * rhs_ );
            odres_  += declsym( lhs * rhs_ );
            sres_   += declsym( lhs * rhs_ );
            osres_  += declsym( lhs * rhs_ );
            refres_ += declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declsym( lhs * orhs_ );
            odres_  += declsym( lhs * orhs_ );
            sres_   += declsym( lhs * orhs_ );
            osres_  += declsym( lhs * orhs_ );
            refres_ += declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declsym( olhs * rhs_ );
            odres_  += declsym( olhs * rhs_ );
            sres_   += declsym( olhs * rhs_ );
            osres_  += declsym( olhs * rhs_ );
            refres_ += declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declsym( olhs * orhs_ );
            odres_  += declsym( olhs * orhs_ );
            sres_   += declsym( olhs * orhs_ );
            osres_  += declsym( olhs * orhs_ );
            refres_ += declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declsym multiplication with addition assignment with evaluated matrices
      {
         test_  = "Declsym multiplication with addition assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declsym( eval( lhs ) * eval( rhs_ ) );
            odres_  += declsym( eval( lhs ) * eval( rhs_ ) );
            sres_   += declsym( eval( lhs ) * eval( rhs_ ) );
            osres_  += declsym( eval( lhs ) * eval( rhs_ ) );
            refres_ += declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declsym( eval( lhs ) * eval( orhs_ ) );
            odres_  += declsym( eval( lhs ) * eval( orhs_ ) );
            sres_   += declsym( eval( lhs ) * eval( orhs_ ) );
            osres_  += declsym( eval( lhs ) * eval( orhs_ ) );
            refres_ += declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declsym( eval( olhs ) * eval( rhs_ ) );
            odres_  += declsym( eval( olhs ) * eval( rhs_ ) );
            sres_   += declsym( eval( olhs ) * eval( rhs_ ) );
            osres_  += declsym( eval( olhs ) * eval( rhs_ ) );
            refres_ += declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declsym( eval( olhs ) * eval( orhs_ ) );
            odres_  += declsym( eval( olhs ) * eval( orhs_ ) );
            sres_   += declsym( eval( olhs ) * eval( orhs_ ) );
            osres_  += declsym( eval( olhs ) * eval( orhs_ ) );
            refres_ += declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declsym multiplication with subtraction assignment
      //=====================================================================================

      // Declsym multiplication with subtraction assignment with the given matrices
      {
         test_  = "Declsym multiplication with subtraction assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declsym( lhs * rhs_ );
            odres_  -= declsym( lhs * rhs_ );
            sres_   -= declsym( lhs * rhs_ );
            osres_  -= declsym( lhs * rhs_ );
            refres_ -= declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( lhs * orhs_ );
            odres_  -= declsym( lhs * orhs_ );
            sres_   -= declsym( lhs * orhs_ );
            osres_  -= declsym( lhs * orhs_ );
            refres_ -= declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declsym( olhs * rhs_ );
            odres_  -= declsym( olhs * rhs_ );
            sres_   -= declsym( olhs * rhs_ );
            osres_  -= declsym( olhs * rhs_ );
            refres_ -= declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( olhs * orhs_ );
            odres_  -= declsym( olhs * orhs_ );
            sres_   -= declsym( olhs * orhs_ );
            osres_  -= declsym( olhs * orhs_ );
            refres_ -= declsym( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declsym multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Declsym multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declsym( eval( lhs ) * eval( rhs_ ) );
            odres_  -= declsym( eval( lhs ) * eval( rhs_ ) );
            sres_   -= declsym( eval( lhs ) * eval( rhs_ ) );
            osres_  -= declsym( eval( lhs ) * eval( rhs_ ) );
            refres_ -= declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( eval( lhs ) * eval( orhs_ ) );
            odres_  -= declsym( eval( lhs ) * eval( orhs_ ) );
            sres_   -= declsym( eval( lhs ) * eval( orhs_ ) );
            osres_  -= declsym( eval( lhs ) * eval( orhs_ ) );
            refres_ -= declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declsym( eval( olhs ) * eval( rhs_ ) );
            odres_  -= declsym( eval( olhs ) * eval( rhs_ ) );
            sres_   -= declsym( eval( olhs ) * eval( rhs_ ) );
            osres_  -= declsym( eval( olhs ) * eval( rhs_ ) );
            refres_ -= declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declsym( eval( olhs ) * eval( orhs_ ) );
            odres_  -= declsym( eval( olhs ) * eval( orhs_ ) );
            sres_   -= declsym( eval( olhs ) * eval( orhs_ ) );
            osres_  -= declsym( eval( olhs ) * eval( orhs_ ) );
            refres_ -= declsym( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the Hermitian sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the Hermitian matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment in the case that the left-hand side element type
// is the lower data type. In case any error resulting from the multiplication or the
// subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclHermOperation( blaze::TrueType )
{
#if BLAZETEST_MATHTEST_TEST_DECLHERM_OPERATION
   if( BLAZETEST_MATHTEST_TEST_DECLHERM_OPERATION > 1 )
   {
      if( lhs_.rows() != rhs_.columns() )
         return;


      //=====================================================================================
      // Test-specific setup
      //=====================================================================================

      MT2  rhs;
      OMT2 orhs;
      RT2  refrhs;

      try {
         rhs    = trans( lhs_ );
         orhs   = trans( lhs_ );
         refrhs = trans( lhs_ );
      }
      catch( std::exception& ) {
         return;
      }


      //=====================================================================================
      // Declherm multiplication
      //=====================================================================================

      // Declherm multiplication with the given matrices
      {
         test_  = "Declherm multiplication the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declherm( lhs_ * rhs );
            odres_  = declherm( lhs_ * rhs );
            sres_   = declherm( lhs_ * rhs );
            osres_  = declherm( lhs_ * rhs );
            refres_ = declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declherm( lhs_ * orhs );
            odres_  = declherm( lhs_ * orhs );
            sres_   = declherm( lhs_ * orhs );
            osres_  = declherm( lhs_ * orhs );
            refres_ = declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declherm( olhs_ * rhs );
            odres_  = declherm( olhs_ * rhs );
            sres_   = declherm( olhs_ * rhs );
            osres_  = declherm( olhs_ * rhs );
            refres_ = declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declherm( olhs_ * orhs );
            odres_  = declherm( olhs_ * orhs );
            sres_   = declherm( olhs_ * orhs );
            osres_  = declherm( olhs_ * orhs );
            refres_ = declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declherm multiplication with evaluated matrices
      {
         test_  = "Declherm multiplication with evaluated left-hand side matrix";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declherm( eval( lhs_ ) * eval( rhs ) );
            odres_  = declherm( eval( lhs_ ) * eval( rhs ) );
            sres_   = declherm( eval( lhs_ ) * eval( rhs ) );
            osres_  = declherm( eval( lhs_ ) * eval( rhs ) );
            refres_ = declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declherm( eval( lhs_ ) * eval( orhs ) );
            odres_  = declherm( eval( lhs_ ) * eval( orhs ) );
            sres_   = declherm( eval( lhs_ ) * eval( orhs ) );
            osres_  = declherm( eval( lhs_ ) * eval( orhs ) );
            refres_ = declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declherm( eval( olhs_ ) * eval( rhs ) );
            odres_  = declherm( eval( olhs_ ) * eval( rhs ) );
            sres_   = declherm( eval( olhs_ ) * eval( rhs ) );
            osres_  = declherm( eval( olhs_ ) * eval( rhs ) );
            refres_ = declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declherm( eval( olhs_ ) * eval( orhs ) );
            odres_  = declherm( eval( olhs_ ) * eval( orhs ) );
            sres_   = declherm( eval( olhs_ ) * eval( orhs ) );
            osres_  = declherm( eval( olhs_ ) * eval( orhs ) );
            refres_ = declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declherm multiplication with addition assignment
      //=====================================================================================

      // Declherm multiplication with addition assignment with the given matrices
      {
         test_  = "Declherm multiplication with addition assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declherm( lhs_ * rhs );
            odres_  += declherm( lhs_ * rhs );
            sres_   += declherm( lhs_ * rhs );
            osres_  += declherm( lhs_ * rhs );
            refres_ += declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declherm( lhs_ * orhs );
            odres_  += declherm( lhs_ * orhs );
            sres_   += declherm( lhs_ * orhs );
            osres_  += declherm( lhs_ * orhs );
            refres_ += declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declherm( olhs_ * rhs );
            odres_  += declherm( olhs_ * rhs );
            sres_   += declherm( olhs_ * rhs );
            osres_  += declherm( olhs_ * rhs );
            refres_ += declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declherm( olhs_ * orhs );
            odres_  += declherm( olhs_ * orhs );
            sres_   += declherm( olhs_ * orhs );
            osres_  += declherm( olhs_ * orhs );
            refres_ += declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declherm multiplication with addition assignment with evaluated matrices
      {
         test_  = "Declherm multiplication with addition assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declherm( eval( lhs_ ) * eval( rhs ) );
            odres_  += declherm( eval( lhs_ ) * eval( rhs ) );
            sres_   += declherm( eval( lhs_ ) * eval( rhs ) );
            osres_  += declherm( eval( lhs_ ) * eval( rhs ) );
            refres_ += declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declherm( eval( lhs_ ) * eval( orhs ) );
            odres_  += declherm( eval( lhs_ ) * eval( orhs ) );
            sres_   += declherm( eval( lhs_ ) * eval( orhs ) );
            osres_  += declherm( eval( lhs_ ) * eval( orhs ) );
            refres_ += declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declherm( eval( olhs_ ) * eval( rhs ) );
            odres_  += declherm( eval( olhs_ ) * eval( rhs ) );
            sres_   += declherm( eval( olhs_ ) * eval( rhs ) );
            osres_  += declherm( eval( olhs_ ) * eval( rhs ) );
            refres_ += declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declherm( eval( olhs_ ) * eval( orhs ) );
            odres_  += declherm( eval( olhs_ ) * eval( orhs ) );
            sres_   += declherm( eval( olhs_ ) * eval( orhs ) );
            osres_  += declherm( eval( olhs_ ) * eval( orhs ) );
            refres_ += declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declherm multiplication with subtraction assignment
      //=====================================================================================

      // Declherm multiplication with subtraction assignment with the given matrices
      {
         test_  = "Declherm multiplication with subtraction assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declherm( lhs_ * rhs );
            odres_  -= declherm( lhs_ * rhs );
            sres_   -= declherm( lhs_ * rhs );
            osres_  -= declherm( lhs_ * rhs );
            refres_ -= declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( lhs_ * orhs );
            odres_  -= declherm( lhs_ * orhs );
            sres_   -= declherm( lhs_ * orhs );
            osres_  -= declherm( lhs_ * orhs );
            refres_ -= declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declherm( olhs_ * rhs );
            odres_  -= declherm( olhs_ * rhs );
            sres_   -= declherm( olhs_ * rhs );
            osres_  -= declherm( olhs_ * rhs );
            refres_ -= declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( olhs_ * orhs );
            odres_  -= declherm( olhs_ * orhs );
            sres_   -= declherm( olhs_ * orhs );
            osres_  -= declherm( olhs_ * orhs );
            refres_ -= declherm( reflhs_ * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declherm multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Declherm multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declherm( eval( lhs_ ) * eval( rhs ) );
            odres_  -= declherm( eval( lhs_ ) * eval( rhs ) );
            sres_   -= declherm( eval( lhs_ ) * eval( rhs ) );
            osres_  -= declherm( eval( lhs_ ) * eval( rhs ) );
            refres_ -= declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( eval( lhs_ ) * eval( orhs ) );
            odres_  -= declherm( eval( lhs_ ) * eval( orhs ) );
            sres_   -= declherm( eval( lhs_ ) * eval( orhs ) );
            osres_  -= declherm( eval( lhs_ ) * eval( orhs ) );
            refres_ -= declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declherm( eval( olhs_ ) * eval( rhs ) );
            odres_  -= declherm( eval( olhs_ ) * eval( rhs ) );
            sres_   -= declherm( eval( olhs_ ) * eval( rhs ) );
            osres_  -= declherm( eval( olhs_ ) * eval( rhs ) );
            refres_ -= declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( eval( olhs_ ) * eval( orhs ) );
            odres_  -= declherm( eval( olhs_ ) * eval( orhs ) );
            sres_   -= declherm( eval( olhs_ ) * eval( orhs ) );
            osres_  -= declherm( eval( olhs_ ) * eval( orhs ) );
            refres_ -= declherm( eval( reflhs_ ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the Hermitian sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the Hermitian matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment in the case that the right-hand side element type
// is the lower data type. In case any error resulting from the multiplication or the
// subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclHermOperation( blaze::FalseType )
{
#if BLAZETEST_MATHTEST_TEST_DECLHERM_OPERATION
   if( BLAZETEST_MATHTEST_TEST_DECLHERM_OPERATION > 1 )
   {
      if( lhs_.rows() != rhs_.columns() )
         return;


      MT1  lhs;
      OMT1 olhs;
      RT1  reflhs;

      try {
         lhs    = trans( rhs_ );
         olhs   = trans( rhs_ );
         reflhs = trans( rhs_ );
      }
      catch( std::exception& ) {
         return;
      }


      //=====================================================================================
      // Declherm multiplication
      //=====================================================================================

      // Declherm multiplication with the given matrices
      {
         test_  = "Declherm multiplication the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declherm( lhs * rhs_ );
            odres_  = declherm( lhs * rhs_ );
            sres_   = declherm( lhs * rhs_ );
            osres_  = declherm( lhs * rhs_ );
            refres_ = declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declherm( lhs * orhs_ );
            odres_  = declherm( lhs * orhs_ );
            sres_   = declherm( lhs * orhs_ );
            osres_  = declherm( lhs * orhs_ );
            refres_ = declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declherm( olhs * rhs_ );
            odres_  = declherm( olhs * rhs_ );
            sres_   = declherm( olhs * rhs_ );
            osres_  = declherm( olhs * rhs_ );
            refres_ = declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declherm( olhs * orhs_ );
            odres_  = declherm( olhs * orhs_ );
            sres_   = declherm( olhs * orhs_ );
            osres_  = declherm( olhs * orhs_ );
            refres_ = declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declherm multiplication with evaluated matrices
      {
         test_  = "Declherm multiplication with evaluated left-hand side matrix";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declherm( eval( lhs ) * eval( rhs_ ) );
            odres_  = declherm( eval( lhs ) * eval( rhs_ ) );
            sres_   = declherm( eval( lhs ) * eval( rhs_ ) );
            osres_  = declherm( eval( lhs ) * eval( rhs_ ) );
            refres_ = declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declherm( eval( lhs ) * eval( orhs_ ) );
            odres_  = declherm( eval( lhs ) * eval( orhs_ ) );
            sres_   = declherm( eval( lhs ) * eval( orhs_ ) );
            osres_  = declherm( eval( lhs ) * eval( orhs_ ) );
            refres_ = declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declherm( eval( olhs ) * eval( rhs_ ) );
            odres_  = declherm( eval( olhs ) * eval( rhs_ ) );
            sres_   = declherm( eval( olhs ) * eval( rhs_ ) );
            osres_  = declherm( eval( olhs ) * eval( rhs_ ) );
            refres_ = declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declherm( eval( olhs ) * eval( orhs_ ) );
            odres_  = declherm( eval( olhs ) * eval( orhs_ ) );
            sres_   = declherm( eval( olhs ) * eval( orhs_ ) );
            osres_  = declherm( eval( olhs ) * eval( orhs_ ) );
            refres_ = declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declherm multiplication with addition assignment
      //=====================================================================================

      // Declherm multiplication with addition assignment with the given matrices
      {
         test_  = "Declherm multiplication with addition assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declherm( lhs * rhs_ );
            odres_  += declherm( lhs * rhs_ );
            sres_   += declherm( lhs * rhs_ );
            osres_  += declherm( lhs * rhs_ );
            refres_ += declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declherm( lhs * orhs_ );
            odres_  += declherm( lhs * orhs_ );
            sres_   += declherm( lhs * orhs_ );
            osres_  += declherm( lhs * orhs_ );
            refres_ += declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declherm( olhs * rhs_ );
            odres_  += declherm( olhs * rhs_ );
            sres_   += declherm( olhs * rhs_ );
            osres_  += declherm( olhs * rhs_ );
            refres_ += declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declherm( olhs * orhs_ );
            odres_  += declherm( olhs * orhs_ );
            sres_   += declherm( olhs * orhs_ );
            osres_  += declherm( olhs * orhs_ );
            refres_ += declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declherm multiplication with addition assignment with evaluated matrices
      {
         test_  = "Declherm multiplication with addition assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declherm( eval( lhs ) * eval( rhs_ ) );
            odres_  += declherm( eval( lhs ) * eval( rhs_ ) );
            sres_   += declherm( eval( lhs ) * eval( rhs_ ) );
            osres_  += declherm( eval( lhs ) * eval( rhs_ ) );
            refres_ += declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declherm( eval( lhs ) * eval( orhs_ ) );
            odres_  += declherm( eval( lhs ) * eval( orhs_ ) );
            sres_   += declherm( eval( lhs ) * eval( orhs_ ) );
            osres_  += declherm( eval( lhs ) * eval( orhs_ ) );
            refres_ += declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declherm( eval( olhs ) * eval( rhs_ ) );
            odres_  += declherm( eval( olhs ) * eval( rhs_ ) );
            sres_   += declherm( eval( olhs ) * eval( rhs_ ) );
            osres_  += declherm( eval( olhs ) * eval( rhs_ ) );
            refres_ += declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declherm( eval( olhs ) * eval( orhs_ ) );
            odres_  += declherm( eval( olhs ) * eval( orhs_ ) );
            sres_   += declherm( eval( olhs ) * eval( orhs_ ) );
            osres_  += declherm( eval( olhs ) * eval( orhs_ ) );
            refres_ += declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declherm multiplication with subtraction assignment
      //=====================================================================================

      // Declherm multiplication with subtraction assignment with the given matrices
      {
         test_  = "Declherm multiplication with subtraction assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declherm( lhs * rhs_ );
            odres_  -= declherm( lhs * rhs_ );
            sres_   -= declherm( lhs * rhs_ );
            osres_  -= declherm( lhs * rhs_ );
            refres_ -= declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( lhs * orhs_ );
            odres_  -= declherm( lhs * orhs_ );
            sres_   -= declherm( lhs * orhs_ );
            osres_  -= declherm( lhs * orhs_ );
            refres_ -= declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declherm( olhs * rhs_ );
            odres_  -= declherm( olhs * rhs_ );
            sres_   -= declherm( olhs * rhs_ );
            osres_  -= declherm( olhs * rhs_ );
            refres_ -= declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( olhs * orhs_ );
            odres_  -= declherm( olhs * orhs_ );
            sres_   -= declherm( olhs * orhs_ );
            osres_  -= declherm( olhs * orhs_ );
            refres_ -= declherm( reflhs * refrhs_ );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declherm multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Declherm multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declherm( eval( lhs ) * eval( rhs_ ) );
            odres_  -= declherm( eval( lhs ) * eval( rhs_ ) );
            sres_   -= declherm( eval( lhs ) * eval( rhs_ ) );
            osres_  -= declherm( eval( lhs ) * eval( rhs_ ) );
            refres_ -= declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( eval( lhs ) * eval( orhs_ ) );
            odres_  -= declherm( eval( lhs ) * eval( orhs_ ) );
            sres_   -= declherm( eval( lhs ) * eval( orhs_ ) );
            osres_  -= declherm( eval( lhs ) * eval( orhs_ ) );
            refres_ -= declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declherm( eval( olhs ) * eval( rhs_ ) );
            odres_  -= declherm( eval( olhs ) * eval( rhs_ ) );
            sres_   -= declherm( eval( olhs ) * eval( rhs_ ) );
            osres_  -= declherm( eval( olhs ) * eval( rhs_ ) );
            refres_ -= declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declherm( eval( olhs ) * eval( orhs_ ) );
            odres_  -= declherm( eval( olhs ) * eval( orhs_ ) );
            sres_   -= declherm( eval( olhs ) * eval( orhs_ ) );
            osres_  -= declherm( eval( olhs ) * eval( orhs_ ) );
            refres_ -= declherm( eval( reflhs ) * eval( refrhs_ ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the lower sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the lower matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclLowOperation( blaze::TrueType )
{
#if BLAZETEST_MATHTEST_TEST_DECLLOW_OPERATION
   if( BLAZETEST_MATHTEST_TEST_DECLLOW_OPERATION > 1 )
   {
      if( lhs_.rows() != rhs_.columns() )
         return;


      //=====================================================================================
      // Test-specific setup of the left-hand side operand
      //=====================================================================================

      MT1 lhs( lhs_ );

      for( size_t i=0UL; i<lhs.rows(); ++i ) {
         for( size_t j=i+1UL; j<lhs.columns(); ++j ) {
            blaze::reset( lhs(i,j) );
         }
      }

      OMT1 olhs  ( lhs );
      RT1  reflhs( lhs );


      //=====================================================================================
      // Test-specific setup of the right-hand side operand
      //=====================================================================================

      MT2 rhs( rhs_ );

      for( size_t i=0UL; i<rhs.rows(); ++i ) {
         for( size_t j=i+1UL; j<rhs.columns(); ++j ) {
            blaze::reset( rhs(i,j) );
         }
      }

      OMT2 orhs  ( rhs );
      RT2  refrhs( rhs );


      //=====================================================================================
      // Decllow multiplication
      //=====================================================================================

      // Decllow multiplication with the given matrices
      {
         test_  = "Decllow multiplication the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = decllow( lhs * rhs );
            odres_  = decllow( lhs * rhs );
            sres_   = decllow( lhs * rhs );
            osres_  = decllow( lhs * rhs );
            refres_ = decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = decllow( lhs * orhs );
            odres_  = decllow( lhs * orhs );
            sres_   = decllow( lhs * orhs );
            osres_  = decllow( lhs * orhs );
            refres_ = decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = decllow( olhs * rhs );
            odres_  = decllow( olhs * rhs );
            sres_   = decllow( olhs * rhs );
            osres_  = decllow( olhs * rhs );
            refres_ = decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = decllow( olhs * orhs );
            odres_  = decllow( olhs * orhs );
            sres_   = decllow( olhs * orhs );
            osres_  = decllow( olhs * orhs );
            refres_ = decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Decllow multiplication with evaluated matrices
      {
         test_  = "Decllow multiplication with evaluated left-hand side matrix";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = decllow( eval( lhs ) * eval( rhs ) );
            odres_  = decllow( eval( lhs ) * eval( rhs ) );
            sres_   = decllow( eval( lhs ) * eval( rhs ) );
            osres_  = decllow( eval( lhs ) * eval( rhs ) );
            refres_ = decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = decllow( eval( lhs ) * eval( orhs ) );
            odres_  = decllow( eval( lhs ) * eval( orhs ) );
            sres_   = decllow( eval( lhs ) * eval( orhs ) );
            osres_  = decllow( eval( lhs ) * eval( orhs ) );
            refres_ = decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = decllow( eval( olhs ) * eval( rhs ) );
            odres_  = decllow( eval( olhs ) * eval( rhs ) );
            sres_   = decllow( eval( olhs ) * eval( rhs ) );
            osres_  = decllow( eval( olhs ) * eval( rhs ) );
            refres_ = decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = decllow( eval( olhs ) * eval( orhs ) );
            odres_  = decllow( eval( olhs ) * eval( orhs ) );
            sres_   = decllow( eval( olhs ) * eval( orhs ) );
            osres_  = decllow( eval( olhs ) * eval( orhs ) );
            refres_ = decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Decllow multiplication with addition assignment
      //=====================================================================================

      // Decllow multiplication with addition assignment with the given matrices
      {
         test_  = "Decllow multiplication with addition assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += decllow( lhs * rhs );
            odres_  += decllow( lhs * rhs );
            sres_   += decllow( lhs * rhs );
            osres_  += decllow( lhs * rhs );
            refres_ += decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += decllow( lhs * orhs );
            odres_  += decllow( lhs * orhs );
            sres_   += decllow( lhs * orhs );
            osres_  += decllow( lhs * orhs );
            refres_ += decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += decllow( olhs * rhs );
            odres_  += decllow( olhs * rhs );
            sres_   += decllow( olhs * rhs );
            osres_  += decllow( olhs * rhs );
            refres_ += decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += decllow( olhs * orhs );
            odres_  += decllow( olhs * orhs );
            sres_   += decllow( olhs * orhs );
            osres_  += decllow( olhs * orhs );
            refres_ += decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Decllow multiplication with addition assignment with evaluated matrices
      {
         test_  = "Decllow multiplication with addition assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += decllow( eval( lhs ) * eval( rhs ) );
            odres_  += decllow( eval( lhs ) * eval( rhs ) );
            sres_   += decllow( eval( lhs ) * eval( rhs ) );
            osres_  += decllow( eval( lhs ) * eval( rhs ) );
            refres_ += decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += decllow( eval( lhs ) * eval( orhs ) );
            odres_  += decllow( eval( lhs ) * eval( orhs ) );
            sres_   += decllow( eval( lhs ) * eval( orhs ) );
            osres_  += decllow( eval( lhs ) * eval( orhs ) );
            refres_ += decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += decllow( eval( olhs ) * eval( rhs ) );
            odres_  += decllow( eval( olhs ) * eval( rhs ) );
            sres_   += decllow( eval( olhs ) * eval( rhs ) );
            osres_  += decllow( eval( olhs ) * eval( rhs ) );
            refres_ += decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += decllow( eval( olhs ) * eval( orhs ) );
            odres_  += decllow( eval( olhs ) * eval( orhs ) );
            sres_   += decllow( eval( olhs ) * eval( orhs ) );
            osres_  += decllow( eval( olhs ) * eval( orhs ) );
            refres_ += decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Decllow multiplication with subtraction assignment
      //=====================================================================================

      // Decllow multiplication with subtraction assignment with the given matrices
      {
         test_  = "Decllow multiplication with subtraction assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= decllow( lhs * rhs );
            odres_  -= decllow( lhs * rhs );
            sres_   -= decllow( lhs * rhs );
            osres_  -= decllow( lhs * rhs );
            refres_ -= decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= decllow( lhs * orhs );
            odres_  -= decllow( lhs * orhs );
            sres_   -= decllow( lhs * orhs );
            osres_  -= decllow( lhs * orhs );
            refres_ -= decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= decllow( olhs * rhs );
            odres_  -= decllow( olhs * rhs );
            sres_   -= decllow( olhs * rhs );
            osres_  -= decllow( olhs * rhs );
            refres_ -= decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= decllow( olhs * orhs );
            odres_  -= decllow( olhs * orhs );
            sres_   -= decllow( olhs * orhs );
            osres_  -= decllow( olhs * orhs );
            refres_ -= decllow( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Decllow multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Decllow multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= decllow( eval( lhs ) * eval( rhs ) );
            odres_  -= decllow( eval( lhs ) * eval( rhs ) );
            sres_   -= decllow( eval( lhs ) * eval( rhs ) );
            osres_  -= decllow( eval( lhs ) * eval( rhs ) );
            refres_ -= decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= decllow( eval( lhs ) * eval( orhs ) );
            odres_  -= decllow( eval( lhs ) * eval( orhs ) );
            sres_   -= decllow( eval( lhs ) * eval( orhs ) );
            osres_  -= decllow( eval( lhs ) * eval( orhs ) );
            refres_ -= decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= decllow( eval( olhs ) * eval( rhs ) );
            odres_  -= decllow( eval( olhs ) * eval( rhs ) );
            sres_   -= decllow( eval( olhs ) * eval( rhs ) );
            osres_  -= decllow( eval( olhs ) * eval( rhs ) );
            refres_ -= decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= decllow( eval( olhs ) * eval( orhs ) );
            odres_  -= decllow( eval( olhs ) * eval( orhs ) );
            sres_   -= decllow( eval( olhs ) * eval( orhs ) );
            osres_  -= decllow( eval( olhs ) * eval( orhs ) );
            refres_ -= decllow( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Skipping the lower sparse matrix/dense matrix multiplication.
//
// \return void
//
// This function is called in case the lower matrix/matrix multiplication operation is not
// available for the given matrix types \a MT1 and \a MT2.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclLowOperation( blaze::FalseType )
{}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the upper sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the upper matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclUppOperation( blaze::TrueType )
{
#if BLAZETEST_MATHTEST_TEST_DECLUPP_OPERATION
   if( BLAZETEST_MATHTEST_TEST_DECLUPP_OPERATION > 1 )
   {
      if( lhs_.rows() != rhs_.columns() )
         return;


      //=====================================================================================
      // Test-specific setup of the left-hand side operand
      //=====================================================================================

      MT1 lhs( lhs_ );

      for( size_t j=0UL; j<lhs.columns(); ++j ) {
         for( size_t i=j+1UL; i<lhs.rows(); ++i ) {
            blaze::reset( lhs(i,j) );
         }
      }

      OMT1 olhs  ( lhs );
      RT1  reflhs( lhs );


      //=====================================================================================
      // Test-specific setup of the right-hand side operand
      //=====================================================================================

      MT2 rhs( rhs_ );

      for( size_t j=0UL; j<rhs.columns(); ++j ) {
         for( size_t i=j+1UL; i<rhs.rows(); ++i ) {
            blaze::reset( rhs(i,j) );
         }
      }

      OMT2 orhs  ( rhs );
      RT2  refrhs( rhs );


      //=====================================================================================
      // Declupp multiplication
      //=====================================================================================

      // Declupp multiplication with the given matrices
      {
         test_  = "Declupp multiplication the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declupp( lhs * rhs );
            odres_  = declupp( lhs * rhs );
            sres_   = declupp( lhs * rhs );
            osres_  = declupp( lhs * rhs );
            refres_ = declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declupp( lhs * orhs );
            odres_  = declupp( lhs * orhs );
            sres_   = declupp( lhs * orhs );
            osres_  = declupp( lhs * orhs );
            refres_ = declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declupp( olhs * rhs );
            odres_  = declupp( olhs * rhs );
            sres_   = declupp( olhs * rhs );
            osres_  = declupp( olhs * rhs );
            refres_ = declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declupp( olhs * orhs );
            odres_  = declupp( olhs * orhs );
            sres_   = declupp( olhs * orhs );
            osres_  = declupp( olhs * orhs );
            refres_ = declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declupp multiplication with evaluated matrices
      {
         test_  = "Declupp multiplication with evaluated left-hand side matrix";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = declupp( eval( lhs ) * eval( rhs ) );
            odres_  = declupp( eval( lhs ) * eval( rhs ) );
            sres_   = declupp( eval( lhs ) * eval( rhs ) );
            osres_  = declupp( eval( lhs ) * eval( rhs ) );
            refres_ = declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = declupp( eval( lhs ) * eval( orhs ) );
            odres_  = declupp( eval( lhs ) * eval( orhs ) );
            sres_   = declupp( eval( lhs ) * eval( orhs ) );
            osres_  = declupp( eval( lhs ) * eval( orhs ) );
            refres_ = declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = declupp( eval( olhs ) * eval( rhs ) );
            odres_  = declupp( eval( olhs ) * eval( rhs ) );
            sres_   = declupp( eval( olhs ) * eval( rhs ) );
            osres_  = declupp( eval( olhs ) * eval( rhs ) );
            refres_ = declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = declupp( eval( olhs ) * eval( orhs ) );
            odres_  = declupp( eval( olhs ) * eval( orhs ) );
            sres_   = declupp( eval( olhs ) * eval( orhs ) );
            osres_  = declupp( eval( olhs ) * eval( orhs ) );
            refres_ = declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declupp multiplication with addition assignment
      //=====================================================================================

      // Declupp multiplication with addition assignment with the given matrices
      {
         test_  = "Declupp multiplication with addition assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declupp( lhs * rhs );
            odres_  += declupp( lhs * rhs );
            sres_   += declupp( lhs * rhs );
            osres_  += declupp( lhs * rhs );
            refres_ += declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declupp( lhs * orhs );
            odres_  += declupp( lhs * orhs );
            sres_   += declupp( lhs * orhs );
            osres_  += declupp( lhs * orhs );
            refres_ += declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declupp( olhs * rhs );
            odres_  += declupp( olhs * rhs );
            sres_   += declupp( olhs * rhs );
            osres_  += declupp( olhs * rhs );
            refres_ += declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declupp( olhs * orhs );
            odres_  += declupp( olhs * orhs );
            sres_   += declupp( olhs * orhs );
            osres_  += declupp( olhs * orhs );
            refres_ += declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declupp multiplication with addition assignment with evaluated matrices
      {
         test_  = "Declupp multiplication with addition assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += declupp( eval( lhs ) * eval( rhs ) );
            odres_  += declupp( eval( lhs ) * eval( rhs ) );
            sres_   += declupp( eval( lhs ) * eval( rhs ) );
            osres_  += declupp( eval( lhs ) * eval( rhs ) );
            refres_ += declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += declupp( eval( lhs ) * eval( orhs ) );
            odres_  += declupp( eval( lhs ) * eval( orhs ) );
            sres_   += declupp( eval( lhs ) * eval( orhs ) );
            osres_  += declupp( eval( lhs ) * eval( orhs ) );
            refres_ += declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += declupp( eval( olhs ) * eval( rhs ) );
            odres_  += declupp( eval( olhs ) * eval( rhs ) );
            sres_   += declupp( eval( olhs ) * eval( rhs ) );
            osres_  += declupp( eval( olhs ) * eval( rhs ) );
            refres_ += declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += declupp( eval( olhs ) * eval( orhs ) );
            odres_  += declupp( eval( olhs ) * eval( orhs ) );
            sres_   += declupp( eval( olhs ) * eval( orhs ) );
            osres_  += declupp( eval( olhs ) * eval( orhs ) );
            refres_ += declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Declupp multiplication with subtraction assignment
      //=====================================================================================

      // Declupp multiplication with subtraction assignment with the given matrices
      {
         test_  = "Declupp multiplication with subtraction assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declupp( lhs * rhs );
            odres_  -= declupp( lhs * rhs );
            sres_   -= declupp( lhs * rhs );
            osres_  -= declupp( lhs * rhs );
            refres_ -= declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declupp( lhs * orhs );
            odres_  -= declupp( lhs * orhs );
            sres_   -= declupp( lhs * orhs );
            osres_  -= declupp( lhs * orhs );
            refres_ -= declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declupp( olhs * rhs );
            odres_  -= declupp( olhs * rhs );
            sres_   -= declupp( olhs * rhs );
            osres_  -= declupp( olhs * rhs );
            refres_ -= declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declupp( olhs * orhs );
            odres_  -= declupp( olhs * orhs );
            sres_   -= declupp( olhs * orhs );
            osres_  -= declupp( olhs * orhs );
            refres_ -= declupp( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Declupp multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Declupp multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= declupp( eval( lhs ) * eval( rhs ) );
            odres_  -= declupp( eval( lhs ) * eval( rhs ) );
            sres_   -= declupp( eval( lhs ) * eval( rhs ) );
            osres_  -= declupp( eval( lhs ) * eval( rhs ) );
            refres_ -= declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= declupp( eval( lhs ) * eval( orhs ) );
            odres_  -= declupp( eval( lhs ) * eval( orhs ) );
            sres_   -= declupp( eval( lhs ) * eval( orhs ) );
            osres_  -= declupp( eval( lhs ) * eval( orhs ) );
            refres_ -= declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= declupp( eval( olhs ) * eval( rhs ) );
            odres_  -= declupp( eval( olhs ) * eval( rhs ) );
            sres_   -= declupp( eval( olhs ) * eval( rhs ) );
            osres_  -= declupp( eval( olhs ) * eval( rhs ) );
            refres_ -= declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= declupp( eval( olhs ) * eval( orhs ) );
            odres_  -= declupp( eval( olhs ) * eval( orhs ) );
            sres_   -= declupp( eval( olhs ) * eval( orhs ) );
            osres_  -= declupp( eval( olhs ) * eval( orhs ) );
            refres_ -= declupp( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Skipping the upper sparse matrix/dense matrix multiplication.
//
// \return void
//
// This function is called in case the upper matrix/matrix multiplication operation is not
// available for the given matrix types \a MT1 and \a MT2.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclUppOperation( blaze::FalseType )
{}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the diagonal sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the diagonal matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclDiagOperation( blaze::TrueType )
{
#if BLAZETEST_MATHTEST_TEST_DECLDIAG_OPERATION
   if( BLAZETEST_MATHTEST_TEST_DECLDIAG_OPERATION > 1 )
   {
      if( lhs_.rows() != rhs_.columns() )
         return;


      //=====================================================================================
      // Test-specific setup of the left-hand side operand
      //=====================================================================================

      MT1 lhs( lhs_ );

      for( size_t i=0UL; i<lhs.rows(); ++i ) {
         const size_t jend( blaze::min( i, lhs.columns() ) );
         for( size_t j=0UL; j<jend; ++j ) {
            blaze::reset( lhs(i,j) );
         }
         for( size_t j=i+1UL; j<lhs.columns(); ++j ) {
            blaze::reset( lhs(i,j) );
         }
      }

      OMT1 olhs  ( lhs );
      RT1  reflhs( lhs );


      //=====================================================================================
      // Test-specific setup of the right-hand side operand
      //=====================================================================================

      MT2 rhs( rhs_ );

      for( size_t i=0UL; i<rhs.rows(); ++i ) {
         const size_t jend( blaze::min( i, rhs.columns() ) );
         for( size_t j=0UL; j<jend; ++j ) {
            blaze::reset( rhs(i,j) );
         }
         for( size_t j=i+1UL; j<rhs.columns(); ++j ) {
            blaze::reset( rhs(i,j) );
         }
      }

      OMT2 orhs  ( rhs );
      RT2  refrhs( rhs );


      //=====================================================================================
      // Decldiag multiplication
      //=====================================================================================

      // Decldiag multiplication with the given matrices
      {
         test_  = "Decldiag multiplication the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = decldiag( lhs * rhs );
            odres_  = decldiag( lhs * rhs );
            sres_   = decldiag( lhs * rhs );
            osres_  = decldiag( lhs * rhs );
            refres_ = decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = decldiag( lhs * orhs );
            odres_  = decldiag( lhs * orhs );
            sres_   = decldiag( lhs * orhs );
            osres_  = decldiag( lhs * orhs );
            refres_ = decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = decldiag( olhs * rhs );
            odres_  = decldiag( olhs * rhs );
            sres_   = decldiag( olhs * rhs );
            osres_  = decldiag( olhs * rhs );
            refres_ = decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = decldiag( olhs * orhs );
            odres_  = decldiag( olhs * orhs );
            sres_   = decldiag( olhs * orhs );
            osres_  = decldiag( olhs * orhs );
            refres_ = decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Decldiag multiplication with evaluated matrices
      {
         test_  = "Decldiag multiplication with evaluated left-hand side matrix";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   = decldiag( eval( lhs ) * eval( rhs ) );
            odres_  = decldiag( eval( lhs ) * eval( rhs ) );
            sres_   = decldiag( eval( lhs ) * eval( rhs ) );
            osres_  = decldiag( eval( lhs ) * eval( rhs ) );
            refres_ = decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   = decldiag( eval( lhs ) * eval( orhs ) );
            odres_  = decldiag( eval( lhs ) * eval( orhs ) );
            sres_   = decldiag( eval( lhs ) * eval( orhs ) );
            osres_  = decldiag( eval( lhs ) * eval( orhs ) );
            refres_ = decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   = decldiag( eval( olhs ) * eval( rhs ) );
            odres_  = decldiag( eval( olhs ) * eval( rhs ) );
            sres_   = decldiag( eval( olhs ) * eval( rhs ) );
            osres_  = decldiag( eval( olhs ) * eval( rhs ) );
            refres_ = decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   = decldiag( eval( olhs ) * eval( orhs ) );
            odres_  = decldiag( eval( olhs ) * eval( orhs ) );
            sres_   = decldiag( eval( olhs ) * eval( orhs ) );
            osres_  = decldiag( eval( olhs ) * eval( orhs ) );
            refres_ = decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Decldiag multiplication with addition assignment
      //=====================================================================================

      // Decldiag multiplication with addition assignment with the given matrices
      {
         test_  = "Decldiag multiplication with addition assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += decldiag( lhs * rhs );
            odres_  += decldiag( lhs * rhs );
            sres_   += decldiag( lhs * rhs );
            osres_  += decldiag( lhs * rhs );
            refres_ += decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += decldiag( lhs * orhs );
            odres_  += decldiag( lhs * orhs );
            sres_   += decldiag( lhs * orhs );
            osres_  += decldiag( lhs * orhs );
            refres_ += decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += decldiag( olhs * rhs );
            odres_  += decldiag( olhs * rhs );
            sres_   += decldiag( olhs * rhs );
            osres_  += decldiag( olhs * rhs );
            refres_ += decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += decldiag( olhs * orhs );
            odres_  += decldiag( olhs * orhs );
            sres_   += decldiag( olhs * orhs );
            osres_  += decldiag( olhs * orhs );
            refres_ += decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Decldiag multiplication with addition assignment with evaluated matrices
      {
         test_  = "Decldiag multiplication with addition assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   += decldiag( eval( lhs ) * eval( rhs ) );
            odres_  += decldiag( eval( lhs ) * eval( rhs ) );
            sres_   += decldiag( eval( lhs ) * eval( rhs ) );
            osres_  += decldiag( eval( lhs ) * eval( rhs ) );
            refres_ += decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   += decldiag( eval( lhs ) * eval( orhs ) );
            odres_  += decldiag( eval( lhs ) * eval( orhs ) );
            sres_   += decldiag( eval( lhs ) * eval( orhs ) );
            osres_  += decldiag( eval( lhs ) * eval( orhs ) );
            refres_ += decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   += decldiag( eval( olhs ) * eval( rhs ) );
            odres_  += decldiag( eval( olhs ) * eval( rhs ) );
            sres_   += decldiag( eval( olhs ) * eval( rhs ) );
            osres_  += decldiag( eval( olhs ) * eval( rhs ) );
            refres_ += decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   += decldiag( eval( olhs ) * eval( orhs ) );
            odres_  += decldiag( eval( olhs ) * eval( orhs ) );
            sres_   += decldiag( eval( olhs ) * eval( orhs ) );
            osres_  += decldiag( eval( olhs ) * eval( orhs ) );
            refres_ += decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Decldiag multiplication with subtraction assignment
      //=====================================================================================

      // Decldiag multiplication with subtraction assignment with the given matrices
      {
         test_  = "Decldiag multiplication with subtraction assignment with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= decldiag( lhs * rhs );
            odres_  -= decldiag( lhs * rhs );
            sres_   -= decldiag( lhs * rhs );
            osres_  -= decldiag( lhs * rhs );
            refres_ -= decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= decldiag( lhs * orhs );
            odres_  -= decldiag( lhs * orhs );
            sres_   -= decldiag( lhs * orhs );
            osres_  -= decldiag( lhs * orhs );
            refres_ -= decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= decldiag( olhs * rhs );
            odres_  -= decldiag( olhs * rhs );
            sres_   -= decldiag( olhs * rhs );
            osres_  -= decldiag( olhs * rhs );
            refres_ -= decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= decldiag( olhs * orhs );
            odres_  -= decldiag( olhs * orhs );
            sres_   -= decldiag( olhs * orhs );
            osres_  -= decldiag( olhs * orhs );
            refres_ -= decldiag( reflhs * refrhs );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Decldiag multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Decldiag multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            dres_   -= decldiag( eval( lhs ) * eval( rhs ) );
            odres_  -= decldiag( eval( lhs ) * eval( rhs ) );
            sres_   -= decldiag( eval( lhs ) * eval( rhs ) );
            osres_  -= decldiag( eval( lhs ) * eval( rhs ) );
            refres_ -= decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            dres_   -= decldiag( eval( lhs ) * eval( orhs ) );
            odres_  -= decldiag( eval( lhs ) * eval( orhs ) );
            sres_   -= decldiag( eval( lhs ) * eval( orhs ) );
            osres_  -= decldiag( eval( lhs ) * eval( orhs ) );
            refres_ -= decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            dres_   -= decldiag( eval( olhs ) * eval( rhs ) );
            odres_  -= decldiag( eval( olhs ) * eval( rhs ) );
            sres_   -= decldiag( eval( olhs ) * eval( rhs ) );
            osres_  -= decldiag( eval( olhs ) * eval( rhs ) );
            refres_ -= decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            dres_   -= decldiag( eval( olhs ) * eval( orhs ) );
            odres_  -= decldiag( eval( olhs ) * eval( orhs ) );
            sres_   -= decldiag( eval( olhs ) * eval( orhs ) );
            osres_  -= decldiag( eval( olhs ) * eval( orhs ) );
            refres_ -= decldiag( eval( reflhs ) * eval( refrhs ) );
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Skipping the diagonal sparse matrix/dense matrix multiplication.
//
// \return void
//
// This function is called in case the diagonal matrix/matrix multiplication operation is not
// available for the given matrix types \a MT1 and \a MT2.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testDeclDiagOperation( blaze::FalseType )
{}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the submatrix-wise sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the submatrix-wise matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testSubmatrixOperation()
{
#if BLAZETEST_MATHTEST_TEST_SUBMATRIX_OPERATION
   if( BLAZETEST_MATHTEST_TEST_SUBMATRIX_OPERATION > 1 )
   {
      if( lhs_.rows() == 0UL || rhs_.columns() == 0UL )
         return;


      //=====================================================================================
      // Submatrix-wise multiplication
      //=====================================================================================

      // Submatrix-wise multiplication with the given matrices
      {
         test_  = "Submatrix-wise multiplication with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Submatrix-wise multiplication with evaluated matrices
      {
         test_  = "Submatrix-wise multiplication with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) = submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) = submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Submatrix-wise multiplication with addition assignment
      //=====================================================================================

      // Submatrix-wise multiplication with addition assignment with the given matrices
      {
         test_  = "Submatrix-wise multiplication with addition assignment with the given matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Submatrix-wise multiplication with addition assignment with evaluated matrices
      {
         test_  = "Submatrix-wise multiplication with addition assignment with evaluated matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) += submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) += submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Submatrix-wise multiplication with subtraction assignment
      //=====================================================================================

      // Submatrix-wise multiplication with subtraction assignment with the given matrices
      {
         test_  = "Submatrix-wise multiplication with subtraction assignment with the given matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( lhs_ * rhs_      , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( lhs_ * orhs_     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( olhs_ * rhs_     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( olhs_ * orhs_    , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( reflhs_ * refrhs_, row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Submatrix-wise multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Submatrix-wise multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( rhs_ )      , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( eval( lhs_ ) * eval( orhs_ )     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<rhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, rhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( rhs_ )     , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t row=0UL, m=0UL; row<lhs_.rows(); row+=m ) {
               m = blaze::rand<size_t>( 1UL, lhs_.rows() - row );
               for( size_t column=0UL, n=0UL; column<orhs_.columns(); column+=n ) {
                  n = blaze::rand<size_t>( 1UL, orhs_.columns() - column );
                  submatrix( dres_  , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( odres_ , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( sres_  , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( osres_ , row, column, m, n ) -= submatrix( eval( olhs_ ) * eval( orhs_ )    , row, column, m, n );
                  submatrix( refres_, row, column, m, n ) -= submatrix( eval( reflhs_ ) * eval( refrhs_ ), row, column, m, n );
               }
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the row-wise sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Addition error detected.
//
// This function tests the row-wise matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testRowOperation()
{
#if BLAZETEST_MATHTEST_TEST_ROW_OPERATION
   if( BLAZETEST_MATHTEST_TEST_ROW_OPERATION > 1 )
   {
      if( lhs_.rows() == 0UL )
         return;


      //=====================================================================================
      // Row-wise multiplication
      //=====================================================================================

      // Row-wise multiplication with the given matrices
      {
         test_  = "Row-wise multiplication with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( lhs_ * rhs_, i );
               row( odres_ , i ) = row( lhs_ * rhs_, i );
               row( sres_  , i ) = row( lhs_ * rhs_, i );
               row( osres_ , i ) = row( lhs_ * rhs_, i );
               row( refres_, i ) = row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( lhs_ * orhs_, i );
               row( odres_ , i ) = row( lhs_ * orhs_, i );
               row( sres_  , i ) = row( lhs_ * orhs_, i );
               row( osres_ , i ) = row( lhs_ * orhs_, i );
               row( refres_, i ) = row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( olhs_ * rhs_, i );
               row( odres_ , i ) = row( olhs_ * rhs_, i );
               row( sres_  , i ) = row( olhs_ * rhs_, i );
               row( osres_ , i ) = row( olhs_ * rhs_, i );
               row( refres_, i ) = row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( olhs_ * orhs_, i );
               row( odres_ , i ) = row( olhs_ * orhs_, i );
               row( sres_  , i ) = row( olhs_ * orhs_, i );
               row( osres_ , i ) = row( olhs_ * orhs_, i );
               row( refres_, i ) = row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Row-wise multiplication with evaluated matrices
      {
         test_  = "Row-wise multiplication with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( eval( lhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) = row( eval( lhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) = row( eval( lhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) = row( eval( lhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) = row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( eval( lhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) = row( eval( lhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) = row( eval( lhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) = row( eval( lhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) = row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( eval( olhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) = row( eval( olhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) = row( eval( olhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) = row( eval( olhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) = row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) = row( eval( olhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) = row( eval( olhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) = row( eval( olhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) = row( eval( olhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) = row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Row-wise multiplication with addition assignment
      //=====================================================================================

      // Row-wise multiplication with addition assignment with the given matrices
      {
         test_  = "Row-wise multiplication with addition assignment with the given matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( lhs_ * rhs_, i );
               row( odres_ , i ) += row( lhs_ * rhs_, i );
               row( sres_  , i ) += row( lhs_ * rhs_, i );
               row( osres_ , i ) += row( lhs_ * rhs_, i );
               row( refres_, i ) += row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( lhs_ * orhs_, i );
               row( odres_ , i ) += row( lhs_ * orhs_, i );
               row( sres_  , i ) += row( lhs_ * orhs_, i );
               row( osres_ , i ) += row( lhs_ * orhs_, i );
               row( refres_, i ) += row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( olhs_ * rhs_, i );
               row( odres_ , i ) += row( olhs_ * rhs_, i );
               row( sres_  , i ) += row( olhs_ * rhs_, i );
               row( osres_ , i ) += row( olhs_ * rhs_, i );
               row( refres_, i ) += row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( olhs_ * orhs_, i );
               row( odres_ , i ) += row( olhs_ * orhs_, i );
               row( sres_  , i ) += row( olhs_ * orhs_, i );
               row( osres_ , i ) += row( olhs_ * orhs_, i );
               row( refres_, i ) += row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Row-wise multiplication with addition assignment with evaluated matrices
      {
         test_  = "Row-wise multiplication with addition assignment with evaluated matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( eval( lhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) += row( eval( lhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) += row( eval( lhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) += row( eval( lhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) += row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( eval( lhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) += row( eval( lhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) += row( eval( lhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) += row( eval( lhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) += row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( eval( olhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) += row( eval( olhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) += row( eval( olhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) += row( eval( olhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) += row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) += row( eval( olhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) += row( eval( olhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) += row( eval( olhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) += row( eval( olhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) += row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Row-wise multiplication with subtraction assignment
      //=====================================================================================

      // Row-wise multiplication with subtraction assignment with the given matrices
      {
         test_  = "Row-wise multiplication with subtraction assignment with the given matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( lhs_ * rhs_, i );
               row( odres_ , i ) -= row( lhs_ * rhs_, i );
               row( sres_  , i ) -= row( lhs_ * rhs_, i );
               row( osres_ , i ) -= row( lhs_ * rhs_, i );
               row( refres_, i ) -= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( lhs_ * orhs_, i );
               row( odres_ , i ) -= row( lhs_ * orhs_, i );
               row( sres_  , i ) -= row( lhs_ * orhs_, i );
               row( osres_ , i ) -= row( lhs_ * orhs_, i );
               row( refres_, i ) -= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( olhs_ * rhs_, i );
               row( odres_ , i ) -= row( olhs_ * rhs_, i );
               row( sres_  , i ) -= row( olhs_ * rhs_, i );
               row( osres_ , i ) -= row( olhs_ * rhs_, i );
               row( refres_, i ) -= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( olhs_ * orhs_, i );
               row( odres_ , i ) -= row( olhs_ * orhs_, i );
               row( sres_  , i ) -= row( olhs_ * orhs_, i );
               row( osres_ , i ) -= row( olhs_ * orhs_, i );
               row( refres_, i ) -= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Row-wise multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Row-wise multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) -= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) -= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) -= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) -= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) -= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) -= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) -= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) -= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) -= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) -= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) -= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) -= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) -= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) -= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) -= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) -= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) -= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Row-wise multiplication with multiplication assignment
      //=====================================================================================

      // Row-wise multiplication with multiplication assignment with the given matrices
      {
         test_  = "Row-wise multiplication with multiplication assignment with the given matrices";
         error_ = "Failed multiplication assignment operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( lhs_ * rhs_, i );
               row( odres_ , i ) *= row( lhs_ * rhs_, i );
               row( sres_  , i ) *= row( lhs_ * rhs_, i );
               row( osres_ , i ) *= row( lhs_ * rhs_, i );
               row( refres_, i ) *= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( lhs_ * orhs_, i );
               row( odres_ , i ) *= row( lhs_ * orhs_, i );
               row( sres_  , i ) *= row( lhs_ * orhs_, i );
               row( osres_ , i ) *= row( lhs_ * orhs_, i );
               row( refres_, i ) *= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( olhs_ * rhs_, i );
               row( odres_ , i ) *= row( olhs_ * rhs_, i );
               row( sres_  , i ) *= row( olhs_ * rhs_, i );
               row( osres_ , i ) *= row( olhs_ * rhs_, i );
               row( refres_, i ) *= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( olhs_ * orhs_, i );
               row( odres_ , i ) *= row( olhs_ * orhs_, i );
               row( sres_  , i ) *= row( olhs_ * orhs_, i );
               row( osres_ , i ) *= row( olhs_ * orhs_, i );
               row( refres_, i ) *= row( reflhs_ * refrhs_, i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Row-wise multiplication with multiplication assignment with evaluated matrices
      {
         test_  = "Row-wise multiplication with multiplication assignment with evaluated matrices";
         error_ = "Failed multiplication assignment operation";

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) *= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) *= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) *= row( eval( lhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) *= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) *= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) *= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) *= row( eval( lhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) *= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( odres_ , i ) *= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( sres_  , i ) *= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( osres_ , i ) *= row( eval( olhs_ ) * eval( rhs_ ), i );
               row( refres_, i ) *= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t i=0UL; i<lhs_.rows(); ++i ) {
               row( dres_  , i ) *= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( odres_ , i ) *= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( sres_  , i ) *= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( osres_ , i ) *= row( eval( olhs_ ) * eval( orhs_ ), i );
               row( refres_, i ) *= row( eval( reflhs_ ) * eval( refrhs_ ), i );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the column-wise sparse matrix/dense matrix multiplication.
//
// \return void
// \exception std::runtime_error Addition error detected.
//
// This function tests the column-wise matrix multiplication with plain assignment, addition
// assignment, and subtraction assignment. In case any error resulting from the multiplication
// or the subsequent assignment is detected, a \a std::runtime_error exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::testColumnOperation()
{
#if BLAZETEST_MATHTEST_TEST_COLUMN_OPERATION
   if( BLAZETEST_MATHTEST_TEST_COLUMN_OPERATION > 1 )
   {
      if( lhs_.columns() == 0UL )
         return;


      //=====================================================================================
      // Column-wise multiplication
      //=====================================================================================

      // Column-wise multiplication with the given matrices
      {
         test_  = "Column-wise multiplication with the given matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( lhs_ * rhs_, j );
               column( odres_ , j ) = column( lhs_ * rhs_, j );
               column( sres_  , j ) = column( lhs_ * rhs_, j );
               column( osres_ , j ) = column( lhs_ * rhs_, j );
               column( refres_, j ) = column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         return;

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( lhs_ * orhs_, j );
               column( odres_ , j ) = column( lhs_ * orhs_, j );
               column( sres_  , j ) = column( lhs_ * orhs_, j );
               column( osres_ , j ) = column( lhs_ * orhs_, j );
               column( refres_, j ) = column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( olhs_ * rhs_, j );
               column( odres_ , j ) = column( olhs_ * rhs_, j );
               column( sres_  , j ) = column( olhs_ * rhs_, j );
               column( osres_ , j ) = column( olhs_ * rhs_, j );
               column( refres_, j ) = column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( olhs_ * orhs_, j );
               column( odres_ , j ) = column( olhs_ * orhs_, j );
               column( sres_  , j ) = column( olhs_ * orhs_, j );
               column( osres_ , j ) = column( olhs_ * orhs_, j );
               column( refres_, j ) = column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Column-wise multiplication with evaluated matrices
      {
         test_  = "Column-wise multiplication with evaluated matrices";
         error_ = "Failed multiplication operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( eval( lhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) = column( eval( lhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) = column( eval( lhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) = column( eval( lhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) = column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( eval( lhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) = column( eval( lhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) = column( eval( lhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) = column( eval( lhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) = column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( eval( olhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) = column( eval( olhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) = column( eval( olhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) = column( eval( olhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) = column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) = column( eval( olhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) = column( eval( olhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) = column( eval( olhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) = column( eval( olhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) = column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Column-wise multiplication with addition assignment
      //=====================================================================================

      // Column-wise multiplication with addition assignment with the given matrices
      {
         test_  = "Column-wise multiplication with addition assignment with the given matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( lhs_ * rhs_, j );
               column( odres_ , j ) += column( lhs_ * rhs_, j );
               column( sres_  , j ) += column( lhs_ * rhs_, j );
               column( osres_ , j ) += column( lhs_ * rhs_, j );
               column( refres_, j ) += column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( lhs_ * orhs_, j );
               column( odres_ , j ) += column( lhs_ * orhs_, j );
               column( sres_  , j ) += column( lhs_ * orhs_, j );
               column( osres_ , j ) += column( lhs_ * orhs_, j );
               column( refres_, j ) += column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( olhs_ * rhs_, j );
               column( odres_ , j ) += column( olhs_ * rhs_, j );
               column( sres_  , j ) += column( olhs_ * rhs_, j );
               column( osres_ , j ) += column( olhs_ * rhs_, j );
               column( refres_, j ) += column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( olhs_ * orhs_, j );
               column( odres_ , j ) += column( olhs_ * orhs_, j );
               column( sres_  , j ) += column( olhs_ * orhs_, j );
               column( osres_ , j ) += column( olhs_ * orhs_, j );
               column( refres_, j ) += column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Column-wise multiplication with addition assignment with evaluated matrices
      {
         test_  = "Column-wise multiplication with addition assignment with evaluated matrices";
         error_ = "Failed addition assignment operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( eval( lhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) += column( eval( lhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) += column( eval( lhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) += column( eval( lhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) += column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( eval( lhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) += column( eval( lhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) += column( eval( lhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) += column( eval( lhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) += column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( eval( olhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) += column( eval( olhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) += column( eval( olhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) += column( eval( olhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) += column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) += column( eval( olhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) += column( eval( olhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) += column( eval( olhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) += column( eval( olhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) += column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Column-wise multiplication with subtraction assignment
      //=====================================================================================

      // Column-wise multiplication with subtraction assignment with the given matrices
      {
         test_  = "Column-wise multiplication with subtraction assignment with the given matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( lhs_ * rhs_, j );
               column( odres_ , j ) -= column( lhs_ * rhs_, j );
               column( sres_  , j ) -= column( lhs_ * rhs_, j );
               column( osres_ , j ) -= column( lhs_ * rhs_, j );
               column( refres_, j ) -= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( lhs_ * orhs_, j );
               column( odres_ , j ) -= column( lhs_ * orhs_, j );
               column( sres_  , j ) -= column( lhs_ * orhs_, j );
               column( osres_ , j ) -= column( lhs_ * orhs_, j );
               column( refres_, j ) -= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( olhs_ * rhs_, j );
               column( odres_ , j ) -= column( olhs_ * rhs_, j );
               column( sres_  , j ) -= column( olhs_ * rhs_, j );
               column( osres_ , j ) -= column( olhs_ * rhs_, j );
               column( refres_, j ) -= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( olhs_ * orhs_, j );
               column( odres_ , j ) -= column( olhs_ * orhs_, j );
               column( sres_  , j ) -= column( olhs_ * orhs_, j );
               column( osres_ , j ) -= column( olhs_ * orhs_, j );
               column( refres_, j ) -= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Column-wise multiplication with subtraction assignment with evaluated matrices
      {
         test_  = "Column-wise multiplication with subtraction assignment with evaluated matrices";
         error_ = "Failed subtraction assignment operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) -= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) -= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) -= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) -= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) -= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) -= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) -= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) -= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) -= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) -= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) -= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) -= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) -= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) -= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) -= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) -= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) -= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }


      //=====================================================================================
      // Column-wise multiplication with multiplication assignment
      //=====================================================================================

      // Column-wise multiplication with multiplication assignment with the given matrices
      {
         test_  = "Column-wise multiplication with multiplication assignment with the given matrices";
         error_ = "Failed multiplication assignment operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( lhs_ * rhs_, j );
               column( odres_ , j ) *= column( lhs_ * rhs_, j );
               column( sres_  , j ) *= column( lhs_ * rhs_, j );
               column( osres_ , j ) *= column( lhs_ * rhs_, j );
               column( refres_, j ) *= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( lhs_ * orhs_, j );
               column( odres_ , j ) *= column( lhs_ * orhs_, j );
               column( sres_  , j ) *= column( lhs_ * orhs_, j );
               column( osres_ , j ) *= column( lhs_ * orhs_, j );
               column( refres_, j ) *= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( olhs_ * rhs_, j );
               column( odres_ , j ) *= column( olhs_ * rhs_, j );
               column( sres_  , j ) *= column( olhs_ * rhs_, j );
               column( osres_ , j ) *= column( olhs_ * rhs_, j );
               column( refres_, j ) *= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( olhs_ * orhs_, j );
               column( odres_ , j ) *= column( olhs_ * orhs_, j );
               column( sres_  , j ) *= column( olhs_ * orhs_, j );
               column( osres_ , j ) *= column( olhs_ * orhs_, j );
               column( refres_, j ) *= column( reflhs_ * refrhs_, j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }

      // Column-wise multiplication with multiplication assignment with evaluated matrices
      {
         test_  = "Column-wise multiplication with multiplication assignment with evaluated matrices";
         error_ = "Failed multiplication assignment operation";

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) *= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) *= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) *= column( eval( lhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) *= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,MT2>( ex );
         }

         checkResults<MT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) *= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) *= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) *= column( eval( lhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) *= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<MT1,OMT2>( ex );
         }

         checkResults<MT1,OMT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( odres_ , j ) *= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( sres_  , j ) *= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( osres_ , j ) *= column( eval( olhs_ ) * eval( rhs_ ), j );
               column( refres_, j ) *= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,MT2>( ex );
         }

         checkResults<OMT1,MT2>();

         try {
            initResults();
            for( size_t j=0UL; j<rhs_.columns(); ++j ) {
               column( dres_  , j ) *= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( odres_ , j ) *= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( sres_  , j ) *= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( osres_ , j ) *= column( eval( olhs_ ) * eval( orhs_ ), j );
               column( refres_, j ) *= column( eval( reflhs_ ) * eval( refrhs_ ), j );
            }
         }
         catch( std::exception& ex ) {
            convertException<OMT1,OMT2>( ex );
         }

         checkResults<OMT1,OMT2>();
      }
   }
#endif
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Testing the customized sparse matrix/dense matrix multiplication.
//
// \param op The custom operation to be tested.
// \param name The human-readable name of the operation.
// \return void
// \exception std::runtime_error Multiplication error detected.
//
// This function tests the matrix multiplication with plain assignment, addition assignment, and
// subtraction assignment in combination with a custom operation. In case any error resulting
// from the multiplication or the subsequent assignment is detected, a \a std::runtime_error
// exception is thrown.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename OP >   // Type of the custom operation
void OperationTest<MT1,MT2>::testCustomOperation( OP op, const std::string& name )
{
   //=====================================================================================
   // Customized multiplication
   //=====================================================================================

   // Customized multiplication with the given matrices
   {
      test_  = "Customized multiplication with the given matrices (" + name + ")";
      error_ = "Failed multiplication operation";

      try {
         initResults();
         dres_   = op( lhs_ * rhs_ );
         odres_  = op( lhs_ * rhs_ );
         sres_   = op( lhs_ * rhs_ );
         osres_  = op( lhs_ * rhs_ );
         refres_ = op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<MT1,MT2>( ex );
      }

      checkResults<MT1,MT2>();

      try {
         initResults();
         dres_   = op( lhs_ * orhs_ );
         odres_  = op( lhs_ * orhs_ );
         sres_   = op( lhs_ * orhs_ );
         osres_  = op( lhs_ * orhs_ );
         refres_ = op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<MT1,OMT2>( ex );
      }

      checkResults<MT1,OMT2>();

      try {
         initResults();
         dres_   = op( olhs_ * rhs_ );
         odres_  = op( olhs_ * rhs_ );
         sres_   = op( olhs_ * rhs_ );
         osres_  = op( olhs_ * rhs_ );
         refres_ = op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,MT2>( ex );
      }

      checkResults<OMT1,MT2>();

      try {
         initResults();
         dres_   = op( olhs_ * orhs_ );
         odres_  = op( olhs_ * orhs_ );
         sres_   = op( olhs_ * orhs_ );
         osres_  = op( olhs_ * orhs_ );
         refres_ = op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,OMT2>( ex );
      }

      checkResults<OMT1,OMT2>();
   }

   // Customized multiplication with evaluated matrices
   {
      test_  = "Customized multiplication with evaluated matrices (" + name + ")";
      error_ = "Failed multiplication operation";

      try {
         initResults();
         dres_   = op( eval( lhs_ ) * eval( rhs_ ) );
         odres_  = op( eval( lhs_ ) * eval( rhs_ ) );
         sres_   = op( eval( lhs_ ) * eval( rhs_ ) );
         osres_  = op( eval( lhs_ ) * eval( rhs_ ) );
         refres_ = op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<MT1,MT2>( ex );
      }

      checkResults<MT1,MT2>();

      try {
         initResults();
         dres_   = op( eval( lhs_ ) * eval( orhs_ ) );
         odres_  = op( eval( lhs_ ) * eval( orhs_ ) );
         sres_   = op( eval( lhs_ ) * eval( orhs_ ) );
         osres_  = op( eval( lhs_ ) * eval( orhs_ ) );
         refres_ = op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<MT1,OMT2>( ex );
      }

      checkResults<MT1,OMT2>();

      try {
         initResults();
         dres_   = op( eval( olhs_ ) * eval( rhs_ ) );
         odres_  = op( eval( olhs_ ) * eval( rhs_ ) );
         sres_   = op( eval( olhs_ ) * eval( rhs_ ) );
         osres_  = op( eval( olhs_ ) * eval( rhs_ ) );
         refres_ = op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,MT2>( ex );
      }

      checkResults<OMT1,MT2>();

      try {
         initResults();
         dres_   = op( eval( olhs_ ) * eval( orhs_ ) );
         odres_  = op( eval( olhs_ ) * eval( orhs_ ) );
         sres_   = op( eval( olhs_ ) * eval( orhs_ ) );
         osres_  = op( eval( olhs_ ) * eval( orhs_ ) );
         refres_ = op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,OMT2>( ex );
      }

      checkResults<OMT1,OMT2>();
   }


   //=====================================================================================
   // Customized multiplication with addition assignment
   //=====================================================================================

   // Customized multiplication with addition assignment with the given matrices
   {
      test_  = "Customized multiplication with addition assignment with the given matrices (" + name + ")";
      error_ = "Failed addition assignment operation";

      try {
         initResults();
         dres_   += op( lhs_ * rhs_ );
         odres_  += op( lhs_ * rhs_ );
         sres_   += op( lhs_ * rhs_ );
         osres_  += op( lhs_ * rhs_ );
         refres_ += op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<MT1,MT2>( ex );
      }

      checkResults<MT1,MT2>();

      try {
         initResults();
         dres_   += op( lhs_ * orhs_ );
         odres_  += op( lhs_ * orhs_ );
         sres_   += op( lhs_ * orhs_ );
         osres_  += op( lhs_ * orhs_ );
         refres_ += op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<MT1,OMT2>( ex );
      }

      checkResults<MT1,OMT2>();

      try {
         initResults();
         dres_   += op( olhs_ * rhs_ );
         odres_  += op( olhs_ * rhs_ );
         sres_   += op( olhs_ * rhs_ );
         osres_  += op( olhs_ * rhs_ );
         refres_ += op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,MT2>( ex );
      }

      checkResults<OMT1,MT2>();

      try {
         initResults();
         dres_   += op( olhs_ * orhs_ );
         odres_  += op( olhs_ * orhs_ );
         sres_   += op( olhs_ * orhs_ );
         osres_  += op( olhs_ * orhs_ );
         refres_ += op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,OMT2>( ex );
      }

      checkResults<OMT1,OMT2>();
   }

   // Customized multiplication with addition assignment with evaluated matrices
   {
      test_  = "Customized multiplication with addition assignment with evaluated matrices (" + name + ")";
      error_ = "Failed addition assignment operation";

      try {
         initResults();
         dres_   += op( eval( lhs_ ) * eval( rhs_ ) );
         odres_  += op( eval( lhs_ ) * eval( rhs_ ) );
         sres_   += op( eval( lhs_ ) * eval( rhs_ ) );
         osres_  += op( eval( lhs_ ) * eval( rhs_ ) );
         refres_ += op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<MT1,MT2>( ex );
      }

      checkResults<MT1,MT2>();

      try {
         initResults();
         dres_   += op( eval( lhs_ ) * eval( orhs_ ) );
         odres_  += op( eval( lhs_ ) * eval( orhs_ ) );
         sres_   += op( eval( lhs_ ) * eval( orhs_ ) );
         osres_  += op( eval( lhs_ ) * eval( orhs_ ) );
         refres_ += op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<MT1,OMT2>( ex );
      }

      checkResults<MT1,OMT2>();

      try {
         initResults();
         dres_   += op( eval( olhs_ ) * eval( rhs_ ) );
         odres_  += op( eval( olhs_ ) * eval( rhs_ ) );
         sres_   += op( eval( olhs_ ) * eval( rhs_ ) );
         osres_  += op( eval( olhs_ ) * eval( rhs_ ) );
         refres_ += op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,MT2>( ex );
      }

      checkResults<OMT1,MT2>();

      try {
         initResults();
         dres_   += op( eval( olhs_ ) * eval( orhs_ ) );
         odres_  += op( eval( olhs_ ) * eval( orhs_ ) );
         sres_   += op( eval( olhs_ ) * eval( orhs_ ) );
         osres_  += op( eval( olhs_ ) * eval( orhs_ ) );
         refres_ += op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,OMT2>( ex );
      }

      checkResults<OMT1,OMT2>();
   }


   //=====================================================================================
   // Customized multiplication with subtraction assignment
   //=====================================================================================

   // Customized multiplication with subtraction assignment with the given matrices
   {
      test_  = "Customized multiplication with subtraction assignment with the given matrices (" + name + ")";
      error_ = "Failed subtraction assignment operation";

      try {
         initResults();
         dres_   -= op( lhs_ * rhs_ );
         odres_  -= op( lhs_ * rhs_ );
         sres_   -= op( lhs_ * rhs_ );
         osres_  -= op( lhs_ * rhs_ );
         refres_ -= op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<MT1,MT2>( ex );
      }

      checkResults<MT1,MT2>();

      try {
         initResults();
         dres_   -= op( lhs_ * orhs_ );
         odres_  -= op( lhs_ * orhs_ );
         sres_   -= op( lhs_ * orhs_ );
         osres_  -= op( lhs_ * orhs_ );
         refres_ -= op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<MT1,OMT2>( ex );
      }

      checkResults<MT1,OMT2>();

      try {
         initResults();
         dres_   -= op( olhs_ * rhs_ );
         odres_  -= op( olhs_ * rhs_ );
         sres_   -= op( olhs_ * rhs_ );
         osres_  -= op( olhs_ * rhs_ );
         refres_ -= op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,MT2>( ex );
      }

      checkResults<OMT1,MT2>();

      try {
         initResults();
         dres_   -= op( olhs_ * orhs_ );
         odres_  -= op( olhs_ * orhs_ );
         sres_   -= op( olhs_ * orhs_ );
         osres_  -= op( olhs_ * orhs_ );
         refres_ -= op( reflhs_ * refrhs_ );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,OMT2>( ex );
      }

      checkResults<OMT1,OMT2>();
   }

   // Customized multiplication with subtraction assignment with evaluated matrices
   {
      test_  = "Customized multiplication with subtraction assignment with evaluated matrices (" + name + ")";
      error_ = "Failed subtraction assignment operation";

      try {
         initResults();
         dres_   -= op( eval( lhs_ ) * eval( rhs_ ) );
         odres_  -= op( eval( lhs_ ) * eval( rhs_ ) );
         sres_   -= op( eval( lhs_ ) * eval( rhs_ ) );
         osres_  -= op( eval( lhs_ ) * eval( rhs_ ) );
         refres_ -= op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<MT1,MT2>( ex );
      }

      checkResults<MT1,MT2>();

      try {
         initResults();
         dres_   -= op( eval( lhs_ ) * eval( orhs_ ) );
         odres_  -= op( eval( lhs_ ) * eval( orhs_ ) );
         sres_   -= op( eval( lhs_ ) * eval( orhs_ ) );
         osres_  -= op( eval( lhs_ ) * eval( orhs_ ) );
         refres_ -= op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<MT1,OMT2>( ex );
      }

      checkResults<MT1,OMT2>();

      try {
         initResults();
         dres_   -= op( eval( olhs_ ) * eval( rhs_ ) );
         odres_  -= op( eval( olhs_ ) * eval( rhs_ ) );
         sres_   -= op( eval( olhs_ ) * eval( rhs_ ) );
         osres_  -= op( eval( olhs_ ) * eval( rhs_ ) );
         refres_ -= op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,MT2>( ex );
      }

      checkResults<OMT1,MT2>();

      try {
         initResults();
         dres_   -= op( eval( olhs_ ) * eval( orhs_ ) );
         odres_  -= op( eval( olhs_ ) * eval( orhs_ ) );
         sres_   -= op( eval( olhs_ ) * eval( orhs_ ) );
         osres_  -= op( eval( olhs_ ) * eval( orhs_ ) );
         refres_ -= op( eval( reflhs_ ) * eval( refrhs_ ) );
      }
      catch( std::exception& ex ) {
         convertException<OMT1,OMT2>( ex );
      }

      checkResults<OMT1,OMT2>();
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  ERROR DETECTION FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Checking and comparing the computed results.
//
// \return void
// \exception std::runtime_error Incorrect dense result detected.
// \exception std::runtime_error Incorrect sparse result detected.
//
// This function is called after each test case to check and compare the computed results. The
// two template arguments \a LT and \a RT indicate the types of the left-hand side and right-hand
// side operands used for the computations.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename LT     // Type of the left-hand side operand
        , typename RT >   // Type of the right-hand side operand
void OperationTest<MT1,MT2>::checkResults()
{
   using blaze::IsRowMajorMatrix;

   if( !isEqual( dres_, refres_ ) || !isEqual( odres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect dense result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Result:\n" << dres_ << "\n"
          << "   Result with opposite storage order:\n" << odres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( sres_, refres_ ) || !isEqual( osres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect sparse result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Result:\n" << sres_ << "\n"
          << "   Result with opposite storage order:\n" << osres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking and comparing the computed transpose results.
//
// \return void
// \exception std::runtime_error Incorrect dense result detected.
// \exception std::runtime_error Incorrect sparse result detected.
//
// This function is called after each test case to check and compare the computed transpose
// results. The two template arguments \a LT and \a RT indicate the types of the left-hand
// side and right-hand side operands used for the computations.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename LT     // Type of the left-hand side operand
        , typename RT >   // Type of the right-hand side operand
void OperationTest<MT1,MT2>::checkTransposeResults()
{
   using blaze::IsRowMajorMatrix;

   if( !isEqual( tdres_, refres_ ) || !isEqual( todres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect dense result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Transpose result:\n" << tdres_ << "\n"
          << "   Transpose result with opposite storage order:\n" << todres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( !isEqual( tsres_, refres_ ) || !isEqual( tosres_, refres_ ) ) {
      std::ostringstream oss;
      oss.precision( 20 );
      oss << " Test : " << test_ << "\n"
          << " Error: Incorrect sparse result detected\n"
          << " Details:\n"
          << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
          << "     " << typeid( LT ).name() << "\n"
          << "   Right-hand side " << ( IsRowMajorMatrix<RT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
          << "     " << typeid( RT ).name() << "\n"
          << "   Transpose result:\n" << tsres_ << "\n"
          << "   Transpose result with opposite storage order:\n" << tosres_ << "\n"
          << "   Expected result:\n" << refres_ << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  UTILITY FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Initializing the non-transpose result matrices.
//
// \return void
//
// This function is called before each non-transpose test case to initialize the according result
// matrices to random values.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::initResults()
{
   const blaze::UnderlyingBuiltin_<DRE> min( randmin );
   const blaze::UnderlyingBuiltin_<DRE> max( randmax );

   resize( dres_, rows( lhs_ ), columns( rhs_ ) );
   randomize( dres_, min, max );

   odres_  = dres_;
   sres_   = dres_;
   osres_  = dres_;
   refres_ = dres_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Initializing the transpose result matrices.
//
// \return void
//
// This function is called before each transpose test case to initialize the according result
// matrices to random values.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void OperationTest<MT1,MT2>::initTransposeResults()
{
   const blaze::UnderlyingBuiltin_<TDRE> min( randmin );
   const blaze::UnderlyingBuiltin_<TDRE> max( randmax );

   resize( tdres_, columns( rhs_ ), rows( lhs_ ) );
   randomize( tdres_, min, max );

   todres_ = tdres_;
   tsres_  = tdres_;
   tosres_ = tdres_;
   refres_ = tdres_;
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Convert the given exception into a \a std::runtime_error exception.
//
// \param ex The \a std::exception to be extended.
// \return void
// \exception std::runtime_error The converted exception.
//
// This function converts the given exception to a \a std::runtime_error exception. Additionally,
// the function extends the given exception message by all available information for the failed
// test. The two template arguments \a LT and \a RT indicate the types of the left-hand side and
// right-hand side operands used for the computations.
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
template< typename LT     // Type of the left-hand side operand
        , typename RT >   // Type of the right-hand side operand
void OperationTest<MT1,MT2>::convertException( const std::exception& ex )
{
   using blaze::IsRowMajorMatrix;

   std::ostringstream oss;
   oss << " Test : " << test_ << "\n"
       << " Error: " << error_ << "\n"
       << " Details:\n"
       << "   Left-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " sparse matrix type:\n"
       << "     " << typeid( LT ).name() << "\n"
       << "   Right-hand side " << ( IsRowMajorMatrix<LT>::value ? ( "row-major" ) : ( "column-major" ) ) << " dense matrix type:\n"
       << "     " << typeid( RT ).name() << "\n"
       << "   Error message: " << ex.what() << "\n";
   throw std::runtime_error( oss.str() );
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Testing the matrix multiplication between two specific matrix types.
//
// \param creator1 The creator for the left-hand side matrix.
// \param creator2 The creator for the right-hand side matrix.
// \return void
*/
template< typename MT1    // Type of the left-hand side sparse matrix
        , typename MT2 >  // Type of the right-hand side dense matrix
void runTest( const Creator<MT1>& creator1, const Creator<MT2>& creator2 )
{
   for( size_t rep=0UL; rep<repetitions; ++rep ) {
      OperationTest<MT1,MT2>( creator1, creator2 );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  MACROS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Macro for the definition of a sparse matrix/dense matrix multiplication test case.
*/
#define DEFINE_SMATDMATMULT_OPERATION_TEST( MT1, MT2 ) \
   extern template class blazetest::mathtest::smatdmatmult::OperationTest<MT1,MT2>
/*! \endcond */
//*************************************************************************************************


//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Macro for the execution of a sparse matrix/dense matrix multiplication test case.
*/
#define RUN_SMATDMATMULT_OPERATION_TEST( C1, C2 ) \
   blazetest::mathtest::smatdmatmult::runTest( C1, C2 )
/*! \endcond */
//*************************************************************************************************

} // namespace smatdmatmult

} // namespace mathtest

} // namespace blazetest

#endif
