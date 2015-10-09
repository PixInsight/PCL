//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/Frame.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_Frame_h
#define __PCL_Frame_h

/// \file pcl/Frame.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Control_h
#include <pcl/Control.h>
#endif

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace FrameStyle
 * \brief Frame styles
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FrameStyle::Flat</td>   <td>No frame is drawn</td></tr>
 * <tr><td>FrameStyle::Box</td>    <td>Simple rectangular frame</td></tr>
 * <tr><td>FrameStyle::Raised</td> <td>Raised 3-D panel</td></tr>
 * <tr><td>FrameStyle::Sunken</td> <td>Sunken (lowered) 3-D panel</td></tr>
 * <tr><td>FrameStyle::Styled</td> <td>The appearance depends on the current platform and GUI style</td></tr>
 * </table>
 */
namespace FrameStyle
{
   enum value_type
   {
      Flat,    // No frame is drawn
      Box,     // Simple rectangular frame
      Raised,  // Raised 3-D panel
      Sunken,  // Sunken (lowered) 3-D panel
      Styled   // The appearance depends on the current platform and GUI style
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

// ----------------------------------------------------------------------------

/*!
 * \class Frame
 * \brief Client-side interface to a PixInsight %Frame control
 *
 * ### TODO: Write a detailed description for %Frame.
 */
class PCL_CLASS Frame : public Control
{
public:

   /*!
    * Represents a frame style.
    */
   typedef FrameStyle::value_type   style;

   /*!
    * Constructs a %Frame as a child control of \a parent.
    */
   Frame( Control& parent = Control::Null() );

   /*!
    * Destroys a %Frame object.
    */
   virtual ~Frame()
   {
   }

   /*!
    * Returns the current frame style.
    */
   style Style() const;

   /*!
    * Sets the frame style.
    */
   void SetStyle( style );

   /*!
    * Returns the current frame's line width in pixels.
    */
   int LineWidth() const;

   /*!
    * Sets the frame's line width in pixels.
    */
   void SetLineWidth( int );

   /*!
    * Returns the border width of this frame.
    *
    * The border width is the distance in pixels from the outer frame rectangle
    * to the inner client rectangle.
    */
   int BorderWidth() const;

   /*! #
    */
   int ScaledLineWidth() const
   {
      return PhysicalPixelsToLogical( LineWidth() );
   }

   /*! #
    */
   void SetScaledLineWidth( int width )
   {
      SetLineWidth( LogicalPixelsToPhysical( width ) );
   }

   /*! #
    */
   int ScaledBorderWidth() const
   {
      return PhysicalPixelsToLogical( BorderWidth() );
   }

protected:

   /*!
    * \internal
    */
   Frame( void* h ) : Control( h )
   {
   }
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Frame_h

// ----------------------------------------------------------------------------
// EOF pcl/Frame.h - Released 2015/10/08 11:24:12 UTC
