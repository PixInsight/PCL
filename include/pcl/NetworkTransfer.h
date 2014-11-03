// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/NetworkTransfer.h - Released 2014/10/29 07:34:07 UTC
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

#ifndef __PCL_NetworkTransfer_h
#define __PCL_NetworkTransfer_h

/// \file pcl/NetworkTransfer.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_Control_h
#include <pcl/Control.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class NetworkTransfer
 * \brief Synchronous network transfers.
 *
 * %NetworkTransfer is a simple network utility class with the following
 * capabilities:
 *
 * \li Synchronous download and upload network data transfers.
 *
 * \li HTTP and FTP protocols fully supported.
 *
 * \li Supports user authentication.
 *
 * \li Event-driven operation with separate download, upload and progress event
 * handlers.
 *
 * \li Transparent reusable connections for maximum performance.
 *
 * \li Thread-safe class.
 */
class PCL_CLASS NetworkTransfer : public UIObject
{
public:

   /*!
    * Constructs a %NetworkTransfer object.
    */
   NetworkTransfer();

   /*!
    * Destroys this client-side instance and dereferences the server-side
    * object. If the server-side object becomes unreferenced, it will be
    * garbage-collected and eventually destroyed by the core application.
    *
    * When the server-side object is destroyed, all active network connections
    * are also closed automatically. See the CloseConnection() member function
    * form more information.
    */
   virtual ~NetworkTransfer()
   {
   }

   /*!
    * Ensures that the server-side object managed by this instance is uniquely
    * referenced.
    *
    * Since network transfers are unique objects by definition, calling this
    * member function has no effect.
    */
   virtual void SetUnique()
   {
      // Unique by definition
   }

   /*!
    * Returns a reference to a null %NetworkTransfer instance. A null
    * %NetworkTransfer does not correspond to an existing network transfer in
    * the PixInsight core application.
    */
   static NetworkTransfer& Null();

   /*!
    * Sets the URL of this %NetworkTransfer object.
    *
    * \param url           A valid URL identifying the network resource of this
    *                      network transfer operation.
    *
    * \param userName      The user name for authentication. The default value
    *                      is an empty string.
    *
    * \param userPassword  The user password for authentication. The default
    *                      value is an empty string.
    *
    * URLs must be valid and conform to the URI specification from RFC 3986
    * (Uniform Resource Identifier: Generic Syntax). The HTTP, FTP, HTTPS,
    * FTPS, SSH and SMTP protocols are supported.
    *
    * The \a url may include user authentication data in the 'user:password@'
    * format. It may also include a port specification. For example:
    *
    * <tt>ftp://somebody:secretpwd@ftp.somewhere.com/mydir/myfile.txt:21</tt>
    *
    * However, if explicit user name and password are specified, they will
    * override any user authentication data included in the URL. In all cases,
    * user authentication data will never be shown on the console.
    *
    * If the specified URL is not syntactically valid, this function throws an
    * Error exception. Existence or responsiveness of the server are not
    * verified.
    */
   void SetURL( const String& url, const String& userName = String(), const String& userPassword = String() );

   /*!
    * Sets the proxy URL of this %NetworkTransfer object.
    *
    * \param url           A valid URL identifying the proxy server used by
    *                      this network transfer operation.
    *
    * \param userName      The user name for proxy authentication. The default
    *                      value is an empty string.
    *
    * \param userPassword  The user password for proxy authentication. The
    *                      default value is an empty string.
    *
    * The parameters used by this member function are identical to those of the
    * SetURL() function.
    *
    * If the specified URL is not syntactically valid, this function throws an
    * Error exception. Existence or responsiveness of the proxy server are not
    * verified.
    */
   void SetProxyURL( const String& url, const String& userName = String(), const String& userPassword = String() );

