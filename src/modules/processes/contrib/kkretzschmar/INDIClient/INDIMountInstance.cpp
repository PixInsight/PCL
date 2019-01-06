//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0228
// ----------------------------------------------------------------------------
// INDIMountInstance.cpp - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

#include "Alignment.h"
#include "INDIClient.h"
#include "INDIMountInstance.h"
#include "INDIMountParameters.h"

#undef J2000 // #defined in INDI/indicom.h

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/Math.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/Position.h>
#include <pcl/StdStatus.h>
#include <pcl/File.h>

namespace pcl
{

// ----------------------------------------------------------------------------

INDIMountInstance::INDIMountInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_deviceName( TheIMCDeviceNameParameter->DefaultValue() ),
   p_command( IMCCommand::Default ),
   p_slewRate( IMCSlewRate::Default ),
   p_alignmentMethod(IMCAlignmentMethod::Default),
   p_pierSide( IMCPierSide::Default),
   p_targetRA( TheIMCTargetRAParameter->DefaultValue() ),
   p_targetDec( TheIMCTargetDecParameter->DefaultValue() ),
   p_computeApparentPosition( TheIMCComputeApparentPositionParameter->DefaultValue() ),
   p_enableAlignmentCorrection(TheIMCEnableAlignmentCorrectionParameter->DefaultValue()),
   p_alignmentFile(TheIMCAlignmentFileParameter->DefaultValue()),
   p_alignmentConfig(TheIMCAlignmentConfigParameter->DefaultValue()),
   o_currentLST( TheIMCCurrentLSTParameter->DefaultValue() ),
   o_currentRA( TheIMCCurrentRAParameter->DefaultValue() ),
   o_currentDec( TheIMCCurrentDecParameter->DefaultValue() ),
   o_apparentTargetRA( TheIMCApparentTargetRAParameter->DefaultValue() ),
   o_apparentTargetDec( TheIMCApparentTargetDecParameter->DefaultValue() ),
   o_geographicLatitude( TheIMCGeographicLatitudeParameter->DefaultValue() ),
   o_syncLST(TheIMCSyncLSTParameter->DefaultValue()),
   o_syncCelestialRA(TheIMCSyncCelestialRAParameter->DefaultValue()) ,
   o_syncCelestialDEC(TheIMCSyncCelestialDecParameter->DefaultValue()),
   o_syncTelescopeRA(TheIMCSyncTelescopeRAParameter->DefaultValue()),
   o_syncTelescopeDEC(TheIMCSyncTelescopeDecParameter->DefaultValue())
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
      p_deviceName                = x->p_deviceName;
      p_command                   = x->p_command;
      p_slewRate                  = x->p_slewRate;
      p_targetRA                  = x->p_targetRA;
      p_targetDec                 = x->p_targetDec;
      p_pierSide                  = x->p_pierSide;
      p_computeApparentPosition   = x->p_computeApparentPosition;
      p_enableAlignmentCorrection = x->p_enableAlignmentCorrection;
      p_alignmentMethod           = x->p_alignmentMethod;
      p_alignmentFile             = x->p_alignmentFile;
      p_alignmentConfig           = x->p_alignmentConfig;
      o_currentLST                = x->o_currentLST;
      o_currentRA                 = x->o_currentRA;
      o_currentDec                = x->o_currentDec;
      o_apparentTargetRA          = x->o_apparentTargetRA;
      o_apparentTargetDec         = x->o_apparentTargetDec;
      o_geographicLatitude        = x->o_geographicLatitude;
      o_syncLST                   = x->o_syncLST;
      o_syncCelestialRA           = x->o_syncCelestialRA;
      o_syncCelestialDEC          = x->o_syncCelestialDEC;
      o_syncTelescopeRA           = x->o_syncTelescopeRA;
      o_syncTelescopeDEC          = x->o_syncTelescopeDEC;
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

