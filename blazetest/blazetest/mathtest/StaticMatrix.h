//=================================================================================================
/*!
//  \file blazetest/mathtest/StaticMatrix.h
//  \brief Header file for the StaticMatrix math test
//
//  Copyright (C) 2011 Klaus Iglberger - All Rights Reserved
//
//  This file is part of the Blaze library. This library is free software; you can redistribute
//  it and/or modify it under the terms of the GNU General Public License as published by the
//  Free Software Foundation; either version 3, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along with a special
//  exception for linking and compiling against the Blaze library, the so-called "runtime
//  exception"; see the file COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#ifndef _BLAZETEST_MATHTEST_STATICMATRIX_H_
#define _BLAZETEST_MATHTEST_STATICMATRIX_H_


//*************************************************************************************************
// Includes
//*************************************************************************************************

#include <sstream>
#include <stdexcept>
#include <string>
#include <blaze/math/constraints/DenseMatrix.h>
#include <blaze/math/StaticMatrix.h>
#include <blaze/math/typetraits/IsRowMajorMatrix.h>
#include <blaze/util/AlignmentTrait.h>
#include <blaze/util/constraints/SameType.h>
#include <blazetest/system/Types.h>


namespace blazetest {

namespace mathtest {

namespace staticmatrix {

//=================================================================================================
//
//  CLASS DEFINITION
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Auxiliary class template for the StaticMatrix math test.
//
// The StaticMatrix class represents a test suite for the blaze::StaticMatrix class template.
// It performs a series of both compile time as well as runtime tests.
*/
class StaticMatrix
{
 public:
   //**Constructors********************************************************************************
   /*!\name Constructors */
   //@{
   explicit StaticMatrix();
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
   template< typename Type >
   void testAlignment( const std::string& type );

   void testConstructors();
   void testAssignment  ();
   void testFunctionCall();
   void testNonZeros    ();
   void testReset       ();
   void testTranspose   ();
   void testIsDiagonal  ();
   void testIsSymmetric ();
   void testScale       ();
   void testSwap        ();

   template< typename Type >
   void checkRows( const Type& matrix, size_t expectedRows ) const;

   template< typename Type >
   void checkColumns( const Type& matrix, size_t expectedColumns ) const;

   template< typename Type >
   void checkCapacity( const Type& matrix, size_t minCapacity ) const;

   template< typename Type >
   void checkNonZeros( const Type& matrix, size_t expectedNonZeros ) const;

   template< typename Type >
   void checkNonZeros( const Type& matrix, size_t index, size_t expectedNonZeros ) const;
   //@}
   //**********************************************************************************************

   //**Member variables****************************************************************************
   /*!\name Member variables */
   //@{
   std::string test_;  //!< Label of the currently performed test.
   //@}
   //**********************************************************************************************

   //**Type definitions****************************************************************************
   typedef blaze::StaticMatrix<int,2UL,3UL,blaze::rowMajor>  MT;   //!< Type of the static matrix
   typedef typename MT::TransposeType                        TMT;  //!< Transpose static matrix type
   typedef typename MT::ElementType                          ET;   //!< Element type of the static matrix
   //**********************************************************************************************

   //**Compile time checks*************************************************************************
   /*! \cond BLAZE_INTERNAL */
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( MT  );
   BLAZE_CONSTRAINT_MUST_BE_DENSE_MATRIX_TYPE( TMT );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( MT, typename TMT::TransposeType );
   BLAZE_CONSTRAINT_MUST_BE_SAME_TYPE( typename MT::ElementType, typename TMT::ElementType );
   /*! \endcond */
   //**********************************************************************************************
};
//*************************************************************************************************




