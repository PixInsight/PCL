// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard CometAlignment Process Module Version 01.00.06.0057
// ****************************************************************************
// CometAlignmentInstance.h - Released 2014/10/29 07:35:26 UTC
// ****************************************************************************
// This file is part of the standard CometAlignment PixInsight module.
//
// Copyright (c) 2012-2014 Nikolay Volkov
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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
// ****************************************************************************

#ifndef __CometAlignmentInstance_h
#define __CometAlignmentInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/PixelInterpolation.h>

#include "CometAlignmentParameters.h"

//#define debug 1

namespace pcl
{

// ----------------------------------------------------------------------------
class CAThread;
struct CAThreadData;

typedef IndirectArray<CAThread> thread_list;

class CometAlignmentInstance : public ProcessImplementation
{
public:

   CometAlignmentInstance( const MetaProcess* );
   CometAlignmentInstance( const CometAlignmentInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   struct ImageItem
   {
      String   path;    // absolute file path
      pcl_bool enabled; // if disabled, skip (ignore) this image
      String   date;    // DATE-OBS yyyy-mm-ddThh:mm:ss[.sss...]
      double   Jdate;   // Julian Date
      double   x, y;    // Comet coordinates

      ImageItem( const String& p = String(), const String& d = String() ) : path( p ), enabled( true ), date( d ), Jdate( GetJdate( d ) ), x( 0 ), y( 0 )
      {
      }

      ImageItem( const ImageItem& i ) : path( i.path ), enabled( i.enabled), date( i.date ), Jdate( i.Jdate ), x( i.x ), y( i.y )
      {
      }

      inline double GetJdate( const String& d ) // Convert "yyyy-mm-ddThh:mm:ss[.sss...]" to Julian Date
      {
         if ( d.IsEmpty() )
            return 0;

         try
         {
            // 1234567890123456789
            // yyyy-mm-ddThh:mm:ss[.sss...]

            if ( d.Length() < 19 )
               throw 0;

            int year, month, day, h, m;
            double s;

            if ( !d.SubString(0,4).TryToInt( year, 10 ) ) throw 0;
            if ( !d.SubString(5,2).TryToInt( month, 10 ) ) throw 0;
            if ( !d.SubString(8,2).TryToInt( day, 10 ) ) throw 0;
            if ( !d.SubString(11,2).TryToInt( h, 10 ) ) throw 0;
            if ( !d.SubString(14,2).TryToInt( m, 10 ) ) throw 0;
            if ( !d.SubString(17).TryToDouble( s ) ) throw 0;

            double dayf = ( 3600.0*h + 60.0*m + s ) / 86400;
            return ComplexTimeToJD( year, month, day, dayf );
         }
         catch ( ... )
         {
            throw Error( "Can't convert DATE-OBS keyword value \'" + d + "\' to a Julian day number." );
         }
      }
   };

   #define outputExtension ".fit"

   typedef Array<ImageItem> image_list;


   // instance ---------------------------------------------------------------
   image_list  p_targetFrames;
   String      p_outputDir;
   pcl_bool    p_overwrite;
   String      p_prefix;
   String      p_postfix;
   size_t      p_reference;

   // Pixel interpolation
   pcl_enum    p_pixelInterpolation;      // bicubic spline | bilinear | nearest neighbor
   float       p_linearClampingThreshold; // for bicubic spline

   // -------------------------------------------------------------------------


   inline thread_list   LoadTargetFrame( size_t fileIndex );
   inline String        OutputFilePath( const String& , const size_t );
   inline void          SaveImage( const CAThread* );
   inline void          InitPixelInterpolation();

   friend class CAThread;
   friend class CometAlignmentInterface;
};

// ----------------------------------------------------------------------------
} // pcl

#endif   // __CometAlignmentInstance_h

// ****************************************************************************
// EOF CometAlignmentInstance.h - Released 2014/10/29 07:35:26 UTC
