//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0371
// ----------------------------------------------------------------------------
// StatisticsInterface.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "StatisticsInterface.h"
#include "StatisticsProcess.h"

#include <pcl/Dialog.h>
#include <pcl/GlobalSettings.h>
#include <pcl/GroupBox.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/PushButton.h>
#include <pcl/Settings.h>
#include <pcl/TextBox.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#include "StatisticsIcon.xpm"

// ----------------------------------------------------------------------------

StatisticsInterface* TheStatisticsInterface = nullptr;

// ----------------------------------------------------------------------------

static int BitsToComboBoxItemIndex( int bits )
{
   switch ( bits )
   {
   default:
   case  0: return 0;
   case  8: return 1;
   case 10: return 2;
   case 12: return 3;
   case 14: return 4;
   case 16: return 5;
   }
}

static int ComboBoxItemIndexToBits( int itemIndex )
{
   switch ( itemIndex )
   {
   default:
   case 0: return  0;
   case 1: return  8;
   case 2: return 10;
   case 3: return 12;
   case 4: return 14;
   case 5: return 16;
   }
}

static const char* s_comboBoxRangeItems[] =
{
   "Normalized Real [0,1]",
   " 8-bit [0,255]",
   "10-bit [0,1023]",
   "12-bit [0,4095]",
   "14-bit [0,16383]",
   "16-bit [0,65535]"
};

// ----------------------------------------------------------------------------

class StatisticsTextDialog : public Dialog
{
public:

   StatisticsTextDialog( const String& );

private:

   VerticalSizer     Global_Sizer;
      TextBox           Text_TextBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        Close_PushButton;

   void __Button_Click( Button& sender, bool checked );
};

StatisticsTextDialog::StatisticsTextDialog( const String& text ) : Dialog()
{
   StringList lines;
   text.Break( lines, '\n', true/*trim*/ );
   size_type widthCh = 0;
   for ( StringList::const_iterator i = lines.Begin(); i != lines.End(); ++i )
      if ( i->Length() > widthCh )
         widthCh = i->Length();

   Text_TextBox.SetStyleSheet( ScaledStyleSheet(
         "QWidget {"
            "font-family: DejaVu Sans Mono, Monospace;"
            "font-size: 10pt;"
         "}"
      ) );
   Text_TextBox.Restyle();
   Text_TextBox.SetMinSize( Text_TextBox.Font().Width( String( 'M', widthCh + 4 ) ), Text_TextBox.Font().Height()*(lines.Length() + 3) );
   Text_TextBox.SetText( text );
   //Text_TextBox.SetCaretPosition( 0 );

   Close_PushButton.SetText( "Close" );
   Close_PushButton.SetDefault();
   Close_PushButton.SetCursor( StdCursor::Checkmark );
   Close_PushButton.OnClick( (Button::click_event_handler)&StatisticsTextDialog::__Button_Click, *this );

   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Close_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( Text_TextBox, 100 );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();

   SetWindowTitle( "Statistics - Text View" );
}

void StatisticsTextDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == Close_PushButton )
      Ok();
}

// ----------------------------------------------------------------------------

class StatisticsOptionsDialog : public Dialog
{
public:

   StatisticsOptionsDialog();

private:

   int m_checkedCount = 0;

   VerticalSizer     Global_Sizer;
      GroupBox          Statistics_GroupBox;
      HorizontalSizer   Statistics_Sizer;
         VerticalSizer     StatisticsLeft_Sizer;
            CheckBox          Count_CheckBox;
            CheckBox          Mean_CheckBox;
            CheckBox          Modulus_CheckBox;
            CheckBox          Norm_CheckBox;
            CheckBox          SumOfSquares_CheckBox;
            CheckBox          MeanOfSquares_CheckBox;
            CheckBox          Median_CheckBox;
            CheckBox          Variance_CheckBox;
            CheckBox          StdDev_CheckBox;
            CheckBox          AvgDev_CheckBox;
         VerticalSizer     StatisticsRight_Sizer;
            CheckBox          MAD_CheckBox;
            CheckBox          BWMV_CheckBox;
            CheckBox          PBMV_CheckBox;
            CheckBox          Sn_CheckBox;
            CheckBox          Qn_CheckBox;
            CheckBox          Minimum_CheckBox;
            CheckBox          Maximum_CheckBox;
            CheckBox          MinimumPos_CheckBox;
            CheckBox          MaximumPos_CheckBox;
      HorizontalSizer   Buttons_Sizer;
         PushButton        SelectAll_PushButton;
         PushButton        UnselectAll_PushButton;
         PushButton        Reset_PushButton;
         PushButton        Save_PushButton;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void __Button_Click( Button& sender, bool checked );
   void __Dialog_Execute( Dialog& sender );
   void __Dialog_Return( Dialog& sender, int retVal );
};

