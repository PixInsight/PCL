//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/WorldTransformation.cpp - Released 2017-07-18T16:14:00Z
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

#include <pcl/WorldTransformation.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

void SplineWorldTransformation::Serialize( ByteArray& data ) const
{
   IsoString text;
   text << "VERSION:1" << '\n'
        << "TYPE:SurfaceSpline" << '\n'
        << "ORDER:" << IsoString( m_order ) << '\n'
        << "SMOOTHING:" << IsoString( m_smoothness ) << '\n'
        << "CONTROLPOINTS:[" << '\n';

   if ( m_weights.IsEmpty() )
      for ( size_type i = 0; i < m_controlPointsW.Length(); ++i )
         text << IsoString().Format( "%.16g;%.16g;%.16g;%.16g",
                                     m_controlPointsI[i].x, m_controlPointsI[i].y,
                                     m_controlPointsW[i].x, m_controlPointsW[i].y ) << '\n';
   else
      for ( size_type i = 0; i < m_controlPointsW.Length(); ++i )
         text << IsoString().Format( "%.16g;%.16g;%.16g;%.16g;%.6g",
                                     m_controlPointsI[i].x, m_controlPointsI[i].y,
                                     m_controlPointsW[i].x, m_controlPointsW[i].y, m_weights[int( i )] ) << '\n';
   text << ']';

   data = ByteArray( reinterpret_cast<uint8*>( text.Begin() ),
                     reinterpret_cast<uint8*>( text.End() ) );
}

// ----------------------------------------------------------------------------

void SplineWorldTransformation::Deserialize( const ByteArray& data )
{
   try
   {
      IsoStringList lines;
      IsoString( reinterpret_cast<const char*>( data.Begin() ),
               reinterpret_cast<const char*>( data.End() ) ).Break( lines, '\n' );
      if ( lines.IsEmpty() )
         throw Error( "Invalid raw serialization" );

      {
         IsoStringList tokens;
         lines[0].Break( tokens, ':' );
         if ( tokens.Length() != 2 || tokens[0] != "VERSION" || tokens[1] != "1" )
            throw Error( "Invalid raw serialization version" );
      }

      Array<float> weights;

      for ( IsoStringList::const_iterator i = lines.Begin(); ++i != lines.End(); )
      {
         IsoStringList tokens;
         i->Break( tokens, ':' );
         if ( tokens.Length() < 2 )
            continue;
         if ( tokens[0] == "ORDER" )
            m_order = tokens[1].ToInt();
         else if ( tokens[0] == "SMOOTHING" )
            m_smoothness = tokens[1].ToFloat();
         else if ( tokens[0] == "CONTROLPOINTS" )
         {
            //tokens[1].Trim(); // number of control points?
            for ( ;; )
            {
               if ( ++i == lines.End() )
                  throw Error( "Unexpected end of data" );
               if ( i->StartsWith( ']' ) )
                  break;

               tokens.Clear();
               i->Break( tokens, ';' );
               if ( tokens.Length() != 4 && tokens.Length() != 5 )
                  throw Error( "Invalid raw control point serialization" );

               double pIx = tokens[0].ToDouble();
               double pIy = tokens[1].ToDouble();
               double pWx = tokens[2].ToDouble();
               double pWy = tokens[3].ToDouble();
               m_controlPointsW << DPoint( pWx, pWy );
               m_controlPointsI << DPoint( pIx, pIy );

               if ( tokens.Length() == 5 )
                  weights << tokens[4].ToFloat();
            }
         }
      }

      if ( !weights.IsEmpty() )
         m_weights = FVector( weights.Begin(), int( weights.Length() ) );
   }
   catch ( const Exception& x )
   {
      throw Error( "SplineWorldTransformation::Deserialize(): " + x.Message() );
   }
   catch ( std::bad_alloc& )
   {
      throw Error( "SplineWorldTransformation::Deserialize(): " + String( "Out of memory" ) );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

void SplineWorldTransformation::InitializeSplines()
{
   try
   {
      if ( m_controlPointsW.Length() != m_controlPointsI.Length() )
         throw Error( "The specified control point arrays have different lengths" );
      if ( m_controlPointsW.Length() < 5 )
         throw Error( "At least five control points are required" );
      if ( m_smoothness > 0 )
         if ( !m_weights.IsEmpty() )
            if ( size_type( m_weights.Length() ) != m_controlPointsW.Length() )
               throw Error( "Invalid length of point weights vector" );

      m_splineWI.Initialize( m_controlPointsW, m_controlPointsI, m_smoothness, m_weights, m_order );
      m_splineIW.Initialize( m_controlPointsI, m_controlPointsW, m_smoothness, m_weights, m_order );

      if ( !m_splineWI.IsValid() || !m_splineIW.IsValid() )
         throw Error( "Invalid surface spline initialization" );
   }
   catch ( const Exception& x )
   {
      throw Error( "SplineWorldTransformation::InitializeSplines(): " + x.Message() );
   }
   catch ( std::bad_alloc& )
   {
      throw Error( "SplineWorldTransformation::InitializeSplines(): " + String( "Out of memory" ) );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

void SplineWorldTransformation::CalculateLinearApproximation()
{
   int N = int( m_controlPointsI.Length() );

   /*
    * Use independent multiple linear regression for x and y. The model is:
    *
    *   Y = X * B + err
    *
    * - The regressand Y contains the x (or y) of the predicted coordinates
    * (m_controlPointsW).
    *
    * - The regressor X contains the vectors (x,y,1) with the source
    * coordinates (m_controlPointsI).
    *
    * - The parameters vector B contains the factors of the expression:
    *
    *   xc = xi*B0 + yi*B1 + B2
    */
   DMatrix Y1( N, 1 );
   DMatrix Y2( N, 1 );
   DMatrix X( N, 3 );
   for ( int i = 0; i < N; ++i )
   {
      Y1[i][0] = m_controlPointsW[i].x;
      Y2[i][0] = m_controlPointsW[i].y;
      X[i][0]  = m_controlPointsI[i].x;
      X[i][1]  = m_controlPointsI[i].y;
      X[i][2]  = 1;
   }

   DMatrix XT = X.Transpose();
   DMatrix XT_X_inv_XT = (XT * X).Inverse() * XT;
   DMatrix B1 = XT_X_inv_XT * Y1;
   DMatrix B2 = XT_X_inv_XT * Y2;
   m_linearIW = LinearTransformation( B1[0][0], B1[1][0], B1[2][0],
                                      B2[0][0], B2[1][0], B2[2][0] );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/WorldTransformation.cpp - Released 2017-07-18T16:14:00Z
