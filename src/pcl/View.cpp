//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/View.cpp - Released 2017-07-09T18:07:16Z
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

#include <pcl/AutoLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

View& View::Null()
{
   static View* nullView = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullView == nullptr )
      nullView = new View( nullptr );
   return *nullView;
}

// ----------------------------------------------------------------------------

bool View::IsMainView() const
{
   return (*API->View->IsPreview)( handle ) == api_false;
}

// ----------------------------------------------------------------------------

bool View::IsPreview() const
{
   return (*API->View->IsPreview)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool View::IsVolatilePreview() const
{
   return (*API->View->IsVolatilePreview)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool View::IsStoredPreview() const
{
   return (*API->View->IsStoredPreview)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

ImageWindow View::Window() const
{
   return ImageWindow( (*API->View->GetViewParentWindow)( handle ) );
}

// ----------------------------------------------------------------------------

IsoString View::Id() const
{
   size_type len = 0;
   (*API->View->GetViewId)( handle, 0, &len );

   IsoString id;
   if ( len > 0 )
   {
      id.SetLength( len );
      if ( (*API->View->GetViewId)( handle, id.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetViewId" );
      id.ResizeToNullTerminated();
   }
   return id;
}

// ----------------------------------------------------------------------------

IsoString View::FullId() const
{
   size_type len = 0;
   (*API->View->GetViewFullId)( handle, 0, &len );

   IsoString id;
   if ( len > 0 )
   {
      id.SetLength( len );
      if ( (*API->View->GetViewFullId)( handle, id.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetViewFullId" );
      id.ResizeToNullTerminated();
   }
   return id;
}

// ----------------------------------------------------------------------------

void View::Rename( const IsoString& newId )
{
   (*API->View->SetViewId)( handle, newId.c_str() );
}

// ----------------------------------------------------------------------------

bool View::CanRead() const
{
   api_bool r = api_false, w = api_false;
   (*API->View->GetViewLocks)( handle, &r, &w );
   return r != api_false;
}

// ----------------------------------------------------------------------------

bool View::CanWrite() const
{
   api_bool r = api_false, w = api_false;
   (*API->View->GetViewLocks)( handle, &r, &w );
   return w != api_false;
}

// ----------------------------------------------------------------------------

void View::Lock( bool notify ) const
{
   if ( !Thread::IsRootThread() )
      throw Error( "Only the root thread can lock a view: " + FullId() );

   api_bool r = api_false, w = api_false;
   (*API->View->GetViewLocks)( handle, &r, &w );

   if ( w == api_false || r == api_false )
      throw Error( "The view is locked for read/write operations: " + FullId() );

   (*API->View->LockView)( handle, true, true, notify );
}

// ----------------------------------------------------------------------------

void View::Unlock( bool notify ) const
{
   (*API->View->UnlockView)( handle, true, true, notify );
}

// ----------------------------------------------------------------------------

void View::LockForWrite( bool notify ) const
{
   if ( !Thread::IsRootThread() )
      throw Error( "Only the root thread can lock a view: " + FullId() );

   api_bool r = api_false, w = api_false;
   (*API->View->GetViewLocks)( handle, &r, &w );

   if ( w == api_false )
      throw Error( "The view is locked for write operations: " + FullId() );

   (*API->View->LockView)( handle, false, true, notify );
}

// ----------------------------------------------------------------------------

void View::UnlockForWrite( bool notify ) const
{
   (*API->View->UnlockView)( handle, false, true, notify );
}

// ----------------------------------------------------------------------------

void View::UnlockForRead( bool notify ) const
{
   (*API->View->UnlockView)( handle, true, false, notify );
}

// ----------------------------------------------------------------------------

void View::RelockForRead( bool notify ) const
{
   if ( !Thread::IsRootThread() )
      throw Error( "Only the root thread can lock a view: " + FullId() );

   api_bool r = api_false, w = api_false;
   (*API->View->GetViewLocks)( handle, &r, &w );

   if ( r == api_false )
      throw Error( "The view is locked for read operations: " + FullId() );

   (*API->View->LockView)( handle, true, false, notify );
}

// ----------------------------------------------------------------------------

bool View::IsDynamicTarget() const
{
   return (*API->View->IsViewDynamicTarget)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void View::AddToDynamicTargets()
{
   (*API->View->AddViewToDynamicTargets)( handle );
}

// ----------------------------------------------------------------------------

void View::RemoveFromDynamicTargets()
{
   (*API->View->RemoveViewFromDynamicTargets)( handle );
}

// ----------------------------------------------------------------------------

ImageVariant View::Image() const
{
   image_handle hImg = (*API->View->GetViewImage)( handle );
   if ( hImg != 0 )
   {
      uint32 bitsPerSample;
      api_bool isFloat;

      if ( !(*API->SharedImage->GetImageFormat)( hImg, &bitsPerSample, &isFloat ) )
         throw APIFunctionError( "GetImageFormat" );

      /*
       * Use a private ImageVariant constructor that forces image ownership by
       * ImageVariant. This constructor is selected with the second dummy int
       * argument (set to zero below).
       */
      if ( isFloat )
         switch ( bitsPerSample )
         {
         case 32 : return ImageVariant( new pcl::Image( hImg ), 0 );
         case 64 : return ImageVariant( new pcl::DImage( hImg ), 0 );
         }
      else
         switch ( bitsPerSample )
         {
         case  8 : return ImageVariant( new pcl::UInt8Image( hImg ), 0 );
         case 16 : return ImageVariant( new pcl::UInt16Image( hImg ), 0 );
         case 32 : return ImageVariant( new pcl::UInt32Image( hImg ), 0 );
         }
   }

   return ImageVariant();
}

// ----------------------------------------------------------------------------

bool View::IsColor() const
{
   return (*API->View->IsViewColorImage)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void View::GetSize( int& width, int& height ) const
{
   (*API->View->GetViewDimensions)( handle, &width, &height );
}

// ----------------------------------------------------------------------------

void View::GetScreenTransferFunctions( stf_list& stf ) const
{
   stf.Clear();

   double m[ 4 ], c0[ 4 ], c1[ 4 ], r0[ 4 ], r1[ 4 ];

   if ( (*API->View->GetViewScreenTransferFunctions)( handle, m, c0, c1, r0, r1 ) == api_false )
      throw APIFunctionError( "GetScreenTransferFunctions" );

   for ( size_type i = 0; i < 4; ++i )
      stf.Add( HistogramTransformation( m[i], c0[i], c1[i], r0[i], r1[i] ) );
}

// ----------------------------------------------------------------------------

void View::SetScreenTransferFunctions( const stf_list& stf, bool notify )
{
   double m[ 4 ], c0[ 4 ], c1[ 4 ], r0[ 4 ], r1[ 4 ];

   for ( size_type i = 0; i < 4; ++i )
   {
      if ( i < stf.Length() )
      {
         const HistogramTransformation& H = stf[i];
         m[i]  = H.MidtonesBalance();
         c0[i] = H.ShadowsClipping();
         c1[i] = H.HighlightsClipping();
         r0[i] = H.LowRange();
         r1[i] = H.HighRange();
      }
      else
      {
         m[i]  = 0.5;
         c0[i] = r0[i] = 0;
         c1[i] = r1[i] = 1;
      }
   }

   if ( (*API->View->SetViewScreenTransferFunctions)( handle, m, c0, c1, r0, r1, notify ) == api_false )
      throw APIFunctionError( "SetViewScreenTransferFunctions" );
}

// ----------------------------------------------------------------------------

void View::DestroyScreenTransferFunctions( bool notify )
{
   if ( (*API->View->DestroyViewScreenTransferFunctions)( handle, notify ) == api_false )
      throw APIFunctionError( "DestroyScreenTransferFunctions" );
}

// ----------------------------------------------------------------------------

bool View::AreScreenTransferFunctionsEnabled() const
{
   return (*API->View->GetViewScreenTransferFunctionsEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void View::EnableScreenTransferFunctions( bool enable, bool notify )
{
   (*API->View->SetViewScreenTransferFunctionsEnabled)( handle, enable, notify );
}

// ----------------------------------------------------------------------------

bool View::IsReservedViewPropertyId( const IsoString& id )
{
   return (*API->View->IsReservedViewPropertyId)( id.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

static api_bool APIPropertyEnumerationCallback( const char* id, uint64 type, void* data )
{
   reinterpret_cast<PropertyDescriptionArray*>( data )->Append(
         PropertyDescription( id, VariantTypeFromAPIPropertyType( type ) ) );
   return api_true;
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray View::Properties() const
{
   PropertyDescriptionArray properties;
   IsoString id;
   size_type len = 0;
   (*API->View->EnumerateViewProperties)( handle, 0, 0, &len, 0 ); // 1st call to get max identifier length
   if ( len > 0 )
   {
      id.Reserve( len );
      if ( (*API->View->EnumerateViewProperties)( handle, APIPropertyEnumerationCallback,
                                                    id.Begin(), &len, &properties ) == api_false )
         throw APIFunctionError( "EnumerateViewProperties" );
   }
   return properties;
}

// ----------------------------------------------------------------------------

PropertyArray View::GetProperties() const
{
   PropertyDescriptionArray descriptions = Properties();
   PropertyArray properties;
   for ( const PropertyDescription& description : descriptions )
   {
      api_property_value value;
      if ( (*API->View->GetViewPropertyValue)( ModuleHandle(), handle, description.id.c_str(), &value ) == api_false )
         throw APIFunctionError( "GetViewPropertyValue" );
      properties << Property( description.id, VariantFromAPIPropertyValue( value ) );
   }
   return properties;
}

// ----------------------------------------------------------------------------

PropertyArray View::GetStorableProperties() const
{
   PropertyDescriptionArray descriptions = Properties();
   PropertyArray properties;
   for ( const PropertyDescription& description : descriptions )
   {
      uint32 flags = 0;
      if ( (*API->View->GetViewPropertyAttributes)( ModuleHandle(), handle, description.id.c_str(), &flags, 0/*type*/ ) == api_false )
         throw APIFunctionError( "GetViewPropertyAttributes" );
      if ( flags & ViewPropertyAttribute::Storable )
      {
         api_property_value value;
         if ( (*API->View->GetViewPropertyValue)( ModuleHandle(), handle, description.id.c_str(), &value ) == api_false )
            throw APIFunctionError( "GetViewPropertyValue" );
         properties << Property( description.id, VariantFromAPIPropertyValue( value ) );
      }
   }
   return properties;
}

// ----------------------------------------------------------------------------

void View::SetProperties( const PropertyArray& properties, bool notify, ViewPropertyAttributes attributes )
{
   for ( const Property& property : properties )
      if ( !IsReservedViewPropertyId( property.Id() ) )
      {
         api_property_value apiValue;
         APIPropertyValueFromVariant( apiValue, property.Value() );
         if ( (*API->View->SetViewPropertyValue)( ModuleHandle(), handle, property.Id().c_str(), &apiValue, attributes, notify ) == api_false )
            throw APIFunctionError( "SetViewPropertyValue" );
      }
}

// ----------------------------------------------------------------------------

Variant View::PropertyValue( const IsoString& property ) const
{
   if ( !HasProperty( property ) )
      return Variant();
   api_property_value value;
   if ( (*API->View->GetViewPropertyValue)( ModuleHandle(), handle, property.c_str(), &value ) == api_false )
      throw APIFunctionError( "GetViewPropertyValue" );
   return VariantFromAPIPropertyValue( value );
}

// ----------------------------------------------------------------------------

Variant View::ComputeProperty( const IsoString& property, bool notify )
{
   api_property_value value;
   if ( (*API->View->ComputeViewProperty)( ModuleHandle(), handle, property.c_str(), notify, &value ) == api_false )
      throw APIFunctionError( "ComputeViewProperty" );
   return VariantFromAPIPropertyValue( value );
}

// ----------------------------------------------------------------------------

void View::SetPropertyValue( const IsoString& property, const Variant& value, bool notify, ViewPropertyAttributes attributes )
{
   api_property_value apiValue;
   APIPropertyValueFromVariant( apiValue, value );
   if ( (*API->View->SetViewPropertyValue)( ModuleHandle(), handle, property.c_str(), &apiValue, attributes, notify ) == api_false )
      throw APIFunctionError( "SetViewPropertyValue" );
}

// ----------------------------------------------------------------------------

Variant::data_type View::PropertyType( const IsoString& property ) const
{
   if ( !HasProperty( property ) )
      return VariantType::Invalid;
   uint64 type = 0;
   if ( (*API->View->GetViewPropertyAttributes)( ModuleHandle(), handle, property.c_str(), 0/*flags*/, &type ) == api_false )
      throw APIFunctionError( "GetViewPropertyAttributes" );
   return VariantTypeFromAPIPropertyType( type );
}

// ----------------------------------------------------------------------------

ViewPropertyAttributes View::PropertyAttributes( const IsoString& property ) const
{
   uint32 flags = 0;
   if ( (*API->View->GetViewPropertyAttributes)( ModuleHandle(), handle, property.c_str(), &flags, 0/*type*/ ) == api_false )
      throw APIFunctionError( "GetViewPropertyAttributes" );
   return ViewPropertyAttributes( ViewPropertyAttribute::mask_type( flags ) );
}

// ----------------------------------------------------------------------------

void View::SetPropertyAttributes( const IsoString& property, ViewPropertyAttributes attributes, bool notify )
{
   if ( (*API->View->SetViewPropertyAttributes)( ModuleHandle(), handle, property.c_str(), unsigned( attributes ), notify ) == api_false )
      throw APIFunctionError( "SetViewPropertyAttributes" );
}

// ----------------------------------------------------------------------------

bool View::HasProperty( const IsoString& property ) const
{
   return (*API->View->GetViewPropertyExists)( ModuleHandle(), handle, property.c_str(), 0/*type*/ ) != api_false;
}

// ----------------------------------------------------------------------------

void View::DeleteProperty( const IsoString& property, bool notify )
{
   if ( (*API->View->DeleteViewProperty)( ModuleHandle(), handle, property.c_str(), notify ) == api_false )
      throw APIFunctionError( "DeleteViewProperty" );
}

// ----------------------------------------------------------------------------

View View::ViewById( const IsoString& fullId )
{
   return View( (*API->View->GetViewById)( fullId.c_str() ) );
}

// ----------------------------------------------------------------------------

class InternalViewEnumerator
{
public:

   static api_bool api_func Callback( view_handle hV, void* ptrToArray )
   {
      reinterpret_cast<Array<View>*>( ptrToArray )->Add( View( hV ) );
      return api_true;
   }
};

Array<View> View::AllViews( bool excludePreviews )
{
   Array<View> a;
   (*API->View->EnumerateViews)( InternalViewEnumerator::Callback, &a, api_true, !excludePreviews );
   return a;
}

Array<View> View::AllPreviews()
{
   Array<View> a;
   (*API->View->EnumerateViews)( InternalViewEnumerator::Callback, &a, api_false, api_true );
   return a;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/View.cpp - Released 2017-07-09T18:07:16Z
