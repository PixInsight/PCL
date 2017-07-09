//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/MorphologicalTransformation.h - Released 2017-07-09T18:07:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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
                                              public ThresholdedTransformation
{                          // NB: ImageTransformation is a virtual base class
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
   MorphologicalTransformation() :
      InterlacedTransformation(), ThresholdedTransformation(),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   /*!
    * Constructs a %MorphologicalTransformation object with the specified
    * operator and structuring element.
    *
    * The specified objects don't have to remain valid while this
    * %MorphologicalTransformation instance is actively used, since it will
    * own private copies of the specified filter operator and structure.
    */
   MorphologicalTransformation( const MorphologicalOperator& op, const StructuringElement& structure ) :
      InterlacedTransformation(), ThresholdedTransformation(),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
      m_operator = op.Clone();
      m_structure = structure.Clone();
      m_structure->Initialize();
   }

   /*!
    * Copy constructor.
    */
   MorphologicalTransformation( const MorphologicalTransformation& x ) :
      InterlacedTransformation( x ), ThresholdedTransformation( x ),
      m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
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
   MorphologicalTransformation( MorphologicalTransformation&& x ) :
      InterlacedTransformation( x ), ThresholdedTransformation( x ),
      m_operator( x.m_operator ), m_structure( x.m_structure ),
      m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
      //x.m_operator = nullptr;  // already done by AutoPointer
      //x.m_structure = nullptr;
   }

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
         m_parallel = x.m_parallel;
         m_maxProcessors = x.m_maxProcessors;
      }
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MorphologicalTransformation& operator =( MorphologicalTransformation&& x )
   {
      if ( &x != this )
      {
         (void)InterlacedTransformation::operator =( x );
         (void)ThresholdedTransformation::operator =( x );
         m_operator = x.m_operator;
         m_structure = x.m_structure;
         m_parallel = x.m_parallel;
         m_maxProcessors = x.m_maxProcessors;
         //x.m_operator = nullptr;  // already done by AutoPointer
         //x.m_structure = nullptr;
      }
      return *this;
   }

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

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of
    * %MorphologicalTransformation.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %MorphologicalTransformation. If \a enable is
    *                false this parameter is ignored. A value <= 0 is ignored.
    *                The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of
    * %MorphologicalTransformation.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %MorphologicalTransformation.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %MorphologicalTransformation.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   AutoPointer<MorphologicalOperator> m_operator;          // morphological operator
   AutoPointer<StructuringElement>    m_structure;         // n-way structuring element
   bool                               m_parallel      : 1; // use multiple execution threads
   unsigned                           m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT; // Maximum number of processors allowed

   /*
    * In-Place 2-D Morphological Transformation Algorithm.
    */
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;

private:

   void Validate() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MorphologicalTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/MorphologicalTransformation.h - Released 2017-07-09T18:07:07Z
