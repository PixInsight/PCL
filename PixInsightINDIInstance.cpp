/// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInstance.cpp - Released 2013/03/24 18:42:27 UTC
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

#include "PixInsightINDIInstance.h"
#include "PixInsightINDIParameters.h"
#include "basedevice.h"
//#include "PixInsightINDIclient.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>
#if defined(__PCL_LINUX)
#include <memory>
#endif
namespace pcl
{


/* Our client auto pointer */
auto_ptr<INDIClient> indiClient(0);

// ----------------------------------------------------------------------------

PixInsightINDIInstance::PixInsightINDIInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
p_host( TheINDIServerHostname->DefaultValue() ),
p_port( uint32( TheINDIServerPort->DefaultValue() ) )
{
}


PixInsightINDIInstance::PixInsightINDIInstance( const PixInsightINDIInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void PixInsightINDIInstance::Assign( const ProcessImplementation& p )
{
   const PixInsightINDIInstance* x = dynamic_cast<const PixInsightINDIInstance*>( &p );
   if ( x != 0 )
   {
	  p_host  = x->p_host;
	  p_port  = x->p_port;
   }
}


class PixInsightINDIEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const PixInsightINDIInstance& instance )
   {
      /*
       * Your magic comes here...
       */
      Console().WriteLn( "<end><cbr>Ah, did I mention that I do just _nothing_ at all? :D" );
   }
};


bool PixInsightINDIInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "INDI client can only be executed in the global context.";
   return false;
}

bool PixInsightINDIInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}



bool PixInsightINDIInstance::ExecuteGlobal()
{
   if (indiClient.get() == 0)
        indiClient.reset(new INDIClient());
   
   IsoString ASCIIHost(p_host);
   indiClient->setServer(ASCIIHost.c_str() , p_port);
   
   bool connected=false;
	
   connected = indiClient->connectServer();
   
   if (connected)
     //Console().WriteLn("Successfully connected to server %s: %i",ASCIIHost.c_str(),p_port);

   Console().Flush();

   Sleep(1);

   vector<INDI::BaseDevice *> pDevices = indiClient->getDevices();
   for (std::vector<INDI::BaseDevice *>::iterator it = pDevices.begin(); it!=pDevices.end(); ++it  )
   {
     //Console().WriteLn("Detected device %s",(*it)->getDeviceName());
   }


   return true;
}

void* PixInsightINDIInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheINDIServerHostname )
      return p_host.c_str();
   if ( p == TheINDIServerPort )
      return &p_port;

   return 0;
}

bool PixInsightINDIInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   
	if ( p == TheINDIServerHostname )
	{
		p_host.Clear();
		if ( sizeOrLength > 0 )
			p_host.Reserve( sizeOrLength );
	}
	else
		return false;

   return true;
}

size_type PixInsightINDIInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheINDIServerHostname )
      return p_host.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF PixInsightINDIInstance.cpp - Released 2013/03/24 18:42:27 UTC
