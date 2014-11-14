// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/ViewSelectionDialog.h - Released 2014/11/14 17:16:34 UTC
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

#ifndef __PCL_ViewSelectionDialog_h
#define __PCL_ViewSelectionDialog_h

/// \file pcl/ViewSelectionDialog.h

#include <pcl/CheckBox.h>
#include <pcl/Dialog.h>
#include <pcl/PushButton.h>
#include <pcl/ViewList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ViewSelectionDialog
 * \brief A simple dialog box to select an existing view.
 *
 * %ViewSelectionDialog consists of a ViewList control populated with all
 * existing views, and standard OK and Cancel push buttons. This class is a
 * useful helper whenever a process needs access to a user-selected view.
 *
 * \sa PreviewSelectionDialog, Dialog
 */
class ViewSelectionDialog : public Dialog
{
public:

   /*!
    * Constructs a %ViewSelectionDialog object.
    *
    * \param id   If specified and a view exists with this identifier, the
    *             dialog will select the corresponding view upon execution.
    *
    * \param allowPreviews    Whether to allow selection of previews, along
    *             with main views. For preview-only selections, see the
    *             PreviewSelectionDialog class. The default value is true.
    */
   ViewSelectionDialog( const String& id = String(), bool allowPreviews = true );

   /*!
    * Returns the identifier of the selected view.
    */
   const String& Id() const
   {
      return m_id;
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

   String m_id;
   bool   m_allowPreviews : 1;

   VerticalSizer  Global_Sizer;
      ViewList          Images_ViewList;
      CheckBox          IncludeMainViews_CheckBox;
      CheckBox          IncludePreviews_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void __ViewSelected( ViewList& sender, View& view );
   void __OptionClick( Button& sender, bool checked );
   void __ButtonClick( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ViewSelectionDialog_h

// ****************************************************************************
// EOF pcl/ViewSelectionDialog.h - Released 2014/11/14 17:16:34 UTC
