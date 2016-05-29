//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIMountInstance.cpp - Released 2016/04/28 15:13:36 UTC
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

#include "INDIMountInstance.h"
#include "INDIMountParameters.h"
#include "INDIClient.h"
#include <pcl/Console.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Math.h>
#include <pcl/MetaModule.h>
#include <pcl/MessageBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------
CoordUtils::HMS CoordUtils::convertToHMS(double coord){
   HMS coordInHMS;
   coordInHMS.hour   = trunc(coord);
   coordInHMS.minute = trunc((coord - coordInHMS.hour) * 60);
   coordInHMS.second = fabs(trunc(((coord - coordInHMS.hour) * 60 - coordInHMS.minute ) * 60*100)/100.0);
   coordInHMS.minute = fabs(coordInHMS.minute);
   return coordInHMS;
}

CoordUtils::HMS CoordUtils::parse(String coordStr, String sep){
   CoordUtils::HMS result;
   StringList tokens;
   coordStr.Break(tokens,IsoString(sep).c_str(),true);
   tokens[0].TrimLeft();
   result.hour=tokens[0].ToDouble();
   result.minute=tokens[1].ToDouble();
   result.second=tokens[2].ToDouble();
   return result;
}

double CoordUtils::convertFromHMS(const CoordUtils::HMS& coord_in_HMS){
   int sign=coord_in_HMS.hour >= 0 ? 1 : -1;
   // seconds are rounded to two fractional decimals
   double coord=sign * (fabs(coord_in_HMS.hour) + coord_in_HMS.minute / 60.0 + coord_in_HMS.second / 3600.0);
   return coord;
}

double CoordUtils::convertFromHMS( double hour, double minutes, double seconds ){
	HMS hms(hour,minutes,seconds);
	return convertFromHMS(hms);
}

double CoordUtils::convertDegFromHMS(const CoordUtils::HMS& coord_in_HMS){
   return CoordUtils::convertFromHMS(coord_in_HMS) * 360.0 / 24.0 ;
}

double CoordUtils::convertRadFromHMS( const CoordUtils::HMS& coord )
{
   return Rad( CoordUtils::convertDegFromHMS( coord ) );
}

// ----------------------------------------------------------------------------

INDIMountInstance::INDIMountInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_deviceName( TheIMCDeviceNameParameter->DefaultValue() ),
   p_commandType( IMCCommandType::Default),
   p_slewRate (IMCSlewRate::Default),
   p_targetRA( TheIMCTargetRightAscensionParameter->DefaultValue()),
   p_targetDEC (TheIMCTargetDeclinationParameter->DefaultValue()),
   p_lst(TheIMCLocalSiderialTimeParameter->DefaultValue()),
   p_currentRA( TheIMCCurrentRightAscensionParameter->DefaultValue()),
   p_currentDEC( TheIMCCurrentDeclinationParameter->DefaultValue() )

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
	   p_deviceName  = x->p_deviceName;
	   p_commandType = x->p_commandType;
	   p_slewRate    = x->p_slewRate;
	   p_lst         = x->p_lst;
	   p_targetRA    = x->p_targetRA;
	   p_targetDEC   = x->p_currentDEC;
	   p_currentRA   = x->p_currentRA;
	   p_currentDEC  = x->p_currentDEC;

   }
}

bool INDIMountInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
	if (p_deviceName.IsEmpty())
	{
		whyNot = "No device has been specified";
		return false;
	}

	if ( !view.IsMainView() )
	{
		whyNot = "IMDI Mount can only be executed on main views, not on previews.";
		return false;
	}
	FITSKeywordArray K;
	view.Window().GetKeywords( K );
	for ( FITSKeywordArray::iterator i = K.Begin(); i != K.End(); ++i )
		if (i->name == "OBJCTRA" || i->name == "OBJCTDEC")
			return true;

    whyNot = "INDI Mount can only be executed on solved views with OBJCTRA and OBJCTDEC keywords";
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
      m_monitor.SetCallback( &m_status );
      m_commandType = IMCCommandType::Default;
   }

