// ****************************************************************************
// PixInsight Class Library - PCL 02.00.14.0695
// Standard CometAlignment Process Module Version 01.02.03.0066
// ****************************************************************************
// CometAlignmentInterface.cpp - Released 2015/02/10 19:50:08 UTC
// ****************************************************************************
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "CometAlignmentInterface.h"
#include "CometAlignmentProcess.h"
#include "StarDetector.h"

#include <pcl/FileDialog.h>
#include <pcl/StdStatus.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Graphics.h>

#define IMAGELIST_MINHEIGHT( fnt )  (8*fnt.Height() + 2)

namespace pcl
{
#include "CometAlignmentIcon.xpm"

#define screenRadius 50   // Radius in screen pixels. DynamicPaint big green circle around coma center.
#define imageRadius 4   // Radius in image pixels. DynamicPaint small green circle around coma center.
DPoint posInImage; // star position in image. For DynamicPaint.

// ----------------------------------------------------------------------------
CometAlignmentInterface* TheCometAlignmentInterface = 0;

CometAlignmentInterface::CometAlignmentInterface () : ProcessInterface (), m_instance (TheCometAlignmentProcess),
GUI (0)

{
   TheCometAlignmentInterface = this;
   DisableAutoSaveGeometry ();
}

CometAlignmentInterface::~CometAlignmentInterface ()

{
   if (GUI != 0) delete GUI, GUI = 0;
}

IsoString
CometAlignmentInterface::Id () const

{
   return "CometAlignment";
}

MetaProcess*
CometAlignmentInterface::Process () const

{
   return TheCometAlignmentProcess;
}

const char** CometAlignmentInterface::IconImageXPM() const
{
   return CometAlignmentIcon_XPM;
}

InterfaceFeatures
CometAlignmentInterface::Features () const

{
   return InterfaceFeature::DefaultGlobal; // | CometAlignmentInterfaceFeature::RealTimeButton;
}

void
CometAlignmentInterface::ApplyInstance () const

{
   m_instance.LaunchOnCurrentView ();
}
// ----------------------------------------------------------------------------

void
CometAlignmentInterface::ResetInstance ()

{
   CometAlignmentInstance defaultInstance (TheCometAlignmentProcess);
   ImportProcess (defaultInstance);
}

bool
CometAlignmentInterface::Launch (const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/)

{
   // ### Deferred initialization
   if (GUI == 0)

   {
      GUI = new GUIData (*this);
      SetWindowTitle ("CometAlignment");
      UpdateControls ();

      // Restore position only
      if (!RestoreGeometry ())
         SetDefaultPosition ();
      AdjustToContents ();
   }

   dynamic = true;
   return &P == TheCometAlignmentProcess;
}

ProcessImplementation*
CometAlignmentInterface::NewProcess () const

{
   return new CometAlignmentInstance (m_instance);
}

bool
CometAlignmentInterface::ValidateProcess (const ProcessImplementation& p, pcl::String& whyNot) const

{
   const CometAlignmentInstance* r = dynamic_cast<const CometAlignmentInstance*> (&p);
   if (r == 0)

   {
      whyNot = "Not an CometAlignment instance.";
      return false;
   }

   whyNot.Clear ();
   return true;
}

bool
CometAlignmentInterface::RequiresInstanceValidation () const

{
   return true;
}

bool
CometAlignmentInterface::ImportProcess (const ProcessImplementation& p)

{
   m_instance.Assign (p);

   UpdateControls ();
   return true;
}

bool
CometAlignmentInterface::IsDynamicInterface () const

{
   return true;
}

void
CometAlignmentInterface::DynamicMousePress (View& view, const DPoint& pos, int button, unsigned buttons, unsigned modifiers)

{
   if (m_instance.p_targetFrames.IsEmpty ()) return; // No images in p_targetFrames list
   const String path = view.Window ().FilePath (); // extract filePath from View
   if (path != m_path0 && path != m_path1) return; // ignore Views which is not First or Last image in our p_targetFrames

#if debug
   Console ().WriteLn ("<br>DynamicMousePress() on " + path);
#endif

   DPoint p (pos); // Mouse click x,y position
   if (!(modifiers == KeyModifier::Control)) // modifiers == Ctrl. Disable centroid detection via KeyBoard Ctrl

   {
      ImageWindow::display_channel channel = view.Window ().CurrentChannel ();
      ImageVariant img = view.Image ();

      String message;

      for (int ch = 0; ch < img.AnyImage ()->NumberOfNominalChannels (); ++ch)

      {
         switch (channel)

         {
         case DisplayChannel::Red:
            if (ch != 0)
               continue;
            break;
         case DisplayChannel::Green:
            if (ch != 1)
               continue;
            break;
         case DisplayChannel::Blue:
            if (ch != 2)
               continue;
            break;
         default:
            break;
         }

         int searchRadius = 8;
         float bkgThreshold = 1; // star signal( bkgThreshold * StandardDeviation ) above surround median
         bool autoAperture = true;

         StarDetector D (img, ch, pos, searchRadius, bkgThreshold, autoAperture); // Call Centroid Detection

         Console ().WriteLn ("Channel:" + img.AnyImage ()->ChannelId (ch) + " " + D.star.StatusToString ().UpperCase ());
         if (D) // status ?

         {
            p = D.star.pos; // DetectedOk
            break;
         }
         else Console ().Show (); // for show warning.
      }
   }

   if (!view.IsMainView ()) // the View is PreView ?
      p.MoveBy (view.Window ().PreviewRect (view.Id ()).LeftTop ()); // calculate click position(x,y) in mainView

   if (path == m_path0) SetFirst (p); // Store star position(x,y) in First image
   else if (path == m_path1) SetLast (p); // Store star position(x,y) in Last image

   view.Window ().UpdateViewport (); // Call RequiresDynamicUpdate()
#if debug
   Console ().WriteLn (String ().Format ("Pos X:%f, Y:%f", p.x, p.y));
   Console ().WriteLn ("DynamicMousePress() Finish");
#endif
}

Rect
ViewportSourceSampleRect (const DPoint pos, const ImageWindow w)

{
   Point p = pos.Truncated ();
   Rect ssr (p - imageRadius, p + imageRadius + 1);

   DPoint c1 (int( pos.x) + 0.5, int( pos.y) + 0.5);
   w.ImageToViewport (c1.x, c1.y);
   Point c0 (int( c1.x), int( c1.y));

   Rect vsr = w.ImageToViewport (ssr);
   --vsr.x0;
   --vsr.y0;
   ++vsr.x1;
   ++vsr.y1; // rounding error compensation
   vsr.Unite (Rect (c0 - screenRadius, c0 + screenRadius + 1));
   return vsr;
}

bool
CometAlignmentInterface::RequiresDynamicUpdate (const View& v, const DRect& updateRect) const

{
   if (m_instance.p_targetFrames.IsEmpty ()) return false;
   ImageWindow w = v.Window ();
   const String path = w.FilePath (); // extract filePath from View
   for (size_t i = 0; i < m_instance.p_targetFrames.Length (); i++) // search the file in targets

   {
      if (m_instance.p_targetFrames[i].path != path) continue; // not equal, try next target

      posInImage.MoveTo (m_instance.p_targetFrames[i].x, m_instance.p_targetFrames[i].y); // get star position from GUI
      if (posInImage.x == 0 && posInImage.y == 0) return false; // star position is not selected by user, so no DynamicPaint graphics in the View

#if debug
      Console ().Clear ();
      Console ().WriteLn (String ().Format ("updateRect"));
      Console ().WriteLn (String ().Format ("x0:%10.5f, x1:%10.5f, w:%10.5f", updateRect.x0, updateRect.x1, updateRect.Width ()));
      Console ().WriteLn (String ().Format ("y0:%10.5f, y1:%10.5f, h:%10.5f", updateRect.y0, updateRect.y1, updateRect.Height ()));
#endif

      Rect r = w.ImageToViewport (updateRect);
      --r.x0;
      --r.y0;
      ++r.x1;
      ++r.y1; // rounding error compensation

      if (r.Intersects (ViewportSourceSampleRect (posInImage, w))) // check: is ViewUpdateRect and DynamicPaint graphics Intersects ?
         return true; // Yes, Intersects. Requires update DynamicPaint
   }
   return false; // no such View in p_targetFrames, so no need to update
}

void
CometAlignmentInterface::DynamicPaint (const View& v, Graphics& g, const DRect& updateRect) const

{
   ImageWindow w = v.Window ();

   Rect r0 = w.ImageToViewport (updateRect);

#if debug
   Console ().Clear ();
   Console ().WriteLn (String ().Format ("updateRect"));
   Console ().WriteLn (String ().Format ("x0:%10.5f, x1:%10.5f, w:%10.5f", updateRect.x0, updateRect.x1, updateRect.Width ()));
   Console ().WriteLn (String ().Format ("y0:%10.5f, y1:%10.5f, h:%10.5f", updateRect.y0, updateRect.y1, updateRect.Height ()));
   Console ().WriteLn (String ().Format ("updateRect in View"));
   Console ().WriteLn (String ().Format ("x0:%i, x1:%i, w:%i", r0.x0, r0.x1, r0.Width ()));
   Console ().WriteLn (String ().Format ("y0:%i, y1:%i, h:%i", r0.y0, r0.y1, r0.Height ()));
#endif

   --r0.x0;
   --r0.y0;
   ++r0.x1;
   ++r0.y1; // rounding error compensation
   Bitmap bmp1 = w.ViewportBitmap (r0); // Working bitmap
   Point p0 = r0.LeftTop (); // Origin of the local bitmap coordinate system in viewport coordinates.

   Graphics G;
   G.BeginPaint (bmp1);

   G.SetPen (0xff00ff00); // Green
   G.SetBrush (Brush::Null ()); // no brush.

   Point pv0 = w.ImageToViewport (posInImage); // star center in image trsnslated to view coordinates
   pv0 -= p0; // star center in updateRect viewport
   G.DrawCircle (pv0, screenRadius); // draw on screen graphics, non scalable with image

   // draw on image graphics, scalsable with image
   DRect r (posInImage.x - imageRadius, posInImage.y - imageRadius, posInImage.x + imageRadius, posInImage.y + imageRadius); // small rectangle around star in image coordinates
   Rect rv = w.ImageToViewport (r); // small rectangle around star scaled to viewport coordinates
   if (!rv.IsPoint ()) // if rectange wider then one pixel

   {
      rv -= p0;
      G.DrawCircle (pv0, rv.Width ()*.5);
   }

   G.EndPaint ();
   g.DrawBitmap (p0, bmp1);
}


//-------------------------------------------------------------------------

void
CometAlignmentInterface::SaveSettings () const

{ //at close PixInsight
   SaveGeometry ();
}

//-------------------------------------------------------------------------

void
FileShow (String& path) // load ( if not loaded before ) and bring to front the ImageWindow

{
#if debug
   Console ().WriteLn ("FileShow():" + path);
#endif
   if (ImageWindow::WindowByFilePath (path).IsNull ())

   {
      Array<ImageWindow> w = ImageWindow::Open (path);
      for (Array<ImageWindow>::iterator i = w.Begin (); i != w.End (); ++i)
         i->Show ();
   }
   else
      ImageWindow::WindowByFilePath (path).BringToFront ();
}

void
CometAlignmentInterface::SetFirst (const DPoint pos)

{
#if debug
   Console ().WriteLn ("Set First");
#endif

   m_instance.p_targetFrames.Begin ()->x = pos.x;
   m_instance.p_targetFrames.Begin ()->y = pos.y;
   UpdateTargetImagesList ();
}

void
CometAlignmentInterface::SetLast (const DPoint pos)

{
#if debug
   Console ().WriteLn ("Set Last");
#endif

   //m_instance.p_targetFrames.ReverseBegin()->x = pos.x; <- VC++ error C2819
   (*m_instance.p_targetFrames.ReverseBegin ()).x = pos.x;
   (*m_instance.p_targetFrames.ReverseBegin ()).y = pos.y;
   UpdateTargetImagesList ();
}

void
CometAlignmentInterface::GetPoint (DPoint& pos, const double jDate)

{
   double dT = jDate - m_date0; // How much days passed from first exposure
   if (m_DateDelta != 0) // to avoid (x/zero) and when first file == last file
      pos = m_pos0 + m_PosDelta * dT / m_DateDelta; // How much Comet moved( in pixels ) from first image
   else
      pos = m_pos0; // the Comet position = comet position(X,Y) in first image
}

void
CometAlignmentInterface::SetReference (const int index)

{
   if (int(m_instance.p_reference) == index) return;

   GUI->TargetImages_TreeBox.Child (int(m_instance.p_reference))->SetIcon (0, Bitmap ());
   GUI->TargetImages_TreeBox.Child (int(m_instance.p_reference = index))->SetIcon (0, Bitmap (":/icons/arrow-right.png"));

   if (m_DateDelta != 0) UpdateTargetImagesList ();
   else ( GUI->TargetImages_TreeBox.AdjustColumnWidthToContents (0));
}

void
CometAlignmentInterface::UpdateSubtractSection ()
{
   const bool d(m_instance.p_subtractFile.IsEmpty());
   GUI->SubtractMode_CheckBox.Disable(d);
   GUI->Normalize_CheckBox.Disable(d);
   GUI->LinearFit_CheckBox.Disable(d);
   GUI->RejectLow_NumericControl.Disable(d);
   GUI->RejectHigh_NumericControl.Disable(d);
}

void
CometAlignmentInterface::UpdateControls ()

{
   m_pos1 = m_pos0 = 0;
   GUI->InputHints_Edit.SetText( m_instance.p_inputHints );
   GUI->OutputHints_Edit.SetText( m_instance.p_outputHints );
   GUI->OutputDir_Edit.SetText (m_instance.p_outputDir);
   GUI->OutputExtension_Edit.SetText( m_instance.p_outputExtension );
   GUI->Postfix_Edit.SetText (m_instance.p_postfix);
   GUI->Prefix_Edit.SetText (m_instance.p_prefix);

   GUI->Overwrite_CheckBox.SetChecked (m_instance.p_overwrite);
   GUI->PixelInterpolation_ComboBox.SetCurrentItem (m_instance.p_pixelInterpolation);

   GUI->ClampingThreshold_NumericControl.SetValue (m_instance.p_linearClampingThreshold);
   GUI->ClampingThreshold_NumericControl.Enable (
                                                 m_instance.p_pixelInterpolation == CAPixelInterpolation::BicubicSpline ||
                                                 m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos3 ||
                                                 m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos4 ||
                                                 m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos5);
   GUI->SubtractFile_Edit.SetText (m_instance.p_subtractFile);

   GUI->SubtractMode_CheckBox.SetChecked (m_instance.p_subtractMode);
   GUI->Normalize_CheckBox.SetChecked (m_instance.p_normalize);
   GUI->LinearFit_CheckBox.SetChecked (m_instance.p_enableLinearFit);
   GUI->RejectLow_NumericControl.SetValue (m_instance.p_rejectLow);
   GUI->RejectHigh_NumericControl.SetValue (m_instance.p_rejectHigh);

   UpdateSubtractSection();

   UpdateTargetImagesList ();
   UpdateImageSelectionButtons ();
}

void
CometAlignmentInterface::UpdateTargetImageItem (size_type i)

{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[int(i)];
   if (node == 0) return;

   CometAlignmentInstance::ImageItem& item = m_instance.p_targetFrames[i];

#if debug
   Console ().WriteLn ("<br>UpdateTargetImageItem " + String (i) + File::ExtractNameAndSuffix (item.path));
   Console ().WriteLn (String ().Format ("old x:%.3f, y:%.3f", item.x, item.y));
#endif

   if (i == m_instance.p_reference) node->SetIcon (0, Bitmap (":/icons/arrow-right.png"));
   node->SetToolTip (0, "Double Click to set reference");
   node->SetText (0, String (i + 1));
   node->SetAlignment (0, TextAlign::Right);

   node->SetIcon (1, Bitmap (item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png"));
   node->SetToolTip (1, "Double Click to check/uncheck");
   node->SetAlignment (1, TextAlign::Left);

   node->SetIcon (2, Bitmap (":/browser/document.png"));
   if (GUI->FullPaths_CheckBox.IsChecked ()) node->SetText (2, item.path);
   else node->SetText (2, File::ExtractNameAndSuffix (item.path));
   node->SetToolTip (2, "<p>DoubleClick to open file.</p>");
   node->SetAlignment (2, TextAlign::Left);

   node->SetText (3, item.date); // show date in FITS DATE-OBS format

   double dT; // DeltaTime
   dT = item.Jdate - m_date0; // set DeltaTime to how much days passed from first exposure
   DPoint pos = m_pos0; // set the Comet position = comet position(X,Y) in first image
   if (m_DateDelta != 0) // to avoid (x/zero) and when first file == last file
      pos += m_PosDelta * dT / m_DateDelta; // Add Delta; how much Comet moved( in pixels ) from first image

   node->SetText (4, String ().Format ("%.3f", item.x = pos.x));
   node->SetText (5, String ().Format ("%.3f", item.y = pos.y));

   // set DeltaTime to how much days passed from Reference exposure
   dT = item.Jdate - m_instance.p_targetFrames[m_instance.p_reference].Jdate;
   DPoint dPos = 0; // delta position( x, y )
   if (m_DateDelta != 0) // to avoid (x/zero) and when first file == last file
      dPos = m_PosDelta * dT / m_DateDelta; // How much Comet moved in pixels according Reference exposure

   node->SetText (6, String ().Format ("%.1f", dT * 86400)); // 86400 seconds per day
   node->SetText (7, String ().Format ("%.3f", dPos.x));
   node->SetText (8, String ().Format ("%.3f", dPos.y));

   if (!ImageWindow::WindowByFilePath (item.path).IsNull () && (item.x != 0 || item.y != 0)) // update dynamicPaint
      ImageWindow::WindowByFilePath (item.path).UpdateViewport ();


#if debug
   Console ().WriteLn (String ().Format ("new x:%.3f, y:%.3f", item.x, item.y));
#endif

}

void
CometAlignmentInterface::UpdateTargetImagesList ()

{
#if debug
   Console ().WriteLn ("Init TargetImages_TreeBox Start");
#endif

   GUI->TargetImages_TreeBox.Clear ();
   m_pos0.MoveTo (0, 0);
   m_pos1 = m_pos0;
   m_path0.Clear ();
   m_path1.Clear ();

   m_length = m_instance.p_targetFrames.Length ();

   GUI->Parameter_SectionBar.Enable (m_length > 0);
   GUI->TargetImages_TreeBox.ShowHeader (m_length > 0);


   if (m_length != 0)

   {
      // Set m_date0, m_date1, x0,y0,x1,y1, delta( x,y,date ) m_path0, m_path1 -----------------------------------------
      const CometAlignmentInstance::ImageItem* item;

      item = m_instance.p_targetFrames.Begin ();
      m_path0 = item->path;
      m_date0 = item->Jdate;

      m_pos0.MoveTo (item->x, item->y);

      if (m_length > 1) item = m_instance.p_targetFrames.ReverseBegin (); // first and last is not equal

      m_path1 = item->path;
      m_date1 = item->Jdate;
      m_pos1.MoveTo (item->x, item->y);

      m_DateDelta = m_date1 - m_date0;
      m_PosDelta = m_pos1 - m_pos0; // how many X,Y pixels comet passed from first to last image


#if debug
      Console ().WriteLn (File::ExtractNameAndSuffix (m_path0) + String ().Format (" 0: x:%.3f, y:%.3f, jD:%f", m_pos0.x, m_pos0.y, m_date0));
      Console ().WriteLn (File::ExtractNameAndSuffix (m_path1) + String ().Format (" 1: x:%.3f, y:%.3f, jD:%f", m_pos1.x, m_pos1.y, m_date1));
      Console ().WriteLn (String ().Format ("dT:%.1f, dX:%.3f, dY:%.3f", (m_DateDelta * 86400), m_PosDelta.x, m_PosDelta.y));
#endif

      // Create TargetImages_TreeBox --------------------------------------------------------------------------
      GUI->TargetImages_TreeBox.DisableUpdates ();

      for (size_type i = 0; i < m_length; ++i)

      {
         new TreeBox::Node (GUI->TargetImages_TreeBox);
         UpdateTargetImageItem (i);
      }

#if debug
      Console ().WriteLn ();
      Console ().WriteLn (File::ExtractNameAndSuffix (m_path0) + String ().Format (" 0: x:%.3f, y:%.3f, jD:%f", m_pos0.x, m_pos0.y, m_date0));
      Console ().WriteLn (File::ExtractNameAndSuffix (m_path1) + String ().Format (" 1: x:%.3f, y:%.3f, jD:%f", m_pos1.x, m_pos1.y, m_date1));
      Console ().WriteLn (String ().Format ("dT:%.1f, dX:%.3f, dY:%.3f", (m_DateDelta * 86400), m_PosDelta.x, m_PosDelta.y));
#endif

   }

   GUI->x0_NumericEdit.SetValue (m_pos0.x);
   GUI->y0_NumericEdit.SetValue (m_pos0.y);
   GUI->File0_Label.SetText (File::ExtractNameAndSuffix (m_path0)); // first file in list

   GUI->x1_NumericEdit.SetValue (m_pos1.x);
   GUI->y1_NumericEdit.SetValue (m_pos1.y);
   GUI->File1_Label.SetText (File::ExtractNameAndSuffix (m_path1)); // last file in list

   if (m_DateDelta != 0)

   {
      GUI->xDelta_NumericEdit.SetValue (m_PosDelta.x / m_DateDelta / 24); // pixels per hour
      GUI->yDelta_NumericEdit.SetValue (m_PosDelta.y / m_DateDelta / 24);
   }

   // Correcting width of TargetImages_TreeBox -------------------------------------------
   int width = 0;
   const int lastColumn = GUI->TargetImages_TreeBox.NumberOfColumns () - 1;
   GUI->TargetImages_TreeBox.ShowColumn (lastColumn); // temporarry show last column, which uset only for GUI width expansion

   for (int i = 0; i < lastColumn; i++)

   {
      GUI->TargetImages_TreeBox.AdjustColumnWidthToContents (i);
      width += GUI->TargetImages_TreeBox.ColumnWidth (i); // calculate total width of columns except last one
   }

   GUI->TargetImages_TreeBox.SetColumnWidth (lastColumn, 0); // set width of last column to zero and
   GUI->TargetImages_TreeBox.HideColumn (lastColumn); // hide last column to hide horisontal scroling
   GUI->TargetImages_TreeBox.SetMinWidth (width + 20);

   GUI->TargetImages_TreeBox.EnableUpdates ();

   Restyle ();
   AdjustToContents ();

#if debug
   Console ().WriteLn ("Init TargetImages_TreeBox Finish");
#endif
}

void
CometAlignmentInterface::UpdateImageSelectionButtons ()

{
   bool hasItems = GUI->TargetImages_TreeBox.NumberOfChildren () > 0;
   bool hasSelection = hasItems && GUI->TargetImages_TreeBox.HasSelectedTopLevelNodes ();

   GUI->SelectAll_PushButton.Enable (hasItems);
   GUI->InvertSelection_PushButton.Enable (hasItems);
   GUI->Clear_PushButton.Enable (hasItems);

   GUI->ToggleSelected_PushButton.Enable (hasSelection);
   GUI->RemoveSelected_PushButton.Enable (hasSelection);
}


//-------------------------------------------------------------------------------------

void
CometAlignmentInterface::SelectDir ()

{
   GetDirectoryDialog d;
   d.SetCaption ("CometAlignment: Select Output Directory");
   if (d.Execute ())
      GUI->OutputDir_Edit.SetText (m_instance.p_outputDir = d.Directory ());
}

void
CometAlignmentInterface::SelectSubtractFile ()

{
   OpenFileDialog d;
   d.LoadImageFilters ();
   d.DisableMultipleSelections ();
   d.SetCaption ("CometAlignment: Select Comet Integration image");
   if (d.Execute ())

   {
      GUI->SubtractFile_Edit.SetText (m_instance.p_subtractFile = d.FileName ());
   }
   UpdateSubtractSection();
}

inline bool
GetDate (String& date, const String& filePath)

{
   date.Clear ();
   String time;

   String extension = File::ExtractExtension (filePath);
   FileFormat format (extension, false, false);

   // try extract DATE-OBS from FitsKeywords
   if (format.CanStoreKeywords ())

   {
      FileFormatInstance file (format);
      ImageDescriptionArray images;
      if (!file.Open (images, filePath)) throw CatchedException ();
      if (images.IsEmpty ()) throw Error (filePath + ": Empty image.");
      FITSKeywordArray keywords;
      file.Extract (keywords);

      file.Close ();

      for (int k = 0; k < int( keywords.Length ()); k++)

      {
         if (keywords[k].name == "DATE-OBS")
            date = keywords[k].StripValueDelimiters ();
         else if (keywords[k].name == "TIME-OBS" || keywords[k].name == "UT-START" )
            time = keywords[k].StripValueDelimiters ();

         if (date.IsEmpty ()) // no DATE-OBS found
            continue;
         if (date.Length () >= 19) // long format DATE-OBS yyyy-mm-ddThh:mm:ss[.sss] /* ### BUG FIXED 20131220 - support decimal seconds */
            return true;
         if (time.IsEmpty ()) // short DATE-OBS found, but no TIME-OBS found
            continue;
         if (date.Length () == 10) // short format DATE-OBS yyyy-mm-dd and TIME-OBS found
         {
            if(date.SubString (2, 1)=="/") // true == DATE-OBS format dd/mm/yyyy , Iris produce that format
               //convert dd/mm/yyyy to yyyy-mm-dd
               date = date.SubString(6,4)          //yyyy
                       +"-"+date.SubString(3,2)    //mm
                       +"-"+date.SubString(0,2);   //dd

            date = date + "T" + time; // add TIME-OBS to short DATE-OBS
            return true;
         }
         else // wrong format DATE-OBS
            break;
      }//end for in keywords
   }

   Console ().CriticalLn ("*** DATE-OBS keyword not found or invalid: " + filePath);
   return false;
}

void
CometAlignmentInterface::__TargetImages_CurrentNodeUpdated (TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent)

{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex (&current);
   if (index < 0 || size_type (index) >= m_instance.p_targetFrames.Length ())
      throw Error ("CometAlignmentInterface: *Warning* Corrupted interface structures");

   // ### If there's something else that depends on which image is selected in the list, do it here.
}

void
CometAlignmentInterface::__TargetImages_NodeActivated (TreeBox& sender, TreeBox::Node& node, int col)

{
   int index = sender.ChildIndex (&node);
   if (index < 0 || size_type (index) >= m_instance.p_targetFrames.Length ())
      throw Error ("CometAlignmentInterface: *Warning* Corrupted interface structures");

   CometAlignmentInstance::ImageItem& item = m_instance.p_targetFrames[index];

   switch (col)

   {
   case 0:
      // Activate the item's index number: set Reference.
      SetReference (index);
      break;
   case 1:
      // Activate the item's checkmark: toggle item's enabled state.
      item.enabled = !item.enabled;
      UpdateTargetImageItem (index);
      break;
   default:
   case 2:
      // Activate the item's path: open the image.
      FileShow (item.path);
      //break;
   }
}

void
CometAlignmentInterface::__TargetImages_NodeSelectionUpdated (TreeBox& sender)

{
   UpdateImageSelectionButtons ();
}

void
CometAlignmentInterface::__TargetImages_BottonClick (Button& sender, bool checked)

{
   if (sender == GUI->AddFiles_PushButton)

   {
      OpenFileDialog d;
      d.LoadImageFilters ();
      d.EnableMultipleSelections ();
      d.SetCaption ("CometAlignment: Select Target Frames");
      if (d.Execute ())

      {
         Console ().Show ();

         for (StringList::const_iterator i = d.FileNames ().Begin (); i != d.FileNames ().End (); ++i)

         {
            // skip files which already in list
            size_t j = 0;
            for (; j < m_instance.p_targetFrames.Length (); j++)
               if (m_instance.p_targetFrames.At (j)->path == *i) break;
            if (j < m_instance.p_targetFrames.Length ()) continue; // file already in list >> skip file

            String date;
            String time;
            if (!GetDate (date, *i)) // get DATE-OBS from file
               continue; // on error

            // insert the file to sorted list by DATE-OBS
            j = 0;
            for (; j < m_instance.p_targetFrames.Length (); j++)
               if (m_instance.p_targetFrames.At (j)->date.Compare (date) > 0) break;
            m_instance.p_targetFrames.Insert (m_instance.p_targetFrames.At (j), CometAlignmentInstance::ImageItem (*i, date));
         }
         UpdateTargetImagesList ();
         UpdateImageSelectionButtons ();
      }
   }
   else if (sender == GUI->SelectAll_PushButton)

   {
      GUI->TargetImages_TreeBox.SelectAllNodes ();
      UpdateImageSelectionButtons ();
   }
   else if (sender == GUI->InvertSelection_PushButton)

   {
      for (int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren (); i < n; ++i)
         GUI->TargetImages_TreeBox[i]->Select (!GUI->TargetImages_TreeBox[i]->IsSelected ());
      UpdateImageSelectionButtons ();
   }
   else if (sender == GUI->ToggleSelected_PushButton)

   {
      for (int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren (); i < n; ++i)
         if (GUI->TargetImages_TreeBox[i]->IsSelected ())
            m_instance.p_targetFrames[i].enabled = !m_instance.p_targetFrames[i].enabled;
      UpdateTargetImagesList ();
      UpdateImageSelectionButtons ();
   }
   else if (sender == GUI->RemoveSelected_PushButton)

   {
      CometAlignmentInstance::image_list newTargets;
      for (int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren (); i < n; ++i)

      {
         if (!GUI->TargetImages_TreeBox[i]->IsSelected ())

         {
            newTargets.Add (m_instance.p_targetFrames[i]);
            if (i == int( m_instance.p_reference)) // set new reference pointer
               m_instance.p_reference = newTargets.Length () - 1;
         }
      }
      m_instance.p_targetFrames = newTargets;

      if (!m_instance.p_targetFrames.IsEmpty () && m_instance.p_reference >= m_instance.p_targetFrames.Length ()) // reference was removed
         m_instance.p_reference = m_instance.p_targetFrames.Length () - 1; // set reference to last image

      UpdateTargetImagesList ();
      UpdateImageSelectionButtons ();
   }
   else if (sender == GUI->Clear_PushButton)

   {
      m_instance.p_reference = 0;
      m_instance.p_targetFrames.Clear ();
      UpdateTargetImagesList ();
      UpdateImageSelectionButtons ();
   }
   else if (sender == GUI->FullPaths_CheckBox)

   {
      UpdateTargetImagesList ();
      UpdateImageSelectionButtons ();
   }
}

void
CometAlignmentInterface::__ToggleSection (SectionBar& sender, Control& section, bool start)

{
   if (start)

   {
      GUI->TargetImages_TreeBox.SetFixedHeight ();
   }
   else

   {
      GUI->TargetImages_TreeBox.SetMinHeight (IMAGELIST_MINHEIGHT (Font ()));
      GUI->TargetImages_TreeBox.SetMaxHeight (int_max);
   }
}

void
CometAlignmentInterface::__MouseDoubleClick (Control& sender, const Point& pos, unsigned buttons, unsigned modifiers)

{
   if (sender == GUI->OutputDir_Edit)
      SelectDir ();
   else if (sender == GUI->SubtractFile_Edit)

   {
      if (m_instance.p_subtractFile.IsEmpty ())
         SelectSubtractFile ();
      else
         FileShow (m_instance.p_subtractFile);
   }
}

void
CometAlignmentInterface::__EditCompleted (Edit& sender)

{
   if (!sender.IsModified ()) return;
   sender.SetModified (false);

#if debug
   Console ().WriteLn ("__EditCompleted()");
#endif

   String text = sender.Text ().Trimmed ();

   if ( sender == GUI->OutputExtension_Edit )
   {
      if ( !text.IsEmpty() && !text.BeginsWith( '.' ) )
         text.Prepend( '.' );
      m_instance.p_outputExtension = text;
   }
   else if (sender == GUI->Prefix_Edit)
      m_instance.p_prefix = text;
   else if (sender == GUI->Postfix_Edit)
      m_instance.p_postfix = text;
   else if (sender == GUI->OutputDir_Edit)
      m_instance.p_outputDir = text;
   else if (sender == GUI->SubtractFile_Edit)
   {
      m_instance.p_subtractFile = text;
      UpdateSubtractSection();
   }
   else if ( sender == GUI->InputHints_Edit )
      m_instance.p_inputHints = text;
   else if ( sender == GUI->OutputHints_Edit )
      m_instance.p_outputHints = text;

   sender.SetText (text);
}

void
CometAlignmentInterface::__Button_Click (Button& sender, bool checked)

{
#if debug
   Console ().WriteLn ("__Button_Click():" + sender.Text ());
   Console ().Flush ();
#endif

   if (sender == GUI->OutputDir_SelectButton)
      SelectDir ();
   else if (sender == GUI->SubtractFile_SelectButton)
      SelectSubtractFile ();
   else if (sender == GUI->SubtractFile_ClearButton)
   {
      GUI->SubtractFile_Edit.SetText (m_instance.p_subtractFile = TheSubtractFile->DefaultValue ());
      UpdateSubtractSection();
   }
   else if (sender == GUI->Overwrite_CheckBox)
      m_instance.p_overwrite = checked;
   else if (sender == GUI->SubtractMode_CheckBox)
      m_instance.p_subtractMode = checked;
   else if (sender == GUI->LinearFit_CheckBox)
      m_instance.p_enableLinearFit = checked;
   else if (sender == GUI->Normalize_CheckBox)
      m_instance.p_normalize = checked;

   else if (sender == GUI->File0_PushButton && !m_instance.p_targetFrames.IsEmpty ())
      FileShow (m_instance.p_targetFrames.Begin ()->path);
   else if (sender == GUI->File1_PushButton && !m_instance.p_targetFrames.IsEmpty ())
      FileShow ((*m_instance.p_targetFrames.ReverseBegin ()).path);
}

void
CometAlignmentInterface::__RealValueUpdated (NumericEdit& sender, double value)

{
   if (sender == GUI->x0_NumericEdit)
      SetFirst (DPoint (value, m_pos0.y));
   else if (sender == GUI->y0_NumericEdit)
      SetFirst (DPoint (m_pos0.x, value));
   else if (sender == GUI->x1_NumericEdit)
      SetLast (DPoint (value, m_pos1.y));
   else if (sender == GUI->y1_NumericEdit)
      SetLast (DPoint (m_pos1.x, value));
   else if (sender == GUI->xDelta_NumericEdit)

   {
      m_pos1.x = m_pos0.x + value * m_DateDelta * 24;
      SetLast (DPoint (m_pos1.x, m_pos1.y));
   }
   else if (sender == GUI->yDelta_NumericEdit)

   {
      m_pos1.y = m_pos0.y + value * m_DateDelta * 24;
      SetLast (DPoint (m_pos1.x, m_pos1.y));
   }
   else if (sender == GUI->RejectLow_NumericControl)

   {
      m_instance.p_rejectLow = value;
      if (m_instance.p_rejectLow >= m_instance.p_rejectHigh)

      {
         m_instance.p_rejectHigh = m_instance.p_rejectLow + 0.05;
         if (m_instance.p_rejectHigh > 1)

         {
            m_instance.p_rejectLow = 0.95;
            m_instance.p_rejectHigh = 1;
         }
         UpdateControls ();
      }
   }
   else if (sender == GUI->RejectHigh_NumericControl)

   {
      m_instance.p_rejectHigh = value;
      if (m_instance.p_rejectHigh <= m_instance.p_rejectLow)

      {
         m_instance.p_rejectLow = m_instance.p_rejectHigh - 0.05;
         if (m_instance.p_rejectLow < 0)

         {
            m_instance.p_rejectLow = 0;
            m_instance.p_rejectHigh = 0.05;
         }
         UpdateControls ();
      }
   }
   else if (sender == GUI->ClampingThreshold_NumericControl)
      m_instance.p_linearClampingThreshold = value;
}

void
CometAlignmentInterface::__ItemSelected (ComboBox& sender, int itemIndex)

{
   if (sender == GUI->PixelInterpolation_ComboBox)

   {
      m_instance.p_pixelInterpolation = itemIndex;
      GUI->ClampingThreshold_NumericControl.Enable (
                                                    m_instance.p_pixelInterpolation == CAPixelInterpolation::BicubicSpline ||
                                                    m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos3 ||
                                                    m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos4 ||
                                                    m_instance.p_pixelInterpolation == CAPixelInterpolation::Lanczos5);
   }
}


// ----------------------------------------------------------------------------

CometAlignmentInterface::GUIData::GUIData (CometAlignmentInterface& w)

{
   pcl::Font fnt = w.Font ();
   int xFixLabelWidth = fnt.Width (String ('M', 5));
   int xyLabelWidth = fnt.Width (String ('X', 3));
   int labelWidth1 = fnt.Width (String ("Clamping threshold:") + 'T');
   int editWidth2 = fnt.Width( String( 'M', 5  ) );

   //

   TargetImages_TreeBox.SetMinHeight (IMAGELIST_MINHEIGHT (fnt));
   TargetImages_TreeBox.SetNumberOfColumns (10); // plus 1 hidden column for GUI Stretch
   TargetImages_TreeBox.SetHeaderText (0, "#");
   TargetImages_TreeBox.SetHeaderText (1, "?");
   TargetImages_TreeBox.SetHeaderText (2, "File");
   TargetImages_TreeBox.SetHeaderText (3, "DATE-OBS");
   TargetImages_TreeBox.SetHeaderText (4, "X");
   TargetImages_TreeBox.SetHeaderText (5, "Y");
   TargetImages_TreeBox.SetHeaderText (6, "dSec");
   TargetImages_TreeBox.SetHeaderText (7, "dX");
   TargetImages_TreeBox.SetHeaderText (8, "dY");
   //TargetImages_TreeBox.SetHeaderText( 9, "" ); // hiden column for Stretch

   TargetImages_TreeBox.EnableMultipleSelections ();
   TargetImages_TreeBox.DisableRootDecoration ();
   TargetImages_TreeBox.EnableAlternateRowColor ();
   TargetImages_TreeBox.OnCurrentNodeUpdated ((TreeBox::node_navigation_event_handler) & CometAlignmentInterface::__TargetImages_CurrentNodeUpdated, w);
   TargetImages_TreeBox.OnNodeActivated ((TreeBox::node_event_handler) & CometAlignmentInterface::__TargetImages_NodeActivated, w);
   TargetImages_TreeBox.OnNodeSelectionUpdated ((TreeBox::tree_event_handler) & CometAlignmentInterface::__TargetImages_NodeSelectionUpdated, w);
   //TargetImages_TreeBox.SetToolTip( "<p>DoubleClick to open file.</p>" );

   AddFiles_PushButton.SetText ("Add Files");
   AddFiles_PushButton.SetToolTip ("<p>Add existing image files to the list of target frames.</p>");
   AddFiles_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__TargetImages_BottonClick, w);

   SelectAll_PushButton.SetText ("Select All");
   SelectAll_PushButton.SetToolTip ("<p>Select all target frames.</p>");
   SelectAll_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__TargetImages_BottonClick, w);

   InvertSelection_PushButton.SetText ("Invert Selection");
   InvertSelection_PushButton.SetToolTip ("<p>Invert the current selection of target frames.</p>");
   InvertSelection_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__TargetImages_BottonClick, w);

