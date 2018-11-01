//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0410
// ----------------------------------------------------------------------------
// ExtractAlphaChannelsInstance.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "ExtractAlphaChannelsInstance.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ExtractAlphaChannelsInstance::ExtractAlphaChannelsInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   channels( EAChannels::Default ),
   channelList( TheEAChannelListParameter->DefaultValue() ),
   extractChannels( TheEAExtractParameter->DefaultValue() ),
   deleteChannels( TheEADeleteParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

ExtractAlphaChannelsInstance::ExtractAlphaChannelsInstance( const ExtractAlphaChannelsInstance& x ) : ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ExtractAlphaChannelsInstance::Assign( const ProcessImplementation& p )
{
   const ExtractAlphaChannelsInstance* x = dynamic_cast<const ExtractAlphaChannelsInstance*>( &p );
   if ( x != nullptr && x != this )
   {
      channels        = x->channels;
      channelList     = x->channelList;
      extractChannels = x->extractChannels;
      deleteChannels  = x->deleteChannels;
   }
}

// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsInstance::Validate( String& info )
{
   if ( !extractChannels && !deleteChannels )
   {
      info = "The instance does not specify an action (neither extract nor delete alpha channels)";
      return false;
   }

   if ( channels == EAChannels::ChannelList )
   {
      String s = channelList;
      s.Trim();

      if ( s.IsEmpty() )
      {
         info = "Empty channel list";
         return false;
      }
   }

   info.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsInstance::IsHistoryUpdater( const View& ) const
{
   return deleteChannels;
}

// ----------------------------------------------------------------------------

UndoFlags ExtractAlphaChannelsInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.IsPreview() )
   {
      whyNot = "ExtractAlphaChannels cannot be executed on previews.";
      return false;
   }

   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ExtractAlphaChannels cannot be executed on complex images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template <class P>
static void ExtractAllAlphaChannels( GenericImage<P>& image, bool extractChannels, bool deleteChannels, const String& baseId )
{
   int n = image.NumberOfAlphaChannels();
   if ( n == 0 )
      throw Error( "No alpha channels to extract!" );

   Array<typename P::sample**> data;

   try
   {
      if ( deleteChannels )
      {
         if ( extractChannels )
         {
            for ( int c = 0, c0 = image.NumberOfNominalChannels(); c < n; ++c )
            {
               typename P::sample** d = image.Allocator().AllocateChannelSlots( 1 );
               *d = image.PixelData( c+c0 );
               data.Add( d );
            }

            image.ForgetAlphaChannels();

            for ( int c = 0; c < n; ++c )
            {
               String id = baseId + String().Format( "_alpha%02d", c+1 );

               ImageWindow w( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false, true, id );
               if ( w.IsNull() )
                  throw Error( "Unable to create image window: " + id );

               static_cast<GenericImage<P>&>( *w.MainView().Image() ).ImportData( data[c], image.Width(), image.Height() );
               data[c] = nullptr;

               w.Show();
               w.ZoomToFit( false );
            }

            Console().WriteLn( String().Format( "%d alpha channel(s) extracted and deleted.", n ) );
         }
         else
         {
            image.DeleteAlphaChannels();
            Console().WriteLn( String().Format( "%d alpha channel(s) deleted.", n ) );
         }
      }
      else
      {
         if ( extractChannels )
         {
            for ( int c = 0, c0 = image.NumberOfNominalChannels(); c < n; ++c )
            {
               String id = baseId + String().Format( "_alpha%02d", c+1 );

               ImageWindow w( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false, true, id );
               if ( w.IsNull() )
                  throw Error( "Unable to create image window: " + id );

               image.SelectChannel( c+c0 );
               static_cast<GenericImage<P>&>( *w.MainView().Image() ) = image;

               w.Show();
               w.ZoomToFit( false );

               Console().WriteLn( String().Format( "%d alpha channel(s) extracted.", n ) );
            }
         }
      }
   }
   catch ( ... )
   {
      for ( size_type i = 0; i < data.Length(); ++i )
         if ( data[i] != nullptr )
         {
            if ( *data[i] != nullptr )
               image.Allocator().Deallocate( *data[i] );
            image.Allocator().Deallocate( data[i] );
         }

      throw;
   }
}

// ----------------------------------------------------------------------------

static void ExtractAllAlphaChannels( ImageVariant& image, bool extractChannels, bool deleteChannels, const String& baseId )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         ExtractAllAlphaChannels( static_cast<Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      case 64:
         ExtractAllAlphaChannels( static_cast<DImage&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         ExtractAllAlphaChannels( static_cast<UInt8Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      case 16:
         ExtractAllAlphaChannels( static_cast<UInt16Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      case 32:
         ExtractAllAlphaChannels( static_cast<UInt32Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template <class P>
static void ExtractActiveAlphaChannel( GenericImage<P>& image, bool extractChannels, bool deleteChannels, const String& baseId )
{
   if ( !image.HasAlphaChannels() )
      throw Error( "No alpha channels to extract!" );

   typename P::sample** data = nullptr;

   String id;
   if ( extractChannels )
      id = baseId + String().Format( "_alpha%02d", 1 );

   try
   {
      if ( deleteChannels )
      {
         if ( extractChannels )
         {
            data = image.Allocator().AllocateChannelSlots( 1 );
            *data = image.PixelData( image.NumberOfNominalChannels() );

            image.ForgetAlphaChannel( 0 );

            ImageWindow w( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false, true, id );
            if ( w.IsNull() )
               throw Error( "Unable to create image window: " + id );

            static_cast<GenericImage<P>&>( *w.MainView().Image() ).ImportData( data, image.Width(), image.Height() );
            w.Show();
            w.ZoomToFit( false );

            Console().WriteLn( "Active alpha channel extracted and deleted." );
         }
         else
         {
            image.DeleteAlphaChannel( 0 );
            Console().WriteLn( "Active alpha channel deleted." );
         }
      }
      else
      {
         if ( extractChannels )
         {
            ImageWindow w( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false, true, id );
            if ( w.IsNull() )
               throw Error( "Unable to create image window: " + id );

            image.SelectChannel( image.NumberOfNominalChannels() );
            static_cast<GenericImage<P>&>( *w.MainView().Image() ) = image;

            w.Show();
            w.ZoomToFit( false );

            Console().WriteLn( "Active alpha channel extracted." );
         }
      }
   }
   catch ( ... )
   {
      if ( data != nullptr )
      {
         if ( *data != nullptr )
            image.Allocator().Deallocate( *data );
         image.Allocator().Deallocate( data );
      }

      throw;
   }
}

// ----------------------------------------------------------------------------

static void ExtractActiveAlphaChannel( ImageVariant& image, bool extractChannels, bool deleteChannels, const String& baseId )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         ExtractActiveAlphaChannel( static_cast<Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      case 64:
         ExtractActiveAlphaChannel( static_cast<DImage&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         ExtractActiveAlphaChannel( static_cast<UInt8Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      case 16:
         ExtractActiveAlphaChannel( static_cast<UInt16Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      case 32:
         ExtractActiveAlphaChannel( static_cast<UInt32Image&>( *image ), extractChannels, deleteChannels, baseId );
         break;
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template <class P>
static void ExtractListOfAlphaChannels( GenericImage<P>& image, const SortedArray<int>& list,
                                        bool extractChannels, bool deleteChannels, const String& baseId )
{
   if ( !image.HasAlphaChannels() )
      throw Error( "No alpha channels to extract!" );

   int n = int( list.Length() );
   if ( n == 0 )
      throw Error( "Empty channel list!" );

   Array<typename P::sample**> data;

   try
   {
      int count = 0;

      if ( deleteChannels )
      {
         if ( extractChannels )
         {
            for ( int i = 0; i < n; ++i )
            {
               int c = list[i];
               if ( c >= 0 && c < image.NumberOfAlphaChannels() )
               {
                  typename P::sample** d = image.Allocator().AllocateChannelSlots( 1 );
                  *d = image.PixelData( c + image.NumberOfNominalChannels() );
                  data.Add( d );
                  ++count;
               }
               else
                  data.Add( nullptr );
            }

            for ( int i = 0; i < n; ++i )
            {
               int c = list[i];
               if ( c >= 0 && c < image.NumberOfAlphaChannels() )
                  image.ForgetAlphaChannel( c );
            }

            for ( int i = 0; i < n; ++i )
               if ( data[i] != nullptr )
               {
                  String id = baseId + String().Format( "_alpha%02d", list[i]+1 );

                  ImageWindow w( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false, true, id );
                  if ( w.IsNull() )
                     throw Error( "Unable to create image window: " + id );

                  static_cast<GenericImage<P>&>( *w.MainView().Image() ).ImportData( data[i], image.Width(), image.Height() );
                  data[i] = nullptr;

                  w.Show();
                  w.ZoomToFit( false );
               }

            Console().WriteLn( String().Format( "%d alpha channel(s) extracted and deleted.", count ) );
         }
         else
         {
            for ( SortedArray<int>::const_iterator i = list.ReverseBegin(); i != list.ReverseEnd(); ++i )
            {
               int c = *i;
               if ( c >= 0 && c < image.NumberOfAlphaChannels() )
                  image.DeleteAlphaChannel( c ), ++count;
            }

            Console().WriteLn( String().Format( "%d alpha channel(s) deleted.", count ) );
         }
      }
      else
      {
         if ( extractChannels )
         {
            for ( int i = 0; i < n; ++i )
            {
               int c = list[i];
               if ( c >= 0 && c < image.NumberOfAlphaChannels() )
               {
                  String id = baseId + String().Format( "_alpha%02d", c+1 );

                  ImageWindow w( 1, 1, 1, P::BitsPerSample(), P::IsFloatSample(), false, true, id );
                  if ( w.IsNull() )
                     throw Error( "Unable to create image window: " + id );

                  image.SelectChannel( c + image.NumberOfNominalChannels() );
                  static_cast<GenericImage<P>&>( *w.MainView().Image() ) = image;

                  w.Show();
                  w.ZoomToFit( false );

                  ++count;
               }
            }

            Console().WriteLn( String().Format( "%d alpha channel(s) extracted.", count ) );
         }
      }
   }
   catch ( ... )
   {
      for ( size_type i = 0; i < data.Length(); ++i )
         if ( data[i] != nullptr )
         {
            if ( *data[i] != nullptr )
               image.Allocator().Deallocate( *data[i] );
            image.Allocator().Deallocate( data[i] );
         }

      throw;
   }
}

// ----------------------------------------------------------------------------

static void ExtractListOfAlphaChannels( ImageVariant& image, const SortedArray<int>& list,
                                        bool extractChannels, bool deleteChannels, const String& baseId )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         ExtractListOfAlphaChannels( static_cast<Image&>( *image ), list, extractChannels, deleteChannels, baseId );
         break;
      case 64:
         ExtractListOfAlphaChannels( static_cast<DImage&>( *image ), list, extractChannels, deleteChannels, baseId );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         ExtractListOfAlphaChannels( static_cast<UInt8Image&>( *image ), list, extractChannels, deleteChannels, baseId );
         break;
      case 16:
         ExtractListOfAlphaChannels( static_cast<UInt16Image&>( *image ), list, extractChannels, deleteChannels, baseId );
         break;
      case 32:
         ExtractListOfAlphaChannels( static_cast<UInt32Image&>( *image ), list, extractChannels, deleteChannels, baseId );
         break;
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsInstance::ExecuteOn( View& view )
{
   if ( view.IsPreview() )
      return false;

   if ( !extractChannels && !deleteChannels )
      return false;

   AutoViewLock lock( view, false/*lock*/ );
   if ( deleteChannels )
      lock.Lock();
   else
      lock.LockForWrite();

   ImageVariant image = view.Image();
   String baseId = view.Id();
   switch ( channels )
   {
   default:
   case EAChannels::AllAlphaChannels:
      ExtractAllAlphaChannels( image, extractChannels, deleteChannels, baseId );
      break;
   case EAChannels::ActiveAlphaChannel:
      ExtractActiveAlphaChannel( image, extractChannels, deleteChannels, baseId );
      break;
   case EAChannels::ChannelList:
      {
         SortedArray<int> list;
         ParseChannelList( list, channelList );
         ExtractListOfAlphaChannels( image, list, extractChannels, deleteChannels, baseId );
      }
      break;
   }

   return true;
}

// ----------------------------------------------------------------------------

void* ExtractAlphaChannelsInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheEAChannelsParameter )
      return &channels;
   if ( p == TheEAChannelListParameter )
      return channelList.Begin();
   if ( p == TheEAExtractParameter )
      return &extractChannels;
   if ( p == TheEADeleteParameter )
      return &deleteChannels;
   return nullptr;
}

// ----------------------------------------------------------------------------

bool ExtractAlphaChannelsInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheEAChannelListParameter )
   {
      channelList.Clear();
      if ( sizeOrLength > 0 )
         channelList.SetLength( sizeOrLength );
   }
   else
      return false;
   return true;
}

// ----------------------------------------------------------------------------

size_type ExtractAlphaChannelsInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheEAChannelListParameter )
      return channelList.Length();
   return 0;
}

// ----------------------------------------------------------------------------

void ExtractAlphaChannelsInstance::ParseChannelList( SortedArray<int>& list, const String& s )
{
   StringList slist;
   s.Break( slist, ',' );
   slist.Remove( String() );
   if ( slist.IsEmpty() )
      throw Error( "Empty channel list" );

   list.Clear();
   for ( StringList::const_iterator i = slist.Begin(); i != slist.End(); ++i )
   {
      int c = i->ToInt();
      if ( !list.Contains( c ) )
         list.Add( c );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ExtractAlphaChannelsInstance.cpp - Released 2018-11-01T11:07:21Z
