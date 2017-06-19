//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/GroupBox.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_GroupBox_h
#define __PCL_GroupBox_h

/// \file pcl/GroupBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Control.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GroupBox
 * \brief Client-side interface to a PixInsight %GroupBox control
 *
 * ### TODO: Write a detailed description for %GroupBox.
 */
class PCL_CLASS GroupBox : public Control
{
public:

   /*!
    * Constructs a %GroupBox object with the specified \a title, as a child
    * control of \a parent.
    */
   GroupBox( const String& title = String(), Control& parent = Control::Null() );

   /*!
    * Destroys a %GroupBox object.
    */
   virtual ~GroupBox()
   {
   }

   /*! #
    */
   String Title() const;

   /*! #
    */
   void SetTitle( const String& );

   /*! #
    */
   bool HasTitleCheckBox() const;

   /*! #
    */
   void EnableTitleCheckBox( bool = true );

   /*! #
    */
   void DisableTitleCheckBox( bool disable = true )
   {
      EnableTitleCheckBox( !disable );
   }

   /*! #
    */
   bool IsChecked() const;

   /*! #
    */
   void SetChecked( bool = true );

   /*! #
    */
   void Check()
   {
      SetChecked( true );
   }

   /*! #
    */
   void Uncheck()
   {
      SetChecked( false );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnCheck( GroupBox& sender, bool checked );

   /*! #
    */
   typedef void (Control::*check_event_handler)( GroupBox& sender, bool checked );

   /*! #
    */
   void OnCheck( check_event_handler, Control& );

private:

   struct EventHandlers
   {
      check_event_handler onCheck = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class GroupBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_GroupBox_h

// ----------------------------------------------------------------------------
// EOF pcl/GroupBox.h - Released 2017-06-17T10:55:43Z
