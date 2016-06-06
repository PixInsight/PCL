//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.12.0183
// ----------------------------------------------------------------------------
// INDIMountInstance.cpp - Released 2016/06/04 15:14:47 UTC
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

#include "INDIClient.h"
#include "INDIMountInstance.h"
#include "INDIMountParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/Math.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/StdStatus.h>

namespace pcl
{

// ----------------------------------------------------------------------------

INDIMountInstance::INDIMountInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_deviceName( TheIMCDeviceNameParameter->DefaultValue() ),
   p_command( IMCCommand::Default ),
   p_slewRate( IMCSlewRate::Default ),
   p_targetRA( TheIMCTargetRAParameter->DefaultValue() ),
   p_targetDec( TheIMCTargetDecParameter->DefaultValue() ),
   o_currentLST( TheIMCCurrentLSTParameter->DefaultValue() ),
   o_currentRA( TheIMCCurrentRAParameter->DefaultValue() ),
   o_currentDec( TheIMCCurrentDecParameter->DefaultValue() )
{
}

INDIMountInstance::INDIMountInstance( const INDIMountInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void INDIMountInstance::Assign( const ProcessImplementation& p )
{
   const INDIMountInstance* x = dynamic_cast<const INDIMountInstance*>( &p );
   if ( x != nullptr )
   {
      p_deviceName = x->p_deviceName;
      p_command    = x->p_command;
      p_slewRate   = x->p_slewRate;
      o_currentLST = x->o_currentLST;
      p_targetRA   = x->p_targetRA;
      p_targetDec  = x->o_currentDec;
      o_currentRA  = x->o_currentRA;
      o_currentDec = x->o_currentDec;
   }
}

bool INDIMountInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( p_deviceName.IsEmpty() )
   {
      whyNot = "No device has been specified";
      return false;
   }

   if ( !view.IsMainView() )
   {
      whyNot = "IMDIMount can only be executed on main views, not on previews.";
      return false;
   }

   if ( !view.HasProperty( "Observation:Center:RA" ) || !view.HasProperty( "Observation:Center:Dec" ) )
   {
      whyNot = "The view does not define valid observation coordinates.";
      return false;
   }

   FITSKeywordArray keywords;
   view.Window().GetKeywords( keywords );
   int keysExist = 0;
   for ( const FITSHeaderKeyword& key : keywords )
      if ( key.name == "OBJCTRA" )
         keysExist |= 1;
      else if ( key.name == "OBJCTDEC" )
         keysExist |= 2;
   if ( keysExist == 3 )
      return true;
   whyNot = "The view does not define valid center coordinates.";
   return false;
}

bool INDIMountInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

bool INDIMountInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

// ---------------------------------------------------------------------------

class INDIMountInstanceExecution : public AbstractINDIMountExecution
{
public:

   INDIMountInstanceExecution( INDIMountInstance& instance ) :
      AbstractINDIMountExecution( instance )
   {
      m_command = IMCCommand::Default;
   }

private:

   Console        m_console;
   StandardStatus m_status;
   StatusMonitor  m_monitor;
   pcl_enum       m_command;

   static size_type TargetDistance( double targetRA, double currentRA, double targetDec, double currentDec )
   {
      return RoundInt64( (Abs( targetRA - currentRA ) + Abs( targetDec - currentDec ))*1e6 );
   }

