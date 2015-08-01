// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// ExternalToolInterface.cpp - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the ExternalTool PixInsight module.
//
// Copyright (c) 2014-2015, Georg Viehoever. All Rights Reserved.
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

#include "ExternalToolInterface.h"

#include <pcl/ViewSelectionDialog.h>
#include <pcl/FileDialog.h>
#include <pcl/FileInfo.h>
#include <pcl/Console.h>
#include <pcl/RealTimePreview.h>

#include "ExternalToolParameters.h"
#include "ExternalToolProcess.h"
#include "ExternalToolIcon.xpm"

namespace pcl
{

// ----------------------------------------------------------------------------

    ExternalToolInterface* TheExternalToolInterface = 0;


// ----------------------------------------------------------------------------

    ExternalToolInterface::ExternalToolInterface() :
            ProcessInterface(),
            m_instance( TheExternalToolProcess ),
            m_realTimeThread( 0 ),
            GUI( 0 )
    {
        TheExternalToolInterface = this;
    }

    ExternalToolInterface::~ExternalToolInterface()
    {
        if ( GUI != 0 )
            delete GUI, GUI = 0;
    }

    IsoString ExternalToolInterface::Id() const
    {
        return "ExternalTool";
    }

    MetaProcess* ExternalToolInterface::Process() const
    {
        return TheExternalToolProcess;
    }

    const char** ExternalToolInterface::IconImageXPM() const
    {
        return ExternalToolIcon_XPM;
    }

    InterfaceFeatures ExternalToolInterface::Features() const {
        // FIXME can we compute this from querying the MetaProcess?
        return InterfaceFeatures(InterfaceFeature::Default|InterfaceFeature::RealTimeButton|InterfaceFeature::ApplyGlobalButton)|
                InterfaceFeatures(InterfaceFeature::BrowseDocumentationButton);
    }

    void ExternalToolInterface::ApplyInstance() const
    {
        m_instance.LaunchOnCurrentView();
    }

    void ExternalToolInterface::ResetInstance()
    {
        ExternalToolInstance defaultInstance( TheExternalToolProcess );
        ImportProcess( defaultInstance );
    }

    bool ExternalToolInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
    {
        if ( GUI == 0 )
        {
            GUI = new GUIData( *this );
            SetWindowTitle( "ExternalTool" );
            UpdateControls();
        }

        dynamic = false;
        return &P == TheExternalToolProcess;
    }

    ProcessImplementation* ExternalToolInterface::NewProcess() const
    {
        return new ExternalToolInstance(m_instance);
    }

    bool ExternalToolInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
    {
        const ExternalToolInstance* r = dynamic_cast<const ExternalToolInstance*>( &p );
        if ( r == 0 )
        {
            whyNot = "Not an ExternalTool m_instance.";
            return false;
        }

        whyNot.Clear();
        return true;
    }

    bool ExternalToolInterface::RequiresInstanceValidation() const
    {
        return true;
    }

    bool ExternalToolInterface::ImportProcess( const ProcessImplementation& p )
    {
        m_instance.Assign( p );
        UpdateControls();
        UpdateRealTimePreview();
        return true;
    }

    //
    // RTP
    //
    void ExternalToolInterface::RealTimePreviewUpdated	(bool active) {
        //Console().WriteLn(String("RealTimePreviewUpdated(): active=")+String(active));
        if ( GUI != 0 )
        if ( active )
            RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
        else
            RealTimePreview::SetOwner( ProcessInterface::Null() );

    }

    bool ExternalToolInterface::RequiresRealTimePreviewUpdate(const UInt16Image &, const View &, int /*zoomlevel*/) const {
        return true;
    }

