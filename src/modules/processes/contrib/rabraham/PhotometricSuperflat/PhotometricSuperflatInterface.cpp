// ****************************************************************************
// PixInsight Class Library - PCL 02.00.09.0651
// Standard PhotometricSuperflat Process Module Version 01.00.02.0116
// ****************************************************************************
// PhotometricSuperflatInterface.cpp - Released 2013/12/13 00:03:11 UTC
// ****************************************************************************
// This file is part of the standard PhotometricSuperflat PixInsight module.
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

#include "PhotometricSuperflatInterface.h"
#include "PhotometricSuperflatParameters.h"
#include "PhotometricSuperflatProcess.h"

#include <pcl/Math.h>
#include <pcl/FileDialog.h>
#include <pcl/Settings.h>
#include <pcl/Console.h>

namespace pcl
{

    // ----------------------------------------------------------------------------

    PhotometricSuperflatInterface* ThePhotometricSuperflatInterface = 0;

    // ----------------------------------------------------------------------------

    //#include "PhotometricSuperflatIcon.xpm"

    // ----------------------------------------------------------------------------

    PhotometricSuperflatInterface::PhotometricSuperflatInterface() :
    ProcessInterface(), instance( ThePhotometricSuperflatProcess ), GUI( 0 )
    {
        ThePhotometricSuperflatInterface = this;
    }

    PhotometricSuperflatInterface::~PhotometricSuperflatInterface()
    {
        if ( GUI != 0 )
            delete GUI, GUI = 0;
    }

    IsoString PhotometricSuperflatInterface::Id() const
    {
        return "PhotometricSuperflat";
    }

    MetaProcess* PhotometricSuperflatInterface::Process() const
    {
        return ThePhotometricSuperflatProcess;
    }

    const char** PhotometricSuperflatInterface::IconImageXPM() const
    {
        return 0; // PhotometricSuperflatIcon_XPM; ---> put a nice icon here
    }

    void PhotometricSuperflatInterface::ApplyInstance() const
    {
        instance.LaunchOnCurrentView();
    }

    void PhotometricSuperflatInterface::ResetInstance()
    {
        PhotometricSuperflatInstance defaultInstance( ThePhotometricSuperflatProcess );
        ImportProcess( defaultInstance );
    }

    bool PhotometricSuperflatInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
    {
        if ( GUI == 0 )
        {
            GUI = new GUIData( *this );
            SetWindowTitle( "PhotometricSuperflat" );
            UpdateControls();
        }

        dynamic = false;
        return &P == ThePhotometricSuperflatProcess;
    }

    ProcessImplementation* PhotometricSuperflatInterface::NewProcess() const
    {
        return new PhotometricSuperflatInstance( instance );
    }

    bool PhotometricSuperflatInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
    {
        const PhotometricSuperflatInstance* r = dynamic_cast<const PhotometricSuperflatInstance*>( &p );
        if ( r == 0 )
        {
            whyNot = "Not a PhotometricSuperflat instance.";
            return false;
        }

        whyNot.Clear();
        return true;
    }

    bool PhotometricSuperflatInterface::RequiresInstanceValidation() const
    {
        return true;
    }

    bool PhotometricSuperflatInterface::ImportProcess( const ProcessImplementation& p )
    {
        instance.Assign( p );
        UpdateControls();
        return true;
    }

    // ----------------------------------------------------------------------------

    void PhotometricSuperflatInterface::UpdateControls()
    {
        GUI->StarDatabase_Edit.Enable( );
        GUI->StarDatabase_Edit.SetText( instance.starDatabasePath );
        GUI->StarDatabase_ToolButton.Enable( );
        GUI->FitDegree_SpinBox.SetValue( instance.fitDegree );
    }

    // ----------------------------------------------------------------------------


    void PhotometricSuperflatInterface::__EditGetFocus( Control& sender )
    {
        if ( sender == GUI->StarDatabase_Edit )
        {
            // If you need to do something when sender gets focus, do it here.
        }
    }

    void PhotometricSuperflatInterface::__EditCompleted( Edit& sender )
    {
        if ( sender == GUI->StarDatabase_Edit )
        {
            String path = sender.Text().Trimmed();
            instance.starDatabasePath = path;
            sender.SetText( path );
        }

    }

