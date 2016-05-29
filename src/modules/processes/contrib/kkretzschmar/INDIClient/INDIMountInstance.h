//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIMountInstance.h - Released 2016/04/28 15:13:36 UTC
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

#ifndef ____INDIMountInstance_h
#define ____INDIMountInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>


namespace pcl
{

class INDIClient;
// ----------------------------------------------------------------------------

class INDIMountInstance : public ProcessImplementation
{
public:

   INDIMountInstance( const MetaProcess* );
   INDIMountInstance( const INDIMountInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool CanExecuteGlobal( String& whyNot ) const;

   virtual bool ExecuteGlobal();
   virtual bool ExecuteOn( View& view );

   virtual void* LockParameter( const MetaParameter* p, size_type tableRow );
   virtual bool  AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   bool ValidateDevice( bool throwErrors = true ) const;
   void SendDeviceProperties( bool asynchronous = true ) const;

   static String MountSlewRatePropertyString( int slewRateIdx );
private:

   String   p_deviceName;
   pcl_enum p_commandType;
   pcl_enum p_slewRate;
   double   p_targetRA;
   double   p_targetDEC;
   double   p_lst;
   double   p_currentRA;
   double   p_currentDEC;

   friend class INDIMountInterface;
   friend class AbstractINDIMountExecution;
};


class AbstractINDIMountExecution
{
public:

   AbstractINDIMountExecution( INDIMountInstance& instance ) :
      m_instance( instance ),
      m_running( false ),
      m_aborted( false )
   {
   }

   virtual ~AbstractINDIMountExecution()
   {
   }

   virtual void Perform();
   virtual void Abort();

   const INDIMountInstance& Instance() const
   {
      return m_instance;
   }

   bool IsRunning() const
   {
      return m_running;
   }

   bool WasAborted() const
   {
      return m_aborted;
   }

   void setCommandType(pcl_enum commandType)
   {
	   m_instance.p_commandType = commandType;
   }
protected:

   INDIMountInstance& m_instance;

   virtual void StartMountEvent(double targetRA, double currentRA, double targetDEC, double currentDEC, pcl_enum commandType) = 0;
   virtual void MountEvent(double targetRA, double currentRA, double targetDEC, double currentDEC) = 0;
   virtual void EndMountEvent() = 0;

   virtual void AbortEvent() = 0;


private:

   void GetCurrentCoordinates(const INDIClient* indi);

   bool m_running, m_aborted;

};

// helper class
class CoordUtils
{
public:
   struct HMS
   {
      double hour;
      double minute;
      double second;
      HMS( ):hour(0),minute(0),second(0) {}
      HMS(double h,double m,double s ):hour(h),minute(m),second(s) {}
   };

   static HMS convertToHMS( double coord );
   static HMS parse( String coordStr, String sep=":" );
   static double convertFromHMS( const HMS& coord );
   static double convertFromHMS( double hour, double minutes, double seconds );
   static double convertDegFromHMS( const HMS& coord );
   static double convertRadFromHMS( const HMS& coord );
};






// ----------------------------------------------------------------------------


} // pcl

#endif   // __INDIMountInstance_h

// ----------------------------------------------------------------------------
// EOF INDIMountInstance.h - Released 2016/04/28 15:13:36 UTC