    //FIXME WantsImageNotifications?
    bool ExternalToolInterface::GenerateRealTimePreview( UInt16Image &image, const View&, int zoomLevel, String& info ) const {
        {
            m_realTimeThread = new RealTimeThread;

            for ( ;; )
            {
                m_realTimeThread->Reset( image, m_instance);
                m_realTimeThread->Start();

                while ( m_realTimeThread->IsActive() )
                {
                    ProcessEvents();

                    if ( !IsRealTimePreviewActive() )
                    {
                        m_realTimeThread->Abort();
                        m_realTimeThread->Wait();

                        delete m_realTimeThread;
                        m_realTimeThread = 0;
                        return false;
                    }
                }

                if ( !m_realTimeThread->IsAborted() )
                {
                    image.Assign( m_realTimeThread->m_image );

                    delete m_realTimeThread;
                    m_realTimeThread = 0;
                    return true;
                }
            }
        }

    }

    void ExternalToolInterface::UpdateRealTimePreview()
    {
        if ( IsRealTimePreviewActive() )
        {
            if ( m_realTimeThread != 0 )
                m_realTimeThread->Abort();
            GUI->UpdateRealTimePreview_Timer.Start();
        }
    }


// ----------------------------------------------------------------------------

    void ExternalToolInterface::UpdateControls()
    {
        GUI->Executable_Edit.SetText(m_instance.m_sExecutable);
        String sWhyNot;
        GUI->Help_ToolButton.Enable(m_instance.CanExecuteGlobal(sWhyNot));
        GUI->Options_Edit.SetText(m_instance.m_sOptions);

        GUI->ReadImage_CheckBox.SetChecked(m_instance.m_bReadImage);
        GUI->ReadFitsKeys_CheckBox.Enable(m_instance.m_bReadImage && !m_instance.m_bNewWindow);
        GUI->ReadFitsKeys_CheckBox.SetChecked(m_instance.m_bReadFitsKeys);
        GUI->RescaleResult_CheckBox.Enable(m_instance.m_bReadImage);
        GUI->RescaleResult_CheckBox.SetChecked(m_instance.m_bRescaleResult);
        GUI->NewWindow_CheckBox.Enable(m_instance.m_bReadImage);
        GUI->NewWindow_CheckBox.SetChecked(m_instance.m_bNewWindow);

        GUI->Parameter1_CheckBox.Enable(true);
        GUI->Parameter1_CheckBox.SetChecked(m_instance.m_bParam1);
        GUI->Parameter2_CheckBox.Enable(m_instance.m_bParam1);
        GUI->Parameter2_CheckBox.SetChecked(m_instance.m_bParam2);
        GUI->Parameter3_CheckBox.Enable(m_instance.m_bParam2);
        GUI->Parameter3_CheckBox.SetChecked(m_instance.m_bParam3);
        GUI->Parameter1_NumericControl.SetValue(m_instance.m_dParam1);
        GUI->Parameter1_NumericControl.Enable(m_instance.m_bParam1);
        GUI->Parameter2_NumericControl.SetValue(m_instance.m_dParam2);
        GUI->Parameter2_NumericControl.Enable(m_instance.m_bParam2);
        GUI->Parameter3_NumericControl.SetValue(m_instance.m_dParam3);
        GUI->Parameter3_NumericControl.Enable(m_instance.m_bParam3);

        GUI->HelpOptions_Edit.SetText(m_instance.m_sHelpOptions);
        GUI->GlobalOptions_Edit.SetText(m_instance.m_sGlobalOptions);
        GUI->SourceOptions_Edit.SetText(m_instance.m_sSourceOptions);
        GUI->ResultOptions_Edit.SetText(m_instance.m_sResultOptions);

    }

// ----------------------------------------------------------------------------

    void ExternalToolInterface::__NumericEditValueUpdated( NumericEdit& sender, double value ) {
        if (sender == GUI->Parameter1_NumericControl) {
            m_instance.m_dParam1 = value;
        } else if (sender == GUI->Parameter2_NumericControl) {
            m_instance.m_dParam2 = value;
        } else if (sender == GUI->Parameter3_NumericControl) {
            m_instance.m_dParam3 = value;
        }
        UpdateControls();
        UpdateRealTimePreview();
    }

