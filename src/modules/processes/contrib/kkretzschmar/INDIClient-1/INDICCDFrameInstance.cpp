//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDICCDFrameInstance.cpp - Released 2016/04/28 15:13:36 UTC
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

#include "INDICCDFrameInstance.h"
#include "INDICCDFrameParameters.h"
#include "INDIClient.h"

#include <pcl/Console.h>
#include <pcl/ElapsedTime.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/ICCProfile.h>
#include <pcl/ImageStatistics.h>
#include <pcl/MetaModule.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>

#include <time.h>

namespace pcl
{

// ----------------------------------------------------------------------------

INDICCDFrameInstance::INDICCDFrameInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_deviceName( TheICFDeviceNameParameter->DefaultValue() ),
   p_uploadMode( ICFUploadMode::Default ),
   p_serverUploadDirectory( TheICFServerUploadDirectoryParameter->DefaultValue() ),
   p_serverFileNameTemplate( TheICFServerFileNameTemplateParameter->DefaultValue() ),
   p_frameType( ICFFrameType::Default ),
   p_binningX( int32( TheICFBinningXParameter->DefaultValue() ) ),
   p_binningY( int32( TheICFBinningYParameter->DefaultValue() ) ),
   p_exposureTime( TheICFExposureTimeParameter->DefaultValue() ),
   p_exposureDelay( TheICFExposureDelayParameter->DefaultValue() ),
   p_exposureCount( TheICFExposureCountParameter->DefaultValue() ),
   p_newImageIdTemplate( TheICFNewImageIdTemplateParameter->DefaultValue() ),
   p_reuseImageWindow( TheICFReuseImageWindowParameter->DefaultValue() ),
   p_autoStretch( TheICFAutoStretchParameter->DefaultValue() ),
   p_linkedAutoStretch( TheICFLinkedAutoStretchParameter->DefaultValue() ),
   o_clientFrames(),
   o_serverFrames(),
   m_exposureNumber( 0 )
{
}

INDICCDFrameInstance::INDICCDFrameInstance( const INDICCDFrameInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void INDICCDFrameInstance::Assign( const ProcessImplementation& p )
{
   const INDICCDFrameInstance* x = dynamic_cast<const INDICCDFrameInstance*>( &p );
   if ( x != nullptr )
   {
      p_deviceName             = x->p_deviceName;
      p_uploadMode             = x->p_uploadMode;
      p_serverUploadDirectory  = x->p_serverUploadDirectory;
      p_serverFileNameTemplate = x->p_serverFileNameTemplate;
      p_frameType              = x->p_frameType;
      p_binningX               = x->p_binningX;
      p_binningY               = x->p_binningY;
      p_exposureTime           = x->p_exposureTime;
      p_exposureDelay          = x->p_exposureDelay;
      p_exposureCount          = x->p_exposureCount;
      p_newImageIdTemplate     = x->p_newImageIdTemplate;
      p_reuseImageWindow       = x->p_reuseImageWindow;
      p_autoStretch            = x->p_autoStretch;
      p_linkedAutoStretch      = x->p_linkedAutoStretch;
      o_clientFrames           = x->o_clientFrames;
      o_serverFrames           = x->o_serverFrames;
   }
}

// ----------------------------------------------------------------------------

bool INDICCDFrameInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "INDICCDFrame can only be executed in the global context.";
   return false;
}

bool INDICCDFrameInstance::CanExecuteGlobal( String& whyNot ) const
{
   if ( p_deviceName.IsEmpty() )
   {
      whyNot = "No device has been specified";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool INDICCDFrameInstance::ValidateDevice( bool throwErrors ) const
{
   if ( p_deviceName.IsEmpty() )
   {
      if ( throwErrors )
         throw Error( "No device has been specified" );
      return false;
   }

   if ( !INDIClient::HasClient() )
      throw Error( "The INDI device controller has not been initialized" );

   INDIDeviceControllerInstance* instance = dynamic_cast<INDIDeviceControllerInstance*>( INDIClient::TheClient()->DeviceControllerInstance() );
   if ( instance == nullptr )
   {
      if ( throwErrors )
         throw Error( "Internal error: Invalid INDI device controller instance" );
      return false;
   }

   for ( auto device : instance->o_devices )
      if ( device.DeviceName == p_deviceName )
      {
         INDIPropertyListItem CCDProp;
         if ( !instance->getINDIPropertyItem( device.DeviceName, "CCD_FRAME", "WIDTH", CCDProp, false/*formatted*/ ) ) // is this a camera device?
         {
            if ( throwErrors )
               throw Error( '\'' + p_deviceName + "' does not seem to be a valid INDI CCD device" );
            return false;
         }
         return true;
      }

   if ( throwErrors )
      throw Error( "INDI device not available: '" + p_deviceName + "'" );
   return false;
}

void INDICCDFrameInstance::SendDeviceProperties( bool asynchronous ) const
{
   if ( !INDIClient::HasClient() )
      throw Error( "The INDI device controller has not been initialized" );

   INDIDeviceControllerInstance* instance = dynamic_cast<INDIDeviceControllerInstance*>( INDIClient::TheClient()->DeviceControllerInstance() );
   if ( instance == nullptr )
      throw Error( "Internal error: Invalid INDI device controller instance" );

   INDIPropertyListItem CCDProp;
   INDINewPropertyListItem newPropertyListItem;
   newPropertyListItem.Device = p_deviceName;

   if ( instance->getINDIPropertyItem( p_deviceName, "UPLOAD_MODE", UploadModePropertyString( p_uploadMode ), CCDProp ) )
   {
      newPropertyListItem.Property = "UPLOAD_MODE";
      newPropertyListItem.Element = UploadModePropertyString( p_uploadMode );
      newPropertyListItem.PropertyType = "INDI_SWITCH";
      newPropertyListItem.NewPropertyValue = "ON";
      instance->sendNewPropertyValue( newPropertyListItem, asynchronous );
   }

   if ( !p_serverUploadDirectory.IsEmpty() )
   {
      newPropertyListItem.Property = "UPLOAD_SETTINGS";
      newPropertyListItem.Element = "UPLOAD_DIR";
      newPropertyListItem.PropertyType = "INDI_TEXT";
      newPropertyListItem.NewPropertyValue = p_serverUploadDirectory;
      instance->sendNewPropertyValue( newPropertyListItem, asynchronous );
   }

   if ( instance->getINDIPropertyItem( p_deviceName, "CCD_FRAME_TYPE", CCDFrameTypePropertyString( p_frameType ), CCDProp ) )
   {
      newPropertyListItem.Property = "CCD_FRAME_TYPE";
      newPropertyListItem.Element = CCDFrameTypePropertyString( p_frameType );
      newPropertyListItem.PropertyType = "INDI_SWITCH";
      newPropertyListItem.NewPropertyValue = "ON";
      instance->sendNewPropertyValue( newPropertyListItem, asynchronous );
   }

   if ( instance->getINDIPropertyItem( p_deviceName, "CCD_BINNING", "HOR_BIN", CCDProp ) )
   {
      newPropertyListItem.Property = "CCD_BINNING";
      newPropertyListItem.Element = "HOR_BIN";
      newPropertyListItem.PropertyType = "INDI_NUMBER";
      newPropertyListItem.NewPropertyValue = String( p_binningX );
      instance->sendNewPropertyValue( newPropertyListItem, asynchronous );
   }

   if ( instance->getINDIPropertyItem( p_deviceName, "CCD_BINNING", "VER_BIN", CCDProp ) )
   {
      newPropertyListItem.Property = "CCD_BINNING";
      newPropertyListItem.Element = "VER_BIN";
      newPropertyListItem.PropertyType = "INDI_NUMBER";
      newPropertyListItem.NewPropertyValue = String( p_binningY );
      instance->sendNewPropertyValue( newPropertyListItem, asynchronous );
   }
}

String INDICCDFrameInstance::ServerFileName( const String& fileNameTemplate ) const
{
   if ( !INDIClient::HasClient() )
      throw Error( "The INDI device controller has not been initialized" );

   INDIDeviceControllerInstance* instance = dynamic_cast<INDIDeviceControllerInstance*>( INDIClient::TheClient()->DeviceControllerInstance() );
   if ( instance == nullptr )
      throw Error( "Internal error: Invalid INDI device controller instance" );
   INDIPropertyListItem CCDProp;

   String fileName;
   for ( String::const_iterator i = fileNameTemplate.Begin(); i < fileNameTemplate.End(); ++i )
      switch ( *i )
      {
      case '%':
         if ( ++i < fileNameTemplate.End() )
            switch ( *i )
            {
            case 'f':
               fileName << CCDFrameTypePrefix( p_frameType );
               break;
            case 'b':
               fileName << String().Format( "%dx%d", p_binningX, p_binningY );
               break;
            case 'e':
               fileName << String().Format( "%.3lf", p_exposureTime );
               break;
            case 'F':
               if ( instance->getINDIPropertyItem( p_deviceName, "FILTER_SLOT", "FILTER_SLOT_VALUE", CCDProp ) )
                  if ( instance->getINDIPropertyItem( p_deviceName, "FILTER_NAME", "FILTER_SLOT_NAME_" + CCDProp.PropertyValue, CCDProp ) )
                     fileName << CCDProp.PropertyValue;
               break;
            case 'T':
               if ( instance->getINDIPropertyItem( p_deviceName, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", CCDProp ) )
                  fileName << String().Format( "%+.2lf", CCDProp.PropertyValue.ToDouble() );
               break;
            case 't':
            case 'd':
               {
                  time_t t0 = ::time( nullptr );
                  const tm* t = ::gmtime( &t0 );
                  fileName << String().Format( "%d-%02d-%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday );
                  if ( *i == 't' )
                     fileName << String().Format( "T%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec );
               }
               break;
            case 'n':
               fileName << String().Format( "%03d", m_exposureNumber+1 );
               break;
            case 'u':
               fileName << IsoString::UUID();
               break;
            case '%':
               fileName << '%';
               break;
            default:
               break;
            }
         break;

      default:
         fileName << *i;
         break;
      }

   fileName.Trim();
   if ( !fileName.IsEmpty() )
      return fileName;

   return ServerFileName( TheICFServerFileNameTemplateParameter->DefaultValue() );
}

class INDICCDFrameInstanceExecution : public AbstractINDICCDFrameExecution
{
public:

   INDICCDFrameInstanceExecution( INDICCDFrameInstance& instance ) :
      AbstractINDICCDFrameExecution( instance )
   {
      m_monitor.SetCallback( &m_status );
      m_waitMonitor.SetCallback( &m_spin );
   }

private:

   Console        m_console;
   StandardStatus m_status;
   StatusMonitor  m_monitor;
   SpinStatus     m_spin;
   StatusMonitor  m_waitMonitor;

   virtual void StartAcquisitionEvent()
   {
      m_console.EnableAbort();
   }

   virtual void NewExposureEvent( int expNum, int expCount )
   {
   }

   virtual void StartExposureDelayEvent( double totalDelayTime )
   {
      m_monitor.Initialize( String().Format( "Waiting for %.3gs", totalDelayTime ), RoundInt( totalDelayTime*100 ) );
   }

   virtual void ExposureDelayEvent( double delayTime )
   {
      m_monitor += size_type( RoundInt( delayTime*100 ) ) - m_monitor.Count();
   }

   virtual void EndExposureDelayEvent()
   {
      m_monitor.Complete();
   }

   virtual void StartExposureEvent( int expNum, int expCount, double expTime )
   {
      if ( m_waitMonitor.IsInitialized() )
         m_waitMonitor.Complete();
      m_monitor.Initialize( String().Format( "<end><cbr>Exposure %d of %d (%.3gs)", expNum+1, expCount, expTime ), RoundInt( expTime*100 ) );
   }

   virtual void ExposureEvent( int expNum, int expCount, double expTime )
   {
      m_monitor += size_type( RoundInt( expTime*100 ) ) - m_monitor.Count();
   }

   virtual void ExposureErrorEvent( const String& errMsg )
   {
      m_console.CriticalLn( "<end><cbr>*** Error: " + errMsg + '.' );
   }

   virtual void EndExposureEvent( int expNum )
   {
      m_monitor.Complete();

      // Print latest INDI server message
      if ( DeviceControllerInstance().Verbosity() > 1 )
         if ( !DeviceControllerInstance().CurrentServerMessage().IsEmpty() )
         {
            m_console.NoteLn( "<end><cbr><br>* Latest INDI server log entry:" );
            m_console.NoteLn( DeviceControllerInstance().CurrentServerMessage() );
            m_console.WriteLn();
         }
   }

   virtual void WaitingForServerEvent()
   {
      if ( m_waitMonitor.IsInitialized() )
         ++m_waitMonitor;
      else
         m_waitMonitor.Initialize( "Waiting for INDI server" );
   }

   virtual void NewFrameEvent( ImageWindow& window, bool reusedWindow )
   {
      if ( reusedWindow )
         window.Regenerate();
      else
      {
         window.BringToFront();
         window.Show();
         window.ZoomToFit( false/*allowZoom*/ );
      }
   }

   virtual void EndAcquisitionEvent()
   {
      m_console.DisableAbort();
      m_console.NoteLn( String().Format( "<end><cbr><br>===== INDICCDFrame: %d succeeded, %d failed =====",
                                       SuccessCount(), ErrorCount() ) );
   }

   virtual void AbortEvent()
   {
   }
};

bool INDICCDFrameInstance::ExecuteGlobal()
{
   INDICCDFrameInstanceExecution( *this ).Perform();
   return true;
}

void* INDICCDFrameInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICFDeviceNameParameter )
      return p_deviceName.Begin();
   if ( p == TheICFUploadModeParameter )
      return &p_uploadMode;
   if ( p == TheICFServerUploadDirectoryParameter )
      return p_serverUploadDirectory.Begin();
   if ( p == TheICFServerFileNameTemplateParameter )
      return p_serverFileNameTemplate.Begin();
   if ( p == TheICFFrameTypeParameter )
      return &p_frameType;
   if ( p == TheICFBinningXParameter )
      return &p_binningX;
   if ( p == TheICFBinningYParameter )
      return &p_binningY;
   if ( p == TheICFExposureTimeParameter )
      return &p_exposureTime;
   if ( p == TheICFExposureDelayParameter )
      return &p_exposureDelay;
   if ( p == TheICFExposureCountParameter )
      return &p_exposureCount;
   if ( p == TheICFNewImageIdTemplateParameter )
      return p_newImageIdTemplate.Begin();
   if ( p == TheICFReuseImageWindowParameter )
      return &p_reuseImageWindow;
   if ( p == TheICFAutoStretchParameter )
      return &p_autoStretch;
   if ( p == TheICFLinkedAutoStretchParameter )
      return &p_linkedAutoStretch;

   if ( p == TheICFClientFrameParameter )
      return o_clientFrames[tableRow].Begin();
   if ( p == TheICFServerFrameParameter )
      return o_serverFrames[tableRow].Begin();

   return nullptr;
}

bool INDICCDFrameInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheICFDeviceNameParameter )
   {
      p_deviceName.Clear();
      if ( sizeOrLength > 0 )
         p_deviceName.SetLength( sizeOrLength );
   }
   else if ( p == TheICFServerUploadDirectoryParameter )
   {
      p_serverUploadDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_serverUploadDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheICFServerFileNameTemplateParameter )
   {
      p_serverFileNameTemplate.Clear();
      if ( sizeOrLength > 0 )
         p_serverFileNameTemplate.SetLength( sizeOrLength );
   }
   else if ( p == TheICFNewImageIdTemplateParameter )
   {
      p_newImageIdTemplate.Clear();
      if ( sizeOrLength > 0 )
         p_newImageIdTemplate.SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientFramesParameter )
   {
      o_clientFrames.Clear();
      if ( sizeOrLength > 0 )
         o_clientFrames.Add( String(), sizeOrLength );
   }
   else if ( p == TheICFClientFrameParameter )
   {
      o_clientFrames[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_clientFrames[tableRow].SetLength( sizeOrLength );
   }
   else if ( p == TheICFServerFramesParameter )
   {
      o_serverFrames.Clear();
      if ( sizeOrLength > 0 )
         o_serverFrames.Add( String(), sizeOrLength );
   }
   else if ( p == TheICFServerFrameParameter )
   {
      o_serverFrames[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_serverFrames[tableRow].SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type INDICCDFrameInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheICFDeviceNameParameter )
      return p_deviceName.Length();
   if ( p == TheICFServerUploadDirectoryParameter )
      return p_serverUploadDirectory.Length();
   if ( p == TheICFServerFileNameTemplateParameter )
      return p_serverFileNameTemplate.Length();
   if ( p == TheICFNewImageIdTemplateParameter )
      return p_newImageIdTemplate.Length();

   if ( p == TheICFClientFramesParameter )
      return o_clientFrames.Length();
   if ( p == TheICFClientFrameParameter )
      return o_clientFrames[tableRow].Length();
   if ( p == TheICFServerFramesParameter )
      return o_serverFrames.Length();
   if ( p == TheICFServerFrameParameter )
      return o_serverFrames[tableRow].Length();

   return 0;
}

String INDICCDFrameInstance::UploadModePropertyString( int uploadModeIdx )
{
   switch ( uploadModeIdx )
   {
   case ICFUploadMode::UploadClient:
      return "UPLOAD_CLIENT";
   case ICFUploadMode::UploadServer:
      return "UPLOAD_LOCAL";
   case ICFUploadMode::UploadServerAndClient:
      return "UPLOAD_BOTH";
   default:
      throw Error( "Internal error: INDICCDFrameInstance: Invalid upload mode." );
   }
}

String INDICCDFrameInstance::CCDFrameTypePropertyString( int frameTypeIdx )
{
   switch ( frameTypeIdx )
   {
   case ICFFrameType::LightFrame:
      return "FRAME_LIGHT";
   case ICFFrameType::BiasFrame:
      return "FRAME_BIAS";
   case ICFFrameType::DarkFrame:
      return "FRAME_DARK";
   case ICFFrameType::FlatFrame:
      return "FRAME_FLAT";
   default:
      throw Error( "Internal error: INDICCDFrameInstance: Invalid frame type." );
   }
}

String INDICCDFrameInstance::CCDFrameTypePrefix( int frameTypeIdx )
{
   switch ( frameTypeIdx )
   {
   case ICFFrameType::LightFrame:
      return "LIGHT";
   case ICFFrameType::BiasFrame:
      return "BIAS";
   case ICFFrameType::DarkFrame:
      return "DARK";
   case ICFFrameType::FlatFrame:
      return "FLAT";
   default:
      throw Error( "Internal error: INDICCDFrameInstance: Invalid frame type." );
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

INDIDeviceControllerInstance& AbstractINDICCDFrameExecution::DeviceControllerInstance() const
{
   if ( !INDIClient::HasClient() )
      throw Error( "The INDI device controller has not been initialized" );

   INDIDeviceControllerInstance* instance = dynamic_cast<INDIDeviceControllerInstance*>( INDIClient::TheClient()->DeviceControllerInstance() );
   if ( instance == nullptr )
      throw Error( "Internal error: Invalid INDI device controller instance" );
   return *instance;
}

void AbstractINDICCDFrameExecution::Perform()
{
   m_instance.o_clientFrames.Clear();
   m_instance.o_serverFrames.Clear();

   if ( IsRunning() )
      throw Error( "Internal error: Recursive call to AbstractINDICCDFrameExecution::Perform() detected." );

   if ( m_instance.p_deviceName.IsEmpty() )
      throw Error( "No device has been specified" );

   INDIDeviceControllerInstance& instance = DeviceControllerInstance();
   if ( instance.o_devices.IsEmpty() )
      throw Error( "No INDI device has been connected." );

   INDINewPropertyListItem newPropertyListItem;
   newPropertyListItem.Device = m_instance.p_deviceName;

   try
   {
      m_instance.ValidateDevice();

      m_instance.SendDeviceProperties( false/*asynchronous*/ );

      INDIPropertyListItem CCDProp;
      bool serverSendsImage = false;
      bool serverKeepsImage = false;
      if ( instance.getINDIPropertyItem( m_instance.p_deviceName, "UPLOAD_MODE", "UPLOAD_LOCAL", CCDProp ) )
         if ( CCDProp.PropertyValue == "ON" )
            serverKeepsImage = true;
         else
         {
            serverSendsImage = true;
            if ( instance.getINDIPropertyItem( m_instance.p_deviceName, "UPLOAD_MODE", "UPLOAD_BOTH", CCDProp ) )
               if ( CCDProp.PropertyValue == "ON" )
                  serverKeepsImage = true;
         }

      m_running = true;
      m_aborted = false;
      m_successCount = m_errorCount = 0;

      StartAcquisitionEvent();

      for ( m_instance.m_exposureNumber = 0; m_instance.m_exposureNumber < m_instance.p_exposureCount; ++m_instance.m_exposureNumber )
      {
         NewExposureEvent( m_instance.m_exposureNumber, m_instance.p_exposureCount );

         if ( m_instance.m_exposureNumber > 0 )
            if ( m_instance.p_exposureDelay > 0 )
            {
               StartExposureDelayEvent( m_instance.p_exposureDelay );

               for ( ElapsedTime T; T() < m_instance.p_exposureDelay; )
                  ExposureDelayEvent( T() );

               EndExposureDelayEvent();
            }

         instance.ResetDownloadedImage();

         if ( serverKeepsImage )
         {
            String serverFileName = m_instance.ServerFileName();
            m_instance.o_serverFrames << serverFileName;

            newPropertyListItem.Property = "UPLOAD_SETTINGS";
            newPropertyListItem.Element = "UPLOAD_PREFIX";
            newPropertyListItem.PropertyType = "INDI_TEXT";
            newPropertyListItem.NewPropertyValue = serverFileName;
            instance.sendNewPropertyValue( newPropertyListItem, false/*async*/ );
         }

         newPropertyListItem.Property = "CCD_EXPOSURE";
         newPropertyListItem.Element = "CCD_EXPOSURE_VALUE";
         newPropertyListItem.PropertyType = "INDI_NUMBER";
         newPropertyListItem.NewPropertyValue = String( m_instance.p_exposureTime );
         if ( !instance.sendNewPropertyValue( newPropertyListItem, true/*async*/ ) )
         {
            ExposureErrorEvent( "Failure to send new property values to INDI server" );
            ++m_errorCount;
            continue; // ### TODO: Implement a p_onError process parameter
         }

         ElapsedTime T;

         for ( bool inExposure = false; ; )
         {
            if ( instance.getInternalAbortFlag() )
               throw ProcessAborted();

            if ( instance.getINDIPropertyItem( m_instance.p_deviceName, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", CCDProp, false/*formatted*/ ) )
               if ( CCDProp.PropertyState == IPS_BUSY )
               {
                  if ( inExposure )
                  {
                     // Exposure running
                     double t = 0;
                     CCDProp.PropertyValue.TryToDouble( t );
                     ExposureEvent( m_instance.m_exposureNumber, m_instance.p_exposureCount, m_instance.p_exposureTime - t ); // CCD_EXPOSURE_VALUE is the remaining exp. time
                  }
                  else
                  {
                     // Exposure started
                     inExposure = true;
                     StartExposureEvent( m_instance.m_exposureNumber, m_instance.p_exposureCount, m_instance.p_exposureTime );
                  }
               }
               else
               {
                  if ( inExposure )
                  {
                     // Exposure completed
                     EndExposureEvent( m_instance.m_exposureNumber );
                     ++m_successCount;
                     break;
                  }

                  if ( T() > 1 )
                     WaitingForServerEvent();
               }
         }

         if ( serverSendsImage )
         {
            for ( T.Reset(); !instance.HasDownloadedImage(); )
               if ( T() > 1 )
                     WaitingForServerEvent();

            String filePath = instance.DownloadedImagePath();
            FileFormat format( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );
            FileFormatInstance file( format );

            ImageDescriptionArray images;
            if ( !file.Open( images, filePath, "raw cfa verbosity 0 up-bottom signed-is-physical" ) )
               throw CatchedException();
            if ( images.IsEmpty() )
               throw Error( filePath + ": Empty image file." );
            if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
               throw Error( filePath + ": Invalid or unsupported image." );

            ImageWindow window;
            if ( m_instance.p_reuseImageWindow )
            {
               window = ImageWindow::WindowById( m_instance.p_newImageIdTemplate );
               if ( !window.IsNull() ) // Make sure our window is valid and has not been write-locked by other task.
                  if ( !window.MainView().CanWrite() )
                     window = ImageWindow::Null();
            }
            bool reusedWindow = !window.IsNull();
            if ( !reusedWindow )
               window = ImageWindow( 1, 1, 1,
                                     images[0].options.bitsPerSample,
                                     images[0].options.ieeefpSampleFormat,
                                     false/*color*/,
                                     true/*initialProcessing*/,
                                     m_instance.p_newImageIdTemplate );

            m_instance.o_clientFrames << String( window.MainView().Id() );

            ImageVariant image = window.MainView().Image();
            if ( !file.ReadImage( image ) )
               throw CatchedException();

            if ( format.CanStoreProperties() )
            {
               View view = window.MainView();
               PropertyDescriptionArray properties = file.Properties();
               for ( auto property : properties )
               {
                  Variant value = file.ReadProperty( property.id );
                  if ( value.IsValid() )
                     view.SetPropertyValue( property.id, value, false/*notify*/,
                                          ViewPropertyAttribute::Storable|ViewPropertyAttribute::Permanent );

               }
            }

            if ( format.CanStoreKeywords() )
            {
               FITSKeywordArray keywords;
               if ( !file.Extract( keywords ) )
                  throw CatchedException();
               window.SetKeywords( keywords );
            }

            if ( format.CanStoreResolution() )
               window.SetResolution( images[0].options.xResolution, images[0].options.yResolution, images[0].options.metricResolution );

            if ( format.CanStoreICCProfiles() )
            {
               ICCProfile icc;
               if ( file.Extract( icc ) )
                  if ( icc )
                     window.SetICCProfile( icc );
            }

            if ( format.CanStoreRGBWS() )
            {
               RGBColorSystem rgbws;
               if ( file.ReadRGBWorkingSpace( rgbws ) )
                  window.SetRGBWS( rgbws );
            }

            if ( format.CanStoreColorFilterArrays() )
            {
               // ### TODO - Cannot be implemented until the core provides support for CFAs
            }

            if ( m_instance.p_autoStretch )
               AutoStretch( window );

            NewFrameEvent( window, reusedWindow );
         }
      }

      instance.setInternalAbortFlag( false );
      instance.ResetDownloadedImage();

      m_running = false;

      EndAcquisitionEvent();
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
         newPropertyListItem.Property = "CCD_ABORT_EXPOSURE";
         newPropertyListItem.Element = "ABORT";
         newPropertyListItem.PropertyType = "INDI_SWITCH";
         newPropertyListItem.NewPropertyValue = "ON";
         instance.sendNewPropertyValue( newPropertyListItem, true/*async*/ );
         AbortEvent();
         throw;
      }
      catch ( ... )
      {
         throw;
      }
   }
}

void AbstractINDICCDFrameExecution::Abort()
{
   if ( IsRunning() )
      throw ProcessAborted();
}

void AbstractINDICCDFrameExecution::AutoStretch( ImageWindow& window ) const
{
   /*
    * AutoStretch parameters
    */
   static const double s_shadowsClipping = -2.80 * 1.4826; // in MAD units from the median
   static const double s_targetBackground = 0.25;

   ImageVariant image = window.MainView().Image();
   Array<ImageStatistics> S;
   for( int c = 0; c < image->NumberOfNominalChannels(); ++c )
   {
      image->SelectChannel( c );
      ImageStatistics s;
      s.EnableRejection();
      s.SetRejectionLimits( 0.0, 1.0 );
      s.DisableSumOfSquares();
      s.DisableBWMV();
      s.DisablePBMV();
      s << image;
      S << s;
   }

   int n = image->NumberOfNominalChannels();
   double c0 = 0, m = 0;

   View::stf_list stf( 4 );

   if ( n == 1 || m_instance.p_linkedAutoStretch )
   {
      for ( int c = 0; c < n; c++ )
      {
         if ( 1 + S[c].MAD() != 1 )
            c0 += S[c].Median() + s_shadowsClipping * S[c].MAD();
         m += S[c].Median();
      }
      c0 = Range( c0/n, 0.0, 1.0 );
      m = HistogramTransformation::MTF( s_targetBackground, m/n - c0 );
      for ( int i = 0; i < 3; i++ )
      {
         stf[i].SetMidtonesBalance( m );
         stf[i].SetClipping( c0, 1 );
         stf[i].SetRange( 0, 1 );
      }
   }
   else
   {
      for ( int c = 0; c < n; c++ )
      {
         c0 = (1 +  S[c].MAD() != 1) ? Range( S[c].Median() + s_shadowsClipping * S[c].MAD(), 0.0, 1.0 ) : 0.0;
         m = HistogramTransformation::MTF( s_targetBackground, S[c].Median() - c0 );
         stf[c].SetMidtonesBalance( m );
         stf[c].SetClipping( c0, 1 );
         stf[c].SetRange( 0, 1 );
      }
   }

   stf[3].SetMidtonesBalance( 0.5 );
   stf[3].SetClipping( 0, 1 );
   stf[3].SetRange( 0, 1 );

   window.MainView().SetScreenTransferFunctions( stf );
   window.MainView().EnableScreenTransferFunctions();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInstance.cpp - Released 2016/04/28 15:13:36 UTC
