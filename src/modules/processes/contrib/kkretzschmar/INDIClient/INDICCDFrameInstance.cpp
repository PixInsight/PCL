//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.10.0163
// ----------------------------------------------------------------------------
// INDICCDFrameInstance.cpp - Released 2016/05/17 15:40:50 UTC
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

#include <pcl/ColorFilterArray.h>
#include <pcl/Console.h>
#include <pcl/DisplayFunction.h>
#include <pcl/ElapsedTime.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfile.h>
#include <pcl/MetaModule.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>
#include <pcl/Version.h>

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
   p_filterSlot( int32( TheICFFilterSlotParameter->DefaultValue() ) ),
   p_exposureTime( TheICFExposureTimeParameter->DefaultValue() ),
   p_exposureDelay( TheICFExposureDelayParameter->DefaultValue() ),
   p_exposureCount( TheICFExposureCountParameter->DefaultValue() ),
   p_openClientImages( TheICFOpenClientImagesParameter->DefaultValue() ),
   p_newImageIdTemplate( TheICFNewImageIdTemplateParameter->DefaultValue() ),
   p_reuseImageWindow( TheICFReuseImageWindowParameter->DefaultValue() ),
   p_autoStretch( TheICFAutoStretchParameter->DefaultValue() ),
   p_linkedAutoStretch( TheICFLinkedAutoStretchParameter->DefaultValue() ),
   p_saveClientImages( TheICFSaveClientImagesParameter->DefaultValue() ),
   p_overwriteClientImages( TheICFOverwriteClientImagesParameter->DefaultValue() ),
   p_clientDownloadDirectory( TheICFClientDownloadDirectoryParameter->DefaultValue() ),
   p_clientFileNameTemplate( TheICFClientFileNameTemplateParameter->DefaultValue() ),
   p_clientOutputFormatHints( TheICFClientOutputFormatHintsParameter->DefaultValue() ),

   o_clientViewIds(),
   o_clientFilePaths(),
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
      p_deviceName              = x->p_deviceName;
      p_uploadMode              = x->p_uploadMode;
      p_serverUploadDirectory   = x->p_serverUploadDirectory;
      p_serverFileNameTemplate  = x->p_serverFileNameTemplate;
      p_frameType               = x->p_frameType;
      p_binningX                = x->p_binningX;
      p_binningY                = x->p_binningY;
      p_filterSlot              = x->p_filterSlot;
      p_exposureTime            = x->p_exposureTime;
      p_exposureDelay           = x->p_exposureDelay;
      p_exposureCount           = x->p_exposureCount;
      p_openClientImages        = x->p_openClientImages;
      p_newImageIdTemplate      = x->p_newImageIdTemplate;
      p_reuseImageWindow        = x->p_reuseImageWindow;
      p_autoStretch             = x->p_autoStretch;
      p_linkedAutoStretch       = x->p_linkedAutoStretch;
      p_saveClientImages        = x->p_saveClientImages;
      p_overwriteClientImages   = x->p_overwriteClientImages;
      p_clientDownloadDirectory = x->p_clientDownloadDirectory;
      p_clientFileNameTemplate  = x->p_clientFileNameTemplate;
      p_clientOutputFormatHints = x->p_clientOutputFormatHints;
      o_clientViewIds           = x->o_clientViewIds;
      o_clientFilePaths         = x->o_clientFilePaths;
      o_serverFrames            = x->o_serverFrames;
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

   INDIClient* indi = INDIClient::TheClientOrDie();
   ExclConstDeviceList x = indi->ConstDeviceList();
   const INDIDeviceListItemArray& devices( x );
   for ( auto device : devices )
      if ( device.DeviceName == p_deviceName )
      {
         if ( !indi->HasPropertyItem( device.DeviceName, "CCD_FRAME", "WIDTH" ) ) // is this a camera device?
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

void INDICCDFrameInstance::SendDeviceProperties( bool async ) const
{
   INDIClient* indi = INDIClient::TheClientOrDie();
   INDIPropertyListItem item;
   INDINewPropertyListItem newItem;
   newItem.Device = p_deviceName;

   if ( indi->GetPropertyItem( p_deviceName, "UPLOAD_MODE", UploadModePropertyString( p_uploadMode ), item ) )
   {
      newItem.Property = "UPLOAD_MODE";
      newItem.Element = UploadModePropertyString( p_uploadMode );
      newItem.PropertyType = "INDI_SWITCH";
      newItem.NewPropertyValue = "ON";
      indi->SendNewPropertyItem( newItem, async );
   }

   if ( !p_serverUploadDirectory.IsEmpty() )
   {
      newItem.Property = "UPLOAD_SETTINGS";
      newItem.Element = "UPLOAD_DIR";
      newItem.PropertyType = "INDI_TEXT";
      newItem.NewPropertyValue = p_serverUploadDirectory;
      indi->SendNewPropertyItem( newItem, async );
   }

   if ( indi->GetPropertyItem( p_deviceName, "CCD_FRAME_TYPE", CCDFrameTypePropertyString( p_frameType ), item ) )
   {
      newItem.Property = "CCD_FRAME_TYPE";
      newItem.Element = CCDFrameTypePropertyString( p_frameType );
      newItem.PropertyType = "INDI_SWITCH";
      newItem.NewPropertyValue = "ON";
      indi->SendNewPropertyItem( newItem, async );
   }

   if ( p_binningX > 0 )
      if ( indi->GetPropertyItem( p_deviceName, "CCD_BINNING", "HOR_BIN", item ) )
      {
         newItem.Property = "CCD_BINNING";
         newItem.Element = "HOR_BIN";
         newItem.PropertyType = "INDI_NUMBER";
         newItem.NewPropertyValue = String( p_binningX );
         indi->SendNewPropertyItem( newItem, async );
      }

   if ( p_binningY > 0 )
      if ( indi->GetPropertyItem( p_deviceName, "CCD_BINNING", "VER_BIN", item ) )
      {
         newItem.Property = "CCD_BINNING";
         newItem.Element = "VER_BIN";
         newItem.PropertyType = "INDI_NUMBER";
         newItem.NewPropertyValue = String( p_binningY );
         indi->SendNewPropertyItem( newItem, async );
      }

   if ( p_filterSlot > 0 )
      if ( indi->GetPropertyItem( p_deviceName, "FILTER_SLOT", "FILTER_SLOT_VALUE", item ) )
      {
         newItem.Property = "FILTER_SLOT";
         newItem.Element = "FILTER_SLOT_VALUE";
         newItem.PropertyType = "INDI_NUMBER";
         newItem.NewPropertyValue = String( p_filterSlot );
         indi->SendNewPropertyItem( newItem, async );
      }
}

String INDICCDFrameInstance::FileNameFromTemplate( const String& fileNameTemplate ) const
{
   INDIClient* indi = INDIClient::TheClientOrDie();
   INDIPropertyListItem item;

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
               if ( indi->GetPropertyItem( p_deviceName, "FILTER_SLOT", "FILTER_SLOT_VALUE", item ) )
                  if ( indi->GetPropertyItem( p_deviceName, "FILTER_NAME", "FILTER_SLOT_NAME_" + item.PropertyValue, item ) )
                     fileName << item.PropertyValue;
               break;
            case 'T':
               if ( indi->GetPropertyItem( p_deviceName, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", item ) )
                  fileName << String().Format( "%+.2lf", item.PropertyValue.ToDouble() );
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

   return fileName.Trimmed();
}

String INDICCDFrameInstance::ServerFileName() const
{
   String fileName = FileNameFromTemplate( p_serverFileNameTemplate );
   if ( !fileName.IsEmpty() )
      return fileName;
   return FileNameFromTemplate( TheICFServerFileNameTemplateParameter->DefaultValue() );

}

String INDICCDFrameInstance::ClientFileName() const
{
   String fileName = FileNameFromTemplate( p_clientFileNameTemplate );
   if ( !fileName.IsEmpty() )
      return fileName;
   return FileNameFromTemplate( TheICFClientFileNameTemplateParameter->DefaultValue() );
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
      if ( INDIClient::TheClient()->Verbosity() > 1 )
      {
         String message = INDIClient::TheClient()->CurrentServerMessage();
         if ( !message.IsEmpty() )
         {
            m_console.NoteLn( "<end><cbr><br>* Latest INDI server log entry:" );
            m_console.NoteLn( message );
            m_console.WriteLn();
         }
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

   virtual void NewFrameEvent( const String& filePath )
   {
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
   if ( p == TheICFFilterSlotParameter )
      return &p_filterSlot;
   if ( p == TheICFExposureTimeParameter )
      return &p_exposureTime;
   if ( p == TheICFExposureDelayParameter )
      return &p_exposureDelay;
   if ( p == TheICFExposureCountParameter )
      return &p_exposureCount;
   if ( p == TheICFOpenClientImagesParameter )
      return &p_openClientImages;
   if ( p == TheICFNewImageIdTemplateParameter )
      return p_newImageIdTemplate.Begin();
   if ( p == TheICFReuseImageWindowParameter )
      return &p_reuseImageWindow;
   if ( p == TheICFAutoStretchParameter )
      return &p_autoStretch;
   if ( p == TheICFLinkedAutoStretchParameter )
      return &p_linkedAutoStretch;
   if ( p == TheICFSaveClientImagesParameter )
      return &p_saveClientImages;
   if ( p == TheICFOverwriteClientImagesParameter )
      return &p_overwriteClientImages;
   if ( p == TheICFClientDownloadDirectoryParameter )
      return p_clientDownloadDirectory.Begin();
   if ( p == TheICFClientFileNameTemplateParameter )
      return p_clientFileNameTemplate.Begin();
   if ( p == TheICFClientOutputFormatHintsParameter )
      return p_clientOutputFormatHints.Begin();
   if ( p == TheICFClientViewIdParameter )
      return o_clientViewIds[tableRow].Begin();
   if ( p == TheICFClientFilePathParameter )
      return o_clientFilePaths[tableRow].Begin();
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
   else if ( p == TheICFClientDownloadDirectoryParameter )
   {
      p_clientDownloadDirectory.Clear();
      if ( sizeOrLength > 0 )
         p_clientDownloadDirectory.SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientFileNameTemplateParameter )
   {
      p_clientFileNameTemplate.Clear();
      if ( sizeOrLength > 0 )
         p_clientFileNameTemplate.SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientOutputFormatHintsParameter )
   {
      p_clientOutputFormatHints.Clear();
      if ( sizeOrLength > 0 )
         p_clientOutputFormatHints.SetLength( sizeOrLength );
   }

   else if ( p == TheICFClientFramesParameter )
   {
      o_clientViewIds.Clear();
      if ( sizeOrLength > 0 )
         o_clientViewIds.Add( String(), sizeOrLength );
   }
   else if ( p == TheICFClientViewIdParameter )
   {
      o_clientViewIds[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_clientViewIds[tableRow].SetLength( sizeOrLength );
   }
   else if ( p == TheICFClientFilePathParameter )
   {
      o_clientFilePaths[tableRow].Clear();
      if ( sizeOrLength > 0 )
         o_clientFilePaths[tableRow].SetLength( sizeOrLength );
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
   if ( p == TheICFClientDownloadDirectoryParameter )
      return p_clientDownloadDirectory.Length();
   if ( p == TheICFClientFileNameTemplateParameter )
      return p_clientFileNameTemplate.Length();
   if ( p == TheICFClientOutputFormatHintsParameter )
      return p_clientOutputFormatHints.Length();

   if ( p == TheICFClientFramesParameter )
      return o_clientViewIds.Length();
   if ( p == TheICFClientViewIdParameter )
      return o_clientViewIds[tableRow].Length();
   if ( p == TheICFClientFilePathParameter )
      return o_clientFilePaths[tableRow].Length();
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

static String UniqueFilePath( const String& filePath )
{
   if ( !File::Exists( filePath ) )
      return filePath;
   for ( unsigned u = 1; ; ++u )
   {
      String tryFilePath = File::AppendToName( filePath, '_' + String( u ) );
      if ( !File::Exists( tryFilePath ) )
         return tryFilePath;
   }
}

void AbstractINDICCDFrameExecution::Perform()
{
   m_instance.o_clientViewIds.Clear();
   m_instance.o_clientFilePaths.Clear();
   m_instance.o_serverFrames.Clear();

   if ( IsRunning() )
      throw Error( "Internal error: Recursive call to AbstractINDICCDFrameExecution::Perform() detected." );

   if ( m_instance.p_deviceName.IsEmpty() )
      throw Error( "No device has been specified." );

   INDIClient* indi = INDIClient::TheClientOrDie();

   if ( !indi->HasDevices() )
      throw Error( "No INDI device has been connected." );

   INDINewPropertyListItem newItem;
   newItem.Device = m_instance.p_deviceName;

   try
   {
      m_instance.ValidateDevice();

      m_instance.SendDeviceProperties( false/*async*/ );

      INDIPropertyListItem item;
      bool serverSendsImage = true; // be compatible with legacy drivers not implementing UPLOAD_MODE
      bool serverKeepsImage = false;
      if ( indi->GetPropertyItem( m_instance.p_deviceName, "UPLOAD_MODE", "UPLOAD_LOCAL", item ) )
         if ( item.PropertyValue == "ON" )
         {
            serverSendsImage = false;
            serverKeepsImage = true;
         }
         else
         {
            if ( indi->GetPropertyItem( m_instance.p_deviceName, "UPLOAD_MODE", "UPLOAD_BOTH", item ) )
               if ( item.PropertyValue == "ON" )
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

         indi->ClearDownloadedImagePath();

         if ( serverKeepsImage )
         {
            String serverFileName = m_instance.ServerFileName();
            m_instance.o_serverFrames << serverFileName;

            newItem.Property = "UPLOAD_SETTINGS";
            newItem.Element = "UPLOAD_PREFIX";
            newItem.PropertyType = "INDI_TEXT";
            newItem.NewPropertyValue = serverFileName;
            indi->SendNewPropertyItem( newItem, false/*async*/ );
         }

         newItem.Property = "CCD_EXPOSURE";
         newItem.Element = "CCD_EXPOSURE_VALUE";
         newItem.PropertyType = "INDI_NUMBER";
         newItem.NewPropertyValue = String( m_instance.p_exposureTime );
         if ( !indi->SendNewPropertyItem( newItem, true/*async*/ ) )
         {
            ExposureErrorEvent( "Failure to send new property values to INDI server" );
            ++m_errorCount;
            continue; // ### TODO: Implement a p_onError process parameter
         }

         ElapsedTime T;

         for ( bool inExposure = false; ; )
         {
            if ( indi->GetPropertyItem( m_instance.p_deviceName, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", item, false/*formatted*/ ) )
               if ( item.PropertyState == IPS_BUSY )
               {
                  if ( inExposure )
                  {
                     // Exposure running
                     double t = 0;
                     item.PropertyValue.TryToDouble( t );
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
            for ( T.Reset(); !indi->HasDownloadedImage(); )
               if ( T() > 1 )
                  WaitingForServerEvent();

            String filePath = indi->DownloadedImagePath();
            FileFormat inputFormat( File::ExtractExtension( filePath ), true/*read*/, false/*write*/ );
            FileFormatInstance inputFile( inputFormat );

            ImageDescriptionArray images;
            if ( !inputFile.Open( images, filePath, "raw cfa verbosity 0 up-bottom signed-is-physical" ) )
               throw CatchedException();
            if ( images.IsEmpty() )
               throw Error( filePath + ": Empty image file." );
            if ( !images[0].info.supported || images[0].info.NumberOfSamples() == 0 )
               throw Error( filePath + ": Invalid or unsupported image." );

            IsoStringList propertyNames;
            Array<Variant> propertyValues;
            if ( inputFormat.CanStoreProperties() )
            {
               ImagePropertyDescriptionArray properties = inputFile.Properties();
               for ( auto property : properties )
               {
                  propertyNames << property.id;
                  propertyValues << inputFile.ReadProperty( property.id );
               }
            }
            /*
             * ### TODO: ASAP: Create standard XISF properties from FITS header
             *           keywords. We cannot rely on instance parameters here,
             *           since the INDI device driver can impose restrictions
             *           and the acquisition process can suffer from
             *           unpredictable variations. Header keywords supposedly
             *           reflect actual acquisition parameters.
             *
            propertyNames << "Instrument:ExposureTime";
            propertyValues << m_instance.p_exposureTime; // unreliable?
            ...
             */

            FITSKeywordArray keywords;
            if ( inputFormat.CanStoreKeywords() )
               inputFile.Extract( keywords );
            keywords << FITSHeaderKeyword( "COMMENT", IsoString(), "Acquired with " + PixInsightVersion::AsString() )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), "Acquired with " + Module->ReadableVersion() )
                     << FITSHeaderKeyword( "HISTORY", IsoString(), "Acquired with " + m_instance.Meta()->Id() + " process" );

            ICCProfile iccProfile;
            if ( inputFormat.CanStoreICCProfiles() )
               inputFile.Extract( iccProfile );

            RGBColorSystem rgbws;
            if ( inputFormat.CanStoreRGBWS() )
               inputFile.ReadRGBWS( rgbws );

            DisplayFunction df;
            if ( inputFormat.CanStoreDisplayFunctions() )
               inputFile.ReadDisplayFunction( df );

            ColorFilterArray cfa;
            if ( inputFormat.CanStoreColorFilterArrays() )
               inputFile.ReadColorFilterArray( cfa );

            ImageWindow window;
            bool reusedWindow = false;

            if ( m_instance.p_saveClientImages )
            {
               ImageVariant image;
               image.CreateSharedImage( images[0].options.ieeefpSampleFormat,
                                        false/*isComplex*/,
                                        images[0].options.bitsPerSample );
               if ( !inputFile.ReadImage( image ) )
                  throw CatchedException();

               inputFile.Close();

               FileFormat outputFormat( ".xisf", false/*read*/, true/*write*/ );
               FileFormatInstance outputFile( outputFormat );

               String outputFilePath = m_instance.p_clientDownloadDirectory.Trimmed();
               if ( outputFilePath.IsEmpty() )
               {
                  outputFilePath = PixInsightSettings::GlobalString( "ImageWindow/DownloadsDirectory" );
                  if ( outputFilePath.IsEmpty() ) // this cannot happen
                     outputFilePath = File::SystemTempDirectory();
               }
               if ( !outputFilePath.EndsWith( '/' ) )
                  outputFilePath += '/';
               outputFilePath += m_instance.ClientFileName();
               outputFilePath += ".xisf";
               if ( !m_instance.p_overwriteClientImages )
                  outputFilePath = UniqueFilePath( outputFilePath );

               String outputHints = m_instance.p_clientOutputFormatHints.Trimmed();
               if ( !outputHints.IsEmpty() )
                  outputHints += ' ';
               outputHints += "image-type " + m_instance.CCDFrameTypePrefix( m_instance.p_frameType ).Lowercase();

               if ( !outputFile.Create( outputFilePath, outputHints ) )
                  throw CatchedException();

               if ( !propertyNames.IsEmpty() )
                  if ( outputFormat.CanStoreProperties() )
                     for ( size_type i = 0; i < propertyNames.Length(); ++i )
                        outputFile.WriteProperty( propertyNames[i], propertyValues[i] );

               if ( outputFormat.CanStoreKeywords() )
                  outputFile.Embed( keywords );

               if ( iccProfile.IsProfile() )
                  if ( outputFormat.CanStoreICCProfiles() )
                     outputFile.Embed( iccProfile );

               if ( rgbws != RGBColorSystem::sRGB )
                  if ( outputFormat.CanStoreRGBWS() )
                     outputFile.WriteRGBWS( rgbws );

               if ( outputFormat.CanStoreDisplayFunctions() )
               {
                  if ( m_instance.p_autoStretch )
                  {
                     int n = image.NumberOfNominalChannels();
                     DVector center( n );
                     DVector sigma( n );
                     for ( int i = 0; i < n; ++i )
                     {
                        center[i] = image.Median( Rect( 0 ), i, i );
                        sigma[i] = 1.4826 * image.MAD( center[i], Rect( 0 ), i, i );
                     }
                     df.SetLinkedRGB( m_instance.p_linkedAutoStretch );
                     df.ComputeAutoStretch( sigma, center );
                  }

                  if ( !df.IsIdentityTransformation() )
                     outputFile.WriteDisplayFunction( df );
               }

               if ( !cfa.IsEmpty() )
                  if ( outputFormat.CanStoreColorFilterArrays() )
                     outputFile.WriteColorFilterArray( cfa );

               outputFile.SetOptions( images[0].options );

               if ( !outputFile.WriteImage( image ) )
                  throw CatchedException();

               outputFile.Close();

               m_instance.o_clientFilePaths << outputFilePath;

               NewFrameEvent( outputFilePath );

               if ( m_instance.p_openClientImages )
               {
                  Array<ImageWindow> windows = ImageWindow::Open( outputFilePath, IsoString()/*id*/, false/*asCopy*/, false/*allowMessages*/ );
                  if ( !windows.IsEmpty() )
                     window = windows[0];
               }
            }
            else
            {
               if ( m_instance.p_openClientImages )
               {
                  if ( m_instance.p_reuseImageWindow )
                  {
                     window = ImageWindow::WindowById( m_instance.p_newImageIdTemplate );
                     if ( !window.IsNull() ) // Make sure our window is valid and has not been write-locked by other task.
                        if ( !window.MainView().CanWrite() )
                           window = ImageWindow::Null();
                  }
                  reusedWindow = !window.IsNull();
                  if ( !reusedWindow )
                     window = ImageWindow( 1, 1, 1,
                                          images[0].options.bitsPerSample,
                                          images[0].options.ieeefpSampleFormat,
                                          false/*color*/,
                                          true/*initialProcessing*/,
                                          m_instance.p_newImageIdTemplate );

                  View view = window.MainView();

                  ImageVariant image = view.Image();
                  if ( !inputFile.ReadImage( image ) )
                     throw CatchedException();

                  inputFile.Close();

                  if ( !propertyNames.IsEmpty() )
                     for ( size_type i = 0; i < propertyNames.Length(); ++i )
                        if ( propertyValues[i].IsValid() )
                           view.SetPropertyValue( propertyNames[i], propertyValues[i], false/*notify*/,
                                                ViewPropertyAttribute::Storable|ViewPropertyAttribute::Permanent );

                  window.SetKeywords( keywords );

                  if ( inputFormat.CanStoreResolution() )
                     window.SetResolution( images[0].options.xResolution, images[0].options.yResolution, images[0].options.metricResolution );

                  if ( iccProfile )
                     window.SetICCProfile( iccProfile );

                  if ( inputFormat.CanStoreRGBWS() )
                     window.SetRGBWS( rgbws );

                  if ( !cfa.IsEmpty() )
                  {
                     // ### TODO - Cannot be implemented until the core provides support for CFAs
                  }

                  if ( m_instance.p_autoStretch )
                  {
                     int n = image.NumberOfNominalChannels();
                     DVector center( n );
                     DVector sigma( n );
                     for ( int i = 0; i < n; ++i )
                     {
                        center[i] = image.Median( Rect( 0 ), i, i );
                        sigma[i] = 1.4826 * image.MAD( center[i], Rect( 0 ), i, i );
                     }
                     df.SetLinkedRGB( m_instance.p_linkedAutoStretch );
                     df.ComputeAutoStretch( sigma, center );
                     View::stf_list stf( 4 );
                     for ( int i = 0; i < n; ++i )
                        stf[i] = df[i];
                     view.SetScreenTransferFunctions( stf );
                     view.EnableScreenTransferFunctions();
                  }
               }

               m_instance.o_clientFilePaths << String();
            }

            if ( window.IsNull() )
               m_instance.o_clientViewIds << String();
            else
            {
               m_instance.o_clientViewIds << window.MainView().Id();
               NewFrameEvent( window, reusedWindow );
            }
         }
      }

      indi->ClearDownloadedImagePath();

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
         newItem.Property = "CCD_ABORT_EXPOSURE";
         newItem.Element = "ABORT";
         newItem.PropertyType = "INDI_SWITCH";
         newItem.NewPropertyValue = "ON";
         indi->SendNewPropertyItem( newItem, true/*async*/ );
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

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInstance.cpp - Released 2016/05/17 15:40:50 UTC
