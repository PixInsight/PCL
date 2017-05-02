//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIDeviceControllerInstance.h - Released 2016/04/28 15:13:36 UTC
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

#ifndef __INDIDeviceControllerInstance_h
#define __INDIDeviceControllerInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "IINDIDeviceControllerInstance.h"
#include "INDIDeviceControllerParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIDeviceControllerInstance : public ProcessImplementation, public IINDIDeviceControllerInstance
{
public:

   typedef Array<INDIDeviceListItem>      DeviceListType;
   typedef Array<INDIPropertyListItem>    PropertyListType;
   typedef Array<INDINewPropertyListItem> NewPropertyListType;

   INDIDeviceControllerInstance( const MetaProcess* );
   INDIDeviceControllerInstance( const INDIDeviceControllerInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool CanExecuteGlobal( pcl::String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   virtual String CurrentServerMessage() const
   {
      return m_currentMessage;
   }

   virtual void SetCurrentServerMessage( const String& message )
   {
      m_currentMessage = message;
   }

   virtual ExclPropertyList getExclusivePropertyList()
   {
      return ExclPropertyList( m_mutex, o_properties );
   }

   virtual ExclDeviceList getExclusiveDeviceList()
   {
      return ExclDeviceList( m_mutex, o_devices );
   }

   bool sendNewPropertyValue( const INDINewPropertyListItem& propItem, bool isAsynch = false );

   bool getINDIPropertyItem( String device, String property, String element, INDIPropertyListItem& result, bool formatted = true );

   void doInternalAbort()
   {
      m_internalAbortFlag = true;
   }

   bool getInternalAbortFlag() const
   {
      return m_internalAbortFlag;
   }

   void setInternalAbortFlag( bool doAbort )
   {
      m_internalAbortFlag=doAbort;
   }

   void clearNewPropertyList()
   {
      p_newProperties.Clear();
   }

   virtual String DownloadedImagePath() const
   {
      return m_downloadedImagePath;
   }

   virtual void SetDownloadedImagePath( const String& path )
   {
      m_downloadedImagePath = path;
   }

   bool HasDownloadedImage() const
   {
      return !m_downloadedImagePath.IsEmpty();
   }

   void ResetDownloadedImage()
   {
      m_downloadedImagePath.Clear();
   }

   int Verbosity() const
   {
      return p_verbosity;
   }

private:

   String                   p_serverHostName;
   uint32                   p_serverPort;
   pcl_bool                 p_serverConnect;
   String                   p_serverCommand;
   pcl_bool                 p_abort;
   int32                    p_verbosity;
   NewPropertyListType      p_newProperties;
   String                   p_getCommandParameters;

   mutable DeviceListType   o_devices;
   mutable PropertyListType o_properties;
   String                   o_getCommandResult;

   String                   m_currentMessage;
   bool                     m_internalAbortFlag;
   String                   m_downloadedImagePath;
   pcl::Mutex               m_mutex; // for access to propertyList

   void AcquireINDIClientOutputProperties() const;

   bool sendNewProperty( bool isAsyncCall = false );
   bool sendNewPropertyVector( const NewPropertyListType& propVector, bool isAsynch = false );
   bool getPropertyFromKeyString( INDINewPropertyListItem& newPropertyKey, const String& keyString );

   friend class INDIClient;
   friend class INDIDeviceControllerProcess;
   friend class INDIDeviceControllerInterface;
   friend class INDICCDFrameInstance;
   friend class INDICCDFrameInterface;
   friend class AbstractINDICCDFrameExecution;
   friend class INDIMountInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIDeviceControllerInstance_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInstance.h - Released 2016/04/28 15:13:36 UTC