    namespace {
        /// generate list of file filters for executable
        OpenFileDialog::filter_list const & GetExecutableFileFilter()
        {
            static OpenFileDialog::filter_list res;
            if(res.Length()>0){
                // already initialized
                return res;
            }
            // initialize
            FileFilter any;
            any.AddExtension("*");
            any.SetDescription("All files");

            FileFilter py2;
            py2.AddExtension("*.py");
            py2.AddExtension(".py2");
            py2.SetDescription("Python 2.x files");

            FileFilter py3;
            py3.AddExtension(".py");
            py3.AddExtension(".py3");
            py3.SetDescription("Python 3.x files");

            FileFilter perl;
            perl.AddExtension(".perl");
            perl.AddExtension(".pl");
            perl.SetDescription("Perl files");

            FileFilter r;
            r.AddExtension(".R");
            r.SetDescription("R files");

            FileFilter js;
            js.AddExtension(".js");
            js.SetDescription("Javascript files");

            FileFilter sh;
            sh.AddExtension(".sh");
            sh.AddExtension(".csh");
            sh.AddExtension(".tcsh");
            sh.SetDescription("Shell files");

            res.Add(any);
            res.Add(py3);
            res.Add(py2);
            res.Add(perl);
            res.Add(r);
            res.Add(js);
            res.Add(sh);
            return res;
        }
    }
    void ExternalToolInterface::__ButtonClicked( Button& sender, bool checked )
    {
        if ( sender == GUI->Executable_ToolButton ) {
            OpenFileDialog d;
            d.Filters().Add(GetExecutableFileFilter());
            d.DisableMultipleSelections();
            d.SetInitialPath(m_instance.m_sExecutable);
            d.SetCaption( "External Tool: Select Executable" );
            // FIXME apparently there is no way to check if file is executable within dialog???
            if ( d.Execute() ) {
                const String path=d.FileName();
                FileInfo const f(path);
                if(!f.IsExecutable())
                {
                    throw Error("Selected File is not executable");
                }
                m_instance.m_sExecutable=path;
                GUI->Executable_Edit.SetText(path);
            }
        } else if (sender == GUI->Help_ToolButton) {
            m_instance.ExecuteHelp();
        } else if (sender == GUI->Parameter1_CheckBox ) {
            m_instance.m_bParam1=checked;
            if(!checked){
                m_instance.m_bParam2= m_instance.m_bParam3=false;
            }
        } else if (sender == GUI->Parameter2_CheckBox) {
            m_instance.m_bParam2=checked;
            if(!checked){
                m_instance.m_bParam3=false;
            }
        } else if (sender == GUI->Parameter3_CheckBox) {
            m_instance.m_bParam3=checked;
        } else if (sender == GUI->ReadImage_CheckBox) {
            m_instance.m_bReadImage=checked;
        } else if (sender == GUI->ReadFitsKeys_CheckBox) {
            m_instance.m_bReadFitsKeys=checked;
        } else if (sender == GUI->RescaleResult_CheckBox) {
            m_instance.m_bRescaleResult = checked;
        } else if (sender == GUI->NewWindow_CheckBox) {
            m_instance.m_bNewWindow = checked;
        }

        UpdateControls();
        UpdateRealTimePreview();
    }

    void ExternalToolInterface::__EditCompleted( Edit& sender )
    {
        const String value = sender.Text(); //.Trimmed() makes it difficult to enter " ", therefore deactivated
        //sender.SetText(value);
        if ( sender == GUI->Executable_Edit ) {
            m_instance.m_sExecutable = value;
        } else if (sender == GUI->Options_Edit ) {
            m_instance.m_sOptions=value;
        } else if (sender == GUI->HelpOptions_Edit ) {
            m_instance.m_sHelpOptions=value;
        } else if (sender == GUI->GlobalOptions_Edit ) {
            m_instance.m_sGlobalOptions=value;
        } else if (sender == GUI->SourceOptions_Edit ) {
            m_instance.m_sSourceOptions=value;
        } else if (sender == GUI->ResultOptions_Edit) {
            m_instance.m_sResultOptions =value;
        }
        UpdateControls();
        UpdateRealTimePreview();
    }

    void ExternalToolInterface::__TextUpdated(Edit &sender,const String & string)
    {
        __EditCompleted(sender);
    }

    void ExternalToolInterface::__SectionToggle( SectionBar& sender, Control& section, bool start )
    {
        //FIXME currently intentionally empty
    }

