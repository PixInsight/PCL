//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 01.02.05.0187
// ----------------------------------------------------------------------------
// CosmeticCorrectionInterface.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2017 Nikolay Volkov
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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

#include "CosmeticCorrectionInterface.h"
#include "CosmeticCorrectionProcess.h"

#include <pcl/Dialog.h>
#include <pcl/FileDialog.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/RealTimePreview.h>
#include <pcl/SpinStatus.h>
#include <pcl/StdStatus.h>


namespace pcl
{
static Rect s_rect;
static String s_viewId; //RTP view.FullId();
static String s_MasterDarkId;

static bool s_requiresGetImageRectangle;
static bool s_requiresGetDarkRectangle;
static bool s_requiresStatistics;
static bool s_requiresFiltering;

static bool s_requiresAutoHotMapGeneration;
static bool s_requiresAutoColdMapGeneration;
static bool s_requiresDarkHotMapGeneration;
static bool s_requiresDarkColdMapGeneration;
static bool s_requiresListMapGeneration;

static bool s_requiresUpdate;
static bool s_requiresSnapshot;

static uint16 s_hotBad;
static uint16 s_coldBad;

// ----------------------------------------------------------------------------
CosmeticCorrectionInterface* TheCosmeticCorrectionInterface = 0;

CosmeticCorrectionInterface::CosmeticCorrectionInterface() : ProcessInterface(), instance( TheCosmeticCorrectionProcess ),
GUI( 0 )
{
   m_md = 0;
   m_Mean = 0.5;
   m_StdDev = 0.01;
   m_MinSlider = 0;
   m_MaxSlider = 65535;

   TheCosmeticCorrectionInterface = this;
   DisableAutoSaveGeometry();
}

CosmeticCorrectionInterface::~CosmeticCorrectionInterface()
{
   if ( GUI != 0 ) delete GUI, GUI = 0;
}

IsoString CosmeticCorrectionInterface::Id() const
{
   return "CosmeticCorrection";
}

MetaProcess* CosmeticCorrectionInterface::Process() const
{
   return TheCosmeticCorrectionProcess;
}

InterfaceFeatures CosmeticCorrectionInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal | InterfaceFeature::RealTimeButton;
}

void CosmeticCorrectionInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}
// ----------------------------------------------------------------------------

void CosmeticCorrectionInterface::ResetInstance()
{
   CosmeticCorrectionInstance defaultInstance( TheCosmeticCorrectionProcess );
   ImportProcess( defaultInstance );
}

bool CosmeticCorrectionInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "CosmeticCorrection" );
      UpdateControls();

      // Restore position only
      if ( !RestoreGeometry() )
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheCosmeticCorrectionProcess;
}

ProcessImplementation* CosmeticCorrectionInterface::NewProcess() const
{
   return new CosmeticCorrectionInstance( instance );
}

bool CosmeticCorrectionInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const CosmeticCorrectionInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an CosmeticCorrection instance.";
   return false;
}

bool CosmeticCorrectionInterface::RequiresInstanceValidation() const
{
   return true;
}

bool CosmeticCorrectionInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   m_Mean = 0.5;
   m_StdDev = 0.01;
   m_MinSlider = 0;
   m_MaxSlider = 65535;

   if ( m_md != 0 )
   {
      delete m_md, m_md = 0;
      m_H.Clear();
   }

   if ( !instance.p_masterDark.IsEmpty() )
   {
      if ( File::Exists( instance.p_masterDark ) )
      {
         //if ( darkPath != instance.p_masterDark ) //no or wrong H in RAM
            LoadMasterDark( instance.p_masterDark );
      }
      else //the file was deleted from disk
      {
         MessageBox("MasterDark not found: " + instance.p_masterDark,
            "CosmeticCorrection", StdIcon::Error).Execute();
         ResetInstance();
      }
   }

   UpdateControls();
   return true;
}


void CosmeticCorrectionInterface::SaveSettings() const
{ //at close PixInsight
   SaveGeometry();
}


//-------------------------------------------------------------------------

bool CosmeticCorrectionInterface::WantsReadoutNotifications() const
{
   return true;
}

void CosmeticCorrectionInterface::UpdateReadout( const View& view, const DPoint& point, double, double, double, double )
{
   if ( GUI != 0 && GUI->UseDefectList_SectionBar.Section().IsVisible() && GUI->UseDefectList_SectionBar.IsChecked() )
   {
      Point p( point.TruncatedToInt() );
      if ( !view.IsMainView() ) p.MoveBy( view.Window().PreviewRect( view.Id() ).LeftTop() );
      int x = p.x, y = p.y;
      if ( GUI->Row_RadioButton.IsChecked() ) Swap( x, y );

      GUI->DefectPosition.SetValue( x );
      GUI->DefectStart.SetValue( y );
      GUI->DefectEnd.SetValue( y );
   }
}

//-------------------------------------------------------------------------

bool CosmeticCorrectionInterface::WantsRealTimePreviewNotifications() const
{
   return true;
}

void CosmeticCorrectionInterface::RealTimePreviewOwnerChanged( ProcessInterface& iface )
{
   if ( GUI != 0 )
   {
      #if debug
      static int runCount = 0;
      Console().WriteLn( "RealTimePreviewOwnerChanged() Run # " + String( runCount++ ));
      //Console().WriteLn( "Curent owner is " + String( iface.Id() ) );
      #endif
      s_requiresUpdate = false;

      GUI->RTP_Control.Enable( iface == *this && IsRealTimePreviewActive() );
   }
}

void CosmeticCorrectionInterface::RealTimePreviewUpdated( bool active ) //RTP button click
{
   s_requiresUpdate = true;
   if ( GUI != 0 )
   {
      #if debug
      static int runCount = 0;
      Console().Write( "<br>RealTimePreviewUpdated() Run # " + String( runCount++ ));
      Console().WriteLn( " RTP is active: " + String( active ));
      #endif
      if ( active )
      {
         if ( pcl::ImageWindow::AllWindows().IsEmpty() ||
            ( !instance.p_useMasterDark && !instance.p_useAutoDetect && !instance.p_useDefectList ) )
         {
            active = false; // because no Images or no one correction method selected
         }
         else if ( instance.p_useMasterDark && m_md && ( m_md->Bounds() != pcl::ImageWindow::ActiveWindow().MainView().Bounds() ) )
         {
            MessageBox("Wrong image geometry!<br><br> MasterDark "+String(m_md->Width())+"x"+String(m_md->Height())+
               ", MainView "+String(pcl::ImageWindow::ActiveWindow().MainView().Width())+"x"+String(pcl::ImageWindow::ActiveWindow().MainView().Height())+
               ".<br><br> Can't use MasterDark.",
               "CosmeticCorrection", StdIcon::Error ).Execute();
            active = false; // because masterDark and Image geometry is not equal
         }
		}

      if ( active )
      {
         //this->SetCursor( StdCursor::ArrowWait );
         //Console().Show();
         Disable();                           // disable GUI while caclulate filtered images
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
         Enable();
         //Console().Hide();
         //this->SetCursor( StdCursor::Arrow );
      }
      else
      {
         DeactivateRealTimePreview();
         RealTimePreview::SetOwner( ProcessInterface::Null() );
      }

      #if debug
      Console().WriteLn( "RealTimePreviewUpdated():End active is " + String( active ));
      #endif
   }
}

bool CosmeticCorrectionInterface::RequiresRealTimePreviewUpdate( const UInt16Image& img, const View& view, int zoomLevel ) const
{
   #if debug
   static int runCount = 0;
   Console().WriteLn( "<br>RequiresRealTimePreviewUpdate() Run # " + String( runCount++ ));
   #endif

   // is other View selected ? Compare curent and previouse View Id.
   if ( s_viewId.Compare( view.FullId() ) ) // true == Selected other View
   {
      s_viewId = view.FullId();
      s_requiresGetImageRectangle = true;
      s_requiresStatistics = true;
      s_rect = 0; // reset
   }

   // compate subImage geomery of previouse calculation
   Rect r;

   if ( view.IsMainView() )
      r = view.Bounds();
   else
      r  = view.Window().PreviewRect( view.Id() );

   int minSize = instance.p_cfa ? 13 : 7;

   if ( (r.Width() < minSize) || (r.Height() < minSize) ) // MorphologicalTransformation can't process images less then BitmapStructure size
   {
      s_viewId = "";
      Console().WarningLn( "<br><br>** Warning: Can't generate RTP, size of Preview image must be > "
                         + String( minSize )
                         + "x" + String( minSize ) );
      Console().Show();
      return false;
   }



   if ( s_rect != r ) // other subImage geometry
   {
      s_rect = r;
      s_requiresGetImageRectangle = true;
      if ( m_md ) s_requiresGetDarkRectangle = true;
      if ( !instance.p_defects.IsEmpty() ) s_requiresListMapGeneration = true;
   }

   // is other MasterDark selected ? Compare curent and previouse MasterDark path.
   if ( s_MasterDarkId.Compare( instance.p_masterDark ) ) // true == Selected other masterDark
   {
      s_MasterDarkId = instance.p_masterDark;
      s_requiresGetDarkRectangle = true;
   }

   if (  GUI->RTPShowMap_CheckBox.IsChecked() )
      s_requiresUpdate = true;

   if (  s_requiresGetImageRectangle
      || s_requiresGetDarkRectangle

      || s_requiresDarkHotMapGeneration
      || s_requiresDarkColdMapGeneration
      || s_requiresAutoHotMapGeneration
      || s_requiresAutoColdMapGeneration
      || s_requiresListMapGeneration

      || s_requiresSnapshot
      || s_requiresUpdate
      )
   {
      #if debug
      Console().WriteLn( "s_requiresGetImageRectangle     " + String( s_requiresGetImageRectangle ) );
      Console().WriteLn( "s_requiresGetDarkRectangle      " + String( s_requiresGetDarkRectangle ) );
      Console().WriteLn( "s_requiresDarkHotMapGeneration  " + String( s_requiresDarkHotMapGeneration ) );
      Console().WriteLn( "s_requiresDarkColdMapGeneration " + String( s_requiresDarkColdMapGeneration ) );
      Console().WriteLn( "s_requiresAutoHotMapGeneration  " + String( s_requiresAutoHotMapGeneration ) );
      Console().WriteLn( "s_requiresAutoColdMapGeneration " + String( s_requiresAutoColdMapGeneration ) );
      Console().WriteLn( "s_requiresListMapGeneration     " + String( s_requiresListMapGeneration ) );
      Console().WriteLn( "s_requiresSnapshot              " + String( s_requiresSnapshot ) );
      Console().WriteLn( "s_requiresUpdate                " + String( s_requiresUpdate ) );
      #endif

      //++img.Status();
      return true;
   }
   else
   {
      #if debug
      Console().WriteLn( "RequiresRealTimePreviewUpdate:false");
      #endif

      return false;
   }
}

void CosmeticCorrectionInterface::RTPApplyBadMap( UInt16Image& target, const UInt8Image& map, const UInt16Image& value ) const
{
   const float f0 = instance.p_amount;
   const float f1 = 1 - f0;
   const int n = map.NumberOfNominalChannels() - 1;
   for ( int c = 0; c < target.NumberOfChannels(); ++c )
   {
      const UInt8Image::sample *m = map.PixelData( Min( c, n ) );
      const UInt16Image::sample *a = value.PixelData( c );
      UInt16Image::sample *t = target.PixelData( c );
      const UInt16Image::sample *end = t + target.NumberOfPixels();
      while ( t < end )
      {
         if ( *m ) *t = *a * f0 + *t * f1;
         t++, a++, m++;
      }
   }
   #if debug
   Console().WriteLn( "RTPApplyBadMap() End.");
   #endif
}

inline void CosmeticCorrectionInterface::RTPGetStatistics( Array<double>& avgDev, const View& view ) const
{
   // Calculate MainImage Statistics -----------------------------------------------
   const ImageVariant v = view.Window().MainView().Image();

   StandardStatus callback;
   StatusMonitor monitor;
   monitor.SetCallback( &callback );
   monitor.Initialize( "<end><cbr>Calculate ImageStatistics", v.NumberOfNominalChannels() );

   avgDev.Clear();                     // clear old statistics
   for ( int c = 0; c < v->NumberOfNominalChannels(); c++, ++monitor  )
   {
      ProcessEvents();
      avgDev.Add( v.AvgDev( v.Median( v.Bounds(), c, c ), v.Bounds(), c, c ) );
   }

   #if debug
   for ( int c = 0; c < v->NumberOfNominalChannels(); c++ )
      Console().WriteLn( "avgDev[c]: "+ String(avgDev[c]*65535) );
   #endif

   s_requiresStatistics = false;
}

inline void CosmeticCorrectionInterface::RTPGetImageRectangle( UInt16Image& wrk, const View& view ) const
{
   #if debug
   Console().Write( "Update working rectangle.<flush>");
   #endif

   ImageVariant v;
   v.CreateUIntImage( 16 );
   v.CopyImage( view.Window().MainView().Image() );
   v->SelectRectangle( s_rect );
   wrk = static_cast<UInt16Image&>( *v );
   v->ResetSelection();

   s_requiresGetImageRectangle = false;

   #if debug
   Console().WriteLn( " done.<flush>");
   #endif
}

inline void CosmeticCorrectionInterface::RTPGetDarkRectangle( UInt16Image& mdRect ) const
{
   #if debug
   Console().Write( "Update MasterDark rectangle.<flush>");
   #endif

   // if MasterDark selected, create badMap with geomety of MasterDark RTP rect
   // and fill badMap by MasterDark pixelData
   m_md->SelectRectangle( s_rect );
   mdRect.Assign( *m_md );

   s_requiresGetDarkRectangle = false;

   #if debug
   Console().WriteLn( " done.<flush>");
   #endif
}

