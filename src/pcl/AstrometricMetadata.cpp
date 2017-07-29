//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/AstrometricMetadata.cpp - Released 2017-06-28T11:58:42Z
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

#include <pcl/AstrometricMetadata.h>
#include <pcl/ImageWindow.h>
#include <pcl/ProjectionFactory.h>
#include <pcl/XISF.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

void AstrometricMetadata::Build( const FITSKeywordArray& keywords, const ByteArray& controlPoints, int width, int height )
{
   WCSKeywords wcs( keywords );

   m_xpixsz = wcs.xpixsz();
   m_dateobs = wcs.dateobs;
   m_width = width;
   m_height = height;

   if ( wcs.ctype1.Substring( 0, 4 ) == "RA--" &&
        wcs.ctype2.Substring( 0, 4 ) == "DEC-" &&
        wcs.crpix1.IsDefined() && wcs.crpix2.IsDefined() && wcs.crval1.IsDefined() && wcs.crval2.IsDefined() )
   {
      m_projection = ProjectionFactory::Create( wcs );

      LinearTransformation linearTransIW;
      if ( wcs.ExtractWorldTransformation( linearTransIW, m_height ) )
      {
         if ( !controlPoints.IsEmpty() )
            m_transformWI = new SplineWorldTransformation( controlPoints, linearTransIW );
         else
            m_transformWI = new LinearWorldTransformation( linearTransIW );

         double resx = Sqrt( linearTransIW.A00()*linearTransIW.A00() + linearTransIW.A01()*linearTransIW.A01() );
         double resy = Sqrt( linearTransIW.A10()*linearTransIW.A10() + linearTransIW.A11()*linearTransIW.A11() );
         m_resolution = (resx + resy)/2;
         m_useFocal = false;
         if ( m_xpixsz.IsDefined() )
            if ( m_xpixsz() > 0 )
               m_focal = FocalFromResolution( m_resolution );
      }
   }

   if ( m_transformWI.IsNull() )
   {
      if ( wcs.focallen.IsDefined() )
         if ( wcs.focallen() > 0 )
         {
            m_focal = wcs.focallen();
            m_useFocal = true;
         }
      if ( m_useFocal )
         if ( m_xpixsz.IsDefined() )
            if ( m_xpixsz() > 0 )
               m_resolution = ResolutionFromFocal( m_focal() );
   }
}

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( const ImageWindow& window )
{
   int width, height;
   View view = window.MainView();
   view.GetSize( width, height );

   FITSKeywordArray keywords = window.Keywords();

   ByteArray controlPoints;
   Variant v = view.PropertyValue( "Transformation_ImageToProjection" );
   if ( v.IsValid() )
      controlPoints = v.ToByteArray();

   Build( keywords, controlPoints, width, height );
}

// ----------------------------------------------------------------------------