   if ( !view.HasProperty( "Image:Center:RA" ) || !view.HasProperty( "Image:Center:Dec" ) )
   {
      FITSKeywordArray keywords;
      view.Window().GetKeywords( keywords );
      int keysExist = 0;
      for ( const FITSHeaderKeyword& key : keywords )
         if ( key.name == "OBJCTRA" )
            keysExist |= 1;
         else if ( key.name == "OBJCTDEC" )
            keysExist |= 2;
      if ( keysExist != 3 )
      {
         whyNot = "The view does not define valid image center coordinates.";
         return false;
      }
   }

   return true;
}

bool INDIMountInstance::IsHistoryUpdater( const View& ) const
{
   return false;
}

bool INDIMountInstance::CanExecuteGlobal( pcl::String& whyNot ) const
{
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
      case IMCCommand::GoTo:
      case IMCCommand::Park:
      case IMCCommand::ParkDefault:
      case IMCCommand::TestSync:
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
            case IMCCommand::GoTo:
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
            case IMCCommand::TestSync:
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
      if ( m_console.AbortRequested() )
      {
         // N.B.: We have to do this here because there is no guarantee that
         // m_monitor will check for abortion below. For example, for very
         // small slew distances, m_monitor could never be updated.
         m_console.Abort();
         Abort();
      }
      else
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


      if ( view.HasProperty( "Image:Center:RA" ) && view.HasProperty( "Image:Center:Dec" ) )
      {
         ra = view.PropertyValue( "Image:Center:RA" );
         dec = view.PropertyValue( "Image:Center:Dec" );
         if ( !ra.IsValid() || !dec.IsValid() )
            throw Error( "The view does not define valid image center coordinates." );
         imageCenterRA = ra.ToDouble()/15;
         imageCenterDec = dec.ToDouble();
      }
      else
      {
         FITSKeywordArray keywords;
         view.Window().GetKeywords( keywords );
         for ( auto k : keywords )
            if ( k.name == "OBJCTRA" )
               k.StripValueDelimiters().TrySexagesimalToDouble( imageCenterRA, ' ' );
            else if ( k.name == "OBJCTDEC" )
               k.StripValueDelimiters().TrySexagesimalToDouble( imageCenterDec, ' ' );
         if ( imageCenterRA < 0 || imageCenterDec < -90 )
            throw Error( "The view does not define image center coordinates." );
         Console().WarningLn( "<end><cbr>Warning: Retrieved image center coordinates from obsolete FITS keywords 'OBJCTRA' and 'OBJCTDEC'" );
      }
   }

   GetCurrentCoordinates();

   double deltaRA = observationCenterRA - imageCenterRA;
   double deltaDec = observationCenterDec - imageCenterDec;

   if ( o_currentLST >= 0 ) // ### N.B.: o_currentLST < 0 if LST property could not be retrieved
   {
      double currentHourAngle = AlignmentModel::RangeShiftHourAngle(o_currentLST - o_currentRA);
      double newHourAngle = currentHourAngle - deltaRA;
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

   // Save original parameter values
   pcl_enum storedCommand = p_command;
   double storedTargetRA = p_targetRA;
   double storedTargetDec = p_targetDec;
   bool stored_alignmentMode = this->p_enableAlignmentCorrection;
   pcl_bool storedComputeApparentPosition = p_computeApparentPosition;
   pcl_bool storedEnableAlignmentCorrection = p_enableAlignmentCorrection;

   try
   {
      p_command = IMCCommand::GoTo;
      p_targetRA = AlignmentModel::RangeShiftRightAscension(o_currentRA + deltaRA);
      p_targetDec = o_currentDec + deltaDec;
      p_computeApparentPosition = false;
      p_enableAlignmentCorrection = false;

      Console().WriteLn( "<end><cbr>Applying differential correction: dRA = "
                  + String::ToSexagesimal( deltaRA,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) )
                  + ", dDec = "
                  + String::ToSexagesimal( deltaDec,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) ) );

      INDIMountInstanceExecution( *this ).Perform();

      // Restore original parameter values
      p_command = storedCommand;
      p_targetRA = storedTargetRA;
      p_targetDec = storedTargetDec;
      p_computeApparentPosition = storedComputeApparentPosition;
      p_enableAlignmentCorrection = stored_alignmentMode;
      p_enableAlignmentCorrection = storedEnableAlignmentCorrection;
      return true;
   }
   catch( ... )
   {
      p_command = storedCommand;
      p_targetRA = storedTargetRA;
      p_targetDec = storedTargetDec;
      p_computeApparentPosition = storedComputeApparentPosition;
      p_enableAlignmentCorrection = storedEnableAlignmentCorrection;
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
   if ( p == TheIMCAlignmentMethodParameter )
         return &p_alignmentMethod;
   if ( p == TheIMCEnableAlignmentCorrectionParameter )
	   return &p_enableAlignmentCorrection;
   if ( p == TheIMCTargetRAParameter )
      return &p_targetRA;
   if ( p == TheIMCTargetDecParameter )
      return &p_targetDec;
   if ( p == TheIMCComputeApparentPositionParameter )
      return &p_computeApparentPosition;
   if ( p == TheIMCAlignmentFileParameter )
	   return p_alignmentFile.Begin();
   if ( p == TheIMCAlignmentConfigParameter )
	   return &p_alignmentConfig;
   if (p == TheIMCCurrentLSTParameter )
      return &o_currentLST;
   if ( p == TheIMCCurrentRAParameter )
      return &o_currentRA;
   if ( p == TheIMCCurrentDecParameter )
      return &o_currentDec;
   if ( p == TheIMCApparentTargetRAParameter )
      return &o_apparentTargetRA;
   if ( p == TheIMCApparentTargetDecParameter )
      return &o_apparentTargetDec;
   if ( p == TheIMCPierSideParameter )
	   return &p_pierSide;
   if ( p == TheIMCSyncLSTParameter )
   	   return &o_syncLST;
   if ( p == TheIMCSyncCelestialRAParameter )
	   return &o_syncCelestialRA;
   if ( p == TheIMCSyncCelestialDecParameter )
	   return &o_syncCelestialDEC;
   if ( p == TheIMCSyncTelescopeRAParameter )
	   return &o_syncTelescopeRA;
   if ( p == TheIMCSyncTelescopeDecParameter )
	   return &o_syncTelescopeDEC;
   if ( p == TheIMCGeographicLatitudeParameter)
   	   return &o_geographicLatitude;

   return nullptr;
}

