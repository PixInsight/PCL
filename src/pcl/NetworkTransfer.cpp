//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/NetworkTransfer.cpp - Released 2016/02/21 20:22:19 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/AutoLock.h>
#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>
#include <pcl/NetworkTransfer.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<NetworkTransfer*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class NetworkTransferEventDispatcher
{
public:

   static api_bool DownloadDataAvailable( network_transfer_handle hSender, control_handle hReceiver, const void* buffer, fsize_type size )
   {
      if ( sender->onDownloadDataAvailable != 0 )
         return (receiver->*sender->onDownloadDataAvailable)( *sender, buffer, size );
      return api_false; // should never happen
   }

   static fsize_type UploadDataRequested( network_transfer_handle hSender, control_handle hReceiver, void* buffer, fsize_type maxSize )
   {
      if ( sender->onUploadDataRequested != 0 )
         return (receiver->*sender->onUploadDataRequested)( *sender, buffer, maxSize );
      return 0; // should never happen
   }

   static api_bool TransferProgress( network_transfer_handle hSender, control_handle hReceiver,
                                     fsize_type dlTotal, fsize_type dlCurrent, fsize_type ulTotal, fsize_type ulCurrent )
   {
      if ( sender->onTransferProgress != 0 )
         return (receiver->*sender->onTransferProgress)( *sender, dlTotal, dlCurrent, ulTotal, ulCurrent );
      return api_false; // should never happen
   }

}; // NetworkTransferEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

NetworkTransfer::NetworkTransfer() :
UIObject( (*API->NetworkTransfer->CreateNetworkTransfer)( ModuleHandle(), this ) ),
onDownloadDataAvailable( 0 ),
onUploadDataRequested( 0 ),
onTransferProgress( 0 )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateNetworkTransfer" );
}

// ----------------------------------------------------------------------------

NetworkTransfer::NetworkTransfer( void* h ) : UIObject( h ),
onDownloadDataAvailable( 0 ),
onUploadDataRequested( 0 ),
onTransferProgress( 0 )
{
}

// ----------------------------------------------------------------------------

