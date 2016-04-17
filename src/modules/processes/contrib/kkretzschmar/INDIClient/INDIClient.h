//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.04.0108
// ----------------------------------------------------------------------------
// INDIClient.h - Released 2016/04/15 15:37:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#ifndef __INDIClient_h
#define __INDIClient_h

#include "IINDIProperty.h"
#include "INDIDeviceControllerInstance.h"
#include "INDIDeviceControllerInterface.h"

#include "INDI/BaseClientImpl.h"
#include "INDI/basedevice.h"
#include "INDI/indicom.h"
#include "INDI/indidevapi.h"

#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PropertyNode;

template<typename T>
class ArrayOperator
{
public:

   virtual void run( Array<T>& _array, const T& _element ) const
   {
   }
};

template<typename T>
class ArrayAppend : public ArrayOperator<T>
{
public:

   virtual void run( Array<T>& _array, const T& _element ) const
   {
      _array.Append( _element );
   }
};

template<typename T>
class ArrayDelete : public ArrayOperator<T>
{
public:

   virtual void run( Array<T>& _array, const T& _element ) const
   {
      typename pcl::Array<T>::iterator iter = _array.Search( _element );
      if ( iter != _array.End() )
         _array.Remove( iter );
   }
};

template<typename T>
class ArrayUpdate : public ArrayOperator<T>
{
public:

   virtual void run( Array<T>& _array, const T& _element ) const
   {
      typename pcl::Array<T>::iterator iter = _array.Search( _element );
      if ( iter != _array.End() )
         *iter = _element;
   }
};

class INDIClient : public INDI::BaseClientImpl
{
public:

   INDIClient( IINDIDeviceControllerInstance* instance ) :
      BaseClientImpl(),
      m_instance( instance ),
      m_scriptInstance( nullptr )
   {
      if ( m_instance == nullptr )
         throw Error( "*** Internal error: INDIClient: Invalid instance pointer." );
   }

   INDIClient( IINDIDeviceControllerInstance* instance, const char* hostname, unsigned int port ) :
      BaseClientImpl( hostname, port ),
      m_instance( instance ),
      m_scriptInstance( nullptr )
   {
      if ( m_instance == nullptr )
         throw Error( "*** Internal error: INDIClient: Invalid instance pointer." );
   }

   ~INDIClient()
   {
   }

   IINDIDeviceControllerInstance* getDeviceControllerInstance()
   {
	   return m_instance;
   }


   void registerScriptInstance( IINDIDeviceControllerInstance* scriptInstance )
   {
      m_scriptInstance = scriptInstance;
   }

protected:

   void newDevice( INDI::BaseDevice* );
   void deleteDevice( INDI::BaseDevice* );
   void newProperty( INDI::Property* );
   void removeProperty( INDI::Property* );
   void newBLOB( IBLOB* );
   void newSwitch( ISwitchVectorProperty* );
   void newNumber( INumberVectorProperty* );
   void newMessage( INDI::BaseDevice*, int messageID );
   void newText( ITextVectorProperty* );
   void newLight( ILightVectorProperty* );


private:

   IINDIDeviceControllerInstance* m_instance;
   IINDIDeviceControllerInstance* m_scriptInstance;

   void runOnPropertyTable( IProperty*, const ArrayOperator<INDIPropertyListItem>*, PropertyFlagType = Idle );
};

extern AutoPointer<INDIClient> indiClient;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIClient_h

// ----------------------------------------------------------------------------
// EOF INDIClient.h - Released 2016/04/15 15:37:39 UTC
