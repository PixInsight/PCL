//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard CloneStamp Process Module Version 01.00.02.0295
// ----------------------------------------------------------------------------
// CloneStampInterface.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard CloneStamp PixInsight module.
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

#include "CloneStampInterface.h"
#include "CloneStampParameters.h"
#include "CloneStampProcess.h"

#include <pcl/Color.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CloneStampInterface* TheCloneStampInterface = nullptr;

static CloneStampInterface::source_view_list sourceViews;

const int tileSize = 64;

// Half size of center mark in logical viewport coordinates
#define CENTER_RADIUS 5

// ----------------------------------------------------------------------------

#include "CloneStampIcon.xpm"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CloneStampInterface::ViewRef::ViewRef( View& v ) :
   view( v ),
   refCount( 1 )
{
   if ( !view.IsNull() )
      view.AddToDynamicTargets();
}

CloneStampInterface::ViewRef::ViewRef( const View& v, int ) :
   view( v ),
   refCount( -1 )
{
}

CloneStampInterface::ViewRef::~ViewRef()
{
   if ( refCount >= 0 && !view.IsNull() )
      view.RemoveFromDynamicTargets();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CloneStampInterface::ClonerAction::ClonerAction( View& view, const BrushData& b, const Point& d ) :
   source( nullptr ),
   brush( b ),
   delta( d ),
   cloner(),
   bounds( 0 )
{
   if ( !view.IsNull() )
   {
      source_view_list::const_iterator i = sourceViews.Search( ViewRef( view, 0 ) );
      if ( i == sourceViews.End() )
         sourceViews.Add( source = new ViewRef( view ) );
      else
      {
         source = *i;
         ++source->refCount;
      }
   }
}

CloneStampInterface::ClonerAction::ClonerAction( const ClonerAction& x ) :
source( x.source ), brush( x.brush ), delta( x.delta ), cloner( x.cloner ), bounds( x.bounds )
{
   if ( source != nullptr )
      ++source->refCount;
}

CloneStampInterface::ClonerAction::~ClonerAction()
{
   if ( source != nullptr )
   {
      if ( --source->refCount == 0 )
         sourceViews.Destroy( *source );
      source = nullptr;
   }
}

void CloneStampInterface::ClonerAction::UpdateBounds()
{
   if ( cloner.IsEmpty() )
      bounds = 0;
   else
   {
      cloner_sequence::const_iterator i = cloner.Begin();
      int d = brush.radius;
      Point p( i->targetPos );
      bounds = Rect( p - d, p + (d + 1) );
      for ( ; ++i != cloner.End(); )
      {
         p = i->targetPos;
         bounds.Unite( Rect( p - d, p + (d + 1) ) );
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CloneStampInterface::StoreRect( const Rect& _r )
{
   ImageVariant image = targetView->Image();
   Rect r( _r );

   if ( r.Intersect( image.Bounds() ) )
   {
      image.ResetChannelRange();

      int row0 = r.y0/tileSize;
      int row1 = (r.y1 - 1)/tileSize;

      int col0 = r.x0/tileSize;
      int col1 = (r.x1 - 1)/tileSize;

      for ( int i = row0; i <= row1; ++i )
         for ( int j = col0, k = tileCols*i + col0; j <= col1; ++j, ++k )
            if ( !swapTiles[k] )
            {
               int x0 = j*tileSize;
               int y0 = i*tileSize;
               image.SelectRectangle( x0, y0, x0+tileSize, y0+tileSize );
               swapTiles[k].CopyImage( image );
            }
   }
}

template <class P>
static void RestoreTile( GenericImage<P>& image, const GenericImage<P>& tile )
{
   image.Apply( tile );
}

static void RestoreTile( ImageVariant& image, const ImageVariant& tile )
{
   if ( image.IsComplexSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: RestoreTile( static_cast<ComplexImage&>( *image ), static_cast<const ComplexImage&>( *tile ) ); break;
      case 64: RestoreTile( static_cast<DComplexImage&>( *image ), static_cast<const DComplexImage&>( *tile ) ); break;
      }
   else if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: RestoreTile( static_cast<Image&>( *image ), static_cast<const Image&>( *tile ) ); break;
      case 64: RestoreTile( static_cast<DImage&>( *image ), static_cast<const DImage&>( *tile ) ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: RestoreTile( static_cast<UInt8Image&>( *image ), static_cast<const UInt8Image&>( *tile ) ); break;
      case 16: RestoreTile( static_cast<UInt16Image&>( *image ), static_cast<const UInt16Image&>( *tile ) ); break;
      case 32: RestoreTile( static_cast<UInt32Image&>( *image ), static_cast<const UInt32Image&>( *tile ) ); break;
      }
}

void CloneStampInterface::RestoreRect( const Rect& _r )
{
   ImageVariant image = targetView->Image();
   Rect r( _r );

   if ( r.Intersect( image.Bounds() ) )
   {
      image.ResetChannelRange();

      int row0 = r.y0/tileSize;
      int row1 = (r.y1 - 1)/tileSize;

      int col0 = r.x0/tileSize;
      int col1 = (r.x1 - 1)/tileSize;

      for ( int i = row0; i <= row1; ++i )
         for ( int j = col0, k = tileCols*i + col0; j <= col1; ++j, ++k )
         {
            ImageVariant& tile = swapTiles[k];

            if ( tile )
            {
               int x0 = j*tileSize;
               int y0 = i*tileSize;
               image.SelectPoint( Max( r.x0, x0 ), Max( r.y0, y0 ) );
               tile.SelectRectangle( r.x0-x0, r.y0-y0, r.x1-x0, r.y1-y0 );
               RestoreTile( image, tile );

               if ( tile.IsFullSelection() )
                  tile.Free();
               else
                  tile.ResetSelections();
            }
         }
   }
}

void CloneStampInterface::RestoreView()
{
   RestoreRect( targetView->Image().Bounds() );
}

template <class P>
static void XchgTile( GenericImage<P>& image, GenericImage<P>& tile )
{
   image.Xchg( tile );
}

static void XchgTile( ImageVariant& image, ImageVariant& tile )
{
   if ( image.IsComplexSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: XchgTile( static_cast<ComplexImage&>( *image ), static_cast<ComplexImage&>( *tile ) ); break;
      case 64: XchgTile( static_cast<DComplexImage&>( *image ), static_cast<DComplexImage&>( *tile ) ); break;
      }
   else if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: XchgTile( static_cast<Image&>( *image ), static_cast<Image&>( *tile ) ); break;
      case 64: XchgTile( static_cast<DImage&>( *image ), static_cast<DImage&>( *tile ) ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: XchgTile( static_cast<UInt8Image&>( *image ), static_cast<UInt8Image&>( *tile ) ); break;
      case 16: XchgTile( static_cast<UInt16Image&>( *image ), static_cast<UInt16Image&>( *tile ) ); break;
      case 32: XchgTile( static_cast<UInt32Image&>( *image ), static_cast<UInt32Image&>( *tile ) ); break;
      }
}

void CloneStampInterface::XchgTiles()
{
   ImageVariant image = targetView->Image();
   image.ResetChannelRange();
   for ( int i = 0, k = 0; i < tileRows; ++i )
      for ( int j = 0; j < tileCols; ++j, ++k )
         if ( swapTiles[k] )
         {
            image.SelectPoint( j*tileSize, i*tileSize );
            XchgTile( image, swapTiles[k] );
         }
}

// ----------------------------------------------------------------------------

void CloneStampInterface::Undo()
{
   if ( !done.IsEmpty() )
   {
      UpdateViews();

      action_list::const_iterator i = done.ReverseBegin();
      Rect r = i->bounds;
      for ( action_list::const_iterator j = i; --j != done.ReverseEnd(); )
         if ( r.Intersects( j->bounds ) )
            r.Unite( j->bounds );

      RestoreRect( r );

      for ( action_list::const_iterator j = done.Begin(); j != i; ++j )
         if ( r.Intersects( j->bounds ) )
         {
            action_list::const_iterator k = j; ++k;
            PlayBack( j, k );
         }

      //undone.Splice( undone.End(), done, i, done.End() );
      undone.Append( i, done.ConstEnd() );
      done.Remove( done.MutableIterator( i ), done.End() );

      if ( showBounds && !done.IsEmpty() )
         JumpToCurrentAction();

      targetView->Window().RegenerateImageRect( r );

      UpdateControls();
      UpdateViews();

      Broadcast();
   }
}

void CloneStampInterface::UndoAll()
{
   if ( !done.IsEmpty() )
   {
      UpdateViews();

      Rect r = done.Begin()->bounds;
      for ( action_list::const_iterator i = done.Begin(); ++i != done.End(); )
         r.Unite( i->bounds );

      RestoreView();

      done.Reverse();
      //undone.Splice( undone.End(), done );
      undone.Append( done );
      done.Clear();

      targetView->Window().RegenerateImageRect( r );

      UpdateControls();
      UpdateViews();

      Broadcast();
   }
}

void CloneStampInterface::Redo()
{
   if ( !undone.IsEmpty() )
   {
      UpdateViews();

      //done.Splice( done.End(), undone, undone.ReverseBegin(), undone.End() );
      done.Append( action_list::iterator( undone.ReverseBegin() ), undone.End() );
      undone.Remove( action_list::iterator( undone.ReverseBegin() ), undone.End() );

      PlayBack( done.ReverseBegin().Iterator(), done.End() );
      targetView->Window().RegenerateImageRect( done.ReverseBegin()->bounds );

      if ( showBounds && !done.IsEmpty() )
         JumpToCurrentAction();

      UpdateControls();
      UpdateViews();

      Broadcast();
   }
}

void CloneStampInterface::RedoAll()
{
   if ( !undone.IsEmpty() )
   {
      UpdateViews();

      Rect r = undone.Begin()->bounds;
      for ( action_list::const_iterator i = undone.Begin(); ++i != undone.End(); )
         r.Unite( i->bounds );

      undone.Reverse();
      PlayBack( undone.Begin(), undone.End() );
      //done.Splice( done.End(), undone );
      done = undone;
      undone.Clear();

      targetView->Window().RegenerateImageRect( r );

      if ( showBounds )
         JumpToCurrentAction();

      UpdateControls();
      UpdateViews();

      Broadcast();
   }
}

void CloneStampInterface::Delete()
{
   if ( !done.IsEmpty() )
   {
      Undo();
      undone.Remove( undone.ReverseBegin() );
      UpdateControls();
   }
}

void CloneStampInterface::PlayBack( action_list::const_iterator i, action_list::const_iterator j )
{
   if ( i != j )
   {
      ImageVariant image = targetView->Image();

      ImageWindow w = targetView->Window();

      ImageVariant mask;
      bool maskInverted = w.IsMaskInverted();
      if ( w.IsMaskEnabled() )
         if ( !w.Mask().IsNull() )
            mask = w.Mask().MainView().Image();

      for ( ;; )
      {
         const ClonerAction& action = *i;

         StoreRect( i->bounds );

         ImageVariant src = (action.source != nullptr) ? action.source->view.Image() : image;

         for ( cloner_sequence::const_iterator k = action.cloner.Begin(); k != action.cloner.End(); ++k )
            CloneStampInstance::Apply( image, src, mask, maskInverted, i->brush, k, i->delta );

         if ( ++i == j )
            break;
      }
   }
}

// ----------------------------------------------------------------------------

void CloneStampInterface::ExportInstance( CloneStampInstance& instance ) const
{
   instance.actions.Clear();
   instance.cloner.Clear();

   if ( targetView != nullptr )
   {
      uint32 actionIdx = 0;
      for ( action_list::const_iterator i = done.Begin(); i != done.End(); ++i, ++actionIdx )
      {
         CloneStampInstance::ActionData action;
         if ( i->source != nullptr )
         {
            action.sourceId = i->source->view.Id();
            i->source->view.GetSize( action.sourceWidth, action.sourceHeight );
         }

         action.sourcePos = i->cloner.Begin()->targetPos - i->delta;
         action.brush = i->brush;

         instance.actions.Add( action );

         for ( cloner_sequence::const_iterator j = i->cloner.Begin(); j != i->cloner.End(); ++j )
         {
            ClonerData data( *j );
            data.actionIdx = actionIdx;
            instance.cloner.Add( data );
         }
      }

      targetView->GetSize( instance.width, instance.height );
   }
   else
      instance.width = instance.height = 0;

   instance.color = color;
   instance.boundsColor = boundsColor;
}

void CloneStampInterface::Reset()
{
   if ( targetView != nullptr )
   {
      ImageWindow mw = targetView->Window().Mask();
      if ( !mw.IsNull() )
         mw.MainView().RemoveFromDynamicTargets();

      targetView->RemoveFromDynamicTargets();
      delete targetView, targetView = nullptr;
   }

   if ( sourceView != nullptr )
   {
      sourceView->RemoveFromDynamicTargets();
      delete sourceView, sourceView = nullptr;
   }

   delta = 0;
   targetPos = 0;

   initialized = selectingSource = selectingTarget = false;
   executing = onTarget = dragging = imageChanged = false;

   done.Clear();
   undone.Clear();

   swapTiles.Clear();

   //action_list::DeleteFreeList();
}

void CloneStampInterface::SelectTarget( View& view )
{
   Reset();

   targetView = new View( view );
   targetView->AddToDynamicTargets();

   ImageWindow mw = targetView->Window().Mask();
   if ( !mw.IsNull() )
      mw.MainView().AddToDynamicTargets();

   ImageVariant image = targetView->Image();

   tileRows = image.Height()/tileSize;
   if ( image.Height()%tileSize != 0 )
      ++tileRows;

   tileCols = image.Width()/tileSize;
   if ( image.Width()%tileSize != 0 )
      ++tileCols;

   swapTiles.Add( ImageVariant(), tileRows*tileCols );
}

void CloneStampInterface::SelectSource( View& view )
{
   if ( sourceView != nullptr )
   {
      if ( view == *sourceView )
         return;

      //sourceView->RemoveFromDynamicTargets(); // will be done when done/undone lists are destroyed
      delete sourceView, sourceView = nullptr;
   }

   if ( view != *targetView )
   {
      sourceView = new View( view );
      sourceView->AddToDynamicTargets();
   }

   initialized = true;
}

void CloneStampInterface::UpdateViews()
{
   if ( targetView != nullptr )
   {
      ImageWindow targetWindow = targetView->Window();

      double d2 = targetWindow.ViewportScalarToImage( targetWindow.DisplayPixelRatio()/2 );

      if ( onTarget )
      {
         ImageWindow sourceWindow = (sourceView != nullptr) ? sourceView->Window() : targetWindow;

         DPoint t = TargetPos();
         DPoint s = SourcePos();

         if ( !selectingSource )
         {
            // Brush
            targetWindow.UpdateImageRect( DRect( t - double( brush.radius ), t + double( brush.radius ) ).InflatedBy( d2 ) );

            // Vector
            if ( sourceView == nullptr )
            {
               DRect r( s, t );
               r.Order();
               targetWindow.UpdateImageRect( r.InflatedBy( d2 ) );
            }
         }

         // Source point
         double dr = sourceWindow.ViewportScalarToImage( sourceWindow.DisplayPixelRatio()*(CENTER_RADIUS + 2) );
         sourceWindow.UpdateImageRect( DRect( s - dr, s + dr ) );
      }

      // Action boundaries
      if ( showBounds && !done.IsEmpty() )
         targetWindow.UpdateImageRect( done.ReverseBegin()->bounds.InflatedBy( d2 ) );
   }
}

void CloneStampInterface::RegenerateView()
{
   if ( targetView != nullptr )
      targetView->Window().RegenerateImageRect( Rect( Point( targetPos ) - int( brush.radius ),
                                                      Point( targetPos ) + (int( brush.radius ) + 1) ) );
}

void CloneStampInterface::JumpToCurrentAction()
{
   if ( targetView != nullptr )
      if ( !done.IsEmpty() )
      {
         const ClonerAction& action = *done.ConstReverseBegin();

         if ( !action.cloner.IsEmpty() )
         {
            ImageWindow w = targetView->Window();
            //w.BringToFront();
            w.SelectView( *targetView );
            w.SetViewport( action.bounds.Center(), w.ZoomFactor() );
         }
      }
}

void CloneStampInterface::Broadcast()
{
   if ( targetView != nullptr )
      BroadcastImageUpdated( *targetView );
}

Rect CloneStampInterface::Bounds() const
{
   if ( done.IsEmpty() )
      return Rect( 0 );

   Rect r = done.Begin()->bounds;
   for ( action_list::const_iterator i = done.Begin(); ++i != done.End(); )
      r.Unite( i->bounds );

   return r;
}

// ----------------------------------------------------------------------------

CloneStampInterface::CloneStampInterface() :
   ProcessInterface(),
   targetView( nullptr ),
   sourceView( nullptr ),
   brush(),
   delta( 0 ),
   targetPos( 0 ),
   selectingSource( true ),
   selectingTarget( false ),
   executing( false ),
   onTarget( false ),
   dragging( false ),
   imageChanged( false ),
   regenRect( 0 ),
   color( uint32( TheCSColorParameter->DefaultValue() ) ),
   boundsColor( uint32( TheCSBoundsColorParameter->DefaultValue() ) ),
   showBounds( false ),
   swapTiles(),
   done(),
   undone(),
   GUI( nullptr )
{
   TheCloneStampInterface = this;
}

// ----------------------------------------------------------------------------

CloneStampInterface::~CloneStampInterface()
{
   if ( targetView != nullptr )
      delete targetView, targetView = nullptr;
   if ( sourceView != nullptr )
      delete sourceView, sourceView = nullptr;
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString CloneStampInterface::Id() const
{
   return "CloneStamp";
}

// ----------------------------------------------------------------------------

MetaProcess* CloneStampInterface::Process() const
{
   return TheCloneStampProcess;
}

// ----------------------------------------------------------------------------

const char** CloneStampInterface::IconImageXPM() const
{
   return CloneStampIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures CloneStampInterface::Features() const
{
   return InterfaceFeature::DefaultDynamic;
}

// ----------------------------------------------------------------------------

void CloneStampInterface::Execute()
{
   if ( targetView != nullptr )
   {
      executing = true;

      /*
       * Recover the original view image and save processed tiles.
       */
      XchgTiles();

      /*
       * Obtain a working instance.
       *
       * The isInterfaceInstance private flag tells the instance that all the
       * work has already been done by the interface.
       */
      CloneStampInstance instance( TheCloneStampProcess );
      ExportInstance( instance );
      instance.isInterfaceInstance = true;

      /*
       * Obtain local working references to the target view and window.
       */
      View v = *targetView;
      ImageWindow w = v.Window();

      /*
       * Reset interface data. Note that we remove the target view from the
       * dynamic targets set. This is necessary since active dynamic targets
       * cannot be modified.
       */

      ImageWindow mw = targetView->Window().Mask();
      if ( !mw.IsNull() )
         mw.MainView().RemoveFromDynamicTargets();

      targetView->RemoveFromDynamicTargets();

      if ( sourceView != nullptr )
         sourceView->RemoveFromDynamicTargets();

      onTarget = false;
      UpdateViews();

      done.Clear();
      undone.Clear();

      /*
       * Ensure that our target view is selected as the current view.
       */
      w.BringToFront();
      w.SelectView( v );

      /*
       * Execute the instance on the target window.
       */
      instance.LaunchOn( w );

      Reset(); // clears the 'executing' flag

      /*
       * Update to reflect the current state.
       */
      UpdateControls();
   }
}

void CloneStampInterface::Cancel()
{
   if ( !done.IsEmpty() )
      if ( MessageBox( "<p><b>Existing CloneStamp actions will be lost</b></p>"
                       "<p>This cannot be undone. Cancel active CloneStamp session?</p>",
                       String(),
                       StdIcon::Warning,
                       StdButton::No,
                       StdButton::Yes ).Execute() != StdButton::Yes )
      {
         return;
      }

   ProcessInterface::Cancel();
}

// ----------------------------------------------------------------------------

void CloneStampInterface::ResetInstance()
{
   CloneStampInstance defaultInstance( TheCloneStampProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool CloneStampInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "CloneStamp" );
      UpdateControls();
   }

   dynamic = true;
   return &P == TheCloneStampProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* CloneStampInterface::NewProcess() const
{
   CloneStampInstance* instance = new CloneStampInstance( TheCloneStampProcess );
   ExportInstance( *instance );
   return instance;
}

// ----------------------------------------------------------------------------

bool CloneStampInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const CloneStampInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a CloneStamp instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool CloneStampInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool CloneStampInterface::ImportProcess( const ProcessImplementation& p )
{
   const CloneStampInstance* instance = dynamic_cast<const CloneStampInstance*>( &p );
   if ( instance == nullptr )
      throw Error( "Not a CloneStamp instance." );

   Rect r( 0 );

   if ( targetView == nullptr )
   {
      ImageWindow w = ImageWindow::ActiveWindow();

      if ( w.IsNull() )
      {
         throw Error( "The CloneStamp interface requires an active image window to import a process instance." );
         return false;
      }

      View v = w.MainView();
      SelectTarget( v );
   }
   else
   {
      if ( !done.IsEmpty() )
         if ( MessageBox( "<p><b>Existing CloneStamp actions will be lost</b></p>"
                          "<p>This cannot be undone. Replace current CloneStamp instance?</p>",
                           String(),
                           StdIcon::Warning,
                           StdButton::No,
                           StdButton::Yes ).Execute() != StdButton::Yes )
         {
            return false;
         }

      r = Bounds();

      RestoreView();

      delta = 0;
      targetPos = 0;

      initialized = selectingSource = selectingTarget = false;
      executing = onTarget = dragging = imageChanged = false;

      done.Clear();
      undone.Clear();
   }

   try
   {
      double tsx = (instance->width > 0) ? double( targetView->Width() )/instance->width : 1.0;
      double tsy = (instance->height > 0) ? double( targetView->Height() )/instance->height : 1.0;

      uint32 actionIdx = 0;
      for ( CloneStampInstance::action_sequence::const_iterator i = instance->actions.Begin();
            i != instance->actions.End();
            ++i, ++actionIdx )
      {
         ImageWindow w = ImageWindow::Null();

         if ( !i->sourceId.IsEmpty() )
         {
            w = ImageWindow::WindowById( i->sourceId );
            if ( w.IsNull() )
               throw Error( "Source image not found: " + i->sourceId );
         }

         View view = w.IsNull() ? View::Null() : w.MainView();

         double ssx = (view.IsNull() || i->sourceWidth <= 0) ? tsx : double( view.Width() )/i->sourceWidth;
         double ssy = (view.IsNull() || i->sourceHeight <= 0) ? tsy : double( view.Height() )/i->sourceHeight;

         BrushData br( i->brush );
         br.radius = RoundInt( Max( tsx, tsy ) * br.radius );

         done.Add( ClonerAction( view, br, 0 ) );
         ClonerAction& action = *done.ReverseBegin();

         for ( cloner_sequence::const_iterator j = instance->cloner.Begin(); j != instance->cloner.End(); ++j )
            if ( j->actionIdx == actionIdx )
            {
               ClonerData cj( *j );

               if ( action.cloner.IsEmpty() )
               {
                  action.delta.x = RoundInt( tsx*cj.targetPos.x - ssx*i->sourcePos.x );
                  action.delta.y = RoundInt( tsy*cj.targetPos.y - ssy*i->sourcePos.y );
               }

               cj.targetPos.x = RoundInt( tsx*cj.targetPos.x );
               cj.targetPos.y = RoundInt( tsy*cj.targetPos.y );

               action.cloner.Add( cj );
            }

         action.UpdateBounds();
      }

      if ( !done.IsEmpty() )
      {
         const ClonerAction& action = *done.ConstReverseBegin();

         brush = action.brush;
         delta = action.delta;

         if ( action.source != nullptr && !action.source->view.IsNull() )
            sourceView = new View( action.source->view );

         initialized = true;
         selectingTarget = true;
      }

      color = instance->color;
      boundsColor = instance->boundsColor;
   }
   catch ( ... )
   {
      Reset();
      UpdateControls();
      throw;
   }

   if ( !done.IsEmpty() )
   {
      PlayBack( done.Begin(), done.End() );

      if ( r.IsRect() )
         r.Unite( Bounds() );
      else
         r = Bounds();
   }

   if ( targetView->Window().CurrentView() != *targetView )
      targetView->Window().SelectView( *targetView );
   else
   {
      if ( r.IsRect() )
         targetView->Window().RegenerateImageRect( r );
   }

   if ( r.IsRect() )
      Broadcast();

   UpdateControls();

   return true;
}

// ----------------------------------------------------------------------------

bool CloneStampInterface::IsDynamicInterface() const
{
   return true;
}

// ----------------------------------------------------------------------------

void CloneStampInterface::ExitDynamicMode()
{
   if ( targetView != nullptr )
      UndoAll();

   Reset();
   UpdateControls();
}

// ----------------------------------------------------------------------------

void CloneStampInterface::DynamicMouseEnter( View& v )
{
   if ( !executing )
      if ( targetView != nullptr )
         if ( v == *targetView )
         {
            onTarget = true;
            UpdateViews();
         }
}

void CloneStampInterface::DynamicMouseLeave( View& v )
{
   if ( !executing )
      if ( targetView != nullptr )
         if ( v == *targetView )
         {
            UpdateViews();
            onTarget = false;
         }
}

void CloneStampInterface::RasterizeStep( int dx, int dy )
{
   ClonerAction& action = *done.ReverseBegin();

   ClonerData d( *action.cloner.ReverseBegin() );
   d.targetPos.x += dx;
   d.targetPos.y += dy;

   action.cloner.Add( d );
   action.bounds.Unite( Rect( Point( d.targetPos ) - int( action.brush.radius ),
                              Point( d.targetPos ) + (int( action.brush.radius ) + 1) ) );
}

/*
 * Bresenham line raster algorithm
 */
void CloneStampInterface::Rasterize( int dx, int dy, int step )
{
   int xa, adx, ya, ady;

   if ( dx < 0 )
   {
      xa = -1;
      adx = -dx;
   }
   else
   {
      xa = 1;
      adx = dx;
   }

   if ( dy < 0 )
   {
      ya = -1;
      ady = -dy;
   }
   else
   {
      ya = 1;
      ady = dy;
   }

   //RasterizeStep( 0, 0 );

   if ( adx > ady )
   {
      int incr1 = ady << 1;
      int d = incr1 - adx;
      int incr2 = incr1 - (adx << 1);

      for ( int x = 0, y = 0, x0 = 0, y0 = 0; x != dx; )
      {
         x += xa;

         if ( d < 0 )
            d += incr1;
         else
         {
            y += ya;
            d += incr2;
         }

         if ( Abs( x - x0 ) >= step || x == dx )
         {
            RasterizeStep( x - x0, y - y0 );
            x0 = x;
            y0 = y;
         }
      }
   }
   else
   {
      int incr1 = adx << 1;
      int d = incr1 - ady;
      int incr2 = incr1 - (ady << 1);

      for ( int x = 0, y = 0, x0 = 0, y0 = 0; y != dy; )
      {
         y += ya;

         if ( d < 0 )
            d += incr1;
         else
         {
            x += xa;
            d += incr2;
         }

         if ( Abs( y - y0 ) >= step || y == dy )
         {
            RasterizeStep( x - x0, y - y0 );
            x0 = x;
            y0 = y;
         }
      }
   }
}

void CloneStampInterface::DynamicMouseMove( View& v, const DPoint& p, unsigned buttons, unsigned modifiers )
{
   if ( executing || targetView == nullptr || v != *targetView || !initialized )
      return;

   UpdateViews();

   if ( selectingTarget )
   {
      if ( !selectingSource )
      {
         delta = p - targetPos;
         UpdateHistoryInfo();
      }
   }
   else
      targetPos = p;

   if ( dragging && !selectingSource )
   {
      ClonerAction& action = *done.ReverseBegin();

      size_type n0 = action.cloner.Length();

      Point d( Point( targetPos ) - action.cloner.ReverseBegin()->targetPos );

      Rasterize( d.x, d.y, action.brush.radius >> 1 );

      SortedArray<int> tiles;

      ImageVariant image = targetView->Image();
      ImageVariant src = (action.source != nullptr) ? action.source->view.Image() : image;

      ImageWindow w = targetView->Window();

      ImageVariant mask;
      bool maskInverted = w.IsMaskInverted();
      if ( w.IsMaskEnabled() )
         if ( !w.Mask().IsNull() )
            mask = w.Mask().MainView().Image();

      image.ResetChannelRange();

      //targetView->Lock( false );

      Point idelta( delta );

      for ( cloner_sequence::const_iterator i = action.cloner.At( n0 ); i != action.cloner.End(); ++i )
      {
         Rect r( Point( i->targetPos ) - int( action.brush.radius ),
                 Point( i->targetPos ) + (int( action.brush.radius ) + 1) );

         if ( r.Intersect( image.Bounds() ) )
         {
            int row0 = r.y0/tileSize;
            int row1 = (r.y1 - 1)/tileSize;

            int col0 = r.x0/tileSize;
            int col1 = (r.x1 - 1)/tileSize;

            for ( int r = row0; r <= row1; ++r )
               for ( int c = col0, k = tileCols*r + col0; c <= col1; ++c, ++k )
                  if ( !tiles.Contains( k ) )
                  {
                     tiles.Add( k );

                     if ( !swapTiles[k] )
                     {
                        int x0 = c*tileSize;
                        int y0 = r*tileSize;
                        image.SelectRectangle( x0, y0, x0+tileSize, y0+tileSize );
                        swapTiles[k].CopyImage( image );
                     }
                  }

            CloneStampInstance::Apply( image, src, mask, maskInverted, action.brush, i, idelta );
         }
      }

      w.RegenerateImageRect( action.bounds );

      //UpdateHistoryInfo(); ### -> excessive overhead; will be updated in DynamicMouseRelease()
   }

   UpdateViews();
}

void CloneStampInterface::DynamicMousePress( View& v, const DPoint& p, int button, unsigned buttons, unsigned modifiers )
{
   if ( executing || button != MouseButton::Left )
      return;

   dragging = true;
   imageChanged = false;

   if ( targetView == nullptr )
   {
      if ( !v.IsMainView() )
      {
         dragging = false;
         throw Error( "<p>CloneStamp cannot run on previews. Please select a main view.</p>" );
      }

      SelectTarget( v );

      delta = 0;
      targetPos = p;
      onTarget = true;
   }
   else
   {
      UpdateViews();

      if ( v != *targetView || (modifiers & KeyModifier::Control) != 0 )
         selectingSource = true;

      if ( selectingSource )
      {
         SelectSource( v );

         delta = 0;
         targetPos = p;
      }
      else
      {
         try
         {
            if ( !initialized )
               throw Error( "<p>No source point has been selected yet for this CloneStamp session.</p>"
                            "<p>Please select a valid source point ("
#ifdef __PCL_MACOSX
                            "Cmd"
#else
                            "Ctrl"
#endif
                            "+click on a main view) before attempting to perform clone stamp actions.</p>" );

            //targetView->Lock( false );

            if ( selectingTarget )
            {
               delta = p - targetPos;
               selectingTarget = false;
            }

            targetPos = p;

            undone.Clear();

            done.Add( ClonerAction( (sourceView != nullptr) ? *sourceView : View::Null(), brush, Point( delta ) ) );
            ClonerAction& action = *done.ReverseBegin();

            action.cloner.Add( ClonerData( Point( targetPos ) ) );
            action.UpdateBounds();

            StoreRect( action.bounds );

            ImageVariant image = targetView->Image();
            ImageVariant src = (action.source != nullptr) ? action.source->view.Image() : image;

            ImageWindow w = targetView->Window();

            ImageVariant mask;
            bool maskInverted = w.IsMaskInverted();
            if ( w.IsMaskEnabled() )
               if ( !w.Mask().IsNull() )
                  mask = w.Mask().MainView().Image();

            CloneStampInstance::Apply( image, src, mask, maskInverted, action.brush, action.cloner.ReverseBegin(), Point( delta ) );

            imageChanged = true;

            //targetView->Unlock( false );
            w.RegenerateImageRect( action.bounds );
         }

         ERROR_CLEANUP( dragging = false; )
      }
   }

   UpdateControls();
   UpdateViews();
}

void CloneStampInterface::DynamicMouseRelease( View& v, const DPoint& p, int button, unsigned buttons, unsigned modifiers )
{
   dragging = false;

   if ( targetView != nullptr )
   {
      if ( selectingSource )
      {
         selectingSource = false;
         selectingTarget = true;
      }

      if ( imageChanged )
      {
         Broadcast();
         imageChanged = false;
      }

      UpdateHistoryInfo();
   }
}

// ----------------------------------------------------------------------------

bool CloneStampInterface::DynamicKeyPress( View& v, int key, unsigned modifiers )
{
   if ( targetView == nullptr )
      return false;

   switch ( key )
   {
   case KeyCode::Control:
      UpdateViews();
      selectingSource = true;
      UpdateHistoryInfo();
      UpdateViews();
      break;

   case KeyCode::Z:
      if ( modifiers & KeyModifier::Control )
      {
         if ( modifiers & KeyModifier::Shift )
            UndoAll();
         else
            Undo();
      }
      else
         return false;
      break;

   case KeyCode::Y:
      if ( modifiers & KeyModifier::Control )
      {
         if ( modifiers & KeyModifier::Shift )
            RedoAll();
         else
            Redo();
      }
      else
         return false;
      break;

   case KeyCode::Delete:
      if ( modifiers == 0 )
         Delete();
      break;

   default:
      return false;
   }

   return true;
}

bool CloneStampInterface::DynamicKeyRelease( View& v, int key, unsigned modifiers )
{
   if ( targetView == nullptr )
      return false;

   switch ( key )
   {
   case KeyCode::Control:
      UpdateViews();
      selectingSource = false;
      UpdateHistoryInfo();
      UpdateViews();
      break;

   default:
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool CloneStampInterface::RequiresDynamicUpdate( const View& v, const DRect& updateRect ) const
{
   if ( targetView == nullptr || (v != *targetView && (sourceView == nullptr || v != *sourceView)) || !initialized )
      return false;

   if ( onTarget )
   {
      ImageWindow targetWindow = targetView->Window();
      ImageWindow sourceWindow = (sourceView != nullptr) ? sourceView->Window() : targetWindow;

      DPoint t = TargetPos();
      DPoint s = SourcePos();

      if ( v == *targetView && !selectingSource )
      {
         // Brush
         if ( DRect( t - double( brush.radius ), t + double( brush.radius ) ).Intersects( updateRect ) )
            return true;

         // Vector
         if ( sourceView == nullptr )
         {
            DRect r( s, t );
            r.Order();
            if ( r.Intersects( updateRect ) )
               return true;
         }
      }

      if ( sourceView == nullptr || v == *sourceView )
      {
         // Source point (add 1 pixel below to guard against roundoff errors)
         double dr = sourceWindow.ViewportScalarToImage( sourceWindow.DisplayPixelRatio()*(CENTER_RADIUS + 2) );
         if ( DRect( s - dr, s + dr ).Intersects( updateRect ) )
            return true;
      }
   }

   // Action boundaries
   if ( showBounds && v == *targetView && !done.IsEmpty() )
      if ( done.ReverseBegin()->bounds.Intersects( updateRect ) )
         return true;

   return false;
}

void CloneStampInterface::DynamicPaint( const View& v, VectorGraphics& g, const DRect& ur  ) const
{
   if ( targetView == nullptr || (v != *targetView && (sourceView == nullptr || v != *sourceView)) || !initialized )
      return;

   ImageWindow window = v.Window();

   // Update rectangle, integer viewport coordinates.
   // Inflated rectangle coordinates compensate for rounding errors.
   Rect r0 = window.ImageToViewport( ur ).TruncatedToInt().InflatedBy( 1 );

   g.SetCompositionOperator( CompositionOp::Difference );
   g.EnableAntialiasing();
   g.SetBrush( Brush::Null() );

   if ( onTarget )
   {
      g.SetPen( color, window.DisplayPixelRatio() );

      DPoint t = TargetPos();
      DPoint s = SourcePos();

      if ( v == *targetView && !selectingSource )
      {
         // Brush circle
         double r = window.ImageScalarToViewport( double( brush.radius ) );
         if ( r >= 1 )
            g.DrawCircle( window.ImageToViewport( t ), r );

         // Vector line
         if ( sourceView == nullptr )
            g.DrawLine( window.ImageToViewport( s ), window.ImageToViewport( t ) );
      }

      if ( sourceView == nullptr || v == *sourceView )
      {
         // Source point
         DPoint p1 = window.ImageToViewport( s );
         double centerRadius = window.DisplayPixelRatio() * CENTER_RADIUS;
         DPoint c1 = p1 - centerRadius;
         DPoint c2 = p1 + centerRadius;
         g.DrawLine( c1.x, c1.y, c2.x, c2.y );
         g.DrawLine( c2.x, c1.y, c1.x, c2.y );
      }
   }

   // Action boundaries
   if ( showBounds && v == *targetView && !done.IsEmpty() )
   {
      Rect r = window.ImageToViewport( done.ReverseBegin()->bounds );
      if ( r.Intersects( r0 ) )
      {
         g.SetPen( boundsColor, window.DisplayPixelRatio() );
         g.DrawRect( r );
      }
   }
}

void CloneStampInterface::SaveSettings() const
{
}

void CloneStampInterface::LoadSettings()
{
}

bool CloneStampInterface::WantsMaskNotifications() const
{
   return true;
}

void CloneStampInterface::MaskUpdated( const View& v )
{
   if ( targetView != nullptr && v.Window() == targetView->Window() )
   {
      UndoAll();
      RedoAll();
   }
}

void CloneStampInterface::MaskEnabled( const View& v )
{
   MaskUpdated( v );
}

void CloneStampInterface::MaskDisabled( const View& v )
{
   MaskUpdated( v );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CloneStampInterface::UpdateControls()
{
   GUI->BrushRadius_SpinBox.SetValue( brush.radius );
   GUI->ShowBounds_CheckBox.SetChecked( showBounds );
   GUI->BrushSoftness_NumericControl.SetValue( brush.softness );
   GUI->BrushOpacity_NumericControl.SetValue( brush.opacity );

   bool isActive = targetView != nullptr;

   GUI->BrushRadius_SpinBox.Enable( isActive );
   GUI->CopyBrush_PushButton.Enable( isActive && !done.IsEmpty() && brush != done.ReverseBegin()->brush );
   GUI->ShowBounds_CheckBox.Enable( isActive );
   GUI->BrushSoftness_NumericControl.Enable( isActive && brush.radius > 0 );
   GUI->BrushOpacity_NumericControl.Enable( isActive );

   GUI->Delete_ToolButton.Enable( isActive && !done.IsEmpty() );
   GUI->UndoAll_ToolButton.Enable( isActive && !done.IsEmpty() );
   GUI->UndoOne_ToolButton.Enable( isActive && !done.IsEmpty() );
   GUI->RedoOne_ToolButton.Enable( isActive && !undone.IsEmpty() );
   GUI->RedoAll_ToolButton.Enable( isActive && !undone.IsEmpty() );

   UpdateHistoryInfo();
}

void CloneStampInterface::UpdateHistoryInfo()
{
   String info;

   if ( targetView != nullptr )
   {
      if ( !initialized )
         info = "No source point defined";
      else if ( selectingSource )
         info = "Selecting source point";
      else if ( selectingTarget )
         info.Format( "Selecting target point: dx=%+d dy=%+d", int( delta.x ), int( delta.y ) );
      else
      {
         if ( done.IsEmpty() )
         {
            if ( undone.IsEmpty() )
               info = "No CloneStamp actions";
            else
               info.Format( "Local history: 0/%u", undone.Length() );
         }
         else
         {
            if ( dragging )
               info.Format( "Local history: %u/%u (...)",
                        done.Length(), done.Length()+undone.Length() );
            else
               info.Format( "Local history: %u/%u (%u)",
                        done.Length(), done.Length()+undone.Length(), done.ReverseBegin()->cloner.Length() );
         }
      }
   }
   else
      info = "No target view selected";

   GUI->HistoryInfo_Label.SetText( info );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void CloneStampInterface::__BrushRadius_ValueUpdated( SpinBox& sender, int value )
{
   UpdateViews();

   brush.radius = int32( value );

   UpdateViews();
   GUI->Brush_Control.Update();
}

void CloneStampInterface::__Brush_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->BrushSoftness_NumericControl )
      brush.softness = value;
   else if ( sender == GUI->BrushOpacity_NumericControl )
      brush.opacity = value;

   GUI->Brush_Control.Update();
}

void CloneStampInterface::__History_Click( Button& sender, bool checked )
{
   if ( sender == GUI->Delete_ToolButton )
      Delete();
   else if ( sender == GUI->UndoAll_ToolButton )
      UndoAll();
   else if ( sender == GUI->UndoOne_ToolButton )
      Undo();
   else if ( sender == GUI->RedoOne_ToolButton )
      Redo();
   else if ( sender == GUI->RedoAll_ToolButton )
      RedoAll();

   UpdateHistoryInfo();
}

void CloneStampInterface::__Brush_Paint( Control& sender, const Rect& updateRect )
{
   KernelFilter flt = brush.CreateBrush();
   const float* f = flt.Begin();
   const float* m = f;

   int n = 1 + (brush.radius << 1);

   Bitmap bmp( n, n, BitmapFormat::RGB32 );
   for ( int y = 0; y < n; ++y )
      for ( int x = 0; x < n; ++x, ++m )
      {
         int v = RoundInt( *m * 255 );
         bmp.SetPixel( x, y, RGBAColor( v, v, v ) );
      }

   Graphics g( sender );
   g.DisableSmoothInterpolation();
   g.DrawScaledBitmap( sender.BoundsRect(), bmp );
}

void CloneStampInterface::__Option_Click( Button& sender, bool checked )
{
   if ( sender == GUI->CopyBrush_PushButton )
   {
      if ( !done.IsEmpty() )
      {
         UpdateViews();
         brush = done.ReverseBegin()->brush;
         UpdateViews();
         UpdateControls();
      }
   }
   else if ( sender == GUI->ShowBounds_CheckBox )
   {
      UpdateViews();
      showBounds = checked;
      UpdateViews();
   }
}

#ifndef __PCL_WINDOWS

void CloneStampInterface::__Regen_Timer( Timer& sender )
{
   if ( targetView != nullptr && targetView->CanRead() )
      if ( regenRect.IsRect() )
      {
         targetView->Window().RegenerateImageRect( regenRect );
         regenRect = 0;
      }
}

#endif // !__PCL_WINDOWS

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CloneStampInterface::GUIData::GUIData( CloneStampInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Softness:" ) + 'T' );
   int editWidth1  = fnt.Width( String( '0', 8 ) );

   //

   BrushRadius_Label.SetText( "Radius:" );
   BrushRadius_Label.SetFixedWidth( labelWidth1 );
   BrushRadius_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BrushRadius_SpinBox.SetRange( int( TheCSRadiusParameter->MinimumValue() ), int( TheCSRadiusParameter->MaximumValue() ) );
   BrushRadius_SpinBox.SetFixedWidth( editWidth1 );
   BrushRadius_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&CloneStampInterface::__BrushRadius_ValueUpdated, w );

   CopyBrush_PushButton.SetText( "Copy brush" );
   CopyBrush_PushButton.SetToolTip( "Copy current action brush parameters" );
   CopyBrush_PushButton.OnClick( (Button::click_event_handler)&CloneStampInterface::__Option_Click, w );

   ShowBounds_CheckBox.SetText( "Show bounds" );
   ShowBounds_CheckBox.SetToolTip( "Show current action boundaries / track actions" );
   ShowBounds_CheckBox.OnClick( (Button::click_event_handler)&CloneStampInterface::__Option_Click, w );

   BrushRadius_Sizer.SetSpacing( 4 );
   BrushRadius_Sizer.Add( BrushRadius_Label );
   BrushRadius_Sizer.Add( BrushRadius_SpinBox );
   BrushRadius_Sizer.AddStretch();
   BrushRadius_Sizer.Add( CopyBrush_PushButton );
   BrushRadius_Sizer.Add( ShowBounds_CheckBox );

   BrushSoftness_NumericControl.label.SetText( "Softness:" );
   BrushSoftness_NumericControl.label.SetFixedWidth( labelWidth1 );
   BrushSoftness_NumericControl.slider.SetScaledMinWidth( 200 );
   BrushSoftness_NumericControl.slider.SetRange( 0, 100 );
   BrushSoftness_NumericControl.SetReal();
   BrushSoftness_NumericControl.SetRange( 0.0, 1.0 );
   BrushSoftness_NumericControl.SetPrecision( 2 );
   BrushSoftness_NumericControl.edit.SetMinWidth( editWidth1 );
   BrushSoftness_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CloneStampInterface::__Brush_ValueUpdated, w );

   BrushOpacity_NumericControl.label.SetText( "Opacity:" );
   BrushOpacity_NumericControl.label.SetFixedWidth( labelWidth1 );
   BrushOpacity_NumericControl.slider.SetScaledMinWidth( 200 );
   BrushOpacity_NumericControl.slider.SetRange( 0, 100 );
   BrushOpacity_NumericControl.SetReal();
   BrushOpacity_NumericControl.SetRange( 0.0, 1.0 );
   BrushOpacity_NumericControl.SetPrecision( 2 );
   BrushOpacity_NumericControl.edit.SetMinWidth( editWidth1 );
   BrushOpacity_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CloneStampInterface::__Brush_ValueUpdated, w );

   BrushParameters_Sizer.SetSpacing( 4 );
   BrushParameters_Sizer.Add( BrushRadius_Sizer );
   BrushParameters_Sizer.AddStretch();
   BrushParameters_Sizer.Add( BrushSoftness_NumericControl );
   BrushParameters_Sizer.Add( BrushOpacity_NumericControl );

   Brush_Control.SetScaledFixedSize( 80, 80 );
   Brush_Control.OnPaint( (Control::paint_event_handler)&CloneStampInterface::__Brush_Paint, w );

   Brush_Sizer.SetSpacing( 4 );
   Brush_Sizer.Add( BrushParameters_Sizer );
   Brush_Sizer.Add( Brush_Control );

   //

   HistoryInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   Delete_ToolButton.SetIcon( w.ScaledResource( ":/icons/delete.png" ) );
   Delete_ToolButton.SetToolTip( "Delete" );
   Delete_ToolButton.OnClick( (Button::click_event_handler)&CloneStampInterface::__History_Click, w );

   UndoAll_ToolButton.SetIcon( w.ScaledResource( ":/icons/goto-first.png" ) );
   UndoAll_ToolButton.SetToolTip( "Undo All" );
   UndoAll_ToolButton.OnClick( (Button::click_event_handler)&CloneStampInterface::__History_Click, w );

   UndoOne_ToolButton.SetIcon( w.ScaledResource( ":/icons/previous.png" ) );
   UndoOne_ToolButton.SetToolTip( "Undo" );
   UndoOne_ToolButton.OnClick( (Button::click_event_handler)&CloneStampInterface::__History_Click, w );

   RedoOne_ToolButton.SetIcon( w.ScaledResource( ":/icons/next.png" ) );
   RedoOne_ToolButton.SetToolTip( "Redo" );
   RedoOne_ToolButton.OnClick( (Button::click_event_handler)&CloneStampInterface::__History_Click, w );

   RedoAll_ToolButton.SetIcon( w.ScaledResource( ":/icons/goto-last.png" ) );
   RedoAll_ToolButton.SetToolTip( "Redo All" );
   RedoAll_ToolButton.OnClick( (Button::click_event_handler)&CloneStampInterface::__History_Click, w );

   History_Sizer.SetSpacing( 4 );
   History_Sizer.Add( HistoryInfo_Label, 100 );
   History_Sizer.Add( Delete_ToolButton );
   History_Sizer.Add( UndoAll_ToolButton );
   History_Sizer.Add( UndoOne_ToolButton );
   History_Sizer.Add( RedoOne_ToolButton );
   History_Sizer.Add( RedoAll_ToolButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Brush_Sizer );
   Global_Sizer.Add( History_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   //

#ifndef __PCL_WINDOWS
   Regen_Timer.SetPeriodic( true );
   Regen_Timer.SetInterval( 0.025 );
   Regen_Timer.OnTimer( (Timer::timer_event_handler)&CloneStampInterface::__Regen_Timer, w );
#endif
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CloneStampInterface.cpp - Released 2017-04-14T23:07:12Z
