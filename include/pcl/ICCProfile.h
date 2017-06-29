//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/ICCProfile.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_ICCProfile_h
#define __PCL_ICCProfile_h

/// \file pcl/ICCProfile.h

#include <pcl/Defs.h>

#include <pcl/ByteArray.h>
#include <pcl/Flags.h>
#include <pcl/StringList.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup color_management Color Management Classes
 */

/*!
 * \namespace pcl::ICCProfileClass
 * \brief Representation of ICC color profile classes
 * \ingroup color_management
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ICCProfileClass::Unknown</td>              <td>Unknown or unsupported ICC profile class</td></tr>
 * <tr><td>ICCProfileClass::Any</td>                  <td>Any ICC profile class</td></tr>
 * <tr><td>ICCProfileClass::InputDevice</td>          <td>Input device (scanner) profile</td></tr>
 * <tr><td>ICCProfileClass::DisplayDevice</td>        <td>Display device (monitor) profile</td></tr>
 * <tr><td>ICCProfileClass::OutputDevice</td>         <td>Output device (printer) profile</td></tr>
 * <tr><td>ICCProfileClass::DeviceLink</td>           <td>Device link profile</td></tr>
 * <tr><td>ICCProfileClass::ColorSpaceConversion</td> <td>Color space conversion profile</td></tr>
 * <tr><td>ICCProfileClass::AbstractProfile</td>      <td>Abstract profile</td></tr>
 * <tr><td>ICCProfileClass::NamedColorProfile</td>    <td>Named color profile</td></tr>
 * </table>
 */
namespace ICCProfileClass
{
   enum mask_type
   {
      Unknown              = 0x80000000,
      Any                  = 0x00000000,
      InputDevice          = 0x00000001,
      DisplayDevice        = 0x00000002,
      OutputDevice         = 0x00000004,
      DeviceLink           = 0x00000008,
      ColorSpaceConversion = 0x00000010,
      AbstractProfile      = 0x00000020,
      NamedColorProfile    = 0x00000040
   };
}

/*!
 * \class pcl::ICCProfileClasses
 * \brief A combination of ICCProfileClass values.
 * \ingroup color_management
 */
typedef Flags<ICCProfileClass::mask_type>   ICCProfileClasses;

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ICCColorSpace
 * \brief Representation of data and connection color spaces in ICC color profiles.
 * \ingroup color_management
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ICCColorSpace::Unknown</td> <td>Unknown or unsupported color space</td></tr>
 * <tr><td>ICCColorSpace::Any</td>     <td>Any color space in an ICC profile</td></tr>
 * <tr><td>ICCColorSpace::XYZ</td>     <td>CIE XYZ color space</td></tr>
 * <tr><td>ICCColorSpace::Lab</td>     <td>CIE L*a*b* color space</td></tr>
 * <tr><td>ICCColorSpace::Luv</td>     <td>CIE L*u*v* color space</td></tr>
 * <tr><td>ICCColorSpace::YCbCr</td>   <td>YCbCr color space</td></tr>
 * <tr><td>ICCColorSpace::Yxy</td>     <td>Yxy (luminance and chromaticity coordinates)</td></tr>
 * <tr><td>ICCColorSpace::RGB</td>     <td>RGB color space</td></tr>
 * <tr><td>ICCColorSpace::Gray</td>    <td>Grayscale color space</td></tr>
 * <tr><td>ICCColorSpace::HSV</td>     <td>HSV color ordering system</td></tr>
 * <tr><td>ICCColorSpace::HLS</td>     <td>HLS color ordering system</td></tr>
 * <tr><td>ICCColorSpace::CMYK</td>    <td>CMYK color space</td></tr>
 * <tr><td>ICCColorSpace::CMY</td>     <td>CMY color space</td></tr>
 * <tr><td>ICCColorSpace::LuvK</td>    <td>LuvK color space</td></tr>
 * </table>
 *
 * Other color spaces supported by the ICC profile specification, as for
 * example 2-color and 15-color spaces, are unknown to this implementation.
 */
namespace ICCColorSpace
{
   enum mask_type
   {
      Unknown  = 0x80000000,
      Any      = 0x00000000,
      XYZ      = 0x00000001,
      Lab      = 0x00000002,
      Luv      = 0x00000004,
      YCbCr    = 0x00000008,
      Yxy      = 0x00000010,
      RGB      = 0x00000020,
      Gray     = 0x00000040,
      HSV      = 0x00000080,
      HLS      = 0x00000100,
      CMYK     = 0x00000200,
      CMY      = 0x00000400,
      LuvK     = 0x00000800
   };
}

