//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0355
// ----------------------------------------------------------------------------
// STFAutoStretchAction.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "STFAutoStretchAction.h"
#include "ScreenTransferFunctionInterface.h"

#include <pcl/ButtonCodes.h>
#include <pcl/ImageWindow.h>
#include <pcl/KeyCodes.h>
#include <pcl/MessageBox.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

STFAutoStretchActionBase::STFAutoStretchActionBase( bool boost, const String& menuItem, const Bitmap& icon ) :
   Action( menuItem, icon ),
   m_boost( boost )
{
}

void STFAutoStretchActionBase::Execute()
{
   if ( TheScreenTransferFunctionInterface != 0 )
   {
      View view = ImageWindow::ActiveWindow().CurrentView();
      if ( view.IsNull() )
      {
         MessageBox( "Cannot apply STF AutoStretch because there is no view available.",
                     "ScreenTransferFunction",
                     StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }

      view.Window().BringToFront();
      view.Window().SelectView( view );

      if ( TheScreenTransferFunctionInterface->IsVisible() )
         TheScreenTransferFunctionInterface->ActivateTrackView();

      TheScreenTransferFunctionInterface->ComputeAutoStretch( view, m_boost );
      TheScreenTransferFunctionInterface->ApplyTo( view );
   }
}

bool STFAutoStretchActionBase::IsEnabled( ActionInfo info ) const
{
   return info.isImage; // every view can be screen stretched
}

// ----------------------------------------------------------------------------

STFAutoStretchAction::STFAutoStretchAction() :
   STFAutoStretchActionBase( false,
                             "Image > STF AutoStretch",
                             Bitmap( Control::Null().ScaledResource( ":/toolbar/image-stf-auto.png" ) ) )
{
   SetAccelerator( KeyModifier::Control, KeyCode::A );
   SetToolTip( "STF AutoStretch" );
}

STFAutoStretchBoostAction::STFAutoStretchBoostAction() :
   STFAutoStretchActionBase( true,
                             "Image > STF AutoStretch (boosted)",
                             Bitmap( Control::Null().ScaledResource( ":/toolbar/image-stf-auto-boost.png" ) ) )
{
   SetToolTip( "STF AutoStretch (Boosted)" );
}

// ----------------------------------------------------------------------------

STFAutoStretchToolBarActionBase::STFAutoStretchToolBarActionBase( bool boost, const Bitmap& icon, const String& toolBar ) :
   Action( String(), icon, toolBar ),
   m_boost( boost )
{
}

void STFAutoStretchToolBarActionBase::Execute()
{
   if ( TheScreenTransferFunctionInterface != 0 )
   {
      bool withControlOrCmdPressed =
#ifdef __PCL_MACOSX
      IsCmdPressed();
#else
      IsControlPressed();
#endif
      bool withShiftPressed = !withControlOrCmdPressed && IsShiftPressed();

      View view = ImageWindow::ActiveWindow().CurrentView();
      if ( view.IsNull() )
      {
         MessageBox( "Cannot apply STF AutoStretch because there is no view available.",
                     "ScreenTransferFunction",
                     StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }

      view.Window().BringToFront();
      view.Window().SelectView( view );

      if ( TheScreenTransferFunctionInterface->IsVisible() )
         TheScreenTransferFunctionInterface->ActivateTrackView();

      if ( withControlOrCmdPressed )
         TheScreenTransferFunctionInterface->LinkRGBChannels( false );
      else if ( withShiftPressed )
         TheScreenTransferFunctionInterface->LinkRGBChannels( true );

      TheScreenTransferFunctionInterface->ComputeAutoStretch( view, m_boost );
      TheScreenTransferFunctionInterface->ApplyTo( view );
   }
}

bool STFAutoStretchToolBarActionBase::IsEnabled( ActionInfo info ) const
{
   return info.isImage; // every view can be screen stretched
}

String STFAutoStretchToolBarActionBase::AdditionalInformation()
{
   return "<ul><li>"
#ifdef __PCL_MACOSX
          "Cmd-Click"
#else
          "Ctrl-Click"
#endif
          " to apply a different STF to each RGB channel (<b>unlinked</b> mode).</li></ul>"
          "<ul><li>Shift-Click to apply a single STF to all RGB channels (<b>linked</b> mode).</li></ul>"
          "<ul><li>Click to apply current STF tool settings.</li></ul>";
}

// ----------------------------------------------------------------------------

STFAutoStretchToolBarAction::STFAutoStretchToolBarAction() :
   STFAutoStretchToolBarActionBase( false,
                                    Bitmap( Control::Null().ScaledResource( ":/toolbar/image-stf-auto.png" ) ),
                                    "> Screen Transfer Functions" ) // > = add separator
{
   SetToolTip( "<p>STF AutoStretch</p>" + AdditionalInformation() );
}

STFAutoStretchToolBarBoostAction::STFAutoStretchToolBarBoostAction() :
   STFAutoStretchToolBarActionBase( true,
                                    Bitmap( Control::Null().ScaledResource( ":/toolbar/image-stf-auto-boost.png" ) ),
                                    "Screen Transfer Functions" )
{
   SetToolTip( "<p>STF AutoStretch (Boosted)</p>" + AdditionalInformation() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF STFAutoStretchAction.cpp - Released 2016/02/21 20:22:43 UTC
