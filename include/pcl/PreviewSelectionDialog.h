//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/PreviewSelectionDialog.h - Released 2015/07/30 17:15:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PreviewSelectionDialog_h
#define __PreviewSelectionDialog_h

/// \file pcl/PreviewSelectionDialog.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/ViewList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PreviewSelectionDialog
 * \brief A simple dialog box to select an existing preview.
 *
 * %PreviewSelectionDialog consists of a ViewList control populated with all
 * existing previews, and standard OK and Cancel push buttons. This class is a
 * useful helper whenever a process needs access to a user-selected preview.
 *
 * \sa ViewSelectionDialog, MultiViewSelectionDialog, Dialog
 */
class PreviewSelectionDialog : public Dialog
{
public:

   /*!
    * Constructs a %PreviewSelectionDialog object. If the optional \a id
    * argument is specified and a preview exists with that identifier, the
    * dialog will select the corresponding preview upon execution.
    */
   PreviewSelectionDialog( const IsoString& id = IsoString() );

   template <class S>
   PreviewSelectionDialog( const S& id ) :
      PreviewSelectionDialog( IsoString( id ) )
   {
   }

   /*!
    * Destroys a %PreviewSelectionDialog object.
    */
   virtual ~PreviewSelectionDialog()
   {
   }

   /*!
    * Returns the identifier of the selected preview.
    */
   IsoString Id() const
   {
      return m_id;
   }

private:

   IsoString m_id;

   VerticalSizer  Global_Sizer;
      ViewList          Images_ViewList;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void ViewSelected( ViewList& sender, View& view );
   void ButtonClick( Button& sender, bool checked );
   void ControlShow( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PreviewSelectionDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/PreviewSelectionDialog.h - Released 2015/07/30 17:15:18 UTC