/*!
 * \class pcl::ICCColorSpaces
 * \brief A combination of ICCColorSpace values.
 * \ingroup color_management
 */
typedef Flags<ICCColorSpace::mask_type>  ICCColorSpaces;

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ICCRenderingIntent
 * \brief ICC rendering intents
 * \ingroup color_management
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ICCRenderingIntent::Perceptual</td>           <td>Perceptual rendering intent (photographic images)</td></tr>
 * <tr><td>ICCRenderingIntent::Saturation</td>           <td>Saturation rendering intent (graphics)</td></tr>
 * <tr><td>ICCRenderingIntent::RelativeColorimetric</td> <td>Relative colorimetric rendering intent (match white points)</td></tr>
 * <tr><td>ICCRenderingIntent::AbsoluteColorimetric</td> <td>Absolute colorimetric rendering intent (proofing)</td></tr>
 * </table>
 */
namespace ICCRenderingIntent
{
   enum value_type
   {
      Perceptual,
      Saturation,
      RelativeColorimetric,
      AbsoluteColorimetric
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ICCRenderingDirection
 * \brief ICC transform rendering direction
 * \ingroup color_management
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ICCRenderingDirection::Input</td>    <td>Denotes a rendering intent applied to input pixel values.</td></tr>
 * <tr><td>ICCRenderingDirection::Output</td>   <td>Denotes a rendering intent applied to output pixel values.</td></tr>
 * <tr><td>ICCRenderingDirection::Proofing</td> <td>Denotes a rendering intent applied by a proofing transform.</td></tr>
 * </table>
 */
namespace ICCRenderingDirection
{
   enum value_type
   {
      Input,
      Output,
      Proofing
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class ICCProfile
 * \brief A high-level interface to ICC color profiles
 *
 * %ICCProfile is a high-level interface to the ICC profile handling
 * functionality implemented in the PixInsight Core application. An instance of
 * %ICCProfile transports an ICC profile structure that can be embedded in
 * image files or used to build color management transformations with the
 * ICCProfileTransformation class.
 *
 * %ICCProfile implements a set of utility functions for profile validation,
 * profile information retrieval, profile disk I/O and profile directory
 * search, among other tasks.
 *
 * \ingroup color_management
 * \sa ICCProfileTransformation
 */
class PCL_CLASS ICCProfile
{
public:

   /*!
    * Represents an opaque handle to an internal ICC profile.
    */
   typedef void*                                handle;

   /*!
    * Represents an ICC profile device class.
    */
   typedef ICCProfileClass::mask_type           profile_class;

   /*!
    * Represents an ICC profile color space.
    */
   typedef ICCColorSpace::mask_type             color_space;

   /*!
    * Represents an ICC rendering intent.
    */
   typedef ICCRenderingIntent::value_type       rendering_intent;

   /*!
    * Represents an ICC transform rendering direction.
    */
   typedef ICCRenderingDirection::value_type    rendering_direction;

   /*!
    * Constructs an empty %ICCProfile object. An empty %ICCProfile doesn't
    * store an ICC profile structure.
    */
   ICCProfile() = default;