inline void CosmeticCorrectionInterface::RTPGetBkg( UInt16Image& bkg, const UInt16Image& wrk ) const
{
   MorphologicalTransformation MT;

   if ( instance.p_cfa )
   {
      const char* Box[] = // define bacground CFA area arround star
      {
         "x-x-x-x-x-x-x"
         "-------------"
         "x-x-x-x-x-x-x"
         "-------------"
         "x-x-------x-x"
         "-------------"
         "x-x-------x-x"
         "-------------"
         "x-x-------x-x"
         "-------------"
         "x-x-x-x-x-x-x"
         "-------------"
         "x-x-x-x-x-x-x"
      };
      BitmapStructure s( Box, 13 );
      MT.SetStructure( s );
   }
   else
   {
      const char* Box[] = // define bacground area arround star
      {
         "xxxxxxx"
         "xxxxxxx"
         "xx---xx"
         "xx---xx"
         "xx---xx"
         "xxxxxxx"
         "xxxxxxx"
      };
      BitmapStructure s( Box, 7 );
      MT.SetStructure( s );
   }

   Console().Write(String().Format("Create%s backgound image...<flush>", instance.p_cfa ? " CFA":"" ));
   ProcessEvents(); //for update console output

   // set the pixel value to filtered by median backgound 7x7 arrea arround central 3x3 area of centaral pixel
   bkg.Assign( wrk );
   MT.SetOperator( MedianFilter() );
   MT >> bkg;

   Console().WriteLn("done.");
   ProcessEvents();
}

inline void CosmeticCorrectionInterface::RTPGetMed( UInt16Image& med, const UInt16Image& wrk ) const
{
   MorphologicalTransformation MT;

   if ( instance.p_cfa )
   {
      const char* Box[] =
      {
         "x-x-x"
         "-----"
         "x---x"
         "-----"
         "x-x-x"
      };
      BitmapStructure s( Box, 5 );
      MT.SetStructure( s );
   }
   else
   {
      const char* Box[] = // define area arround pixel
      {
         "xxx"
         "x-x"
         "xxx"
      };
      BitmapStructure s( Box, 3 );
      MT.SetStructure( s );
   }

   Console().Write(String().Format("Create%s median filtered image...<flush>", instance.p_cfa ? " CFA":"" ));
   ProcessEvents(); //for update console output

   // set the pixel value to filter by median 3x3 arrea around centaral pixel
   med.Assign( wrk );
   MT.SetOperator( MedianFilter() );
   MT >> med;

   Console().WriteLn("done.");
   ProcessEvents();
}

inline void CosmeticCorrectionInterface::RTPGetAvr( UInt16Image& avr, const UInt16Image& wrk ) const
{
   MorphologicalTransformation MT;

   if ( instance.p_cfa )
   {
      const char* Box[] =
      {
         "x-x-x"
         "-----"
         "x---x"
         "-----"
         "x-x-x"
      };
      BitmapStructure s( Box, 5 );
      MT.SetStructure( s );
   }
   else
   {
      const char* Box[] = // define area arround pixel
      {
         "xxx"
         "x-x"
         "xxx"
      };
      BitmapStructure s( Box, 3 );
      MT.SetStructure( s );
   }

   //StandardStatus s;
   //StatusMonitor m;
   //m.SetCallback( &s );
   //m.Initialize( "<end><cbr>Create average filtered image", 1 );
   Console().Write(String().Format("Create%s average filtered image...<flush>", instance.p_cfa ? " CFA":"" ));
   ProcessEvents();   //for update console output

   // set the pixel value to ad value of 3x3 arrea around centaral pixel
   avr.Assign( wrk );
   MT.SetOperator( AlphaTrimmedMeanFilter( 0 ) );
   MT >> avr;

   //++m;
   Console().WriteLn("done.");
   ProcessEvents();
}