   ToggleSelected_PushButton.SetText ("Toggle Selected");
   ToggleSelected_PushButton.SetToolTip ("<p>Toggle the enabled/disabled state of currently selected target frames.</p>"
                                         "<p>Disabled target frames will be ignored during the CometAlignment process.</p>");
   ToggleSelected_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__TargetImages_BottonClick, w);

   RemoveSelected_PushButton.SetText ("Remove Selected");
   RemoveSelected_PushButton.SetToolTip ("<p>Remove all currently selected target frames.</p>");
   RemoveSelected_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__TargetImages_BottonClick, w);

   Clear_PushButton.SetText ("Clear");
   Clear_PushButton.SetToolTip ("<p>Clear the list of target frames.</p>");
   Clear_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__TargetImages_BottonClick, w);

   FullPaths_CheckBox.SetText ("Full paths");
   FullPaths_CheckBox.SetToolTip ("<p>Show full paths for target frame files.</p>");
   FullPaths_CheckBox.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__TargetImages_BottonClick, w);

   TargetImages_Control.SetSizer (TargetImages_Sizer);

   TargetImages_SectionBar.SetTitle ("Target Frames");
   TargetImages_SectionBar.SetSection (TargetImages_Control);
   TargetImages_SectionBar.OnToggleSection ((SectionBar::section_event_handler) & CometAlignmentInterface::__ToggleSection, w);

   TargetButtons_Sizer.SetSpacing (4);
   TargetButtons_Sizer.Add (AddFiles_PushButton);
   TargetButtons_Sizer.Add (SelectAll_PushButton);
   TargetButtons_Sizer.Add (InvertSelection_PushButton);
   TargetButtons_Sizer.Add (ToggleSelected_PushButton);
   TargetButtons_Sizer.Add (RemoveSelected_PushButton);
   TargetButtons_Sizer.Add (Clear_PushButton);
   TargetButtons_Sizer.Add (FullPaths_CheckBox);
   TargetButtons_Sizer.AddStretch ();

   TargetImages_Sizer.SetSpacing (4);
   TargetImages_Sizer.Add (TargetImages_TreeBox, 100);
   TargetImages_Sizer.Add (TargetButtons_Sizer);

   //---------------------------------------------------

   FormatHints_SectionBar.SetTitle( "Format Hints" );
   FormatHints_SectionBar.SetSection( FormatHints_Control );
   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CometAlignmentInterface::__ToggleSection, w );

   const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings for image "
      "files used by specific processes. In CometAlignment, input hints change the way input images of some particular "
      "file formats are loaded, while output hints modify the way output registered image files are written.</p>"
      "<p>For example, you can use the \"lower-range\" and \"upper-range\" input hints to load floating point FITS and "
      "TIFF files generated by other applications that don't use PixInsight's normalized [0,1] range. Similarly, you "
      "can specify the \"up-bottom\" output hint to write all registered images (in FITS format) with the coordinate "
      "origin at the top-left corner. Most standard file format modules support hints; each format supports a number of "
      "input and/or output hints that you can use for different purposes with tools that give you access to format hints.</p>";

   InputHints_Label.SetText( "Input hints:" );
   InputHints_Label.SetFixedWidth( labelWidth1 );
   InputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   InputHints_Label.SetToolTip( hintsToolTip );

   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::__EditCompleted, w );

   InputHints_Sizer.SetSpacing( 4 );
   InputHints_Sizer.Add( InputHints_Label );
   InputHints_Sizer.Add( InputHints_Edit, 100 );

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetFixedWidth( labelWidth1 );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputHints_Label.SetToolTip( hintsToolTip );

   OutputHints_Edit.SetToolTip( hintsToolTip );
   OutputHints_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::__EditCompleted, w );

   OutputHints_Sizer.SetSpacing( 4 );
   OutputHints_Sizer.Add( OutputHints_Label );
   OutputHints_Sizer.Add( OutputHints_Edit, 100 );

   FormatHints_Sizer.SetSpacing( 4 );
   FormatHints_Sizer.Add( InputHints_Sizer );
   FormatHints_Sizer.Add( OutputHints_Sizer );

   FormatHints_Control.SetSizer( FormatHints_Sizer );

   //---------------------------------------------------

   const char* ToolTipOutputDir = "<p>This is the directory (or folder) where all output files "
           "will be written.</p>"
           "<p>If this field is left blank, output files will be written to the same directories as their "
           "corresponding target files. In this case, make sure that source directories are writable, or the "
           "CometAlignment process will fail.</p>";

   OutputDir_Label.SetText( "Output directory:" );
   OutputDir_Label.SetFixedWidth( labelWidth1 );
   OutputDir_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDir_Label.SetToolTip( ToolTipOutputDir );

   OutputDir_Edit.SetToolTip (ToolTipOutputDir);
   OutputDir_Edit.OnMouseDoubleClick ((Control::mouse_event_handler) & CometAlignmentInterface::__MouseDoubleClick, w);
   OutputDir_Edit.OnEditCompleted ((Edit::edit_event_handler) & CometAlignmentInterface::__EditCompleted, w);

   OutputDir_SelectButton.SetIcon (Bitmap (":/browser/select-file.png"));
   OutputDir_SelectButton.SetFixedSize (19, 19);
   OutputDir_SelectButton.SetToolTip ("<p>Select output directory</p>");
   OutputDir_SelectButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);

   OutputDir_Sizer.SetSpacing (4);
   OutputDir_Sizer.Add( OutputDir_Label );
   OutputDir_Sizer.Add (OutputDir_Edit, 100);
   OutputDir_Sizer.Add (OutputDir_SelectButton);

   const char* outputExtensionToolTip = "<p>The output file extension determines the file format used to "
      "generate output (registered) image files. If this field is left blank, output files will be written in "
      "the same format as their corresponding target images.</p>"
      "<p>Take into account that the selected output format must be a <i>writable</i> one, that is, a format "
      "able to generate image files, or the CometAlignment process will fail upon trying to write the first "
      "registered image.</p>";

   OutputExtension_Label.SetText( "Output extension:" );
   OutputExtension_Label.SetFixedWidth( labelWidth1 );
   OutputExtension_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputExtension_Label.SetToolTip( outputExtensionToolTip );

   OutputExtension_Edit.SetFixedWidth( editWidth2 );
   OutputExtension_Edit.SetToolTip( outputExtensionToolTip );
   OutputExtension_Edit.OnEditCompleted( (Edit::edit_event_handler)&CometAlignmentInterface::__EditCompleted, w );

   const char* ToolTipPrefix =
           "<p>This is a prefix that will be appended to the file name of each registered image.</p>";
   Prefix_Label.SetText ("Prefix:");
   Prefix_Label.SetTextAlignment (TextAlign::Right | TextAlign::VertCenter);
   Prefix_Label.SetToolTip (ToolTipPrefix);
   Prefix_Edit.SetFixedWidth (xFixLabelWidth);
   Prefix_Edit.SetToolTip (ToolTipPrefix);
   Prefix_Edit.OnEditCompleted ((Edit::edit_event_handler) & CometAlignmentInterface::__EditCompleted, w);

   const char* ToolTipPostfix =
           "<p>This is a postfix that will be appended to the file name of each registered image.</p>";
   Postfix_Label.SetText ("Postfix:");
   Postfix_Label.SetTextAlignment (TextAlign::Right | TextAlign::VertCenter);
   Postfix_Label.SetToolTip (ToolTipPostfix);
   Postfix_Edit.SetFixedWidth (xFixLabelWidth);
   Postfix_Edit.SetToolTip (ToolTipPostfix);
   Postfix_Edit.OnEditCompleted ((Edit::edit_event_handler) & CometAlignmentInterface::__EditCompleted, w);

   Overwrite_CheckBox.SetText ("Overwrite");
   Overwrite_CheckBox.SetToolTip ("<p>If this option is selected, CometAlignment will overwrite "
                                  "existing files with the same names as generated output files. This can be dangerous because the original "
                                  "contents of overwritten files will be lost.</p>"
                                  "<p><b>Enable this option <u>at your own risk.</u></b></p>");
   Overwrite_CheckBox.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);

   OutputChunks_Sizer.Add( OutputExtension_Label );
   OutputChunks_Sizer.AddSpacing( 4 );
   OutputChunks_Sizer.Add( OutputExtension_Edit );
   OutputChunks_Sizer.AddSpacing( 20 );
   OutputChunks_Sizer.Add (Prefix_Label);
   OutputChunks_Sizer.AddSpacing (4);
   OutputChunks_Sizer.Add (Prefix_Edit);
   OutputChunks_Sizer.AddSpacing (20);
   OutputChunks_Sizer.Add (Postfix_Label);
   OutputChunks_Sizer.AddSpacing (4);
   OutputChunks_Sizer.Add (Postfix_Edit);
   OutputChunks_Sizer.AddSpacing (20);
   OutputChunks_Sizer.Add (Overwrite_CheckBox);
   OutputChunks_Sizer.AddStretch ();

   //---------------------------------------------------
   Output_Sizer.SetSpacing (4);
   Output_Sizer.Add (OutputDir_Sizer);
   Output_Sizer.Add (OutputChunks_Sizer);

   Output_Control.SetSizer (Output_Sizer);
   Output_SectionBar.SetTitle ("Output");
   Output_SectionBar.SetSection (Output_Control);
   Output_SectionBar.OnToggleSection ((SectionBar::section_event_handler) & CometAlignmentInterface::__ToggleSection, w);

   //---------------------------------------------------

   // Section Bar & Control
   Parameter_Control.SetSizer (Parameter_Sizer);
   Parameter_SectionBar.SetTitle ("Parameters");
   Parameter_SectionBar.SetSection (Parameter_Control);
   Parameter_SectionBar.OnToggleSection ((SectionBar::section_event_handler) & CometAlignmentInterface::__ToggleSection, w);

   // GUI centroid selection elements

   const char* centroidXYToolTip = "<p>Core coordinates.</p>";

   x0_NumericEdit.label.SetText ("X");
   x0_NumericEdit.label.SetMinWidth( labelWidth1 );
   x0_NumericEdit.SetPrecision (TheTargetFrameX->Precision ());
   x0_NumericEdit.SetRange (TheTargetFrameX->MinimumValue (), TheTargetFrameX->MaximumValue ());
   x0_NumericEdit.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);
   x0_NumericEdit.SetToolTip (centroidXYToolTip);

   y0_NumericEdit.label.SetText ("Y");
   y0_NumericEdit.label.SetMinWidth (xyLabelWidth);
   y0_NumericEdit.SetPrecision (TheTargetFrameY->Precision ());
   y0_NumericEdit.SetRange (TheTargetFrameY->MinimumValue (), TheTargetFrameY->MaximumValue ());
   y0_NumericEdit.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);
   y0_NumericEdit.SetToolTip (centroidXYToolTip);

   x1_NumericEdit.label.SetText ("X");
   x1_NumericEdit.label.SetMinWidth( labelWidth1 );
   x1_NumericEdit.SetPrecision (TheTargetFrameX->Precision ());
   x1_NumericEdit.SetRange (TheTargetFrameX->MinimumValue (), TheTargetFrameX->MaximumValue ());
   x1_NumericEdit.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);
   x1_NumericEdit.SetToolTip (centroidXYToolTip);

   y1_NumericEdit.label.SetText ("Y");
   y1_NumericEdit.label.SetMinWidth (xyLabelWidth);
   y1_NumericEdit.SetPrecision (TheTargetFrameY->Precision ());
   y1_NumericEdit.SetRange (TheTargetFrameY->MinimumValue (), TheTargetFrameY->MaximumValue ());
   y1_NumericEdit.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);
   y1_NumericEdit.SetToolTip (centroidXYToolTip);

   //

   const char* showImageToolTip = "<p>Open or bring image to front.<br/>"
           "Click on the comet to get centroid coordinates. Use Ctrl to disable centroid detection.</p>";

   File0_PushButton.SetText ("Show");
   File0_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);
   File0_PushButton.SetToolTip (showImageToolTip);
   File0_Label.SetTextAlignment (pcl::TextAlign::VertCenter);

   File1_PushButton.SetText ("Show");
   File1_PushButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);
   File1_PushButton.SetToolTip (showImageToolTip);
   File1_Label.SetTextAlignment (pcl::TextAlign::VertCenter);

   //

   const char* centroidDeltaToolTip = "<p>Comet velocity in pixels per hour.</p>";

   xDelta_NumericEdit.label.SetText ("dX");
   xDelta_NumericEdit.label.SetMinWidth( labelWidth1 );
   xDelta_NumericEdit.SetPrecision (7);
   xDelta_NumericEdit.SetRange (-100000.0, 100000.0);
   xDelta_NumericEdit.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);
   xDelta_NumericEdit.SetToolTip (centroidDeltaToolTip);

   yDelta_NumericEdit.label.SetText ("dY");
   yDelta_NumericEdit.label.SetMinWidth (xyLabelWidth);
   yDelta_NumericEdit.SetPrecision (7);
   yDelta_NumericEdit.SetRange (-100000.0, 100000.0);
   yDelta_NumericEdit.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);
   yDelta_NumericEdit.SetToolTip (centroidDeltaToolTip);

   // Sizers

   File0_Sizer.Add (x0_NumericEdit);
   File0_Sizer.AddSpacing (4);
   File0_Sizer.Add (y0_NumericEdit);
   File0_Sizer.AddSpacing (6);
   File0_Sizer.Add (File0_PushButton);
   File0_Sizer.AddSpacing (6);
   File0_Sizer.Add (File0_Label, 100);

   File1_Sizer.Add (x1_NumericEdit);
   File1_Sizer.AddSpacing (4);
   File1_Sizer.Add (y1_NumericEdit);
   File1_Sizer.AddSpacing (6);
   File1_Sizer.Add (File1_PushButton);
   File1_Sizer.AddSpacing (6);
   File1_Sizer.Add (File1_Label, 100);

   Delta_Sizer.Add (xDelta_NumericEdit);
   Delta_Sizer.AddSpacing (4);
   Delta_Sizer.Add (yDelta_NumericEdit);
   Delta_Sizer.AddStretch ();

   Parameter_Sizer.SetSpacing (4);
   Parameter_Sizer.Add (File0_Sizer);
   Parameter_Sizer.Add (File1_Sizer);
   Parameter_Sizer.Add (Delta_Sizer);

   //---------------------------------------------------

   // Subtract Section Bar & Control
   const char* ToolTipSubtract = "<p>The selected image will be subtracted from each target image.</p>"
           "<p>With <i>Mode</i> checked: Selected Image (usually pure comet image) "
               "will be comet aligned to every target frame and subtracted from it"
               ", i.e. resulting in frames with erased comet and without aligning change.</p>"
           "<p>With <i>Mode</i> uncheked: Selected Image (usually pure star field) "
               "will be subtracted from every target frame and each result will be comet aligned using reference frame"
               ", i.e. resulting in frames aligned to comet without stars.</p>";

   Subtract_Control.SetSizer (Subtract_Sizer);
   Subtract_SectionBar.SetTitle ("Subtract");
   Subtract_SectionBar.SetSection (Subtract_Control);
   Subtract_SectionBar.OnToggleSection ((SectionBar::section_event_handler) & CometAlignmentInterface::__ToggleSection, w);

   SubtractFile_Label.SetText( "Subtract image:" );
   SubtractFile_Label.SetFixedWidth( labelWidth1 );
   SubtractFile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SubtractFile_Label.SetToolTip( ToolTipSubtract );

   SubtractFile_Edit.SetToolTip (ToolTipSubtract);
   SubtractFile_Edit.OnMouseDoubleClick ((Control::mouse_event_handler) & CometAlignmentInterface::__MouseDoubleClick, w);
   SubtractFile_Edit.OnEditCompleted ((Edit::edit_event_handler) & CometAlignmentInterface::__EditCompleted, w);

   SubtractFile_SelectButton.SetIcon (Bitmap (":/browser/select-file.png"));
   SubtractFile_SelectButton.SetFixedSize (19, 19);
   SubtractFile_SelectButton.SetToolTip ("<p>Select image file</p>");
   SubtractFile_SelectButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);

   SubtractFile_ClearButton.SetIcon (Bitmap (":/icons/clear.png"));
   SubtractFile_ClearButton.SetFixedSize (19, 19);
   SubtractFile_ClearButton.SetToolTip ("<p>Clear</p>");
   SubtractFile_ClearButton.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);

   SubtractFile_Sizer.SetSpacing (4);
   SubtractFile_Sizer.Add (SubtractFile_Label);
   SubtractFile_Sizer.Add (SubtractFile_Edit, 100);
   SubtractFile_Sizer.Add (SubtractFile_SelectButton);
   SubtractFile_Sizer.Add (SubtractFile_ClearButton);

   //
   SubtractMode_CheckBox.SetText ("Mode");
   SubtractMode_CheckBox.SetToolTip (ToolTipSubtract);
   SubtractMode_CheckBox.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);

   //
   Normalize_CheckBox.SetText ("Normalize");
   Normalize_CheckBox.SetToolTip ("<p>Normalize median after subtraction.</p>");
   Normalize_CheckBox.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);

   //

   LinearFit_CheckBox.SetText ("Enable LinearFit");
   LinearFit_CheckBox.SetToolTip ("<p>Enable LinearFit before subtraction.</p>");
   LinearFit_CheckBox.OnClick ((Button::click_event_handler) & CometAlignmentInterface::__Button_Click, w);

   //

   SubtractChekers_Sizer.SetSpacing (20);
   SubtractChekers_Sizer.AddSpacing( labelWidth1 + 4 );
   SubtractChekers_Sizer.Add (SubtractMode_CheckBox);
   SubtractChekers_Sizer.Add (LinearFit_CheckBox);
   SubtractChekers_Sizer.Add (Normalize_CheckBox);
   SubtractChekers_Sizer.AddStretch ();

   //

   RejectLow_NumericControl.label.SetText ("Reject low:");
   RejectLow_NumericControl.label.SetFixedWidth (labelWidth1);
   RejectLow_NumericControl.slider.SetRange (0, 100);
   RejectLow_NumericControl.slider.SetMinWidth (200);
   RejectLow_NumericControl.SetReal ();
   RejectLow_NumericControl.SetRange (TheRejectLow->MinimumValue (), TheRejectLow->MaximumValue ());
   RejectLow_NumericControl.SetPrecision (TheRejectLow->Precision ());
   RejectLow_NumericControl.SetToolTip ("<p>Low rejection limit. LinearFit will ignore all reference and target "
                                        "pixels whose values are less than or equal to this limit. Note that according to that definition, black "
                                        "pixels (zero pixel values) are always rejected.</p>");
   RejectLow_NumericControl.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);

   //

   RejectHigh_NumericControl.label.SetText ("Reject high:");
   RejectHigh_NumericControl.label.SetFixedWidth (labelWidth1);
   RejectHigh_NumericControl.slider.SetRange (0, 100);
   RejectHigh_NumericControl.slider.SetMinWidth (200);
   RejectHigh_NumericControl.SetReal ();
   RejectHigh_NumericControl.SetRange (TheRejectHigh->MinimumValue (), TheRejectHigh->MaximumValue ());
   RejectHigh_NumericControl.SetPrecision (TheRejectHigh->Precision ());
   RejectHigh_NumericControl.SetToolTip ("<p>High rejection limit. LinearFit will ignore all reference and target "
                                         "pixels whose values are greater than or equal to this limit. Note that according to that definition, white "
                                         "pixels (pixel values equal to one in the normalized [0,1] range) are always rejected.</p>");
   RejectHigh_NumericControl.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);



   //

   Subtract_Sizer.SetSpacing (4);
   Subtract_Sizer.Add (SubtractFile_Sizer);
   Subtract_Sizer.Add (SubtractChekers_Sizer);
   Subtract_Sizer.Add (RejectLow_NumericControl);
   Subtract_Sizer.Add (RejectHigh_NumericControl);


   //---------------------------------------------------


   Interpolation_SectionBar.SetTitle ("Interpolation");
   Interpolation_SectionBar.SetSection (Interpolation_Control);
   Interpolation_SectionBar.OnToggleSection ((SectionBar::section_event_handler) & CometAlignmentInterface::__ToggleSection, w);

   const char* pixelInterpolationToolTip = "<p>Pixel interpolation algorithm</p>"
           "<p><b>Lanczos</b> is, in general, the best interpolation algorithm for image registration of linear "
           "images when no large scale changes are required. Lanczos has excellent detail preservation and subpixel "
           "registration performance with minimal generation of aliasing effects. Its main drawback is generation of "
           "undershooting (ringing) artifacts, but this problem can be fixed in most cases with the implemented "
           "<i>clamping</i> mechanism (see the <i>clamping threshold</i> parameter below).</p>"
           "<p><b>Bicubic spline</b> is an excellent interpolation algorithm in terms of accuracy and execution "
           "speed. As Lanczos interpolation, bicubic spline generates ringing artifacts, but the implemented clamping "
           "mechanism can usually fix most of them. The main problem with bicubic spline is generation of aliasing "
           "effects, such as moire patterns, which can be problematic with noisy images, especially on sky background "
           "areas. This is one of the reasons why Lanczos is in general the preferred option.</p>"
           "<p><b>Bilinear interpolation</b> can be useful to register low SNR linear images, in the rare cases where "
           "Lanczos and bicubic spline interpolation generates too strong oscillations between noisy pixels that can't "
           "be avoided completely with the clamping feature. Bilinear interpolation has no ringing artifacts, but it "
           "generates strong aliasing patterns on noisy areas and provides less accurate results than Lanczos and "
           "bicubic spline.</p>"
           "<p><b>Cubic filter interpolations</b>, such as Mitchell-Netravali, Catmull-Rom spline and cubic B-spline, "
           "provide higher smoothness and subsampling accuracy that can be necessary when the registration transformation "
           "involves relatively strong size reductions.</p>"
           "<p><b>Nearest neighbor</b> is the simplest possible pixel interpolation method. It always produces the "
           "worst results, especially in terms of registration accuracy (no subpixel registration is possible), and "
           "discontinuities due to the simplistic interpolation scheme. However, in absence of scaling and rotation "
           "nearest neighbor preserves the original noise distribution in the registered images, a property that can "
           "be useful in some image analysis applications.</p>";

   PixelInterpolation_Label.SetText ("Pixel interpolation:");
   PixelInterpolation_Label.SetFixedWidth (labelWidth1);
   PixelInterpolation_Label.SetTextAlignment (TextAlign::Right | TextAlign::VertCenter);
   PixelInterpolation_Label.SetToolTip (pixelInterpolationToolTip);

   PixelInterpolation_ComboBox.AddItem ("Nearest Neighbor");
   PixelInterpolation_ComboBox.AddItem ("Bilinear");
   PixelInterpolation_ComboBox.AddItem ("Bicubic Spline");
   PixelInterpolation_ComboBox.AddItem ("Bicubic B-Spline");
   PixelInterpolation_ComboBox.AddItem ("Lanczos-3");
   PixelInterpolation_ComboBox.AddItem ("Lanczos-4");
   PixelInterpolation_ComboBox.AddItem ("Lanczos-5");
   PixelInterpolation_ComboBox.AddItem ("Mitchell-Netravali Filter");
   PixelInterpolation_ComboBox.AddItem ("Catmull-Rom Spline Filter");
   PixelInterpolation_ComboBox.AddItem ("Cubic B-Spline Filter");
   PixelInterpolation_ComboBox.SetMaxVisibleItemCount (16);
   PixelInterpolation_ComboBox.SetToolTip (pixelInterpolationToolTip);
   PixelInterpolation_ComboBox.OnItemSelected ((ComboBox::item_event_handler) & CometAlignmentInterface::__ItemSelected, w);

   PixelInterpolation_Sizer.SetSpacing (4);
   PixelInterpolation_Sizer.Add (PixelInterpolation_Label);
   PixelInterpolation_Sizer.Add (PixelInterpolation_ComboBox);
   PixelInterpolation_Sizer.AddStretch ();

   ClampingThreshold_NumericControl.label.SetText ("Clamping threshold:");
   ClampingThreshold_NumericControl.label.SetFixedWidth (labelWidth1);
   ClampingThreshold_NumericControl.slider.SetRange (0, 100);
   ClampingThreshold_NumericControl.slider.SetMinWidth (200);
   ClampingThreshold_NumericControl.SetReal ();
   ClampingThreshold_NumericControl.SetRange (TheLinearClampingThresholdParameter->MinimumValue (), TheLinearClampingThresholdParameter->MaximumValue ());
   ClampingThreshold_NumericControl.SetPrecision (TheLinearClampingThresholdParameter->Precision ());
   ClampingThreshold_NumericControl.SetToolTip (
         "<p>Clamping threshold for the bicubic spline and Lanczos interpolation algorithms.</p>"
         "<p>The main drawback of the Lanczos and bicubic spline interpolation algorithms is generation of strong "
         "undershooting (ringing) artifacts. This is caused by negative lobes of the interpolation functions falling "
         "over high-contrast edges and isolated bright pixels. This usually happens with linear data, such as raw CCD "
         "and DSLR images, and rarely with nonlinear or stretched images.</p>"
         "<p>In the current PixInsight/PCL implementations of these algorithms we have included a <i>clamping</i> "
         "mechanism that prevents negative interpolated values and ringing artifacts for most images. This parameter "
         "represents a threshold value to trigger interpolation clamping. A lower threshold leads to a more aggressive "
         "deringing effect; however, too low of a clamping threshold can degrade interpolation performance, especially "
         "in terms of aliasing and detail preservation.</p>");
   ClampingThreshold_NumericControl.OnValueUpdated ((NumericEdit::value_event_handler) & CometAlignmentInterface::__RealValueUpdated, w);

   Interpolation_Sizer.SetSpacing (4);
   Interpolation_Sizer.Add (PixelInterpolation_Sizer);
   Interpolation_Sizer.Add (ClampingThreshold_NumericControl);

   Interpolation_Control.SetSizer (Interpolation_Sizer);

   //

   Global_Sizer.SetMargin (8);
   Global_Sizer.SetSpacing (6);
   Global_Sizer.Add (TargetImages_SectionBar);
   Global_Sizer.Add (TargetImages_Control);
   Global_Sizer.Add( FormatHints_SectionBar );
   Global_Sizer.Add( FormatHints_Control );
   Global_Sizer.Add (Output_SectionBar);
   Global_Sizer.Add (Output_Control);

   Global_Sizer.Add (Parameter_SectionBar);
   Global_Sizer.Add (Parameter_Control);

   Global_Sizer.Add (Subtract_SectionBar);
   Global_Sizer.Add (Subtract_Control);
   Global_Sizer.Add (Interpolation_SectionBar);
   Global_Sizer.Add (Interpolation_Control);

   w.SetSizer (Global_Sizer);
   w.AdjustToContents ();

   FormatHints_Control.Hide();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF CometAlignmentInterface.cpp - Released 2015/02/10 19:50:08 UTC