StatisticsOptionsDialog::StatisticsOptionsDialog()
{
   Count_CheckBox.SetText( "Count" );
   Count_CheckBox.SetToolTip( "<p>Total number of pixel samples within the estimation range ]0,1[ (clipped) or [0,1] (unclipped).</p>" );
   Count_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Mean_CheckBox.SetText( "Mean" );
   Mean_CheckBox.SetToolTip( "<p>Arithmetic mean.</p>" );
   Mean_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Modulus_CheckBox.SetText( "Modulus" );
   Modulus_CheckBox.SetToolTip( "<p>Sum of absolute sample values.</p>" );
   Modulus_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Norm_CheckBox.SetText( "Norm" );
   Norm_CheckBox.SetToolTip( "<p>Sum of sample values.</p>" );
   Norm_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   SumOfSquares_CheckBox.SetText( "Sum of squares" );
   SumOfSquares_CheckBox.SetToolTip( "<p>Sum of the squares of all sample values.</p>" );
   SumOfSquares_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   MeanOfSquares_CheckBox.SetText( "Mean of squares" );
   MeanOfSquares_CheckBox.SetToolTip( "<p>Mean of the squares of all sample values.</p>" );
   MeanOfSquares_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Median_CheckBox.SetText( "Median" );
   Median_CheckBox.SetToolTip( "<p>Median</p>" );
   Median_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Variance_CheckBox.SetText( "Variance" );
   Variance_CheckBox.SetToolTip( "<p>Variance from the mean.</p>" );
   Variance_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   StdDev_CheckBox.SetText( "Standard deviation" );
   StdDev_CheckBox.SetToolTip( "<p>Standard deviation from the mean.</p>" );
   StdDev_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   AvgDev_CheckBox.SetText( "Average absolute deviation" );
   AvgDev_CheckBox.SetToolTip( "<p>Average absolute deviation from the median.</p>" );
   AvgDev_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   MAD_CheckBox.SetText( "Median absolute deviation (MAD)" );
   MAD_CheckBox.SetToolTip( "<p>Median absolute deviation from the median.</p>" );
   MAD_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   BWMV_CheckBox.SetText( "Biweight midvariance (BWMV)" );
   BWMV_CheckBox.SetToolTip( "<p>Square root of the biweight midvariance.</p>" );
   BWMV_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   PBMV_CheckBox.SetText( "Percentage bend midvariance (PBMV)" );
   PBMV_CheckBox.SetToolTip( "<p>Square root of the percentage bend midvariance.</p>" );
   PBMV_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Sn_CheckBox.SetText( "Sn" );
   Sn_CheckBox.SetToolTip( "<p>Sn scale estimator of Rousseeuw and Croux.</p>" );
   Sn_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Qn_CheckBox.SetText( "Qn" );
   Qn_CheckBox.SetToolTip( "<p>Qn scale estimator of Rousseeuw and Croux</p>" );
   Qn_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Minimum_CheckBox.SetText( "Minimum" );
   Minimum_CheckBox.SetToolTip( "<p>Minimum sample value.</p>" );
   Minimum_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Maximum_CheckBox.SetText( "Maximum" );
   Maximum_CheckBox.SetToolTip( "<p>Maximum sample value.</p>" );
   Maximum_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   MinimumPos_CheckBox.SetText( "Minimum position" );
   MinimumPos_CheckBox.SetToolTip( "<p>Image coordinates of the first occurrence of the minimum sample value.</p>" );
   MinimumPos_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   MaximumPos_CheckBox.SetText( "Maximum position" );
   MaximumPos_CheckBox.SetToolTip( "<p>Image coordinates of the first occurrence of the maximum sample value.</p>" );
   MaximumPos_CheckBox.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   StatisticsLeft_Sizer.SetSpacing( 6 );
   StatisticsLeft_Sizer.Add( Count_CheckBox );
   StatisticsLeft_Sizer.Add( Mean_CheckBox );
   StatisticsLeft_Sizer.Add( Modulus_CheckBox );
   StatisticsLeft_Sizer.Add( Norm_CheckBox );
   StatisticsLeft_Sizer.Add( SumOfSquares_CheckBox );
   StatisticsLeft_Sizer.Add( MeanOfSquares_CheckBox );
   StatisticsLeft_Sizer.Add( Median_CheckBox );
   StatisticsLeft_Sizer.Add( Variance_CheckBox );
   StatisticsLeft_Sizer.Add( StdDev_CheckBox );
   StatisticsLeft_Sizer.Add( AvgDev_CheckBox );
   StatisticsLeft_Sizer.AddStretch();

   StatisticsRight_Sizer.SetSpacing( 6 );
   StatisticsRight_Sizer.Add( MAD_CheckBox );
   StatisticsRight_Sizer.Add( BWMV_CheckBox );
   StatisticsRight_Sizer.Add( PBMV_CheckBox );
   StatisticsRight_Sizer.Add( Sn_CheckBox );
   StatisticsRight_Sizer.Add( Qn_CheckBox );
   StatisticsRight_Sizer.Add( Minimum_CheckBox );
   StatisticsRight_Sizer.Add( Maximum_CheckBox );
   StatisticsRight_Sizer.Add( MinimumPos_CheckBox );
   StatisticsRight_Sizer.Add( MaximumPos_CheckBox );
   StatisticsRight_Sizer.AddStretch();

   Statistics_Sizer.SetMargin( 8 );
   Statistics_Sizer.SetSpacing( 8 );
   Statistics_Sizer.Add( StatisticsLeft_Sizer );
   Statistics_Sizer.Add( StatisticsRight_Sizer );

   Statistics_GroupBox.SetTitle( "Available Statistics" );
   Statistics_GroupBox.SetSizer( Statistics_Sizer );

   SelectAll_PushButton.SetText( "Select All" );
   SelectAll_PushButton.SetToolTip( "<p>Select all available statistics.</p>" );
   SelectAll_PushButton.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   UnselectAll_PushButton.SetText( "Unselect All" );
   UnselectAll_PushButton.SetToolTip( "<p>Unselect all statistics.</p>" );
   UnselectAll_PushButton.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Reset_PushButton.SetText( "Reset" );
   Reset_PushButton.SetToolTip( "<p>Select a factory-default set of statistics.</p>" );
   Reset_PushButton.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Save_PushButton.SetText( "Save" );
   Save_PushButton.SetToolTip( "<p>Save the current selection as default.</p>" );
   Save_PushButton.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&StatisticsOptionsDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( SelectAll_PushButton );
   Buttons_Sizer.Add( UnselectAll_PushButton );
   Buttons_Sizer.Add( Reset_PushButton );
   Buttons_Sizer.Add( Save_PushButton );
   Buttons_Sizer.AddSpacing( 24 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Statistics_GroupBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Statistics Options" );

   OnExecute( (Dialog::execute_event_handler)&StatisticsOptionsDialog::__Dialog_Execute, *this );
   OnReturn( (Dialog::return_event_handler)&StatisticsOptionsDialog::__Dialog_Return, *this );
}

void StatisticsOptionsDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == SelectAll_PushButton )
   {
      m_checkedCount = 0;
      ++m_checkedCount, Count_CheckBox.Check();
      ++m_checkedCount, Mean_CheckBox.Check();
      ++m_checkedCount, Modulus_CheckBox.Check();
      ++m_checkedCount, Norm_CheckBox.Check();
      ++m_checkedCount, SumOfSquares_CheckBox.Check();
      ++m_checkedCount, MeanOfSquares_CheckBox.Check();
      ++m_checkedCount, Median_CheckBox.Check();
      ++m_checkedCount, Variance_CheckBox.Check();
      ++m_checkedCount, StdDev_CheckBox.Check();
      ++m_checkedCount, AvgDev_CheckBox.Check();
      ++m_checkedCount, MAD_CheckBox.Check();
      ++m_checkedCount, BWMV_CheckBox.Check();
      ++m_checkedCount, PBMV_CheckBox.Check();
      ++m_checkedCount, Sn_CheckBox.Check();
      ++m_checkedCount, Qn_CheckBox.Check();
      ++m_checkedCount, Minimum_CheckBox.Check();
      ++m_checkedCount, Maximum_CheckBox.Check();
      ++m_checkedCount, MinimumPos_CheckBox.Check();
      ++m_checkedCount, MaximumPos_CheckBox.Check();
   }
   else if ( sender == UnselectAll_PushButton )
   {
      m_checkedCount = 0;
                        Count_CheckBox.Uncheck();
                        Mean_CheckBox.Uncheck();
                        Modulus_CheckBox.Uncheck();
                        Norm_CheckBox.Uncheck();
                        SumOfSquares_CheckBox.Uncheck();
                        MeanOfSquares_CheckBox.Uncheck();
                        Median_CheckBox.Uncheck();
                        Variance_CheckBox.Uncheck();
                        StdDev_CheckBox.Uncheck();
                        AvgDev_CheckBox.Uncheck();
                        MAD_CheckBox.Uncheck();
                        BWMV_CheckBox.Uncheck();
                        PBMV_CheckBox.Uncheck();
                        Sn_CheckBox.Uncheck();
                        Qn_CheckBox.Uncheck();
                        Minimum_CheckBox.Uncheck();
                        Maximum_CheckBox.Uncheck();
                        MinimumPos_CheckBox.Uncheck();
                        MaximumPos_CheckBox.Uncheck();
   }
   else if ( sender == Reset_PushButton )
   {
      m_checkedCount = 0;
      ++m_checkedCount, Count_CheckBox.Check();
      ++m_checkedCount, Mean_CheckBox.Check();
                        Modulus_CheckBox.Uncheck();
                        Norm_CheckBox.Uncheck();
                        SumOfSquares_CheckBox.Uncheck();
                        MeanOfSquares_CheckBox.Uncheck();
      ++m_checkedCount, Median_CheckBox.Check();
                        Variance_CheckBox.Uncheck();
                        StdDev_CheckBox.Uncheck();
      ++m_checkedCount, AvgDev_CheckBox.Check();
      ++m_checkedCount, MAD_CheckBox.Check();
                        BWMV_CheckBox.Uncheck();
                        PBMV_CheckBox.Uncheck();
                        Sn_CheckBox.Uncheck();
                        Qn_CheckBox.Uncheck();
      ++m_checkedCount, Minimum_CheckBox.Check();
      ++m_checkedCount, Maximum_CheckBox.Check();
                        MinimumPos_CheckBox.Uncheck();
                        MaximumPos_CheckBox.Uncheck();
   }
   else if ( sender == Save_PushButton )
   {
      if ( m_checkedCount > 0 )
      {
         StringList items;
         if ( Count_CheckBox.IsChecked() )          items.Append( "Count" );
         if ( Mean_CheckBox.IsChecked() )           items.Append( "Mean" );
         if ( Modulus_CheckBox.IsChecked() )        items.Append( "Modulus" );
         if ( Norm_CheckBox.IsChecked() )           items.Append( "Norm" );
         if ( SumOfSquares_CheckBox.IsChecked() )   items.Append( "SumOfSquares" );
         if ( MeanOfSquares_CheckBox.IsChecked() )  items.Append( "MeanOfSquares" );
         if ( Median_CheckBox.IsChecked() )         items.Append( "Median" );
         if ( Variance_CheckBox.IsChecked() )       items.Append( "Variance" );
         if ( StdDev_CheckBox.IsChecked() )         items.Append( "StdDev" );
         if ( AvgDev_CheckBox.IsChecked() )         items.Append( "AvgDev" );
         if ( MAD_CheckBox.IsChecked() )            items.Append( "MAD" );
         if ( BWMV_CheckBox.IsChecked() )           items.Append( "BWMV" );
         if ( PBMV_CheckBox.IsChecked() )           items.Append( "PBMV" );
         if ( Sn_CheckBox.IsChecked() )             items.Append( "Sn" );
         if ( Qn_CheckBox.IsChecked() )             items.Append( "Qn" );
         if ( Minimum_CheckBox.IsChecked() )        items.Append( "Minimum" );
         if ( Maximum_CheckBox.IsChecked() )        items.Append( "Maximum" );
         if ( MinimumPos_CheckBox.IsChecked() )     items.Append( "MinimumPos" );
         if ( MaximumPos_CheckBox.IsChecked() )     items.Append( "MaximumPos" );

         String contents = String().ToCommaSeparated( items );
         Settings::Write( TheStatisticsInterface->SettingsKey() + "Contents", contents );

         MessageBox( "<p>Default statistics set saved:</p>"
                     "<p>" + contents + "</p>",
                     "Statistics",
                     StdIcon::Information,
                     StdButton::Ok ).Execute();
      }
   }
   else if ( sender == OK_PushButton )
   {
      if ( m_checkedCount > 0 )
         Ok();
   }
   else if ( sender == Cancel_PushButton )
   {
      Cancel();
   }
   else
   {
      m_checkedCount += checked ? +1 : -1;
   }

   OK_PushButton.Enable( m_checkedCount > 0 );
   Save_PushButton.Enable( m_checkedCount > 0 );
}

