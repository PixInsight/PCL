//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/ICCProfileTransformation.h - Released 2017-05-02T10:38:59Z
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

#ifndef __PCL_ICCProfileTransformation_h
#define __PCL_ICCProfileTransformation_h

/// \file pcl/ICCProfileTransformation.h

#include <pcl/Defs.h>

#include <pcl/Array.h>
#include <pcl/Color.h>
#include <pcl/ICCProfile.h>
#include <pcl/ImageTransformation.h>

// ----------------------------------------------------------------------------

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
   class ImageWindow;
} // pi
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ICCProfileTransformation
 * \brief Conversion of pixel values between ICC profile color spaces
 *
 * %ICCProfileTransformation is a portable color transformation based on ICC
 * color profiles. This class is a high-level interface to the color management
 * functionality implemented in the PixInsight Core application.
 *
 * %ICCProfileTransformation implements simple profile-to-profile color
 * transformations, multiprofile transformations, and device proofing
 * transformations with out-of-gamut checks.
 *
 * \ingroup color_management
 * \sa ICCProfile, ImageTransformation
 */
class PCL_CLASS ICCProfileTransformation : public ImageTransformation
{
public:

   /*!
    * Represents an ICC rendering intent.
    */
   typedef ICCRenderingIntent::value_type    rendering_intent;

   /*!
    * Represents an opaque handle to a server-side ICC transformation.
    */
   typedef void*                             transformation_handle;

   /*!
    * Represents a list of handles to open ICC profiles.
    */
   typedef Array<ICCProfile::handle>         profile_list;

   /*!
    * Constructs an empty %ICCPRofileTransformation object.
    */
   ICCProfileTransformation() :
      ImageTransformation(),
      m_transformation( nullptr ),
      m_profiles(),
      m_intent( ICCRenderingIntent::Perceptual ),
      m_proofingIntent( ICCRenderingIntent::AbsoluteColorimetric ),
      m_blackPointCompensation( false ),
      m_forceFloatingPoint( false ),
      m_highResolutionCLUT( true ),
      m_lowResolutionCLUT( false ),
      m_proofingTransformation( false ),
      m_gamutCheck( false ),
      m_parallel( true ),
      m_maxProcessors( PCL_MAX_PROCESSORS ),
      m_srcRGB( false ),
      m_dstRGB( false ),
      m_floatingPoint( false )
   {
   }

