//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0397
// ----------------------------------------------------------------------------
// ReadoutOptionsInstance.h - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ReadoutOptionsInstance_h
#define __ReadoutOptionsInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/ReadoutOptions.h>

#include "ReadoutOptionsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ReadoutOptionsInstance : public ProcessImplementation
{
public:

   ReadoutOptionsInstance( const MetaProcess* );
   ReadoutOptionsInstance( const ReadoutOptionsInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool CanExecuteGlobal( pcl::String& whyNot ) const;
   virtual bool ExecuteGlobal();
   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
   virtual bool ValidateParameter( void* value, const MetaParameter*, size_type tableRow ) const;

   pcl::ReadoutOptions Options() const
   {
      ReadoutOptions o;
      o.SetData( ReadoutOptions::readout_data( data ) );
      o.SetMode( ReadoutOptions::readout_mode( mode ) );
      o.SetProbeSize( probeSize );
      o.SetPreviewSize( previewSize );
      o.SetPreviewZoomFactor( previewZoom );
      o.SetPrecision( precision );
      o.SetIntegerRange( range );
      o.EnableAlphaChannel( showAlpha );
      o.EnableMaskChannel( showMask );
      o.EnablePreview( showPreview );
      o.EnablePreviewCenter( previewCenter );
      o.EnableEquatorialCoordinates( showEquatorial );
      o.EnableEclipticCoordinates( showEcliptic );
      o.EnableGalacticCoordinates( showGalactic );
      o.SetCoordinateItems( coordinateItems );
      o.SetCoordinatePrecision( coordinatePrecision );
      o.EnableBroadcast( broadcast );
      o.SetReal( real );
      return o;
   }

   void SetOptions( const pcl::ReadoutOptions& o )
   {
      data = o.Data();
      mode = o.Mode();
      probeSize = o.ProbeSize();
      previewSize = o.PreviewSize();
      previewZoom = o.PreviewZoomFactor();
      precision = o.Precision();
      range = o.IntegerRange();
      showAlpha = o.ShowAlphaChannel();
      showMask = o.ShowMaskChannel();
      showPreview = o.ShowPreview();
      previewCenter = o.ShowPreviewCenter();
      showEquatorial = o.ShowEquatorialCoordinates();
      showEcliptic = o.ShowEclipticCoordinates();
      showGalactic = o.ShowGalacticCoordinates();
      coordinateItems = o.CoordinateItems();
      coordinatePrecision = o.CoordinatePrecision();
      broadcast = o.IsBroadcastEnabled();
      real = o.IsReal();
   }

   void LoadCurrentOptions();

private:

   pcl_enum data;
   pcl_enum mode;
   int32    probeSize;           // size of the square probe - must be an odd number
   int32    previewSize;         // size of the square preview - must be an odd number
   int32    previewZoom;         // readout preview zoom factor >= 1
   int32    precision;           // number of decimal digits if real==true
   uint32   range;               // maximum discrete value if real==false
   pcl_bool showAlpha;           // show alpha channel readouts?
   pcl_bool showMask;            // show mask channel readouts?
   pcl_bool showPreview;         // show real-time readout previews?
   pcl_bool previewCenter;       // plot center hairlines on real-time readout previews?
   pcl_bool showEquatorial;      // show equatorial coordinates
   pcl_bool showEcliptic;        // show ecliptic coordinates
   pcl_bool showGalactic;        // show galactic coordinates
   int32    coordinateItems;     // 1=degrees/hours 2=minutes 3=seconds
   int32    coordinatePrecision; // decimal digits of last represented item
   pcl_bool broadcast;           // broadcast readouts?
   pcl_bool real;                // true=real, false=integer
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ReadoutOptionsInstance_h

// ----------------------------------------------------------------------------
// EOF ReadoutOptionsInstance.h - Released 2018-12-12T09:25:24Z
