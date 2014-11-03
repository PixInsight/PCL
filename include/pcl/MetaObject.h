// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/MetaObject.h - Released 2014/10/29 07:34:11 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_MetaObject_h
#define __PCL_MetaObject_h

/// \file pcl/MetaObject.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Array_h
#include <pcl/Array.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MetaObject
 * \brief Root base class for all PixInsight module components.
 *
 * In the PCL, a \e metaobject is a special class describing in formal terms
 * the functionality and properties of a \e component in a PixInsight module.
 *
 * The %MetaObject class provides basic functionality to implement the existing
 * hierarchical relationships between a module and all of its components. A
 * %MetaObject contains a list of %MetaObject instances, which generates a tree
 * structure based on parent-children container relations. This structural
 * layout has been used in the PCL to automatize communication between the core
 * PixInsight application, modules, and all module components.
 *
 * \sa MetaModule, MetaProcess, MetaParameter, MetaFileFormat
 */
class PCL_CLASS MetaObject
{
public:

   /*!
    * A container used to implement the list of children module components in
    * %MetaObject.
    */
   typedef IndirectArray<MetaObject>   children_list;

   /*!
    * Constructs a %MetaObject as a child of the specified parent object \a p.
    */
   MetaObject( MetaObject* p ) : parent( p ), children()
   {
      if ( parent != 0 )
         parent->children.Add( this );
   }

   /*!
    * Destroys this %MetaObject and all of its children module components.
    */
   virtual ~MetaObject()
   {
      children.Destroy();
   }

   /*!
    * Returns the number of module components that depend on this %MetaObject.
    */
   size_type Length() const
   {
      return children.Length();
   }

   /*!
    * Returns a pointer to the parent %MetaObject.
    */
   const MetaObject* Parent() const
   {
      return parent;
   }

   MetaObject* Parent()
   {
      return parent;
   }

   /*!
    * Returns a pointer to a %MetaObject children module component, selected by
    * its array index \a i.
    */
   const MetaObject* operator[]( size_type i ) const
   {
      return children[i];
   }

protected:

   MetaObject*        parent;
   children_list children;

   // MetaObject instances are unique objects by definition.
   MetaObject( const MetaObject& ) { PCL_CHECK( 0 ) }
   void operator =( const MetaObject& ) { PCL_CHECK( 0 ) }

   virtual void PerformAPIDefinitions() const = 0;

   friend class MetaModule;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_MetaObject_h

// ****************************************************************************
// EOF pcl/MetaObject.h - Released 2014/10/29 07:34:11 UTC