private:

   Console        m_console;
   StandardStatus m_status;
   StatusMonitor  m_monitor;
   pcl_enum		  m_commandType;

   double targetDistance(double targetRA, double currentRA, double targetDEC, double currentDEC)
   {
	   return Abs(targetRA - currentRA) + Abs(targetDEC-currentDEC);
   }

   virtual void StartMountEvent(double targetRA, double currentRA, double targetDEC, double currentDEC, pcl_enum commandType)
   {
	   m_console.EnableAbort();
	   m_commandType = commandType;
	   if (commandType == IMCCommandType::Goto)
		   m_monitor.Initialize( String().Format( "<end><cbr>Telescope is slewing"), RoundInt( targetDistance(targetRA, currentRA, targetDEC, currentDEC)  ) );
	   else if (commandType == IMCCommandType::Park)
		   m_monitor.Initialize( String().Format( "<end><cbr>Telescope is parking"), RoundInt( targetDistance(targetRA, currentRA, targetDEC, currentDEC)  ) );
	   else if (commandType == IMCCommandType::Synch)
	   {
		   double ra_m = Frac(Abs(targetRA-currentRA))*60;
		   double ra_s = Frac(ra_m)*60;
		   double dec_m = Frac(Abs(targetDEC-currentDEC))*60;
		   double dec_s = Frac(dec_m)*60;
		   m_console.WriteLn(String().Format("<end><cbr>Synch: Delta-RA: %d::%d::%.2lf (h::m::s), Delta-DEC: %d::%d::%.2lf (h::m::s) ",
				   TruncInt(Abs(targetRA-currentRA)), TruncInt(ra_m), ra_s, TruncInt(Abs(targetDEC-currentDEC)), TruncInt(dec_m), dec_s ));
	   }
	   else if (commandType == IMCCommandType::MoveNorthStart)
	   {
		   m_console.WriteLn("Start slewing toward North");
	   }
	   else if (commandType == IMCCommandType::MoveSouthStart)
	   {
		   m_console.WriteLn("Start slewing toward South");
	   }
	   else if (commandType == IMCCommandType::MoveWestStart)
	   {
		   m_console.WriteLn("Start slewing toward West");
	   }
	   else if (commandType == IMCCommandType::MoveEastStart)
	   {
		   m_console.WriteLn("Start slewing toward East");
	   }
	   else if (commandType == IMCCommandType::MoveNorthStop || commandType == IMCCommandType::MoveSouthStop || commandType == IMCCommandType::MoveWestStop || commandType == IMCCommandType::MoveEastStop)
	   {
		   m_console.WriteLn("Stop slewing");
	   }
   }

   virtual void MountEvent(double targetRA, double currentRA, double targetDEC, double currentDEC)
   {
	   if (m_commandType == IMCCommandType::Goto || m_commandType == IMCCommandType::Park )
		   m_monitor += m_monitor.Total() - size_type( RoundInt( targetDistance(targetRA, currentRA, targetDEC, currentDEC) ) ) - m_monitor.Count() ;
   }

   virtual void EndMountEvent()
   {
	   m_console.DisableAbort();
	   if (m_commandType == IMCCommandType::Goto || m_commandType == IMCCommandType::Park )
		   m_monitor.Complete();
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
	FITSKeywordArray K;
	view.Window().GetKeywords( K );
	double storedTargetRA=p_targetRA;
	double storedTargetDEC=p_targetDEC;
	pcl_enum storedCommandType=p_commandType;
	for ( FITSKeywordArray::iterator i = K.Begin(); i != K.End(); ++i )
	{
		if (i->name == "OBJCTRA" )
		{
			IsoString valueStr = i->value.Substring(1,i->value.Length()-2);
			CoordUtils::HMS hms=CoordUtils::parse(valueStr," ");
			double centerRA=CoordUtils::convertFromHMS(hms);

			p_targetRA = 2 * p_currentRA - centerRA;
			if (p_targetRA < p_lst)
			{
				MessageBox warningMsg("New center right ascension coordinate crossed the meridian and will possibly trigger a meridian flip. Continue?","",StdIcon::NoIcon,StdButton::Ok,StdButton::Cancel);
				MessageBox::std_button button = warningMsg.Execute();
				if (button == StdButton::Cancel)
					return false;
			}

		}
		if (i->name == "OBJCTDEC" )
		{
			IsoString valueStr = i->value.Substring(1,i->value.Length()-2);
			CoordUtils::HMS hms=CoordUtils::parse(valueStr," ");
			double centerDEC=CoordUtils::convertFromHMS(hms);
			p_targetDEC = 2 * p_currentDEC - centerDEC;
		}
	}
	p_commandType = IMCCommandType::Goto;
	INDIMountInstanceExecution( *this ).Perform();
	// restore original target values;;
	p_targetRA=storedTargetRA;
	p_targetDEC=storedTargetDEC;
	p_commandType=storedCommandType;
	return true;
}

void* INDIMountInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
	if ( p == TheIMCDeviceNameParameter )
		return p_deviceName.Begin();
	if ( p == TheIMCCommandTypeParameter )
		return &p_commandType;
	if ( p == TheIMCSlewRateParameter )
			return &p_slewRate;
	if ( p == TheIMCTargetRightAscensionParameter )
		return &p_targetRA;
	if ( p == TheIMCTargetDeclinationParameter )
		return &p_targetDEC;
	if (p == TheIMCLocalSiderialTimeParameter )
		return &p_lst;
	if ( p == TheIMCCurrentRightAscensionParameter )
		return &p_currentRA;
	if ( p == TheIMCCurrentDeclinationParameter )
		return &p_currentDEC;

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


bool INDIMountInstance::ValidateDevice( bool throwErrors ) const
{
   if ( p_deviceName.IsEmpty() )
   {
      if ( throwErrors )
         throw Error( "No device has been specified" );
      return false;
   }

   if ( !INDIClient::HasClient() )
      throw Error( "The INDI device controller has not been initialized" );

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
	INDIPropertyListItem item;

	if ( indi->GetPropertyItem( p_deviceName, "TELESCOPE_SLEW_RATE", MountSlewRatePropertyString( p_slewRate ), item ) )
	{
		INDINewPropertyItem newItem;
		newItem.Device = p_deviceName;
		newItem.Property = "TELESCOPE_SLEW_RATE";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair(MountSlewRatePropertyString( p_slewRate ),"ON"));
		indi->SendNewPropertyItem( newItem, async );
	}
}


void AbstractINDIMountExecution::GetCurrentCoordinates(const INDIClient* indi)
{
	  INDIPropertyListItem RA_item;
	  INDIPropertyListItem DEC_item;
	  if ( indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "RA", RA_item, false/*formatted*/ )
		&&  indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "DEC", DEC_item, false/*formatted*/ ))
	  {
		  m_instance.p_currentRA = RA_item.PropertyValue.ToDouble();
		  m_instance.p_currentDEC = DEC_item.PropertyValue.ToDouble();
	  } else {
		  throw Error("Cannot get current mount coordinates");
	  }

}