   virtual void StartMountEvent( double targetRA, double currentRA, double targetDec, double currentDec, pcl_enum command )
   {
      m_console.EnableAbort();
      m_command = command;

      switch ( m_command )
      {
      case IMCCommand::Goto:
      case IMCCommand::Park:
      case IMCCommand::ParkDefault:
      case IMCCommand::Sync:
         {
            String targetPosText =
                 "RA = "
               + String::ToSexagesimal( targetRA,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/ ) )
               + ", Dec = "
               + String::ToSexagesimal( targetDec,
                           SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/ ) )
               + ' ';

            switch ( m_command )
            {
            case IMCCommand::Goto:
               m_monitor.Clear();
               m_monitor.SetCallback( &m_status );
               m_monitor.Initialize( "Slewing to " + targetPosText, TargetDistance( targetRA, currentRA, targetDec, currentDec ) );
               break;
            case IMCCommand::Park:
            case IMCCommand::ParkDefault:
               m_monitor.Clear();
               m_monitor.SetCallback( &m_status );
               m_monitor.Initialize( "Parking telescope to " + targetPosText, TargetDistance( targetRA, currentRA, targetDec, currentDec ) );
               break;
            case IMCCommand::Sync:
               m_console.WriteLn( "<end><cbr>Syncing mount: dRA = "
                  + String::ToSexagesimal( targetRA - currentRA,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) )
                  + ", dDec = "
                  + String::ToSexagesimal( targetDec - currentDec,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) )
                  + ", Position: "
                  + targetPosText );
               break;
            }
         }
         break;

      case IMCCommand::MoveNorthStart:
         m_console.WriteLn( "<end><cbr>Start slewing toward North..." );
         break;
      case IMCCommand::MoveSouthStart:
         m_console.WriteLn( "<end><cbr>Start slewing toward South..." );
         break;
      case IMCCommand::MoveWestStart:
         m_console.WriteLn( "<end><cbr>Start slewing toward West..." );
         break;
      case IMCCommand::MoveEastStart:
         m_console.WriteLn( "<end><cbr>Start slewing toward East..." );
         break;

      case IMCCommand::MoveNorthStop:
      case IMCCommand::MoveSouthStop:
      case IMCCommand::MoveWestStop:
      case IMCCommand::MoveEastStop:
         m_console.WriteLn( "<end><cbr>Stop slewing." );
         break;

      default: // ?!
         break;
      }

      Module->ProcessEvents();
   }

   virtual void MountEvent( double targetRA, double currentRA, double targetDec, double currentDec )
   {
      if ( m_monitor.IsInitialized() )
      {
         // Always make sure we have a valid monitor count available.
         size_type distance = TargetDistance( targetRA, currentRA, targetDec, currentDec );
         if ( m_monitor.Total() > distance )
         {
            size_type delta = m_monitor.Total() - distance;
            if ( delta > m_monitor.Count() )
               m_monitor += delta - m_monitor.Count();
         }
      }
      Module->ProcessEvents();
   }

   virtual void EndMountEvent()
   {
      m_console.DisableAbort();
      if ( m_monitor.IsInitialized() )
         m_monitor.Complete();
      Module->ProcessEvents();
   }

   virtual void WaitEvent()
   {
      Module->ProcessEvents();
   }

   virtual void AbortEvent()
   {
   }
};

// ---------------------------------------------------------------------------

bool INDIMountInstance::ExecuteGlobal()
{
   INDIMountInstanceExecution( *this ).Perform();
   return true;
}

