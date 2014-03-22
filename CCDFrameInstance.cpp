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

#include "CCDFrameInstance.h"


#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>
#include <pcl/Mutex.h>
#if defined(__PCL_LINUX)
#include <memory>
#endif
namespace pcl
{





// ----------------------------------------------------------------------------

CCDFrameInstance::CCDFrameInstance( const MetaProcess* m ) :
ProcessImplementation( m )
{
	
}

CCDFrameInstance::CCDFrameInstance( const CCDFrameInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void CCDFrameInstance::Assign( const ProcessImplementation& p )
{
   const CCDFrameInstance* x = dynamic_cast<const CCDFrameInstance*>( &p );
   if ( x != 0 )
   {
	  

   }
}


class CCDFrameEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const CCDFrameInstance& instance )
   {
      /*
       * Your magic comes here...
       */
      Console().WriteLn( "<end><cbr>Ah, did I mention that I do just _nothing_ at all? :D" );
   }
};


bool CCDFrameInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "INDI client can only be executed in the global context.";
   return false;
}

bool CCDFrameInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}





bool CCDFrameInstance::ExecuteGlobal()
{
   
   return true;
}



// ----------------------------------------------------------------------------


} // pcl

// ****************************************************************************
// EOF PixInsightINDIInstance.cpp - Released 2013/03/24 18:42:27 UTC