bool INDIMountInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheIMCDeviceNameParameter )
   {
      p_deviceName.Clear();
      if ( sizeOrLength > 0 )
         p_deviceName.SetLength( sizeOrLength );
   } else if ( p == TheIMCAlignmentFileParameter )
   {
	   p_alignmentFile.Clear();
	   if ( sizeOrLength > 0 )
		   p_alignmentFile.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type INDIMountInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheIMCDeviceNameParameter )
      return p_deviceName.Length();

   if ( p == TheIMCAlignmentFileParameter )
         return p_alignmentFile.Length();

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

void INDIMountInstance::GetTargetCoordinates( double& targetRA, double& targetDec ) const
{
   if ( p_computeApparentPosition )
   {
      Position P( TimePoint::Now(), "UTC" );
      P.InitEquinoxBasedParameters();
      Vector u2 = Vector::FromSpherical( Rad( p_targetRA*15 ), Rad( p_targetDec ) );
      Vector u3 = P.EquinoxBiasPrecessionNutationMatrix() * u2;
      u3.ToSpherical2Pi( targetRA, targetDec );
      targetRA = o_apparentTargetRA = Deg( targetRA )/15;
      targetDec = o_apparentTargetDec = Deg( targetDec );
   }
   else
   {
      targetRA = p_targetRA;
      targetDec = p_targetDec;
   }
}


