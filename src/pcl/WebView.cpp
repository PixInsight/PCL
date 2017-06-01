//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/WebView.cpp - Released 2017-05-28T08:29:05Z
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

#include <pcl/MetaModule.h>
#include <pcl/WebView.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

WebView::WebView( Control& parent ) :
   Control( (*API->WebView->CreateWebView)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) )
{
   if ( handle == nullptr )
      throw APIFunctionError( "CreateWebView" );
}

// ----------------------------------------------------------------------------

void WebView::SetContent( const ByteArray& data, const IsoString& mimeType )
{
   if ( (*API->WebView->SetWebViewContent)( handle, data.Begin(), data.Size(), mimeType.Begin() ) == api_false )
      throw APIFunctionError( "SetWebViewContent" );
}

// ----------------------------------------------------------------------------

void WebView::SetHTML( const IsoString& html )
{
   if ( (*API->WebView->SetWebViewContent)( handle, html.Begin(), html.Size(), "text/html;charset=UTF-8" ) == api_false )
      throw APIFunctionError( "SetWebViewContent" );
}

// ----------------------------------------------------------------------------

void WebView::SetHTML( const String& html )
{
   if ( (*API->WebView->SetWebViewContent)( handle, html.Begin(), html.Size(), "text/html;charset=UTF-16" ) == api_false )
      throw APIFunctionError( "SetWebViewContent" );
}

// ----------------------------------------------------------------------------

void WebView::SetPlainText( const IsoString& text )
{
   if ( (*API->WebView->SetWebViewContent)( handle, text.Begin(), text.Size(), "text/plain;charset=UTF-8" ) == api_false )
      throw APIFunctionError( "SetWebViewContent" );
}

// ----------------------------------------------------------------------------

void WebView::SetPlainText( const String& text )
{
   if ( (*API->WebView->SetWebViewContent)( handle, text.Begin(), text.Size(), "text/plain;charset=UTF-16" ) == api_false )
      throw APIFunctionError( "SetWebViewContent" );
}

// ----------------------------------------------------------------------------

void WebView::LoadContent( const String& uri )
{
   if ( (*API->WebView->LoadWebViewContent)( handle, uri.c_str() ) == api_false )
      throw APIFunctionError( "LoadWebViewContent" );
}

// ----------------------------------------------------------------------------

void WebView::RequestPlainText() const
{
   if ( (*API->WebView->RequestWebViewPlainText)( handle ) == api_false )
      throw APIFunctionError( "RequestWebViewPlainText" );
}

// ----------------------------------------------------------------------------

void WebView::RequestHTML() const
{
   if ( (*API->WebView->RequestWebViewHTML)( handle ) == api_false )
      throw APIFunctionError( "RequestWebViewHTML" );
}

// ----------------------------------------------------------------------------

void WebView::SaveAsPDF( const String& filePath, double pageWidth, double pageHeight,
                         double marginLeft, double marginTop, double marginRight, double marginBottom,
                         bool landscape )
{
   if ( (*API->WebView->SaveWebViewAsPDF)( handle, filePath.c_str(), &pageWidth, &pageHeight,
                        &marginLeft, &marginTop, &marginRight, &marginBottom, landscape ? 1 : 0 ) == api_false )
      throw APIFunctionError( "SaveWebViewAsPDF" );
}

// ----------------------------------------------------------------------------