inline int CosmeticCorrectionInterface::RTPGetHotAutoMap( UInt8Image& mapAutoHot, float& hotAutoValue, const UInt16Image& med, const UInt16Image& bkg, const UInt16Image& avr,   const UInt16Image& wrk, const Array<double>& avgDev) const
{
   static int count = 0;
   s_requiresAutoHotMapGeneration = false;
   if ( !instance.p_hotAutoCheck ) return 0;

   #if debug
   Console().WriteLn( "hotAutoValue: "+ String(hotAutoValue)+" ,instance.p_hotAutoValue: "+ String(instance.p_hotAutoValue) );
   #endif
   if ( hotAutoValue == instance.p_hotAutoValue )
   {
      #if debug
      Console().WriteLn( "Use old AutoHotMap.<flush>");
      #endif
      return count;
   }
   hotAutoValue = instance.p_hotAutoValue;   // save curent value to avoid unusual regeneration if user check/uncheck CheckCold

   #if debug
   //Console().Show();
   Console().Write( "Generate AutoHotMap.<flush>"); //instance.p_hotAutoValue changed >> regenerate map
   #endif

   mapAutoHot.AllocateData( wrk.Width(), wrk.Height(), wrk.NumberOfNominalChannels(), wrk.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
   mapAutoHot.Zero();
   count = 0;
   for ( int c = 0; c < wrk.NumberOfNominalChannels(); ++c )
   {
      const UInt16Image::sample* t = wrk.PixelData( c );
      const UInt16Image::sample* end = t + wrk.NumberOfPixels();
      const UInt16Image::sample* f = med.PixelData( c );
      const UInt16Image::sample* b = bkg.PixelData( c );
      const UInt16Image::sample* a = avr.PixelData( c );
            UInt8Image::sample*  m = mapAutoHot.PixelData(c);

      const double k1 = avgDev[c] * 65535;            // avrDev * 65535
      const double k2 = k1 / 2;                        // ( avrDev / 2 ) * 65535
      const double k3 = instance.p_hotAutoValue * k1;   // k * avrDev * 65535

      while ( t < end )
      {
         if (  ( *a < *b + k2 )   //ignore pixel surrounded by other bright pixels at avrDev/2
            && ( *t > *b + k1 )   //ignore pixel with brightnes less then (background + avrDev)
            && ( *t > *f + k3 )   //ignore pixel with brightnes less then avr of surrounded pixels * k * avrDev
            ) *m = -1,   count++;
         t++, f++, b++, a++, m++;
      }
   }
   #if debug
   Console().WriteLn( " Done.<flush>");
   #endif

   return count;
}

inline int CosmeticCorrectionInterface::RTPGetColdAutoMap( UInt8Image& mapAutoCold, float& coldAutoValue, const UInt16Image& med, const UInt16Image& bkg,   const UInt16Image& wrk, const Array<double>& avgDev) const
{
   static int count = 0;
   s_requiresAutoColdMapGeneration = false;
   if ( !instance.p_coldAutoCheck ) return 0;

   if ( coldAutoValue == instance.p_coldAutoValue )
   {
      #if debug
      Console().Write( "Use old ColdMap.<flush>");
      #endif
      return count;
   }

   coldAutoValue = instance.p_coldAutoValue;   // save curent value to avoid unusual regeneration if user check/uncheck CheckCold

   #if debug
   //Console().Show();
   Console().Write( "Generate ColdMap.<flush>"); //instance.p_coldAutoValue changed >> regenerate map
   #endif

   mapAutoCold.AllocateData( wrk.Width(), wrk.Height(), wrk.NumberOfNominalChannels(), wrk.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
   mapAutoCold.Zero();
   count = 0;
   for ( int c = 0; c < wrk.NumberOfNominalChannels(); ++c )
   {
      const UInt16Image::sample* t = wrk.PixelData( c );
      const UInt16Image::sample* end = t + wrk.NumberOfPixels();
      const UInt16Image::sample* f = med.PixelData( c );
      const UInt16Image::sample* b = bkg.PixelData( c );
            UInt8Image::sample*  m = mapAutoCold.PixelData(c);

      const double k = avgDev[c] * instance.p_coldAutoValue * 65535;   // k * avrDev * 65535 == how much pixel must be less

      while ( t < end )
      {
         const double T = *t + k;
         if ( ( T < *b ) && ( T < *f ) ) *m = -1, count++;
         t++, f++, b++, m++;
      } //end while
   } //end for
   #if debug
   Console().WriteLn( " Done.<flush>");
   #endif

   return count;
}

inline int CosmeticCorrectionInterface::RTPGetHotDarkMap( UInt8Image& mapDarkHot, float& hotDarkValue, const UInt16Image& mdRect ) const
{
   // prepare Hot Map via MasterDark ----------------------------------------------
   static int count = 0;

   s_requiresDarkHotMapGeneration = false;
   if ( !instance.p_hotDarkCheck ) return 0;
   if ( hotDarkValue == instance.p_hotDarkLevel )
   {
      #if debug
      Console().WriteLn( "Use old DarkHotMap.<flush>");
      #endif
      return count;
   }
   hotDarkValue = instance.p_hotDarkLevel;

   #if debug
   Console().Write( "Generate DarkHotMap.<flush>");
   #endif

   mapDarkHot.AllocateData( mdRect.Width(), mdRect.Height(), mdRect.NumberOfNominalChannels(), mdRect.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
   mapDarkHot.Zero();

   const bool cfaMode = instance.p_cfa && mdRect.IsColor();
   count = 0;
   for ( int c = 0; c < mdRect.NumberOfNominalChannels(); ++c )
   {
      UInt8Image::sample* m = mapDarkHot.PixelData(c);
      const UInt8Image::sample* end = m + mapDarkHot.NumberOfPixels();
      const UInt16Image::sample* d = mdRect.PixelData(c);
      while ( m < end )
      {
         if ( ( *d > 0 || !cfaMode ) && ( *d >= s_hotBad ) )
         {
            *m = -1;
            count++;
         }
         m++, d++;
      }
   }

   #if debug
   Console().WriteLn( " Done.<flush>");
   #endif

   return count;
}

inline int CosmeticCorrectionInterface::RTPGetColdDarkMap( UInt8Image& mapDarkCold, float& coldDarkValue, const UInt16Image& mdRect ) const
{
   // prepare Cold Map via MasterDark ----------------------------------------------
   static int count = 0;

   s_requiresDarkColdMapGeneration = false;
   if ( !instance.p_coldDarkCheck ) return 0;
   if ( coldDarkValue == instance.p_coldDarkLevel )
   {
      #if debug
      Console().WriteLn( "Use old DarkColdMap.<flush>");
      #endif
      return count;
   }
   coldDarkValue = instance.p_coldDarkLevel;

   #if debug
   Console().Write( "Generate DarkColdMap.<flush>");
   #endif

   mapDarkCold.AllocateData( mdRect.Width(), mdRect.Height(), mdRect.NumberOfNominalChannels(), mdRect.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
   mapDarkCold.Zero();

   const bool cfaMode = instance.p_cfa && mdRect.IsColor();
   count = 0;
   for ( int c = 0; c < mdRect.NumberOfNominalChannels(); ++c )
   {
      UInt8Image::sample* m = mapDarkCold.PixelData(c);
      const UInt8Image::sample* end = m + mapDarkCold.NumberOfPixels();
      const UInt16Image::sample* d = mdRect.PixelData(c);
      while ( m < end )
      {
         if ( ( *d > 0 || !cfaMode ) && ( *d <= s_coldBad ) )
         {
            *m = -1;
            count++;
         }
         m++, d++;
      }
   }

   #if debug
   Console().WriteLn( " Done.<flush>");
   #endif

   return count;
}

inline void CosmeticCorrectionInterface::RTPGetListDefectMap( UInt8Image& mapListDef, const View& view ) const
{
   // prepare Map via ListDefect ----------------------------------------------
   s_requiresListMapGeneration = false;

   #if debug
   Console().WriteLn( "Generate ListDefectMap.<flush>");
   #endif

   Rect r;
   if ( view.IsMainView() ) r = view.Bounds();
   else r  = view.Window().PreviewRect( view.Id() );

   mapListDef.AllocateData( s_rect.Width(), s_rect.Height(), view.Image().NumberOfNominalChannels(), view.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
   mapListDef.Zero();

   for ( size_t i = 0; i < instance.p_defects.Length(); i++ )
   {
      const CosmeticCorrectionInstance::DefectItem& item = instance.p_defects[i];
      if ( !item.enabled ) continue;

      int x0 = r.x0, x1 = r.x1-1, y0 = r.y0, y1 = r.y1-1;
      if ( !item.isRow ) Swap( x0, y0 ), Swap( x1, y1 );

      int k = item.address;                        // address in mainView coordinate
      if ( k < y0 || k > y1 ) continue;            // skip because the defect out of image view
      k -= y0;                                     // convert from mainView to RTP view coordinate

      int begin = x0, end = x1;                    // first and last visible pixel

      if ( item.isRange )
      {
         begin = Min( item.begin, item.end );
         if ( begin > x1 ) continue;               // skip out of view defects

         end = Max( item.begin, item.end );
         if ( end < x0 ) continue;                 // skip out of view defects

         begin = Max( begin, x0 );                 // cut begin of out of view defects
         end = Min( end, x1 );                     // cut end of out of view defects
      }

      begin -= x0; end -= x0;                      // convert to RTP view coordinate

      for ( int c = 0; c < mapListDef.NumberOfNominalChannels(); c++ )
      {
         for ( int j = begin; j <= end; j++ )
         {
            if ( item.isRow ) mapListDef.Pixel( j, k, c ) = -1;
            else mapListDef.Pixel( k, j, c ) = -1;

         }
      }
   }
}


inline void CosmeticCorrectionInterface::RTPMakeSnapshot( const UInt16Image& img, const View& view ) const
{
   #if debug
   Console().WriteLn( "s_requiresSnapshot <flush>");
   #endif

   String id = view.Id();
   if ( view.IsPreview() ) id.Prepend(view.Window().MainView().Id() + "_" );

   ImageWindow targetWindow( img.Width(), img.Height(),
      img.NumberOfChannels(), img.BitsPerSample(), img.IsFloatSample(),
      img.IsColor(),   true, id + "_Snapshot" );

   targetWindow.MainView().Image().CopyImage( img );

   if ( view.AreScreenTransferFunctionsEnabled() )
   {
      View::stf_list stf;
      view.GetScreenTransferFunctions( stf );
      targetWindow.MainView().SetScreenTransferFunctions( stf );
   }

   targetWindow.Show();
   s_requiresSnapshot = false;
}


bool CosmeticCorrectionInterface::GenerateRealTimePreview( UInt16Image& img, const View& view, int zoomLevel, String& info ) const
{
   #if debug
   static int runCount = 0;
   Console().WriteLn( "<br>GenerateRealTimePreview() Run # " + String( runCount++ ) );
   Console().WriteLn( "masterDark: " + String( m_md != 0 )
      + ", p_hotAutoCheck: "  + String( bool(instance.p_hotAutoCheck ) )
      + ", p_coldAutoCheck: " + String( bool(instance.p_coldAutoCheck) )
      + ", p_hotDarkCheck: "  + String( bool(instance.p_hotDarkCheck ) )
      + ", p_coldDarkCheck: " + String( bool(instance.p_coldDarkCheck) ) );
   Console().Flush();
   #endif

   static UInt16Image wrk;         // == view.Window().CurrentView().Image(). Changed only if other View or rectangle selected.
   static UInt16Image med;         // Filtered by median 3x3 arrea arround centaral pixel. Used for Hot/Cold AutoDetection and Dark corection Cold pixels.
   static UInt16Image bkg;         // Filtered by median backgound 7x7 arrea arround central 3x3 area of centaral pixel. Used for Hot/Cold AutoDetection and Auto corection Cold pixels.
   static UInt16Image avr;         // Averaged value of 3x3 arrea arround centaral pixel. Used for Hot AutoDetection and Auto/Dark corection Hot pixels.
   static bool requiresAvr = false;
   static bool requiresMed = false;
   static bool requiresBkg = false;
   static bool cfa = false;

   static UInt16Image mdRect;      // Rectangle form masterDark

   static UInt8Image mapDarkHot;   // Defective Map via DarkMaster
   static UInt8Image mapDarkCold;  // Defective Map via DarkMaster
   static UInt8Image mapAutoHot;   // Defective Map via AutoDetect
   static UInt8Image mapAutoCold;  // Defective Map via AutoDetect
   static UInt8Image mapListDef;   // Defective Map via ListDefect

   static Array<double> avgDev;   // statictics by channel
   static int countDarkHot, countDarkCold, countAutoHot, countAutoCold;

   static float hotDarkValue = -1; // -1 == mapDarkHot  is not generated
   static float coldDarkValue = -1;// -1 == mapDarkCold is not generated
   static float hotAutoValue = -1; // -1 == mapAutoHot  is not generated
   static float coldAutoValue = -1;// -1 == mapAutoCold is not generated

   // prepare MainImage statistics if image changed and required for Auto correction -------------
   if ( s_requiresStatistics && ( instance.p_hotAutoCheck || instance.p_coldAutoCheck ) )
   {
      RTPGetStatistics( avgDev, view );
   }

   // Create image from View rectangle ------------------------------------------
   if ( s_requiresGetImageRectangle )
   {
      RTPGetImageRectangle( wrk, view );
      cfa = !instance.p_cfa;                  // forse regenerate filtered images
      hotAutoValue = coldAutoValue = -1;   // forse regenerate Auto maps
      s_requiresListMapGeneration = true;
   }

   #if debug
   Console().WriteLn( "cfa: "+ String(cfa)+" ,instance.cfa: "+ String(bool(instance.p_cfa)) );
   Console().Flush();
   #endif

   // if (cfa mode changed) regenetate filtered images
   if ( cfa == !instance.p_cfa )
   {
      #if debug
      Console().WriteLn( "cfa == !instance.p_cfa" );
      Console().Flush();
      #endif

      cfa = instance.p_cfa;
      s_requiresFiltering = true;
      requiresAvr = requiresMed = requiresBkg = true; // required regenerate filtered images
      hotAutoValue = coldAutoValue = -1;   // recuired generate AutoMaps
   }

   // Create image from MasterDark with size of View rectangle -----------------------
   if ( s_requiresGetDarkRectangle && m_md && instance.p_useMasterDark )
   {
      RTPGetDarkRectangle( mdRect );
      s_requiresDarkHotMapGeneration = s_requiresDarkColdMapGeneration = true;
      hotDarkValue = coldDarkValue = -1; // forse regenerate Dark maps
   }

   // Create Average, Median and Background images from View rectangle -------------------
   if ( s_requiresFiltering )
   {
      if ( requiresAvr && (
            ( instance.p_useMasterDark && instance.p_hotDarkCheck )
            ||
            ( instance.p_useAutoDetect && instance.p_hotAutoCheck )
         ))
      {
         requiresAvr = false;
         RTPGetAvr( avr, wrk );
         s_requiresAutoHotMapGeneration = true;
      }

      if ( requiresMed && (
            ( instance.p_useMasterDark && instance.p_coldDarkCheck )
            ||
            ( instance.p_useAutoDetect && ( instance.p_hotAutoCheck || instance.p_coldAutoCheck ) )
         ))
      {
         requiresMed = false;
         RTPGetMed( med, wrk );
         s_requiresAutoColdMapGeneration = true;
         s_requiresAutoHotMapGeneration = true;
      }

      if ( requiresBkg &&
            (
               ( instance.p_useAutoDetect && ( instance.p_hotAutoCheck || instance.p_coldAutoCheck ) )
               ||
               ( instance.p_useDefectList && !instance.p_defects.IsEmpty() )
            )
         )
      {
         requiresBkg = false;
         RTPGetBkg( bkg, wrk );
         s_requiresAutoColdMapGeneration = true;
         s_requiresAutoHotMapGeneration = true;
      }

      if ( !( requiresAvr || requiresMed || requiresBkg ) ) // if (all == false)
         s_requiresFiltering = false;
   }

   // generate maps by request -----------------------------------------------------
   if ( s_requiresAutoHotMapGeneration )
   {
      countAutoHot = RTPGetHotAutoMap( mapAutoHot, hotAutoValue, med, bkg, avr, wrk, avgDev );
      GUI->RTPAutoQtyHot_Edit.SetValue( countAutoHot );
   }
   if ( s_requiresAutoColdMapGeneration )
   {
      countAutoCold = RTPGetColdAutoMap( mapAutoCold, coldAutoValue, med, bkg, wrk, avgDev );
      GUI->RTPAutoQtyCold_Edit.SetValue(  countAutoCold );
   }
   if ( s_requiresDarkHotMapGeneration )
   {
      countDarkHot = RTPGetHotDarkMap( mapDarkHot, hotDarkValue, mdRect );
      GUI->RTPDarkQtyHot_Edit.SetValue( countDarkHot );
   }
   if ( s_requiresDarkColdMapGeneration )
   {
      countDarkCold = RTPGetColdDarkMap( mapDarkCold, coldDarkValue, mdRect );
      GUI->RTPDarkQtyCold_Edit.SetValue(  countDarkCold );
   }
   if ( s_requiresListMapGeneration && instance.p_useDefectList )
   {
      RTPGetListDefectMap( mapListDef, view );
   }

   // prepare output image ----------------------------------------------------------
   UInt16Image tmp;

   if ( GUI->RTPShowMap_CheckBox.IsChecked() )
   {
      #if debug
      Console().Write( "Show BadMap. <flush>" );
      #endif

      tmp.AllocateData( wrk.Width(), wrk.Height(), wrk.NumberOfNominalChannels(), wrk.IsColor() ? ColorSpace::RGB : ColorSpace::Gray );
      tmp.Zero();

      // accumulate bad pixels from all bad maps to one map
      if ( instance.p_useAutoDetect && instance.p_hotAutoCheck       ) tmp.Or( mapAutoHot  );
      if ( instance.p_useAutoDetect && instance.p_coldAutoCheck      ) tmp.Or( mapAutoCold );
      if ( instance.p_useMasterDark && instance.p_hotDarkCheck       ) tmp.Or( mapDarkHot  );
      if ( instance.p_useMasterDark && instance.p_coldDarkCheck      ) tmp.Or( mapDarkCold );
      if ( instance.p_useDefectList && !instance.p_defects.IsEmpty() ) tmp.Or( mapListDef  );
   }
   else
   {
      #if debug
      Console().WriteLn( "Show Light<flush>" );
      #endif

      tmp.Assign( wrk );

      if ( instance.p_useMasterDark && instance.p_hotDarkCheck       ) RTPApplyBadMap( tmp, mapDarkHot,  avr );   // replace hot pixel by surrounding neighbors Mean
      if ( instance.p_useMasterDark && instance.p_coldDarkCheck      ) RTPApplyBadMap( tmp, mapDarkCold, med );   // replace hot pixel by surrounding neighbors Median
      if ( instance.p_useAutoDetect && instance.p_hotAutoCheck       ) RTPApplyBadMap( tmp, mapAutoHot,  avr );   // replace hot pixel by surrounding neighbors Mean
      if ( instance.p_useAutoDetect && instance.p_coldAutoCheck      ) RTPApplyBadMap( tmp, mapAutoCold, bkg );   // replace cold pixel by background Median
      if ( instance.p_useDefectList && !instance.p_defects.IsEmpty() ) RTPApplyBadMap( tmp, mapListDef,  bkg );   // replace Defective List pixels by background Median
   }

   img.Assign( tmp ); // Send Output image to RTP

   // make Snapshot ---------------------------------------------------------------------------
   if ( s_requiresSnapshot )   RTPMakeSnapshot( img, view );

   //s_requiresUpdate = false;
   #if debug
   Console().WriteLn( "GenerateRealTimePreview() End." );
   #endif

   return true;
}

void CosmeticCorrectionInterface::RTPUpdate()
{
   if ( !GUI->RTP_Control.IsEnabled() ) return;
   Disable();
   RealTimePreview::Update();
   //Console().Hide();
   Enable();
}
// ----------------------------------------------------------------------------

void FileShow( String& path )
{
   if ( ImageWindow::WindowByFilePath( path ).IsNull() )
   {
      Array<ImageWindow> windows = ImageWindow::Open( path, IsoString()/*id*/, "raw cfa"/*formatHints*/ );
      for ( ImageWindow& window : windows )
         window.Show();
   }
   else
      ImageWindow::WindowByFilePath( path ).BringToFront();
}

void CosmeticCorrectionInterface::LoadMasterDark( const String& filePath)
{
   #if debug
   Console().WriteLn( "LoadMasterDark()" );
   #endif

   if ( m_md != 0 )
	{
		delete m_md, m_md = 0;
		m_H.Clear();
	}

   Console().WriteLn( "<end><cbr>Loading MasterDark image:<flush>" );
   Console().WriteLn( filePath );

   FileFormat format( File::ExtractExtension( filePath ), true, false );
   FileFormatInstance file( format );
   ImageDescriptionArray images;
   if ( !file.Open( images, filePath ) )
      throw CaughtException();
   if ( images.IsEmpty() )
      throw Error( filePath + ": Empty MasterDark image." );
   //if ( images.Length() > 1 ) throw Error( filePath + ": Multiple images cannot be used as MasterDark." );
   if ( !file.SelectImage( 0 ) )
      throw CaughtException();
   m_md = new DarkImg( (void*)0, 0, 0 );
   if ( !file.ReadImage( *m_md ) || !file.Close() )
      throw CaughtException();

   m_channels = m_md->NumberOfNominalChannels();

   ImageWindow w( m_md->Width(), m_md->Height(), m_md->NumberOfChannels(), m_md->BitsPerSample(),
      m_md->IsFloatSample(), m_md->IsColor(), true, "CosmeticCorrectionDark2" );

   ImageVariant v = w.MainView().Image();
   v.CopyImage(*m_md);

   m_H.Clear();
   UI64Matrix histogram = w.MainView().ComputeOrFetchProperty( "Histogram16", false/*notify*/ ).ToUI64Matrix();
   for ( int i = 0; i < histogram.Rows(); ++i )
      m_H.Add( Histogram( histogram.RowVector( i ) ) );

   ImageStatistics ss;
   ss.DisableExtremes();
   ss.DisableMedian();
   ss.EnableRejection( (m_channels > 1) , false ); //During Mean calculation reject zero if RGB CFA dark

   int max = 0;
   m_StdDev = m_Mean = 0;
   int min = m_H[0].LastLevel(); //65535
   for ( int c = 0; c < m_channels; c++ )
   {
      v->SelectChannel(c);
      ss << v;
      m_Mean += ss.Mean();
      m_StdDev += ss.StdDev();

      max = Max( max , m_H[c].ClipHigh(0) );
      min = Min( min , m_H[c].ClipLow(0) );

      #if debug
      Console().WriteLn("min = H[c].ClipLow(0):" + String( m_H[c].ClipLow(0) ));
      Console().WriteLn("min:" + String( min ));
      Console().WriteLn("max:" + String( max ));
      for( int i = 0; i<min; i++)
      {
         size_t v = m_H[c].Count(i);
         if ( v != 0)
            Console().WriteLn("H[c].Count("+String(i)+"):" + String( m_H[c].Count(i) ));
      }

      Console().WriteLn("H[c].Count():" + String( m_H[c].Count() ));
      Console().WriteLn("H[c].Count( 0, min-1 ):" + String( m_H[c].Count( 0, min-1 ) ));
      Console().WriteLn("H[c].Count( max+1, 65535 ):" + String( m_H[c].Count( max+1, 65535 ) ));
      Console().WriteLn("H[c].Count( min, max ):" + String( m_H[c].Count( min, max ) ));
      #endif
   }
   w.Close();

   m_Mean /= m_channels;
   m_StdDev /= m_channels;

   m_MaxSlider = Histogram().HistogramLevel( Histogram().NormalizedLevel(max+1) );
   m_MinSlider = Histogram().HistogramLevel( Histogram().NormalizedLevel(min-1) );

   UpdateMasterDarkControls();

   s_requiresGetDarkRectangle = true; // if RTP enabled and MD is changed >> requiresDarkMapGeneration
   if ( GUI->RTP_Control.IsEnabled() ) RealTimePreviewUpdated( true ); //Call to check geometry with RTP mainView
   RTPUpdate(); //Call update badMap
}

void CosmeticCorrectionInterface::SelectMasterFrameDialog ()
{
   #if debug
   Console().WriteLn( "SelectMasterFrameDialog()");
   #endif

   OpenFileDialog d;
   d.LoadImageFilters();
   d.DisableMultipleSelections();
   d.SetCaption( "CosmeticCorrection: Select MasterDark Frame" );
   if ( d.Execute() )
   {
      if (instance.p_masterDark == d.FileName()) return;
      instance.p_masterDark = d.FileName();
      LoadMasterDark( instance.p_masterDark );
   }
}


void CosmeticCorrectionInterface::HotUpdateGUI( const size_t count )
{
   const float sigma = (instance.p_hotDarkLevel - m_Mean)/m_StdDev;
   GUI->HotDarkSigma_NumericControl.SetValue( sigma );
   GUI->HotRealQty_Value.SetText( String().Format( "Qty: %u, Level: %1.10f, Sigma: %f",
      count, instance.p_hotDarkLevel, sigma ));
}
void CosmeticCorrectionInterface::HotQtyUpdated( size_t qty ) //caclulate Level and Sigma
{
   size_t count = 0;
   for ( int i = m_MaxSlider; i >= m_MinSlider; i--)
   {
      for ( int c = 0; c < m_channels; c++ ) count += m_H[c].Count(i);
      if ( count >= qty )
      {
         instance.p_hotDarkLevel = Histogram().NormalizedLevel( i );
         if ( s_hotBad != i )
         {
            s_hotBad = i;
            if ( instance.p_hotDarkCheck ) s_requiresDarkHotMapGeneration = true;
         }
         break;
      }
   }

   GUI->HotDarkLevel_NumericControl.SetValue( instance.p_hotDarkLevel );
   HotUpdateGUI( count );
}

void CosmeticCorrectionInterface::HotLevelUpdated( const float value, const bool recalculate = false ) //calculate Qty and Sigma
{
   static int min = -1;                              //previous HistogramLevel
   int v = Histogram().HistogramLevel( value );      // =RoundI(float*65535)
   if ( (min == v) && !recalculate ) return;         //same HistogramLevel
   instance.p_hotDarkLevel = Histogram().NormalizedLevel( min = v );
   s_hotBad = v;
   if ( instance.p_hotDarkCheck ) s_requiresDarkHotMapGeneration = true;

   size_t count = 0;
   for ( int c = 0; c < m_channels; c++ ) count += m_H[c].Count( min, m_MaxSlider );

   GUI->HotDarkQty_NumericControl.SetValue( count );
   HotUpdateGUI( count );
}

void CosmeticCorrectionInterface::HotSigmaUpdated( const float sigma ) // calculate Level and QTY
{
   const float level = m_Mean + sigma * m_StdDev;
   HotLevelUpdated( level, false );
   GUI->HotDarkLevel_NumericControl.SetValue( instance.p_hotDarkLevel );
}

void CosmeticCorrectionInterface::ColdUpdateGUI( const size_t count )
{
   const float sigma = (m_Mean - instance.p_coldDarkLevel)/m_StdDev;
   GUI->ColdDarkSigma_NumericControl.SetValue( sigma );
   GUI->ColdRealQty_Value.SetText( String().Format( "Qty: %u, Level: %1.10f, Sigma: %f",
      count, instance.p_coldDarkLevel, sigma ) );
}

void CosmeticCorrectionInterface::ColdQtyUpdated( size_t qty ) //caclulate Level
{
   size_t count = 0;
   int min = m_MinSlider;
   if ( instance.p_cfa && m_channels>1 && min==0 ) min++;

   for ( int i = min; i <= m_MaxSlider; i++)
   {
      for ( int c = 0; c < m_channels; c++ ) count += m_H[c].Count(i);
      if ( count >= qty )
      {
         instance.p_coldDarkLevel = Histogram().NormalizedLevel( i );
         if ( s_coldBad != i )
         {
            s_coldBad = i;
            if ( instance.p_coldDarkCheck ) s_requiresDarkColdMapGeneration = true;
         }
         break;
      }
   }

   GUI->ColdDarkLevel_NumericControl.SetValue( instance.p_coldDarkLevel );
   ColdUpdateGUI( count );
}

void CosmeticCorrectionInterface::ColdLevelUpdated( const float value, const bool recalculate = false ) //calculate Qty
{
   static int max = -1;                           //previous HistogramLevel
   int v = Histogram().HistogramLevel( value );   // =RoundI(float*65535)
   if ( (max == v) && !recalculate ) return;      //same HistogramLevel
   max = v;                                       //save new HistogramLevel
   s_coldBad = v;

   if ( instance.p_coldDarkCheck ) s_requiresDarkColdMapGeneration = true;
   int min = m_MinSlider;
   if ( instance.p_cfa && m_channels>1 && min==0 )
      max = Max( ++min, max ); // for CFA RGB

   instance.p_coldDarkLevel = Histogram().NormalizedLevel( max ); // =float(max/65535)

   size_t count = 0;
   for ( int c = 0; c < m_channels; c++ ) count += m_H[c].Count( min, max );

   GUI->ColdDarkQty_NumericControl.SetValue( count );
   ColdUpdateGUI( count );
}

void CosmeticCorrectionInterface::ColdSigmaUpdated( const float sigma ) // calculate Level and QTY
{
   const float level = m_Mean - sigma * m_StdDev;

   ColdLevelUpdated( level, false );
   GUI->ColdDarkLevel_NumericControl.SetValue( instance.p_coldDarkLevel );
}

void CosmeticCorrectionInterface::UpdateControls()
{
   GUI->OutputDir_Edit.SetText( instance.p_outputDir );
   GUI->Postfix_Edit.SetText( instance.p_postfix );
   GUI->Prefix_Edit.SetText( instance.p_prefix );
   GUI->Overwrite_CheckBox.SetChecked( instance.p_overwrite );

   GUI->CFA_CheckBox.SetChecked( instance.p_cfa );
   GUI->Amount_NumericControl.SetValue( instance.p_amount );

   GUI->RTP_Control.Disable();

   UpdateTargetImagesList();
   UpdateImageSelectionButtons();
   UpdateMasterDarkControls();
   UpdateAutoDetectControls();
   UpdateDefectListControls();
}

void CosmeticCorrectionInterface::UpdateTargetImageItem( size_type i )
{
   TreeBox::Node* node = GUI->TargetImages_TreeBox[int(i)];
   if ( node == 0 )
      return;

   const CosmeticCorrectionInstance::ImageItem& item = instance.p_targetFrames[i];

   node->SetText( 0, String( i+1 ) );
   node->SetAlignment( 0, TextAlign::Right );

   node->SetIcon( 1, Bitmap( ScaledResource( item.enabled ? ":/browser/enabled.png" : ":/browser/disabled.png" ) ) );
   node->SetAlignment( 1, TextAlign::Left );

   node->SetIcon( 2, Bitmap( ScaledResource( ":/browser/document.png" ) ) );
   if ( GUI->FullPaths_CheckBox.IsChecked() )
      node->SetText( 2, item.path );
   else
      node->SetText( 2, File::ExtractNameAndSuffix( item.path ) );
   node->SetToolTip( 2, item.path );
   node->SetAlignment( 2, TextAlign::Left );
}

void CosmeticCorrectionInterface::UpdateTargetImagesList()
{
   int currentIdx = GUI->TargetImages_TreeBox.ChildIndex( GUI->TargetImages_TreeBox.CurrentNode() );

   GUI->TargetImages_TreeBox.DisableUpdates();
   GUI->TargetImages_TreeBox.Clear();

   for ( size_type i = 0; i < instance.p_targetFrames.Length(); ++i )
   {
      new TreeBox::Node( GUI->TargetImages_TreeBox );
      UpdateTargetImageItem( i );
   }

   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 0 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 1 );
   GUI->TargetImages_TreeBox.AdjustColumnWidthToContents( 2 );

   if ( !instance.p_targetFrames.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->TargetImages_TreeBox.NumberOfChildren() )
         GUI->TargetImages_TreeBox.SetCurrentNode( GUI->TargetImages_TreeBox[currentIdx] );

   GUI->TargetImages_TreeBox.EnableUpdates();
}

void CosmeticCorrectionInterface::UpdateImageSelectionButtons()
{
   bool hasItems = GUI->TargetImages_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->TargetImages_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAll_PushButton.Enable( hasItems );
   GUI->InvertSelection_PushButton.Enable( hasItems );
   GUI->ToggleSelected_PushButton.Enable( hasSelection );
   GUI->RemoveSelected_PushButton.Enable( hasSelection );
   GUI->Clear_PushButton.Enable( hasItems );
}

void CosmeticCorrectionInterface::UpdateMasterDarkControls()
{
   #if debug
   Console().WriteLn( "UpdateMasterDarkControls():instance.p_masterDark:" + instance.p_masterDark );
   #endif

   const int hotPrecision = TheHotLevel->Precision();
   const double hotMinRound = Round<double>( m_Mean, hotPrecision );
   const double hotMaxRound = Round<double>( Histogram().NormalizedLevel( m_MaxSlider ), hotPrecision );
   GUI->HotDarkLevel_NumericControl.SetRange( hotMinRound, hotMaxRound );

   const int coldPrecision = TheHotLevel->Precision();
   const double coldMaxRound = Round<double>( m_Mean, coldPrecision );
   const double coldMinRound = Round<double>( Histogram().NormalizedLevel( m_MinSlider ), coldPrecision );
   GUI->ColdDarkLevel_NumericControl.SetRange( coldMinRound, coldMaxRound);

   GUI->UseMasterDark_SectionBar.SetChecked( instance.p_useMasterDark );
   GUI->MasterDarkPath_Edit.SetText( instance.p_masterDark );

   GUI->Hot_GroupBox.Enable( m_md != 0 );
   GUI->Cold_GroupBox.Enable( m_md != 0 );

   GUI->HotDark_CheckBox.SetChecked( instance.p_hotDarkCheck );
   GUI->HotDarkLevel_NumericControl.SetValue( instance.p_hotDarkLevel );
   GUI->ColdDark_CheckBox.SetChecked( instance.p_coldDarkCheck );
   GUI->ColdDarkLevel_NumericControl.SetValue( instance.p_coldDarkLevel );
   if ( m_md != 0 )
   {
      HotLevelUpdated( instance.p_hotDarkLevel, true );      // Level >> Qty; Level >> Sigma
      ColdLevelUpdated( instance.p_coldDarkLevel, true );   // Level >> Qty; Level >> Sigma
   }
   else
   {
      GUI->HotDarkQty_NumericControl.SetValue( 0 );
      HotUpdateGUI( 0 );

      GUI->ColdDarkQty_NumericControl.SetValue( 0 );
      ColdUpdateGUI( 0 );
   }
}

void CosmeticCorrectionInterface::ClearMasterDark()
{
   #if debug
   Console().WriteLn( "ClearMasterDark()");
   #endif
   instance.p_masterDark = TheOutputDir->DefaultValue();
   if ( m_md != 0 )
	{
		delete m_md, m_md = 0;
		m_H.Clear();
	}
   UpdateMasterDarkControls();
   s_requiresUpdate = true;
   RTPUpdate();
}

void CosmeticCorrectionInterface::UpdateAutoDetectControls()
{
   GUI->UseAutoDetect_SectionBar.SetChecked( instance.p_useAutoDetect );
   GUI->DetectHot_CheckBox.SetChecked( instance.p_hotAutoCheck );
   GUI->DetectHot_NumericControl.SetValue( instance.p_hotAutoValue );
   GUI->DetectCold_CheckBox.SetChecked( instance.p_coldAutoCheck );
   GUI->DetectCold_NumericControl.SetValue( instance.p_coldAutoValue );
}

void CosmeticCorrectionInterface::UpdateDefectListItem( size_type i )
{
   TreeBox::Node* node = GUI->DefectList_TreeBox[int(i)];
   if ( node == 0 ) return;

   const CosmeticCorrectionInstance::DefectItem& item = instance.p_defects[i];

   node->Check( item.enabled );
   node->SetText( 0, item.isRow ? "Row": "Col"  );
   node->SetText( 1, String( int(item.address) ) );
   if( item.isRange )
   {
      node->SetText( 2, String( int( item.begin ) ) );
      node->SetText( 3, String( int( item.end ) ) );
   }
   else node->SetText( 2, "All" );
}

void CosmeticCorrectionInterface::UpdateDefectListControls()
{
   GUI->UseDefectList_SectionBar.SetChecked( instance.p_useDefectList );

   int currentIdx = GUI->DefectList_TreeBox.ChildIndex( GUI->DefectList_TreeBox.CurrentNode() );

   GUI->DefectList_TreeBox.DisableUpdates();
   GUI->DefectList_TreeBox.Clear();

   for ( size_type i = 0; i < instance.p_defects.Length(); ++i )
   {
      new TreeBox::Node( GUI->DefectList_TreeBox );
      UpdateDefectListItem( i );
   }

   if ( !instance.p_defects.IsEmpty() )
      if ( currentIdx >= 0 && currentIdx < GUI->DefectList_TreeBox.NumberOfChildren() )
         GUI->DefectList_TreeBox.SetCurrentNode( GUI->DefectList_TreeBox[currentIdx] );

   for ( int i =0; i < GUI->DefectList_TreeBox.NumberOfColumns(); i++)
      GUI->DefectList_TreeBox.AdjustColumnWidthToContents( i );

   GUI->DefectList_TreeBox.EnableUpdates();
   UpdateDefectListSelectionButtons();
}

void CosmeticCorrectionInterface::UpdateDefectListSelectionButtons()
{
   bool hasItems = GUI->DefectList_TreeBox.NumberOfChildren() > 0;
   bool hasSelection = hasItems && GUI->DefectList_TreeBox.HasSelectedTopLevelNodes();

   GUI->SelectAllDefect_PushButton.Enable( hasItems );
   GUI->InvertSelectionDefect_PushButton.Enable( hasItems );
   GUI->ToggleSelectedDefect_PushButton.Enable( hasSelection );
   GUI->RemoveSelectedDefect_PushButton.Enable( hasSelection );
   GUI->ClearDefect_PushButton.Enable( hasItems );
   GUI->SaveList_PushButton.Enable( hasItems );
}
void CosmeticCorrectionInterface::AddDefect( const bool isRow, const int position, const bool setRange, const int start, const int end )
{
   instance.p_defects.Add( CosmeticCorrectionInstance::DefectItem( true, isRow, position, setRange, start, end ) );
   UpdateDefectListControls();
}
//-------------------------------------------------------------------------------------
void CosmeticCorrectionInterface::SelectDir()
{
   GetDirectoryDialog d;
   d.SetCaption( "CosmeticCorrection: Select Output Directory" );
   if ( d.Execute() )
      GUI->OutputDir_Edit.SetText( instance.p_outputDir = d.Directory() );
}

void CosmeticCorrectionInterface::__TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   // Actually do nothing (placeholder). Just perform a sanity check.
   int index = sender.ChildIndex( &current );
   if ( index < 0 || size_type( index ) >= instance.p_targetFrames.Length() )
      throw Error( "CosmeticCorrectionInterface: *Warning* Corrupted interface structures" );

   // ### If there's something else that depends on which image is selected in the list, do it here.
}

void CosmeticCorrectionInterface::__NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   #if debug
   Console().WriteLn( "__NodeActivated()");
   #endif

   int index = sender.ChildIndex( &node );
   if ( sender == GUI->TargetImages_TreeBox )
   {
      CosmeticCorrectionInstance::ImageItem& item = instance.p_targetFrames[index];

      if ( col == 1 ) // Activate the item's checkmark: toggle item's enabled state.
      {
         item.enabled = !item.enabled;
         UpdateTargetImageItem( index );
      }
      else if ( col == 2 ) // Activate the item's path: open the image.
      {
         FileShow(item.path);
      }
   }
}

void CosmeticCorrectionInterface::__NodeUpdated( TreeBox& sender, TreeBox::Node& node, int col )
{
   #if debug
   Console().WriteLn( "__NodeUpdated()");
   #endif

   if ( sender == GUI->DefectList_TreeBox )
   {
      const int index = sender.ChildIndex( &node );
      instance.p_defects[index].enabled = node.IsChecked();
      UpdateDefectListItem( index );
      s_requiresListMapGeneration = true;
      RTPUpdate();
   }
}

void CosmeticCorrectionInterface::__NodeSelectionUpdated( TreeBox& sender )
{
   if ( sender == GUI->TargetImages_TreeBox )
      UpdateImageSelectionButtons();
   else if ( sender == GUI->DefectList_TreeBox )
      UpdateDefectListSelectionButtons();
}

static size_type TreeInsertionIndex( const TreeBox& tree )
{
   const TreeBox::Node* n = tree.CurrentNode();
   return (n != 0) ? tree.ChildIndex( n ) + 1 : tree.NumberOfChildren();
}

void CosmeticCorrectionInterface::__TargetImages_Click( Button& sender, bool checked )
{
   if ( sender == GUI->AddFiles_PushButton )
   {
      OpenFileDialog d;
      d.LoadImageFilters();
      d.EnableMultipleSelections();
      d.SetCaption( "CosmeticCorrection: Select Target Frames" );
      if ( d.Execute() )
      {
         size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
         for ( StringList::const_iterator i = d.FileNames().Begin(); i != d.FileNames().End(); ++i )
            instance.p_targetFrames.Insert( instance.p_targetFrames.At( i0++ ), CosmeticCorrectionInstance::ImageItem( *i ) );
         UpdateTargetImagesList();
         UpdateImageSelectionButtons();
      }
   }
   else if ( sender == GUI->SelectAll_PushButton )
   {
      GUI->TargetImages_TreeBox.SelectAllNodes();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->InvertSelection_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         GUI->TargetImages_TreeBox[i]->Select( !GUI->TargetImages_TreeBox[i]->IsSelected() );
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->ToggleSelected_PushButton )
   {
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( GUI->TargetImages_TreeBox[i]->IsSelected() )
            instance.p_targetFrames[i].enabled = !instance.p_targetFrames[i].enabled;
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->RemoveSelected_PushButton )
   {
      CosmeticCorrectionInstance::image_list newTargets;
      for ( int i = 0, n = GUI->TargetImages_TreeBox.NumberOfChildren(); i < n; ++i )
         if ( !GUI->TargetImages_TreeBox[i]->IsSelected() )
            newTargets.Add( instance.p_targetFrames[i] );
      instance.p_targetFrames = newTargets;
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->Clear_PushButton )
   {
      instance.p_targetFrames.Clear();
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->FullPaths_CheckBox )
   {
      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
}

void CosmeticCorrectionInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start ) GUI->TargetImages_TreeBox.SetFixedHeight();
   else GUI->TargetImages_TreeBox.SetVariableHeight();
}

void CosmeticCorrectionInterface::__MouseDoubleClick( Control& sender, const Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( sender == GUI->MasterDarkPath_Edit )
   {
      if ( instance.p_masterDark.IsEmpty() )
         SelectMasterFrameDialog();
      else
         FileShow(instance.p_masterDark);
   }
   else if ( sender == GUI->OutputDir_Edit )
      SelectDir();
}

void CosmeticCorrectionInterface::__EditCompleted( Edit& sender )
{
   if ( !sender.IsModified() ) return;

   //static bool busy = false;
   //if ( busy ) return;
   //busy = true;
   sender.SetModified( false );

   //try
   //{

   #if debug
   Console().WriteLn( "__EditCompleted()" );
   #endif

   String text = sender.Text().Trimmed();

   if ( sender == GUI->Prefix_Edit )
   instance.p_prefix = text;
   else if ( sender == GUI->Postfix_Edit )
      instance.p_postfix = text;
   else if ( sender == GUI->OutputDir_Edit )
      instance.p_outputDir = text;
   else if ( sender == GUI->MasterDarkPath_Edit )
   {
      if ( text.IsEmpty() )
      {
         if ( !instance.p_masterDark.IsEmpty() ) ClearMasterDark();
      }
      else if ( text != instance.p_masterDark )
      {
         if ( !File::Exists( text ) )
         {
            Console().WriteLn("No such file!");
            //MessageBox( "No such file!" ).Execute();
            text = instance.p_masterDark;
         }
         else
         {
            instance.p_masterDark = text;
            //open new MasteDark
            LoadMasterDark( instance.p_masterDark );
         }
      }
   }
   sender.SetText( text );

   //busy = false;
   //}
   //catch ( ... )
   //{
   //   busy = false;
   //   throw;
   //}
}

void CosmeticCorrectionInterface::__Button_Click( Button& sender, bool checked )
{
   #if debug
   Console().WriteLn( "__Button_Click():" + sender.Text() );
   Console().Flush();
   #endif

   if ( sender == GUI->OutputDir_SelectButton )
      SelectDir();

   else if ( sender == GUI->OutputDir_ClearButton )
      GUI->OutputDir_Edit.SetText( instance.p_outputDir = TheOutputDir->DefaultValue() );

   else if ( sender == GUI->Overwrite_CheckBox )
      instance.p_overwrite = checked;

   else if ( sender == GUI->MasterDarkPath_SelectButton )
      SelectMasterFrameDialog();

   else if ( sender == GUI->MasterDarkPath_ClearButton )
   {
      ClearMasterDark();
   }
   else if ( sender == GUI->CFA_CheckBox )
   {
      instance.p_cfa = checked;
		if ( !m_H.IsEmpty() )
			ColdLevelUpdated(instance.p_coldDarkLevel, true ); // true = CFA Click
      s_requiresUpdate = true;
      //if ( GUI->RTP_Control.IsEnabled() )   Console().Show();
      RTPUpdate();
   }
   else if ( sender == GUI->HotDark_CheckBox )
   {
      instance.p_hotDarkCheck = checked;
      s_requiresDarkHotMapGeneration = true;
      //if ( GUI->RTP_Control.IsEnabled() ) Console().Show(checked);
      RTPUpdate();
   }
   else if ( sender == GUI->ColdDark_CheckBox )
   {
      instance.p_coldDarkCheck = checked;
      s_requiresDarkColdMapGeneration = true;
      //if ( GUI->RTP_Control.IsEnabled() ) Console().Show(checked);
      RTPUpdate();
   }
   else if ( sender == GUI->DetectHot_CheckBox )
   {
      instance.p_hotAutoCheck = checked;
      s_requiresAutoHotMapGeneration = true;
      //if ( GUI->RTP_Control.IsEnabled() ) Console().Show(checked);
      RTPUpdate();
   }
   else if ( sender == GUI->DetectCold_CheckBox )
   {
      instance.p_coldAutoCheck = checked;
      s_requiresAutoColdMapGeneration = true;
      //if ( GUI->RTP_Control.IsEnabled() ) Console().Show(checked);
      RTPUpdate();
   }
   else if ( sender == GUI->RTPShowMap_CheckBox )
   {
      s_requiresUpdate = true;
      RTPUpdate();
   }
   else if ( sender == GUI->RTPSnapshot_PushButton )
   {
      s_requiresSnapshot = true;
      RTPUpdate();
   }
   else if ( sender == GUI->AddDefect_PushButton )
   {
      AddDefect( GUI->Row_RadioButton.IsChecked(), GUI->DefectPosition.Value(), GUI->SelectRange_CheckBox.IsChecked(), GUI->DefectStart.Value(), GUI->DefectEnd.Value() );
      s_requiresListMapGeneration = true;
      RTPUpdate();
   }
   else if ( sender == GUI->ClearDefect_PushButton )
   {
      instance.p_defects.Clear();
      GUI->DefectList_TreeBox.Clear();
      UpdateDefectListSelectionButtons();
      s_requiresListMapGeneration = true;
      RTPUpdate();
   }
   else if ( sender == GUI->RemoveSelectedDefect_PushButton )
   {
      GUI->DefectList_TreeBox.DisableUpdates();
      for ( int i = GUI->DefectList_TreeBox.NumberOfChildren(); --i >= 0; )
         if ( GUI->DefectList_TreeBox.Child( i )->IsSelected() )
         {
            instance.p_defects.Remove( instance.p_defects.At( i ) );
            GUI->DefectList_TreeBox.Remove( i );
         }
      GUI->DefectList_TreeBox.EnableUpdates();
      s_requiresListMapGeneration = true;
      RTPUpdate();
   }
   else if ( sender == GUI->ToggleSelectedDefect_PushButton )
   {
      for ( int i = GUI->DefectList_TreeBox.NumberOfChildren(); --i >= 0; )
         if ( GUI->DefectList_TreeBox.Child( i )->IsSelected() )
         {
            const bool isChecked = GUI->DefectList_TreeBox.Child( i )->IsChecked();
            GUI->DefectList_TreeBox.Child( i )->Uncheck( isChecked );
            instance.p_defects[i].enabled = !isChecked;
         }
      s_requiresListMapGeneration = true;
      RTPUpdate();
   }
   else if ( sender == GUI->InvertSelectionDefect_PushButton )
   {
      for ( int i = GUI->DefectList_TreeBox.NumberOfChildren(); --i >= 0; )
         GUI->DefectList_TreeBox.Child( i )->Select( !GUI->DefectList_TreeBox.Child( i )->IsSelected() );
      UpdateDefectListSelectionButtons();
   }
   else if ( sender == GUI->SelectAllDefect_PushButton )
   {
      GUI->DefectList_TreeBox.SelectAllNodes();
      UpdateDefectListSelectionButtons();
   }
   else if ( sender == GUI->LoadList_PushButton )
   {
      OpenFileDialog d;
      d.SetCaption( "CosmeticCorrection: Load Defect Map List" );
      d.SetFilter( FileFilter( "Text Files", ".txt" ) );
      if ( !d.Execute() )
         return;

      IsoStringList list( File::ReadLines( d.FileName(), ReadTextOption::RemoveEmptyLines|ReadTextOption::TrimTrailingSpaces|ReadTextOption::TrimLeadingSpaces ) );
      instance.p_defects.Clear();
      for ( size_t i = 0; i < list.Length(); i++ )
      {
         IsoString line( list[i] );               //get one line
         StringList s;
         line.Break( s, ' ');                     //breake line to Array<String>
         const bool setRange = s.Length() > 2;   //if more then 2 string value available >> swich to Range mode, else select All pixels in the col/row
         instance.p_defects.Add( CosmeticCorrectionInstance::DefectItem( true, s[0] == "Row", s[1].ToUInt(), setRange, setRange ? s[2].ToUInt() : 0 , setRange ? s[3].ToUInt() : 0  ) );
      }
      UpdateDefectListControls();
      s_requiresListMapGeneration = true;
      RTPUpdate();
   }
   else if ( sender == GUI->SaveList_PushButton )
   {
      const int n = int( instance.p_defects.Length() );
      if ( n == 0 ) return;
      SaveFileDialog d;
      d.SetCaption( "CosmeticCorrection: Save Defect Map List" );
      d.SetFilter( FileFilter( "Text Files", ".txt" ) );
      d.EnableOverwritePrompt();
      if ( !d.Execute() ) return;

      File f( d.FileName(), FileMode::Write|FileMode::Create );

      for ( int i = 0; i < n; i++ )
      {
         const CosmeticCorrectionInstance::DefectItem& item = instance.p_defects[i];
         f.OutText( item.isRow ? "Row":"Col");
         f.OutText( ' ' + IsoString( item.address ) );
         if ( item.isRange )
            f.OutText( ' ' + IsoString( item.begin ) + ' ' + IsoString( item.end ) );

         f.OutTextLn( IsoString() );
      }
      f.Close();
   }
   else if ( sender == GUI->SelectRange_CheckBox )
   {
      GUI->DefectEnd.Enable( checked );
      GUI->DefectStart.Enable( checked );
   }
}

void CosmeticCorrectionInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->HotDarkLevel_NumericControl )        HotLevelUpdated( value );
   else if ( sender == GUI->HotDarkSigma_NumericControl )   HotSigmaUpdated( value );
   else if ( sender == GUI->HotDarkQty_NumericControl )     HotQtyUpdated( value );
   else if ( sender == GUI->ColdDarkLevel_NumericControl )  ColdLevelUpdated( value );
   else if ( sender == GUI->ColdDarkSigma_NumericControl )  ColdSigmaUpdated( value );
   else if ( sender == GUI->ColdDarkQty_NumericControl )    ColdQtyUpdated( value );
   else if ( sender == GUI->Amount_NumericControl )         instance.p_amount = value;
   else if ( sender == GUI->DetectHot_NumericControl )
   {
      instance.p_hotAutoValue = value;
      if ( instance.p_hotAutoCheck )
         s_requiresAutoHotMapGeneration = true;
   }
   else if ( sender == GUI->DetectCold_NumericControl )
   {
      instance.p_coldAutoValue = value;
      if ( instance.p_coldAutoCheck )
         s_requiresAutoColdMapGeneration = true;
   }
   if (   s_requiresDarkHotMapGeneration || s_requiresDarkColdMapGeneration || s_requiresAutoHotMapGeneration || s_requiresAutoColdMapGeneration
      || (   sender == GUI->Amount_NumericControl && !GUI->RTPShowMap_CheckBox.IsChecked()
            &&   ( instance.p_hotDarkCheck || instance.p_hotAutoCheck || instance.p_coldDarkCheck || instance.p_coldAutoCheck )
         )
      )
   {
      RealTimePreview::Update();
   }
}

