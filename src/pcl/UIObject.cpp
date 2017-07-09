//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/UIObject.cpp - Released 2017-07-09T18:07:16Z
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

#include <pcl/Array.h>
#include <pcl/AutoLock.h>
#include <pcl/UIObject.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

//#define __PCL_DEBUG_UIOBJECT_INDEX 1

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
#  include <iostream>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class UIEventDispatcher
{
public:

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
   static void api_func HandleDestroyed( api_handle );
#endif
};

// ----------------------------------------------------------------------------

class UIObjectIndex
{
public:

   struct IndexItem
   {
      api_handle  handle;
      size_type   count;

      IndexItem( UIObject* object ) : handle( object->handle ), count( 0 )
      {
         Add( object );
      }

      IndexItem( api_handle h, int ) : handle( h ), count( 0 )
      {
      }

      IndexItem( const IndexItem& ) = default;

      bool operator ==( const IndexItem& item ) const
      {
         return handle == item.handle;
      }

      bool operator <( const IndexItem& item ) const
      {
         return handle < item.handle;
      }

      bool IsEmpty() const
      {
         return count == 0;
      }

      void Add( UIObject* object )
      {
         if ( object == nullptr )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Add() : Null object pointer.\n"
                      << std::flush;
#endif
            return;
         }

         if ( object->handle != handle )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Add() : Invalid object (" << object->ObjectType() << ") "
                      << IsoString().Format( "at address %p with handle %p, expected %p\n", (void*)object, object->handle, handle )
                      << std::flush;
#endif
            return;
         }

         object->alias = count > 0;
         ++count;
      }

      void Remove( UIObject* object )
      {
         if ( object == nullptr )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Remove() : Null object pointer.\n"
                      << std::flush;
#endif
            return;
         }

         if ( object->handle != handle )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Remove() : Invalid object (" << object->ObjectType() << ") "
                      << IsoString().Format( "at address %p with handle %p, expected %p\n", (void*)object, object->handle, handle )
                      << std::flush;
#endif
            return;
         }

         object->handle = nullptr;
         object->alias = false;

         if ( count == 0 )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Remove() : Empty index item.\n"
                      << std::flush;
#endif
            return;
         }

         --count;
      }
   };

   typedef SortedArray<IndexItem>               index_implementation;
   typedef index_implementation::iterator       iterator;
   typedef index_implementation::const_iterator const_iterator;

   UIObjectIndex() = default;

   void Add( UIObject* object )
   {
      if ( object == nullptr )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIObjectIndex::Add() : Null object pointer.\n"
                   << std::flush;
#endif
         return;
      }

      if ( object->IsNull() )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIObjectIndex::Add() : Invalid null object (" << object->ObjectType() << ") "
                   << IsoString().Format( "at address %p\n", (void*)object )
                   << std::flush;
#endif
         return;
      }

      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( object->handle, 0 ) );
         if ( i != m_index.End() )
            m_index.MutableIterator( i )->Add( object );
         else
            m_index.Add( IndexItem( object ) );
      }

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
      (*API->UI->SetHandleDestroyedEventRoutine)( object->handle, UIEventDispatcher::HandleDestroyed );
#endif
   }

   void Remove( UIObject* object )
   {
      if ( object == nullptr )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIObjectIndex::Remove() : Null object pointer.\n"
                   << std::flush;
#endif
         return;
      }

      if ( object->IsNull() )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIObjectIndex::Remove() : Invalid null object (" << object->ObjectType() << ") "
                   << IsoString().Format( "at address %p\n", (void*)object )
                   << std::flush;
#endif
         return;
      }

      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( object->handle, 0 ) );
         if ( i != m_index.End() )
         {
            iterator m = m_index.MutableIterator( i );
            m->Remove( object );
            if ( m->IsEmpty() )
               m_index.Remove( m );
         }
      }
   }

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
   void HandleDestroyed( api_handle handle )
   {
      if ( handle == nullptr )
      {
         std::cerr << "\n** Warning: UIObjectIndex::HandleDestroyed() : Invalid zero handle.\n" << std::flush;
         return;
      }

      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( handle, 0 ) );
         if ( i != m_index.End() )
         {
            std::cerr << "\n** Warning: UIObjectIndex::HandleDestroyed() : "
                      << IsoString().Format( "handle %p invalidates %u existing object(s)\n", handle, i->count )
                      << std::flush;
            m_index.Remove( m_index.MutableIterator( i ) );
         }
      }
   }
#endif

private:

           index_implementation m_index;
   mutable Mutex                m_mutex;
};

static UIObjectIndex s_objects;

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
void api_func UIEventDispatcher::HandleDestroyed( api_handle handle )
{
   s_objects.HandleDestroyed( handle );
}
#endif