   /*!
    * Enable SSL/TLS for the FTP and SMTP protocols.
    *
    * \param useSSL     Enable SSL/TLS. The default value is true.
    *
    * \param forceSSL   If true, SSL will be required for communication during
    *                   the whole transfer operation; in this case, if SSL is
    *                   not available the connection will fail. If this
    *                   parameter is false, SSL will be used if available, but
    *                   the operation will continue (insecurely) otherwise. The
    *                   default value is true.
    *
    * \param verifyPeer Verify the authenticity of the peer's certificate, and
    *                   fail if it is not authentic. The peer's certificate is
    *                   checked through a chain of certification authority (CA)
    *                   cerfificates supplied by the calling machine. If this
    *                   parameter is false, the connection won't fail if the
    *                   authenticity of the server's certificate cannot be
    *                   verified. The default value is true.
    *
    * \param verifyHost Verify the identity of the host. The connection will
    *                   fail if the Common Name or Subject Alternate Name
    *                   fields of the server's certificate don't match the host
    *                   name in the URL set via the SetURL() member function.
    *                   If this parameter is false, the connection won't fail
    *                   if the identity of the server cannot be verified. If
    *                   the \a verifyPeer parameter is false, this parameter is
    *                   ignored. The default value is true.
    *
    * If the specified SSL settings are not valid for some reason, or if an
    * internal error occurs, this function throws an Error exception.
    */
   void SetSSL( bool useSSL = true, bool forceSSL = true, bool verifyPeer = true, bool verifyHost = true );

   /*!
    * Sets the connection timeout for this %NetworkTransfer object.
    *
    * \param seconds    Connection timeout in seconds. Can legally be zero to
    *                   reset the connection timeout to its default value of 30
    *                   seconds.
    *
    * If the specified timeout is not valid for some reason, or if an internal
    * error occurs, this function throws an Error exception.
    */
   void SetConnectionTimeout( int seconds );

   /*!
    * Performs a data download operation.
    *
    * Returns true upon successful completion of the download operation.
    * Returns false in the event of error, or if the operation was aborted.
    *
    * \note Before invoking this function, a download event handler must be set
    * by calling OnDownloadDataAvailable().
    */
   bool Download();

   /*!
    * Performs a data upload operation.
    *
    * \param uploadSize The total size in bytes of the data that will be
    *                   transferred. For example, if you are going to upload a
    *                   disk file, you should specify the file size here. If
    *                   the size cannot be known in advance, this argument can
    *                   legally be zero. However, specifying the total upload
    *                   size can improve uploading performance significantly.
    *                   The default value is zero.
    *
    * Returns true upon successful completion of the upload operation. Returns
    * false in the event of error, or if the operation was aborted.
    *
    * \note Before invoking this function, an upload event handler must be set
    * by calling OnUploadDataRequested().
    */
   bool Upload( fsize_type uploadSize = 0 );

   /*!
    * Performs an HTTP POST operation.
    *
    * Example:
    *
    * \code
    * NetworkTransfer transfer;
    * transfer.SetURL( "hhtp://example-cars-info-site.com/" );
    * transfer.OnDownloadDataAvailable( GetCarInfo, receiver );
    * if ( !transfer.POST( "brand=Toyota&model=RAV4" ) )
    *    throw Error( "POST operation failed: " + transfer.ErrorInformation() );
    * \endcode
    *
    * Returns true upon successful completion of the POST operation. Returns
    * false in the event of error, or if the operation has been aborted.
    *
    * This member function performs a download operation. The downloaded data
    * will be the server response to the POSTed \a fields.
    *
    * \note Before invoking this function, a download event handler must be set
    * by calling OnDownloadDataAvailable().
    */
   bool POST( const String& fields );