void CosmeticCorrectionInterface::__CheckSection( SectionBar& sender, bool checked )
{
   GUI->TargetImages_TreeBox.SetFixedHeight();
   if ( checked ) sender.Section().Show();
   else sender.Section().Hide();
   AdjustToContents();
   GUI->TargetImages_TreeBox.SetVariableHeight();

   if ( sender == GUI->UseMasterDark_SectionBar )
   {
      instance.p_useMasterDark = checked;
      // N.B.: The code below has been disabled because it prevents using
      // FileDrop events on GUI->MasterDarkPath_Edit
//       if ( checked && !m_md )         // if master dark is not loaded
//          SelectMasterFrameDialog();   // forse to select master dark
   }
   else if ( sender == GUI->UseAutoDetect_SectionBar ) instance.p_useAutoDetect = checked;
   else if ( sender == GUI->UseDefectList_SectionBar ) instance.p_useDefectList = checked;

   if ( sender == GUI->UseMasterDark_SectionBar || sender == GUI->UseAutoDetect_SectionBar || sender == GUI->UseDefectList_SectionBar )
   {
      s_requiresUpdate = true;
      RTPUpdate();
   }
}

void CosmeticCorrectionInterface::__FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
      wantsFiles = true;
   else if ( sender == GUI->OutputDir_Edit )
      wantsFiles = files.Length() == 1 && File::DirectoryExists( files[0] );
   else if ( sender == GUI->UseMasterDark_SectionBar || sender == GUI->MasterDarkPath_Edit )
      wantsFiles = files.Length() == 1 && File::Exists( files[0] );
}