void AbstractINDIMountExecution::Perform()
{
   if ( IsRunning() )
      throw Error( "Internal error: Recursive call to AbstractINDIMountExecution::Perform() detected." );

   if ( m_instance.p_deviceName.IsEmpty() )
      throw Error( "No device has been specified" );

   INDIClient* indi = INDIClient::TheClientOrDie();

   if ( !indi->HasDevices() )
      throw Error( "No INDI device has been connected." );

   try {

	   m_instance.ValidateDevice();
	   m_instance.SendDeviceProperties(false /*async*/);
	   GetCurrentCoordinates(indi);

	   m_running = true;
	   m_aborted = false;
	   switch (m_instance.p_commandType)
	   {
	   case IMCCommandType::Unpark:
	   {
		   INDIPropertyListItem item;
		   if ( indi->GetPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK", "UNPARK", item ) )
		   	{
		   		INDINewPropertyItem newItem;
		   		newItem.Device = m_instance.p_deviceName;
		   		newItem.Property = "TELESCOPE_PARK";
		   		newItem.PropertyType = "INDI_SWITCH";
		   		newItem.ElementValue.Add(ElementValuePair("UNPARK","ON"));
		   		indi->SendNewPropertyItem( newItem, false );
		   	}
	   }
	   break;
	   case IMCCommandType::Goto:
	   {
		   StartMountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC,m_instance.p_commandType);
		   {
			   INDINewPropertyItem newItem;
			   newItem.Device = m_instance.p_deviceName;
			   newItem.Property = "EQUATORIAL_EOD_COORD";
			   newItem.PropertyType = "INDI_NUMBER";
			   newItem.ElementValue.Add(ElementValuePair("RA",String(m_instance.p_targetRA)));
			   newItem.ElementValue.Add(ElementValuePair("DEC",String(m_instance.p_targetDEC)));
			   // send (RA,DEC) coordinates in bulk request
			   indi->SendNewPropertyItem( newItem, true/*async*/ );
		   }

		   for (;;)
		   {
			   Sleep(200);
			   Module->ProcessEvents();
			   INDIPropertyListItem RA_item;
			   INDIPropertyListItem DEC_item;
			   if (     indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "RA", RA_item, false/*formatted*/ )
					   &&  indi->GetPropertyItem( m_instance.p_deviceName, "EQUATORIAL_EOD_COORD", "DEC", DEC_item, false/*formatted*/ ))
				   if ( (RA_item.PropertyState == IPS_BUSY) || (DEC_item.PropertyState == IPS_BUSY)  )
				   {
					   m_instance.p_currentRA = RA_item.PropertyValue.ToDouble();
					   m_instance.p_currentDEC = DEC_item.PropertyValue.ToDouble();
					   MountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC);
				   } else
				   {
					   EndMountEvent();
					   break;
				   }
		   }

	   }
	   break;
	   case IMCCommandType::Park:
	   {
		   StartMountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC,m_instance.p_commandType);
		   {
			   INDINewPropertyItem newItem;
			   newItem.Device = m_instance.p_deviceName;
			   newItem.Property = "TELESCOPE_PARK";
			   newItem.PropertyType = "INDI_SWITCH";
			   newItem.ElementValue.Add(ElementValuePair("PARK","ON"));
			   // send (RA,DEC) coordinates in bulk request
			   indi->SendNewPropertyItem( newItem, true/*async*/  );
		   }
		   for (;;)
		   {
			   Sleep(200);
			   Module->ProcessEvents();
			   INDIPropertyListItem item;
			   if ( indi->GetPropertyItem( m_instance.p_deviceName, "TELESCOPE_PARK", "PARK", item, false/*formatted*/ ))
				   if ( item.PropertyState == IPS_BUSY )
				   {
					   GetCurrentCoordinates(indi);
					   MountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC);
				   } else
				   {
					   EndMountEvent();
					   break;
				   }
		   }

	   }
	   break;
	   case IMCCommandType::Synch:
	   {
		   StartMountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC,m_instance.p_commandType);

		   INDINewPropertyItem newItem;
		   newItem.Device = m_instance.p_deviceName;
		   newItem.Property = "ON_COORD_SET";
		   newItem.PropertyType = "INDI_SWITCH";
		   newItem.ElementValue.Add(ElementValuePair("SYNC","ON"));

		   indi->SendNewPropertyItem( newItem, false );
		   newItem.ElementValue.Clear();

		   newItem.Property = "EQUATORIAL_EOD_COORD";
		   newItem.PropertyType = "INDI_NUMBER";
		   newItem.ElementValue.Add(ElementValuePair("RA",String(m_instance.p_targetRA)));
		   newItem.ElementValue.Add(ElementValuePair("DEC",String(m_instance.p_targetDEC)));


		   // send (RA,DEC) coordinates in propertyVector
		   indi->SendNewPropertyItem( newItem, false );
		   newItem.ElementValue.Clear();

		   newItem.Property = "ON_COORD_SET";
		   newItem.PropertyType = "INDI_SWITCH";
		   newItem.ElementValue.Add(ElementValuePair("TRACK","ON"));
		   indi->SendNewPropertyItem( newItem, false );

		   GetCurrentCoordinates(indi);
		   EndMountEvent();
	   }
	   break;
	   case IMCCommandType::MoveNorthStart:
	   case IMCCommandType::MoveNorthStop:
	   {
		   StartMountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC,m_instance.p_commandType);

		   INDINewPropertyItem newItem;
		   newItem.Device = m_instance.p_deviceName;
		   newItem.Property = "TELESCOPE_MOTION_NS";
		   newItem.PropertyType = "INDI_SWITCH";
		   if (m_instance.p_commandType == IMCCommandType::MoveNorthStart)
			   newItem.ElementValue.Add(ElementValuePair("MOTION_NORTH","ON"));
		   else
			   newItem.ElementValue.Add(ElementValuePair("MOTION_NORTH","OFF"));

		   indi->SendNewPropertyItem( newItem, true /*async*/  );

		   GetCurrentCoordinates(indi);
		   EndMountEvent();
	   }
	   break;
	   case IMCCommandType::MoveSouthStart:
	   case IMCCommandType::MoveSouthStop:
	   {
		   StartMountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC,m_instance.p_commandType);

		   INDINewPropertyItem newItem;
		   newItem.Device = m_instance.p_deviceName;
		   newItem.Property = "TELESCOPE_MOTION_NS";
		   newItem.PropertyType = "INDI_SWITCH";
		   if (m_instance.p_commandType == IMCCommandType::MoveSouthStart)
			   newItem.ElementValue.Add(ElementValuePair("MOTION_SOUTH","ON"));
		   else
			   newItem.ElementValue.Add(ElementValuePair("MOTION_SOUTH","OFF"));

		   indi->SendNewPropertyItem( newItem, true /*async*/  );

		   GetCurrentCoordinates(indi);
		   EndMountEvent();
	   }
	   break;
	   case IMCCommandType::MoveWestStart:
	   case IMCCommandType::MoveWestStop:
	   {
		   StartMountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC,m_instance.p_commandType);

		   INDINewPropertyItem newItem;
		   newItem.Device = m_instance.p_deviceName;
		   newItem.Property = "TELESCOPE_MOTION_WE";
		   newItem.PropertyType = "INDI_SWITCH";
		   if (m_instance.p_commandType == IMCCommandType::MoveWestStart)
			   newItem.ElementValue.Add(ElementValuePair("MOTION_WEST","ON"));
		   else
			   newItem.ElementValue.Add(ElementValuePair("MOTION_WEST","OFF"));

		   indi->SendNewPropertyItem( newItem, true /*async*/  );

		   GetCurrentCoordinates(indi);
		   EndMountEvent();
	   }
	   break;
	   case IMCCommandType::MoveEastStart:
	   case IMCCommandType::MoveEastStop:
	   {
		   StartMountEvent(m_instance.p_targetRA, m_instance.p_currentRA, m_instance.p_targetDEC, m_instance.p_currentDEC,m_instance.p_commandType);

		   INDINewPropertyItem newItem;
		   newItem.Device = m_instance.p_deviceName;
		   newItem.Property = "TELESCOPE_MOTION_WE";
		   newItem.PropertyType = "INDI_SWITCH";
		   if (m_instance.p_commandType == IMCCommandType::MoveEastStart)
			   newItem.ElementValue.Add(ElementValuePair("MOTION_EAST","ON"));
		   else
			   newItem.ElementValue.Add(ElementValuePair("MOTION_EAST","OFF"));

		   indi->SendNewPropertyItem( newItem, true /*async*/  );

		   GetCurrentCoordinates(indi);
		   EndMountEvent();
	   }
	   break;
	   default:
          throw Error("Unknown INDI Mount command type");
	   }
   } catch ( ... )
   {
      m_running = false;

      try
      {
    	  throw;
      }
      catch ( ProcessAborted& )
      {
    	  m_aborted = true;
    	  INDIPropertyListItem item;
    	  // check if driver supports TELESCOPE_ABORT_MOTION
    	  if ( indi->GetPropertyItem( m_instance.p_deviceName, "TELESCOPE_ABORT_MOTION", "ABORT", item ) )
    	  {
    		  INDINewPropertyItem newItem;

    		  newItem.Device = m_instance.p_deviceName;
    		  newItem.Property = "TELESCOPE_ABORT_MOTION";
    		  newItem.PropertyType = "INDI_SWITCH";
    		  newItem.ElementValue.Add(ElementValuePair("ABORT","ON"));
    		  indi->SendNewPropertyItem( newItem, true/*async*/ );
    	  }
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



// ---------------------------------------------------------------------------


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountInstance.cpp - Released 2016/04/28 15:13:36 UTC
