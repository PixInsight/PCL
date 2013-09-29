// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInstance.h - Released 2013/03/24 18:42:27 UTC
// ****************************************************************************
// This file is part of the standard PixInsightINDI PixInsight module.
//
// Copyright (c) 2003-2013, Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PixInsightINDIInstance_h
#define __PixInsightINDIInstance_h

#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>
#include <pcl/Timer.h>
#include <pcl/Console.h>
#include "PixInsightINDIclient.h"
#include "PixInsightINDIParameters.h"
//#include "PixInsightINDIMediator.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class PixInsightINDIInstance : public ProcessImplementation
{
public:
   
   typedef Array<INDIPropertyListItem>    PropertyListType;
   typedef Array<INDINewPropertyListItem> NewPropertyListType;
   typedef Array<INDINewPropertyListItem> RemovePropertyListType;


   PixInsightINDIInstance( const MetaProcess* );
   PixInsightINDIInstance( const PixInsightINDIInstance& );
  
   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool CanExecuteGlobal( pcl::String& whyNot ) const;

   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;
   		
private:
   PropertyListType       p_propertyList;
   NewPropertyListType    p_newPropertyList;
   RemovePropertyListType p_PropertiesToBeRemoved;
   String	              p_host;       // String hostname of INDI server
   uint32                 p_port;	    // uint32 port of INDI server  
   uint32                 p_connect;	// uint32 port of INDI server
   IsoString              p_currentMessage;
   pcl_bool				  p_doAbort;
   
   void getProperties();
   void sendNewProperties();
   void sendNewProperty2();
   bool getPropertyFromKeyString(INDINewPropertyListItem& newPropertyKey, const String& keyString);
   void writeCurrentMessageToConsole(); 
   void removeNewPropertyListItems();

   friend class PixInsightINDIMediator;
   PixInsightINDIMediator*  p_Mediator;

   friend class PixInsightINDIEngine;
   friend class PixInsightINDIProcess;
   friend class PixInsightINDIInterface;
   
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __PixInsightINDIInstance_h

// ****************************************************************************
// EOF PixInsightINDIInstance.h - Released 2013/03/24 18:42:27 UTC
