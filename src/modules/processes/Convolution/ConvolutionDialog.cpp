// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Convolution Process Module Version 01.01.03.0138
// ****************************************************************************
// ConvolutionDialog.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Convolution PixInsight module.
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

#include "ConvolutionDialog.h"
#include "FilterLibrary.h"

#include <pcl/Math.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{
   static int tableRow = 0;
   static String evenBgColor, oddBgColor;

   static void UpdateTableColors()
   {
      RGBA bg = TextBox().CanvasColor();
      double r = Red( bg )/255.0;
      double g = Green( bg )/255.0;
      double b = Blue( bg )/255.0;
      double h, s, v;
      RGBColorSystem::RGBToHSV( h, s, v, r, g, b );
      v *= .95;
      RGBColorSystem::HSVToRGB( r, g, b, h, s, v );
      evenBgColor = RGBColorToHexString( RGBAColor( float( r ), float( g ), float( b ) ) );
      v *= .92;
      RGBColorSystem::HSVToRGB( r, g, b, h, s, v );
      oddBgColor = RGBColorToHexString( RGBAColor( float( r ), float( g ), float( b ) ) );
   }

   static String TD( const String& data )
   {
      return "<td style=\"padding:0.2em;\"><p style=\"margin-left:0.3em; margin-right:0.3em;\">" + data + "</p></td>";
   }

   static String TR( const StringList& cols )
   {
      String tr( "<tr style=\"background:" + ((tableRow & 1) ? oddBgColor : evenBgColor) + "\">" );
      for ( StringList::const_iterator i = cols.Begin(); i != cols.End(); ++i )
         tr += TD( *i );
      tr += "</tr>";
      ++tableRow;
      return tr;
   }

   ConvolutionFilterCodeDialog::ConvolutionFilterCodeDialog( String mode, const Filter& _filter ) : Dialog(), filter( _filter )
   {
      // ### TextBox
      FilterCode_TextBox.SetMinSize( 700, 300 );
      FilterCode_TextBox.OnCaretPositionUpdated( (TextBox::caret_event_handler)&ConvolutionFilterCodeDialog::__TextBox_CaretPosition, *this );

      if ( mode == "Edit" )
      {
         FilterCode_TextBox.SetReadWrite();
         FilterCode_TextBox.SetText( filter.ToSource() );
      }

      tableRow = 0;
      String s;
      String html( "<html style=\"white-space:pre;text-align:right;\">"
                     "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">" );

      StringList cols;
      UpdateTableColors();

      if ( mode == "View" )
      {
         FilterCode_TextBox.SetReadOnly();
         if ( filter.IsSeparable() )
         {
            SeparableFilter fs( filter.Separable() );
            float element;

            // Rows
            for ( int i = 0; i < fs.Size(); ++i )
            {
               // Cols
               for ( int j = 0; j < fs.Size(); ++j )
               {
                  // Calculus of each coefficient of the separable filter
                  element = Round( fs.ColFilter()[i] * fs.RowFilter()[j], 6 );
                  cols.Add( String().Format( "%10.6f", element ) );


               }
               html += TR( cols );
               cols.Clear();
            }
         }
         else
         {
            KernelFilter k( filter.Kernel() );

            for ( int i = 0; i < k.Size(); ++i )
            {
               for ( int j = 0; j < k.Size(); ++j )
               {
                  float element = Round( k.Coefficients()[i][j], 6 );
                  // The elements of the KernelFilter can be accessed directly God thanks and the [] operator.
                  cols.Add( String().Format( "%10.6f", element ) );
               }
               html += TR( cols );
               cols.Clear();
            }
         }
         html += "</table></html>";
         // We got it!! we set the TextBox text by passing the composed String s.
         FilterCode_TextBox.SetText( html );
         UpdateTableColors();
      }

      if ( mode == "View" )
         LineCol_Label.Hide();
      else
         LineCol_Label.Show();

      LineCol_Sizer.SetSpacing( 4 );
      LineCol_Sizer.Add( LineCol_Label );
      LineCol_Sizer.AddStretch();

      // ### Save PushButton
      Save_PushButton.SetText( "Save" );
      Save_PushButton.SetCursor( StdCursor::Checkmark );
      Save_PushButton.OnClick( (Button::click_event_handler)&ConvolutionFilterCodeDialog::__Button_Click, *this );
      if ( mode == "View" )
         Save_PushButton.Hide();
      else
         Save_PushButton.SetDefault();

      if ( mode == "View" )
      {
         Cancel_PushButton.SetText( "Close" );
         Cancel_PushButton.SetCursor( StdCursor::Checkmark );
         Cancel_PushButton.SetDefault();
      }
      else
      {
         Cancel_PushButton.SetText( "Cancel" );
         Cancel_PushButton.SetCursor( StdCursor::Crossmark );
      }
      Cancel_PushButton.OnClick( (Button::click_event_handler)&ConvolutionFilterCodeDialog::__Button_Click, *this );

      DialogButtons_Sizer.SetSpacing( 8 );
      DialogButtons_Sizer.AddStretch();
      DialogButtons_Sizer.Add( Save_PushButton );
      DialogButtons_Sizer.Add( Cancel_PushButton );
      DialogButtons_Sizer.AddStretch();

      Global_Sizer.SetMargin( 4 );
      Global_Sizer.SetSpacing( 4 );
      Global_Sizer.Add( FilterCode_TextBox );
      Global_Sizer.Add( LineCol_Sizer );
      Global_Sizer.Add( DialogButtons_Sizer );

      SetSizer( Global_Sizer );
      EnableUserResizing();
      AdjustToContents();
      BringToFront();
      //SetFixedSize();

      if ( mode == "View" )
         SetWindowTitle( '\"' + filter.Name() + '\"' + " filter elements" );
      else if ( mode == "Edit" )
         SetWindowTitle( '\"' + filter.Name() + '\"' + " edit" );
      else if ( mode == "New" )
         SetWindowTitle( "New filter" );
      else
         throw Error( "Internal error: Invalid dialog operation mode in ConvolutionFilterCodeDialog" );
   }

   void ConvolutionFilterCodeDialog::__Button_Click( Button& sender, bool checked )
   {
      if ( sender == Cancel_PushButton )
      {
         Cancel();
      }
      else if ( sender == Save_PushButton )
      {
         try
         {
            IsoString sourceCode = FilterCode_TextBox.Text().ToUTF8().Trimmed();
            if ( sourceCode.IsEmpty() )
               throw Error( "Empty filter source code" );
            filter = Filter::FromSource( sourceCode );
            if ( !filter.IsValid() )
               throw Error( "invalid filter" );
            Ok();
            return;
         }
         ERROR_HANDLER
         FilterCode_TextBox.Focus();
      }
   }

   void ConvolutionFilterCodeDialog::__TextBox_CaretPosition( TextBox& sender, int oldPos, int newPos )
   {
      if ( sender == FilterCode_TextBox )
      {
         String t = FilterCode_TextBox.Text();

         line = 1;
         column = 1;

         for ( String::iterator i = t.Begin(); i < t.At( newPos ); i++ )
         {
            if( *i == '\n')
            {
               ++line;
               column = 0;
            }

            ++column;
         }

         LineCol_Label.SetText( String().Format( "Line: %d Col: %d", line , column ) );
      }
   }

   // ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ConvolutionDialog.cpp - Released 2014/10/29 07:34:55 UTC
