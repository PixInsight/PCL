//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/WebView.h - Released 2017-07-18T16:13:52Z
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

#ifndef __PCL_WebView_h
#define __PCL_WebView_h

/// \file pcl/WebView.h

#include <pcl/Defs.h>

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/AutoPointer.h>
#include <pcl/Color.h>
#include <pcl/Control.h>
#include <pcl/Variant.h>

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class WebView
 * \brief Client-side interface to a PixInsight %WebView control.
 *
 * %WebView is a specialized control for rendering text and HTML content on the
 * PixInsight platform. The underlying implementation, which is based on Qt5's
 * QWebEngineView class, has full HTML 5.1 support and an advandced,
 * last-generation JavaScript engine.
 *
 * It is very important to point out that this class is \e not intended to
 * implement a web browser, even one with barebones functionality. The purpose
 * of %WebView is to render high-quality, interactive contents such as tables,
 * spreadsheets and graphics, where the contained data are dynamically
 * generated as HTML and JavaScript code.
 */
class PCL_CLASS WebView : public Control
{
public:

   /*!
    * Constructs a %WebView control.
    *
    * \param parent     The parent control of this object. The default value is
    *                   a null control.
    */
   WebView( Control& parent = Control::Null() );

   /*!
    * Destroys a %WebView control.
    */
   virtual ~WebView()
   {
   }

   /*!
    * Sets the content of this %WebView.
    *
    * \param data             The content as a sequence of bytes, stored as a
    *                         dynamic array of unsigned 8-bit integers.
    *
    * \param mimeType         A MIME type describing the type of the data. By
    *                         default, that is, if an empty string is
    *                         specified, the type will be detected based on
    *                         heuristics.
    *
    * The content loading process is asynchronous. This member function will
    * return immediately before the content has actually been loaded and
    * rendered in this %WebView. After calling this function, a sequence of
    * events will be generated:
    *
    * \li A load started event. Can be received by a handler set with
    * OnLoadStarted().
    *
    * \li A number of successive load progress events. Can be received by a
    * handler set with OnLoadProgress().
    *
    * \li A final load finished event. Can be received by a handler set with
    * OnLoadFinished(). This event handler in particular is necessary if the
    * caller depends on (or has to wait for) the content being rendered by this
    * %WebView.
    *
    * \sa SetHTML(), SetPlainText(), LoadContent()
    */
   void SetContent( const ByteArray& data, const IsoString& mimeType = IsoString() );

   /*!
    * Sets the content of this %WebView in HTML format, encoded as UTF-8. See
    * SetContent() for important information on %WebView's asynchronous content
    * rendering.
    */
   void SetHTML( const IsoString& html );

   /*!
    * Sets the content of this %WebView in HTML format, encoded as UTF-16. See
    * SetContent() for important information on %WebView's asynchronous content
    * rendering.
    */
   void SetHTML( const String& html );

   /*!
    * Sets the content of this %WebView as plain Unicode \a text encoded as
    * UTF-8. See SetContent() for important information on %WebView's
    * asynchronous content rendering.
    */
   void SetPlainText( const IsoString& text );

   /*!
    * Sets the content of this %WebView as plain Unicode \a text encoded as
    * UTF-16. See SetContent() for important information on %WebView's
    * asynchronous content rendering.
    */
   void SetPlainText( const String& text );

   /*!
    * Loads the content of this %WebView from a local or external resource.
    *
    * \param uri              A valid Uniform Resource Identifier, as defined
    *                         by RFC 3986 (Uniform Resource Identifier: Generic
    *                         Syntax).
    *
    * See SetContent() for important information on %WebView's asynchronous
    * content rendering, also applicable to this member function.
    *
    * \sa SetContent()
    */
   void LoadContent( const String& uri );

   /*!
    * Requests retrieval of the current content of this %WebView as plain
    * Unicode text encoded in UTF-16, with all HTML markup removed.
    *
    * This function is asynchronous. Once the requested content is available,
    * it will be sent to the OnPlainTextAvailable() event handler, if defined
    * for this object.
    */
   void RequestPlainText() const;

   /*!
    * Requests retrieval of the current content of this %WebView as HTML code
    * encoded in UTF-16.
    *
    * This function is asynchronous. Once the requested content is available,
    * it will be sent to the OnHTMLAvailable() event handler, if defined for
    * this object.
    */
   void RequestHTML() const;