void StatisticsOptionsDialog::__Dialog_Execute( Dialog& sender )
{
   m_checkedCount = 0;
   if ( TheStatisticsInterface->m_doCount         ) ++m_checkedCount, Count_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMean          ) ++m_checkedCount, Mean_CheckBox.Check();
   if ( TheStatisticsInterface->m_doModulus       ) ++m_checkedCount, Modulus_CheckBox.Check();
   if ( TheStatisticsInterface->m_doNorm          ) ++m_checkedCount, Norm_CheckBox.Check();
   if ( TheStatisticsInterface->m_doSumOfSquares  ) ++m_checkedCount, SumOfSquares_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMeanOfSquares ) ++m_checkedCount, MeanOfSquares_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMedian        ) ++m_checkedCount, Median_CheckBox.Check();
   if ( TheStatisticsInterface->m_doVariance      ) ++m_checkedCount, Variance_CheckBox.Check();
   if ( TheStatisticsInterface->m_doStdDev        ) ++m_checkedCount, StdDev_CheckBox.Check();
   if ( TheStatisticsInterface->m_doAvgDev        ) ++m_checkedCount, AvgDev_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMAD           ) ++m_checkedCount, MAD_CheckBox.Check();
   if ( TheStatisticsInterface->m_doBWMV          ) ++m_checkedCount, BWMV_CheckBox.Check();
   if ( TheStatisticsInterface->m_doPBMV          ) ++m_checkedCount, PBMV_CheckBox.Check();
   if ( TheStatisticsInterface->m_doSn            ) ++m_checkedCount, Sn_CheckBox.Check();
   if ( TheStatisticsInterface->m_doQn            ) ++m_checkedCount, Qn_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMinimum       ) ++m_checkedCount, Minimum_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMaximum       ) ++m_checkedCount, Maximum_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMinimumPos    ) ++m_checkedCount, MinimumPos_CheckBox.Check();
   if ( TheStatisticsInterface->m_doMaximumPos    ) ++m_checkedCount, MaximumPos_CheckBox.Check();

   OK_PushButton.Enable( m_checkedCount > 0 );
   Save_PushButton.Enable( m_checkedCount > 0 );
}