bool WebView::HasSelection() const
{
   return (*API->WebView->GetWebViewHasSelection)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String WebView::SelectedText() const
{
   size_type len = 0;
   (*API->WebView->GetWebViewSelectedText)( handle, nullptr, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->WebView->GetWebViewSelectedText)( handle, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetWebViewSelectedText" );
      text.ResizeToNullTerminated();
   }
   return text;

}

// ----------------------------------------------------------------------------

double WebView::ZoomFactor() const
{
   double zoom = 1.0;
   if ( (*API->WebView->GetWebViewZoomFactor)( handle, &zoom ) == api_false )
      throw APIFunctionError( "GetWebViewZoomFactor" );
   return zoom;
}

// ----------------------------------------------------------------------------

void WebView::SetZoomFactor( double zoom )
{
   if ( (*API->WebView->SetWebViewZoomFactor)( handle, &zoom ) == api_false )
      throw APIFunctionError( "SetWebViewZoomFactor" );
}

// ----------------------------------------------------------------------------

RGBA WebView::BackgroundColor() const
{
   return (*API->WebView->GetWebViewBackgroundColor)( handle );
}

// ----------------------------------------------------------------------------

void WebView::SetBackgroundColor( RGBA color )
{
   if ( (*API->WebView->SetWebViewBackgroundColor)( handle, color ) == api_false )
      throw APIFunctionError( "SetWebViewBackgroundColor" );
}

// ----------------------------------------------------------------------------

void WebView::Reload()
{
   if ( (*API->WebView->ReloadWebView)( handle ) == api_false )
      throw APIFunctionError( "ReloadWebView" );
}

// ----------------------------------------------------------------------------

void WebView::Stop()
{
   if ( (*API->WebView->StopWebView)( handle ) == api_false )
      throw APIFunctionError( "StopWebView" );
}

// ----------------------------------------------------------------------------

void WebView::EvaluateScript( const String& sourceCode, const IsoString& language )
{
   if ( (*API->WebView->EvaluateWebViewScript)( handle, sourceCode.c_str(), language.c_str() ) == api_false )
      throw APIFunctionError( "EvaluateWebViewScript" );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<WebView*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class WebViewEventDispatcher
{
public:

   static void LoadStarted( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onLoadStarted != nullptr )
         (receiver->*handlers->onLoadStarted)( *sender );
   }

   static void LoadProgress( control_handle hSender, control_handle hReceiver, int32 progress )
   {
      if ( handlers->onLoadProgress != nullptr )
         (receiver->*handlers->onLoadProgress)( *sender, progress );
   }

   static void LoadFinished( control_handle hSender, control_handle hReceiver, api_bool success )
   {
      if ( handlers->onLoadFinished != nullptr )
         (receiver->*handlers->onLoadFinished)( *sender, success != api_false );
   }

   static void SelectionUpdated( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onSelectionUpdated != nullptr )
         (receiver->*handlers->onSelectionUpdated)( *sender );
   }

   static void PlainTextAvailable( control_handle hSender, control_handle hReceiver, const char16_type* text )
   {
      if ( handlers->onPlainTextAvailable != nullptr )
         (receiver->*handlers->onPlainTextAvailable)( *sender, String( text ) );
   }

   static void HTMLAvailable( control_handle hSender, control_handle hReceiver, const char16_type* html )
   {
      if ( handlers->onHTMLAvailable != nullptr )
         (receiver->*handlers->onHTMLAvailable)( *sender, String( html ) );
   }

   static void ScriptResultAvailable( control_handle hSender, control_handle hReceiver, const api_property_value* result )
   {
      if ( handlers->onScriptResultAvailable != nullptr )
         (receiver->*handlers->onScriptResultAvailable)( *sender, VariantFromAPIPropertyValue( *result ) );
   }

}; // WebViewEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void WebView::OnLoadStarted( view_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->WebView->SetWebViewLoadStartedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? WebViewEventDispatcher::LoadStarted : nullptr ) == api_false )
      throw APIFunctionError( "SetWebViewLoadStartedEventRoutine" );
   m_handlers->onLoadStarted = handler;
}

void WebView::OnLoadProgress( progress_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->WebView->SetWebViewLoadProgressEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? WebViewEventDispatcher::LoadProgress : nullptr ) == api_false )
      throw APIFunctionError( "SetWebViewLoadProgressEventRoutine" );
   m_handlers->onLoadProgress = handler;
}

void WebView::OnLoadFinished( state_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->WebView->SetWebViewLoadFinishedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? WebViewEventDispatcher::LoadFinished : nullptr ) == api_false )
      throw APIFunctionError( "SetWebViewLoadFinishedEventRoutine" );
   m_handlers->onLoadFinished = handler;
}

void WebView::OnSelectionUpdated( view_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->WebView->SetWebViewSelectionUpdatedEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? WebViewEventDispatcher::SelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetWebViewSelectionUpdatedEventRoutine" );
   m_handlers->onSelectionUpdated = handler;
}

void WebView::OnPlainTextAvailable( content_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->WebView->SetWebViewPlainTextAvailableEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? WebViewEventDispatcher::PlainTextAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetWebViewPlainTextAvailableEventRoutine" );
   m_handlers->onPlainTextAvailable = handler;
}

void WebView::OnHTMLAvailable( content_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->WebView->SetWebViewHTMLAvailableEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? WebViewEventDispatcher::HTMLAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetWebViewHTMLAvailableEventRoutine" );
   m_handlers->onHTMLAvailable = handler;
}

void WebView::OnScriptResultAvailable( result_event_handler handler, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->WebView->SetWebViewScriptResultAvailableEventRoutine)( handle, &receiver,
                  (handler != nullptr) ? WebViewEventDispatcher::ScriptResultAvailable : nullptr ) == api_false )
      throw APIFunctionError( "SetWebViewScriptResultAvailableEventRoutine" );
   m_handlers->onScriptResultAvailable = handler;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/WebView.cpp - Released 2017-05-28T08:29:05Z