void CosmeticCorrectionInterface::__FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers )
{
   if ( sender == GUI->TargetImages_TreeBox.Viewport() )
   {
      StringList inputFiles;
      bool recursive = IsControlOrCmdPressed();
      for ( const String& item : files )
         if ( File::Exists( item ) )
            inputFiles << item;
         else if ( File::DirectoryExists( item ) )
            inputFiles << FileFormat::SupportedImageFiles( item, true/*toRead*/, false/*toWrite*/, recursive );

      inputFiles.Sort();
      size_type i0 = TreeInsertionIndex( GUI->TargetImages_TreeBox );
      for ( const String& file : inputFiles )
         instance.p_targetFrames.Insert( instance.p_targetFrames.At( i0++ ), CosmeticCorrectionInstance::ImageItem( file ) );

      UpdateTargetImagesList();
      UpdateImageSelectionButtons();
   }
   else if ( sender == GUI->OutputDir_Edit )
   {
      if ( File::DirectoryExists( files[0] ) )
         GUI->OutputDir_Edit.SetText( instance.p_outputDir = files[0] );
   }
   else if ( sender == GUI->UseMasterDark_SectionBar || sender == GUI->MasterDarkPath_Edit )
   {
      if ( File::Exists( files[0] ) )
      {
         if ( instance.p_masterDark != files[0] )
            LoadMasterDark( instance.p_masterDark = files[0] );
         GUI->UseMasterDark_Control.Show();
      }
   }
}