NetworkTransfer& NetworkTransfer::Null()
{
   static NetworkTransfer* nullNetworkTransfer = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullNetworkTransfer == nullptr )
      nullNetworkTransfer = new NetworkTransfer( nullptr );
   return *nullNetworkTransfer;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetURL( const String& url, const String& userName, const String& userPassword )
{
   if ( (*API->NetworkTransfer->SetNetworkTransferURL)( handle, url.c_str(), userName.c_str(), userPassword.c_str() ) == api_false )
      throw APIFunctionError( "SetNetworkTransferURL" );
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetProxyURL( const String& url, const String& userName, const String& userPassword )
{
   if ( (*API->NetworkTransfer->SetNetworkTransferProxyURL)( handle, url.c_str(), userName.c_str(), userPassword.c_str() ) == api_false )
      throw APIFunctionError( "SetNetworkTransferProxyURL" );
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetSSL( bool useSSL, bool forceSSL, bool verifyPeer, bool verifyHost )
{
   if ( (*API->NetworkTransfer->SetNetworkTransferSSL)( handle, useSSL, forceSSL, verifyPeer, verifyHost ) == api_false )
      throw APIFunctionError( "SetNetworkTransferSSL" );
}

// ----------------------------------------------------------------------------

void NetworkTransfer::SetConnectionTimeout( int seconds )
{
   if ( (*API->NetworkTransfer->SetNetworkTransferConnectionTimeout)( handle, seconds ) == api_false )
      throw APIFunctionError( "SetNetworkTransferConnectionTimeout" );
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::Download()
{
   return (*API->NetworkTransfer->PerformNetworkTransferDownload)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::Upload( fsize_type uploadSize )
{
   return (*API->NetworkTransfer->PerformNetworkTransferUpload)( handle, uploadSize ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::POST( const String& fields )
{
   return (*API->NetworkTransfer->PerformNetworkTransferPOST)( handle, fields.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::SMTP( const String& mailFrom, const StringList& mailRecipients )
{
   String csRecipients;
   mailRecipients.ToCommaSeparated( csRecipients );
   return (*API->NetworkTransfer->PerformNetworkTransferSMTP)( handle, mailFrom.c_str(), csRecipients.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::CloseConnection()
{
   (*API->NetworkTransfer->CloseNetworkTransferConnection)( handle );
}

// ----------------------------------------------------------------------------

String NetworkTransfer::URL() const
{
   size_type len = 0;
   (*API->NetworkTransfer->GetNetworkTransferURL)( handle, 0, &len );

   String url;
   if ( len > 0 )
   {
      url.SetLength( len );
      if ( (*API->NetworkTransfer->GetNetworkTransferURL)( handle, url.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferURL" );
      url.ResizeToNullTerminated();
   }
   return url;
}

// ----------------------------------------------------------------------------

String NetworkTransfer::ProxyURL() const
{
   size_type len = 0;
   (*API->NetworkTransfer->GetNetworkTransferProxyURL)( handle, 0, &len );

   String url;
   if ( len > 0 )
   {
      url.SetLength( len );
      if ( (*API->NetworkTransfer->GetNetworkTransferProxyURL)( handle, url.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferProxyURL" );
      url.ResizeToNullTerminated();
   }
   return url;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::PerformedOK() const
{
   return (*API->NetworkTransfer->GetNetworkTransferStatus)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool NetworkTransfer::WasAborted() const
{
   return (*API->NetworkTransfer->GetNetworkTransferIsAborted)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

int NetworkTransfer::ResponseCode() const
{
   return (*API->NetworkTransfer->GetNetworkTransferResponseCode)( handle );
}

// ----------------------------------------------------------------------------

String NetworkTransfer::ContentType() const
{
   size_type len = 0;
   (*API->NetworkTransfer->GetNetworkTransferContentType)( handle, 0, &len );

   String contentType;
   if ( len > 0 )
   {
      contentType.SetLength( len );
      if ( (*API->NetworkTransfer->GetNetworkTransferContentType)( handle, contentType.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferContentType" );
      contentType.ResizeToNullTerminated();
   }
   return contentType;
}

// ----------------------------------------------------------------------------

fsize_type NetworkTransfer::BytesTransferred() const
{
   return (*API->NetworkTransfer->GetNetworkTransferBytesTransferred)( handle );
}

// ----------------------------------------------------------------------------

double NetworkTransfer::TotalSpeed() const
{
   double KiBPerSec = 0;
   (*API->NetworkTransfer->GetNetworkTransferTotalSpeed)( handle, &KiBPerSec );
   return KiBPerSec;
}

// ----------------------------------------------------------------------------

double NetworkTransfer::TotalTime() const
{
   double totalSecs = 0;
   (*API->NetworkTransfer->GetNetworkTransferTotalTime)( handle, &totalSecs );
   return totalSecs;
}

// ----------------------------------------------------------------------------

String NetworkTransfer::ErrorInformation() const
{
   size_type len = 0;
   (*API->NetworkTransfer->GetNetworkTransferErrorInformation)( handle, 0, &len );

   String errorInfo;
   if ( len > 0 )
   {
      errorInfo.SetLength( len );
      if ( (*API->NetworkTransfer->GetNetworkTransferErrorInformation)( handle, errorInfo.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetNetworkTransferErrorInformation" );
      errorInfo.ResizeToNullTerminated();
   }
   return errorInfo;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::OnDownloadDataAvailable( download_event_handler handler, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLERS;
   onDownloadDataAvailable = 0;
   if ( (*API->NetworkTransfer->SetNetworkTransferDownloadEventRoutine)( handle, &receiver,
                     (handler != 0) ? NetworkTransferEventDispatcher::DownloadDataAvailable : 0 ) == api_false )
      throw APIFunctionError( "SetNetworkTransferDownloadEventRoutine" );
   onDownloadDataAvailable = handler;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::OnUploadDataRequested( upload_event_handler handler, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLERS;
   onUploadDataRequested = 0;
   if ( (*API->NetworkTransfer->SetNetworkTransferUploadEventRoutine)( handle, &receiver,
                     (handler != 0) ? NetworkTransferEventDispatcher::UploadDataRequested : 0 ) == api_false )
      throw APIFunctionError( "SetNetworkTransferUploadEventRoutine" );
   onUploadDataRequested = handler;
}

// ----------------------------------------------------------------------------

void NetworkTransfer::OnTransferProgress( progress_event_handler handler, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLERS;
   onTransferProgress = 0;
   if ( (*API->NetworkTransfer->SetNetworkTransferProgressEventRoutine)( handle, &receiver,
                     (handler != 0) ? NetworkTransferEventDispatcher::TransferProgress : 0 ) == api_false )
      throw APIFunctionError( "SetNetworkTransferProgressEventRoutine" );
   onTransferProgress = handler;
}

// ----------------------------------------------------------------------------

void* NetworkTransfer::CloneHandle() const
{
   throw Error( "Cannot clone a NetworkTransfer handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/NetworkTransfer.cpp - Released 2016/02/21 20:22:19 UTC
