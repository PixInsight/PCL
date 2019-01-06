//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/MorphologicalTransformation.h - Released 2018-12-12T09:24:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#ifndef __PCL_MorphologicalTransformation_h
#define __PCL_MorphologicalTransformation_h

/// \file pcl/MorphologicalTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AutoPointer.h>
#include <pcl/InterlacedTransformation.h>
#include <pcl/MorphologicalOperator.h>
#include <pcl/ParallelProcess.h>
#include <pcl/StructuringElement.h>
#include <pcl/ThresholdedTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MorphologicalTransformation
 * \brief Generic n-way morphological transformation.
 *
 * ### TODO: Write a detailed description for %MorphologicalTransformation.
 *
 * \sa MorphologicalOperator, StructuringElement
 */
class PCL_CLASS MorphologicalTransformation : public InterlacedTransformation,
                                              public ThresholdedTransformation,
                                              public ParallelProcess
{                          // N.B. ImageTransformation is a virtual base class
public:

   /*!
    * Constructs a default %MorphologicalTransformation object.
    *
    * \note This constructor creates an uninitialized instance. In order to use
    * this object, it must be associated with a particular operator and
    * structuring element by calling the SetOperator() and SetStructure()
    * member functions, respectively, with the appropriate instances of
    * MorphologicalOperator and StructuringElement.
    */
   MorphologicalTransformation() = default;

   /*!
    * Constructs a %MorphologicalTransformation object with the specified
    * operator and structuring element.
    *
    * The specified objects don't have to remain valid while this
    * %MorphologicalTransformation instance is actively used, since it will
    * own private copies of the specified filter operator and structure.
    */
   MorphologicalTransformation( const MorphologicalOperator& op, const StructuringElement& structure )
   {
      m_operator = op.Clone();
      m_structure = structure.Clone();
      m_structure->Initialize();
   }

   /*!
    * Copy constructor.
    */
   MorphologicalTransformation( const MorphologicalTransformation& x ) :
      InterlacedTransformation( x ),
      ThresholdedTransformation( x ),
      ParallelProcess( x )
   {
      if ( !x.m_operator.IsNull() )
         if ( !x.m_structure.IsNull() )
         {
            m_operator = x.m_operator->Clone();
            m_structure = x.m_structure->Clone();
         }
   }

   /*!
    * Move constructor.
    */
   MorphologicalTransformation( MorphologicalTransformation&& x ) = default;

   /*!
    * Destroys a %MorphologicalTransformation object.
    */
   virtual ~MorphologicalTransformation()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MorphologicalTransformation& operator =( const MorphologicalTransformation& x )
   {
      if ( &x != this )
      {
         (void)InterlacedTransformation::operator =( x );
         (void)ThresholdedTransformation::operator =( x );
         (void)ParallelProcess::operator =( x );
         if ( !x.m_operator.IsNull() && !x.m_structure.IsNull() )
         {
            m_operator = x.m_operator->Clone();
            m_structure = x.m_structure->Clone();
         }
         else
         {
            m_operator.Destroy();
            m_structure.Destroy();
         }
      }
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MorphologicalTransformation& operator =( MorphologicalTransformation&& ) = default;

   /*!
    * Returns a reference to the morphological operator associated with this
    * transformation.
    */
   const MorphologicalOperator& Operator() const
   {
      PCL_PRECONDITION( !m_operator.IsNull() )
      return *m_operator;
   }

   /*!
    * Causes this transformation to use a duplicate of the specified
    * morphological operator.
    */
   void SetOperator( const MorphologicalOperator& op )
   {
      m_operator = op.Clone();
   }

   /*!
    * Returns a reference to the structuring element associated with this
    * transformation.
    */
   const StructuringElement& Structure() const
   {
      PCL_PRECONDITION( !m_structure.IsNull() )
      return *m_structure;
   }

   /*!
    * Causes this transformation to use a duplicate of the specified
    * structuring element.
    */
   void SetStructure( const StructuringElement& structure )
   {
      m_structure = structure.Clone();
      m_structure->Initialize();
   }

   /*!
    * Returns the size in pixels of the overlapping regions between adjacent
    * areas processed by parallel execution threads. The overlapping distance
    * is a function of the size of the associated structuring element and the
    * current interlacing distance.
    */
   int OverlappingDistance() const
   {
      PCL_PRECONDITION( !m_structure.IsNull() )
      return m_structure->Size() + (m_structure->Size() - 1)*(InterlacingDistance() - 1);
   }

protected:

   AutoPointer<MorphologicalOperator> m_operator;  // morphological operator
   AutoPointer<StructuringElement>    m_structure; // n-way structuring element

   /*
    * In-Place 2-D Morphological Transformation Algorithm.
    */
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;

private:

   void Validate() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MorphologicalTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/MorphologicalTransformation.h - Released 2018-12-12T09:24:21Z
