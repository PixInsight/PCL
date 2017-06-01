//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0203
// ----------------------------------------------------------------------------
// INDIMountInstance.h - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#ifndef __INDIMountInstance_h
#define __INDIMountInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/Array.h>

#include "Alignment.h"

namespace pcl
{



// ----------------------------------------------------------------------------

class INDIMountInstance : public ProcessImplementation
{
public:

   INDIMountInstance( const MetaProcess* );
   INDIMountInstance( const INDIMountInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;
   virtual bool ExecuteOn( View& view );

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter* p, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   bool ValidateDevice( bool throwErrors = true ) const;
   void SendDeviceProperties( bool asynchronous = true ) const;

   static String MountSlewRatePropertyString( int slewRateIdx );

   void GetCurrentCoordinates();
   void GetTargetCoordinates( double& ra, double& dec ) const;

   void AddSyncDataPoint(const SyncDataPoint& syncDataPoint);

   void loadSyncData();
   static void loadSyncData(Array<SyncDataPoint>& syncDataList, String syncDataFile);

private:

           String   p_deviceName;
           pcl_enum p_command;
           pcl_enum p_slewRate;
           pcl_enum p_alignmentMethod;
           pcl_enum p_pierSide;
           double   p_targetRA;
           double   p_targetDec;
           pcl_bool p_computeApparentPosition;
           pcl_bool p_enableAlignmentCorrection;
           String   p_alignmentFile;
           String   p_syncDataFile;
           int32    p_alignmentConfig;

           double   o_currentLST;
           double   o_currentRA;
           double   o_currentDec;
   mutable double   o_apparentTargetRA;
   mutable double   o_apparentTargetDec;
   	   	   double   o_geographicLatitude;
   	   	   double   o_syncLST;
   	   	   double   o_syncCelestialRA;
  	   	   double   o_syncCelestialDEC;
  	   	   double   o_syncTelescopeRA;
  	   	   double   o_syncTelescopeDEC;




   Array<SyncDataPoint>  syncDataArray;

   friend class INDIMountInterface;
   friend class AbstractINDIMountExecution;

};

// ----------------------------------------------------------------------------

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

   void Perform( pcl_enum command )
   {
      m_instance.p_command = command;
      Perform();
   }

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

protected:

   INDIMountInstance& m_instance;

   virtual void StartMountEvent( double targetRA, double currentRA, double targetDec, double currentDec, pcl_enum command ) = 0;
   virtual void MountEvent( double targetRA, double currentRA, double targetDec, double currentDec ) = 0;
   virtual void EndMountEvent() = 0;
   virtual void WaitEvent() = 0;
   virtual void AbortEvent() = 0;

private:

   bool m_running, m_aborted;

   void ApplyPointingModelCorrection(AlignmentModel* aModel, double& targetRA, double& targetDec);
   void WriteSyncDataPointToFile(const SyncDataPoint& syncPoint);
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIMountInstance_h

// ----------------------------------------------------------------------------
// EOF INDIMountInstance.h - Released 2017-05-02T09:43:01Z