bool INDIMountInstance::ExecuteOn( View& view )
{
   double observationCenterRA, observationCenterDec;
   double imageCenterRA = -1, imageCenterDec = -91;
   {
      AutoViewLock lock( view );

      Variant ra = view.PropertyValue( "Observation:Center:RA" );
      Variant dec = view.PropertyValue( "Observation:Center:Dec" );
      if ( !ra.IsValid() || !dec.IsValid() )
         throw Error( "The view does not define valid observation coordinates." );
      observationCenterRA = ra.ToDouble()/15;
      observationCenterDec = dec.ToDouble();

      // ### TODO: Implement this via standard XISF properties.
      FITSKeywordArray keywords;
      view.Window().GetKeywords( keywords );
      for ( auto k : keywords )
         if ( k.name == "OBJCTRA" )
            k.StripValueDelimiters().TrySexagesimalToDouble( imageCenterRA, ' ' );
         else if ( k.name == "OBJCTDEC" )
            k.StripValueDelimiters().TrySexagesimalToDouble( imageCenterDec, ' ' );
      if ( imageCenterRA < 0 || imageCenterDec < -90 )
         throw Error( "The view does not define valid center coordinates." );
   }

   // Save original parameters
   double storedTargetRA = p_targetRA;
   double storedTargetDec = p_targetDec;
   pcl_enum storedCommand = p_command;

   GetCurrentCoordinates();

   double deltaRA = imageCenterRA - observationCenterRA;
   double deltaDec = imageCenterDec - observationCenterDec;

   p_targetRA = o_currentRA + deltaRA;
   p_targetDec = o_currentDec + deltaDec;

   if ( o_currentLST >= 0 ) // ### N.B.: o_currentLST < 0 if LST property could not be retrieved
   {
      double currentHourAngle = o_currentLST - o_currentRA;
      double newHourAngle = o_currentLST - p_targetRA;
      if ( (currentHourAngle < 0) != (newHourAngle < 0) )
         if ( MessageBox( "<p>New center right ascension coordinate crosses the meridian, and will possibly trigger a meridian flip.</p>"
                           "<p><b>Continue?</b></p>",
                           Meta()->Id(),
                           StdIcon::Warning,
                           StdButton::Yes, StdButton::No ).Execute() != StdButton::Yes )
         {
            return false;
         }
   }

   try
   {
      Console().WriteLn( "<end><cbr>Applying differential correction: dRA = "
                  + String::ToSexagesimal( deltaRA,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) )
                  + ", dDec = "
                  + String::ToSexagesimal( deltaDec,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) ) );

      p_command = IMCCommand::Goto;
      INDIMountInstanceExecution( *this ).Perform();

      // Restore original parameters
      p_targetRA = storedTargetRA;
      p_targetDec = storedTargetDec;
      p_command = storedCommand;
      return true;
   }
   catch( ... )
   {
      p_targetRA = storedTargetRA;
      p_targetDec = storedTargetDec;
      p_command = storedCommand;
      throw;
   }
}

void* INDIMountInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIMCDeviceNameParameter )
      return p_deviceName.Begin();
   if ( p == TheIMCCommandParameter )
      return &p_command;
   if ( p == TheIMCSlewRateParameter )
      return &p_slewRate;
   if ( p == TheIMCTargetRAParameter )
      return &p_targetRA;
   if ( p == TheIMCTargetDecParameter )
      return &p_targetDec;
   if (p == TheIMCCurrentLSTParameter )
      return &o_currentLST;
   if ( p == TheIMCCurrentRAParameter )
      return &o_currentRA;
   if ( p == TheIMCCurrentDecParameter )
      return &o_currentDec;

   return nullptr;
}

bool INDIMountInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIMCDeviceNameParameter )
   {
      p_deviceName.Clear();
      if ( sizeOrLength > 0 )
         p_deviceName.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type INDIMountInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheIMCDeviceNameParameter )
      return p_deviceName.Length();

   return 0;
}

bool INDIMountInstance::ValidateDevice( bool throwErrors ) const
{
   if ( p_deviceName.IsEmpty() )
   {
      if ( throwErrors )
         throw Error( "No device has been specified" );
      return false;
   }

   INDIClient* indi = INDIClient::TheClientOrDie();
   ExclConstDeviceList x = indi->ConstDeviceList();
   const INDIDeviceListItemArray& devices( x );
   for ( auto device : devices )
      if ( device.DeviceName == p_deviceName )
      {
         if ( !indi->HasPropertyItem( device.DeviceName, "EQUATORIAL_EOD_COORD", "RA" ) ) // is this a mount device?
         {
            if ( throwErrors )
               throw Error( '\'' + p_deviceName + "' does not seem to be a valid INDI Mount device" );
            return false;
         }
         return true;
      }

   if ( throwErrors )
      throw Error( "INDI device not available: '" + p_deviceName + "'" );
   return false;
}

void INDIMountInstance::SendDeviceProperties( bool async ) const
{
   INDIClient* indi = INDIClient::TheClientOrDie();
   indi->MaybeSendNewPropertyItem( p_deviceName, "TELESCOPE_SLEW_RATE", "INDI_SWITCH", MountSlewRatePropertyString( p_slewRate ), "ON", async );
}

String INDIMountInstance::MountSlewRatePropertyString( int slewRateIdx )
{
   switch ( slewRateIdx )
   {
   case IMCSlewRate::Guide:
      return "SLEW_GUIDE";
   case IMCSlewRate::Centering:
      return "SLEW_CENTERING";
   case IMCSlewRate::Find:
      return "SLEW_FIND";
   case IMCSlewRate::Max:
      return "SLEW_MAX";
   default:
      throw Error( "Internal error: INDIMountInstance: Invalid slew rate." );
   }
}