//=================================================================================================
//
//  TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Test of the alignment of different StaticMatrix instances.
//
// \return void
// \param type The string representation of the given template type.
// \exception std::runtime_error Error detected.
//
// This function performs a test of the alignment of both a row-major and a column-major
// StaticMatrix instance of the given element type. In case an error is detected, a
// \a std::runtime_error exception is thrown.
*/
template< typename Type >
void StaticMatrix::testAlignment( const std::string& type )
{
   // Testing the alignment of the row-major matrix instance
   {
      blaze::StaticMatrix<Type,7UL,5UL,blaze::rowMajor> mat;
      const size_t alignment( blaze::AlignmentTrait<Type>::value );
      const size_t deviation( reinterpret_cast<size_t>( &mat(0UL,0UL) ) % alignment );

      if( deviation != 0UL ) {
         std::ostringstream oss;
         oss << " Test: StaticMatrix<" << type << ",7,5,rowMajor> alignment test\n"
             << " Error: Invalid alignment detected\n"
             << " Details:\n"
             << "   Expected alignment: " << alignment << "\n"
             << "   Deviation         : " << deviation << "\n";
         throw std::runtime_error( oss.str() );
      }
   }

   // Testing the alignment of the column-major matrix instance
   {
      blaze::StaticMatrix<Type,7UL,5UL,blaze::columnMajor> mat;
      const size_t alignment( blaze::AlignmentTrait<Type>::value );
      const size_t deviation( reinterpret_cast<size_t>( &mat(0UL,0UL) ) % alignment );

      if( deviation != 0UL ) {
         std::ostringstream oss;
         oss << " Test: StaticMatrix<" << type << ",7,5,columnMajor> alignment test\n"
             << " Error: Invalid alignment detected\n"
             << " Details:\n"
             << "   Expected alignment: " << alignment << "\n"
             << "   Deviation         : " << deviation << "\n";
         throw std::runtime_error( oss.str() );
      }
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking the number of rows of the given static matrix.
//
// \param matrix The static matrix to be checked.
// \param expectedRows The expected number of rows of the static matrix.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the number of rows of the given static matrix. In case the actual number
// of rows does not correspond to the given expected number of rows, a \a std::runtime_error
// exception is thrown.
*/
template< typename Type >  // Type of the static matrix
void StaticMatrix::checkRows( const Type& matrix, size_t expectedRows ) const
{
   if( matrix.rows() != expectedRows ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid number of rows detected\n"
          << " Details:\n"
          << "   Number of rows         : " << matrix.rows() << "\n"
          << "   Expected number of rows: " << expectedRows << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking the number of columns of the given static matrix.
//
// \param matrix The static matrix to be checked.
// \param expectedRows The expected number of columns of the static matrix.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the number of columns of the given static matrix. In case the
// actual number of columns does not correspond to the given expected number of columns,
// a \a std::runtime_error exception is thrown.
*/
template< typename Type >  // Type of the static matrix
void StaticMatrix::checkColumns( const Type& matrix, size_t expectedColumns ) const
{
   if( matrix.columns() != expectedColumns ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid number of columns detected\n"
          << " Details:\n"
          << "   Number of columns         : " << matrix.columns() << "\n"
          << "   Expected number of columns: " << expectedColumns << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking the capacity of the given static matrix.
//
// \param matrix The static matrix to be checked.
// \param minCapacity The expected minimum capacity of the static matrix.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the capacity of the given static matrix. In case the actual capacity
// is smaller than the given expected minimum capacity, a \a std::runtime_error exception is
// thrown.
*/
template< typename Type >  // Type of the static matrix
void StaticMatrix::checkCapacity( const Type& matrix, size_t minCapacity ) const
{
   if( matrix.capacity() < minCapacity ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid capacity detected\n"
          << " Details:\n"
          << "   Capacity                 : " << matrix.capacity() << "\n"
          << "   Expected minimum capacity: " << minCapacity << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking the total number of non-zero elements of the given static matrix.
//
// \param matrix The static matrix to be checked.
// \param expectedNonZeros The expected number of non-zero elements of the static matrix.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the total number of non-zero elements of the given static matrix.
// In case the actual number of non-zero elements does not correspond to the given expected
// number, a \a std::runtime_error exception is thrown.
*/
template< typename Type >  // Type of the static matrix
void StaticMatrix::checkNonZeros( const Type& matrix, size_t expectedNonZeros ) const
{
   if( matrix.nonZeros() != expectedNonZeros ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid number of non-zero elements\n"
          << " Details:\n"
          << "   Number of non-zeros         : " << matrix.nonZeros() << "\n"
          << "   Expected number of non-zeros: " << expectedNonZeros << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( matrix.capacity() < matrix.nonZeros() ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid capacity detected\n"
          << " Details:\n"
          << "   Number of non-zeros: " << matrix.nonZeros() << "\n"
          << "   Capacity           : " << matrix.capacity() << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************


//*************************************************************************************************
/*!\brief Checking the number of non-zero elements in a specific row/column of the given static matrix.
//
// \param matrix The static matrix to be checked.
// \param index The row/column to be checked.
// \param expectedNonZeros The expected number of non-zero elements in the specified row/column.
// \return void
// \exception std::runtime_error Error detected.
//
// This function checks the number of non-zero elements in the specified row/column of the
// given static matrix. In case the actual number of non-zero elements does not correspond
// to the given expected number, a \a std::runtime_error exception is thrown.
*/
template< typename Type >  // Type of the static matrix
void StaticMatrix::checkNonZeros( const Type& matrix, size_t index, size_t expectedNonZeros ) const
{
   if( matrix.nonZeros( index ) != expectedNonZeros ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid number of non-zero elements in "
          << ( blaze::IsRowMajorMatrix<Type>::value ? "row " : "column " ) << index << "\n"
          << " Details:\n"
          << "   Number of non-zeros         : " << matrix.nonZeros( index ) << "\n"
          << "   Expected number of non-zeros: " << expectedNonZeros << "\n";
      throw std::runtime_error( oss.str() );
   }

   if( matrix.capacity( index ) < matrix.nonZeros( index ) ) {
      std::ostringstream oss;
      oss << " Test: " << test_ << "\n"
          << " Error: Invalid capacity detected in "
          << ( blaze::IsRowMajorMatrix<Type>::value ? "row " : "column " ) << index << "\n"
          << " Details:\n"
          << "   Number of non-zeros: " << matrix.nonZeros( index ) << "\n"
          << "   Capacity           : " << matrix.capacity( index ) << "\n";
      throw std::runtime_error( oss.str() );
   }
}
//*************************************************************************************************




//=================================================================================================
//
//  GLOBAL TEST FUNCTIONS
//
//=================================================================================================

//*************************************************************************************************
/*!\brief Testing the functionality of the StaticMatrix class template.
//
// \return void
*/
void runTest()
{
   StaticMatrix();
}
//*************************************************************************************************




//=================================================================================================
//
//  MACRO DEFINITIONS
//
//=================================================================================================

//*************************************************************************************************
/*! \cond BLAZE_INTERNAL */
/*!\brief Macro for the execution of the StaticMatrix test.
*/
#define RUN_STATICMATRIX_TEST \
   blazetest::mathtest::staticmatrix::runTest()
/*! \endcond */
//*************************************************************************************************

} // namespace staticmatrix

} // namespace mathtest

} // namespace blazetest

#endif