    void ExternalToolInterface::__UpdateRealTimePreview_Timer( Timer& sender )
    {
        if ( m_realTimeThread == 0 )
        {
            if ( IsRealTimePreviewActive() )
                RealTimePreview::Update();
        }
    }

    // class ExternalToolInterface::RealTimeThread

    ExternalToolInterface::RealTimeThread::RealTimeThread() :
            Thread(), m_instance( TheExternalToolProcess )
    {
    }

    void ExternalToolInterface::RealTimeThread::Reset( const UInt16Image& image,
            const ExternalToolInstance& instance )
    {
        image.ResetSelections();
        m_image.Assign( image );
        m_instance.Assign( instance );
    }

    void ExternalToolInterface::RealTimeThread::Run()
    {
        m_instance.Preview( m_image );
    }

// ----------------------------------------------------------------------------

    ExternalToolInterface::GUIData::GUIData( ExternalToolInterface& w )
    {
        pcl::Font const fnt = w.Font();
        int const labelWidth1 = fnt.Width( String( "G.ViewId:" ) ); // the longest label text
        int const editWidth1 = fnt.Width( String( '0', 7 ) );
        int const editWidth2 = fnt.Width( String( '0', 10  ) );

        String const sExecutableToolTip="Path to executable.";
        Executable_Label.SetText( "Exec:");
        Executable_Label.SetMinWidth(labelWidth1);
        Executable_Label.SetToolTip(sExecutableToolTip);
        Executable_Edit.SetMinWidth(editWidth1);
        Executable_Edit.SetToolTip(sExecutableToolTip);
        //Executable_Edit.OnEditCompleted(static_cast<Edit::edit_event_handler>(&ExternalToolInterface::__EditCompleted), w);
        Executable_Edit.OnTextUpdated(static_cast<Edit::text_event_handler>(&ExternalToolInterface::__TextUpdated), w); // need this to change help status on any key press

        Executable_ToolButton.SetIcon( Bitmap( ":/icons/select-file.png" ) );
        Executable_ToolButton.SetToolTip("Select path to executable");
        Executable_ToolButton.OnClick(static_cast<ToolButton::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);

        //Help_ToolButton.SetText("Get Help from Executable");
        Help_ToolButton.SetToolTip("<p>Run executable with help option. The resulting text is displayed on the Process console.</p>");
        Help_ToolButton.SetIcon(Bitmap(":/images/help.png"));
        Help_ToolButton.Enable(false);
        Help_ToolButton.OnClick(static_cast<pcl::Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked), w);

        Executable_Sizer.SetSpacing(4);
        Executable_Sizer.Add(Executable_Label);
        Executable_Sizer.Add(Executable_Edit);
        Executable_Sizer.Add(Executable_ToolButton);
        Executable_Sizer.Add(Help_ToolButton);

        String const sOptionsToolTip="Options always passed to the executable, even with help mode";
        Options_Label.SetText( "Options:");
        Options_Label.SetMinWidth(labelWidth1);
        Options_Label.SetToolTip(sOptionsToolTip);
        Options_Edit.SetMinWidth(editWidth1);
        Options_Edit.SetToolTip(sOptionsToolTip);
        //Options_Edit.OnEditCompleted(static_cast<Edit::edit_event_handler>(&ExternalToolInterface::__EditCompleted), w);
        Options_Edit.OnTextUpdated(static_cast<Edit::text_event_handler>(&ExternalToolInterface::__TextUpdated), w);

        Options_Sizer.SetSpacing(4);
        Options_Sizer.Add(Options_Label);
        Options_Sizer.Add(Options_Edit);

        String const sDestinationToolTip="<p>Determine how the result image is used.</p>";
        Destination_SectionBar.SetTitle("Destination");
        Destination_SectionBar.SetToolTip(sDestinationToolTip);
        Destination_SectionBar.OnToggleSection(static_cast<SectionBar::section_event_handler>(&ExternalToolInterface::__SectionToggle),w);

