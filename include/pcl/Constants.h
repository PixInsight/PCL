//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/Constants.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_Constants_h
#define __PCL_Constants_h

/// \file pcl/Constants.h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Const
 * \brief Fundamental numeric constants
 *
 * Specialize this class to obtain constants of the appropriate floating-point
 * types. For example: Const<double>::pi()
 */
template <typename T>
class PCL_CLASS Const
{
public:

   /*!
    * pi/4 (45 degrees)
    */
   static constexpr T pi4       () { return T( 0.785398163397448309615660845819875725e+00L ); }

   /*!
    * pi/2 (90 degrees)
    */
   static constexpr T pi2       () { return T( 0.157079632679489661923132169163975145e+01L ); }

   /*!
    * 3*pi/4 (135 degrees)
    */
   static constexpr T _3pi4     () { return T( 0.2356194490192344928846982537459627175e+01L ); }

   /*!
    * The pi constant (180 degrees)
    */
   static constexpr T pi        () { return T( 0.31415926535897932384626433832795029e+01L ); }

   /*!
    * 5*pi/4 (225 degrees)
    */
   static constexpr T _5pi4     () { return T( 0.3926990816987241548078304229099378625e+01L ); }

   /*!
    * 3*pi/2 (270 degrees)
    */
   static constexpr T _3pi2     () { return T( 0.471238898038468985769396507491925435e+01L ); }

   /*!
    * 7*pi/4 (315 degrees)
    */
   static constexpr T _7pi4     () { return T( 0.5497787143782138167309625920739130075e+01L ); }

   /*!
    * 2*pi (360 degrees)
    */
   static constexpr T _2pi      () { return T( 0.62831853071795864769252867665590058e+01L ); }

   /*!
    * 1/pi
    */
   static constexpr T _1_pi     () { return T( 0.3183098861837906715377675267450287224677e+00L ); }

   /*!
    * 2/pi
    */
   static constexpr T _2_pi     () { return T( 0.6366197723675813430755350534900574449355e+00L ); }

   /*!
    * 2/sqrt( pi )
    */
   static constexpr T _2_sqrtpi () { return T( 0.11283791670955125738961589031215451688501e+01L ); }

   /*!
    * 180/pi - the radians-to-degrees conversion factor.
    */
   static constexpr T deg       () { return T( 0.572957795130823208767981548141051700441964e+02L ); }

   /*!
    * Pi/180 - the degrees-to-radians conversion factor.
    */
   static constexpr T rad       () { return T( 0.174532925199432957692369076848861272222e-01L ); }

   /*!
    * The Euler constant e.
    */
   static constexpr T e         () { return T( 0.27182818284590452353602874713526625e+01L ); }

   /*!
    * log2( e )
    */
   static constexpr T log2e     () { return T( 0.14426950408889634073599246810018920709799e+01L ); }

   /*!
    * log2( 10 )
    */
   static constexpr T log210    () { return T( 0.33219280948873623478703194294893900118996e+01L ); }

   /*!
    * log10( e )
    */
   static constexpr T log10e    () { return T( 0.4342944819032518276511289189166050837280e+00L ); }

   /*!
    * log10( 2 )
    */
   static constexpr T log102    () { return T( 0.3010299956639811952137388947244930416265e+00L ); }

   /*!
    * ln( 2 )
    */
   static constexpr T ln2       () { return T( 0.6931471805599453094172321214581766e+00L ); }

   /*!
    * ln( 10 )
    */
   static constexpr T ln10      () { return T( 0.23025850929940456840179914546843642e+01L ); }

   /*!
    * sqrt( 2 )
    */
   static constexpr T sqrt2     () { return T( 0.14142135623730950488016887242096980785696e+01L ); }

   /*!
    * 1/sqrt( 2 ) = sqrt( 2 )/2
    */
   static constexpr T _1_sqrt2  () { return T( 0.7071067811865475244008443621048490392848e+00L ); }

   /*!
    * sqrt( 2 )/2 = 1/sqrt( 2 )
    */
   static constexpr T sqrt2_2   () { return T( 0.7071067811865475244008443621048490392848e+00L ); }
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Constants_h

// ----------------------------------------------------------------------------
// EOF pcl/Constants.h - Released 2017-04-14T23:04:40Z
