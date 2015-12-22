//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0348
// ----------------------------------------------------------------------------
// DynamicPSFInterface.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "DynamicPSFInterface.h"
#include "DynamicPSFProcess.h"
#include "StarDetector.h"

#include <pcl/Dialog.h>
#include <pcl/FileDialog.h>
#include <pcl/GaussianFilter.h>
#include <pcl/GlobalSettings.h>
#include <pcl/GroupBox.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/MoffatFilter.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/Settings.h>
#include <pcl/TextBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "DynamicPSFIcon.xpm"

// ----------------------------------------------------------------------------

DynamicPSFInterface* TheDynamicPSFInterface = nullptr;

static const pcl_bool* signedAngles = nullptr;

// Half size of center mark in logical viewport pixels.
#define CENTER_RADIUS   5

#define DATATREE_MINHEIGHT( fnt )   RoundInt( 12.125*fnt.Height() )

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class SortStarsDialog : public Dialog
{
public:

   DynamicPSFInterface::SortingCriterion sortBy;

   SortStarsDialog() :
      sortBy( DynamicPSFInterface::SortByMAD )
   {
      SortingCriterion_GroupBox.SetTitle( "Sorting Criterion" );

      SortByStarId_RadioButton.SetText(           "Star Id" );
      SortByBackground_RadioButton.SetText(       "Background" );
      SortByAmplitude_RadioButton.SetText(        "Amplitude" );
      SortBySigma_RadioButton.SetText(            "Standard deviation" );
      SortByAspectRatio_RadioButton.SetText(      "Aspect ratio" );
      SortByRotationAngle_RadioButton.SetText(    "Rotation angle" );
      SortByAbsRotationAngle_RadioButton.SetText( "Rotation angle (absolute value)" );
      SortByShape_RadioButton.SetText(            "Beta exponent" );
      SortByMAD_RadioButton.SetText(              "Mean absolute difference" );
      SortByMAD_RadioButton.SetChecked();

      SortingCriterion_Sizer.SetMargin( 6 );
      SortingCriterion_Sizer.SetSpacing( 6 );
      SortingCriterion_Sizer.Add( SortByStarId_RadioButton );
      SortingCriterion_Sizer.Add( SortByBackground_RadioButton );
      SortingCriterion_Sizer.Add( SortByAmplitude_RadioButton );
      SortingCriterion_Sizer.Add( SortBySigma_RadioButton );
      SortingCriterion_Sizer.Add( SortByAspectRatio_RadioButton );
      SortingCriterion_Sizer.Add( SortByRotationAngle_RadioButton );
      SortingCriterion_Sizer.Add( SortByAbsRotationAngle_RadioButton );
      SortingCriterion_Sizer.Add( SortByShape_RadioButton );
      SortingCriterion_Sizer.Add( SortByMAD_RadioButton );

      SortingCriterion_GroupBox.SetSizer( SortingCriterion_Sizer );
      SortingCriterion_GroupBox.SetMinWidth( Font().Width( String( '0', 40 ) ) );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.SetCursor( StdCursor::Checkmark );
      OK_PushButton.OnClick( (Button::click_event_handler)&SortStarsDialog::__Button_Click, *this );

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.SetCursor( StdCursor::Crossmark );
      Cancel_PushButton.OnClick( (Button::click_event_handler)&SortStarsDialog::__Button_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( SortingCriterion_GroupBox );
      Global_Sizer.AddSpacing( 6 );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );
      AdjustToContents();
      SetFixedSize();

      SetWindowTitle( "Sort PSF Collections" );
   }

private:

   VerticalSizer  Global_Sizer;
      GroupBox       SortingCriterion_GroupBox;
      VerticalSizer  SortingCriterion_Sizer;
         RadioButton SortByStarId_RadioButton;
         RadioButton SortByBackground_RadioButton;
         RadioButton SortByAmplitude_RadioButton;
         RadioButton SortBySigma_RadioButton;
         RadioButton SortByAspectRatio_RadioButton;
         RadioButton SortByRotationAngle_RadioButton;
         RadioButton SortByAbsRotationAngle_RadioButton;
         RadioButton SortByShape_RadioButton;
         RadioButton SortByMAD_RadioButton;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void __Button_Click( Button& sender, bool checked )
   {
      if ( sender == OK_PushButton )
      {
         if ( SortByStarId_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByStarId;
         if ( SortByBackground_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByBackground;
         if ( SortByAmplitude_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByAmplitude;
         if ( SortBySigma_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortBySigma;
         if ( SortByAspectRatio_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByAspectRatio;
         if ( SortByRotationAngle_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByRotationAngle;
         if ( SortByAbsRotationAngle_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByAbsRotationAngle;
         if ( SortByShape_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByShape;
         if ( SortByMAD_RadioButton.IsChecked() )
            sortBy = DynamicPSFInterface::SortByMAD;
         Ok();
      }
      else if ( sender == Cancel_PushButton )
         Cancel();
   }
};

static SortStarsDialog* sortStarsDialog = nullptr;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class AverageStarDialog : public Dialog
{
public:

   typedef DynamicPSFInterface::star_list star_list;
   typedef DynamicPSFInterface::psf_list  psf_list;

   AverageStarDialog( const star_list& stars, float xScale, float yScale )
   {
      Data_TextBox.SetReadOnly();
      Data_TextBox.SetMinSize( Font().Width( String( '0', 40 ) ), LogicalPixelsToPhysical( 250 ) );

      RenderPSF_ToolButton.SetIcon( ScaledResource( ":/icons/camera.png" ) );
      RenderPSF_ToolButton.SetScaledFixedSize( 24, 24 );
      RenderPSF_ToolButton.SetToolTip( "<p>Render the average PSF as a new image.</p>" );
      RenderPSF_ToolButton.OnClick( (Button::click_event_handler)&AverageStarDialog::__Button_Click, *this );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.SetCursor( StdCursor::Checkmark );
      OK_PushButton.OnClick( (Button::click_event_handler)&AverageStarDialog::__Button_Click, *this );

      Buttons_Sizer.Add( RenderPSF_ToolButton );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( Data_TextBox );
      Global_Sizer.AddSpacing( 6 );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );
      AdjustToContents();
      SetMinSize();

      SetWindowTitle( "Average Star Data" );

      N = 0;
      gaussian = -1;
      B = A = sx = sy = theta = beta = mad = 0;
      for ( star_list::const_iterator s = stars.Begin(); s != stars.End(); ++s )
         for ( psf_list::const_iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
            if ( *p )
            {
               if ( gaussian < 0 )
                  gaussian = p->function == PSFFit::Gaussian;
               else if ( gaussian != (p->function == PSFFit::Gaussian) )
                  throw Error( "Internal Error: Incongruent PSF functions in selected stars set." );
               B += p->B;
               A += p->A;
               sx += p->sx;
               sy += p->sy;
               theta += (*signedAngles && p->theta > 90) ? p->theta - 180 : p->theta;
               beta += p->beta;
               mad += p->mad;
               ++N;
            }

      if ( N == 0 )
         throw Error( "Internal Error: Insufficient PSF data." );

      B /= N;
      A /= N;
      sx /= N;
      sy /= N;
      theta /= N;
      beta /= N;
      mad /= N;

      bool isXScale = xScale > 0; if ( !isXScale ) xScale = 1;
      bool isYScale = yScale > 0; if ( !isYScale ) yScale = 1;
      int function = gaussian ? PSFFit::Gaussian : PSFFit::Moffat;

      IsoString fmt =
         "<monospace>"
         "Average %s PSF\n"
         "N ....... %d stars\n"
         "B ....... %10.6f\n"
         "A ....... %10.6f\n"
         "sx ...... %6.2f px\n"
         "sy ...... %6.2f px\n"
         "FWHMx ... %6.2f %s\n"
         "FWHMy ... %6.2f %s\n"
         "r ....... %7.3f\n"
         "theta ... %";
      if ( *signedAngles )
         fmt += '+';
      fmt +=
         "6.2f deg\n"
         "beta .... %6.2f\n"
         "MAD ..... %.3e\n";

      Data_TextBox.SetText( String().Format(
         fmt.c_str(),
         gaussian ? "Gaussian" : "Moffat",
         N,
         B,
         A,
         sx,
         sy,
         PSFData::FWHM( function, sx, beta )*xScale, isXScale ? "\"" : "px",
         PSFData::FWHM( function, sy, beta )*yScale, isYScale ? "\"" : "px",
         sy/sx,
         theta,
         beta,
         mad ) );
   }

private:

   int    N;
   int    gaussian;
   double B;
   double A;
   double sx;
   double sy;
   double theta;
   double beta;
   double mad;

   VerticalSizer  Global_Sizer;
      TextBox     Data_TextBox;
      HorizontalSizer   Buttons_Sizer;
         ToolButton        RenderPSF_ToolButton;
         PushButton        OK_PushButton;

   void __Button_Click( Button& sender, bool checked )
   {
      if ( sender == RenderPSF_ToolButton )
      {
         ImageWindow w( 1, 1, 1, 32, true/*floatSample*/, false/*color*/, true/*initialProcessing*/, "PSF" );
         ImageVariant v = w.MainView().Image();

         if ( gaussian )
         {
            GaussianFilter G( float( sx ), 0.01F, sy/sx, Rad( theta ) );
            G.ToImage( v );
         }
         else
         {
            MoffatFilter M( float( sx ), beta, 0.01F, sy/sx, Rad( theta ) );
            M.ToImage( v );
         }

         w.Show();
         w.ZoomToFit( false/*allowMagnification*/ );
      }
      else if ( sender == OK_PushButton )
      {
         Ok();
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class PSFTreeNode : public TreeBox::Node
{
public:

   PSFTreeNode( TreeBox& parent ) : TreeBox::Node()
   {
      parent.Add( this );
   }

   PSFTreeNode( TreeBox::Node& parent ) : TreeBox::Node()
   {
      parent.Add( this );
   }

   virtual ~PSFTreeNode()
   {
   }

   virtual void Update() = 0;
};

class PSFCollectionNode : public PSFTreeNode
{
public:

   DynamicPSFInterface::PSFCollection* collection;

   PSFCollectionNode( TreeBox& parent, DynamicPSFInterface::PSFCollection* c ) :
      PSFTreeNode( parent ),
      collection( c )
   {
      collection->node = this;
      Update();
      Expand();
   }

   virtual ~PSFCollectionNode()
   {
      collection->node = nullptr;
   }

   virtual void Update()
   {
      SetIcon( 0, ParentTree().ScaledResource( collection->view.IsNull() ? ":/icons/error.png" :
                        (collection->view.IsPreview() ? ":/browser/preview.png" : ":/browser/main-view.png") ) );

      SetText( 0, collection->ViewId() );
      SetAlignment( 0, TextAlign::Left|TextAlign::VertCenter );

      if ( collection->view.IsNull() )
         SetToolTip( 0, "No such view: " + collection->ViewId() );
   }
};

class StarNode : public PSFTreeNode
{
public:

   DynamicPSFInterface::Star* star;

   StarNode( TreeBox::Node& parent, DynamicPSFInterface::Star* s ) :
      PSFTreeNode( parent ),
      star( s )
   {
      star->node = this;
      Update();
      Expand();
   }

   virtual ~StarNode()
   {
      star->node = nullptr;
   }

   virtual void Update()
   {
      SetIcon( 0, ParentTree().ScaledResource( *star ? ":/icons/star.png" : ":/icons/error.png" ) );
      if ( !*star )
         SetToolTip( 0, star->StatusToString() );

      SetText( 0, String().Format( "%5d", star->uniqueId ) );
      SetText( 1, String().Format( "%d", star->channel ) );
      SetAlignment( 0, TextAlign::Right|TextAlign::VertCenter );
      SetAlignment( 1, TextAlign::Right|TextAlign::VertCenter );
      // TODO: show also barycenter and selection coordinates?
   }
};

class PSFNode : public PSFTreeNode
{
public:

   DynamicPSFInterface::PSF* psf;

   PSFNode( TreeBox::Node& parent, DynamicPSFInterface::PSF* p ) :
      PSFTreeNode( parent ),
      psf( p )
   {
      psf->node = this;
      Update();
   }

   virtual ~PSFNode()
   {
      psf->node = nullptr;
   }

   virtual void Update()
   {
      SetIcon( 0, ParentTree().ScaledResource( *psf ? ":/bullets/bullet-ball.png" : ":/icons/error.png" ) );
      if ( !*psf )
         SetToolTip( 0, psf->StatusToString() );

      SetText( 0, psf->FunctionToString() );
      SetAlignment( 0, TextAlign::Left|TextAlign::VertCenter );

      if ( *psf )
      {
         SetText( 2, String().Format( "%8.6f", psf->B ) );     // B
         SetText( 3, String().Format( "%8.6f", psf->A ) );     // A
         SetText( 4, String().Format( "%9.2f", psf->c0.x ) );  // cx
         SetText( 5, String().Format( "%9.2f", psf->c0.y ) );  // cy
         SetText( 6, String().Format( "%6.2f", psf->sx ) );    // sx

         if ( psf->star->collection->xScale > 0 )
            SetText( 8, String().Format( "%6.2f\"", psf->FWHMx() * psf->star->collection->xScale ) ); // FWHMx arcsec/px
         else
            SetText( 8, String().Format( "%6.2fpx", psf->FWHMx() ) ); // FWHMx px

         if ( !psf->circular )
         {
            SetText( 7, String().Format( "%6.2f", psf->sy ) ); // sy

            if ( psf->star->collection->yScale > 0 )
               SetText( 9, String().Format( "%6.2f\"", psf->FWHMy() * psf->star->collection->yScale ) ); // FWHMy arcsec/px
            else
               SetText( 9, String().Format( "%6.2fpx", psf->FWHMy() ) ); // FWHMy

            SetText( 10, String().Format( "%5.3f", psf->sy/psf->sx ) ); // r
            SetText( 11, String().Format( "%6.2f", (*signedAngles && psf->theta > 90) ? psf->theta - 180 : psf->theta ) ); // theta (degrees)
         }

         if ( psf->function == PSFFit::Gaussian )
            SetText( 12, String() );
         else
            SetText( 12, String().Format( "%5.2f", psf->beta ) ); // beta

         SetText( 13, String().Format( "%.3e", psf->mad ) );      // MAD

         for ( int i = 2; i < 14; ++i )
            SetAlignment( i, TextAlign::Right|TextAlign::VertCenter );
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DynamicPSFInterface::DynamicPSFInterface() :
   ProcessInterface(),
   instance( TheDynamicPSFProcess ),
   GUI( nullptr ),
   data(),
   selectedStars()
{
   TheDynamicPSFInterface = this;
   signedAngles = &instance.signedAngles;
}

DynamicPSFInterface::~DynamicPSFInterface()
{
   selectedStars.Clear();
   data.Destroy();
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString DynamicPSFInterface::Id() const
{
   return "DynamicPSF";
}

MetaProcess* DynamicPSFInterface::Process() const
{
   return TheDynamicPSFProcess;
}

const char** DynamicPSFInterface::IconImageXPM() const
{
   return DynamicPSFIcon_XPM;
}

InterfaceFeatures DynamicPSFInterface::Features() const
{
   return InterfaceFeature::DragObject |
          InterfaceFeature::CancelButton |
          InterfaceFeature::BrowseDocumentationButton |
          InterfaceFeature::ResetButton;
}

void DynamicPSFInterface::ResetInstance()
{
   DynamicPSFInstance defaultInstance( TheDynamicPSFProcess );
   ImportProcess( defaultInstance );
}

bool DynamicPSFInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& flags )
{
   // ### Deferred initialization
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "DynamicPSF" );
      UpdateControls();
      RegenerateDataTree();
   }

   dynamic = true;
   return &P == TheDynamicPSFProcess;
}

ProcessImplementation* DynamicPSFInterface::NewProcess() const
{
   DynamicPSFInstance* exportInstance = new DynamicPSFInstance( TheDynamicPSFProcess );

   exportInstance->AssignOptions( instance );

   for ( psf_data_set::const_iterator c = data.Begin(); c != data.End(); ++c )
   {
      exportInstance->views.Add( c->ViewId() );

      for ( star_list::const_iterator s = c->stars.Begin(); s != c->stars.End(); ++s )
      {
         exportInstance->stars.Add( DynamicPSFInstance::Star( *s, exportInstance->views.Length()-1 ) );

         for ( psf_list::const_iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
            exportInstance->psfs.Add( DynamicPSFInstance::PSF( *p, exportInstance->stars.Length()-1 ) );
      }
   }

   return exportInstance;
}

bool DynamicPSFInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const DynamicPSFInstance* r = dynamic_cast<const DynamicPSFInstance*>( &p );
   if ( r == nullptr )
   {
      whyNot = "Not a DynamicPSF instance.";
      return false;
   }
   whyNot.Clear();
   return true;
}

bool DynamicPSFInterface::RequiresInstanceValidation() const
{
   return true;
}

bool DynamicPSFInterface::ImportProcess( const ProcessImplementation& P )
{
   const DynamicPSFInstance* importInstance = dynamic_cast<const DynamicPSFInstance*>( &P );
   if ( importInstance == nullptr )
      throw Error( "Not a DynamicPSF instance." );

   Update();

   GUI->Data_TreeBox.Clear();

   selectedStars.Clear();
   data.Destroy();

   instance.AssignOptions( *importInstance );

   for ( size_type i = 0; i < importInstance->views.Length(); ++i )
   {
      PSFCollection* c = new PSFCollection( importInstance->views[i] );
      data.Add( c );
      c->UpdateImageScale( instance.scaleMode, instance.scaleValue, instance.scaleKeyword );

      for ( size_type j = 0; j < importInstance->stars.Length(); ++j )
      {
         const DynamicPSFInstance::Star& star = importInstance->stars[j];
         if ( star.view == i )
         {
            Star* s = c->AddStar( star );

            for ( size_type k = 0; k < importInstance->psfs.Length(); ++k )
            {
               const DynamicPSFInstance::PSF& psf = importInstance->psfs[k];
               if ( psf.star == j )
                  s->AddPSF( psf );
            }
         }
      }
   }

   for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
      c->Recalculate( instance.threshold, instance.autoAperture );

   RegenerateDataTree();
   Update();
   UpdateControls();
   return true;
}

bool DynamicPSFInterface::IsDynamicInterface() const
{
   return true;
}

void DynamicPSFInterface::ExitDynamicMode()
{
   Update();

   GUI->Data_TreeBox.Clear();

   selectedStars.Clear();
   data.Destroy();
}

void DynamicPSFInterface::DynamicMouseEnter( View& view )
{
}

void DynamicPSFInterface::DynamicMouseLeave( View& view )
{
}

void DynamicPSFInterface::DynamicMousePress( View& view, const DPoint& pos, int button, unsigned buttons, unsigned modifiers )
{
   AcquirePSFCollection( view );
   if ( !(modifiers & KeyModifier::Shift) )
      UnselectStars();
   ImageWindow window = view.Window();
   window.BeginSelection( pos.RoundedToInt() );
}

void DynamicPSFInterface::DynamicMouseMove( View& view, const DPoint& pos, unsigned buttons, unsigned modifiers )
{
   ImageWindow window = view.Window();
   if ( window.IsSelection() )
   {
      window.ModifySelection( pos.RoundedToInt() );
      star_list stars = FindStars( view, window.SelectionRect(), window.CurrentChannel() );
      SelectStars( stars, modifiers & KeyModifier::Shift/*addToSelection*/ );
   }
}

void DynamicPSFInterface::DynamicMouseRelease( View& view, const DPoint& pos, int button, unsigned buttons, unsigned modifiers )
{
   ImageWindow window = view.Window();

   if ( !window.IsSelection() )
      return;

   window.ModifySelection( pos.RoundedToInt() );
   DRect rect = window.SelectionRect();
   window.EndSelection();

   ImageWindow::display_channel channel = window.CurrentChannel();

   PSFCollection* c = AcquirePSFCollection( view );

   star_list stars = FindStars( view, rect, channel );

   String message;

   if ( stars.IsEmpty() )
      if ( !rect.IsRect() )
      {
         for ( int ch = 0; ch < c->image.NumberOfNominalChannels(); ++ch )
         {
            switch ( channel )
            {
            case DisplayChannel::Red:
               if ( ch != 0 )
                  continue;
               break;
            case DisplayChannel::Green:
               if ( ch != 1 )
                  continue;
               break;
            case DisplayChannel::Blue:
               if ( ch != 2 )
                  continue;
               break;
            default:
               break;
            }

            StarDetector D( c->image, ch, pos, instance.searchRadius, instance.threshold, instance.autoAperture );
            if ( D )
            {
               Star* star = c->AddStar( D.star );
               star->Regenerate( instance.psfOptions );
               new StarNode( *c->node, star );
               star->CreatePSFNodes();
               stars.Add( star );
               if ( star->psfs.IsEmpty() )
                  message = "NO CONVERGENCE";
            }
            else
            {
               message = D.star.StatusToString().Uppercase();
            }
         }

         AdjustDataTreeColumns();
      }

   for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
   {
      s->node->Expand();
      if ( s == stars.Begin() )
         GUI->Data_TreeBox.SetCurrentNode( s->node );
      for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
         GUI->Data_TreeBox.SetNodeIntoView( p->node );
   }

   SelectStars( stars, modifiers & KeyModifier::Shift/*addToSelection*/ );

   UpdateControls();

   if ( !message.IsEmpty() )
      GUI->StarInfo_Label.SetText( "*** " + message );
}

void DynamicPSFInterface::DynamicMouseDoubleClick( View& view, const DPoint& pos, unsigned buttons, unsigned modifiers )
{
   // Placeholder
}

bool DynamicPSFInterface::DynamicKeyPress( View& view, int key, unsigned modifiers )
{
   if ( !view.IsDynamicTarget() )
      return false;

   switch ( key )
   {
   case KeyCode::Delete:
   case KeyCode::Backspace:
      // Simulate a button click
      __Click( GUI->DeleteStar_ToolButton, false );
      break;

   case KeyCode::Escape:
      //ImageWindow::TerminateDynamicSession();
      break;

   default:
      return false;
   }

   return true;
}

bool DynamicPSFInterface::RequiresDynamicUpdate( const View& view, const DRect& rect ) const
{
   const PSFCollection* c = FindPSFCollection( view );
   if ( c != nullptr )
   {
      ImageWindow window = view.Window();
      ImageWindow::display_channel channel = window.CurrentChannel();

      Rect r = window.ImageToViewport( rect ).TruncatedToInt().InflatedBy( 1 );
      for ( star_list::const_iterator s = c->stars.Begin(); s != c->stars.End(); ++s )
      {
         switch ( channel )
         {
         case DisplayChannel::Red:
            if ( s->channel != 0 )
               continue;
            break;
         case DisplayChannel::Green:
            if ( s->channel != 1 )
               continue;
            break;
         case DisplayChannel::Blue:
            if ( s->channel != 2 )
               continue;
            break;
         default:
            break;
         }

         Rect rs = window.ImageToViewport( s->rect ).TruncatedToInt().InflatedBy( 1 );
         if ( r.Intersects( rs ) )
            return true;
      }
   }
   return false;
}

void DynamicPSFInterface::DynamicPaint( const View& view, VectorGraphics& g, const DRect& rect ) const
{
   const PSFCollection* c = FindPSFCollection( view );
   if ( c == nullptr ) // should not happen!
      return;

   ImageWindow window = view.Window();
   ImageWindow::display_channel channel = window.CurrentChannel();

   // Update rectangle, integer viewport coordinates.
   // Inflated rectangle coordinates compensate for rounding errors.
   Rect r0 = window.ImageToViewport( rect ).TruncatedToInt().InflatedBy( 1 );

   g.EnableAntialiasing();
   g.SetCompositionOperator( CompositionOp::Difference );

   // Draw stars and their PSFs
   double penWidth = DisplayPixelRatio();
   for ( star_list::const_iterator star = c->stars.Begin(); star != c->stars.End(); ++star )
   {
      switch ( channel )
      {
      case DisplayChannel::Red:
         if ( star->channel != 0 )
            continue;
         break;
      case DisplayChannel::Green:
         if ( star->channel != 1 )
            continue;
         break;
      case DisplayChannel::Blue:
         if ( star->channel != 2 )
            continue;
         break;
      default:
         break;
      }
      DrawStar( g, penWidth, star, window, r0 );
   }
}

void DynamicPSFInterface::SaveSettings() const
{
   // Placeholder
}

void DynamicPSFInterface::LoadSettings()
{
   // Placeholder
}

void DynamicPSFInterface::ExecuteInstance( DynamicPSFInstance& instance )
{
   psf_data_set data;

   for ( size_type i = 0; i < instance.views.Length(); ++i )
   {
      PSFCollection* c = new PSFCollection( instance.views[i] );
      data.Add( c );

      for ( size_type j = 0; j < instance.stars.Length(); ++j )
      {
         const DynamicPSFInstance::Star& star = instance.stars[j];
         if ( star.view == i )
         {
            Star* s = c->AddStar( star );

            for ( size_type k = 0; k < instance.psfs.Length(); ++k )
            {
               const DynamicPSFInstance::PSF& psf = instance.psfs[k];
               if ( psf.star == j )
                  s->AddPSF( psf );
            }
         }
      }
   }

   instance.views.Clear();
   instance.stars.Clear();
   instance.psfs.Clear();

   for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
      if ( instance.regenerate )
         c->Regenerate( instance.threshold, instance.autoAperture, instance.psfOptions );
      else
         c->Recalculate( instance.threshold, instance.autoAperture );

   for ( psf_data_set::const_iterator c = data.Begin(); c != data.End(); ++c )
   {
      instance.views.Add( c->ViewId() );

      for ( star_list::const_iterator s = c->stars.Begin(); s != c->stars.End(); ++s )
      {
         instance.stars.Add( DynamicPSFInstance::Star( *s, instance.views.Length()-1 ) );

         for ( psf_list::const_iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
            instance.psfs.Add( DynamicPSFInstance::PSF( *p, instance.stars.Length()-1 ) );
      }
   }

   data.Destroy();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::UpdateControls()
{
   bool haveStars = !data.IsEmpty();
   GUI->ExpandAll_ToolButton.Enable( haveStars );
   GUI->CollapseAll_ToolButton.Enable( haveStars );
   GUI->DeleteStar_ToolButton.Enable( haveStars );
   GUI->TrackStars_ToolButton.Enable( haveStars );
   GUI->Regenerate_ToolButton.Enable( haveStars );
   GUI->RegenerateAll_ToolButton.Enable( haveStars );
   GUI->Sort_ToolButton.Enable( haveStars );
   GUI->ExportPSF_ToolButton.Enable( haveStars );
   GUI->AverageStars_ToolButton.Enable( haveStars );
   GUI->ExportCSV_ToolButton.Enable( haveStars );

   bool fixedPSF = !instance.psfOptions.autoPSF;
   GUI->AutoPSF_CheckBox.SetChecked( !fixedPSF );
   GUI->Gaussian_CheckBox.Enable( fixedPSF );
   GUI->Moffat_CheckBox.Enable( fixedPSF );
   GUI->MoffatA_CheckBox.Enable( fixedPSF );
   GUI->Moffat8_CheckBox.Enable( fixedPSF );
   GUI->Moffat6_CheckBox.Enable( fixedPSF );
   GUI->Moffat4_CheckBox.Enable( fixedPSF );
   GUI->Moffat25_CheckBox.Enable( fixedPSF );
   GUI->Moffat15_CheckBox.Enable( fixedPSF );
   GUI->Lorentzian_CheckBox.Enable( fixedPSF );

   GUI->SignedAngles_CheckBox.SetChecked( instance.signedAngles );

   GUI->SearchRadius_SpinBox.SetValue( instance.searchRadius );
   GUI->Threshold_NumericEdit.SetValue( instance.threshold );
   GUI->AutoAperture_CheckBox.SetChecked( instance.autoAperture );

   GUI->ScaleMode_ComboBox.SetCurrentItem( instance.scaleMode );

   GUI->ScaleValue_NumericEdit.SetValue( instance.scaleValue );
   GUI->ScaleValue_NumericEdit.Enable( instance.scaleMode == DPScaleMode::LiteralValue );

   GUI->ScaleKeyword_Edit.SetText( instance.scaleKeyword );
   GUI->ScaleKeyword_Label.Enable( instance.scaleMode == DPScaleMode::CustomKeyword );
   GUI->ScaleKeyword_Edit.Enable( instance.scaleMode == DPScaleMode::CustomKeyword );

   UpdateStarInfo();
}

static double KeywordValue( const FITSKeywordArray& keywords, const IsoString& keyName )
{
   for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
      if ( !i->name.CompareIC( keyName ) )
      {
         if ( i->IsNumeric() )
         {
            double v;
            if ( i->GetNumericValue( v ) )
               return v;
         }

         break;
      }

   return 0;
}

void DynamicPSFInterface::UpdateScaleItems()
{
   for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
      c->UpdateImageScale( instance.scaleMode, instance.scaleValue, instance.scaleKeyword );
   AdjustDataTreeColumns();
}

void DynamicPSFInterface::UpdateStarInfo()
{
   TreeBox::Node* node = GUI->Data_TreeBox.CurrentNode();
   if ( node == nullptr )
      GUI->StarInfo_Label.Clear();
   else
   {
      StarNode* s = dynamic_cast<StarNode*>( node );
      if ( s == nullptr )
      {
         PSFNode* psfNode = dynamic_cast<PSFNode*>( node );
         if ( psfNode != nullptr )
            s = dynamic_cast<StarNode*>( node->Parent() );
      }

      if ( s != nullptr )
      {
         String info;
         info.Format( "Star %5d of %5d",
                      GUI->Data_TreeBox.ChildIndex( s ) + 1,
                      s->Parent()->NumberOfChildren() );
         if ( !selectedStars.IsEmpty() )
            info.AppendFormat( " / %u selected", selectedStars.Length() );
         GUI->StarInfo_Label.SetText( info );
      }
      else
      {
         PSFCollectionNode* c = dynamic_cast<PSFCollectionNode*>( node );
         if ( c != nullptr )
         {
            String info = String().Format( "%5u stars", c->collection->stars.Length() );
            if ( c->collection->xScale > 0 && c->collection->yScale > 0 )
               info += String().Format( " | x-scale=%.2f \"/px | y-scale=%.2f \"/px",
                                        c->collection->xScale, c->collection->yScale );
            GUI->StarInfo_Label.SetText( info );
         }
         else
            GUI->StarInfo_Label.Clear();
      }
   }
}

void DynamicPSFInterface::RegenerateDataTree()
{
   GUI->Data_TreeBox.Clear();
   GUI->Data_TreeBox.DisableUpdates();

   for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
   {
      new PSFCollectionNode( GUI->Data_TreeBox, c );
      c->CreateStarNodes();
   }

   GUI->Data_TreeBox.EnableUpdates();

   AdjustDataTreeColumns();
}

void DynamicPSFInterface::AdjustDataTreeColumns()
{
   for ( int i = 0, n = GUI->Data_TreeBox.NumberOfColumns(); i < n; ++i )
      GUI->Data_TreeBox.AdjustColumnWidthToContents( i );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::__CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   UpdateStarInfo();

   if ( GUI->TrackStars_ToolButton.IsChecked() )
      TrackStar( StarFromTreeBoxNode( current ) );
}

void DynamicPSFInterface::__NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   Star* star = StarFromTreeBoxNode( node );
   if ( star != nullptr )
   {
      TrackStar( star );
      SelectStar( star );
   }
   else
   {
      PSFCollectionNode* collectionNode = dynamic_cast<PSFCollectionNode*>( &node );
      if ( collectionNode != nullptr )
         collectionNode->collection->view.Window().BringToFront();
   }
}

void DynamicPSFInterface::__NodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col )
{
}

void DynamicPSFInterface::__NodeExpanded( TreeBox& sender, TreeBox::Node& node )
{
   AdjustDataTreeColumns();
}

void DynamicPSFInterface::__NodeCollapsed( TreeBox& sender, TreeBox::Node& node )
{
}

void DynamicPSFInterface::__NodeSelectionUpdated( TreeBox& sender )
{
   star_list stars;
   IndirectArray<TreeBox::Node> nodes = sender.SelectedNodes();
   for ( size_type i = 0; i < nodes.Length(); ++i )
   {
      Star* star = StarFromTreeBoxNode( *nodes[i] );
      if ( star != nullptr )
         if ( !stars.Contains( star ) )
            stars.Add( star );
   }
   SelectStars( stars );
}

void DynamicPSFInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->ExpandAll_ToolButton )
   {
      for ( int n = GUI->Data_TreeBox.NumberOfChildren(), i = n; --i >= 0; )
         ExpandTreeBoxNodes( GUI->Data_TreeBox[i] );
      AdjustDataTreeColumns();
   }
   else if ( sender == GUI->CollapseAll_ToolButton )
   {
      for ( int n = GUI->Data_TreeBox.NumberOfChildren(), i = n; --i >= 0; )
         CollapseTreeBoxNodes( GUI->Data_TreeBox[i] );
      AdjustDataTreeColumns();
   }
   else if ( sender == GUI->DeleteStar_ToolButton )
   {
      IndirectArray<TreeBox::Node> nodes = GUI->Data_TreeBox.SelectedNodes();
      if ( nodes.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected items.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
      }
      else
      {
         ReferenceArray<PSF> psfs;
         ReferenceArray<Star> stars;
         ReferenceArray<PSFCollection> psfCollections;

         for ( IndirectArray<TreeBox::Node>::iterator i = nodes.Begin(); i != nodes.End(); ++i )
         {
            PSFNode* psfNode = dynamic_cast<PSFNode*>( *i );
            if ( psfNode != nullptr )
               psfs.Add( psfNode->psf );
            else
            {
               StarNode* starNode = dynamic_cast<StarNode*>( *i );
               if ( starNode != nullptr )
                  stars.Add( starNode->star );
               else
               {
                  PSFCollectionNode* psfCollectionNode = dynamic_cast<PSFCollectionNode*>( *i );
                  if ( psfCollectionNode != nullptr )
                     psfCollections.Add( psfCollectionNode->collection );
               }
            }
         }

         GUI->Data_TreeBox.DisableUpdates();

         for ( int step = 0; !nodes.IsEmpty() && step < 3; nodes = GUI->Data_TreeBox.SelectedNodes(), ++step )
         {
            IndirectArray<TreeBox::Node> stepNodes;

            for ( IndirectArray<TreeBox::Node>::iterator i = nodes.Begin(); i != nodes.End(); ++i )
               switch ( step )
               {
               case 0:
                  if ( dynamic_cast<PSFCollectionNode*>( *i ) )
                     stepNodes.Add( *i );
                  break;
               case 1:
                  if ( dynamic_cast<StarNode*>( *i ) )
                     stepNodes.Add( *i );
                  break;
               case 2:
                  if ( dynamic_cast<PSFNode*>( *i ) )
                     stepNodes.Add( *i );
                  break;
               }

            for ( IndirectArray<TreeBox::Node>::iterator i = stepNodes.Begin(); i != stepNodes.End(); ++i )
               DeleteTreeBoxNode( *i );
         }

         GUI->Data_TreeBox.EnableUpdates();

         for ( ReferenceArray<PSFCollection>::iterator c = psfCollections.Begin(); c != psfCollections.End(); ++c )
         {
            for ( star_list::iterator s = c->stars.Begin(); s != c->stars.End(); ++s )
            {
               if ( s->selected )
                  selectedStars.Remove( selectedStars.Search( (Star*)s ) );

               for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
                  psfs.Remove( psfs.Search( (PSF*)p ) );

               stars.Remove( stars.Search( (Star*)s ) );
            }

            c->Update();
            data.Destroy( data.Search( (PSFCollection*)c ) );
         }

         for ( ReferenceArray<Star>::iterator s = stars.Begin(); s != stars.End(); ++s )
         {
            if ( s->selected )
               selectedStars.Remove( selectedStars.Search( (Star*)s ) );

            for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
               psfs.Remove( psfs.Search( (PSF*)p ) );

            s->Update();
            s->collection->stars.Destroy( s->collection->stars.Search( (Star*)s ) );
         }

         for ( ReferenceArray<PSF>::iterator p = psfs.Begin(); p != psfs.End(); ++p )
         {
            p->star->Update();
            p->star->psfs.Destroy( p->star->psfs.Search( (PSF*)p ) );
         }

         TreeBox::Node* current = GUI->Data_TreeBox.CurrentNode();
         if ( current != nullptr )
         {
            current->Select();
            SelectStar( StarFromTreeBoxNode( *current ) );
         }

         UpdateControls();
      }
   }
   else if ( sender == GUI->TrackStars_ToolButton )
   {
      if ( checked )
      {
         TreeBox::Node* current = GUI->Data_TreeBox.CurrentNode();
         if ( current != nullptr )
            TrackStar( StarFromTreeBoxNode( *current ) );
      }
   }
   else if ( sender == GUI->Regenerate_ToolButton )
   {
      if ( selectedStars.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected stars to regenerate.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
      }
      else
      {
         GUI->StarInfo_Label.SetText( "Regenerating selection..." );
         ProcessEvents();

         GUI->Data_TreeBox.DisableUpdates();

         for ( star_list::iterator s = selectedStars.Begin(); s != selectedStars.End(); ++s )
         {
            s->DestroyPSFNodes();
            s->Regenerate( instance.threshold, instance.autoAperture, instance.psfOptions );
            s->CreatePSFNodes();
            s->Update();
         }

         GUI->Data_TreeBox.EnableUpdates();

         GUI->StarInfo_Label.Clear();
      }
   }
   else if ( sender == GUI->RegenerateAll_ToolButton )
   {
      star_list wereSelected = selectedStars;
      UnselectStars();

      GUI->StarInfo_Label.SetText( "Regenerating..." );
      ProcessEvents();

      for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
         c->Regenerate( instance.threshold, instance.autoAperture, instance.psfOptions );

      RegenerateDataTree();

      GUI->StarInfo_Label.Clear();

      Update();
      UpdateControls();
      SelectStars( wereSelected );
   }
   else if ( sender == GUI->Sort_ToolButton )
   {
      if ( sortStarsDialog == nullptr )
         sortStarsDialog = new SortStarsDialog;
      if ( sortStarsDialog->Execute() )
      {
         GUI->StarInfo_Label.SetText( "Sorting..." );
         ProcessEvents();

         for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
            c->Sort( sortStarsDialog->sortBy );

         RegenerateDataTree();

         GUI->StarInfo_Label.Clear();

         for ( star_list::iterator s = selectedStars.Begin(); s != selectedStars.End(); ++s )
            if ( s->node != nullptr )
               s->node->Select();
      }
   }
   else if ( sender == GUI->ExportPSF_ToolButton )
   {
      if ( selectedStars.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected stars to generate a synthetic PSF.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
         return;
      }

      FMatrix R;
      for ( star_list::iterator s = selectedStars.Begin(); s != selectedStars.End(); ++s )
      {
         for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
         {
            FMatrix r;
            if ( p->function == PSFFit::Gaussian )
               r = GaussianFilter( float( p->sx ), 0.01F, p->sy/p->sx, Rad( p->theta ) ).Coefficients();
            else // Moffat
               r = MoffatFilter( float( p->sx ), p->beta, 0.01F, p->sy/p->sx, Rad( p->theta ) ).Coefficients();

            if ( R.Rows() < r.Rows() )
               Swap( R, r );

            for ( int i0 = 0, k = (R.Rows()-r.Rows())>>1, i = k; i0 < r.Rows(); ++i0, ++i )
               for ( int j0 = 0, j = k; j0 < r.Cols(); ++j0, ++j )
                  R[i][j] += r[i0][j0];
         }
      }

      ImageWindow window( 1, 1, 1, 32, true/*floatSample*/, false/*color*/, true/*initialProcessing*/, "PSF" );
      ImageVariant image = window.MainView().Image();
      R.ToImage( image );
      image.Rescale();
      window.Show();
      window.ZoomToFit( false/*allowMagnification*/ );
   }
   else if ( sender == GUI->AverageStars_ToolButton )
   {
      if ( selectedStars.IsEmpty() )
      {
         pcl::MessageBox( "<p>There are no selected stars to average.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
         return;
      }

      PSFCollection* collection = nullptr;
      int gaussian = -1;
      int count = 0;
      for ( star_list::iterator s = selectedStars.Begin(); s != selectedStars.End(); ++s )
      {
         if ( collection == nullptr )
            collection = s->collection;
         else if ( s->collection != collection )
         {
            pcl::MessageBox( "<p>Cannot average stars from different images.</p>",
                              "DynamicPSF",
                              StdIcon::Error ).Execute();
            return;
         }

         for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
            if ( *p )
            {
               if ( gaussian < 0 )
                  gaussian = p->function == PSFFit::Gaussian;
               else if ( gaussian != (p->function == PSFFit::Gaussian) )
               {
                  pcl::MessageBox( "<p>Incongruent PSF functions. Cannot average Gaussian and Moffat functions.</p>",
                                    "DynamicPSF",
                                    StdIcon::Error ).Execute();
                  return;
               }

               ++count;
            }
      }

      if ( count == 0 )
      {
         pcl::MessageBox( "<p>There are no valid PSF functions among the selected stars.</p>",
                           "DynamicPSF",
                           StdIcon::Error ).Execute();
         return;
      }

      AverageStarDialog( selectedStars, collection->xScale, collection->yScale ).Execute();
   }
   else if ( sender == GUI->ExportCSV_ToolButton )
   {
      FileFilter csvFiles;
      csvFiles.SetDescription( "CSV Files" );
      csvFiles.AddExtension( ".csv" );
      FileFilter txtFiles;
      txtFiles.SetDescription( "Plain Text Files" );
      txtFiles.AddExtension( ".txt" );
      FileFilter anyFiles;
      anyFiles.SetDescription( "All Files" );
      anyFiles.AddExtension( "*" );

      SaveFileDialog d;
      d.EnableOverwritePrompt();
      d.Filters().Add( csvFiles );
      d.Filters().Add( txtFiles );
      d.Filters().Add( anyFiles );
      d.SetCaption( "DynamicPSF: Export As CSV File" );
      if ( d.Execute() )
         ExportCSV( d.FileName() );
   }
   else if ( sender == GUI->AutoPSF_CheckBox )
   {
      instance.psfOptions.autoPSF = checked;
      UpdateControls();
   }
   else if ( sender == GUI->Gaussian_CheckBox )
      instance.psfOptions.gaussian = checked;
   else if ( sender == GUI->Moffat_CheckBox )
      instance.psfOptions.moffat = checked;
   else if ( sender == GUI->MoffatA_CheckBox )
      instance.psfOptions.moffatA = checked;
   else if ( sender == GUI->Moffat8_CheckBox )
      instance.psfOptions.moffat8 = checked;
   else if ( sender == GUI->Moffat6_CheckBox )
      instance.psfOptions.moffat6 = checked;
   else if ( sender == GUI->Moffat4_CheckBox )
      instance.psfOptions.moffat4 = checked;
   else if ( sender == GUI->Moffat25_CheckBox )
      instance.psfOptions.moffat25 = checked;
   else if ( sender == GUI->Moffat15_CheckBox )
      instance.psfOptions.moffat15 = checked;
   else if ( sender == GUI->Lorentzian_CheckBox )
      instance.psfOptions.lorentzian = checked;
   else if ( sender == GUI->CircularPSF_CheckBox )
      instance.psfOptions.circular = checked;
   else if ( sender == GUI->SignedAngles_CheckBox )
   {
      instance.signedAngles = checked;
      for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
         c->UpdateNodes();
      AdjustDataTreeColumns();
   }
   else if ( sender == GUI->AutoAperture_CheckBox )
      instance.autoAperture = checked;
}

void DynamicPSFInterface::__NumericEdit_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Threshold_NumericEdit )
      instance.threshold = value;
   else if ( sender == GUI->ScaleValue_NumericEdit )
   {
      instance.scaleValue = value;
      UpdateControls();
      UpdateScaleItems();
   }
}

void DynamicPSFInterface::__SpinBox_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->SearchRadius_SpinBox )
      instance.searchRadius = value;
}

void DynamicPSFInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ScaleMode_ComboBox )
   {
      instance.scaleMode = itemIndex;
      UpdateControls();
      UpdateScaleItems();
   }
}

void DynamicPSFInterface::__EditCompleted( Edit& sender )
{
   if ( sender == GUI->ScaleKeyword_Edit )
   {
      instance.scaleKeyword = sender.Text().Trimmed();
      UpdateControls();
      UpdateScaleItems();
   }
}

void DynamicPSFInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->Data_TreeBox.SetFixedHeight();
   else
   {
      GUI->Data_TreeBox.SetMinHeight( DATATREE_MINHEIGHT( Font() ) );
      GUI->Data_TreeBox.SetMaxHeight( int_max );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DynamicPSFInterface::PSFCollection* DynamicPSFInterface::AcquirePSFCollection( View& view )
{
   PSFCollection* c = FindPSFCollection( view );
   if ( c == nullptr )
   {
      data.Add( c = new PSFCollection( view ) );
      c->UpdateImageScale( instance.scaleMode, instance.scaleValue, instance.scaleKeyword );
      new PSFCollectionNode( GUI->Data_TreeBox, c );
   }
   return c;
}

DynamicPSFInterface::star_list DynamicPSFInterface::FindStars(
               const View& view, const DRect& rect, ImageWindow::display_channel channel )
{
   star_list stars;
   PSFCollection* c = FindPSFCollection( view );
   if ( c != nullptr )
      for ( star_list::iterator s = c->stars.Begin(); s != c->stars.End(); ++s )
      {
         switch ( channel )
         {
         case DisplayChannel::Red:
            if ( s->channel != 0 )
               continue;
            break;
         case DisplayChannel::Green:
            if ( s->channel != 1 )
               continue;
            break;
         case DisplayChannel::Blue:
            if ( s->channel != 2 )
               continue;
            break;
         default:
            break;
         }

         if ( s->rect.Intersects( rect ) )
            stars.Add( s );
      }
   return stars;
}

void DynamicPSFInterface::SelectStars( const star_list& stars, bool addToSelection )
{
   if ( addToSelection )
   {
      for ( star_list::const_iterator s = stars.Begin(); s != stars.End(); ++s )
         if ( !s->selected )
            selectedStars.Add( s );
   }
   else
   {
      for ( star_list::iterator s = selectedStars.Begin(); s != selectedStars.End(); ++s )
      {
         s->selected = false;
         s->Update();
         if ( s->node != nullptr )
            s->node->Unselect();
      }

      selectedStars = stars;
   }

   for ( star_list::iterator s = selectedStars.Begin(); s != selectedStars.End(); ++s )
   {
      s->selected = true;
      s->Update();
      if ( s->node != nullptr )
         s->node->Select();
   }

   UpdateStarInfo();
}

void DynamicPSFInterface::TrackStar( const Star* star )
{
   if ( star != nullptr )
   {
      ImageWindow window = star->collection->view.Window();
      window.SetViewport( star->pos, window.ZoomFactor() );
      if ( window != ImageWindow::ActiveWindow() )
      {
         window.BringToFront();
         ActivateWindow();
      }
   }
}

void DynamicPSFInterface::ExportCSV( const String& filePath )
{
   File f;
   f.CreateForWriting( filePath );

   f.OutTextLn( "ViewId,StarId,Channel,Function,B,A,cx,cy,sx,sy,FWHMx,FWHMy,unit,r,theta,beta,MAD" );

   for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
   {
      IsoString viewId = c->ViewId();

      const char* scaleUnit = "px";
      double xScale = 1, yScale = 1;
      if ( c->xScale > 0 && c->yScale > 0 )
      {
         scaleUnit = "\"";
         xScale = c->xScale;
         yScale = c->yScale;
      }

      for ( star_list::iterator s = c->stars.Begin(); s != c->stars.End(); ++s )
      {
         IsoString viewIdAndStarIdAndCh = viewId + ',' + IsoString( s->uniqueId ) + ',' + IsoString( s->channel ) + ',';

         for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
            f.OutTextLn( viewIdAndStarIdAndCh +
                         IsoString( p->FunctionToString() ) + ',' +
                         IsoString().Format( "%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%s,%.3f,%.2f,%.2f,%.3e",
                                             p->B,
                                             p->A,
                                             p->c0.x,
                                             p->c0.y,
                                             p->sx,
                                             p->sy,
                                             p->FWHMx()*xScale,
                                             p->FWHMy()*yScale,
                                             scaleUnit,
                                             p->sy/p->sx,
                                             (*signedAngles && p->theta > 90) ? p->theta - 180 : p->theta,
                                             p->beta,
                                             p->mad ) );
      }
   }

   f.Flush();
   f.Close();
}

void DynamicPSFInterface::DrawStar( VectorGraphics& g, double penWidth, const Star* star,
                                    ImageWindow& window, const Rect& r0 ) const
{
   DRect r = window.ImageToViewport( star->rect );
   if ( r0.Intersects( r ) )
   {
      g.SetPen( star->selected ? instance.selectedStarColor : instance.starColor, penWidth );

      if ( *star )
      {
         g.StrokeRect( r );

         g.EnableAntialiasing();

         for ( psf_list::const_iterator psf = star->psfs.Begin(); psf != star->psfs.End(); ++psf )
         {
            DPoint d = window.ImageToViewport( psf->c0 );
            g.TranslateTransformation( d.x, d.y );
            g.RotateTransformation( Rad( psf->theta ) );
            double dr = penWidth * CENTER_RADIUS;
            g.DrawLine( -dr, 0, +dr, 0 );
            g.DrawLine( 0, -dr, 0, +dr );

            double rx = window.ImageScalarToViewport( psf->FWHMx() );
            double ry = window.ImageScalarToViewport( psf->FWHMy() );
            if ( rx > 0.5 && ry > 0.5 )
               g.StrokeEllipse( -rx, -ry, +rx, +ry );

            g.ResetTransformation();
         }

         g.DisableAntialiasing();
      }
      else
      {
         g.SetBrush( instance.badStarFillColor );
         g.DrawRect( r );
      }
   }
}

DynamicPSFInterface::Star* DynamicPSFInterface::StarFromTreeBoxNode( TreeBox::Node& node )
{
   StarNode* starNode = dynamic_cast<StarNode*>( &node );
   if ( starNode != nullptr )
      return starNode->star;
   PSFNode* psfNode = dynamic_cast<PSFNode*>( &node );
   if ( psfNode != nullptr )
      return psfNode->psf->star;
   return nullptr;
}

void DynamicPSFInterface::DeleteTreeBoxNode( TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      for ( int n = node->NumberOfChildren(), i = n; --i >= 0; )
         DeleteTreeBoxNode( node->Child( i ) );

      int index = GUI->Data_TreeBox.ChildIndex( node );
      if ( node->Parent() == nullptr )
         GUI->Data_TreeBox.Remove( index );
      else
         node->Parent()->Remove( index );
      delete node;
   }
}

void DynamicPSFInterface::ExpandTreeBoxNodes( TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      for ( int n = node->NumberOfChildren(), i = n; --i >= 0; )
         ExpandTreeBoxNodes( node->Child( i ) );
      node->Expand();
   }
}

void DynamicPSFInterface::CollapseTreeBoxNodes( TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      for ( int n = node->NumberOfChildren(), i = n; --i >= 0; )
         CollapseTreeBoxNodes( node->Child( i ) );
      node->Collapse();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::Star::Regenerate( const DynamicPSFInterface::PSFOptions& options )
{
   ImageVariant img = collection->view.Image();
   Regenerate( img, options );
}

void DynamicPSFInterface::Star::Regenerate( const ImageVariant& img, const DynamicPSFInterface::PSFOptions& options )
{
   psfs.Destroy();

   if ( !*this )
      return;

   if ( options.autoPSF )
   {
      Array<PSFFit> fits;

      PSFFit Fg( img, pos, rect, PSFFit::Gaussian, options.circular );
      if ( Fg )
         fits.Add( Fg );

      PSFFit Fm( img, pos, rect, PSFFit::Moffat, options.circular );
      if ( Fm )
         fits.Add( Fm );
      else
      {
         /*
          * Moffat fit didn't converge - try all fixed beta functions
          */
         PSFFit FA( img, pos, rect, PSFFit::MoffatA, options.circular );
         if ( FA )
            fits.Add( FA );
         PSFFit F8( img, pos, rect, PSFFit::Moffat8, options.circular );
         if ( F8 )
            fits.Add( F8 );
         PSFFit F6( img, pos, rect, PSFFit::Moffat6, options.circular );
         if ( F6 )
            fits.Add( F6 );
         PSFFit F4( img, pos, rect, PSFFit::Moffat4, options.circular );
         if ( F4 )
            fits.Add( F4 );
         PSFFit F25( img, pos, rect, PSFFit::Moffat25, options.circular );
         if ( F25 )
            fits.Add( F25 );
         PSFFit F15( img, pos, rect, PSFFit::Moffat15, options.circular );
         if ( F15 )
            fits.Add( F15 );
         PSFFit F1( img, pos, rect, PSFFit::Lorentzian, options.circular );
         if ( F1 )
            fits.Add( F1 );
      }

      if ( !fits.IsEmpty() )
      {
         PSFFit F = fits[0];
         for ( size_type i = 1; i < fits.Length(); ++i )
            if ( fits[i].psf.mad < F.psf.mad )
               F = fits[i];
         AddPSF( F.psf );
      }
   }
   else
   {
      if ( options.gaussian )
      {
         PSFFit F( img, pos, rect, PSFFit::Gaussian, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat )
      {
         PSFFit F( img, pos, rect, PSFFit::Moffat, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffatA )
      {
         PSFFit F( img, pos, rect, PSFFit::MoffatA, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat8 )
      {
         PSFFit F( img, pos, rect, PSFFit::Moffat8, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat6 )
      {
         PSFFit F( img, pos, rect, PSFFit::Moffat6, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat4 )
      {
         PSFFit F( img, pos, rect, PSFFit::Moffat4, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat25 )
      {
         PSFFit F( img, pos, rect, PSFFit::Moffat25, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.moffat15 )
      {
         PSFFit F( img, pos, rect, PSFFit::Moffat15, options.circular );
         if ( F )
            AddPSF( F.psf );
      }

      if ( options.lorentzian )
      {
         PSFFit F( img, pos, rect, PSFFit::Lorentzian, options.circular );
         if ( F )
            AddPSF( F.psf );
      }
   }
}

void DynamicPSFInterface::Star::Regenerate( float threshold, bool autoAperture,
                                            const DynamicPSFInterface::PSFOptions& options )
{
   ImageVariant img = collection->view.Image();
   Regenerate( img, threshold, autoAperture, options );
}

void DynamicPSFInterface::Star::Regenerate( const ImageVariant& img, float threshold, bool autoAperture,
                                            const DynamicPSFInterface::PSFOptions& options )
{
   StarDetector D( collection->image, channel, pos, RoundI( rect.Width()/2 ), threshold, autoAperture );
   AssignData( D.star );
   Regenerate( img, options );
}

void DynamicPSFInterface::Star::Recalculate( float threshold, bool autoAperture )
{
   ImageVariant img = collection->view.Image();
   Recalculate( img, threshold, autoAperture );
}

void DynamicPSFInterface::Star::Recalculate( const ImageVariant& img, float threshold, bool autoAperture )
{
   StarDetector D( collection->image, channel, pos, RoundI( rect.Width()/2 ), threshold, autoAperture );
   if ( D )
   {
      AssignData( D.star );
      for ( psf_list::iterator p = psfs.Begin(); p != psfs.End(); ++p )
      {
         PSFFit F( img, pos, rect, PSFFit::Function( p->function ), p->circular );
         if ( F )
            p->AssignData( F.psf );
         else
            p->status = F.psf.status;
      }
   }
   else
   {
      status = D.star.status;
      for ( psf_list::iterator p = psfs.Begin(); p != psfs.End(); ++p )
         p->status = PSFFit::NotFitted;
   }
}

void DynamicPSFInterface::Star::Update()
{
   if ( !collection->view.IsNull() )
   {
      ImageWindow window = collection->view.Window();
      window.UpdateViewportRect( window.ImageToViewport( rect ).RoundedToInt().InflatedBy( 1 ) );
   }
}

void DynamicPSFInterface::Star::UpdateNodes()
{
   if ( node != nullptr )
   {
      for ( psf_list::iterator p = psfs.Begin(); p != psfs.End(); ++p )
         if ( p->node != nullptr )
            p->node->Update();
      node->Update();
   }
}

void DynamicPSFInterface::Star::DestroyPSFNodes()
{
   if ( node != nullptr )
      for ( psf_list::iterator p = psfs.Begin(); p != psfs.End(); ++p )
         if ( p->node != nullptr )
         {
            // PSF nodes are terminal
            node->Remove( node->ParentTree().ChildIndex( p->node ) );
            delete p->node;
         }
}

void DynamicPSFInterface::Star::CreatePSFNodes()
{
   if ( node != nullptr )
      for ( psf_list::iterator p = psfs.Begin(); p != psfs.End(); ++p )
         new PSFNode( *node, p );
}

double DynamicPSFInterface::Star::SortingValue( DynamicPSFInterface::SortingCriterion sortBy ) const
{
   if ( sortBy == DynamicPSFInterface::SortByStarId )
      return uniqueId;

   if ( psfs.IsEmpty() )
      return 0;

   switch ( sortBy )
   {
   case DynamicPSFInterface::SortByBackground:
      {
         double B = psfs[0].B;
         for ( psf_list::const_iterator i = psfs.Begin(); ++i != psfs.End(); )
            if ( i->B < B )
               B = i->B;
         return B;
      }

   case DynamicPSFInterface::SortByAmplitude:
      {
         double A = psfs[0].A;
         for ( psf_list::const_iterator i = psfs.Begin(); ++i != psfs.End(); )
            if ( i->A < A )
               A = i->A;
         return A;
      }

   case DynamicPSFInterface::SortBySigma:
      {
         double s = Max( psfs[0].sx, psfs[0].sy );
         for ( psf_list::const_iterator i = psfs.Begin(); ++i != psfs.End(); )
         {
            double si = Max( i->sx, i->sy );
            if ( si < s )
               s = si;
         }
         return s;
      }

   case DynamicPSFInterface::SortByAspectRatio:
      {
         double r = psfs[0].sy/psfs[0].sx;
         for ( psf_list::const_iterator i = psfs.Begin(); ++i != psfs.End(); )
         {
            double ri = i->sy/i->sx;
            if ( ri < r )
               r = ri;
         }
         return r;
      }

   case DynamicPSFInterface::SortByRotationAngle:
      {
         double theta = psfs[0].theta;
         if ( *signedAngles )
            if ( theta > 90 )
               theta -= 180;
         for ( psf_list::const_iterator i = psfs.Begin(); ++i != psfs.End(); )
         {
            double itheta = i->theta;
            if ( *signedAngles )
               if ( itheta > 90 )
                  itheta -= 180;
            if ( itheta < theta )
               theta = itheta;
         }
         return theta;
      }

   case DynamicPSFInterface::SortByAbsRotationAngle:
      {
         double theta = psfs[0].theta;
         for ( psf_list::const_iterator i = psfs.Begin(); ++i != psfs.End(); )
            if ( i->theta < theta )
               theta = i->theta;
         return theta;
      }

   case DynamicPSFInterface::SortByShape:
      {
         double beta = 0;
         for ( psf_list::const_iterator i = psfs.Begin(); i != psfs.End(); ++i )
            if ( i->function != PSFFit::Gaussian )
            {
               beta = i->beta;
               for ( psf_list::const_iterator j = i; ++j != psfs.End(); )
                  if ( j->beta < beta )
                     beta = j->beta;
               break;
            }
         return beta;
      }

   default:
   case DynamicPSFInterface::SortByMAD:
      {
         double mad = psfs[0].mad;
         for ( psf_list::const_iterator i = psfs.Begin(); ++i != psfs.End(); )
            if ( i->mad < mad )
               mad = i->mad;
         return mad;
      }
   }

   return 0; // ?
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DynamicPSFInterface::PSFCollection::UpdateDetectionImage()
{
   image.FreeData();

   if ( !view.IsNull() )
   {
      ImageVariant v = view.Image();
      ImageVariant( &image ).CopyImage( v );
      image.ResetSelections();
      image.Status().DisableInitialization();

      // Apply a 5x5 Gaussian blur for noise reduction
      static float G5hv[] = { 0.010000, 0.316228, 1.000000, 0.316228, 0.010000 };
      SeparableFilter G5( G5hv, G5hv, 5 );
      SeparableConvolution C( G5 );
      C >> image;
   }
}

void DynamicPSFInterface::PSFCollection::Regenerate( float threshold, bool autoAperture,
                                                     const DynamicPSFInterface::PSFOptions& options )
{
   if ( view.IsNull() )
   {
      for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
      {
         s->status = StarDetector::NotDetected;
         s->psfs.Destroy();
      }
   }
   else
   {
      ImageVariant img = view.Image();
      int numberOfThreads = Thread::NumberOfThreads( stars.Length(), 4 );
      if ( numberOfThreads == 1 )
      {
         for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
            s->Regenerate( img, threshold, autoAperture, options );
      }
      else
      {
         int starsPerThread = stars.Length()/numberOfThreads;
         ReferenceArray<RegenerateThread> threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new RegenerateThread( img, threshold, autoAperture, options,
                                               stars.At( i*starsPerThread ),
                                               (j < numberOfThreads) ? stars.At( j*starsPerThread ) : stars.End() ) );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Wait();
         threads.Destroy();
      }
   }
}

void DynamicPSFInterface::PSFCollection::Recalculate( float threshold, bool autoAperture )
{
   if ( view.IsNull() )
   {
      for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
      {
         s->status = StarDetector::NotDetected;
         for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
            p->status = PSFFit::NotFitted;
      }
   }
   else
   {
      ImageVariant img = view.Image();
      int numberOfThreads = Thread::NumberOfThreads( stars.Length(), 4 );
      if ( numberOfThreads == 1 )
      {
         for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
            s->Recalculate( img, threshold, autoAperture );
      }
      else
      {
         int starsPerThread = stars.Length()/numberOfThreads;
         ReferenceArray<RecalculateThread> threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new RecalculateThread( img, threshold, autoAperture,
                                                stars.At( i*starsPerThread ),
                                                (j < numberOfThreads) ? stars.At( j*starsPerThread ) : stars.End() ) );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Wait();
         threads.Destroy();
      }
   }
}

void DynamicPSFInterface::PSFCollection::Update()
{
   if ( !view.IsNull() )
   {
      ImageWindow window = view.Window();
      if ( view == window.CurrentView() )
      {
         Rect visibleRect = window.VisibleViewportRect();
         for ( star_list::const_iterator s = stars.Begin(); s != stars.End(); ++s )
         {
            Rect r = window.ImageToViewport( s->rect ).RoundedToInt().InflatedBy( 1 );
            if ( visibleRect.Intersects( r ) )
               window.UpdateViewportRect( r );
         }
      }
   }
}

void DynamicPSFInterface::PSFCollection::Update( const Rect& rect )
{
   if ( !view.IsNull() )
   {
      ImageWindow window = view.Window();
      if ( view == window.CurrentView() )
      {
         Rect visibleRect = window.VisibleViewportRect();
         for ( star_list::const_iterator s = stars.Begin(); s != stars.End(); ++s )
            if ( rect.Intersects( s->rect ) )
            {
               Rect r = window.ImageToViewport( s->rect ).RoundedToInt().InflatedBy( 1 );
               if ( visibleRect.Intersects( r ) )
                  window.UpdateViewportRect( r );
            }
      }
   }
}

void DynamicPSFInterface::PSFCollection::UpdateImageScale( pcl_enum scaleMode, float scaleValue, const IsoString& scaleKeyword )
{
   xScale = yScale = 0;

   if ( !view.IsNull() )
      switch ( scaleMode )
      {
      default:
      case DPScaleMode::Pixels:
         break;

      case DPScaleMode::StandardKeywords:
         {
            FITSKeywordArray keywords;
            view.Window().GetKeywords( keywords );
            double focalLength = KeywordValue( keywords, "FOCALLEN" ); // mm
            double xPixSize = KeywordValue( keywords, "XPIXSZ" ); // um
            double yPixSize = KeywordValue( keywords, "YPIXSZ" ); // um

            if ( focalLength > 0 )
            {
               if ( yPixSize <= 0 )
                  yPixSize = xPixSize;
               if ( xPixSize > 0 )
               {
                  xScale = 3.6 * Deg( 2*ArcTan( xPixSize, 2*focalLength ) );
                  yScale = 3.6 * Deg( 2*ArcTan( yPixSize, 2*focalLength ) );
               }
            }
         }
         break;

      case DPScaleMode::CustomKeyword:
         {
            FITSKeywordArray keywords;
            view.Window().GetKeywords( keywords );
            xScale = yScale = KeywordValue( keywords, scaleKeyword );
         }
         break;

      case DPScaleMode::LiteralValue:
         xScale = yScale = scaleValue;
         break;
      }

      if ( node != nullptr )
         for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
            if ( s->node != nullptr )
               for ( psf_list::iterator p = s->psfs.Begin(); p != s->psfs.End(); ++p )
                  if ( p->node != nullptr )
                     p->node->Update();
}

void DynamicPSFInterface::PSFCollection::UpdateNodes()
{
   if ( node != nullptr )
   {
      for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
         s->UpdateNodes();
      node->Update();
   }
}

void DynamicPSFInterface::PSFCollection::DestroyStarNodes()
{
   if ( node != nullptr )
      for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
      {
         if ( s->node != nullptr )
         {
            s->DestroyPSFNodes();
            node->Remove( node->ParentTree().ChildIndex( s->node ) );
            delete s->node;
         }
      }
}

void DynamicPSFInterface::PSFCollection::CreateStarNodes()
{
   if ( node != nullptr )
      for ( star_list::iterator s = stars.Begin(); s != stars.End(); ++s )
      {
         new StarNode( *node, s );
         s->CreatePSFNodes();
      }
}

class StarSortingBinaryPredicate
{
public:

   typedef DynamicPSFInterface::psf_list psf_list;

   StarSortingBinaryPredicate( DynamicPSFInterface::SortingCriterion s ) : sortBy( s )
   {
   }

   bool operator ()( const DynamicPSFInterface::Star& s1, const DynamicPSFInterface::Star& s2 ) const
   {
      return s1.SortingValue( sortBy ) < s2.SortingValue( sortBy );
   }

private:

   DynamicPSFInterface::SortingCriterion sortBy;
};

void DynamicPSFInterface::PSFCollection::Sort( DynamicPSFInterface::SortingCriterion sortBy )
{
   stars.Sort( StarSortingBinaryPredicate( sortBy ) );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DynamicPSFInterface::GUIData::GUIData( DynamicPSFInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Image scale (arcsec/px):" ) + 'T' );
   int editWidth1 = fnt.Width( String( '0', 20 ) );

   //

   Data_TreeBox.SetMinHeight( DATATREE_MINHEIGHT( w.Font() ) );
   Data_TreeBox.SetNumberOfColumns( 15 );
   Data_TreeBox.SetHeaderText(  0, "" );   // icon/view-id/star-no/PSF-func
   Data_TreeBox.SetHeaderText(  1, "Ch" );
   Data_TreeBox.SetHeaderText(  2, "B" );
   Data_TreeBox.SetHeaderText(  3, "A" );
   Data_TreeBox.SetHeaderText(  4, "cx" );
   Data_TreeBox.SetHeaderText(  5, "cy" );
   Data_TreeBox.SetHeaderText(  6, "sx" );
   Data_TreeBox.SetHeaderText(  7, "sy" );
   Data_TreeBox.SetHeaderText(  8, "FWHMx" );
   Data_TreeBox.SetHeaderText(  9, "FWHMy" );
   Data_TreeBox.SetHeaderText( 10, "r" );
   Data_TreeBox.SetHeaderText( 11, "theta" );
   Data_TreeBox.SetHeaderText( 12, "beta" );
   Data_TreeBox.SetHeaderText( 13, "MAD" );
   Data_TreeBox.SetHeaderText( 14, String() );
   for ( int i = 0; i < Data_TreeBox.NumberOfColumns(); ++i )
      Data_TreeBox.SetHeaderAlignment( i, TextAlign::Center|TextAlign::VertCenter );
   Data_TreeBox.EnableRootDecoration();
   Data_TreeBox.EnableAlternateRowColor();
   Data_TreeBox.EnableMultipleSelections();
   Data_TreeBox.SetStyleSheet( w.ScaledStyleSheet(
         "QTreeView {"
            "font-family: DejaVu Sans Mono, Monospace;"
            "font-size: 9pt;"
         "}"
      ) );
   Data_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&DynamicPSFInterface::__CurrentNodeUpdated, w );
   Data_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&DynamicPSFInterface::__NodeActivated, w );
   Data_TreeBox.OnNodeDoubleClicked( (TreeBox::node_event_handler)&DynamicPSFInterface::__NodeDoubleClicked, w );
   Data_TreeBox.OnNodeExpanded( (TreeBox::node_expand_event_handler)&DynamicPSFInterface::__NodeExpanded, w );
   Data_TreeBox.OnNodeCollapsed( (TreeBox::node_expand_event_handler)&DynamicPSFInterface::__NodeCollapsed, w );
   Data_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&DynamicPSFInterface::__NodeSelectionUpdated, w );

   //

   StarInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   StarInfo_Label.SetStyle( FrameStyle::Sunken );
   StarInfo_Label.SetLineWidth( 1 );
   StarInfo_Label.SetMinWidth( fnt.Width( String( '0', 60 ) ) );

   ExpandAll_ToolButton.SetIcon( w.ScaledResource( ":/browser/expand.png" ) );
   ExpandAll_ToolButton.SetScaledFixedSize( 24, 24 );
   ExpandAll_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExpandAll_ToolButton.SetToolTip( "<p>Expand all tree nodes.</p>" );
   ExpandAll_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   CollapseAll_ToolButton.SetIcon( w.ScaledResource( ":/browser/collapse.png" ) );
   CollapseAll_ToolButton.SetScaledFixedSize( 24, 24 );
   CollapseAll_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   CollapseAll_ToolButton.SetToolTip( "<p>Collapse all tree nodes.</p>" );
   CollapseAll_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   DeleteStar_ToolButton.SetIcon( w.ScaledResource( ":/icons/remove.png" ) );
   DeleteStar_ToolButton.SetScaledFixedSize( 24, 24 );
   DeleteStar_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DeleteStar_ToolButton.SetToolTip( "<p>Delete selected item(s).</p>" );
   DeleteStar_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   TrackStars_ToolButton.SetIcon( w.ScaledResource( ":/icons/goto-next.png" ) );
   TrackStars_ToolButton.SetScaledFixedSize( 24, 24 );
   TrackStars_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   TrackStars_ToolButton.SetToolTip( "<p>Track star positions on source views.</p>" );
   TrackStars_ToolButton.SetCheckable();
   TrackStars_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Regenerate_ToolButton.SetIcon( w.ScaledResource( ":/icons/redo.png" ) );
   Regenerate_ToolButton.SetScaledFixedSize( 24, 24 );
   Regenerate_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Regenerate_ToolButton.SetToolTip( "<p>Regenerate the selected star(s) with the current fitting parameters.</p>" );
   Regenerate_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   RegenerateAll_ToolButton.SetIcon( w.ScaledResource( ":/icons/reload.png" ) );
   RegenerateAll_ToolButton.SetScaledFixedSize( 24, 24 );
   RegenerateAll_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RegenerateAll_ToolButton.SetToolTip( "<p>Regenerate all stars with the current fitting parameters.</p>" );
   RegenerateAll_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Sort_ToolButton.SetIcon( w.ScaledResource( ":/icons/sort-down.png" ) );
   Sort_ToolButton.SetScaledFixedSize( 24, 24 );
   Sort_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Sort_ToolButton.SetToolTip( "<p>Sort the list of stars by selectable criteria.</p>" );
   Sort_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   ExportPSF_ToolButton.SetIcon( w.ScaledResource( ":/icons/camera.png" ) );
   ExportPSF_ToolButton.SetScaledFixedSize( 24, 24 );
   ExportPSF_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExportPSF_ToolButton.SetToolTip( "<p>Export a synthetic PSF function as a new image window. The synthetic PSF "
      "will be computed as the normalized average of the fitted functions for the selected star(s).</p>" );
   ExportPSF_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   AverageStars_ToolButton.SetIcon( w.ScaledResource( ":/icons/document-math.png" ) );
   AverageStars_ToolButton.SetScaledFixedSize( 24, 24 );
   AverageStars_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   AverageStars_ToolButton.SetToolTip( "<p>Average PSF parameters for selected stars.</p>" );
   AverageStars_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   ExportCSV_ToolButton.SetIcon( w.ScaledResource( ":/icons/document-text-export.png" ) );
   ExportCSV_ToolButton.SetScaledFixedSize( 24, 24 );
   ExportCSV_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ExportCSV_ToolButton.SetToolTip( "<p>Export data in Comma Separated Value (CSV) file format.</p>" );
   ExportCSV_ToolButton.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   StarTools_Sizer.SetSpacing( 4 );
   StarTools_Sizer.Add( StarInfo_Label, 100 );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( ExpandAll_ToolButton );
   StarTools_Sizer.Add( CollapseAll_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( DeleteStar_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( TrackStars_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( Regenerate_ToolButton );
   StarTools_Sizer.Add( RegenerateAll_ToolButton );
   StarTools_Sizer.Add( Sort_ToolButton );
   StarTools_Sizer.AddSpacing( 12 );
   StarTools_Sizer.Add( ExportPSF_ToolButton );
   StarTools_Sizer.Add( AverageStars_ToolButton );
   StarTools_Sizer.Add( ExportCSV_ToolButton );

   //

   PSFModelFunctions_SectionBar.SetTitle( "PSF Model Functions" );
   PSFModelFunctions_SectionBar.SetSection( PSFModelFunctions_Control );
   PSFModelFunctions_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DynamicPSFInterface::__ToggleSection, w );

   AutoPSF_CheckBox.SetText( "Auto" );
   AutoPSF_CheckBox.SetToolTip( "<p>Find the PSF model function that best approximates actual image data.</p>" );
   AutoPSF_CheckBox.SetChecked();
   AutoPSF_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Gaussian_CheckBox.SetText( "Gaussian" );
   Gaussian_CheckBox.SetToolTip( "<p>A Gaussian PSF model function.</p>" );
   Gaussian_CheckBox.SetChecked();
   Gaussian_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat_CheckBox.SetText( "Moffat" );
   Moffat_CheckBox.SetToolTip( "<p>A Moffat PSF model function with variable beta exponent.</p>" );
   Moffat_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   MoffatA_CheckBox.SetText( "Moffat10" );
   MoffatA_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=10</p>" );
   MoffatA_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat8_CheckBox.SetText( "Moffat8" );
   Moffat8_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=8</p>" );
   Moffat8_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat6_CheckBox.SetText( "Moffat6" );
   Moffat6_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=6</p>" );
   Moffat6_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat4_CheckBox.SetText( "Moffat4" );
   Moffat4_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=4</p>" );
   Moffat4_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat25_CheckBox.SetText( "Moffat25" );
   Moffat25_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=2.5</p>" );
   Moffat25_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Moffat15_CheckBox.SetText( "Moffat15" );
   Moffat15_CheckBox.SetToolTip( "<p>A Moffat PSF model function with fixed beta=1.5</p>" );
   Moffat15_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   Lorentzian_CheckBox.SetText( "Lorentzian" );
   Lorentzian_CheckBox.SetToolTip( "<p>A Lorentzian PSF model function. This is equivalent to a Moffat function with fixed beta=1.</p>" );
   Lorentzian_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   CircularPSF_CheckBox.SetText( "Circular PSF" );
   CircularPSF_CheckBox.SetToolTip( "<p>Enable this option to fit circular PSF functions. Disable it to fit elliptical "
      "functions. An elliptical function provides two separate axes and a rotation angle.</p>"
      "<p>In general, elliptical functions are preferable because they provide more information about the true shapes "
      "and orientations of the fitted PSFs. Sometimes, however, circular functions may be preferable. For example, strongly "
      "undersampled images usually don't provide enough data to fit elliptical functions reliably. Uncertainty due to high "
      "noise levels also affect complex function fittings adversely. In such cases a circular function can provide more "
      "robust results.</p>"
      "<p>Elliptical PSF functions are fitted by default.</p>" );
   CircularPSF_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   SignedAngles_CheckBox.SetText( "Signed angles" );
   SignedAngles_CheckBox.SetToolTip( "<p>When this option is enabled, rotation angles are shown in the [-90&deg;,+90&deg;] "
      "range. When this option is disabled, rotation angles are shown in the [0&deg;,180&deg;) range.</p>"
      "<p>Signed angles are useful to prevent ambiguities introduced by small rotations around zero degrees. These "
      "'oscillatory' rotations occur frequently due to uncertainty in fitting rotation angles for nearly circular stars. "
      "For example, imagine a set of PSF fits where we have two subsets with rotations around zero and 180 degrees. If we "
      "compute average PSF parameters, the resulting average rotation would be around 90 degrees. This may be incorrect if "
      "both subsets are actually due to dispersion caused by fitting uncertainty, in which case the correct average rotation "
      "would be a value close to zero degrees.</p>"
      "<p>This option is enabled by default, so rotation angles are represented as signed quantities in the "
      "(-90&deg;,+90&deg;] range.</p>" );
   SignedAngles_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   PSFModelFunctions1_Sizer.SetSpacing( 4 );
   PSFModelFunctions1_Sizer.Add( AutoPSF_CheckBox );
   PSFModelFunctions1_Sizer.Add( Moffat6_CheckBox );
   PSFModelFunctions1_Sizer.Add( CircularPSF_CheckBox );
   PSFModelFunctions1_Sizer.AddStretch();

   PSFModelFunctions2_Sizer.SetSpacing( 4 );
   PSFModelFunctions2_Sizer.Add( Gaussian_CheckBox );
   PSFModelFunctions2_Sizer.Add( Moffat4_CheckBox );
   PSFModelFunctions2_Sizer.Add( SignedAngles_CheckBox );
   PSFModelFunctions2_Sizer.AddStretch();

   PSFModelFunctions3_Sizer.SetSpacing( 4 );
   PSFModelFunctions3_Sizer.Add( Moffat_CheckBox );
   PSFModelFunctions3_Sizer.Add( Moffat25_CheckBox );
   PSFModelFunctions3_Sizer.AddStretch();

   PSFModelFunctions4_Sizer.SetSpacing( 4 );
   PSFModelFunctions4_Sizer.Add( MoffatA_CheckBox );
   PSFModelFunctions4_Sizer.Add( Moffat15_CheckBox );
   PSFModelFunctions4_Sizer.AddStretch();

   PSFModelFunctions5_Sizer.SetSpacing( 4 );
   PSFModelFunctions5_Sizer.Add( Moffat8_CheckBox );
   PSFModelFunctions5_Sizer.Add( Lorentzian_CheckBox );
   PSFModelFunctions5_Sizer.AddStretch();

   PSFModelFunctions_Sizer.SetMargin( 4 );
   PSFModelFunctions_Sizer.SetSpacing( 16 );
   PSFModelFunctions_Sizer.Add( PSFModelFunctions1_Sizer );
   PSFModelFunctions_Sizer.Add( PSFModelFunctions2_Sizer );
   PSFModelFunctions_Sizer.Add( PSFModelFunctions3_Sizer );
   PSFModelFunctions_Sizer.Add( PSFModelFunctions4_Sizer );
   PSFModelFunctions_Sizer.Add( PSFModelFunctions5_Sizer );
   PSFModelFunctions_Sizer.AddStretch();

   PSFModelFunctions_Control.SetSizer( PSFModelFunctions_Sizer );

   //

   StarDetectionParameters_SectionBar.SetTitle( "Star Detection" );
   StarDetectionParameters_SectionBar.SetSection( StarDetectionParameters_Control );
   StarDetectionParameters_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DynamicPSFInterface::__ToggleSection, w );

   static const char* searchRadiusToolTip =
   "<p>This parameter determines the size in pixels of the initial search box used to detect stars when you clic on "
   "the image. Increase it to favor detection of larger stars. Decrease it to facilitate selection of smaller stars. "
   "For example, a smaller search radius may be necessary to deal with dense star fields.</p>"
   "<p>The default value of eight pixels is quite appropriate in most cases.</p>";

   SearchRadius_Label.SetText( "Search radius:" );
   SearchRadius_Label.SetMinWidth( labelWidth1 );
   SearchRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SearchRadius_Label.SetToolTip( searchRadiusToolTip );

   SearchRadius_SpinBox.SetRange( int( TheDPSearchRadiusParameter->MinimumValue() ),
                                  int( TheDPSearchRadiusParameter->MaximumValue() ) );
   SearchRadius_SpinBox.SetToolTip( searchRadiusToolTip );
   SearchRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&DynamicPSFInterface::__SpinBox_ValueUpdated, w );

   SearchRadius_Sizer.SetSpacing( 4 );
   SearchRadius_Sizer.Add( SearchRadius_Label );
   SearchRadius_Sizer.Add( SearchRadius_SpinBox );
   SearchRadius_Sizer.AddStretch();

   Threshold_NumericEdit.SetReal();
   Threshold_NumericEdit.SetPrecision( TheDPThresholdParameter->Precision() );
   Threshold_NumericEdit.SetRange( TheDPThresholdParameter->MinimumValue(), TheDPThresholdParameter->MaximumValue() );
   Threshold_NumericEdit.label.SetText( "Background threshold:" );
   Threshold_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Threshold_NumericEdit.sizer.AddStretch();
   Threshold_NumericEdit.SetToolTip( "<p>Threshold value for rejection of background pixels, in sigma units.</p>"
      "<p>This value is used by the object detection routine to classify sampled pixels into two disjoint sets: "
      "pixels belonging to the local background, and pixels belonging to the object being detected. A smaller "
      "threshold value makes the star detection routine less sensitive. This can be useful to avoid detection of "
      "very faint objects. On the other hand, a higher threshold allows you to isolate very small and faint objects, "
      "which may or may not be desirable, depending on what you want to do.</p>"
      "<p>The default value of one sigma is normally appropriate in most cases.</p>" );
   Threshold_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicPSFInterface::__NumericEdit_ValueUpdated, w );

   AutoAperture_CheckBox.SetText( "Automatic aperture" );
   AutoAperture_CheckBox.SetToolTip( "<p>Compute optimal sampling dimensions adaptively for each fitted star.</p>"
      "<p>Each star is fitted to a PSF function inside a rectangular <i>sampling region</i>. If the sampling region "
      "is too small, the local background cannot be determined accurately, and hence the PSF fit will be less accurate. "
      "If the sampling region is too large, pixels from nearby objects can contaminate the sample, and computation "
      "times grow unnecessarily. The automatic aperture feature of DynamicPSF implements a robust algorithm to find "
      "the smallest sampling region necessary to minimize uncertainty in the evaluation of the local background.</p>"
      "<p>This option is enabled by default. Disabling it is not recommended under normal working conditions.</p>" );
   AutoAperture_CheckBox.OnClick( (ToolButton::click_event_handler)&DynamicPSFInterface::__Click, w );

   AutoAperture_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   AutoAperture_Sizer.Add( AutoAperture_CheckBox );
   AutoAperture_Sizer.AddStretch();

   StarDetectionParameters_Sizer.SetSpacing( 4 );
   StarDetectionParameters_Sizer.Add( SearchRadius_Sizer );
   StarDetectionParameters_Sizer.Add( Threshold_NumericEdit );
   StarDetectionParameters_Sizer.Add( AutoAperture_Sizer );
   StarDetectionParameters_Sizer.AddStretch();

   StarDetectionParameters_Control.SetSizer( StarDetectionParameters_Sizer );

   //

   Scale_SectionBar.SetTitle( "Image Scale" );
   Scale_SectionBar.SetSection( Scale_Control );
   Scale_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DynamicPSFInterface::__ToggleSection, w );

   const char* scaleModeTip = "<p>This parameter tells DynamicPSF how to compute the image scale to show "
      "full width at half maximum (FWHM) values:</p>"
      "<p><b>Pixels</b> will ignore image scale and show FWHM in pixel units. This is the default option.</p>"
      "<p><b>Standard FITS keywords</b> will try to read standard FOCALLEN, XPIXSZ and YPIXSZ FITS header "
      "keywords to compute the image scale in arcseconds per pixel.</p>"
      "<p><b>literal value</b> allows you to specify directly the image scale in arcseconds per pixel.</p>"
      "<p><b>Custom FITS keyword</b> allows you to specify the name of a custom FITS keyword whose value is "
      "the image scale in arcseconds per pixel.</p>";

   ScaleMode_Label.SetText( "Scale mode:" );
   ScaleMode_Label.SetToolTip( scaleModeTip );
   ScaleMode_Label.SetMinWidth( labelWidth1 );
   ScaleMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ScaleMode_ComboBox.AddItem( "Pixels" );
   ScaleMode_ComboBox.AddItem( "Standard FITS keywords" );
   ScaleMode_ComboBox.AddItem( "Literal value" );
   ScaleMode_ComboBox.AddItem( "Custom FITS keyword" );
   ScaleMode_ComboBox.SetToolTip( scaleModeTip );
   ScaleMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DynamicPSFInterface::__ItemSelected, w );

   ScaleMode_Sizer.SetSpacing( 4 );
   ScaleMode_Sizer.Add( ScaleMode_Label );
   ScaleMode_Sizer.Add( ScaleMode_ComboBox );
   ScaleMode_Sizer.AddStretch();

   const char* scaleValueTip = "<p>When <i>scale mode</i> has been set to <i>literal value</i>, this is the image "
      "scale in arcseconds per pixel.</p>";

   ScaleValue_NumericEdit.label.SetText( "Image scale (arcsec/px):" );
   ScaleValue_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ScaleValue_NumericEdit.SetReal();
   ScaleValue_NumericEdit.SetRange( TheDPScaleValueParameter->MinimumValue(), TheDPScaleValueParameter->MaximumValue() );
   ScaleValue_NumericEdit.SetPrecision( TheDPScaleValueParameter->Precision() );
   ScaleValue_NumericEdit.sizer.AddStretch();
   ScaleValue_NumericEdit.SetToolTip( scaleValueTip );
   ScaleValue_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&DynamicPSFInterface::__NumericEdit_ValueUpdated, w );

   const char* scaleKeywordTip = "<p>When <i>scale mode</i> has been set to <i>custom FITS keyword</i>, this is "
      "the name of a FITS header keyword whose value is the image scale in arcseconds per pixel.</p>";

   ScaleKeyword_Label.SetText( "Custom keyword:" );
   ScaleKeyword_Label.SetFixedWidth( labelWidth1 );
   ScaleKeyword_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ScaleKeyword_Label.SetToolTip( scaleKeywordTip );

   ScaleKeyword_Edit.SetMinWidth( editWidth1 );
   ScaleKeyword_Edit.SetToolTip( scaleKeywordTip );
   ScaleKeyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&DynamicPSFInterface::__EditCompleted, w );

   ScaleKeyword_Sizer.SetSpacing( 4 );
   ScaleKeyword_Sizer.Add( ScaleKeyword_Label );
   ScaleKeyword_Sizer.Add( ScaleKeyword_Edit );
   ScaleKeyword_Sizer.AddStretch();

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( ScaleMode_Sizer );
   Scale_Sizer.Add( ScaleValue_NumericEdit );
   Scale_Sizer.Add( ScaleKeyword_Sizer );

   Scale_Control.SetSizer( Scale_Sizer );

   //

   Global_Sizer.SetMargin( 6 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Data_TreeBox, 100 );
   Global_Sizer.Add( StarTools_Sizer );
   Global_Sizer.Add( PSFModelFunctions_SectionBar );
   Global_Sizer.Add( PSFModelFunctions_Control );
   Global_Sizer.Add( StarDetectionParameters_SectionBar );
   Global_Sizer.Add( StarDetectionParameters_Control );
   Global_Sizer.Add( Scale_SectionBar );
   Global_Sizer.Add( Scale_Control );

   w.SetSizer( Global_Sizer );

   StarDetectionParameters_Control.Hide();
   Scale_Control.Hide();

   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DynamicPSFInterface.cpp - Released 2015/12/18 08:55:08 UTC
