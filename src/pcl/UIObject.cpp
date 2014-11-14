// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/UIObject.cpp - Released 2014/11/14 17:17:00 UTC
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

class UIObjectIndex
{
public:

   struct IndexItem
   {
      api_handle       handle;
      Array<UIObject*> objects;

      IndexItem( UIObject* object ) : handle( object->handle ), objects()
      {
         Add( object );
      }

      IndexItem( api_handle h, int ) : handle( h ), objects()
      {
      }

      IndexItem( const IndexItem& item ) : handle( item.handle ), objects( item.objects )
      {
      }

      ~IndexItem()
      {
      }

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
         return objects.IsEmpty();
      }

      void Add( UIObject* object )
      {
         if ( object == 0 )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Add() : Null object pointer\n"
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

         if ( objects.Has( object ) )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Add() : Duplicate object (" << object->ObjectType() << ") "
                      << IsoString().Format( "at address %p with handle %p\n", (void*)object, handle )
                      << std::flush;
#endif
            return;
         }

         object->alias = !objects.IsEmpty();
         objects.Add( object );
      }

      void Remove( UIObject* object )
      {
         if ( object == 0 )
         {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
            std::cerr << "\n** Warning: UIObjectIndex::IndexItem::Remove() : Null object pointer\n"
                      << std::flush;
#endif
            return;
         }

         objects.Remove( object );
         object->handle = 0;
         object->alias = false;
      }
   };

   typedef SortedArray<IndexItem>               index_implementation;
   typedef index_implementation::iterator       iterator;
   typedef index_implementation::const_iterator const_iterator;

   UIObjectIndex() : m_index(), m_mutex()
   {
   }

   virtual ~UIObjectIndex()
   {
   }

   void Add( UIObject* object )
   {
      if ( object == 0 )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIObjectIndex::Add() : Null object pointer\n"
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
   }

   void Remove( UIObject* object )
   {
      if ( object == 0 )
      {
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         std::cerr << "\n** Warning: UIObjectIndex::Remove() : Null object pointer\n"
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
      if ( handle == 0 )
      {
         std::cerr << "\n** Warning: UIObjectIndex::HandleDestroyed() : Invalid zero handle\n" << std::flush;
         return;
      }

      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( handle, 0 ) );
         if ( i == m_index.End() )
            return;

         iterator m = m_index.MutableIterator( i );
         for ( Array<UIObject*>::iterator o = m->objects.Begin(); o != m->objects.End(); ++o )
            if ( *o != 0 ) // cannot happen
            {
               if ( (*o)->handle == handle )
                  (*o)->HandleDestroyed();
               else
                  std::cerr << "\n** Warning: UIObjectIndex::HandleDestroyed() : Invalid object (" << (*o)->ObjectType() << ") "
                            << IsoString().Format( "at address %p with handle %p, expected %p\n", (void*)*o, (*o)->handle, handle )
                            << std::flush;
            }

         m_index.Remove( m );
      }
   }
#endif

   UIObject* ObjectByHandle( api_handle handle )
   {
      if ( handle != 0 )
      {
         volatile AutoLock lock( m_mutex );
         const_iterator i = m_index.Search( IndexItem( handle, 0 ) );
         if ( i != m_index.End() )
            if ( !i->IsEmpty() )
            {
               iterator m = m_index.MutableIterator( i );
               if ( m->objects[0] != 0 )
                  if ( m->objects[0]->handle == handle )
                     return m->objects[0];
            }
      }
      return 0;
   }

private:

           index_implementation m_index;
   mutable Mutex                m_mutex;
};

static UIObjectIndex s_objects;

// ----------------------------------------------------------------------------

class UIEventDispatcher
{
public:

#ifdef __PCL_DEBUG_UIOBJECT_INDEX
   static void api_func HandleDestroyed( api_handle hUI )
   {
      s_objects.HandleDestroyed( hUI );
   }
#endif
};

// ----------------------------------------------------------------------------

UIObject::UIObject( void* h ) : handle( h ), alias( false )
{
   if ( handle != 0 )
   {
      s_objects.Add( this );
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
      (*API->UI->SetHandleDestroyedEventRoutine)( handle, UIEventDispatcher::HandleDestroyed );
#endif
   }
}

// ----------------------------------------------------------------------------

