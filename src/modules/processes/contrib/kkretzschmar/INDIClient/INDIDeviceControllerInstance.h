//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// INDIDeviceControllerInstance.h - Released 2015/10/13 15:55:45 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2015 Klaus Kretzschmar
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

#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>
#include <pcl/Timer.h>
#include <pcl/Console.h>
#include "INDIParameters.h"
#include "IINDIDeviceControllerInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------



class INDIDeviceControllerInstance : public ProcessImplementation, IINDIDeviceControllerInstance
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

   virtual Array<INDIPropertyListItem>& getPropertyList() {return p_propertyList;}
   virtual Array<INDIDeviceListItem>& getDeviceList(){return p_deviceList; }
   virtual IsoString& getCurrentMessage() {return p_currentMessage;}

   ExclPropertyList getExclusivePropertyList() {return ExclPropertyList(m_mutex,p_propertyList);}

   bool sendNewPropertyValue(const INDINewPropertyListItem& propItem,bool isAsynch=false);

   bool getINDIPropertyItem(String device, String property, String element, INDIPropertyListItem& result, bool formatted=true );

   void doInternalAbort() {m_internalAbortFlag=true;}
   bool getInternalAbortFlag() {return m_internalAbortFlag;}
   void setInternalAbortFlag(bool doAbort) {m_internalAbortFlag=doAbort;}
   void clearNewPropertyList(){p_newPropertyList.Clear();}
   virtual bool getImageDownloadedFlag(){return m_ImageDownloaded;}
   virtual void setImageDownloadedFlag(bool flag){m_ImageDownloaded=flag;}
private:
   DeviceListType          p_deviceList;
   PropertyListType        p_propertyList;
   NewPropertyListType     p_newPropertyList;
   String	               p_host;       // String hostname of INDI server
   uint32                  p_port;	     // uint32 port of INDI server
   uint32                  p_connect;	 // uint32 port of INDI server
   IsoString               p_currentMessage;
   String                  p_command;
   String                  p_getPropertyReturnValue;
   String                  p_getPropertyParam;
   pcl_bool				   p_doAbort;

   bool                    m_internalAbortFlag;
   bool                    m_ImageDownloaded;

   // Mutex for access to propertyList
   pcl::Mutex             m_mutex;


   void getProperties();
   bool sendNewProperty(bool isAsyncCall=false);
   bool sendNewPropertyVector(const NewPropertyListType& propVector,bool isAsynch=false);
   bool getPropertyFromKeyString(INDINewPropertyListItem& newPropertyKey, const String& keyString);
   void writeCurrentMessageToConsole();

   friend class INDIClient;


   friend class PixInsightINDIEngine;
   friend class INDIDeviceControllerProcess;
   friend class INDIDeviceControllerInterface;
   friend class CCDFrameInterface;
   friend class INDIMountInterface;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __INDIDeviceControllerInstance_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInstance.h - Released 2015/10/13 15:55:45 UTC
