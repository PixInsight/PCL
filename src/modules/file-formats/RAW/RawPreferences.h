//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 01.05.00.0405
// ----------------------------------------------------------------------------
// RawPreferences.h - Released 2018-11-01T11:07:09Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
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

#ifndef __RawPreferences_h
#define __RawPreferences_h

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class RawPreferences
{
public:

   enum Interpolation { Bilinear, VNG, PPG, AHD, HalfSize, DCB, DHT, AAHD, Default = VNG };

   Interpolation interpolation          = Default;
   bool          interpolateAs4Colors   = false;
   bool          useAutoWhiteBalance    = false;
   bool          useCameraWhiteBalance  = true;
   bool          noWhiteBalance         = false;
   bool          createSuperPixels      = false;
   bool          outputRawRGB           = false;
   bool          outputCFA              = false;
   bool          noAutoFlip             = false;
   bool          noAutoCrop             = false;
   bool          noBlackPointCorrection = false;
   bool          noClipHighlights       = false;
   int           noiseThreshold         = 0;
   int           dcbIterations          = 3;
   bool          dcbRefinement          = false;
   int           fbddNoiseReduction     = 2;

   String InterpolationAsString() const
   {
      switch ( interpolation )
      {
      case Bilinear: return "Bilinear";
      case VNG:      return "VNG";
      case PPG:      return "PPG";
      case AHD:      return "AHD";
      case HalfSize: return "HalfSize";
      case DCB:      return "DCB";
      case DHT:      return "DHT";
      case AAHD:     return "AAHD";
      default: // ?!
         return "<unknown>";
      }
   }

   String WhiteBalancingAsString() const
   {
      if ( noWhiteBalance )
         return "disabled";
      String text;
      if ( useCameraWhiteBalance )
         text << "camera";
      if ( useAutoWhiteBalance )
      {
         if ( !text.IsEmpty() )
            text << ',';
         text << "auto";
      }
      return text;
   }

   String OutputModeAsString() const
   {
      if ( createSuperPixels )
         return "super-pixel";
      if ( outputRawRGB )
         return "raw-RGB";
      if ( outputCFA )
         return "cfa";
      return "demosaic";
   }

public :

   RawPreferences() = default;

   RawPreferences( const RawPreferences& ) = default;

   virtual ~RawPreferences()
   {
   }

   RawPreferences& operator =( const RawPreferences& ) = default;

#define READ_OR_WRITE( what )                                                               \
      Settings::what##I( "RawPreferences_interpolation",          interpolation );          \
      Settings::what   ( "RawPreferences_interpolateAs4Colors",   interpolateAs4Colors );   \
      Settings::what   ( "RawPreferences_useAutoWhiteBalance",    useAutoWhiteBalance );    \
      Settings::what   ( "RawPreferences_useCameraWhiteBalance",  useCameraWhiteBalance );  \
      Settings::what   ( "RawPreferences_noWhiteBalance",         noWhiteBalance );         \
      Settings::what   ( "RawPreferences_createSuperPixels",      createSuperPixels );      \
      Settings::what   ( "RawPreferences_outputRawRGB",           outputRawRGB );           \
      Settings::what   ( "RawPreferences_outputCFA",              outputCFA );              \
      Settings::what   ( "RawPreferences_noAutoFlip",             noAutoFlip );             \
      Settings::what   ( "RawPreferences_noAutoCrop",             noAutoCrop );             \
      Settings::what   ( "RawPreferences_noBlackPointCorrection", noBlackPointCorrection ); \
      Settings::what   ( "RawPreferences_noClipHighlights",       noClipHighlights );       \
      Settings::what   ( "RawPreferences_noiseThreshold",         noiseThreshold );         \
      Settings::what   ( "RawPreferences_dcbIterations",          dcbIterations );          \
      Settings::what   ( "RawPreferences_dcbRefinement",          dcbRefinement );          \
      Settings::what   ( "RawPreferences_fbddNoiseReduction",     fbddNoiseReduction );

   void SaveToSettings()
   {
      READ_OR_WRITE( Write )
   }

   void LoadFromSettings()
   {
      READ_OR_WRITE( Read )
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __RawPreferences_h

// ----------------------------------------------------------------------------
// EOF RawPreferences.h - Released 2018-11-01T11:07:09Z