void StatisticsOptionsDialog::__Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      if ( m_checkedCount <= 0 )
      {
         MessageBox( "No statistics have been selected - current set not modified.",
                     "Statistics",
                     StdIcon::Error,
                     StdButton::Ok ).Execute();
         return;
      }

      TheStatisticsInterface->m_doCount         = Count_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMean          = Mean_CheckBox.IsChecked();
      TheStatisticsInterface->m_doModulus       = Modulus_CheckBox.IsChecked();
      TheStatisticsInterface->m_doNorm          = Norm_CheckBox.IsChecked();
      TheStatisticsInterface->m_doSumOfSquares  = SumOfSquares_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMeanOfSquares = MeanOfSquares_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMedian        = Median_CheckBox.IsChecked();
      TheStatisticsInterface->m_doVariance      = Variance_CheckBox.IsChecked();
      TheStatisticsInterface->m_doStdDev        = StdDev_CheckBox.IsChecked();
      TheStatisticsInterface->m_doAvgDev        = AvgDev_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMAD           = MAD_CheckBox.IsChecked();
      TheStatisticsInterface->m_doBWMV          = BWMV_CheckBox.IsChecked();
      TheStatisticsInterface->m_doPBMV          = PBMV_CheckBox.IsChecked();
      TheStatisticsInterface->m_doSn            = Sn_CheckBox.IsChecked();
      TheStatisticsInterface->m_doQn            = Qn_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMinimum       = Minimum_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMaximum       = Maximum_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMinimumPos    = MinimumPos_CheckBox.IsChecked();
      TheStatisticsInterface->m_doMaximumPos    = MaximumPos_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

#define m_currentView   GUI->AllViews_ViewList.CurrentView()

// ----------------------------------------------------------------------------

StatisticsInterface::StatisticsInterface() :
   m_doCount( true ),
   m_doMean( true ),
   m_doModulus( false ),
   m_doNorm( false ),
   m_doSumOfSquares( false ),
   m_doMeanOfSquares( false ),
   m_doMedian( true ),
   m_doVariance( false ),
   m_doStdDev( false ),
   m_doAvgDev( true ),
   m_doMAD( true ),
   m_doBWMV( false ),
   m_doPBMV( false ),
   m_doSn( false ),
   m_doQn( false ),
   m_doMinimum( true ),
   m_doMinimumPos( false ),
   m_doMaximum( true ),
   m_doMaximumPos( false ),
   m_rangeBits( 0 )
{
   TheStatisticsInterface = this;
}

StatisticsInterface::~StatisticsInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString StatisticsInterface::Id() const
{
   return "Statistics";
}

MetaProcess* StatisticsInterface::Process() const
{
   return TheStatisticsProcess;
}

const char** StatisticsInterface::IconImageXPM() const
{
   return StatisticsIcon_XPM;
}

InterfaceFeatures StatisticsInterface::Features() const
{
   return InterfaceFeature::TrackViewButton;
}

void StatisticsInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.CurrentView() );
         else
            UpdateControls();
      }
}

bool StatisticsInterface::Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Statistics" );
      GUI->AllViews_ViewList.Regenerate( true, true, true );
      UpdateControls();
   }

   dynamic = false;
   return true;
}

bool StatisticsInterface::IsInstanceGenerator() const
{
   return false;
}

bool StatisticsInterface::CanImportInstances() const
{
   return false;
}

bool StatisticsInterface::WantsImageNotifications() const
{
   return true;
}

void StatisticsInterface::ImageUpdated( const View& view )
{
   if ( GUI != nullptr )
      if ( view == m_currentView )
         UpdateControls();
}

void StatisticsInterface::ImageFocused( const View& view )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
      {
         GUI->AllViews_ViewList.SelectView( view ); // normally not necessary, but we can invoke this f() directly
         UpdateControls();
      }
}

void StatisticsInterface::ImageDeleted( const View& view )
{
   if ( GUI != nullptr )
      if ( view == m_currentView )
         UpdateControls();
}

bool StatisticsInterface::WantsViewPropertyNotifications() const
{
   return true;
}

void StatisticsInterface::ViewPropertyDeleted( const View& view, const IsoString& property )
{
   if ( GUI != nullptr )
      if ( view == m_currentView )
         if ( ViewPropertyRequired( property ) )
         {
            DeactivateTrackView();
            GUI->AllViews_ViewList.SelectView( View::Null() );
            UpdateControls();
         }
}

void StatisticsInterface::SaveSettings() const
{
   IsoString key = SettingsKey();

   Settings::Write( key + "ScientificNotation", GUI->Scientific_CheckBox.IsChecked() );
   Settings::Write( key + "NormalizedScale", GUI->Normalized_CheckBox.IsChecked() );
   Settings::Write( key + "Unclipped", GUI->Unclipped_CheckBox.IsChecked() );
   Settings::Write( key + "Range", m_rangeBits );
   // NB: The Contents settings key is written by the Statistics Options dialog.
}

void StatisticsInterface::LoadSettings()
{
   IsoString key = SettingsKey();

   bool b = true;
   Settings::Read( key + "ScientificNotation", b );
   GUI->Scientific_CheckBox.SetChecked( b );
   b = true;
   Settings::Read( key + "NormalizedScale", b );
   GUI->Normalized_CheckBox.SetChecked( b );
   b = false;
   Settings::Read( key + "Unclipped", b );
   GUI->Unclipped_CheckBox.SetChecked( b );

   Settings::Read( key + "Range", m_rangeBits );

   String contents;
   Settings::Read( key + "Contents", contents );
   StringList items;
   contents.Break( items, ',', true/*trim*/ );
   if ( !contents.IsEmpty() )
   {
      m_doCount         =
      m_doMean          =
      m_doModulus       =
      m_doNorm          =
      m_doSumOfSquares  =
      m_doMeanOfSquares =
      m_doMedian        =
      m_doVariance      =
      m_doStdDev        =
      m_doAvgDev        =
      m_doMAD           =
      m_doBWMV          =
      m_doPBMV          =
      m_doSn            =
      m_doQn            =
      m_doMinimum       =
      m_doMaximum       =
      m_doMinimumPos    =
      m_doMaximumPos    = false;

      int itemCount = 0;
      for ( StringList::const_iterator i = items.Begin(); i != items.End(); ++i )
      {
         ++itemCount;
              if ( *i == "Count" )         m_doCount         = true;
         else if ( *i == "Mean" )          m_doMean          = true;
         else if ( *i == "Modulus" )       m_doModulus       = true;
         else if ( *i == "Norm" )          m_doNorm          = true;
         else if ( *i == "SumOfSquares" )  m_doSumOfSquares  = true;
         else if ( *i == "MeanOfSquares" ) m_doMeanOfSquares = true;
         else if ( *i == "Median" )        m_doMedian        = true;
         else if ( *i == "Variance" )      m_doVariance      = true;
         else if ( *i == "StdDev" )        m_doStdDev        = true;
         else if ( *i == "AvgDev" )        m_doAvgDev        = true;
         else if ( *i == "MAD" )           m_doMAD           = true;
         else if ( *i == "BWMV" )          m_doBWMV          = true;
         else if ( *i == "PBMV" )          m_doPBMV          = true;
         else if ( *i == "Sn" )            m_doSn            = true;
         else if ( *i == "Qn" )            m_doQn            = true;
         else if ( *i == "Minimum" )       m_doMinimum       = true;
         else if ( *i == "Maximum" )       m_doMaximum       = true;
         else if ( *i == "MinimumPos" )    m_doMinimumPos    = true;
         else if ( *i == "MaximumPos" )    m_doMaximumPos    = true;
         else
            --itemCount;
      }

      if ( itemCount == 0 )
         m_doCount   =
         m_doMean    =
         m_doMedian  =
         m_doAvgDev  =
         m_doMAD     =
         m_doMinimum =
         m_doMaximum = true;
   }

   UpdateControls();
}