        ReadImage_CheckBox.SetText("Read Image");
        ReadImage_CheckBox.SetToolTip("<p>Excpect the external tool to produce and image, and read the contents. If not checked, "
                "the external tool need not produce an image. If one is produced, it is ignored.");
        ReadImage_CheckBox.OnClick(static_cast<Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);

        NewWindow_CheckBox.SetText("New Window");
        NewWindow_CheckBox.SetToolTip("<p>Display result image in new window.</p>");
        NewWindow_CheckBox.OnClick(static_cast<Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);
        ReadImage_HorizontalSizer.SetSpacing(4);
        ReadImage_HorizontalSizer.Add(ReadImage_CheckBox);
        ReadImage_HorizontalSizer.Add(NewWindow_CheckBox);

        ReadFitsKeys_CheckBox.SetText("Read FITS keys");
        ReadFitsKeys_CheckBox.SetToolTip("<p>Read the FITS keys and the ICC profile from the result image, if the result image is going into a main view. "
                "If not checked, both are always ignored.</p>");
        ReadFitsKeys_CheckBox.OnClick(static_cast<Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);
        RescaleResult_CheckBox.SetText("Rescale Result");
        RescaleResult_CheckBox.SetToolTip("<p>Rescale result image to range of image data type ([0,1] for real numbers). If not selected, the result is truncated "
                "to the native range if necessary</p>");
        RescaleResult_CheckBox.OnClick(static_cast<Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);
        ReadFits_HorizontalSizer.SetSpacing(4);
        ReadFits_HorizontalSizer.Add(ReadFitsKeys_CheckBox);
        ReadFits_HorizontalSizer.Add(RescaleResult_CheckBox);


        Destination_VerticalSizer.SetSpacing(4);
        Destination_VerticalSizer.Add(ReadImage_HorizontalSizer);
        Destination_VerticalSizer.Add(ReadFits_HorizontalSizer);

        Destination_Control.SetSizer(Destination_VerticalSizer);
        Destination_Control.AdjustToContents();
        Destination_SectionBar.SetSection(Destination_Control);

        Parameters_SectionBar.SetTitle("Numerical Parameters");
        Parameters_SectionBar.SetToolTip("<p>Optional 3 numerical parameters that are added at the end of the command line</p>");
        Parameters_SectionBar.OnToggleSection(static_cast<SectionBar::section_event_handler>(&ExternalToolInterface::__SectionToggle),w);

        Parameter1_CheckBox.SetText("P1:");
        Parameter1_CheckBox.SetToolTip("Activate to set parameter 1");
        Parameter1_CheckBox.OnClick(static_cast<Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);

        Parameter1_NumericControl.SetToolTip("Parameter 1: a value in [0,1]");
        Parameter1_NumericControl.slider.SetRange(0,250);
        Parameter1_NumericControl.SetReal();
        Parameter1_NumericControl.SetRange(TheExternalToolParamDouble1->MinimumValue(), TheExternalToolParamDouble1->MaximumValue());
        Parameter1_NumericControl.SetPrecision(TheExternalToolParamDouble1->Precision());
        Parameter1_NumericControl.edit.SetFixedWidth(editWidth2);
        Parameter1_NumericControl.OnValueUpdated(static_cast<pcl::NumericEdit::value_event_handler>(&ExternalToolInterface::__NumericEditValueUpdated),w);

        Parameter1_HorizontalSizer.SetSpacing(4);
        Parameter1_HorizontalSizer.Add(Parameter1_CheckBox);
        Parameter1_HorizontalSizer.Add(Parameter1_NumericControl);

        Parameter2_CheckBox.SetText("P2:");
        Parameter2_CheckBox.SetToolTip("Activate to set parameter 2");
        Parameter2_CheckBox.OnClick(static_cast<Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);

        Parameter2_NumericControl.SetToolTip("Parameter 2: a value in [0,1]");
        Parameter2_NumericControl.slider.SetRange(0,250);
        Parameter2_NumericControl.SetReal();
        Parameter2_NumericControl.SetRange(TheExternalToolParamDouble2->MinimumValue(), TheExternalToolParamDouble2->MaximumValue());
        Parameter2_NumericControl.SetPrecision(TheExternalToolParamDouble2->Precision());
        Parameter2_NumericControl.edit.SetFixedWidth(editWidth2);
        Parameter2_NumericControl.OnValueUpdated(static_cast<pcl::NumericEdit::value_event_handler>(&ExternalToolInterface::__NumericEditValueUpdated),w);

