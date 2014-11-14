// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/ColorDialog.h - Released 2014/11/14 17:16:41 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_ColorDialog_h
#define __PCL_ColorDialog_h

/// \file pcl/ColorDialog.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Dialog_h
#include <pcl/Dialog.h>
#endif

#ifndef __PCL_NumericControl_h
#include <pcl/NumericControl.h>
#endif

#ifndef __PCL_PushButton_h
#include <pcl/PushButton.h>
#endif

#ifndef __PCL_Color_h
#include <pcl/Color.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### TODO: Include a complete color dialog box in the PCL.
 */

// ----------------------------------------------------------------------------

class PCL_INTERNAL SimpleColorDialogPrivate;

/*!
 * \class SimpleColorDialog
 * \brief A simple color selection dialog
 *
 * %SimpleColorDialog is a simplified color selection dialog box. This class is
 * suitable to implement quick definition of 32-bit RGBA color items as part of
 * a user preferences definition interface. It implements optional support for
 * an alpha (transparency) component. Color components are specified in the
 * eight-bit range [0,255].
 *
 * \sa Dialog
 */
class PCL_CLASS SimpleColorDialog : public Dialog
{
public:

   enum
   {
     AlphaEnabled = 0x0080,   //!< Enable definition of the alpha (transparency) component.
     Grayscale    = 0x0020    //!< Define a grayscale AARRGGBB color, where RR=GG=BB.
   };

   /*!
    * Constructs a %SimpleColorDialog object.
    *
    * \param color   Address of a 32-bit unsigned integer variable (a RGBA
    *                color value). If a valid pointer is specified, when the
    *                user accepts the dialog the selected color value will be
    *                written to the specified variable. If this parameter is a
    *                null pointer, no automatic write occurs.
    *
    * \param flags   A combination of flags that define the behavior of the
    *                color selection dialog. Currently the following flags are
    *                supported:\n
    *                \n
    *                SimpleColorDialog::AlphaEnabled: Enable definition of the
    *                alpha (transparency) component\n
    *                \n
    *                SimpleColorDialog::Grayscale: Define a grayscale AARRGGBB
    *                color, where RR=GG=BB.
    */
   SimpleColorDialog( RGBA* color = 0, unsigned flags = 0 );

   /*!
    * Destroys a %SimpleColorDialog object.
    */
   virtual ~SimpleColorDialog();

   /*!
    * Sets the current RGBA color value.
    *
    * This function can be called before executing the dialog to set an initial
    * color value, as an alternative to passing a non-null pointer to the
    * constructor.
    */
   void SetColor( RGBA color );

   /*!
    * Returns the RGBA color value that has been defined by the user.
    *
    * If a non-null pointer was passed to the constructor, this function
    * returns the same color value that will be (or has been) written to the
    * corresponding variable.
    */
   RGBA Color() const
   {
      return workingColor;
   }

   /*!
    * Returns true if this dialog allows definition of an alpha (transparency)
    * color component.
    */
   bool IsAlphaEnabled() const
   {
      return (flags & AlphaEnabled) != 0;
   }

   /*!
    * Returns true if this dialog defines a grayscale color, where the three
    * individual RGB components are equal.
    */
   bool IsGrayscale() const
   {
      return (flags & Grayscale) != 0;
   }

protected:

   RGBA*    color;
   RGBA     workingColor;
   unsigned flags;

   VerticalSizer  Global_Sizer;
      HorizontalSizer   Color_Sizer;
         VerticalSizer     Sliders_Sizer;
            NumericControl    V0_NumericControl;
            NumericControl    V1_NumericControl;
            NumericControl    V2_NumericControl;
            NumericControl    V3_NumericControl;
         Control           ColorSample_Control;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void __Color_ValueUpdated( NumericEdit& sender, double value );
   void __ColorSample_Paint( Control& sender, const Rect& updateRect );
   void __Done_Click( Button& sender, bool checked );
   void __Dialog_Return( Dialog& sender, int retVal );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ColorDialog_h

// ****************************************************************************
// EOF pcl/ColorDialog.h - Released 2014/11/14 17:16:41 UTC