// ----------------------------------------------------------------------------

bool StatisticsInterface::ViewPropertyRequired( const IsoString& property ) const
{
   if ( property == "*" )
      return true;

   if ( GUI->Unclipped_CheckBox.IsChecked() )
      return m_doCount         && property == "UCCount"         ||
             m_doMean          && property == "UCMean"          ||
             m_doModulus       && property == "UCModulus"       ||
             m_doNorm          && property == "UCNorm"          ||
             m_doSumOfSquares  && property == "UCSumOfSquares"  ||
             m_doMeanOfSquares && property == "UCMeanOfSquares" ||
             m_doMedian        && property == "UCMedian"        ||
             m_doVariance      && property == "UCVariance"      ||
             m_doStdDev        && property == "UCStdDev"        ||
             m_doAvgDev        && property == "UCAvgDev"        ||
             m_doMAD           && property == "UCMAD"           ||
             m_doBWMV          && property == "UCBWMV"          ||
             m_doPBMV          && property == "UCPBMV"          ||
             m_doSn            && property == "UCSn"            ||
             m_doQn            && property == "UCQn"            ||
             m_doMinimum       && property == "UCMinimum"       ||
             m_doMinimumPos    && property == "UCMinimumPos"    ||
             m_doMaximum       && property == "UCMaximum"       ||
             m_doMaximumPos    && property == "UCMaximumPos";

   return m_doCount         && property == "Count"         ||
          m_doMean          && property == "Mean"          ||
          m_doModulus       && property == "Modulus"       ||
          m_doNorm          && property == "Norm"          ||
          m_doSumOfSquares  && property == "SumOfSquares"  ||
          m_doMeanOfSquares && property == "MeanOfSquares" ||
          m_doMedian        && property == "Median"        ||
          m_doVariance      && property == "Variance"      ||
          m_doStdDev        && property == "StdDev"        ||
          m_doAvgDev        && property == "AvgDev"        ||
          m_doMAD           && property == "MAD"           ||
          m_doBWMV          && property == "BWMV"          ||
          m_doPBMV          && property == "PBMV"          ||
          m_doSn            && property == "Sn"            ||
          m_doQn            && property == "Qn"            ||
          m_doMinimum       && property == "Minimum"       ||
          m_doMinimumPos    && property == "MinimumPos"    ||
          m_doMaximum       && property == "Maximum"       ||
          m_doMaximumPos    && property == "MaximumPos";
}

void StatisticsInterface::CalculateStatistics()
{
   if ( m_currentView.IsNull() )
      return;

   bool u = GUI->Unclipped_CheckBox.IsChecked();

   if ( m_doCount )
      m_count = m_currentView.ComputeOrFetchProperty( u ? "UCCount" : "Count", false/*notify*/ ).ToUI64Vector();
   if ( m_doMean )
      m_mean = m_currentView.ComputeOrFetchProperty( u ? "UCMean" : "Mean", false/*notify*/ ).ToDVector();
   if ( m_doModulus )
      m_modulus = m_currentView.ComputeOrFetchProperty( u ? "UCModulus" : "Modulus", false/*notify*/ ).ToDVector();
   if ( m_doNorm )
      m_norm = m_currentView.ComputeOrFetchProperty( u ? "UCNorm" : "Norm", false/*notify*/ ).ToDVector();
   if ( m_doSumOfSquares )
      m_sumOfSquares = m_currentView.ComputeOrFetchProperty( u ? "UCSumOfSquares" : "SumOfSquares", false/*notify*/ ).ToDVector();
   if ( m_doMeanOfSquares )
      m_meanOfSquares = m_currentView.ComputeOrFetchProperty( u ? "UCMeanOfSquares" : "MeanOfSquares", false/*notify*/ ).ToDVector();
   if ( m_doMedian )
      m_median = m_currentView.ComputeOrFetchProperty( u ? "UCMedian" : "Median", false/*notify*/ ).ToDVector();
   if ( m_doVariance )
      m_variance = m_currentView.ComputeOrFetchProperty( u ? "UCVariance" : "Variance", false/*notify*/ ).ToDVector();
   if ( m_doStdDev )
      m_stdDev = m_currentView.ComputeOrFetchProperty( u ? "UCStdDev" : "StdDev", false/*notify*/ ).ToDVector();
   if ( m_doAvgDev )
      m_avgDev = m_currentView.ComputeOrFetchProperty( u ? "UCAvgDev" : "AvgDev", false/*notify*/ ).ToDVector();
   if ( m_doMAD )
      m_MAD = m_currentView.ComputeOrFetchProperty( u ? "UCMAD" : "MAD", false/*notify*/ ).ToDVector();
   if ( m_doBWMV )
      m_BWMV = m_currentView.ComputeOrFetchProperty( u ? "UCBWMV" : "BWMV", false/*notify*/ ).ToDVector().Sqrt();
   if ( m_doPBMV )
      m_PBMV = m_currentView.ComputeOrFetchProperty( u ? "UCPBMV" : "PBMV", false/*notify*/ ).ToDVector().Sqrt();
   if ( m_doSn )
      m_Sn = m_currentView.ComputeOrFetchProperty( u ? "UCSn" : "Sn", false/*notify*/ ).ToDVector();
   if ( m_doQn )
      m_Qn = m_currentView.ComputeOrFetchProperty( u ? "UCQn" : "Qn", false/*notify*/ ).ToDVector();
   if ( m_doMinimum )
      m_minimum = m_currentView.ComputeOrFetchProperty( u ? "UCMinimum" : "Minimum", false/*notify*/ ).ToDVector();
   if ( m_doMaximum )
      m_maximum = m_currentView.ComputeOrFetchProperty( u ? "UCMaximum" : "Maximum", false/*notify*/ ).ToDVector();
   if ( m_doMinimumPos )
      m_minimumPos = m_currentView.ComputeOrFetchProperty( u ? "UCMinimumPos" : "MinimumPos", false/*notify*/ ).ToIMatrix();
   if ( m_doMaximumPos )
      m_maximumPos = m_currentView.ComputeOrFetchProperty( u ? "UCMaximumPos" : "MaximumPos", false/*notify*/ ).ToIMatrix();
}