AstrometricMetadata::AstrometricMetadata( XISFReader& reader )
{
   ImageInfo info = reader.ImageInfo();

   FITSKeywordArray keywords = reader.ReadFITSKeywords();

   ByteArray controlPoints;
   Variant v = reader.ReadImageProperty( "Transformation_ImageToProjection" );
   if ( v.IsValid() )
      controlPoints = v.ToByteArray();

   Build( keywords, controlPoints, info.width, info.height );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Write( ImageWindow& window, bool notify ) const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Write(): Invalid or uninitialized metadata" );

   View view = window.MainView();
   if( view.Width() != m_width || view.Height() != m_height )
      throw Error( "AstrometricMetadata::Write(): Metadata not compatible with the dimensions of the image" );

   FITSKeywordArray keywords = window.Keywords();
   UpdateBasicKeywords( keywords );
   UpdateWCSKeywords( keywords );
   window.SetKeywords( keywords );

   const SplineWorldTransformation* splineTransform = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
   if ( splineTransform != nullptr )
   {
      ByteArray data;
      splineTransform->Serialize( data );
      view.SetPropertyValue( "Transformation_ImageToProjection", data, notify, ViewPropertyAttribute::Storable );
   }
   else
      view.DeleteProperty( "Transformation_ImageToProjection" );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Write( XISFWriter& writer ) const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::Write(): Invalid or uninitialized metadata" );

   FITSKeywordArray keywords = writer.FITSKeywords();
   UpdateBasicKeywords( keywords );
   UpdateWCSKeywords( keywords );
   writer.WriteFITSKeywords( keywords );

   const SplineWorldTransformation* splineTransform = dynamic_cast<const SplineWorldTransformation*>( m_transformWI.Pointer() );
   if ( splineTransform != nullptr )
   {
      ByteArray data;
      splineTransform->Serialize( data );
      writer.WriteImageProperty( "Transformation_ImageToProjection", data );
   }
   else
      writer.RemoveImageProperty( "Transformation_ImageToProjection" );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::Validate() const
{
   try
   {
      if ( !IsValid() )
         throw Error( "Invalid or uninitialized metadata" );

      /*
       * Does a full cycle transformation (image > celestial > image) and
       * checks if the result is the same as the input.
       */
      DPoint pI0( m_width/2.0, m_height/2.0 );
      DPoint pRD;
      if ( !ImageToCelestial( pRD, pI0 ) )
         throw Error( "Failed to perform ImageToCelestial() coordinate transformation" );
      DPoint pI1;
      if ( !CelestialToImage( pI1, pRD ) )
         throw Error( "Failed to perform CelestialToImage() coordinate transformation" );
      if ( Abs( pI0.x - pI1.x ) > 1.0e-05 || Abs( pI0.y - pI1.y ) > 1.0e-03 )
         throw Error( "Inconsistent coordinate transformation results" );
   }
   catch ( const Exception& x )
   {
      throw Error( "AstrometricMetadata::Validate(): " + x.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

// ----------------------------------------------------------------------------

double AstrometricMetadata::Rotation( bool& flipped ) const
{
   if ( m_transformWI.IsNull() )
      throw Error( "Invalid call to AstrometricMetadata::Rotation(): No world transformation defined" );

   LinearTransformation linearTransIW = m_transformWI->ApproximateLinearTransform();
   double det = linearTransIW.A01() * linearTransIW.A10() - linearTransIW.A00() * linearTransIW.A11();
   double rotation = Deg( ArcTan( linearTransIW.A00() + linearTransIW.A01(), linearTransIW.A10() + linearTransIW.A11() ) ) + 135;
   if ( det > 0 )
      rotation = -90 - rotation;
   if ( rotation < -180 )
      rotation += 360;
   if ( rotation > 180 )
      rotation -= 360;
   flipped = det > 0;
   return rotation;
}

// ----------------------------------------------------------------------------

static String FieldString( double field )
{
   int sign, s1, s2; double s3;
   IsoString::ToSexagesimal( field ).ParseSexagesimal( sign, s1, s2, s3 );
   if ( s1 > 0 )
      return String().Format( "%dd %d' %.1f\"", s1, s2, s3 );
   if ( s2 > 0 )
      return String().Format( "%d' %.1f\"", s2, s3 );
   return String().Format( "%.2f\"", s3 );
}

static String CelestialToString( const DPoint& pRD )
{
   double ra = pRD.x;
   if ( ra < 0 )
      ra += 360;
   return String()
   << "RA: "   << String().ToSexagesimal( ra/15,
                     SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/,
                                                   2/*width*/, ' '/*separator*/, '0'/*padding*/ ) )
   << " Dec: " << String().ToSexagesimal( pRD.y,
                     SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/,
                                                   3/*width*/, ' '/*separator*/, '0'/*padding*/ ) );
}

static String ImageToCelestialToString( const AstrometricMetadata* A, const DPoint& pI )
{
   DPoint pRD;
   if ( A->ImageToCelestial( pRD, pI ) )
      return CelestialToString( pRD );
   return "------";
}

String AstrometricMetadata::Summary() const
{
   if ( !IsValid() )
      throw Error( "Invalid call to AstrometricMetadata::Summary(): No astrometric solution" );

   LinearTransformation linearTransIW = m_transformWI->ApproximateLinearTransform();
   DPoint projOrgPx = linearTransIW.TransformInverse( DPoint( 0, 0 ) );
   DPoint projOrgRD = m_projection->ProjectionOrigin();
   bool flipped;
   double rotation = Rotation( flipped );

   String summary;
   summary  << "Referentiation Matrix (World = Matrix * Coords[x,y]):" << '\n'
            << linearTransIW.ToString( 3 ) << '\n'
            << "Projection .......... " << m_projection->Name() << '\n'
            << "Projection origin ... " << String().Format( "[%.6f %.6f]px", projOrgPx.x, projOrgPx.y )
                                        << " -> [" << CelestialToString( projOrgRD ) << ']' << '\n'
            << "Resolution .......... " << String().Format( "%.3f arcsec/px", m_resolution*3600 ) << '\n'
            << "Rotation ............ " << String().Format( "%.3f deg", rotation ) << (flipped ? " (flipped)" : "") << '\n';

   if ( m_xpixsz.IsDefined() )
      if ( m_xpixsz() > 0 )
         if ( m_focal.IsDefined() )
         {
            summary
            << "Focal ............... " << String().Format( "%.2f mm", m_focal() ) << '\n'
            << "Pixel size .......... " << String().Format( "%.2f um", m_xpixsz() ) << '\n';
         }

   summary  << "Field of view ....... " << FieldString( m_width*m_resolution ) << " x " << FieldString( m_height*m_resolution ) << '\n'
            << "Image center ........ " << ImageToCelestialToString( this, DPoint( m_width/2.0, m_height/2.0 ) ) << '\n'
            << "Image bounds:" << '\n'
            << "   top-left ......... " << ImageToCelestialToString( this, DPoint( 0, 0 ) ) << '\n'
            << "   top-right ........ " << ImageToCelestialToString( this, DPoint( m_width, 0 ) ) << '\n'
            << "   bottom-left ...... " << ImageToCelestialToString( this, DPoint( 0, m_height ) ) << '\n'
            << "   bottom-right ..... " << ImageToCelestialToString( this, DPoint( m_width, m_height ) ) << '\n';

   return summary;
}

// ----------------------------------------------------------------------------

static void ModifyKeyword( FITSKeywordArray& keywords, IsoString name, IsoString value, IsoString comment )
{
   for ( FITSHeaderKeyword& keyword : keywords )
      if ( keyword.name == name )
      {
         keyword.value = value;
         keyword.comment = comment;
         return;
      }

   keywords << FITSHeaderKeyword( name, value, comment );
}

static void RemoveKeyword( FITSKeywordArray& keywords, IsoString name )
{
   keywords.Remove( FITSHeaderKeyword( name ), []( const FITSHeaderKeyword& k1, const FITSHeaderKeyword& k2 )
                                               { return k1.name == k2.name; } );
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::UpdateBasicKeywords( FITSKeywordArray& keywords ) const
{
   if ( m_focal.IsDefined() && m_focal() > 0 )
      ModifyKeyword( keywords, "FOCALLEN", IsoString().Format( "%.2f", m_focal() ), "Focal length (mm)" );
   else
      RemoveKeyword( keywords, "FOCALLEN" );

   if ( m_xpixsz.IsDefined() )
      if ( m_xpixsz() > 0 )
      {
         ModifyKeyword( keywords, "XPIXSZ", IsoString().Format( "%.3f", m_xpixsz() ), "Pixel size, X-axis (um)" );
         ModifyKeyword( keywords, "YPIXSZ", IsoString().Format( "%.3f", m_xpixsz() ), "Pixel size, Y-axis (um)" );
      }

   /*
    * ### N.B.: It is unclear if writing the center coordinates to nonstandard
    * OBJCTRA/OBJCTDEC keywords is really a good idea. However, this is
    * customary practice...
    */
   DPoint center;
   if ( ImageToCelestial( center, DPoint( m_width/2.0, m_height/2.0 ) ) )
   {
      ModifyKeyword( keywords, "OBJCTRA",
               '\'' + IsoString::ToSexagesimal( center.x/15,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/, 0/*width*/, ' '/*separator*/ ) ) + '\'',
               "Right Ascension of the center of the image" );
      ModifyKeyword( keywords, "OBJCTDEC",
               '\'' + IsoString::ToSexagesimal( center.y,
                           SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/, 0/*width*/, ' '/*separator*/ ) ) + '\'',
               "Declination of the center of the image" );
   }
}

// ----------------------------------------------------------------------------

void AstrometricMetadata::UpdateWCSKeywords( FITSKeywordArray& keywords ) const
{
   RemoveKeyword( keywords, "EQUINOX" );
   RemoveKeyword( keywords, "CTYPE1" );
   RemoveKeyword( keywords, "CTYPE2" );
   RemoveKeyword( keywords, "CRPIX1" );
   RemoveKeyword( keywords, "CRPIX2" );
   RemoveKeyword( keywords, "CRVAL1" );
   RemoveKeyword( keywords, "CRVAL2" );
   RemoveKeyword( keywords, "PV1_1" );
   RemoveKeyword( keywords, "PV1_2" );
   RemoveKeyword( keywords, "LONPOLE" );
   RemoveKeyword( keywords, "LATPOLE" );
   RemoveKeyword( keywords, "CD1_1" );
   RemoveKeyword( keywords, "CD1_2" );
   RemoveKeyword( keywords, "CD2_1" );
   RemoveKeyword( keywords, "CD2_2" );
   RemoveKeyword( keywords, "REFSPLINE" );
   RemoveKeyword( keywords, "CDELT1" );
   RemoveKeyword( keywords, "CDELT2" );
   RemoveKeyword( keywords, "CROTA1" );
   RemoveKeyword( keywords, "CROTA2" );

   if ( IsValid() )
   {
      WCSKeywords wcs = GetWCSvalues();

      keywords << FITSHeaderKeyword( "EQUINOX", "2000.0", "Equinox of the celestial coordinate system" )
               << FITSHeaderKeyword( "CTYPE1", wcs.ctype1, "Axis1 projection: " + m_projection->Name() )
               << FITSHeaderKeyword( "CTYPE2", wcs.ctype2, "Axis2 projection: " + m_projection->Name() )
               << FITSHeaderKeyword( "CRPIX1", IsoString().Format( "%.6f", wcs.crpix1() ), "Axis1 reference pixel" )
               << FITSHeaderKeyword( "CRPIX2", IsoString().Format( "%.6f", wcs.crpix2() ), "Axis2 reference pixel" );

      if ( wcs.crval1.IsDefined() )
         keywords << FITSHeaderKeyword( "CRVAL1", IsoString().Format( "%.12g", wcs.crval1() ), "Axis1 reference value" );
      if ( wcs.crval2.IsDefined() )
         keywords << FITSHeaderKeyword( "CRVAL2", IsoString().Format( "%.12g", wcs.crval2() ), "Axis2 reference value" );
      if ( wcs.pv1_1.IsDefined() )
         keywords << FITSHeaderKeyword( "PV1_1", IsoString().Format( "%.12g", wcs.pv1_1() ), "Native longitude of the reference point" );
      if ( wcs.pv1_2.IsDefined() )
         keywords << FITSHeaderKeyword( "PV1_2", IsoString().Format( "%.12g", wcs.pv1_2() ), "Native latitude of the reference point" );
      if ( wcs.lonpole.IsDefined() )
         keywords << FITSHeaderKeyword( "LONPOLE", IsoString().Format( "%.12g", wcs.lonpole() ), "Longitude of the celestial pole" );
      if ( wcs.latpole.IsDefined() )
         keywords << FITSHeaderKeyword( "LATPOLE", IsoString().Format( "%.12g", wcs.latpole() ), "Latitude of the celestial pole" );

      keywords << FITSHeaderKeyword( "CD1_1", IsoString().Format( "%.12g", wcs.cd1_1() ), "Scale matrix (1,1)" )
               << FITSHeaderKeyword( "CD1_2", IsoString().Format( "%.12g", wcs.cd1_2() ), "Scale matrix (1,2)" )
               << FITSHeaderKeyword( "CD2_1", IsoString().Format( "%.12g", wcs.cd2_1() ), "Scale matrix (2,1)" )
               << FITSHeaderKeyword( "CD2_2", IsoString().Format( "%.12g", wcs.cd2_2() ), "Scale matrix (2,2)" );

      if ( HasSplineWorldTransformation() )
         keywords << FITSHeaderKeyword( "REFSPLINE", "T", "Coordinates stored in properties as splines" );

      // AIPS keywords (CDELT1, CDELT2, CROTA1, CROTA2)
      keywords << FITSHeaderKeyword( "CDELT1", IsoString().Format( "%.12g", wcs.cdelt1() ), "Axis1 scale" )
               << FITSHeaderKeyword( "CDELT2", IsoString().Format( "%.12g", wcs.cdelt2() ), "Axis2 scale" )
               << FITSHeaderKeyword( "CROTA1", IsoString().Format( "%.12g", wcs.crota1() ), "Axis1 rotation angle (deg)" )
               << FITSHeaderKeyword( "CROTA2", IsoString().Format( "%.12g", wcs.crota2() ), "Axis2 rotation angle (deg)" );
   }
}

// ----------------------------------------------------------------------------

WCSKeywords AstrometricMetadata::GetWCSvalues() const
{
   if ( !IsValid() )
      throw Error( "AstrometricMetadata::GetWCSvalues(): Invalid or uninitialized metadata" );

   LinearTransformation trans_F_I( 1,  0, -0.5,
                                   0, -1,  m_height+0.5 );
   LinearTransformation trans_I_W = m_transformWI->ApproximateLinearTransform();
   LinearTransformation trans_F_W = trans_I_W.Multiply( trans_F_I );

   WCSKeywords wcs;
   m_projection->GetWCS( wcs );

   wcs.cd1_1 = trans_F_W.A00();
   wcs.cd1_2 = trans_F_W.A01();
   wcs.cd2_1 = trans_F_W.A10();
   wcs.cd2_2 = trans_F_W.A11();

   DPoint orgF = trans_F_W.TransformInverse( DPoint( 0 ) );
   wcs.crpix1 = orgF.x;
   wcs.crpix2 = orgF.y;

   /*
    * CDELT1, CDELT2 and CROTA2 are computed using the formulas in section 6.2
    * of http://fits.gsfc.nasa.gov/fits_wcs.html "Representations of celestial
    * coordinates in FITS"
    */
   double rot1, rot2;
   if ( wcs.cd2_1() > 0 )
      rot1 = ArcTan( wcs.cd2_1(), wcs.cd1_1() );
   else if ( wcs.cd2_1() < 0 )
      rot1 = ArcTan( -wcs.cd2_1(), -wcs.cd1_1() );
   else
      rot1 = 0;

   if ( wcs.cd1_2() > 0 )
      rot2 = ArcTan( wcs.cd1_2(), -wcs.cd2_2() );
   else if ( wcs.cd1_2() < 0 )
      rot2 = ArcTan( -wcs.cd1_2(), wcs.cd2_2() );
   else
      rot2 = 0;

   double rot = (rot1 + rot2)/2;
   rot2 = rot1 = rot;
   double sinrot, cosrot;
   SinCos( rot, sinrot, cosrot );
   if ( Abs( cosrot ) > Abs( sinrot ) )
   {
      wcs.cdelt1 =  wcs.cd1_1()/cosrot;
      wcs.cdelt2 =  wcs.cd2_2()/cosrot;
   }
   else
   {
      wcs.cdelt1 =  wcs.cd2_1()/sinrot;
      wcs.cdelt2 = -wcs.cd1_2()/sinrot;
   }

   wcs.crota1 = Deg( rot1 );
   wcs.crota2 = Deg( rot2 );

   return wcs;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/AstrometricMetadata.cpp - Released 2017-06-28T11:58:42Z