// ----------------------------------------------------------------------------

UIObject::UIObject( void* h ) : handle( h )
{
   if ( handle != nullptr )
      s_objects.Add( this );
}

UIObject::UIObject( const void* h ) : handle( const_cast<void*>( h ) )
{
   if ( handle != nullptr )
      s_objects.Add( this );
}

// ----------------------------------------------------------------------------

UIObject::UIObject( const UIObject& x ) : handle( x.handle )
{
   if ( handle != nullptr )
   {
      if ( (*API->UI->AttachToUIObject)( ModuleHandle(), handle ) == api_false )
      {
         handle = nullptr;
         throw APIFunctionError( "AttachToUIObject" );
      }

      s_objects.Add( this );
   }
}

// ----------------------------------------------------------------------------

UIObject::~UIObject() noexcept( false )
{
   if ( handle != nullptr )
   {
      api_handle theHandle = handle;

      s_objects.Remove( this );

      if ( (*API->UI->GetUIObjectRefCount)( theHandle ) > 0 )
         if ( (*API->UI->DetachFromUIObject)( ModuleHandle(), theHandle ) == api_false )
            throw APIFunctionError( "DetachFromUIObject" );
   }
}

// ----------------------------------------------------------------------------

UIObject& UIObject::Null()
{
   static UIObject* nullUIObject = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullUIObject == nullptr )
      nullUIObject = new UIObject( nullptr );
   return *nullUIObject;
}

// ----------------------------------------------------------------------------

size_type UIObject::RefCount() const
{
   return (handle != nullptr) ? (*API->UI->GetUIObjectRefCount)( handle ) : size_type( 0 );
}

// ----------------------------------------------------------------------------

void UIObject::EnsureUnique()
{
   if ( handle != nullptr )
      if ( !IsUnique() )
         SetHandle( CloneHandle() );
}

// ----------------------------------------------------------------------------

IsoString UIObject::ObjectType() const
{
   if ( handle == nullptr )
      return "Null";

   size_type len = 0;
   (*API->UI->GetUIObjectType)( handle, nullptr, &len );
   if ( len == 0 )
      throw APIFunctionError( "GetUIObjectType" );

   IsoString objType;
   objType.SetLength( len );
   if ( (*API->UI->GetUIObjectType)( handle, objType.Begin(), &len ) == api_false )
      throw APIFunctionError( "GetUIObjectType" );
   objType.ResizeToNullTerminated();
   return objType;
}

// ----------------------------------------------------------------------------

String UIObject::ObjectId() const
{
   if ( handle == nullptr )
      return String();

   size_type len = 0;
   (*API->UI->GetUIObjectId)( handle, nullptr, &len );

   String objId;
   if ( len > 0 )
   {
      objId.SetLength( len );
      if ( (*API->UI->GetUIObjectId)( handle, objId.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetUIObjectId" );
      objId.ResizeToNullTerminated();
   }
   return objId;
}

// ----------------------------------------------------------------------------

void UIObject::SetObjectId( const String& id )
{
   if ( (*API->UI->SetUIObjectId)( handle, id.c_str() ) == api_false )
      throw APIFunctionError( "SetUIObjectId" );
}

// ----------------------------------------------------------------------------

void UIObject::SetHandle( void* newHandle )
{
   if ( newHandle != handle )
   {
      if ( handle != nullptr )
      {
         api_handle oldHandle = handle;

         s_objects.Remove( this );

         if ( (*API->UI->GetUIObjectRefCount)( oldHandle ) > 0 )
            if ( (*API->UI->DetachFromUIObject)( ModuleHandle(), oldHandle ) == api_false )
               throw APIFunctionError( "DetachFromUIObject" );
      }

      if ( newHandle != nullptr )
      {
         if ( (*API->UI->AttachToUIObject)( ModuleHandle(), newHandle ) == api_false )
            throw APIFunctionError( "AttachToUIObject" );

         handle = newHandle;
         s_objects.Add( this );
      }
   }
}

// ----------------------------------------------------------------------------

void UIObject::TransferHandle( void* newHandle )
{
   if ( newHandle != handle )
   {
      if ( handle != nullptr )
      {
         api_handle oldHandle = handle;

         s_objects.Remove( this );

         if ( (*API->UI->GetUIObjectRefCount)( oldHandle ) > 0 )
            if ( (*API->UI->DetachFromUIObject)( ModuleHandle(), oldHandle ) == api_false )
               throw APIFunctionError( "DetachFromUIObject" );
      }

      if ( newHandle != nullptr )
      {
         handle = newHandle;
         s_objects.Add( this );
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/UIObject.cpp - Released 2017-07-09T18:07:16Z