void INDIMountInstance::GetPierSide() {
   INDIClient* indi = INDIClient::TheClientOrDie();

   INDIPropertyListItem item;
   static const char* indiPierSides[] = { "PIER_WEST", "PIER_EAST" };
   for ( size_type i = 0; i < ItemsInArray( indiPierSides ); ++i )
      if ( indi->GetPropertyItem( p_deviceName, "TELESCOPE_PIER_SIDE", indiPierSides[i], item ) )
         if ( item.PropertyValue == "ON" )
         {
            p_pierSide = i;
            break;
         }
      else
         {
            // pier side fallback
            // If the INDI mount device does not support the TELESCOPE_PIER_SIDE property, compute the pierside from hour angle
            double hourAngle = AlignmentModel::RangeShiftHourAngle(o_currentLST - o_currentRA);
            p_pierSide = hourAngle <= 0 ? IMCPierSide::West : IMCPierSide::East;
         }

}

 bool INDIMountInstance::isForceCounterWeightUp() const {
    INDIClient* indi = INDIClient::TheClientOrDie();
    INDIPropertyListItem item;
    if ( indi->GetPropertyItem( p_deviceName, "FORCECWUP", "ENABLE", item))
       if ( item.PropertyValue == "ON" )
          return true;

    return false;
 }


void INDIMountInstance::AddSyncDataPoint(const SyncDataPoint& syncDataPoint){
	this->syncDataArray.Add(syncDataPoint);
}


void INDIMountInstance::loadSyncData( Array<SyncDataPoint>& syncDataList, String syncDataFile )
{
   IsoStringList syncDataRows = File::ReadLines( syncDataFile );
   for ( const IsoString& row : syncDataRows )
   {
      IsoStringList tokens;
      row.Break( tokens, ',', true/*trim*/ );
      if ( !tokens.IsEmpty() )
         if ( tokens.Length() == 7 )
            syncDataList.Add( { TimePoint::Now(),
                                tokens[0].ToDouble(),
                                tokens[1].ToDouble(),
                                tokens[2].ToDouble(),
                                tokens[3].ToDouble(),
                                tokens[4].ToDouble(),
                                (tokens[5] == "West") ? IMCPierSide::West : ((tokens[5] == "East") ? IMCPierSide::East : IMCPierSide::None),
                                tokens[6] == "true" } );
         // else
         //    throw Error( "Invalid sync data format" );
   }
}