   /*!
    * Writes the current content of this %WebView as a PDF document.
    *
    * \param filePath         Path to the output file.
    *
    * \param pageWidth        Page width in millimeters. The default width is
    *                         210 mm, corresponding to the standard A4 size.
    *
    * \param pageHeight       Page height in millimeters. The default height is
    *                         297 mm, corresponding to the standard A4 size.
    *
    * \param marginLeft       Left margin in millimeters. The default left
    *                         margin is 10 mm.
    *
    * \param marginTop        Top margin in millimeters. The default top margin
    *                         is 10 mm.
    *
    * \param marginRight      Right margin in millimeters. The default right
    *                         margin is 10 mm.
    *
    * \param marginBottom     Bottom margin in millimeters. The default bottom
    *                         margin is 10 mm.
    *
    * \param landscape        If true, the document will be generated with
    *                         landscape orientation. This means that the width
    *                         and height dimensions will be reversed. If false,
    *                         the normal portrait orientation will be applied.
    *                         This parameter is false by default.
    */
   void SaveAsPDF( const String& filePath, double pageWidth = 210, double pageHeight = 297,
                   double marginLeft = 10, double marginTop = 10, double marginRight = 10, double marginBottom = 10,
                   bool landscape = false );

   /*!
    * Returns true if there is a non-empty text selection in this %WebView.
    */
   bool HasSelection() const;

   /*!
    * Returns the currently selected text as plain text. Returns an empty
    * string if there is no text selection.
    */
   String SelectedText() const;

   /*!
    * Returns the zoom factor of this %WebView rendition. The returned value is
    * in the range [0.25,5.0].
    */
   double ZoomFactor() const;

   /*!
    * Sets the zoom factor of this %WebView rendition. The specified \a zoom
    * factor must be in the range [0.25,5.0].
    */
   void SetZoomFactor( double zoom );

   /*!
    * Returns the background color of the %WebView rendition encoded in 32-bit
    * RGBA format.
    */
   RGBA BackgroundColor() const;

   /*!
    * Sets the background color of the %WebView rendition. The specified
    * \a color must be encoded in 32-bit RGBA format.
    */
   void SetBackgroundColor( RGBA color );

   /*!
    * Reloads the content of this %WebView. If no content has previously been
    * loaded, calling this function has no effect.
    */
   void Reload();

   /*!
    * Stops loading the content of this %WebView. If there is no ongoing
    * content loading operation, calling this function has no effect.
    */
   void Stop();

   /*!
    * Executes a script in this %WebView.
    *
    * \param sourceCode    A string containing valid source code in the
    *                      specified \a language.
    *
    * \param language      The name of a supported scripting language.
    *                      Currently only the JavaScript language is supported
    *                      by this function. JavaScript is assumed if this
    *                      string is either empty or equal to "JavaScript".
    *
    * This function is asynchronous: It will return immediately without waiting
    * for the script to complete execution. Once the script has finished, its
    * result value will be sent to the OnScriptResultAvailable() event handler,
    * if defined for this object. The result value is the value of the last
    * executed expression statement in the script that is not in a function
    * definition. Note that for scripts whose value is not required, the
    * OnScriptResultAvailable event handler is not necessary.
    *
    * The script will be executed by a web-oriented JavaScript engine with full
    * support of the HTML 5.1 standard in current versions of PixInsight. Note
    * that this engine is completely different from the core JavaScript Runtime
    * (PJSR), so no PJSR resources are available. If you want to execute
    * JavaScript code in PJSR, use MetaModule::EvaluateScript() instead of this
    * member function.
    *
    * This function has no way to know if script execution failed, for example
    * as a result of syntax errors, invalid code, or exceptions thrown from
    * JavaScript code. A Variant will always be generated to transport the
    * result, but it may not transport a valid object (see Variant::IsValid())
    * if the script failed. How to diagnose script execution is completely up
    * to the caller implementation.
    *
    * \note This function can only be called from the root thread, since the
    * underlying JavaScript engine in not reentrant in current versions of
    * PixInsight. Calling this function from a running thread will throw an
    * Error exception.
    *
    * \warning You should make sure that your code has been well tested before
    * calling this function. The core PixInsight application will wait until
    * the script terminates execution. If your code enters an infinite loop, it
    * may lead to a crash of the PixInsight platform. Also bear in mind that
    * scripts are extremely powerful and potentially dangerous if you don't
    * know well what you are doing. What happens during execution of your
    * scripts is your entire responsibility.
    */
   void EvaluateScript( const String& sourceCode, const IsoString& language = IsoString() );

   /*!
    * \defgroup web_view_event_handlers WebView Event Handlers
    */

   /*!
    * Defines the prototype of a <em>web view event</em> handler.
    *
    * A web view event is generated when a %WebView instance changes its state
    * or contents, or the value of one of its properties, either
    * programmatically or because of user interaction.
    *
    * \param sender     The control that sends a web view event.
    *
    * \ingroup web_view_event_handlers
    */
   typedef void (Control::*view_event_handler)( WebView& sender );