// ----------------------------------------------------------------------------

class StatisticsTreeNodeBase : public TreeBox::Node
{
public:

   StatisticsTreeNodeBase( TreeBox& parent, const String& property ) : TreeBox::Node()
   {
      parent.Add( this );

      SetText( 0, property );

      pcl::Font font( PixInsightSettings::GlobalFont( "ConsoleWindow/Font" ) );
      for ( int i = 0, n = parent.NumberOfColumns(); i < n; ++i )
      {
         SetFont( i, font );
         SetAlignment( i, TextAlign::Left|TextAlign::VertCenter );
      }

      font.SetBold( true );
      SetFont( 0, font );
   }
};

class StatisticsTreeNode : public StatisticsTreeNodeBase
{
public:

   StatisticsTreeNode( TreeBox& parent, const String& property,
                       const DVector& data, const char* format, int precision, double scale ) : StatisticsTreeNodeBase( parent, property )
   {
      for ( int i = 0; i < data.Length(); ++i )
         SetText( i+1, String().Format( format, precision, scale*data[i] ) );
   }

   StatisticsTreeNode( TreeBox& parent, const String& property,
                       const UI64Vector& data, uint64 total ) : StatisticsTreeNodeBase( parent, property )
   {
      for ( int i = 0; i < data.Length(); ++i )
         SetText( i+1, String().Format( "%.5f", 100.0*data[i]/total ) );
   }

   StatisticsTreeNode( TreeBox& parent, const String& property,
                       const UI64Vector& data ) : StatisticsTreeNodeBase( parent, property )
   {
      for ( int i = 0; i < data.Length(); ++i )
         SetText( i+1, String().Format( "%llu", data[i] ) );
   }

   StatisticsTreeNode( TreeBox& parent, const String& property,
                       const IMatrix& data ) : StatisticsTreeNodeBase( parent, property )
   {
      for ( int i = 0; i < data.Rows(); ++i )
         SetText( i+1, String().Format( "%6d,%6d", data[i][0], data[i][1] ) );
   }
};

// ----------------------------------------------------------------------------