    void PhotometricSuperflatInterface::__IntegerValueUpdated( SpinBox& sender, int value )
    {
        if ( sender == GUI->FitDegree_SpinBox )
            instance.fitDegree = value;
    }

    // ----------------------------------------------------------------------------

    // RGA: Interface is defined here!

    PhotometricSuperflatInterface::GUIData::GUIData( PhotometricSuperflatInterface& w )
    {
        pcl::Font fnt = w.Font();

        // The editable field that lets the user specify the text file. To the right of the field is an icon
        // which opens up a file browser when it is clicked.

        const char* starDatabaseToolTip = "<p>Output from the AperturePhotometry process in CSV format.</p>";
        StarDatabase_Label.SetText( "AperturePhotometry CSV file:" );

        int labelWidth1 = fnt.Width( String( "AperturePhotometry CSV file:" ) ); // the longest label text
        StarDatabase_Label.SetMinWidth(labelWidth1);

        StarDatabase_Edit.SetToolTip( starDatabaseToolTip );
        StarDatabase_Edit.OnEditCompleted( (Edit::edit_event_handler)&PhotometricSuperflatInterface::__EditCompleted, w );
        int editWidth1 = fnt.Width( String( '0', 50 ) );
        StarDatabase_Edit.SetMinWidth(editWidth1);

        StarDatabase_ToolButton.SetIcon( Bitmap( String( ":/icons/select-file.png" ) ) );
        StarDatabase_ToolButton.SetFixedSize( 20, 20 );
        StarDatabase_ToolButton.SetToolTip( starDatabaseToolTip );
        StarDatabase_ToolButton.OnClick( (Button::click_event_handler)&PhotometricSuperflatInterface::__Button_Clicked, w );

        StarDatabase_Sizer.SetSpacing( 4 );
        StarDatabase_Sizer.Add( StarDatabase_Edit, 100 );
        StarDatabase_Sizer.Add( StarDatabase_ToolButton );

        // The spin box that lets the user select the fit order

        int labelWidth2 = fnt.Width( String( "Fit Degree:" ) ); // the longest label text
        FitDegree_Label.SetText( "Fit Degree:" );
        FitDegree_Label.SetMinWidth( labelWidth2 );
        FitDegree_Label.SetToolTip( "<p>Degree of the fit. 0=constant, 1=linear, 2=quadratic, 3=cubic, 4=quartic.</p>" );
        FitDegree_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

        int spinBoxWidth1 = fnt.Width( String( '0', 5 ) );
        FitDegree_SpinBox.SetRange( int( ThePhotometricSuperflatFitDegreeParameter->MinimumValue() ),
                                    int( ThePhotometricSuperflatFitDegreeParameter->MaximumValue() ) );
        FitDegree_SpinBox.SetFixedWidth( spinBoxWidth1 );
        FitDegree_SpinBox.SetToolTip( "<p>Degree of the fit. 0=constant, 1=linear, 2=quadratic, 3=cubic, 4=quartic.</p>"  );
        FitDegree_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricSuperflatInterface::__IntegerValueUpdated, w );
        FitDegree_Sizer.SetSpacing( 4 );
        FitDegree_Sizer.Add( FitDegree_Label );
        FitDegree_Sizer.Add( FitDegree_SpinBox );
        FitDegree_Sizer.AddStretch();

        // Assemble the elements into the final GUI

        Global_Sizer.SetMargin( 8 );
        Global_Sizer.SetSpacing( 6 );
        Global_Sizer.Add( StarDatabase_Sizer );
        Global_Sizer.Add( FitDegree_Sizer );
        w.SetSizer( Global_Sizer );
        w.AdjustToContents();
        w.SetFixedSize();
    }


    // RGA: Action on button click is defined here

    void PhotometricSuperflatInterface::__Button_Clicked( Button& sender, bool checked )
    {
        if ( sender == GUI->StarDatabase_ToolButton )
        {
            OpenFileDialog d;
            d.DisableMultipleSelections();
            d.SetCaption( "StarGenerator: Select Star Database File" );
            if ( d.Execute() )
            {
                instance.starDatabasePath = d.FileName();
                GUI->StarDatabase_Edit.SetText( instance.starDatabasePath );
            }
        }
    }

    // ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF PhotometricSuperflatInterface.cpp - Released 2013/12/13 00:03:11 UTC
