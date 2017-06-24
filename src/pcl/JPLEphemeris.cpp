//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/JPLEphemeris.cpp - Released 2017-06-21T11:36:44Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/File.h>
#include <pcl/JPLEphemeris.h>

#ifndef __PCL_NO_JPL_EPHEMERIS_TEST_ROUTINES
#include <iostream>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

static bool IsGroupLine( const IsoString& line )
{
   return line.StartsWith( "GROUP" );
}

static int GroupNumber( const IsoString& line )
{
   return ::atoi( line.c_str()+6 );
}

static IsoStringList GroupLines( IsoStringList::const_iterator& i, const IsoStringList& L )
{
   IsoStringList lines;
   while ( ++i != L.End() && !IsGroupLine( *i ) )
      lines << *i;
   return lines;
}

static IsoStringList GroupTokens( IsoStringList::const_iterator& i, const IsoStringList& L )
{
   IsoStringList lines = GroupLines( i, L );
   IsoStringList tokens;
   for ( auto line : lines )
   {
      IsoStringList lineTokens;
      line.Break( lineTokens, ' ', true/*trim*/ );
      lineTokens.Remove( IsoString() );
      tokens << lineTokens;
   }
   return tokens;
}

static double FortranLiteralToDouble( IsoString& s )
{
   s.ReplaceChar( 'D', 'e' );
   return s.ToDouble();
}

JPLEphemeris::JPLEphemeris( const String& filePath ) :
   m_ephStartJD( 0 ),
   m_ephEndJD( 0 ),
   m_blockIndex( JPLEphemerisItem::NumberOfNativeItems ),
   m_startJD( 0 ),
   m_endJD( 0 )
{
   IsoStringList lines = File::ReadLines( filePath, ReadTextOption::RemoveEmptyLines|ReadTextOption::TrimSpaces );
   for ( IsoStringList::const_iterator i = lines.Begin(); i != lines.End(); )
   {
      if ( IsGroupLine( *i ) )
      {
         int group = GroupNumber( *i );
         IsoStringList tokens = GroupTokens( i, lines );
         switch ( group )
         {
         case 1030:
            {
               if ( tokens.Length() < 3 )
                  throw Error( "Invalid group 1030: expected three or more tokens." );
               m_ephStartJD = FortranLiteralToDouble( tokens[0] );
               m_ephEndJD = FortranLiteralToDouble( tokens[1] );
               m_blockDelta = int( FortranLiteralToDouble( tokens[2] ) );
            }
            break;

         case 1040:
            {
               if ( tokens.IsEmpty() )
                  throw Error( "Invalid group 1040: empty group contents." );
               size_type n = tokens[0].ToUInt();
               if ( tokens.Length() != n+1 )
                  throw Error( "Invalid group 1040: insufficient constant name tokens." );
               for ( size_type i = 1; i <= n; ++i )
                  m_constants << constant( tokens[i], 0.0 );
            }
            break;

         case 1041:
            {
               if ( tokens.IsEmpty() )
                  throw Error( "Invalid group 1041: empty group contents." );
               size_type n = tokens[0].ToUInt();
               if ( tokens.Length() != n+1 )
                  throw Error( "Invalid group 1041: insufficient constant value tokens." );
               if ( n != m_constants.Length() )
                  throw Error( "Invalid group 1041: mismatched constant name/value token counts." );
               for ( size_type i = 0; i < n; ++i )
                  m_constants[i].value = FortranLiteralToDouble( tokens[i+1] );
            }
            break;

         case 1050:
            {
               int numberOfItems = int( tokens.Length()/3 );
               if ( numberOfItems < JPLEphemerisItem::NumberOfRequiredItems )
                  throw Error( "Invalid group 1050: expected three lines with at least " + String( int( JPLEphemerisItem::NumberOfRequiredItems ) ) + " items each." );
               for ( int i = 0, n = Min( numberOfItems, int( JPLEphemerisItem::NumberOfNativeItems ) ); i < n; ++i )
               {
                  m_blockIndex[i].offset       = tokens[i                  ].ToUInt() - 1; // FORTRAN arrays are 1-based
                  m_blockIndex[i].coefficients = tokens[i +   numberOfItems].ToUInt();
                  m_blockIndex[i].subblocks    = tokens[i + 2*numberOfItems].ToUInt();
               }
            }
            break;

         default:
            break;
         }
      }
      else
         ++i;
   }

   if ( m_ephStartJD == 0 || m_ephEndJD == 0 || m_constants.IsEmpty() )
      throw Error( "Invalid JPL DE/LE ASCII ephemeris header file." );

   m_constants.Sort();

   m_km2au = 1/ConstantValue( "AU" );
   m_emb2Earth = 1/(1 + ConstantValue( "EMRAT" ));
}

