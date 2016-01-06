//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/DrizzleDataDecoder.h - Released 2015/12/17 18:52:09 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_DrizzleDataDecoder_h
#define __PCL_DrizzleDataDecoder_h

#include <pcl/SurfaceSplines.h>

#include <errno.h>

/*
 * ### TODO: Document this file.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

class DrizzleDecoderBase
{
public:

   DrizzleDecoderBase()
   {
   }

   virtual ~DrizzleDecoderBase()
   {
   }

   void Decode( const IsoString& s, int start = 0, int end = -1 )
   {
      Initialize();

      if ( end < 0 )
         end = s.Length();
      IsoString itemId;
      int block = 0;
      int blockStart = 0;
      for ( int i = start; i < end; ++i )
         switch ( s[i] )
         {
         case '{':
            if ( block++ == 0 )
            {
               blockStart = i;
               itemId.Trim();
               if ( itemId.IsEmpty() )
                  throw Error( "At offset=" + String( i ) + ": Missing item identifier." );
            }
            break;
         case '}':
            if ( --block < 0 )
               throw Error( "At offset=" + String( i ) + ": Unexpected block termination." );
            if ( block == 0 )
            {
               ProcessBlock( s, itemId, blockStart+1, i );
               itemId.Clear();
            }
            break;
         default:
            if ( block == 0 )
               itemId.Append( s[i] );
            break;
         }

      if ( block > 0 )
         throw Error( "At offset=" + String( blockStart ) + ": Unterminated block." );
      if ( !itemId.IsEmpty() )
         throw Error( "Uncompleted item definition \'" + itemId + '\'' );

      Validate();
   }

   IsoString Filter( const IsoString& s, int start = 0, int end = -1 )
   {
      IsoString filtered;

      if ( end < 0 )
         end = s.Length();
      IsoString itemId;
      int block = 0;
      int blockStart = 0;
      for ( int i = start; i < end; ++i )
         switch ( s[i] )
         {
         case '{':
            if ( block++ == 0 )
            {
               blockStart = i;
               itemId.Trim();
               if ( itemId.IsEmpty() )
                  return IsoString();
            }
            break;
         case '}':
            if ( --block < 0 )
               return IsoString();
            if ( block == 0 )
            {
               if ( !FilterBlock( itemId ) )
               {
                  filtered.Append( itemId );
                  filtered.Append( s.Substring( blockStart, i-blockStart+1 ) );
               }
               itemId.Clear();
            }
            break;
         default:
            if ( block == 0 )
               itemId.Append( s[i] );
            break;
         }

      if ( block > 0 || !itemId.IsEmpty() )
         return IsoString();

      return filtered;
   }

protected:

   virtual void Initialize()
   {
   }

   virtual void ProcessBlock( const IsoString& s, const IsoString& itemId, int start, int end )
   {
   }

   virtual void Validate()
   {
   }

   virtual bool FilterBlock( const IsoString& s )
   {
      return false;
   }

   static bool TryToInt( int& value, IsoString::const_iterator p )
   {
      IsoString::iterator endptr = 0;
      errno = 0;
      long val = ::strtol( p, &endptr, 0 );
      if ( errno == 0 && (endptr == 0 || *endptr == '\0') )
      {
         value = int( val );
         return true;
      }
      return false;
   }

   static bool TryToDouble( double& value, IsoString::const_iterator p )
   {
      IsoString::iterator endptr = 0;
      errno = 0;
      double val = ::strtod( p, &endptr );
      if ( errno == 0 && (endptr == 0 || *endptr == '\0') )
      {
         value = val;
         return true;
      }
      return false;
   }

   static DVector ParseListOfRealValues( const IsoString& s, int start, int end, int minItems = 0, int maxItems = 0 )
   {
      IsoString t = s.Substring( start, end-start );
      Array<double> v;
      for ( size_type i = 0, j, n = t.Length(); i < n; ++i )
      {
         for ( j = i; j < n; ++j )
            if ( t[j] == ',' )
               break;
         t[j] = '\0';
         double x;
         if ( !TryToDouble( x, t.At( i ) ) )
            throw Error( "Parsing list from offset=" + IsoString( start ) + ": Invalid floating point numeric literal \'" + IsoString( t.At( i ) ) + "\'" );
         if ( maxItems > 0 )
            if ( v.Length() == size_type( maxItems ) )
               throw Error( "Parsing list from offset=" + IsoString( start ) + ": Too many items." );
         v.Append( x );
         i = j;
      }
      if ( v.Length() < size_type( minItems ) )
         throw Error( "Parsing list from offset=" + IsoString( start ) + ": Too few items." );
      return DVector( v.Begin(), int( v.Length() ) );
   }

   static IVector ParseListOfIntegerValues( const IsoString& s, int start, int end, int minItems = 0, int maxItems = 0 )
   {
      IsoString t = s.Substring( start, end-start );
      Array<int> v;
      for ( size_type i = 0, j, n = t.Length(); i < n; ++i )
      {
         for ( j = i; j < n; ++j )
            if ( t[j] == ',' )
               break;
         t[j] = '\0';
         int x;
         if ( !TryToInt( x, t.At( i ) ) )
            throw Error( "Parsing list from offset=" + IsoString( start ) + ": Invalid integer numeric literal \'" + IsoString( t.At( i ) ) + "\'" );
         if ( maxItems > 0 )
            if ( v.Length() == size_type( maxItems ) )
               throw Error( "Parsing list from offset=" + IsoString( start ) + ": Too many items." );
         v.Append( x );
         i = j;
      }
      if ( v.Length() < size_type( minItems ) )
         throw Error( "Parsing list from offset=" + IsoString( start ) + ": Too few items." );
      return IVector( v.Begin(), int( v.Length() ) );
   }

   static double ParseRealValue( const IsoString& s, int start, int end )
   {
      double x;
      if ( !s.Substring( start, end-start ).TryToDouble( x ) )
         throw Error( "At offset=" + IsoString( start ) + ": Invalid floating point numeric literal \'" + s + "\'" );
      return x;
   }

   static int ParseIntegerValue( const IsoString& s, int start, int end )
   {
      int x;
      if ( !s.Substring( start, end-start ).TryToInt( x ) )
         throw Error( "At offset=" + IsoString( start ) + ": Invalid integer numeric literal \'" + s + "\'" );
      return x;
   }
};

// ----------------------------------------------------------------------------

class DrizzleTargetDecoder : public DrizzleDecoderBase
{
public:

   DrizzleTargetDecoder() : DrizzleDecoderBase()
   {
   }

   virtual ~DrizzleTargetDecoder()
   {
   }

   const String& TargetPath() const
   {
      return m_targetPath;
   }

   bool HasTarget() const
   {
      return !m_targetPath.IsEmpty();
   }

private:

   String m_targetPath;

   virtual void Initialize()
   {
      m_targetPath.Clear();
   }

   virtual void ProcessBlock( const IsoString& s, const IsoString& itemId, int start, int end )
   {
      if ( itemId == "T" )
      {
         m_targetPath = s.Substring( start, end-start ).Trimmed().UTF8ToUTF16();
         if ( m_targetPath.IsEmpty() )
            throw Error( "At offset=" + String( start ) + ": Empty file path defined." );
      }
   }
};

// ----------------------------------------------------------------------------

class DrizzleSplineDecoder : public DrizzleDecoderBase
{
public:

   typedef PointSurfaceSpline::spline spline;

   DrizzleSplineDecoder() : DrizzleDecoderBase()
   {
   }

   virtual ~DrizzleSplineDecoder()
   {
   }

   const spline& Spline() const
   {
      return m_S;
   }

private:

   spline m_S;

   virtual void Initialize()
   {
      m_S.Clear();
   }

   virtual void ProcessBlock( const IsoString& s, const IsoString& itemId, int start, int end )
   {
      if ( itemId == "x" )
         m_S.m_x = ParseListOfRealValues( s, start, end, 3 );
      else if ( itemId == "y" )
         m_S.m_y = ParseListOfRealValues( s, start, end, 3 );
      else if ( itemId == "r0" )
         m_S.m_r0 = ParseRealValue( s, start, end );
      else if ( itemId == "x0" )
         m_S.m_x0 = ParseRealValue( s, start, end );
      else if ( itemId == "y0" )
         m_S.m_y0 = ParseRealValue( s, start, end );
      else if ( itemId == "m" )
         m_S.m_order = ParseIntegerValue( s, start, end );
      else if ( itemId == "r" )
         m_S.m_smoothing = ParseRealValue( s, start, end );
      else if ( itemId == "w" )
         m_S.m_weights = ParseListOfRealValues( s, start, end );
      else if ( itemId == "s" )
         m_S.m_spline = ParseListOfRealValues( s, start, end, 3 );
      else
         throw Error( "At offset=" + String( start ) + ": Unknown item identifier \'" + itemId + '\'' );
   }

   virtual void Validate()
   {
      if ( m_S.m_x.Length() < 3 || m_S.m_x.Length() != m_S.m_y.Length() ||
           m_S.m_r0 <= 0 ||
           m_S.m_order < 1 ||
           m_S.m_smoothing < 0 ||
          !m_S.m_weights.IsEmpty() && m_S.m_weights.Length() != m_S.m_x.Length() ||
           m_S.m_spline.Length() != m_S.m_x.Length() + ((m_S.m_order*(m_S.m_order + 1)) >> 1) )
      {
         throw Error( "Invalid surface spline definition." );
      }
   }
};

// ----------------------------------------------------------------------------

class DrizzleDataDecoder : public DrizzleDecoderBase
{
public:

   DrizzleDataDecoder() : DrizzleDecoderBase()
   {
   }

   virtual ~DrizzleDataDecoder()
   {
   }

   const String& FilePath() const
   {
      return m_filePath;
   }

   const String& TargetPath() const
   {
      return m_targetPath;
   }

   int ReferenceWidth() const
   {
      return m_referenceWidth;
   }

   int ReferenceHeight() const
   {
      return m_referenceHeight;
   }

   int NumberOfChannels() const
   {
      return m_location.Length();
   }

   const Matrix& AlignmentMatrix() const
   {
      return m_H;
   }

   const PointSurfaceSpline& AlignmentSplines() const
   {
      return m_S;
   }

   bool HasMatrix() const
   {
      return !m_H.IsEmpty();
   }

   bool HasSplines() const
   {
      return m_S.IsValid();
   }

   const DVector& Location() const
   {
      return m_location;
   }

   const DVector& ReferenceLocation() const
   {
      return m_referenceLocation;
   }

   const DVector& Scale() const
   {
      return m_scale;
   }

   const DVector& Weight() const
   {
      return m_weight;
   }

   const UI64Vector& RejectionLowCount() const
   {
      return m_rejectionLowCount;
   }

   const UI64Vector& RejectionHighCount() const
   {
      return m_rejectionHighCount;
   }

   const UInt8Image& RejectionMap() const
   {
      return m_rejectionMap;
   }

   bool HasRejectionData() const
   {
      return !m_rejectionMap.IsEmpty();
   }

   void Clear()
   {
      Initialize();
   }

protected:

   typedef PointSurfaceSpline::spline   spline;
   typedef Array<Point>                 rejection_coordinates;
   typedef Array<rejection_coordinates> rejection_data;

   /*
    * Drizzle data
    */
   String             m_filePath;
   String             m_targetPath;
   int                m_referenceWidth;
   int                m_referenceHeight;
   Matrix             m_H;
   PointSurfaceSpline m_S;
   Vector             m_location;
   Vector             m_referenceLocation;
   Vector             m_scale;
   Vector             m_weight;
   UI64Vector         m_rejectionLowCount;
   UI64Vector         m_rejectionHighCount;
   UInt8Image         m_rejectionMap;

   /*
    * Intermediate working data
    */
   spline             m_Sx;
   spline             m_Sy;
   rejection_data     m_rejectLowData;
   rejection_data     m_rejectHighData;

   virtual void Initialize()
   {
      m_filePath.Clear();
      m_targetPath.Clear();
      m_referenceWidth = m_referenceHeight = -1;
      m_H = Matrix();
      m_S.Clear();
      m_location = m_referenceLocation = m_scale = m_weight = Vector();
      m_rejectionLowCount = m_rejectionHighCount = UI64Vector();
      m_rejectionMap.FreeData();
      m_Sx.Clear();
      m_Sy.Clear();
      m_rejectLowData = m_rejectHighData = rejection_data();
   }

   virtual void ProcessBlock( const IsoString& s, const IsoString& itemId, int start, int end )
   {
      if ( itemId == "P" ) // drizzle source image
      {
         m_filePath = s.Substring( start, end-start ).Trimmed().UTF8ToUTF16();
         if ( m_filePath.IsEmpty() )
            throw Error( "At offset=" + String( start ) + ": Empty file path defined." );
      }
      else if ( itemId == "T" ) // alignment target image (optional)
      {
         m_targetPath = s.Substring( start, end-start ).Trimmed().UTF8ToUTF16();
         if ( m_targetPath.IsEmpty() )
            throw Error( "At offset=" + String( start ) + ": Empty file path defined." );
      }
      else if ( itemId == "D" ) // alignment reference image dimensions
      {
         IVector v = ParseListOfIntegerValues( s, start, end, 2, 2 );
         m_referenceWidth = v[0];
         m_referenceHeight = v[1];
         if ( m_referenceWidth < 1 || m_referenceHeight < 1 )
            throw Error( "At offset=" + String( start ) + ": Invalid reference dimensions." );
      }
      else if ( itemId == "H" ) // alignment matrix (projective)
      {
         Vector v = ParseListOfRealValues( s, start, end, 9, 9 );
         m_H = Matrix( v.Begin(), 3, 3 );
      }
      else if ( itemId == "Sx" ) // registration thin plates, X-axis
         m_Sx = ParseSurfaceSpline( s, start, end );
      else if ( itemId == "Sy" ) // registration thin plates, Y-axis
         m_Sy = ParseSurfaceSpline( s, start, end );
      else if ( itemId == "m" ) // location vector
         m_location = ParseListOfRealValues( s, start, end, 1 );
      else if ( itemId == "m0" ) // reference location vector
         m_referenceLocation = ParseListOfRealValues( s, start, end, 1 );
      else if ( itemId == "s" ) // scaling factors vector
         m_scale = ParseListOfRealValues( s, start, end, 1 );
      else if ( itemId == "w" ) // image weights vector
         m_weight = ParseListOfRealValues( s, start, end, 1 );
      else if ( itemId == "Rl" ) // rejection pixel coordinates, low values
         m_rejectLowData = ParseRejectionData( s, start, end );
      else if ( itemId == "Rh" ) // rejection pixel coordinates, high values
         m_rejectHighData = ParseRejectionData( s, start, end );
      else
         throw Error( "At offset=" + String( start ) + ": Unknown item identifier \'" + itemId + '\'' );
   }

   virtual void Validate()
   {
      if ( m_filePath.IsEmpty() )
         throw Error( "No file path definition." );
      if ( m_referenceWidth < 1 )
         throw Error( "No reference width definition." );
      if ( m_referenceHeight < 1 )
         throw Error( "No reference height definition." );
      if ( m_H.IsEmpty() && !m_Sx.IsValid() )
         throw Error( "No alignment matrix definition." );
      if ( m_Sx.IsValid() != m_Sy.IsValid() )
         throw Error( "Missing surface spline definition." );
      if ( m_location.IsEmpty() )
         throw Error( "No location vector definition." );
      if ( m_referenceLocation.IsEmpty() )
         throw Error( "No reference location vector definition." );
      if ( m_scale.IsEmpty() )
         throw Error( "No scale vector definition." );
      if ( m_weight.IsEmpty() )
         throw Error( "No weight vector definition." );

      if ( m_location.Length() != m_referenceLocation.Length() ||
           m_location.Length() != m_scale.Length() ||
           m_location.Length() != m_weight.Length() ||
          !m_rejectLowData.IsEmpty() && size_type( m_location.Length() ) != m_rejectLowData.Length() ||
          !m_rejectHighData.IsEmpty() && size_type( m_location.Length() ) != m_rejectHighData.Length() )
         throw Error( "Incongruent vector definition(s)." );

      if ( m_Sx.IsValid() )
      {
         m_S.m_Sx = m_Sx;
         m_S.m_Sy = m_Sy;
      }

      if ( !m_rejectHighData.IsEmpty() || !m_rejectLowData.IsEmpty() )
      {
         int n = m_location.Length();
         m_rejectionLowCount = UI64Vector( uint64( 0 ), n );
         m_rejectionHighCount = UI64Vector( uint64( 0 ), n );
         m_rejectionMap.AllocateData( m_referenceWidth, m_referenceHeight, n );
         m_rejectionMap.Zero();

         if ( !m_rejectHighData.IsEmpty() )
            for ( int c = 0; c < n; ++c )
            {
               const rejection_coordinates& p = m_rejectHighData[c];
               m_rejectionHighCount[c] = p.Length();
               for ( rejection_coordinates::const_iterator i = p.Begin(); i != p.End(); ++i )
                  m_rejectionMap( *i, c ) = uint8( 1 );
            }

         if ( !m_rejectLowData.IsEmpty() )
            for ( int c = 0; c < n; ++c )
            {
               const rejection_coordinates& p = m_rejectLowData[c];
               m_rejectionLowCount[c] = p.Length();
               for ( rejection_coordinates::const_iterator i = p.Begin(); i != p.End(); ++i )
                  m_rejectionMap( *i, c ) |= uint8( 2 );
            }
      }
   }

   static rejection_coordinates ParseRejectionCoordinates( const IsoString& s, int start, int end )
   {
      IVector v = ParseListOfIntegerValues( s, start, end );
      if ( v.Length() & 1 )
         throw Error( "Parsing list from offset=" + IsoString( start ) + ": Missing point coordinate(s)." );
      rejection_coordinates p;
      for ( int i = 0; i < v.Length(); i += 2 )
         p.Append( Point( v[i], v[i+1] ) );
      return p;
   }

   static rejection_data ParseRejectionData( const IsoString& s, int start, int end )
   {
      rejection_data R;
      for ( int i = start; i < end; ++i )
         if ( s[i] == '{' )
         {
            size_type j = s.Find( '}', ++i );
            if ( j >= size_type( end ) )
               throw Error( "At offset=" + String( i ) + ": Unterminated block." );
            R.Append( ParseRejectionCoordinates( s, i, j ) );
            i = j;
         }
         else if ( !IsoCharTraits::IsSpace( s[i] ) )
            throw Error( "At offset=" + String( i ) + ": Unexpected token \'" + s[i] + '\'' );
      return R;
   }

   static spline ParseSurfaceSpline( const IsoString& s, int start, int end )
   {
      DrizzleSplineDecoder S;
      S.Decode( s, start, end );
      return S.Spline();
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_DrizzleDataDecoder_h

// ----------------------------------------------------------------------------
// EOF pcl/DrizzleDataDecoder.h - Released 2015/12/17 18:52:09 UTC