   /*!
    * Performs an SMTP operation.
    *
    * \param mailFrom   The 'return-to' email address.
    *
    * \param mailRecipients   The list of recipients for this SMTP operation,
    *                   usually a list of email addresses that will receive the
    *                   transferred data as an email message.
    *
    * Returns true upon successful completion of the SMTP operation. Returns
    * false in the event of error, or if the operation has been aborted.
    *
    * This member function performs an upload operation. The uploaded data will
    * be the header and body of an email message. See RFC 5322 for detailed
    * information on the data format applicable.
    *
    * \note Before invoking this function, an upload event handler must be set
    * by calling OnUploadDataRequested().
    */
   bool SMTP( const String& mailFrom, const StringList& mailRecipients );

   /*!
    * Closes the internal network connection associated with this object.
    *
    * Normally, when you call one of the Download(), Upload(), POST() or SMTP()
    * member functions for the first time with a %NetworkTransfer object, the
    * PixInsight Core application does not close the network connection
    * automatically after completing the operation. The connection remains
    * active for some time, ready to be reused if the same %NetworkTransfer
    * object is used again to perform additional (and compatible) data
    * transfers. This greatly improves performance of network transactions.
    *
    * By calling this function you can force immediate close of the active
    * connection. If there is no active connection associated with this object,
    * this function is simply ignored. Note that the destructor of
    * %NetworkTransfer also closes any active connection automatically.
    */
   void CloseConnection();

   /*!
    * Returns the URL of the last network transfer operation performed with
    * this object, or an empty string if no transfer operation has been done.
    *
    * \note User authentication data (user name and password) will never be
    * included in the URL returned by this function.
    */
   String URL() const;

   /*!
    * Returns the URL of the proxy server used in the last network transfer
    * operation performed with this object, or an empty string if no proxy
    * server has been specified.
    *
    * \note Proxy user authentication data (user name and password) will never
    * be included in the URL returned by this function.
    */
   String ProxyURL() const;

   /*!
    * Returns true if the last network transfer operation was successful; false
    * in the event of error, if no transfer operation has been performed with
    * this object, or if the last operation was aborted.
    */
   bool PerformedOK() const;

   /*!
    * A synonym for PerformedOK()
    */
   operator bool() const
   {
      return PerformedOK();
   }

   /*!
    * Returns true if the last network transfer operation was aborted by an
    * event handler. Returns false if the operation was not aborted (whether
    * successful or not), or if no transfer operation has been performed with
    * this object.
    */
   bool WasAborted() const;

   /*!
    * Returns the server response code after the last call to Perform().
    * Returns zero if the response code could not be retrieved, or if no
    * transfer operation has been done.
    */
   int ResponseCode() const;

   /*!
    * The content type (or MIME type) reported by the remote server after a
    * successful download operation, or an empty string if no valid content
    * type could be retrieved, if the last operation was an upload, or if no
    * transfer operation has been performed with this object.
    */
   String ContentType() const;

   /*!
    * Returns the total number of bytes transferred in the last network
    * operation performed with this object (whether successful or not).
    */
   fsize_type BytesTransferred() const;

   /*!
    * Returns the transfer speed in KiB per second, measured for the last
    * network transfer operation performed with this object.
    */
   double TotalSpeed() const;

   /*!
    * Returns the total elapsed time in seconds, measured for the last
    * network transfer operation performed with this object.
    */
   double TotalTime() const;

   /*!
    * Returns the error information generated in the last network transfer
    * operation. Returns an empty string if no error occurred, or if no
    * transfer operation has been performed with this object.
    *
    * Note that this function may return some useful diagnostics information,
    * even if the last operation was successful.
    */
   String ErrorInformation() const;

   /*!
    * \defgroup network_transfer_event_handlers NetworkTransfer Event Handlers
    */

   /*!
    * Defines the prototype of a <em>download event handler</em>.
    *
    * A download event is generated when a %NetworkTransfer instance receives
    * data from a remote location, which is ready to be processed by the
    * calling process.
    *
    * \param sender  The object that sends a download event.
    *
    * \param buffer  The starting address of a contiguous block of data that
    *                has been downloaded. The handler function must not modify
    *                these data in any way.
    *
    * \param size    The length in bytes of the \a buffer data block.
    *
    * The handler returns true to continue the download operation. If it
    * returns false, the operation will be aborted.
    *
    * \ingroup network_transfer_event_handlers
    */
   typedef bool (Control::*download_event_handler)( NetworkTransfer& sender, const void* buffer, fsize_type size );