// ----------------------------------------------------------------------------

void JPLEphemeris::AddData( const String& filePath )
{
   int nc = 0;
   for ( int i = JPLEphemerisItem::NumberOfNativeItems; --i >= 0; )
      if ( m_blockIndex[i].subblocks > 0 )
      {
         nc = m_blockIndex[i].offset + m_blockIndex[i].coefficients*m_blockIndex[i].subblocks*ComponentsForItem( i );
         break;
      }
   if ( nc == 0 )
      throw Error( "Invalid call to JPLEphemeris::AddData(): object not initialized." );

   IsoStringList lines = File::ReadLines( filePath, ReadTextOption::RemoveEmptyLines );
   for ( IsoStringList::iterator i = lines.Begin(); i != lines.End(); ++i )
   {
      IsoStringList tokens;
      i->Break( tokens, ' ' );
      tokens.Remove( IsoString() );
      if ( tokens.Length() != 2 )
         throw Error( "Invalid block header: expected two tokens in block header line." );
      if ( tokens[1].ToInt() != nc )
         throw Error( "Invalid block: invalid number of coefficients." );
      tokens.Clear();
      do
      {
         if ( ++i == lines.End() )
            throw Error( "Invalid data block: unexpected end of file." );
         i->ReplaceChar( 'D', 'e' );
         IsoStringList lineTokens;
         i->Break( lineTokens, ' ' );
         lineTokens.Remove( IsoString() );
         tokens << lineTokens;
      }
      while ( int( tokens.Length() ) < nc );

      Vector k( nc );
      for ( int i = 0; i < nc; ++i )
         k[i] = tokens[i].ToDouble();

      if ( k[1] - k[0] != m_blockDelta )
         throw Error( "Invalid data block: invalid time span." );

      if ( m_blocks.IsEmpty() )
      {
         m_startJD = k[0];
         m_endJD = k[1];
      }
      else
      {
         if ( k[0] < m_endJD )
            continue;
         if ( k[0] != m_endJD )
            throw Error( "Invalid call to JPLEphemeris::AddData(): non-contiguous time span defined." );
         m_endJD = k[1];
      }

      m_blocks << k;
   }
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_JPL_EPHEMERIS_TEST_ROUTINES

bool JPLEphemeris::Test( const String& filePath, bool verbose ) const
{
   /*
    * Columns in a line of a standard DE/LE test file:
    *
    * JPL Ephemeris Number
    * calendar date
    * Julian Ephemeris Date
    * target number (1-Mercury, ...,3-Earth, ,,,9-Pluto, 10-Moon, 11-Sun,
    *               12-Solar System Barycenter, 13-Earth-Moon Barycenter
    *               14-Nutations, 15-Librations)
    * center number (same codes as target number)
    * coordinate number (1-x, 2-y, ... 6-zdot)
    * coordinate  [au, au/day]
    */
   Vector r( 3 ), r0( 3 ), v( 3 ), v0( 3 );
   IsoStringList lines = File::ReadLines( filePath, ReadTextOption::RemoveEmptyLines );
   for ( IsoStringList::iterator i = lines.Begin(); i != lines.End(); ++i )
      if ( i->StartsWith( "EOT" ) )
      {
         int failed = 0;
         for ( ; ++i != lines.End(); )
         {
            IsoStringList tokens;
            i->Break( tokens, ' ', true/*trim*/ );
            tokens.Remove( IsoString() );

            double jd = tokens[2].ToDouble();
            if ( jd < m_startJD || jd > m_endJD )
               continue;

            int targetNo = tokens[3].ToInt();
            int centerNo = tokens[4].ToInt();
            int component = tokens[5].ToInt();
            double value = tokens[6].ToDouble();

            if ( targetNo == 12 ) // SSB
               r = v = 0;
            else
            {
               item_index target;
               switch ( targetNo )
               {
               case  1: target = JPLEphemerisItem::Mercury; break;
               case  2: target = JPLEphemerisItem::Venus; break;
               case  3: target = JPLEphemerisItem::Earth; break;
               case  4: target = JPLEphemerisItem::Mars; break;
               case  5: target = JPLEphemerisItem::Jupiter; break;
               case  6: target = JPLEphemerisItem::Saturn; break;
               case  7: target = JPLEphemerisItem::Uranus; break;
               case  8: target = JPLEphemerisItem::Neptune; break;
               case  9: target = JPLEphemerisItem::Pluto; break;
               case 10: target = JPLEphemerisItem::SSBMoon; break;
               case 11: target = JPLEphemerisItem::Sun; break;
               case 13: target = JPLEphemerisItem::EarthMoonBarycenter; break;
               //case 14:
               case 15: target = JPLEphemerisItem::LunarLibration; break;
               default: target = JPLEphemerisItem::Unknown; break;
               }

               if ( target == JPLEphemerisItem::Unknown )
                  continue;
               if ( !IsItemAvailable( target ) )
               {
                  if ( verbose )
                     std::cout << "Target item not available: " << target << '\n';
                  continue;
               }

               ComputeState( r, v, jd, 0, target );

               if ( target == JPLEphemerisItem::Moon )
               {
                  r *= m_km2au;
                  v *= m_km2au;
               }
            }

            if ( centerNo == 12 ) // SSB
               r0 = v0 = 0;
            else
            {
               item_index center;
               switch ( centerNo )
               {
               case  1: center = JPLEphemerisItem::Mercury; break;
               case  2: center = JPLEphemerisItem::Venus; break;
               case  3: center = JPLEphemerisItem::Earth; break;
               case  4: center = JPLEphemerisItem::Mars; break;
               case  5: center = JPLEphemerisItem::Jupiter; break;
               case  6: center = JPLEphemerisItem::Saturn; break;
               case  7: center = JPLEphemerisItem::Uranus; break;
               case  8: center = JPLEphemerisItem::Neptune; break;
               case  9: center = JPLEphemerisItem::Pluto; break;
               case 10: center = JPLEphemerisItem::SSBMoon; break;
               case 11: center = JPLEphemerisItem::Sun; break;
               case 13: center = JPLEphemerisItem::EarthMoonBarycenter; break;
               //case 14:
               //case 15:
               default: center = JPLEphemerisItem::Unknown; break;
               }

               if ( center == JPLEphemerisItem::Unknown )
                  continue;
               if ( !IsItemAvailable( center ) )
               {
                  if ( verbose )
                     std::cout << "Center item not available: " << center << '\n';
                  continue;
               }

               ComputeState( r0, v0, jd, 0, center );

               if ( center == JPLEphemerisItem::Moon )
               {
                  r0 *= m_km2au;
                  v0 *= m_km2au;
               }
            }

            r -= r0;
            v -= v0;

            double eps;
            switch ( component )
            {
            case 1: eps = r[0] - value; break;
            case 2: eps = r[1] - value; break;
            case 3: eps = r[2] - value; break;
            case 4: eps = v[0] - value; break;
            case 5: eps = v[1] - value; break;
            case 6: eps = v[2] - value; break;
            default: // ?!
               eps = 0;
               break;
            }

            bool ok = Abs( eps ) < 1e-13;
            if ( verbose )
               std::cout << IsoString().Format( "%c %.1f %2d %2d %d %+.15f\n", ok ? ' ' : 'E', jd, targetNo, centerNo, component, eps );
            if ( !ok )
               ++failed;
         }

         return failed == 0;
      }

   return false;
}

#endif   // !__PCL_NO_JPL_EPHEMERIS_TEST_ROUTINES

// ----------------------------------------------------------------------------

IsoString JPLEphemeris::Summary() const
{
   IsoString text;
   //                  1234567890
   text.AppendFormat( "DE Number  : %d\n", DENumber() );
   text.AppendFormat( "LE Number  : %d\n", LENumber() );
   text.AppendFormat( "Start JD   : %.2f\n", m_ephStartJD );
   text.AppendFormat( "End JD     : %.2f\n", m_ephEndJD );
   text.AppendFormat( "BlkDelta   : %d\n", m_blockDelta );
   text.AppendFormat( "Constants  : %u\n", m_constants.Length() );
   for ( auto c : m_constants )
   {
      text.Append( c.key.LeftJustified( 10 ) );
      text.AppendFormat( " = %+.15g\n", c.value );
   }
   text.Append( "Offsets    : " );
   for ( int i = 0; i < JPLEphemerisItem::NumberOfNativeItems; ++i )
      text.AppendFormat( "%5d", m_blockIndex[i].offset );
   text.Append( '\n' );
   text.Append( "NumCoeffs  : " );
   for ( int i = 0; i < JPLEphemerisItem::NumberOfNativeItems; ++i )
      text.AppendFormat( "%5d", m_blockIndex[i].coefficients );
   text.Append( '\n' );
   text.Append( "NumSets    : " );
   for ( int i = 0; i < JPLEphemerisItem::NumberOfNativeItems; ++i )
      text.AppendFormat( "%5d", m_blockIndex[i].subblocks );
   text.Append( '\n' );
   return text;
}

// ----------------------------------------------------------------------------

}  // pcl

// ----------------------------------------------------------------------------
// EOF pcl/JPLEphemeris.cpp - Released 2017-06-21T11:36:44Z