UIObject::UIObject( const UIObject& x ) : handle( x.handle ), alias( false )
{
   if ( handle != 0 )
   {
      if ( (*API->UI->AttachToUIObject)( ModuleHandle(), handle ) == api_false )
      {
         handle = 0;
         throw APIFunctionError( "AttachToUIObject" );
      }

      s_objects.Add( this );
   }
}

// ----------------------------------------------------------------------------

UIObject::~UIObject()
{
   if ( handle != 0 )
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
   static UIObject* nullUIObject = 0;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullUIObject == 0 )
      nullUIObject = new UIObject( reinterpret_cast<void*>( 0 ) );
   return *nullUIObject;
}

// ----------------------------------------------------------------------------

size_type UIObject::RefCount() const
{
   return (handle != 0) ? (*API->UI->GetUIObjectRefCount)( handle ) : size_type( 0 );
}

// ----------------------------------------------------------------------------

void UIObject::SetUnique()
{
   if ( handle != 0 )
      if ( !IsUnique() )
         SetHandle( CloneHandle() );
}

// ----------------------------------------------------------------------------

IsoString UIObject::ObjectType() const
{
   if ( handle == 0 )
      return "Null";

   size_type len = 0;
   (*API->UI->GetUIObjectType)( handle, 0, &len );
   if ( len == 0 )
      throw APIFunctionError( "GetUIObjectType" );

   IsoString objType;
   objType.Reserve( len );
   if ( (*API->UI->GetUIObjectType)( handle, objType.c_str(), &len ) == api_false )
      throw APIFunctionError( "GetUIObjectType" );

   return objType;
}

// ----------------------------------------------------------------------------

String UIObject::ObjectId() const
{
   String objId;

   if ( handle != 0 )
   {
      size_type len = 0;
      (*API->UI->GetUIObjectId)( handle, 0, &len );

      if ( len != 0 )
      {
         objId.Reserve( len );
         if ( (*API->UI->GetUIObjectId)( handle, objId.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetUIObjectId" );
      }
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

UIObject& UIObject::ObjectByHandle( void* handle )
{
   UIObject* object = s_objects.ObjectByHandle( handle );
   return object ? *object : Null();
}

// ----------------------------------------------------------------------------

void UIObject::SetHandle( void* newHandle )
{
   if ( newHandle != handle )
   {
      if ( handle != 0 )
      {
         api_handle oldHandle = handle;

         s_objects.Remove( this );

         if ( (*API->UI->GetUIObjectRefCount)( oldHandle ) > 0 )
            if ( (*API->UI->DetachFromUIObject)( ModuleHandle(), oldHandle ) == api_false )
               throw APIFunctionError( "DetachFromUIObject" );
      }

      if ( newHandle != 0 )
      {
         if ( (*API->UI->AttachToUIObject)( ModuleHandle(), newHandle ) == api_false )
            throw APIFunctionError( "AttachToUIObject" );

         handle = newHandle;
         s_objects.Add( this );
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         (*API->UI->SetHandleDestroyedEventRoutine)( handle, UIEventDispatcher::HandleDestroyed );
#endif
      }
   }
}

// ----------------------------------------------------------------------------

void UIObject::TransferHandle( void* newHandle )
{
   if ( newHandle != handle )
   {
      if ( handle != 0 )
      {
         api_handle oldHandle = handle;

         s_objects.Remove( this );

         if ( (*API->UI->GetUIObjectRefCount)( oldHandle ) > 0 )
            if ( (*API->UI->DetachFromUIObject)( ModuleHandle(), oldHandle ) == api_false )
               throw APIFunctionError( "DetachFromUIObject" );
      }

      if ( newHandle != 0 )
      {
         handle = newHandle;
         s_objects.Add( this );
#ifdef __PCL_DEBUG_UIOBJECT_INDEX
         (*API->UI->SetHandleDestroyedEventRoutine)( handle, UIEventDispatcher::HandleDestroyed );
#endif
      }
   }
}

// ----------------------------------------------------------------------------

void UIObject::HandleDestroyed()
{
   /*
    * This virtual function is called when a server handle becomes invalid.
    * (debug mode only)
    */
}

// ----------------------------------------------------------------------------

void* UIObject::CloneHandle() const
{
   /*
    * This virtual function is called to construct object duplicates.
    * Derived classes must reimplement this function.
    */
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/UIObject.cpp - Released 2014/11/14 17:17:00 UTC