   /*!
    * Move constructor.
    */
   ICCProfileTransformation( ICCProfileTransformation&& x ) :
      ImageTransformation( x ),
      m_transformation( x.m_transformation ),
      m_profiles( std::move( x.m_profiles ) ),
      m_intent( x.m_intent ),
      m_proofingIntent( x.m_proofingIntent ),
      m_blackPointCompensation( x.m_blackPointCompensation ),
      m_forceFloatingPoint( x.m_forceFloatingPoint ),
      m_highResolutionCLUT( x.m_highResolutionCLUT ),
      m_lowResolutionCLUT( x.m_lowResolutionCLUT ),
      m_proofingTransformation( x.m_proofingTransformation ),
      m_gamutCheck( x.m_gamutCheck ),
      m_parallel( x.m_parallel ),
      m_maxProcessors( x.m_maxProcessors ),
      m_srcRGB( x.m_srcRGB ),
      m_dstRGB( x.m_dstRGB ),
      m_floatingPoint( x.m_floatingPoint )
   {
      x.m_transformation = nullptr;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ICCProfileTransformation& operator =( ICCProfileTransformation&& x )
   {
      (void)ImageTransformation::operator =( x );
      m_transformation = x.m_transformation;
      m_profiles = std::move( x.m_profiles );
      m_intent = x.m_intent;
      m_proofingIntent = x.m_proofingIntent;
      m_blackPointCompensation = x.m_blackPointCompensation;
      m_forceFloatingPoint = x.m_forceFloatingPoint;
      m_highResolutionCLUT = x.m_highResolutionCLUT;
      m_lowResolutionCLUT = x.m_lowResolutionCLUT;
      m_proofingTransformation = x.m_proofingTransformation;
      m_gamutCheck = x.m_gamutCheck;
      m_parallel = x.m_parallel;
      m_maxProcessors = x.m_maxProcessors;
      m_srcRGB = x.m_srcRGB;
      m_dstRGB = x.m_dstRGB;
      m_floatingPoint = x.m_floatingPoint;
      x.m_transformation = nullptr;
      return *this;
   }

   /*!
    * Copy constructor. This constructor is disabled because ICC color profile
    * transformations are unique objects.
    */
   ICCProfileTransformation( const ICCProfileTransformation& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because ICC color profile
    * transformations are unique objects.
    */
   ICCProfileTransformation& operator =( const ICCProfileTransformation& ) = delete;

   /*!
    * Destroys an %ICCPRofileTransformation object.
    */
   virtual ~ICCProfileTransformation()
   {
      Clear();
   }

   /*!
    * Loads an ICC profile from a file at \a profilePath and adds it to this
    * ICC profile transformation.
    */
   void Add( const String& profilePath );

   /*!
    * Adds the specified ICC \a profile to this ICC profile transformation.
    */
   void Add( const ICCProfile& profile );

   /*!
    * Adds an already open ICC profile \a profileHandle to this ICC profile
    * transformation.
    */
   void Add( const ICCProfile::handle profileHandle );

   /*!
    * Returns true iff this object represents a valid color space transformation
    * based on ICC color profiles.
    *
    * For this member function to return true, the underlying low-level
    * transformation should have been created, either implicitly by using
    * this object to apply a color transformation, or explicitly by calling
    * Create().
    */
   bool IsValid() const
   {
      return m_transformation != nullptr;
   }

   /*!
    * Returns a reference to the immutable list of ICC profiles in this ICC
    * profile transformation.
    */
   const profile_list& Profiles() const
   {
      return m_profiles;
   }

   /*!
    * Returns the ICC rendering intent for this ICC profile transformation.
    */
   rendering_intent RenderingIntent() const
   {
      return m_intent;
   }

   /*!
    * Sets the ICC rendering intent for this ICC profile transformation.
    *
    * When no intent is set explicitly with this function, the default
    * rendering intent is ICCRenderingIntent::Perceptual.
    */
   void SetRenderingIntent( rendering_intent i )
   {
      CloseTransformation();
      m_intent = i;
   }

   /*!
    * Returns true iff this ICC profile transformation applies a <em>black point
    * compensation</em> algorithm.
    */
   bool UsingBlackPointCompensation() const
   {
      return m_blackPointCompensation;
   }

   /*!
    * Enables or disables <em>black point compensation</em> for this ICC
    * profile transformation.
    *
    * When not enabled explicitly with this function, no black point
    * compensation is applied by default.
    */
   void EnableBlackPointCompensation( bool enable = true )
   {
      CloseTransformation();
      m_blackPointCompensation = enable;
   }

   /*!
    * Disables or enables <em>black point compensation</em> for this ICC
    * profile transformation.
    *
    * This is a convenience member function, equivalent to
    * EnableBlackPointCompensation( !disable )
    */
   void DisableBlackPointCompensation( bool disable = true )
   {
      EnableBlackPointCompensation( !disable );
   }

   /*!
    * Returns true iff this ICC profile transformation forces the use of
    * floating point operations for computation of transformed pixel samples of
    * all numerical data types.
    *
    * When this option is disabled (which is its default state), 16-bit integer
    * arithmetics will be used for 8-bit and 16-bit integer images. Floating
    * point will always be used for the rest of images, irrespective of the
    * state of this option, in order to preserve numerical accuracy.
    */
   bool ForcesFloatingPointTransformation() const
   {
      return m_forceFloatingPoint;
   }

   /*!
    * Enables or disables enforcement of floating point computations for this
    * ICC profile transformation.
    */
   void ForceFloatingPointTransformation( bool force = true )
   {
      CloseTransformation();
      m_forceFloatingPoint = force;
   }

   /*!
    * Disables or enables enforcement of floating point computations for this
    * ICC profile transformation.
    *
    * This is a convenience member function, equivalent to
    * ForceFloatingPointTransformation( !relax )
    */
   void RelaxFloatingPointTransformation( bool relax = true )
   {
      ForceFloatingPointTransformation( !relax );
   }

   /*!
    * Returns true iff this ICC profile transformation uses high-resolution
    * color lookup tables (CLUTs) for precalculation of device link functions.
    *
    * Disabling high-resolution CLUTs may provide a (usually small) speed
    * improvement.
    *
    * By default, %ICCProfileTransformation uses high-resolution CLUTs.
    */
   bool UsingHighResolutionCLUT() const
   {
      return m_highResolutionCLUT;
   }

   /*!
    * Enables or disables usage of high-resolution CLUT tables for this ICC
    * profile transformation.
    */
   void EnableHighResolutionCLUT( bool enable = true )
   {
      CloseTransformation();
      if ( (m_highResolutionCLUT = enable) == true )
         m_lowResolutionCLUT = false;
   }

   /*!
    * Disables or enables usage of high-resolution CLUT tables for this ICC
    * profile transformation.
    *
    * This is a convenience member function, equivalent to
    * EnableHighResolutionCLUT( !disable )
    */
   void DisableHighResolutionCLUT( bool disable = true )
   {
      EnableHighResolutionCLUT( !disable );
   }

   /*!
    * Returns true iff this ICC profile transformation uses low-resolution color
    * lookup tables (CLUTs) for precalculation of device link functions.
    *
    * Enabling low-resolution CLUTs may provide a (usually small) speed
    * improvement.
    *
    * By default, %ICCProfileTransformation uses high-resolution CLUTs.
    */
   bool UsingLowResolutionCLUT() const
   {
      return m_lowResolutionCLUT;
   }

   /*!
    * Enables or disables usage of low-resolution color lookup tables (CLUTs)
    * in this ICC color transformation.
    */
   void EnableLowResolutionCLUT( bool enable = true )
   {
      CloseTransformation();
      if ( (m_lowResolutionCLUT = enable) == true )
         m_highResolutionCLUT = false;
   }

   /*!
    * Disables or enables usage of low-resolution color lookup tables (CLUTs)
    * in this ICC color transformation.
    *
    * This is a convenience member function, equivalent to
    * EnableLowResolutionCLUT( !disable )
    */
   void DisableLowResolutionCLUT( bool disable = true )
   {
      EnableLowResolutionCLUT( !disable );
   }

   /*!
    * Returns true iff this object represents a device proofing transformation.
    */
   bool IsProofingTransformation() const
   {
      return m_proofingTransformation;
   }

   /*!
    * Forces the immediate creation (or re-creation) of the underlying
    * low-level transformation.
    *
    * You normally shouldn't need to call this member function, since
    * %ICCProfileTransformation automatically creates the necessary low-level
    * transformations at the points they are required.
    */
   void Create()
   {
      CreateTransformation( m_forceFloatingPoint );
   }

   /*!
    * \internal
    * Returns the low-level handle that this %ICCProfileTransformation object
    * serves as a high-level interface to.
    *
    * This function is intended for private use of the PixInsight Core
    * application; you normally shouldn't need to call it.
    */
   transformation_handle Handle() const
   {
      return m_transformation;
   }

   /*!
    * Resets this %ICCProfileTransformation object to a default state.
    *
    * The list of ICC profiles is cleared and the underlying transformation and
    * all associated data structures are destroyed.
    */
   void Clear();

   /*!
    * Returns true iff the starting ICC profile in this transformation is a RGB
    * profile.
    */
   bool IsSourceRGBProfile() const
   {
      return m_srcRGB;
   }

   /*!
    * Returns true iff the target (last) ICC profile in this transformation is a
    * RGB profile.
    */
   bool IsTargetRGBProfile() const
   {
      return m_dstRGB;
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of
    * %ICCProfileTransformation.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %ICCProfileTransformation. If \a enable is
    *                false this parameter is ignored. A value <= 0 is ignored.
    *                The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of
    * %ICCProfileTransformation.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %ICCProfileTransformation.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %ICCProfileTransformation.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   /*
    * Handle to the profile transformation.
    */
   mutable transformation_handle m_transformation;

   /*
    * The ICC profiles and rendering intent that define this color
    * transformation.
    */
   profile_list     m_profiles;
   rendering_intent m_intent;
   rendering_intent m_proofingIntent;
   bool             m_blackPointCompensation : 1;
   bool             m_forceFloatingPoint     : 1;
   bool             m_highResolutionCLUT     : 1;
   bool             m_lowResolutionCLUT      : 1;
   bool             m_proofingTransformation : 1;
   bool             m_gamutCheck             : 1;
   bool             m_parallel               : 1;
   unsigned         m_maxProcessors          : PCL_MAX_PROCESSORS_BITCOUNT;

   /*
    * Flags indicating source and destination color spaces.
    * ### NB: These refer to the color spaces of the source and target pixel
    * buffers, *not* to the color spaces of the profiles involved in the
    * transformation.
    */
   mutable bool     m_srcRGB                 : 1;
   mutable bool     m_dstRGB                 : 1;

   /*
    * Flag indicating whether the current transformation (if any) has been
    * created forcing floating point arithmetics.
    */
   mutable bool     m_floatingPoint          : 1;

   void AddAt( size_type, const String& );
   void AddAt( size_type, const ICCProfile& );
   void AddAt( size_type, const ICCProfile::handle );
   void AddOrReplaceAt( size_type i, const ICCProfile::handle );

   void CreateTransformation( bool ) const;
   void CloseTransformation() const;

   // Inherited from ImageTransformation
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class ICCProofingTransformation
 * \brief A soft proofing ICC profile transformation
 *
 * %ICCProofingTransformation implements a <em>device proofing
 * transformation</em>. This kind of color transformation allows previewing of
 * the final results that would be obtained on a specific <em>proofing
 * device</em> without actually performing any rendition on physical media.
 *
 * A device proofing transformation uses three ICC profiles: a <em>source
 * profile</em>, a <em>proofing profile</em>, and a <em>target profile</em>. In
 * a typical scenario, the proofing device is a printer and we want to proof it
 * on a display monitor. In this common case we have:
 *
 * Source profile = The ICC profile describing the color space to which source
 * pixel values are referred.
 *
 * Proofing profile = The ICC profile describing the device where the final
 * result will be obtained (usually a printer).
 *
 * Target profile = The ICC profile describing the display device where we'll
 * preview the result (usually a monitor).
 *
 * \ingroup color_management
 * \sa ICCProfileTransformation, ICCProfile
 */
class PCL_CLASS ICCProofingTransformation : public ICCProfileTransformation
{
public:

   /*!
    * Constructs a default %ICCProofingTransformation object.
    */
   ICCProofingTransformation() :
      ICCProfileTransformation()
   {
      m_proofingTransformation = true;
      m_profiles.Add( static_cast<ICCProfile::handle>( nullptr ), 3 );
   }

   /*!
    * Move constructor.
    */
   ICCProofingTransformation( ICCProofingTransformation&& x ) :
      ICCProfileTransformation( std::move( x ) )
   {
   }

   /*!
    * Destroys an %ICCProofingTransformation object.
    */
   virtual ~ICCProofingTransformation()
   {
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ICCProofingTransformation& operator =( ICCProofingTransformation&& x )
   {
      (void)ICCProfileTransformation::operator =( std::move( x ) );
      return *this;
   }

   /*!
    * Loads an ICC profile from a file at \a profilePath and selects it as the
    * profile describing the source color space in this transformation.
    */
   void SetSourceProfile( const String& profilePath );

   /*!
    * Selects the specified ICC \a profile as the profile describing the source
    * color space in this transformation.
    */
   void SetSourceProfile( const ICCProfile& profile );

   /*!
    * Selects an already open ICC profile \a profileHandle as the profile
    * describing the source color space in this transformation.
    */
   void SetSourceProfile( const ICCProfile::handle profileHandle );

   /*!
    * Loads an ICC profile from a file at \a profilePath and selects it as the
    * profile describing the proofing device in this transformation.
    */
   void SetProofingProfile( const String& profilePath );

   /*!
    * Selects the specified ICC \a profile as the profile describing the
    * proofing device in this transformation.
    */
   void SetProofingProfile( const ICCProfile& profile );

   /*!
    * Selects an already open ICC profile \a profileHandle as the profile
    * describing the proofing device in this transformation.
    */
   void SetProofingProfile( const ICCProfile::handle profileHandle );

   /*!
    * Loads an ICC profile from a file at \a profilePath and selects it as the
    * profile describing the target (output) color space in this
    * transformation.
    */
   void SetTargetProfile( const String& profilePath );

   /*!
    * Selects the specified ICC \a profile as the profile describing the target
    * (output) color space in this transformation.
    */
   void SetTargetProfile( const ICCProfile& profile );

   /*!
    * Selects an already open ICC profile \a profileHandle as the profile
    * describing the target (output) color space in this transformation.
    */
   void SetTargetProfile( const ICCProfile::handle profileHandle );

   /*!
    * Returns the proofing rendering intent for this ICC proofing
    * transformation.
    */
   rendering_intent ProofingIntent() const
   {
      return m_intent;
   }

   /*!
    * Sets the proofing rendering intent for this ICC proofing transformation.
    *
    * The default proofing intent is ICCRenderingIntent::AbsoluteColorimetric.
    */
   void SetProofingIntent( rendering_intent i )
   {
      CloseTransformation();
      m_proofingIntent = i;
   }

   /*!
    * Returns true iff <em>gamut checking</em> has been enabled for this color
    * transformation. When gamut check is enabled, out-of-gamut colors in the
    * final rendition are automatically replaced by the current <em>gamut
    * warning color</em> (see the SetGamutWarningColor() member function).
    */
   bool IsGamutCheckEnabled() const
   {
      return m_gamutCheck;
   }

   /*!
    * Enables or disables <em>gamut check</em> for this color transformation.
    */
   void EnableGamutCheck( bool enable = true )
   {
      CloseTransformation();
      m_gamutCheck = enable;
   }

   /*!
    * Disables or enables <em>gamut check</em> for this color transformation.
    */
   void DisableGamutCheck( bool disable = true )
   {
      EnableGamutCheck( !disable );
   }

   /*!
    * Returns the current <em>gamut warning color</em> used to represent
    * out-of-gamut pixel values in final proofing renditions. The returned
    * value is a 32-bit RGBA pixel value.
    */
   static RGBA GamutWarningColor();

   /*!
    * Specifies the special RGB color used to represent out-of-gamut pixel
    * values in final proofing renditions. Only used for proofing
    * transformations with <em>gamut check</em> enabled.
    *
    * \param color   Gamut warning color encoded as a 32-bit RGBA pixel value.
    *
    * Note that since this is a static member function, the current gamut
    * warning color is a global setting that affects all proofing
    * transformations performed by the calling module.
    */
   static void SetGamutWarningColor( RGBA color );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ICCProfileTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/ICCProfileTransformation.h - Released 2017-05-02T10:38:59Z