        Parameter2_HorizontalSizer.SetSpacing(4);
        Parameter2_HorizontalSizer.Add(Parameter2_CheckBox);
        Parameter2_HorizontalSizer.Add(Parameter2_NumericControl);

        Parameter3_CheckBox.SetText("P3:");
        Parameter3_CheckBox.SetToolTip("Activate to set parameter 3");
        Parameter3_CheckBox.OnClick(static_cast<Button::click_event_handler>(&ExternalToolInterface::__ButtonClicked),w);

        Parameter3_NumericControl.SetToolTip("Parameter 3: a value in [0,1]");
        Parameter3_NumericControl.slider.SetRange(0,250);
        Parameter3_NumericControl.SetReal();
        Parameter3_NumericControl.SetRange(TheExternalToolParamDouble3->MinimumValue(), TheExternalToolParamDouble3->MaximumValue());
        Parameter3_NumericControl.SetPrecision(TheExternalToolParamDouble3->Precision());
        Parameter3_NumericControl.edit.SetFixedWidth(editWidth2);
        Parameter3_NumericControl.OnValueUpdated(static_cast<pcl::NumericEdit::value_event_handler>(&ExternalToolInterface::__NumericEditValueUpdated),w);

        Parameter3_HorizontalSizer.SetSpacing(4);
        Parameter3_HorizontalSizer.Add(Parameter3_CheckBox);
        Parameter3_HorizontalSizer.Add(Parameter3_NumericControl);

        Parameters_VerticalSizer.SetSpacing(4);
        Parameters_VerticalSizer.Add(Parameter1_HorizontalSizer);
        Parameters_VerticalSizer.Add(Parameter2_HorizontalSizer);
        Parameters_VerticalSizer.Add(Parameter3_HorizontalSizer);

        Parameters_Control.SetSizer(Parameters_VerticalSizer);
        Parameters_Control.AdjustToContents();

        Parameters_SectionBar.SetSection(Parameters_Control);

        Options_SectionBar.SetTitle("Functional Command Line Options");
        Options_SectionBar.SetToolTip("<p>Define the detailed options used to indicate different elements of the command line. "
                "If possible simply use the defaults. The options are added to the begin of the command line options.</p>");
        Options_SectionBar.OnToggleSection(static_cast<SectionBar::section_event_handler>(&ExternalToolInterface::__SectionToggle),w);


        String const sHelpOptionsToolTip="<p>Option to indicate that user selects help mode."
                "The help text generated by the executable is displayed in the process console.</p>";
        HelpOptions_Label.SetText( "Help options:");
        HelpOptions_Label.SetMinWidth(labelWidth1);
        HelpOptions_Label.SetToolTip(sHelpOptionsToolTip);
        HelpOptions_Edit.SetMinWidth(editWidth1);
        HelpOptions_Edit.SetToolTip(sHelpOptionsToolTip);
        HelpOptions_Edit.OnEditCompleted(static_cast<Edit::edit_event_handler>(&ExternalToolInterface::__EditCompleted), w);
        HelpOptions_Edit.OnTextUpdated(static_cast<Edit::text_event_handler>(&ExternalToolInterface::__TextUpdated), w);
        HelpOptions_Sizer.SetSpacing(4);
        HelpOptions_Sizer.Add(HelpOptions_Label);
        HelpOptions_Sizer.Add(HelpOptions_Edit);

        String const sGlobalOptionsToolTip="<p>Option to indicate a global apply.</p>";
        GlobalOptions_Label.SetText( "Global options:");
        GlobalOptions_Label.SetMinWidth(labelWidth1);
        GlobalOptions_Label.SetToolTip(sGlobalOptionsToolTip);
        GlobalOptions_Edit.SetMinWidth(editWidth1);
        GlobalOptions_Edit.SetToolTip(sGlobalOptionsToolTip);
        GlobalOptions_Edit.OnEditCompleted(static_cast<Edit::edit_event_handler>(&ExternalToolInterface::__EditCompleted), w);
        GlobalOptions_Edit.OnTextUpdated(static_cast<Edit::text_event_handler>(&ExternalToolInterface::__TextUpdated), w);
        GlobalOptions_Sizer.SetSpacing(4);
        GlobalOptions_Sizer.Add(GlobalOptions_Label);
        GlobalOptions_Sizer.Add(GlobalOptions_Edit);