   /*!
    * Defines the prototype of a <em>progress event</em> handler.
    *
    * A progress event is generated while the %WebView object is loading
    * contents, to inform on the amount of data already loaded.
    *
    * \param sender     The control that sends a progress event.
    *
    * \param loadPercent   The percentage of content loaded. Should be in the
    *                   range [0,100].
    *
    * \ingroup web_view_event_handlers
    */
   typedef void (Control::*progress_event_handler)( WebView& sender, int loadPercent );

   /*!
    * Defines the prototype of a <em>state event</em> handler.
    *
    * A state event is generated when the value of a Boolean %WebView property
    * has changed.
    *
    * \param sender     The control that sends a state event.
    *
    * \param state      The current value of the Boolean property.
    *
    * \ingroup web_view_event_handlers
    */
   typedef void (Control::*state_event_handler)( WebView& sender, bool state );

   /*!
    * Defines the prototype of a <em>content event</em> handler.
    *
    * A content event is generated after a call to RequestPlainText() or
    * RequestHTML(), when the requested content is available to the caller.
    *
    * \param sender     The control that sends a content event.
    *
    * \param content    The requested content (HTML or plain text) encoded in
    *                   UTF-16.
    *
    * \ingroup web_view_event_handlers
    */
   typedef void (Control::*content_event_handler)( WebView& sender, const String& content );

   /*!
    * Defines the prototype of a <em>result event</em> handler.
    *
    * A result event is generated after a call to EvaluateScript(), when the
    * script has finished execution.
    *
    * \param sender     The control that sends a result event.
    *
    * \param result     The result value of the executed script. The result
    *                   value is the value of the last executed expression
    *                   statement in the script that is not in a function
    *                   definition.
    *
    * \ingroup web_view_event_handlers
    */
   typedef void (Control::*result_event_handler)( WebView& sender, const Variant& result );

   /*!
    * Sets the handler for <em>load started</em> events generated by this
    * %WebView object. A load started event is generated when the %WebView
    * starts loading new content, after a call to SetContent().
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive load started events from
    *                   this %WebView.
    *
    * \ingroup web_view_event_handlers
    */
   void OnLoadStarted( view_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for progress events generated by this %WebView object. A
    * progress event is generated while the %WebView is loading new content,
    * after a call to SetContent(), to inform on the amount of data already
    * loaded.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive progress events from this
    *                   %WebView.
    *
    * \ingroup web_view_event_handlers
    */
   void OnLoadProgress( progress_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>load finished</em> events generated by this
    * %WebView object. A load finished event is generated when the %WebView
    * finishes loading new content, after a call to SetContent().
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class. The handler will receive a
    *                   Boolean value true if the loading process was completed
    *                   successfully, false in the event of error.
    *
    * \param receiver   The control that will receive load finished events from
    *                   this %WebView.
    *
    * \ingroup web_view_event_handlers
    */
   void OnLoadFinished( state_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>selection updated</em> events generated by this
    * %WebView object. A selection updated event is generated each time the
    * text selection is changed in this %WebView as a result of a direct user
    * interaction.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive selection updated events
    *                   from this %WebView.
    *
    * \ingroup web_view_event_handlers
    */
   void OnSelectionUpdated( view_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>plain text content available</em> events
    * generated by this %WebView object. These events are generated
    * asynchronously after a call to RequestPlainText(), when the requested
    * content is available to the caller.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive plain text content
    *                   available events from this %WebView.
    *
    * \ingroup web_view_event_handlers
    */
   void OnPlainTextAvailable( content_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>HTML content available</em> events generated by
    * this %WebView object. These events are generated asynchronously after a
    * call to RequestHTML(), when the requested content is available to the
    * caller.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive HTML content available
    *                   events from this %WebView.
    *
    * \ingroup web_view_event_handlers
    */
   void OnHTMLAvailable( content_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>script result available</em> events generated by
    * this %WebView object. These events are generated asynchronously after a
    * call to EvaluateScript(), when the script has finished execution and its
    * result value is available to the caller.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive script result available
    *                   events from this %WebView.
    *
    * \ingroup web_view_event_handlers
    */
   void OnScriptResultAvailable( result_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      view_event_handler     onLoadStarted = nullptr;
      progress_event_handler onLoadProgress = nullptr;
      state_event_handler    onLoadFinished = nullptr;
      view_event_handler     onSelectionUpdated = nullptr;
      content_event_handler  onPlainTextAvailable = nullptr;
      content_event_handler  onHTMLAvailable = nullptr;
      result_event_handler   onScriptResultAvailable = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   friend class WebViewEventDispatcher;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_WebView_h

// ----------------------------------------------------------------------------
// EOF pcl/WebView.h - Released 2017-07-18T16:13:52Z