// ----------------------------------------------------------------------------

CosmeticCorrectionInterface::GUIData::GUIData( CosmeticCorrectionInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Sigma:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( "Output extension:" ) + 'M' );

   //

   TargetImages_SectionBar.SetTitle( "Target Frames" );
   TargetImages_SectionBar.SetSection( TargetImages_Control );
   TargetImages_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CosmeticCorrectionInterface::__ToggleSection, w );

   TargetImages_TreeBox.SetNumberOfColumns( 3 );
   TargetImages_TreeBox.HideHeader();
   TargetImages_TreeBox.SetScaledMinWidth( 300 );
   TargetImages_TreeBox.EnableMultipleSelections();
   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&CosmeticCorrectionInterface::__TargetImages_CurrentNodeUpdated, w );
   TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&CosmeticCorrectionInterface::__NodeActivated, w );
   TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&CosmeticCorrectionInterface::__NodeSelectionUpdated, w );
   TargetImages_TreeBox.SetToolTip( "<p>Double click a file name to open the file as a new image window.</p>" );
   TargetImages_TreeBox.Viewport().OnFileDrag( (Control::file_drag_event_handler)&CosmeticCorrectionInterface::__FileDrag, w );
   TargetImages_TreeBox.Viewport().OnFileDrop( (Control::file_drop_event_handler)&CosmeticCorrectionInterface::__FileDrop, w );

   AddFiles_PushButton.SetText( "Add Files" );
   AddFiles_PushButton.SetToolTip( "<p>Add existing image files to the list of target frames.</p>" );
   AddFiles_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__TargetImages_Click, w );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all target frames.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__TargetImages_Click, w );

   InvertSelection_PushButton.SetText( "Invert Selection" );
   InvertSelection_PushButton.SetToolTip( "<p>Invert the current selection of target frames.</p>" );
   InvertSelection_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__TargetImages_Click, w );

   ToggleSelected_PushButton.SetText( "Toggle Selected" );
   ToggleSelected_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected target frames.</p>"
      "<p>Disabled target frames will be ignored during the CosmeticCorrection process.</p>" );
   ToggleSelected_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__TargetImages_Click, w );

   RemoveSelected_PushButton.SetText( "Remove Selected" );
   RemoveSelected_PushButton.SetToolTip( "<p>Remove all currently selected target frames.</p>" );
   RemoveSelected_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__TargetImages_Click, w );

   Clear_PushButton.SetText( "Clear" );
   Clear_PushButton.SetToolTip( "<p>Clear the list of target frames.</p>" );
   Clear_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__TargetImages_Click, w );

   FullPaths_CheckBox.SetText( "Full paths" );
   FullPaths_CheckBox.SetToolTip( "<p>Show full paths for target frame files.</p>" );
   FullPaths_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__TargetImages_Click, w );

   TargetButtons_Sizer.SetSpacing( 4 );
   TargetButtons_Sizer.Add( AddFiles_PushButton );
   TargetButtons_Sizer.Add( SelectAll_PushButton );
   TargetButtons_Sizer.Add( InvertSelection_PushButton );
   TargetButtons_Sizer.Add( ToggleSelected_PushButton );
   TargetButtons_Sizer.Add( RemoveSelected_PushButton );
   TargetButtons_Sizer.Add( Clear_PushButton );
   TargetButtons_Sizer.Add( FullPaths_CheckBox );
   TargetButtons_Sizer.AddStretch();

   TargetImages_Sizer.SetSpacing( 4 );
   TargetImages_Sizer.Add( TargetImages_TreeBox, 100 );
   TargetImages_Sizer.Add( TargetButtons_Sizer );

   TargetImages_Control.SetSizer( TargetImages_Sizer );

   //---------------------------------------------------
   Output_SectionBar.SetTitle( "Output" );
   Output_SectionBar.SetSection( Output_Control );
   Output_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CosmeticCorrectionInterface::__ToggleSection, w );

   const char* ToolTipOutputDir = "<p>This is the directory (or folder) where all output files "
      "will be written.</p>"
      "<p>If this field is left blank, output files will be written to the same directories as their "
      "corresponding target files. In this case, make sure that source directories are writable, or the "
      "CosmeticCorrection process will fail.</p>";

   OutputDir_Label.SetText( "Output directory:" );
   OutputDir_Label.SetFixedWidth( labelWidth2 );
   OutputDir_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OutputDir_Label.SetToolTip( ToolTipOutputDir );

   OutputDir_Edit.SetToolTip( ToolTipOutputDir );
   OutputDir_Edit.OnMouseDoubleClick( (Control::mouse_event_handler)&CosmeticCorrectionInterface::__MouseDoubleClick, w );
   OutputDir_Edit.OnEditCompleted( (Edit::edit_event_handler)&CosmeticCorrectionInterface::__EditCompleted, w );
   OutputDir_Edit.OnFileDrag( (Control::file_drag_event_handler)&CosmeticCorrectionInterface::__FileDrag, w );
   OutputDir_Edit.OnFileDrop( (Control::file_drop_event_handler)&CosmeticCorrectionInterface::__FileDrop, w );

   OutputDir_SelectButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputDir_SelectButton.SetScaledFixedSize( 20, 20 );
   OutputDir_SelectButton.SetToolTip( "<p>Select output directory</p>" );
   OutputDir_SelectButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   OutputDir_ClearButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/clear.png" ) ) );
   OutputDir_ClearButton.SetScaledFixedSize( 20, 20 );
   OutputDir_ClearButton.SetToolTip( "<p>Reset output directory</p>" );
   OutputDir_ClearButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   OutputDir_Sizer.SetSpacing( 4 );
   OutputDir_Sizer.Add( OutputDir_Label );
   OutputDir_Sizer.Add( OutputDir_Edit, 100 );
   OutputDir_Sizer.Add( OutputDir_SelectButton );
   OutputDir_Sizer.Add( OutputDir_ClearButton );

   const char* ToolTipPrefix =
      "<p>This is a prefix that will be appended to the file name of each corrected image.</p>";
   Prefix_Label.SetText( "Prefix:" );
   Prefix_Label.SetFixedWidth( labelWidth2 );
   Prefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Prefix_Label.SetToolTip( ToolTipPrefix );
   Prefix_Edit.SetFixedWidth( fnt.Width( String( 'M', 6 ) ) );
   Prefix_Edit.SetToolTip( ToolTipPrefix );
   Prefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&CosmeticCorrectionInterface::__EditCompleted, w );

   const char* ToolTipPostfix =
      "<p>This is a postfix that will be appended to the file name of each corrected image.</p>";
   Postfix_Label.SetText( "Postfix:" );
   Postfix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Postfix_Label.SetToolTip( ToolTipPostfix );
   Postfix_Edit.SetFixedWidth( fnt.Width( String( 'M', 6 ) ) );
   Postfix_Edit.SetToolTip( ToolTipPostfix );
   Postfix_Edit.OnEditCompleted( (Edit::edit_event_handler)&CosmeticCorrectionInterface::__EditCompleted, w );

   OutputChunks_Sizer.SetSpacing( 4 );
   OutputChunks_Sizer.Add( Prefix_Label );
   OutputChunks_Sizer.Add( Prefix_Edit );
   OutputChunks_Sizer.AddSpacing( 12 );
   OutputChunks_Sizer.Add( Postfix_Label );
   OutputChunks_Sizer.Add( Postfix_Edit );
   OutputChunks_Sizer.AddStretch();

   CFA_CheckBox.SetText( "CFA" );
   CFA_CheckBox.SetToolTip( "<p>Enable this option if the image has been mosaiced with a "
      "Color Filter Array (CFA) or Bayer matrix (OSC CCD, DSLR camera).<br/>"
      "Disable if the image comes from a monochrome imager or has already been deBayered.</p>" );
   CFA_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   CFA_Sizer.AddUnscaledSpacing( labelWidth2 );
   CFA_Sizer.AddSpacing( 4 );
   CFA_Sizer.Add( CFA_CheckBox );
   CFA_Sizer.AddStretch();

   Overwrite_CheckBox.SetText( "Overwrite" );
   Overwrite_CheckBox.SetToolTip( "<p>If this option is selected, CosmeticCorrection will p_overwrite "
      "existing files with the same names as generated output files. This can be dangerous because the original "
      "contents of overwritten files will be lost.</p>"
      "<p><b>Enable this option <u>at your own risk.</u></b></p>" );
   Overwrite_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   Overwrite_Sizer.AddUnscaledSpacing( labelWidth2 );
   Overwrite_Sizer.AddSpacing( 4 );
   Overwrite_Sizer.Add( Overwrite_CheckBox );
   Overwrite_Sizer.AddStretch();

   const char* ToolTipTransferFunction = "<p>0 = No correction.<br/>1 = 100% corrected.<p>";
   Amount_NumericControl.label.SetText( "Amount:" );
   Amount_NumericControl.label.SetFixedWidth( labelWidth2 );
   Amount_NumericControl.label.SetToolTip( ToolTipTransferFunction );
   Amount_NumericControl.slider.SetRange( TheAmount->MinimumValue(), TheAmount->MaximumValue()*65535 );
   Amount_NumericControl.SetRange( TheAmount->MinimumValue(), TheAmount->MaximumValue() );
   Amount_NumericControl.SetPrecision( TheAmount->Precision() );
   Amount_NumericControl.SetToolTip( ToolTipTransferFunction );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   //---------------------------------------------------
   Output_Sizer.SetSpacing( 4 );
   Output_Sizer.Add( OutputDir_Sizer );
   Output_Sizer.Add( OutputChunks_Sizer );
   Output_Sizer.Add( CFA_Sizer );
   Output_Sizer.Add( Overwrite_Sizer );
   Output_Sizer.Add( Amount_NumericControl );

   Output_Control.SetSizer( Output_Sizer );
   Output_Control.AdjustToContents();

   //---------------------------------------------------
   // detect via MasterDark section

   UseMasterDark_SectionBar.EnableTitleCheckBox();
   UseMasterDark_SectionBar.SetTitle( "Use Master Dark" );
   UseMasterDark_SectionBar.SetSection( UseMasterDark_Control );
   UseMasterDark_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CosmeticCorrectionInterface::__ToggleSection, w );
   UseMasterDark_SectionBar.OnCheck( (SectionBar::check_event_handler)&CosmeticCorrectionInterface::__CheckSection, w );
   UseMasterDark_SectionBar.OnFileDrag( (Control::file_drag_event_handler)&CosmeticCorrectionInterface::__FileDrag, w );
   UseMasterDark_SectionBar.OnFileDrop( (Control::file_drop_event_handler)&CosmeticCorrectionInterface::__FileDrop, w );

   const char* MasterDarkPathToolTip = "<p>Select a master dark frame file.</p>";
   MasterDarkPath_Edit.SetToolTip( MasterDarkPathToolTip );
   MasterDarkPath_Edit.OnEditCompleted( (Edit::edit_event_handler)&CosmeticCorrectionInterface::__EditCompleted, w );
   MasterDarkPath_Edit.OnMouseDoubleClick( (Control::mouse_event_handler)&CosmeticCorrectionInterface::__MouseDoubleClick, w );
   MasterDarkPath_Edit.OnFileDrag( (Control::file_drag_event_handler)&CosmeticCorrectionInterface::__FileDrag, w );
   MasterDarkPath_Edit.OnFileDrop( (Control::file_drop_event_handler)&CosmeticCorrectionInterface::__FileDrop, w );

   MasterDarkPath_SelectButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   MasterDarkPath_SelectButton.SetScaledFixedSize( 20, 20 );
   MasterDarkPath_SelectButton.SetToolTip( MasterDarkPathToolTip );
   MasterDarkPath_SelectButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   MasterDarkPath_ClearButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/clear.png" ) ) );
   MasterDarkPath_ClearButton.SetScaledFixedSize( 20, 20 );
   MasterDarkPath_ClearButton.SetToolTip( "<p>Reset master dark.</p>" );
   MasterDarkPath_ClearButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   MasterDarkPath_Sizer.SetMargin( 6 );
   MasterDarkPath_Sizer.SetSpacing( 2 );
   MasterDarkPath_Sizer.Add( MasterDarkPath_Edit, 100 );
   MasterDarkPath_Sizer.Add( MasterDarkPath_SelectButton );
   MasterDarkPath_Sizer.Add( MasterDarkPath_ClearButton );

   MasterDark_Sizer.Add( MasterDarkPath_Sizer );

   MasterDark_GroupBox.SetTitle( "Master Dark" );
   MasterDark_GroupBox.SetSizer( MasterDark_Sizer );
   //MasterDark_GroupBox.AdjustToContents();
   //MasterDark_GroupBox.SetFixedHeight();


   //---------------------------------------------------
   // Hot

   const char* ToolTipLevel = "<p>Define the %s pixel clipping level.</p>";
   const char* ToolTipQty = "<p>Quantity: How many %s pixels you want to replace?</p>";
   const char* ToolTipRealQty = "<p>At the selected <i>Level</i>, <i>Qty</i> is the actual amount of pixels "
      "that will be processed.<br/>"
      "<i>Sigma</i> is the corresponding standard deviation from the mean.</p>";
   const char* ToolTipUse = "<p>Use a master dark frame to detect and correct %s pixels.</p>";

   HotDark_CheckBox.SetText( "Enable" );
   HotDark_CheckBox.SetToolTip( String().Format( ToolTipUse, "hot" ) );
   HotDark_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   HotDarkLevel_NumericControl.label.SetText( "Level:" );
   HotDarkLevel_NumericControl.label.SetFixedWidth( labelWidth1 );
   HotDarkLevel_NumericControl.slider.SetScaledMinWidth( 200 );
   HotDarkLevel_NumericControl.slider.SetRange( 0, 0x7fffffff );
   HotDarkLevel_NumericControl.SetPrecision( TheHotLevel->Precision() );
   HotDarkLevel_NumericControl.SetToolTip( String().Format( ToolTipLevel, "hot" ) );
   HotDarkLevel_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   HotDarkSigma_NumericControl.label.SetText( "Sigma:" );
   HotDarkSigma_NumericControl.label.SetFixedWidth( labelWidth1 );
   HotDarkSigma_NumericControl.slider.SetScaledMinWidth( 200 );
   HotDarkSigma_NumericControl.slider.SetRange( 0, 0x7fffffff );
   HotDarkSigma_NumericControl.SetRange( 0, 50 );
   HotDarkSigma_NumericControl.SetPrecision( 5 );
   //HotDarkSigma_NumericControl.SetToolTip( String().Format( ToolTipLevel, "hot" ) );
   HotDarkSigma_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   HotDarkQty_NumericControl.label.SetText( "Qty:" );
   HotDarkQty_NumericControl.label.SetFixedWidth( labelWidth1 );
   HotDarkQty_NumericControl.slider.SetScaledMinWidth( 200 );
   HotDarkQty_NumericControl.slider.SetRange( 0, 10000 );
   HotDarkQty_NumericControl.SetInteger();
   HotDarkQty_NumericControl.SetRange( 0, 10000 );
   HotDarkQty_NumericControl.SetToolTip( String().Format( ToolTipQty, "hot" ) );
   HotDarkQty_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   HotRealQty_Label.SetText( "Real:" );
   HotRealQty_Label.SetMinWidth( labelWidth1 );
   HotRealQty_Label.SetTextAlignment( TextAlign::Right );
   HotRealQty_Label.SetToolTip( ToolTipRealQty );
   HotRealQty_Value.SetToolTip( ToolTipRealQty );
   HotRealQty_Sizer.Add( HotRealQty_Label );
   HotRealQty_Sizer.AddSpacing( 4 );
   HotRealQty_Sizer.Add( HotRealQty_Value );
   HotRealQty_Sizer.AddStretch();

   Hot_Sizer.SetMargin( 6 );
   Hot_Sizer.SetSpacing( 4 );
   Hot_Sizer.Add( HotDark_CheckBox );
   Hot_Sizer.Add( HotDarkLevel_NumericControl );
   Hot_Sizer.Add( HotDarkSigma_NumericControl );
   Hot_Sizer.Add( HotDarkQty_NumericControl );
   Hot_Sizer.Add( HotRealQty_Sizer );
   Hot_Sizer.Add( DetectHot_NumericControl );

   Hot_GroupBox.SetTitle( "Hot Pixels Threshold" );
   Hot_GroupBox.SetSizer( Hot_Sizer );
   //Hot_GroupBox.AdjustToContents();
   //Hot_GroupBox.SetFixedHeight();

   //---------------------------------------------------
   // Cold

   ColdDark_CheckBox.SetText( "Enable" );
   ColdDark_CheckBox.SetToolTip( String().Format( ToolTipUse, "cold" ) );
   ColdDark_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   ColdDarkLevel_NumericControl.label.SetText( "Level:" );
   ColdDarkLevel_NumericControl.label.SetFixedWidth( labelWidth1 );
   ColdDarkLevel_NumericControl.slider.SetScaledMinWidth( 200 );
   ColdDarkLevel_NumericControl.slider.SetRange( 0, 0x7fffffff );
   ColdDarkLevel_NumericControl.SetPrecision( TheColdLevel->Precision() );
   ColdDarkLevel_NumericControl.SetToolTip( String().Format( ToolTipLevel, "cold" ) );
   ColdDarkLevel_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   ColdDarkSigma_NumericControl.label.SetText( "Sigma:" );
   ColdDarkSigma_NumericControl.label.SetFixedWidth( labelWidth1 );
   ColdDarkSigma_NumericControl.slider.SetScaledMinWidth( 200 );
   ColdDarkSigma_NumericControl.slider.SetRange( 0, 0x7fffffff );
   ColdDarkSigma_NumericControl.SetRange( 0, 50 );
   ColdDarkSigma_NumericControl.SetPrecision( 5 );
   //ColdDarkSigma_NumericControl.SetToolTip( String().Format( ToolTipLevel, "cold" ) );
   ColdDarkSigma_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   ColdDarkQty_NumericControl.label.SetText( "Qty:" );
   ColdDarkQty_NumericControl.label.SetFixedWidth( labelWidth1 );
   ColdDarkQty_NumericControl.slider.SetScaledMinWidth( 200 );
   ColdDarkQty_NumericControl.slider.SetRange( 0, 10000 );
   ColdDarkQty_NumericControl.SetInteger();
   ColdDarkQty_NumericControl.SetRange( 0, 10000 );
   ColdDarkQty_NumericControl.SetToolTip( String().Format( ToolTipQty, "cold" ) );
   ColdDarkQty_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   ColdRealQty_Label.SetText( "Real:" );
   ColdRealQty_Label.SetMinWidth( labelWidth1 );
   ColdRealQty_Label.SetTextAlignment( TextAlign::Right );
   ColdRealQty_Label.SetToolTip( ToolTipRealQty );
   ColdRealQty_Value.SetToolTip( ToolTipRealQty );
   ColdRealQty_Sizer.Add( ColdRealQty_Label );
   ColdRealQty_Sizer.AddSpacing( 4 );
   ColdRealQty_Sizer.Add( ColdRealQty_Value );
   ColdRealQty_Sizer.AddStretch();

   Cold_Sizer.SetMargin( 6 );
   Cold_Sizer.SetSpacing( 4 );
   Cold_Sizer.Add( ColdDark_CheckBox );
   Cold_Sizer.Add( ColdDarkLevel_NumericControl );
   Cold_Sizer.Add( ColdDarkSigma_NumericControl );
   Cold_Sizer.Add( ColdDarkQty_NumericControl );
   Cold_Sizer.Add( ColdRealQty_Sizer );

   Cold_GroupBox.SetTitle( "Cold Pixels Threshold" );
   Cold_GroupBox.SetSizer( Cold_Sizer );

   UseMasterDark_Sizer.SetSpacing( 4 );
   UseMasterDark_Sizer.Add( MasterDark_GroupBox );
   UseMasterDark_Sizer.Add( Hot_GroupBox );
   UseMasterDark_Sizer.Add( Cold_GroupBox );

   UseMasterDark_Control.SetSizer( UseMasterDark_Sizer );
   //UseMasterDark_Control.AdjustToContents();
   //UseMasterDark_Control.SetFixedHeight();

   //---------------------------------------------------
   // Auto Detect Hot/Cold

   UseAutoDetect_SectionBar.EnableTitleCheckBox();
   UseAutoDetect_SectionBar.SetTitle( "Use Auto detect" );
   UseAutoDetect_SectionBar.SetSection( UseAutoDetect_Control );
   UseAutoDetect_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CosmeticCorrectionInterface::__ToggleSection, w );
   UseAutoDetect_SectionBar.OnCheck( (SectionBar::check_event_handler)&CosmeticCorrectionInterface::__CheckSection, w );

   const String AutoDetect_toolTip = "<p>How many times (in average deviation units) the value of a pixel must differ "
      "from the surrounding neighbors to be considered as a defective pixel?<p/>";
   int labelWidth3 = fnt.Width( String( "Cold Sigma:" ) );

   DetectHot_CheckBox.SetToolTip( "Enable/Disable Detection" + AutoDetect_toolTip );
   DetectHot_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );
   DetectHot_NumericControl.sizer.Insert( 1, DetectHot_CheckBox );
   DetectHot_NumericControl.SetPrecision( TheHotAutoValue->Precision() );
   DetectHot_NumericControl.SetToolTip( AutoDetect_toolTip );
   DetectHot_NumericControl.label.SetText( "Hot Sigma:" );
   DetectHot_NumericControl.label.SetFixedWidth( labelWidth3 );
   DetectHot_NumericControl.slider.SetScaledMinWidth( 200 );
   DetectHot_NumericControl.slider.SetRange( 0, 0x7fffffff );
   DetectHot_NumericControl.SetRange( TheHotAutoValue->MinimumValue(), TheHotAutoValue->MaximumValue() );
   DetectHot_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   DetectCold_CheckBox.SetToolTip( "Enable/Disable Detection" + AutoDetect_toolTip );
   DetectCold_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );
   DetectCold_NumericControl.sizer.Insert( 1, DetectCold_CheckBox );
   DetectCold_NumericControl.SetPrecision( TheColdAutoValue->Precision() );
   DetectCold_NumericControl.SetToolTip( AutoDetect_toolTip );
   DetectCold_NumericControl.label.SetText( "Cold Sigma:" );
   DetectCold_NumericControl.label.SetFixedWidth( labelWidth3 );
   DetectCold_NumericControl.slider.SetScaledMinWidth( 200 );
   DetectCold_NumericControl.slider.SetRange( 0, 0x7fffffff );
   DetectCold_NumericControl.SetRange( TheColdAutoValue->MinimumValue(), TheColdAutoValue->MaximumValue() );
   DetectCold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CosmeticCorrectionInterface::__RealValueUpdated, w );

   UseAutoDetect_Sizer.SetSpacing( 4 );
   UseAutoDetect_Sizer.Add( DetectHot_NumericControl );
   UseAutoDetect_Sizer.Add( DetectCold_NumericControl );

   UseAutoDetect_Control.SetSizer( UseAutoDetect_Sizer );
   //UseAutoDetect_Control.AdjustToContents();
   //UseAutoDetect_Control.SetFixedHeight();


   //---------------------------------------------------
   // via defective list section

   UseDefectList_SectionBar.SetTitle( "Use Defect List" );
   UseDefectList_SectionBar.SetSection( UseDefectList_Control );
   UseDefectList_SectionBar.EnableTitleCheckBox();
   UseDefectList_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CosmeticCorrectionInterface::__ToggleSection, w );
   UseDefectList_SectionBar.OnCheck( (SectionBar::check_event_handler)&CosmeticCorrectionInterface::__CheckSection, w );

   DefectList_TreeBox.SetNumberOfColumns( 4 );
   DefectList_TreeBox.HideHeader();
   DefectList_TreeBox.EnableMultipleSelections();
   DefectList_TreeBox.DisableRootDecoration();
   DefectList_TreeBox.EnableAlternateRowColor();
   DefectList_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&CosmeticCorrectionInterface::__NodeSelectionUpdated, w );
   DefectList_TreeBox.OnNodeUpdated( (TreeBox::node_event_handler)&CosmeticCorrectionInterface::__NodeUpdated, w );

   SelectAllDefect_PushButton.SetText( "Select All" );
   SelectAllDefect_PushButton.SetToolTip( "<p>Select all defined defects.</p>" );
   SelectAllDefect_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   InvertSelectionDefect_PushButton.SetText( "Invert Selection" );
   InvertSelectionDefect_PushButton.SetToolTip( "<p>Invert the current selection of defects.</p>" );
   InvertSelectionDefect_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   ToggleSelectedDefect_PushButton.SetText( "Toggle Selected" );
   ToggleSelectedDefect_PushButton.SetToolTip( "<p>Toggle the enabled/disabled state of currently selected defects.</p>"
      "<p>Disabled defects will be ignored during the CosmeticCorrection process.</p>" );
   ToggleSelectedDefect_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   RemoveSelectedDefect_PushButton.SetText( "Remove Selected" );
   RemoveSelectedDefect_PushButton.SetToolTip( "<p>Remove all currently selected defects.</p>" );
   RemoveSelectedDefect_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   ClearDefect_PushButton.SetText( "Clear" );
   ClearDefect_PushButton.SetToolTip( "<p>Clear the defects list.</p>" );
   ClearDefect_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   LoadList_PushButton.SetText( "Load" );
   LoadList_PushButton.SetToolTip( "<p>Load the defects list from a text file.</p>" );
   LoadList_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   SaveList_PushButton.SetText( "Save" );
   SaveList_PushButton.SetToolTip( "<p>Save the defects list to a text file.</p>" );
   SaveList_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   DefectListButtons_Sizer.SetSpacing( 4 );
   DefectListButtons_Sizer.Add( LoadList_PushButton );
   DefectListButtons_Sizer.Add( SaveList_PushButton );
   DefectListButtons_Sizer.Add( SelectAllDefect_PushButton );
   DefectListButtons_Sizer.Add( InvertSelectionDefect_PushButton );
   DefectListButtons_Sizer.Add( ToggleSelectedDefect_PushButton );
   DefectListButtons_Sizer.Add( RemoveSelectedDefect_PushButton );
   DefectListButtons_Sizer.Add( ClearDefect_PushButton );
   DefectListButtons_Sizer.AddStretch();

   DefectListTreeBox_Sizer.SetSpacing( 4 );
   DefectListTreeBox_Sizer.Add( DefectList_TreeBox );
   DefectListTreeBox_Sizer.Add( DefectListButtons_Sizer );

   // Row 1 in DefineDefect ---------------------------------
   Row_RadioButton.SetText( "Row" );
   Row_RadioButton.SetChecked();

   Col_RadioButton.SetText( "Col" );

   DefectPosition.label.Hide();
   DefectPosition.SetInteger();
   DefectPosition.SetRange( 0, 65535 );

   SelectRange_CheckBox.SetText( "Limit" );
   SelectRange_CheckBox.SetToolTip( "<p>Set the start and end coordinates in the selected column or row defect.</p>"
      "<p>Uncheck to select all pixels in the selected column or row.</p>");
   SelectRange_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   DefectStart.SetInteger();
   DefectStart.SetRange( 0, 65535 );
   DefectStart.label.Hide();
   DefectStart.Disable();

   DefectEnd.SetInteger();
   DefectEnd.SetRange( 0, 65535 );
   DefectEnd.label.Hide();
   DefectEnd.Disable();

   DefineDefect_Sizer1.SetMargin( 2 );
   DefineDefect_Sizer1.SetSpacing( 4 );
   DefineDefect_Sizer1.Add( DefectPosition );
   DefineDefect_Sizer1.Add( Row_RadioButton );
   DefineDefect_Sizer1.Add( Col_RadioButton );
   DefineDefect_Sizer1.AddStretch();
   DefineDefect_Sizer1.Add( SelectRange_CheckBox );
   DefineDefect_Sizer1.Add( DefectStart );
   DefineDefect_Sizer1.Add( DefectEnd );

   // Row 2 in DefineDefect ---------------------------------

   AddDefect_PushButton.SetText( "Add defect" );
   AddDefect_PushButton.SetToolTip( "<p>Add the current row or column defect to the defects list.</p>" );
   AddDefect_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   DefineDefect_Sizer2.Add( AddDefect_PushButton );

   //---------------------------------
   DefineDefect_Sizer.SetMargin( 4 );
   DefineDefect_Sizer.SetSpacing( 4 );
   DefineDefect_Sizer.Add( DefineDefect_Sizer1 );
   DefineDefect_Sizer.Add( DefineDefect_Sizer2 );

   UseDefectList_Sizer.SetSpacing( 4 );
   UseDefectList_Sizer.Add( DefectListTreeBox_Sizer );
   UseDefectList_Sizer.Add( DefineDefect_Sizer );

   UseDefectList_Control.SetSizer( UseDefectList_Sizer );
   //UseDefectList_Control.AdjustToContents();
   //UseDefectList_Control.SetFixedHeight();

   //---------------------------------------------------
   // RTP section

   RTP_SectionBar.SetTitle( "Real Time Preview" );
   RTP_SectionBar.SetSection( RTP_Control );
   RTP_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&CosmeticCorrectionInterface::__ToggleSection, w );

   //right column (buttons)
   RTPShowMap_CheckBox.SetText( "Show map" );
   RTPShowMap_CheckBox.SetToolTip( "<p>Show defect map on the Real Time Preview window.</p>" );
   RTPShowMap_CheckBox.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   RTPSnapshot_PushButton.SetText( "Snapshot" );
   RTPSnapshot_PushButton.SetToolTip( "<p>Take a snapshot of the current defect map or target frame<br/>"
      "Note: The dimensions of the snapshot image will be the same of the defined preview.</p>" );
   RTPSnapshot_PushButton.OnClick( (Button::click_event_handler)&CosmeticCorrectionInterface::__Button_Click, w );

   // QTY columns