   /*!
    * Defines the prototype of an <em>upload event handler</em>.
    *
    * An upload event is generated when a %NetworkTransfer instance is ready to
    * send new data to a remote location, which must be provided by the calling
    * process.
    *
    * \param sender  The object that sends an upload event.
    *
    * \param buffer  The starting address of a contiguous memory block where
    *                the data to be uploaded must be copied by the handler
    *                function.
    *
    * \param maxSize The maximum length in bytes of the \a buffer data block.
    *                The handler function must \e not store more bytes than the
    *                value passed in this argument.
    *
    * The handler returns an integer value, which will be interpreted as
    * follows:
    *
    * \li An integer greater than zero is the actual size in bytes of the
    * contiguous block that has been copied back to the passed \a buffer. In
    * this case the returned value must be less than or equal to \a maxSize.
    *
    * \li Zero to finalize the upload operation.
    *
    * \li A negative integer to abort the upload operation.
    *
    * \ingroup network_transfer_event_handlers
    */
   typedef fsize_type (Control::*upload_event_handler)( NetworkTransfer& sender, void* buffer, fsize_type maxSize );

   /*!
    * Defines the prototype of a <em>progress event handler</em>.
    *
    * A %NetworkTransfer instance generates progress events at regular
    * intervals during an active data transfer operation. Handling these events
    * can be useful to provide feedback to the user during long download or
    * upload procedures.
    *
    * \param sender           The object that sends a progress event.
    *
    * \param downloadTotal    Total download size in bytes.
    *
    * \param downloadCurrent  Size of currently downloaded data in bytes.
    *
    * \param uploadTotal      Total upload size in bytes.
    *
    * \param uploadCurrent    Size of currently uploaded data in bytes.
    *
    * The handler returns true to continue the ongoing operation. If it returns
    * false, the operation will be aborted.
    *
    * \ingroup network_transfer_event_handlers
    */
   typedef bool (Control::*progress_event_handler)( NetworkTransfer& sender,
                                    fsize_type downloadTotal, fsize_type downloadCurrent,
                                    fsize_type uploadTotal, fsize_type uploadCurrent );

   /*!
    * Sets the handler for <em>download data available</em> events generated by
    * this %NetworkTransfer object. These events are generated when the object
    * receives data from a remote location, which is ready to be processed.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive events from this object.
    */
   void OnDownloadDataAvailable( download_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>upload data requested</em> events generated by
    * this %NetworkTransfer object. These events are generated when the object
    * is ready to send new data to a remote location.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive events from this object.
    */
   void OnUploadDataRequested( upload_event_handler handler, Control& receiver );

   /*!
    * Sets the handler for <em>transfer progress</em> events generated by this
    * %NetworkTransfer object. These events are generated at regular intervals
    * during an active data transfer operation.
    *
    * \param handler    The event handler. Must be a member function of the
    *                   \a receiver object's class.
    *
    * \param receiver   The control that will receive events from this object.
    */
   void OnTransferProgress( progress_event_handler handler, Control& receiver );

private:

   download_event_handler onDownloadDataAvailable;
   upload_event_handler   onUploadDataRequested;
   progress_event_handler onTransferProgress;

   NetworkTransfer( void* );
   virtual void* CloneHandle() const;

   NetworkTransfer( const NetworkTransfer& ) { PCL_CHECK( 0 ) }
   void operator =( const NetworkTransfer& ) { PCL_CHECK( 0 ) }

   friend class NetworkTransferEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_NetworkTransfer_h

// ****************************************************************************
// EOF pcl/NetworkTransfer.h - Released 2014/10/29 07:34:07 UTC