void StatisticsInterface::UpdateControls()
{
   GUI->Scientific_CheckBox.Enable( m_rangeBits == 0 );

   GUI->Range_ComboBox.SetCurrentItem( BitsToComboBoxItemIndex( m_rangeBits ) );

   ImageVariant image;
   if ( !m_currentView.IsNull() )
      image = m_currentView.Image();

   /*
    * ### N.B. m_currentView.Image() can be invalid if the current view is
    *     virtual (e.g., RealTimePreview).
    */
   if ( !image )
   {
      GUI->Data_TreeBox.Clear();
      GUI->Data_TreeBox.HideHeader();
      return;
   }

   CalculateStatistics();

   int imageBits = image.BitsPerSample();
   int precision;
   const char* realFormat = "%.*f";
   double scale;

   switch ( m_rangeBits )
   {
   default: // ?!
   case 0:
      switch ( imageBits )
      {
      case 8: precision = 3; break;
      case 16: precision = 5; break;
      default: // ?!
      case 32: precision = image.IsFloatSample() ? 7 : 10; break;
      case 64: precision = 15; break;
      }
      if ( GUI->Scientific_CheckBox.IsChecked() )
      {
         --precision;
         realFormat = "%.*e";
      }
      scale = 1.0;
      break;

   case 8:
      switch ( imageBits )
      {
      case 8: precision = 1; break;
      case 16: precision = 3; break;
      default: // ?!
      case 32: precision = image.IsFloatSample() ? 5 : 8; break;
      case 64: precision = 13; break;
      }
      scale = 255;
      break;

   case 10:
      switch ( imageBits )
      {
      case 8: precision = 1; break;
      case 16: precision = 2; break;
      default: // ?!
      case 32: precision = image.IsFloatSample() ? 4 : 7; break;
      case 64: precision = 12; break;
      }
      scale = 1023;
      break;

   case 12:
      switch ( imageBits )
      {
      case 8: precision = 1; break;
      case 16: precision = 2; break;
      default: // ?!
      case 32: precision = image.IsFloatSample() ? 4 : 7; break;
      case 64: precision = 12; break;
      }
      scale = 4095;
      break;

   case 14:
      switch ( imageBits )
      {
      case 8: precision = 1; break;
      case 16: precision = 1; break;
      default: // ?!
      case 32: precision = image.IsFloatSample() ? 3 : 6; break;
      case 64: precision = 11; break;
      }
      scale = 16383;
      break;

   case 16:
      switch ( imageBits )
      {
      case 8: precision = 1; break;
      case 16: precision = 1; break;
      default: // ?!
      case 32: precision = image.IsFloatSample() ? 3 : 6; break;
      case 64: precision = 11; break;
      }
      scale = 65535;
      break;
   }

   GUI->Data_TreeBox.Clear();
   GUI->Data_TreeBox.DisableUpdates();

   GUI->Data_TreeBox.ShowHeader();

   int n = 2;
   if ( image->HasAlphaChannels() )
      ++n;

   double f = ResourcePixelRatio();
   if ( image->IsColor() )
   {
      GUI->Data_TreeBox.SetNumberOfColumns( n + 3 );
      GUI->Data_TreeBox.SetHeaderText( 1, " R" );
      GUI->Data_TreeBox.SetHeaderIcon( 1, UIScaledResource( f, ":/toolbar/image-display-red.png" ) );
      GUI->Data_TreeBox.SetHeaderText( 2, " G" );
      GUI->Data_TreeBox.SetHeaderIcon( 2, UIScaledResource( f, ":/toolbar/image-display-green.png" ) );
      GUI->Data_TreeBox.SetHeaderText( 3, " B" );
      GUI->Data_TreeBox.SetHeaderIcon( 3, UIScaledResource( f, ":/toolbar/image-display-blue.png" ) );
      if ( image->HasAlphaChannels() )
      {
         GUI->Data_TreeBox.SetHeaderText( 4, " A" );
         GUI->Data_TreeBox.SetHeaderIcon( 4, UIScaledResource( f, ":/toolbar/image-display-alpha.png" ) );
      }
   }
   else
   {
      GUI->Data_TreeBox.SetNumberOfColumns( n + 1 );
      GUI->Data_TreeBox.SetHeaderText( 1, " K" );
      GUI->Data_TreeBox.SetHeaderIcon( 1, UIScaledResource( f, ":/toolbar/image-display-luminance.png" ) );
      if ( image->HasAlphaChannels() )
      {
         GUI->Data_TreeBox.SetHeaderText( 2, " A" );
         GUI->Data_TreeBox.SetHeaderIcon( 2, UIScaledResource( f, ":/toolbar/image-display-alpha.png" ) );
      }
   }

   GUI->Data_TreeBox.SetHeaderText( 0, "" );
   GUI->Data_TreeBox.SetHeaderText( GUI->Data_TreeBox.NumberOfColumns()-1, "" );
   GUI->Data_TreeBox.SetHeaderIcon( GUI->Data_TreeBox.NumberOfColumns()-1, Bitmap() );

   for ( int i = 0, n = GUI->Data_TreeBox.NumberOfColumns(); i < n; ++i )
      GUI->Data_TreeBox.SetHeaderAlignment( i, TextAlign::Left|TextAlign::VertCenter );

   DVector avgDev = m_avgDev;
   DVector MAD    = m_MAD;
   DVector BWMV   = m_BWMV;
   DVector PBMV   = m_PBMV;
   DVector Sn     = m_Sn;
   DVector Qn     = m_Qn;
   if ( GUI->Normalized_CheckBox.IsChecked() )
   {
      avgDev *= 1.2533;
      MAD    *= 1.4826;
      BWMV   *= 0.9901;
      PBMV   *= 0.9709;
      Sn     *= 1.1926;
      Qn     *= 2.2191;
   }

   if ( m_doCount )
   {
      new StatisticsTreeNode( GUI->Data_TreeBox, "count (%)",     m_count,         image->NumberOfPixels() );
      new StatisticsTreeNode( GUI->Data_TreeBox, "count (px)",    m_count );
   }
   if ( m_doMean )
      new StatisticsTreeNode( GUI->Data_TreeBox, "mean",          m_mean,          realFormat, precision, scale );
   if ( m_doModulus )
      new StatisticsTreeNode( GUI->Data_TreeBox, "modulus",       m_modulus,       realFormat, precision, scale );
   if ( m_doNorm )
      new StatisticsTreeNode( GUI->Data_TreeBox, "norm",          m_norm,          realFormat, precision, scale );
   if ( m_doSumOfSquares )
      new StatisticsTreeNode( GUI->Data_TreeBox, "sumOfSquares",  m_sumOfSquares,  realFormat, precision, scale );
   if ( m_doMeanOfSquares )
      new StatisticsTreeNode( GUI->Data_TreeBox, "meanOfSquares", m_meanOfSquares, realFormat, precision, scale );
   if ( m_doMedian )
      new StatisticsTreeNode( GUI->Data_TreeBox, "median",        m_median,        realFormat, precision, scale );
   if ( m_doVariance )
      new StatisticsTreeNode( GUI->Data_TreeBox, "variance",      m_variance,      realFormat, precision, scale*scale ); // the variance scales on the square of the data
   if ( m_doStdDev )
      new StatisticsTreeNode( GUI->Data_TreeBox, "stdDev",        m_stdDev,        realFormat, precision, scale );
   if ( m_doAvgDev )
      new StatisticsTreeNode( GUI->Data_TreeBox, "avgDev",          avgDev,        realFormat, precision, scale );
   if ( m_doMAD )
      new StatisticsTreeNode( GUI->Data_TreeBox, "MAD",             MAD,           realFormat, precision, scale );
   if ( m_doBWMV )
      new StatisticsTreeNode( GUI->Data_TreeBox, "sqrt(BWMV)",      BWMV,          realFormat, precision, scale );
   if ( m_doPBMV )
      new StatisticsTreeNode( GUI->Data_TreeBox, "sqrt(PBMV)",      PBMV,          realFormat, precision, scale );
   if ( m_doSn )
      new StatisticsTreeNode( GUI->Data_TreeBox, "Sn",              Sn,            realFormat, precision, scale );
   if ( m_doQn )
      new StatisticsTreeNode( GUI->Data_TreeBox, "Qn",              Qn,            realFormat, precision, scale );
   if ( m_doMinimum )
      new StatisticsTreeNode( GUI->Data_TreeBox, "minimum",       m_minimum,       realFormat, precision, scale );
   if ( m_doMaximum )
      new StatisticsTreeNode( GUI->Data_TreeBox, "maximum",       m_maximum,       realFormat, precision, scale );
   if ( m_doMinimumPos )
      new StatisticsTreeNode( GUI->Data_TreeBox, "minimumPos",    m_minimumPos );
   if ( m_doMaximumPos )
      new StatisticsTreeNode( GUI->Data_TreeBox, "maximumPos",    m_maximumPos );

   GUI->Data_TreeBox.EnableUpdates();

   for ( int i = 0, n = GUI->Data_TreeBox.NumberOfColumns(); i < n; ++i )
      GUI->Data_TreeBox.AdjustColumnWidthToContents( i );
}