#define range 0, 9999999

   RTPAutoQtyHot_Edit.label.SetText( "Auto" );
   RTPAutoQtyHot_Edit.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RTPAutoQtyHot_Edit.SetInteger();
   RTPAutoQtyHot_Edit.SetRange( range );
   RTPAutoQtyHot_Edit.edit.SetRightAlignment();
   RTPAutoQtyHot_Edit.edit.SetReadOnly();

   RTPAutoQtyCold_Edit.label.Hide();
   RTPAutoQtyCold_Edit.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RTPAutoQtyCold_Edit.SetInteger();
   RTPAutoQtyCold_Edit.SetRange( range );
   RTPAutoQtyCold_Edit.edit.SetRightAlignment();
   RTPAutoQtyCold_Edit.edit.SetReadOnly();

   RTPDarkQtyHot_Edit.label.SetText( "Dark" );
   RTPDarkQtyHot_Edit.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RTPDarkQtyHot_Edit.SetInteger();
   RTPDarkQtyHot_Edit.SetRange( range );
   RTPDarkQtyHot_Edit.edit.SetRightAlignment();
   RTPDarkQtyHot_Edit.edit.SetReadOnly();
   RTPDarkQtyHot_Edit.label.SetFixedWidth( fnt.Width( RTPAutoQtyHot_Edit.label.Text() ) );

   RTPDarkQtyCold_Edit.label.Hide();
   RTPDarkQtyCold_Edit.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RTPDarkQtyCold_Edit.SetInteger();
   RTPDarkQtyCold_Edit.SetRange( range );
   RTPDarkQtyCold_Edit.edit.SetRightAlignment();
   RTPDarkQtyCold_Edit.edit.SetReadOnly();

   //QTY title
   TRPTitleHot_Label.SetText( "Hot" );
   TRPTitleHot_Label.SetFixedWidth( RTPDarkQtyHot_Edit.edit.Width() );
   TRPTitleHot_Label.SetTextAlignment( TextAlign::HorzCenter|TextAlign::Bottom );

   TRPTitleCold_Label.SetText( "Cold" );
   TRPTitleCold_Label.SetFixedWidth( RTPDarkQtyCold_Edit.edit.Width() );
   TRPTitleCold_Label.SetTextAlignment( TextAlign::HorzCenter|TextAlign::Bottom );

   // RTP Seizers ----------------------------------------------------------
   RTPLine1_Sizer.AddSpacing( RTPDarkQtyHot_Edit.label.Width() + 4 );
   RTPLine1_Sizer.Add( TRPTitleHot_Label );
   RTPLine1_Sizer.Add( TRPTitleCold_Label );
   RTPLine1_Sizer.AddStretch();

   RTPLine2_Sizer.Add( RTPDarkQtyHot_Edit );
   RTPLine2_Sizer.Add( RTPDarkQtyCold_Edit );
   RTPLine2_Sizer.AddStretch();

   RTPLine3_Sizer.Add( RTPAutoQtyHot_Edit );
   RTPLine3_Sizer.Add( RTPAutoQtyCold_Edit );
   RTPLine3_Sizer.AddStretch();
   RTPLine3_Sizer.Add( RTPShowMap_CheckBox );
   RTPLine3_Sizer.AddStretch();
   RTPLine3_Sizer.Add( RTPSnapshot_PushButton );

   RTP_Sizer.Add( RTPLine1_Sizer );
   RTP_Sizer.Add( RTPLine2_Sizer );
   RTP_Sizer.Add( RTPLine3_Sizer );

   RTP_Control.SetSizer( RTP_Sizer );
   //RTP_Control.AdjustToContents();
   //RTP_Control.SetFixedHeight();

   //---------------------------------------------------

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( TargetImages_SectionBar );
   Global_Sizer.Add( TargetImages_Control );
   Global_Sizer.Add( Output_SectionBar );
   Global_Sizer.Add( Output_Control );
   Global_Sizer.Add( UseMasterDark_SectionBar );
   Global_Sizer.Add( UseMasterDark_Control );
   Global_Sizer.Add( UseAutoDetect_SectionBar );
   Global_Sizer.Add( UseAutoDetect_Control );

   Global_Sizer.Add( UseDefectList_SectionBar );
   Global_Sizer.Add( UseDefectList_Control );

   Global_Sizer.Add( RTP_SectionBar );
   Global_Sizer.Add( RTP_Control );

   UseMasterDark_Control.Hide();
   UseAutoDetect_Control.Hide();
   UseDefectList_Control.Hide();
   RTP_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedWidth();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionInterface.cpp - Released 2017-07-09T18:07:33Z
