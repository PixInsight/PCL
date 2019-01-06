//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/MultiViewSelectionDialog.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_MultiViewSelectionDialog_h
#define __PCL_MultiViewSelectionDialog_h

/// \file pcl/MultiViewSelectionDialog.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/TreeBox.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MultiViewSelectionDialog
 * \brief A simple dialog box to select a set of existing views.
 *
 * %MultiViewSelectionDialog consists of a TreeBox control, populated with all
 * existing views, plus standard OK and Cancel push buttons. This class is a
 * useful helper whenever a process needs access to several user-selected views
 * in a single operation.
 *
 * \sa ViewSelectionDialog, PreviewSelectionDialog, Dialog
 */
class MultiViewSelectionDialog : public Dialog
{
public:

   /*!
    * Constructs a %MultiViewSelectionDialog object.
    *
    * \param allowPreviews    Whether to allow selection of previews, along
    *                         with main views. The default value is true.
    */
   MultiViewSelectionDialog( bool allowPreviews = true );

   /*!
    * Destroys a %MultiViewSelectionDialog object.
    */
   virtual ~MultiViewSelectionDialog()
   {
   }

   /*!
    * Returns the set of selected views.
    */
   const Array<View>& Views() const
   {
      return m_selectedViews;
   }

   /*!
    * Returns true if this dialog allows selection of previews along with main
    * views; false if it can only select main views. This option can be
    * controlled with a constructor parameter.
    */
   bool PreviewsAllowed() const
   {
      return m_allowPreviews;
   }

private:

   Array<View> m_selectedViews;
   bool        m_allowPreviews : 1;

   void Regenerate();

   VerticalSizer  Global_Sizer;
      TreeBox           Views_TreeBox;
      HorizontalSizer   Row2_Sizer;
         PushButton        SelectAll_PushButton;
         PushButton        UnselectAll_PushButton;
         CheckBox          IncludeMainViews_CheckBox;
         CheckBox          IncludePreviews_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void OptionClick( Button& sender, bool checked );
   void ButtonClick( Button& sender, bool checked );
   void ControlShow( Control& sender );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MultiViewSelectionDialog_h

// ----------------------------------------------------------------------------
// EOF pcl/MultiViewSelectionDialog.h - Released 2018-12-12T09:24:21Z