void AbstractINDIMountExecution::ApplyPointingModelCorrection(AlignmentModel* aModel, double& targetRA, double& targetDec) {
	double localSiderialTime = m_instance.o_currentLST;
	aModel->ReadObject(m_instance.p_alignmentFile);
	double newHourAngle=-1;
	double newDec=-1;

   aModel->Apply(newHourAngle, newDec, AlignmentModel::RangeShiftHourAngle(localSiderialTime - targetRA), targetDec, m_instance.p_pierSide);
	targetRA=localSiderialTime-newHourAngle;
	targetDec=newDec;
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

      case IMCCommand::GoTo:
         {
            double targetRARaw, targetDecRaw;
            m_instance.GetTargetCoordinates( targetRARaw, targetDecRaw );
            double targetRA, targetDec, hourAngle;

            // Do not apply telescope pointing model in differential correction or during collection of sync points
            if (m_instance.p_enableAlignmentCorrection){
          	  AutoPointer<AlignmentModel> aModel = nullptr;

          	  switch (m_instance.p_alignmentMethod){
          	  case IMCAlignmentMethod::AnalyticalModel:
                 aModel = GeneralAnalyticalPointingModel::Create(m_instance.o_geographicLatitude, m_instance.p_alignmentConfig, CHECK_BIT(m_instance.p_alignmentConfig, 0));
              	  aModel->ReadObject(m_instance.p_alignmentFile);
          		  break;
          	  default:
          		  aModel = AlignmentModel::Create(m_instance.p_alignmentFile);
          	  }
              if (aModel == nullptr){
          		throw Error( "Alignment model could not be loaded" );
          	  }
              bool isForceCounterWeightUp = m_instance.isForceCounterWeightUp();
              pcl_enum pierside = aModel->PierSideFromHourAngle(AlignmentModel::RangeShiftHourAngle(m_instance.o_currentLST - targetRARaw), isForceCounterWeightUp);
              aModel->Apply(hourAngle, targetDec, AlignmentModel::RangeShiftHourAngle(m_instance.o_currentLST - targetRARaw), targetDecRaw, pierside);
          	  targetRA=AlignmentModel::RangeShiftRightAscension(m_instance.o_currentLST-hourAngle);

          	  double deltaRA  = targetRA  - targetRARaw;
          	  double deltaDec = targetDec - targetDecRaw;

          	  Console().WriteLn( "<end><cbr>Applying pointing model correction: dRA = "
          	                  + String::ToSexagesimal( deltaRA,
          	                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) )
          	                  + ", dDec = "
          	                  + String::ToSexagesimal( deltaDec,
          	                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) ) );


            } else {
            	targetRA = targetRARaw;
            	targetDec = targetDecRaw;
            }

            StartMountEvent( targetRA, m_instance.o_currentRA, targetDec, m_instance.o_currentDec, m_instance.p_command );
            indi->SendNewPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "INDI_NUMBER",  // send (RA,DEC) coordinates in bulk request
                                       "RA", targetRA,
                                       "DEC", targetDec, true/*async*/ );
            for ( ElapsedTime T; ; )
            {
               INDIPropertyListItem RA_item;
               INDIPropertyListItem Dec_item;
               if (    indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "RA", RA_item, false/*formatted*/ )
                  && indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "DEC", Dec_item, false/*formatted*/ ) )
                  if ( RA_item.PropertyState == INDIGO_BUSY_STATE || Dec_item.PropertyState == INDIGO_BUSY_STATE )
                  {
                     if ( T() > 0.1 )
                     {
                        T.Reset();
                        m_instance.o_currentRA = RA_item.PropertyValue.ToDouble();
                        m_instance.o_currentDec = Dec_item.PropertyValue.ToDouble();
                        MountEvent( targetRA, m_instance.o_currentRA, targetDec, m_instance.o_currentDec );
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
               if ( item.PropertyState == INDIGO_BUSY_STATE )
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
                  if ( item.PropertyState == INDIGO_BUSY_STATE )
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
      case IMCCommand::TestSync:
      {
    	  double targetRA, targetDec;
    	  m_instance.GetTargetCoordinates( targetRA, targetDec );
    	  m_instance.GetCurrentCoordinates();

    	  AutoPointer<AlignmentModel> aModel;

    	  switch (m_instance.p_alignmentMethod){
    	  case IMCAlignmentMethod::AnalyticalModel:
           aModel = GeneralAnalyticalPointingModel::Create(m_instance.o_geographicLatitude, m_instance.p_alignmentConfig, CHECK_BIT(m_instance.p_alignmentConfig, 0));
    		  break;
    	  default:
    		  throw Error( "Internal error: AbstractINDIMountExecution::Perform(): Unknown Pointing Model." );
    	  }
    	  // Apply corrected position to simulate an unaligned telescope
    	  double trueTargetRa = targetRA;
    	  double trueTargetDec = targetDec;
        m_instance.GetPierSide();
    	  ApplyPointingModelCorrection(aModel.Ptr(), trueTargetRa, trueTargetDec);

          StartMountEvent( targetRA, m_instance.o_currentRA, targetDec, m_instance.o_currentDec, m_instance.p_command );
          switch (m_instance.p_alignmentMethod){
           case IMCAlignmentMethod::AnalyticalModel:
           {
        	   SyncDataPoint syncPoint;
        	   syncPoint.creationTime      = TimePoint::Now();
        	   syncPoint.localSiderialTime = m_instance.o_currentLST;
        	   syncPoint.celestialRA       = AlignmentModel::RangeShiftHourAngle(targetRA);
        	   syncPoint.celestialDEC      = targetDec;
        	   syncPoint.telecopeRA        = trueTargetRa;
        	   syncPoint.telecopeDEC       = trueTargetDec;
            syncPoint.pierSide          = m_instance.p_pierSide;

        	   aModel->AddSyncDataPoint(syncPoint);
        	   aModel->WriteObject(m_instance.p_alignmentFile);
        	   break;
           }

          }
          m_instance.GetCurrentCoordinates();

          EndMountEvent();

    	  break; // TestSync
      }
      case IMCCommand::Sync:
         {
            double targetRA, targetDec;
            m_instance.GetTargetCoordinates( targetRA, targetDec );
            m_instance.GetCurrentCoordinates();
            m_instance.GetPierSide();

            StartMountEvent( targetRA, m_instance.o_currentRA, targetDec, m_instance.o_currentDec, m_instance.p_command );
            switch (m_instance.p_alignmentMethod){
            case IMCAlignmentMethod::ServerModel:
            {
            	indi->SendNewPropertyItem( m_instance.p_deviceName, "ON_COORD_SET", "INDI_SWITCH", "SYNC", "ON", false/*async*/ );
            	indi->SendNewPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "INDI_NUMBER",
            			                  "RA", targetRA,
						                  "DEC", targetDec, false/*async*/ );
            	indi->SendNewPropertyItem( m_instance.p_deviceName, "ON_COORD_SET", "INDI_SWITCH", "TRACK", "ON", false/*async*/ );

            	// TODO: get sync point coordinates from server and set parameter
            }
            break;
            case IMCAlignmentMethod::AnalyticalModel:
            case IMCAlignmentMethod::None:
            {
               try {
                  AutoPointer<AlignmentModel> aModel = AlignmentModel::Create(m_instance.p_alignmentFile);
                  SyncDataPoint syncPoint;
                  syncPoint.creationTime      = TimePoint::Now();
                  syncPoint.localSiderialTime = m_instance.o_currentLST;
                  syncPoint.celestialRA       = targetRA;
                  syncPoint.celestialDEC      = targetDec;
                  syncPoint.telecopeRA        = m_instance.o_currentRA;
                  syncPoint.telecopeDEC       = m_instance.o_currentDec;
                  syncPoint.pierSide          = m_instance.p_pierSide;
                  aModel->AddSyncDataPoint(syncPoint);
                  aModel->WriteObject(m_instance.p_alignmentFile);
               } catch (...) {
                  EndMountEvent();
                  throw;
               }

            	break;
            }
            break;
            default:
            	throw Error( "Internal error: AbstractINDIMountExecution::Perform(): Unknown Alignment Method." );
            }
            m_instance.GetCurrentCoordinates();

            EndMountEvent();
         }
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
      case IMCCommand::FitPointingModel:
      {
    	 AutoPointer<AlignmentModel> aModel = nullptr;
    	 switch (m_instance.p_alignmentMethod){
    	 case IMCAlignmentMethod::AnalyticalModel:
          aModel = GeneralAnalyticalPointingModel::Create(m_instance.o_geographicLatitude, m_instance.p_alignmentConfig, CHECK_BIT(m_instance.p_alignmentConfig, 0));
    		 break;
    	 default:
    		 throw Error( "Internal error: AbstractINDIMountExecution::Perform(): Unknown Pointing Model." );
    	 }

       aModel->ReadSyncData(m_instance.p_alignmentFile);
    	 // fit model
    	 aModel->FitModel();
    	 aModel->WriteObject(m_instance.p_alignmentFile);
    	 aModel->PrintParameters();
    	 break;
      }
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
// EOF INDIMountInstance.cpp - Released 2018-11-23T18:45:59Z