void INDIMountInstance::GetCurrentCoordinates()
{
   // Unknown coordinate values
   o_currentLST = -1;
   o_currentRA = -1;
   o_currentDec = -91;

   INDIClient* indi = INDIClient::TheClientOrDie();

   INDIPropertyListItem itemH, itemD;
   if ( !indi->GetPropertyItem( p_deviceName, "EQUATORIAL_EOD_COORD", "RA", itemH, false/*formatted*/ ) ||
        !indi->GetPropertyItem( p_deviceName, "EQUATORIAL_EOD_COORD", "DEC", itemD, false/*formatted*/ ) )
      throw Error( "Cannot get current mount coordinates" );

   o_currentRA = itemH.PropertyValue.ToDouble();
   o_currentDec = itemD.PropertyValue.ToDouble();

   if ( indi->GetPropertyItem( p_deviceName, "TIME_LST", "LST", itemH, false/*formatted*/ ) )
      o_currentLST = itemH.PropertyValue.ToDouble();
}

void AbstractINDIMountExecution::Perform()
{
   if ( IsRunning() )
      throw Error( "Internal error: Recursive call to AbstractINDIMountExecution::Perform() detected." );

   if ( m_instance.p_deviceName.IsEmpty() )
      throw Error( "No device has been specified." );

   INDIClient* indi = INDIClient::TheClientOrDie();

   if ( !indi->HasDevices() )
      throw Error( "No INDI device has been connected." );

   try
   {
      m_instance.ValidateDevice();
      m_instance.SendDeviceProperties( false/*async*/ );
      m_instance.GetCurrentCoordinates();

      m_running = true;
      m_aborted = false;
      switch ( m_instance.p_command )
      {
      case IMCCommand::Unpark:
         indi->MaybeSendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK", "INDI_SWITCH", "UNPARK", "ON", false/*async*/ );
         break;

      case IMCCommand::Goto:
         StartMountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec, m_instance.p_command );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "INDI_NUMBER",  // send (RA,DEC) coordinates in bulk request
                                    "RA", m_instance.p_targetRA,
                                    "DEC", m_instance.p_targetDec, true/*async*/ );
         for ( ElapsedTime T; ; )
         {
            INDIPropertyListItem RA_item;
            INDIPropertyListItem Dec_item;
            if (    indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "RA", RA_item, false/*formatted*/ )
                 && indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "DEC", Dec_item, false/*formatted*/ ) )
               if ( RA_item.PropertyState == IPS_BUSY || Dec_item.PropertyState == IPS_BUSY )
               {
                  if ( T() > 0.1 )
                  {
                     T.Reset();
                     m_instance.o_currentRA = RA_item.PropertyValue.ToDouble();
                     m_instance.o_currentDec = Dec_item.PropertyValue.ToDouble();
                     MountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec );
                  }
                  else
                     WaitEvent();
               }
               else
               {
                  m_instance.GetCurrentCoordinates();
                  EndMountEvent();
                  break;
               }
         }
         m_instance.GetCurrentCoordinates();
         break;

      case IMCCommand::Park:
         StartMountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec, m_instance.p_command );
         indi->MaybeSendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK_POSITION", "INDI_NUMBER",
                                         "PARK_RA", m_instance.p_targetRA,
                                         "PARK_DEC", m_instance.p_targetDec, true/*async*/ );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK", "INDI_SWITCH", "PARK", "ON", true/*async*/ );
         for ( ElapsedTime T; ; )
         {
            INDIPropertyListItem item;
            if ( indi->GetPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK", "PARK", item, false/*formatted*/ ) )
               if ( item.PropertyState == IPS_BUSY )
               {
                  if ( T() > 0.1 )
                  {
                     T.Reset();
                     m_instance.GetCurrentCoordinates();
                     MountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec );
                  }
                  else
                     WaitEvent();

               }
               else
               {
                  m_instance.GetCurrentCoordinates();
                  EndMountEvent();
                  break;
               }
         }
         break;

      case IMCCommand::ParkDefault:
         {
            double parkRA = 0, parkDec = 0;
            INDIPropertyListItem item;
            if ( indi->GetPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK_POSITION", "PARK_RA", item, false/*formatted*/ ) )
               parkRA = item.PropertyValue.ToDouble();
            if ( indi->GetPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK_POSITION", "PARK_DEC", item, false/*formatted*/ ) )
               parkDec = item.PropertyValue.ToDouble();

            StartMountEvent( parkRA, m_instance.o_currentRA, parkDec, m_instance.o_currentDec, m_instance.p_command );
            indi->SendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK", "INDI_SWITCH", "PARK", "ON", true/*async*/ );
            for ( ElapsedTime T; ; )
            {
               if ( indi->GetPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK", "PARK", item, false/*formatted*/ ) )
                  if ( item.PropertyState == IPS_BUSY )
                  {
                     if ( T() > 0.1 )
                     {
                        T.Reset();
                        m_instance.GetCurrentCoordinates();
                        MountEvent( parkRA, m_instance.o_currentRA, parkDec, m_instance.o_currentDec );
                     }
                     else
                        WaitEvent();
                  }
                  else
                  {
                     m_instance.GetCurrentCoordinates();
                     EndMountEvent();
                     break;
                  }
            }
         }
         break;

      case IMCCommand::Sync:
         StartMountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec, m_instance.p_command );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "ON_COORD_SET", "INDI_SWITCH", "SYNC", "ON", false/*async*/ );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "INDI_NUMBER",
                                    "RA", m_instance.p_targetRA,
                                    "DEC",m_instance.p_targetDec, false/*async*/ );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "ON_COORD_SET", "INDI_SWITCH", "TRACK", "ON", false/*async*/ );
         m_instance.GetCurrentCoordinates();
         EndMountEvent();
         break;

      case IMCCommand::MoveNorthStart:
      case IMCCommand::MoveNorthStop:
         StartMountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec, m_instance.p_command );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH",
                                    (m_instance.p_command == IMCCommand::MoveNorthStart) ? "ON" : "OFF", true/*async*/ );
         m_instance.GetCurrentCoordinates();
         EndMountEvent();
         break;

      case IMCCommand::MoveSouthStart:
      case IMCCommand::MoveSouthStop:
         StartMountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec, m_instance.p_command );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH",
                                    (m_instance.p_command == IMCCommand::MoveSouthStart) ? "ON" : "OFF", true /*async*/ );
         m_instance.GetCurrentCoordinates();
         EndMountEvent();
         break;

      case IMCCommand::MoveWestStart:
      case IMCCommand::MoveWestStop:
         StartMountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec, m_instance.p_command );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST",
                                    (m_instance.p_command == IMCCommand::MoveWestStart) ? "ON" : "OFF", true/*async*/ );
         m_instance.GetCurrentCoordinates();
         EndMountEvent();
         break;

      case IMCCommand::MoveEastStart:
      case IMCCommand::MoveEastStop:
         StartMountEvent( m_instance.p_targetRA, m_instance.o_currentRA, m_instance.p_targetDec, m_instance.o_currentDec, m_instance.p_command );
         indi->SendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST",
                                    (m_instance.p_command == IMCCommand::MoveEastStart) ? "ON" : "OFF", true/*async*/ );
         m_instance.GetCurrentCoordinates();
         EndMountEvent();
         break;

      default:
         throw Error( "Internal error: AbstractINDIMountExecution::Perform(): Unknown INDI Mount command." );
      }
   }
   catch ( ... )
   {
      m_running = false;

      try
      {
         throw;
      }
      catch ( ProcessAborted& )
      {
         m_aborted = true;
         indi->MaybeSendNewPropertyItem( m_instance.p_deviceName, "TELESCOPE_ABORT_MOTION", "INDI_SWITCH", "ABORT", "ON", true/*async*/ );
         AbortEvent();
         throw;
      }
      catch ( ... )
      {
         throw;
      }
   }
}

void AbstractINDIMountExecution::Abort()
{
   if ( IsRunning() )
      throw ProcessAborted();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountInstance.cpp - Released 2016/06/04 15:14:47 UTC