        String const sSourceOptionsToolTip="<p>Option to indicate the next value is the name of the source image file. "
                "The source image file is processed to produce the output file. There might be no source image on a global apply.</p>";
        SourceOptions_Label.SetText( "Source image options:");
        SourceOptions_Label.SetMinWidth(labelWidth1);
        SourceOptions_Label.SetToolTip(sSourceOptionsToolTip);
        SourceOptions_Edit.SetMinWidth(editWidth1);
        SourceOptions_Edit.SetToolTip(sSourceOptionsToolTip);
        SourceOptions_Edit.OnEditCompleted(static_cast<Edit::edit_event_handler>(&ExternalToolInterface::__EditCompleted), w);
        SourceOptions_Edit.OnTextUpdated(static_cast<Edit::text_event_handler>(&ExternalToolInterface::__TextUpdated), w);
        SourceOptions_Sizer.SetSpacing(4);
        SourceOptions_Sizer.Add(SourceOptions_Label);
        SourceOptions_Sizer.Add(SourceOptions_Edit);

        String const sResultOptionsToolTip="<p>Option to indicate the next value is the name of the result image file. "
                "The result image file is produced by the executable, and is read into PixInsight as the result of this process.</p>";
        ResultOptions_Label.SetText( "Result image options:");
        ResultOptions_Label.SetMinWidth(labelWidth1);
        ResultOptions_Label.SetToolTip(sResultOptionsToolTip);
        ResultOptions_Edit.SetMinWidth(editWidth1);
        ResultOptions_Edit.SetToolTip(sResultOptionsToolTip);
        ResultOptions_Edit.OnEditCompleted(static_cast<Edit::edit_event_handler>(&ExternalToolInterface::__EditCompleted), w);
        ResultOptions_Edit.OnTextUpdated(static_cast<Edit::text_event_handler>(&ExternalToolInterface::__TextUpdated), w);
        ResultOptions_Sizer.SetSpacing(4);
        ResultOptions_Sizer.Add(ResultOptions_Label);
        ResultOptions_Sizer.Add(ResultOptions_Edit);

        Options_VerticalSizer.SetSpacing(4);
        Options_VerticalSizer.Add(HelpOptions_Sizer);
        Options_VerticalSizer.Add(GlobalOptions_Sizer);
        Options_VerticalSizer.Add(SourceOptions_Sizer);
        Options_VerticalSizer.Add(ResultOptions_Sizer);

        Options_Control.SetSizer(Options_VerticalSizer);
        Options_Control.AdjustToContents();

        Options_SectionBar.SetSection(Options_Control);

        Global_Sizer.SetMargin(8);
        Global_Sizer.SetSpacing(6);
        Global_Sizer.Add(Executable_Sizer);
        Global_Sizer.Add(Options_Sizer);
        Global_Sizer.Add(Destination_SectionBar);
        Global_Sizer.Add(Destination_Control);
        Global_Sizer.Add(Parameters_SectionBar);
        Global_Sizer.Add(Parameters_Control);
        Global_Sizer.Add(Options_SectionBar);
        Global_Sizer.Add(Options_Control);

        w.SetSizer(Global_Sizer);
        //w.SetFixedWidth();
        Destination_Control.Hide();
        Parameters_Control.Hide();
        Options_Control.Hide();
        w.AdjustToContents();
        w.SetFixedSize();

        UpdateRealTimePreview_Timer.SetSingleShot();
        UpdateRealTimePreview_Timer.SetInterval( 0.025 );
        UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&ExternalToolInterface::__UpdateRealTimePreview_Timer, w );
    }

// ----------------------------------------------------------------------------

} // pcl
