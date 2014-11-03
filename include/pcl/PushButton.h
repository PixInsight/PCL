// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/PushButton.h - Released 2014/10/29 07:34:08 UTC
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

#ifndef __PCL_PushButton_h
#define __PCL_PushButton_h

/// \file pcl/PushButton.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Button_h
#include <pcl/Button.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PushButton
 * \brief Client-side interface to a PixInsight %PushButton control.
 *
 * ### TODO: Write a detailed description for %PushButton
 */
class PCL_CLASS PushButton : public Button
{
public:

   /*!
    * Constructs a %PushButton with the specified \a text and \a icon, as a
    * child control of \a parent.
    */
   PushButton( const String& text = String(),
               const pcl::Bitmap& icon = Bitmap::Null(),
               Control& parent = Control::Null() );

   /*!
    * Destroys a %PushButton control.
    */
   virtual ~PushButton()
   {
   }

   /*!
    * Returns true, since push buttons are pushable buttons.
    */
   virtual bool IsPushable() const
   {
      return true;
   }

   /*!
    * Returns false, since push buttons are not checkable buttons.
    */
   virtual bool IsCheckable() const
   {
      return false;
   }

   /*!
    * Returns true if this button has been selected as the <em>default
    * button</em> of its parent dialog.
    *
    * The default button is activated when the user presses the Return or Enter
    * keys while a modal dialog has the keyboard focus (and these keys are not
    * captured by other sibling controls).
    */
   bool IsDefault() const;

   /*!
    * Selectes this button as the <em>default button</em> of its parent dialog.
    * If this button is not a child control of a modal dialog, caling this
    * member function has no effect.
    */
   void SetDefault( bool = true );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_PushButton_h

// ****************************************************************************
// EOF pcl/PushButton.h - Released 2014/10/29 07:34:08 UTC