   /*!
    * Copy constructor.
    */
   ICCProfile( const ICCProfile& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   ICCProfile( ICCProfile&& ) = default;
#endif

   /*!
    * Constructs an %ICCProfile object and loads an ICC profile from a file at
    * the specified \a profilePath.
    *
    * If the specified file does not exist, is not readable, or does not
    * contain a valid ICC profile structure, this constructor throws an Error
    * exception.
    */
   ICCProfile( const String& profilePath )
   {
      Load( profilePath );
   }

   /*!
    * Constructs an %ICCProfile object to store a copy of the raw ICC profile
    * structure available in the specified container. This constructor simply
    * calls Set( rawData ).
    */
   ICCProfile( const ByteArray& rawData )
   {
      Set( rawData );
   }

   /*!
    * Constructs an %ICCProfile object to store a copy of the raw ICC profile
    * structure at the specified location. This constructor simply calls
    * Set( rawData ).
    */
   ICCProfile( const void* rawData )
   {
      Set( rawData );
   }

   /*!
    * Destroys an %ICCProfile object. If this object stores an ICC profile
    * structure, it is deallocated upon destruction.
    */
   virtual ~ICCProfile()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ICCProfile& operator =( const ICCProfile& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   ICCProfile& operator =( ICCProfile&& ) = default;
#endif

   /*!
    * Returns true iff this object stores an ICC color profile structure.
    */
   bool IsProfile() const
   {
      return !m_data.IsEmpty();
   }

   /*!
    * A convenience synonym for IsProfile().
    */
   operator bool() const
   {
      return IsProfile();
   }

   /*!
    * Returns true if this profile has been read from a disk file. Otherwise
    * this object either does not transport an ICC profile, or the profile has
    * been generated or assigned directly from a memory buffer.
    */
   bool IsDiskProfile() const
   {
      return !m_path.IsEmpty();
   }

   /*!
    * Returns true iff the ICC profile stored in this object is identical to the
    * profile stored in \a other %ICCProfile object.
    *
    * If necessary, this function performs a byte-to-byte comparison between
    * both ICC profile structures (when both have the same size in bytes).
    */
   bool IsSameProfile( const ICCProfile& other ) const;

   /*!
    * Returns a reference to the immutable container that stores an ICC profile
    * in this %ICCProfile object. The returned ByteArray is empty if this
    * object does not transport an ICC color profile.
    */
   const ByteArray& ProfileData() const
   {
      return m_data;
   }

   /*!
    * Returns the length of the stored ICC profile structure in bytes. Returns
    * zero if this %ICCProfile object doesn't store an ICC color profile.
    */
   size_type ProfileSize() const;

   /*!
    * Returns the full file path of the stored ICC profile, if this object
    * transports an ICC profile structure that has been loaded from a file, or
    * an empty string otherwise.
    */
   String FilePath() const
   {
      return m_path;
   }

   /*!
    * Returns the localized profile description of the stored ICC profile.
    *
    * \param language   Optional language code (ISO 639/2) of the requested
    *                   profile description. The default value is "en" for
    *                   English.
    *
    * \param country    Optional country code (ISO 3166) of the requested
    *                   profile description. The default value is "US" for
    *                   United States.
    *
    * If this object does not transport an ICC profile, this routine returns an
    * empty string. Otherwise this routine will always return a valid profile
    * description, even if no localized version exists for the requested
    * language and country. The returned description will be as close as
    * possible to the requested localization.
    */
   String Description( const char* language = "en", const char* country = "US" ) const;

   /*!
    * Returns the localized manufacturer information of the stored ICC profile.
    *
    * See Description( const char*, const char* ) const for detailed
    * information on function parameters and behavior.
    */
   String Manufacturer( const char* language = "en", const char* country = "US" ) const;

   /*!
    * Returns the localized device model information of the stored ICC profile.
    *
    * See Description( const char*, const char* ) const for detailed
    * information on function parameters and behavior.
    */
   String Model( const char* language = "en", const char* country = "US" ) const;

   /*!
    * Returns the localized copyright information of the stored ICC profile.
    *
    * See Description( const char*, const char* ) const for detailed
    * information on function parameters and behavior.
    */
   String Copyright( const char* language = "en", const char* country = "US" ) const;

   /*!
    * Retrieves localized description, manufacturer, device model and copyright
    * information from the stored ICC profile.
    *
    * If this object does not transport an ICC profile, this routine clears the
    * \a description, \a manufacturer, \a model and \a copyright strings. If a
    * valid ICC profile is stored in this object, the passed strings will be
    * assigned with the corresponding information, as close as possible to the
    * requested localization. See Description( const char*, const char* ) const
    * for detailed information on localization parameters.
    */
   void GetInformation( String& description, String& manufacturer, String& model, String& copyright,
                        const char* language = "en", const char* country = "US" ) const;

   /*!
    * Returns the ICC profile class of the stored profile. See the
    * ICCProfileClass namespace for supported profile classes.
    *
    * If this object doesn't transport an ICC profile, this function returns
    * ICCProfileClass::Unknown.
    */
   profile_class Class() const;

   /*!
    * Returns the ICC color space of the stored profile. See the ICCColorSpace
    * namespace for supported color spaces.
    *
    * If this object doesn't transport an ICC profile, this function returns
    * ICCColorSpace::Unknown.
    */
   color_space ColorSpace() const;

   /*!
    * Returns true iff this object stores an RGB ICC profile.
    */
   bool IsRGB() const
   {
      return ColorSpace() == ICCColorSpace::RGB;
   }

   /*!
    * Returns true iff this object stores a grayscale ICC profile.
    */
   bool IsGrayscale() const
   {
      return ColorSpace() == ICCColorSpace::Gray;
   }

   /*!
    * Returns true iff this object stores an ICC profile that supports the
    * specified \a intent in the specified transform \a direction. See the
    * ICCRenderingIntent and ICCRenderingDirection namespaces, respectively,
    * for supported rendering intents and directions.
    */
   bool SupportsRenderingIntent( rendering_intent intent, rendering_direction direction ) const;

   /*!
    * Returns true iff this object stores an embedded ICC profile.
    *
    * An embedded ICC profile must have the <em>Embedded Profile</em> bit field
    * (bit #0 of header byte #44) set.
    */
   bool IsEmbedded() const;

   /*!
    * Sets or clears the <em>Embedded Profile</em> header bit field (bit #0 of
    * header byte #44) of the stored ICC profile.
    *
    * \note If this %ICCProfile object is being used to embed an ICC profile in
    * an image file, the <em>Embedded Profile</em> flag must be set before
    * embedding.
    */
   void SetEmbeddedFlag( bool on = true );

   /*!
    * Clears or sets the <em>Embedded Profile</em> header bit field (bit #0 of
    * header byte #44) of the stored ICC profile.
    *
    * This is a convenience member function, equivalent to
    * SetEmbeddedFlag( false )
    */
   void ClearEmbeddedFlag()
   {
      SetEmbeddedFlag( false );
   }

   /*!
    * Loads an ICC profile from a disk file at \a profilePath and stores it in
    * this %ICCProfile object.
    *
    * If this object stores an ICC profile before calling this function, it is
    * deallocated upon successful load of the specified ICC profile.
    *
    * If the specified file does not exist or is not readable, or if the file
    * contains data that cannot be identified as a valid ICC profile, this
    * function throws an Error exception.
    */
   void Load( const String& profilePath );

   /*!
    * Forces this %ICCProfile object to store a copy of the ICC profile stored
    * in the specified ByteArray container. Previously existing ICC profile
    * data will be deallocated.
    *
    * If the specified container is empty, calling this function is equivalent
    * to Clear(). If the container is not empty, it must store a valid ICC
    * profile structure; otherwise this function will throw an Error exception.
    */
   void Set( const ByteArray& profile );

   /*!
    * Forces this %ICCProfile object to store a copy of the raw ICC profile
    * data at the specified location. Previously existing ICC profile data will
    * be deallocated.
    *
    * The specified pointer must be the starting address of a valid ICC profile
    * structure; otherwise this function will throw an Error exception.
    */
   void Set( const void* rawData );

   /*!
    * Deallocates the stored ICC profile structure and all auxiliary data.
    *
    * If the profile has been opened before calling this function, any existing
    * handle to it will not be closed or invalidated.
    */
   void Clear()
   {
      m_data.Clear();
      m_path.Clear();
   }

   /*!
    * Opens the ICC profile stored in this %ICCProfile object. Returns a handle
    * to the opened ICC profile.
    *
    * If this object does not transport a valid ICC profile structure, this
    * function throws an Error exception.
    *
    * Color management transformations are defined through handles to open ICC
    * profiles. The caller is responsible for closing ICC profile handles when
    * they are no longer needed.
    */
   handle Open() const
   {
      return Open( m_data );
   }

   /*!
    * Exchanges two ICC profiles \a x1 and \a x2.
    */
   friend void Swap( ICCProfile& x1, ICCProfile& x2 )
   {
      pcl::Swap( x1.m_data, x2.m_data );
      pcl::Swap( x1.m_path, x2.m_path );
   }

   /*!
    * Opens an ICC profile disk file at \a profilePath. Returns a handle to the
    * opened ICC profile.
    *
    * If the specified file does not exist or is not readable, or if it does
    * not contain a valid ICC profile structure, this function throws an Error
    * exception.
    */
   static handle Open( const String& profilePath );

   /*!
    * Opens an ICC profile from raw ICC profile data stored at the specified
    * location. Returns a handle to the opened ICC profile.
    *
    * If the argument does not point to a valid ICC profile structure, this
    * function throws an Error exception.
    */
   static handle Open( const void* rawData );

   /*!
    * Opens an ICC profile stored in the specified ByteArray container \a icc.
    * Returns a handle to the opened ICC profile.
    *
    * If the specified container is empty or does not contain a valid ICC
    * profile structure, this function throws an Error exception.
    */
   static handle Open( const ByteArray& icc )
   {
      return Open( icc.Begin() );
   }

   /*!
    * Closes an open ICC profile handle.
    */
   static void Close( handle h );

   /*!
    * Validates an ICC profile handle. Returns true iff the specified handle is
    * a valid handle to an open ICC profile.
    */
   static bool IsValidHandle( handle h );

   /*!
    * Validates an ICC profile structure stored in a file at \a profilePath.
    * Returns true iff the specified file exists and contains a valid ICC
    * profile.
    */
   static bool IsValidFile( const String& profilePath );

   /*!
    * Validates an ICC profile from raw ICC profile data stored at the
    * specified location. Returns true iff the argument points to a valid ICC
    * profile structure.
    */
   static bool IsValid( const void* rawdata );

   /*!
    * Validates an ICC profile structure stored in the specified ByteArray
    * container \a icc. Returns true iff the container stores a valid ICC
    * profile.
    */
   static bool IsValid( const ByteArray& icc )
   {
      return IsValid( icc.Begin() );
   }

   /*!
    * Returns the localized profile description of an open ICC profile.
    *
    * \param h          Handle to an open ICC profile, from which the requested
    *                   profile description will be obtained.
    *
    * \param language   Optional language code (ISO 639/2) of the requested
    *                   profile description. The default value is "en" for
    *                   English.
    *
    * \param country    Optional country code (ISO 3166) of the requested
    *                   profile description. The default value is "US" for the
    *                   United States.
    *
    * This routine will always return a valid profile description, even if no
    * localized version is available for the requested language and country.
    * The returned description will be as close as possible to the requested
    * localization.
    */
   static String Description( handle h, const char* language = "en", const char* country = "US" );

   /*!
    * Returns the localized manufacturer information of an open ICC profile.
    *
    * See Description( handle, const char*, const char* ) for detailed
    * information on function parameters and behavior.
    */
   static String Manufacturer( handle h, const char* language = "en", const char* country = "US" );

   /*!
    * Returns the localized device model information of an open ICC profile.
    *
    * See Description( handle, const char*, const char* ) for detailed
    * information on function parameters and behavior.
    */
   static String Model( handle h, const char* language = "en", const char* country = "US" );

   /*!
    * Returns the localized copyright information of an open ICC profile.
    *
    * See Description( handle, const char*, const char* ) for detailed
    * information on function parameters and behavior.
    */
   static String Copyright( handle h, const char* language = "en", const char* country = "US" );

   /*!
    * Returns the ICC profile class of an open profile. See the ICCProfileClass
    * namespace for supported profile classes.
    */
   static profile_class Class( handle h );

   /*!
    * Returns the ICC color space of an open profile. See the ICCColorSpace
    * namespace for supported color spaces.
    */
   static color_space ColorSpace( handle h );

   /*!
    * Returns true iff an open profile supports the specified \a intent in the
    * specified transform \a direction. See the ICCRenderingIntent and
    * ICCRenderingDirection namespaces, respectively, for supported rendering
    * intents and directions.
    */
   static bool SupportsRenderingIntent( handle h, rendering_intent intent, rendering_direction direction );

   /*!
    * Returns the list of profile directories.
    *
    * The profile directories contain the ICC profiles currently installed in
    * the system. The specific directories provided by this function are
    * platform-dependent.
    *
    * On X11 Linux and FreeBSD, there is no universally standardized color
    * management system. By default, the returned list will include a single
    * directory that can be one of the following:
    *
    *    /usr/share/color/icc
    *    ~/.color/icc
    *
    * for system-wide or user-local profiles, respectively. If none of the
    * above directories exists (and is readable) on the local filesystem, the
    * routine will return a fallback directory within the installation
    * directory tree of the PixInsight Core application.
    *
    * On OS X, the returned list may contain one or more from the following
    * directories:
    *
    *    ~/Library/ColorSync/Profiles
    *    /Library/ColorSync/Profiles
    *    /Network/Library/ColorSync/Profiles
    *
    * Finally, on Windows this function returns the current COLOR directory,
    * as reported by the GetColorDirectory() Win32 API function.
    */
   static StringList ProfileDirectories();

   /*!
    * Gets a list of full paths for every ICC profile on a given directory.
    *
    * \param dirPath    Optional path to a search directory. If no directory is
    *                   specified, or if an empty string is passed, the whole
    *                   list of system profile directories, as returned by the
    *                   ProfileDirectories() member function, will be searched
    *                   recursively.
    *
    * This routine performs a recursive directory search on the specified
    * directory, or on each system profiles directory if no directory is
    * specified. Profiles are quickly identified by opening them and validating
    * their profile headers. The search is not limited to any particular file
    * suffix such as ".icc" or ".icm".
    *
    * Returns a list of full paths to the ICC profile files found on the
    * search directory.
    */
   static StringList FindProfiles( const String& dirPath = String() );

   /*!
    * Finds the file path to an installed ICC profile, given its profile
    * description.
    *
    * \param description   Description of the profile to search for.
    *
    * \param exactMatch    If true, this routine will search for a profile that
    *                   matches the specified \a description exactly, including
    *                   character case. If false, the routine will report any
    *                   profile whose description contains the specified
    *                   description performing case-insensitive comparisons.
    *                   The default value is true, so profile descriptions must
    *                   be matched exactly by default.
    *
    * This function searches the system color directories, as reported by the
    * ProfileDirectories() member function, until it finds an ICC profile with
    * the specified \a description. This routine performs a recursive directory
    * search on each profiles directory.
    *
    * Returns the full path to the ICC profile, or an empty string if no
    * profile was found matching the specified \a description.
    */
   static String FindInstalledProfile( const String& description, bool exactMatch = true );

   /*!
    * Extracts a subset of profile paths and their descriptions from a
    * previously retrieved list of full paths to ICC profiles.
    *
    * \param[out] selectedDescriptionsList   The list of profile descriptions
    *                                        for selected profiles.
    *
    * \param[out] selectedPathsList    The list of paths to selected profiles.
    *
    * \param pathList   The input list of paths to ICC profiles.
    *
    * \param spaces     A combination of ICC color spaces for selected ICC
    *                   profiles.
    *
    * \param classes    A combination of ICC device classes for selected ICC
    *                   profiles.
    */
   static void ExtractProfileList( StringList& selectedDescriptionsList,
                                   StringList& selectedPathsList,
                                   const StringList& pathList,
                                   ICCColorSpaces spaces = ICCColorSpace::Any,
                                   ICCProfileClasses classes = ICCProfileClass::Any );

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::ICCProfile::Info
    * \brief A structure to hold descriptive data about ICC profiles.
    * \ingroup color_management
    *
    * The %ICCProfile::Info structure is designed to be used with the
    * ICCProfile::FindProfilesByColorSpace() utility function.
    */
   struct Info
   {
      String description;  //!< Description of an ICC profile
      String path;         //!< Full path to an ICC profile disk file

      /*!
       * Constructs an %ICCProfile::Info structure.
       */
      Info( const String& a_description, const String& a_path = String() ) :
      description( a_description ), path( a_path )
      {
      }

      /*!
       * Equality operator. Two %ICCProfile::Info objects are equal if their
       * profile descriptions are equal.
       */
      bool operator ==( const Info& x ) const
      {
         return description == x.description;
      }

      /*!
       * Less than relational operator. %ICCProfile::Info objects are sorted by
       * their profile descriptions
       */
      bool operator <( const Info& x ) const
      {
         return description < x.description;
      }
   };

   /*!
    * Represents a sorted list of ICC profile information items.
    */
   typedef SortedArray<Info> profile_list;

   /*!
    * Returns a sorted list of profile information items (full file paths and
    * profile description strings) for the existing ICC profiles of the
    * specified ICC profile color spaces.
    *
    * \param spaces  A combination of ICC profile color spaces to search for.
    *
    * The search operation is restricted to the system color directories, as
    * reported by the ProfileDirectories() member function. This routine
    * performs a recursive directory search on each profiles directory.
    *
    * Duplicate profiles are not included in the output \a info list. If the
    * same profile is present in two or more directories, or if the same
    * profile is present on the same directory with different file names, only
    * the first instance seen will be included. Profiles are compared by their
    * profile descriptions.
    */
   static profile_list FindProfilesByColorSpace( ICCColorSpaces spaces );

   /*!
    * \internal
    */
   static void ThrowErrorWithCMSInfo( const String& message );

private:

   ByteArray m_data; // ICC profile data
   String    m_path; // empty if this is an embedded or newly created profile
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ICCProfile_h

// ----------------------------------------------------------------------------
// EOF pcl/ICCProfile.h - Released 2017-06-28T11:58:36Z