String StatisticsInterface::ToText() const
{
   if ( m_currentView.IsNull() )
      return "<* No view selected *>";

   IVector columnWidth( 0, GUI->Data_TreeBox.NumberOfColumns() );
   if ( GUI->Data_TreeBox.IsHeaderVisible() )
      for ( int k = 0; k < columnWidth.Length(); ++k )
         if ( int( GUI->Data_TreeBox.HeaderText( k ).Length() ) > columnWidth[k] )
            columnWidth[k] = int( GUI->Data_TreeBox.HeaderText( k ).Length() );
   for ( int j = 0; j < GUI->Data_TreeBox.NumberOfChildren(); ++j )
   {
      const TreeBox::Node* child = GUI->Data_TreeBox.Child( j );
      for ( int k = 0; k < columnWidth.Length(); ++k )
         if ( int( child->Text( k ).Length() ) > columnWidth[k] )
            columnWidth[k] = int( child->Text( k ).Length() );
   }

   String text = m_currentView.FullId() + '\n';
   if ( GUI->Data_TreeBox.IsHeaderVisible() )
   {
      String rowText;
      for ( int k = 0; k < columnWidth.Length(); ++k )
         if ( columnWidth[k] > 0 )
         {
            String colText = GUI->Data_TreeBox.HeaderText( k ).Trimmed();
            rowText += colText + String( ' ', columnWidth[k] - colText.Length() + 2 );
         }
      text += rowText.TrimmedRight() + '\n';
   }
   for ( int j = 0; j < GUI->Data_TreeBox.NumberOfChildren(); ++j )
   {
      const TreeBox::Node* child = GUI->Data_TreeBox.Child( j );
      String rowText;
      for ( int k = 0; k < columnWidth.Length(); ++k )
         if ( columnWidth[k] > 0 )
         {
            String colText = child->Text( k ).TrimmedRight();
            rowText += colText + String( ' ', columnWidth[k] - colText.Length() + 2 );
         }
      text += rowText.TrimmedRight() + '\n';
   }

   return text;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void StatisticsInterface::__ViewList_ViewSelected( ViewList& sender, View& view )
{
   DeactivateTrackView();
   UpdateControls();
}

void StatisticsInterface::__Button_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Scientific_CheckBox )
   {
      UpdateControls();
   }
   else if ( sender == GUI->Normalized_CheckBox )
   {
      UpdateControls();
   }
   else if ( sender == GUI->Unclipped_CheckBox )
   {
      UpdateControls();
   }
   else if ( sender == GUI->ToText_ToolButton )
   {
      StatisticsTextDialog d( ToText() );
      d.Execute();
   }
   else if ( sender == GUI->Options_ToolButton )
   {
      StatisticsOptionsDialog d;
      if ( d.Execute() )
         UpdateControls();
   }
}

void StatisticsInterface::__ComboBox_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Range_ComboBox )
   {
      m_rangeBits = ComboBoxItemIndexToBits( itemIndex );
      UpdateControls();
   }
}

void StatisticsInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllViews_ViewList || sender == GUI->Data_TreeBox.Viewport() )
      wantsView = true;
}

void StatisticsInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->AllViews_ViewList || sender == GUI->Data_TreeBox.Viewport() )
   {
      DeactivateTrackView();
      GUI->AllViews_ViewList.SelectView( view );
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

StatisticsInterface::GUIData::GUIData( StatisticsInterface& w )
{
   AllViews_ViewList.OnViewSelected( (ViewList::view_event_handler)&StatisticsInterface::__ViewList_ViewSelected, w );
   AllViews_ViewList.OnViewDrag( (Control::view_drag_event_handler)&StatisticsInterface::__ViewDrag, w );
   AllViews_ViewList.OnViewDrop( (Control::view_drop_event_handler)&StatisticsInterface::__ViewDrop, w );

   for ( size_type i = 0; i < ItemsInArray( s_comboBoxRangeItems ); ++i )
      Range_ComboBox.AddItem( s_comboBoxRangeItems[i] );
   Range_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&StatisticsInterface::__ComboBox_ItemSelected, w );

   Scientific_CheckBox.SetText( "Scientific notation" );
   Scientific_CheckBox.OnClick( (Button::click_event_handler)&StatisticsInterface::__Button_Click, w );

   Normalized_CheckBox.SetText( "Normalized" );
   Normalized_CheckBox.SetToolTip( "<p>When this option is enabled, all scale estimates are consistent with the "
      "standard deviation of a normal distribution.</p>" );
   Normalized_CheckBox.OnClick( (Button::click_event_handler)&StatisticsInterface::__Button_Click, w );

   Unclipped_CheckBox.SetText( "Unclipped" );
   Unclipped_CheckBox.SetToolTip( "<p>By default, the Statistics tool computes statistics for the subset of pixel "
      "sample values within the (0,1) normalized range, that is, <i>excluding</i> pure black and white pixels. "
      "Enable this option to compute statistics for the unclipped [0,1] range.</p>" );
   Unclipped_CheckBox.OnClick( (Button::click_event_handler)&StatisticsInterface::__Button_Click, w );

   ToText_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/window-text.png" ) ) );
   ToText_ToolButton.SetScaledFixedSize( 24, 24 );
   ToText_ToolButton.SetToolTip( "Text View" );
   ToText_ToolButton.OnClick( (Button::click_event_handler)&StatisticsInterface::__Button_Click, w );

   Options_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/wrench.png" ) ) );
   Options_ToolButton.SetScaledFixedSize( 24, 24 );
   Options_ToolButton.SetToolTip( "Statistics Options" );
   Options_ToolButton.OnClick( (Button::click_event_handler)&StatisticsInterface::__Button_Click, w );

   Format_Sizer.Add( Range_ComboBox );
   Format_Sizer.AddSpacing( 16 );
   Format_Sizer.Add( Scientific_CheckBox );
   Format_Sizer.AddSpacing( 16 );
   Format_Sizer.Add( Normalized_CheckBox );
   Format_Sizer.AddSpacing( 16 );
   Format_Sizer.Add( Unclipped_CheckBox );
   Format_Sizer.AddSpacing( 32 );
   Format_Sizer.AddStretch();
   Format_Sizer.Add( ToText_ToolButton );
   Format_Sizer.AddSpacing( 8 );
   Format_Sizer.Add( Options_ToolButton );

   Data_TreeBox.DisableRootDecoration();
   Data_TreeBox.EnableAlternateRowColor();
   Data_TreeBox.SetStyleSheet( w.ScaledStyleSheet(
         "QTreeView {"
            "font-family: DejaVu Sans Mono, Monospace;"
            "font-size: 9pt;"
         "}"
         "QTreeView::item {"
            "padding: 4px 8px 4px 8px;"
         "}"
         "QHeaderView::section {"
            "padding: 2px 8px 2px 8px;"
         "}"
      ) );
   Data_TreeBox.HideHeader();
   Data_TreeBox.Restyle();
   Data_TreeBox.SetMinSize( Data_TreeBox.Font().Width( String( 'M', 60 ) ), w.LogicalPixelsToPhysical( 220 ) );
   Data_TreeBox.Viewport().OnViewDrag( (Control::view_drag_event_handler)&StatisticsInterface::__ViewDrag, w );
   Data_TreeBox.Viewport().OnViewDrop( (Control::view_drop_event_handler)&StatisticsInterface::__ViewDrop, w );

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( AllViews_ViewList );
   Global_Sizer.Add( Format_Sizer );
   Global_Sizer.Add( Data_TreeBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StatisticsInterface.cpp - Released 2017-05-02T09:43:00Z
