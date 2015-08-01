// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// ExternalToolInterface.h - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the ExternalTool PixInsight module.
//
// Copyright (c) 2014-2015, Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ExternalToolInterface_h
#define __ExternalToolInterface_h

#include <pcl/CheckBox.h>
#include <pcl/PushButton.h>
#include <pcl/ToolButton.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ProcessInterface.h>

#include <pcl/Thread.h>
#include <pcl/Timer.h>

#include "ExternalToolInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

    class ExternalToolInterface : public ProcessInterface
    {
    public:

        ExternalToolInterface();
        virtual ~ExternalToolInterface();

        virtual IsoString Id() const;
        virtual MetaProcess* Process() const;
        virtual const char** IconImageXPM() const;

        virtual InterfaceFeatures Features() const;

        virtual void ApplyInstance() const;
        virtual void ResetInstance();

        virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

        virtual ProcessImplementation* NewProcess() const;

        virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
        virtual bool RequiresInstanceValidation() const;

        virtual bool ImportProcess( const ProcessImplementation& );

        virtual void RealTimePreviewUpdated	(bool active);
        virtual bool RequiresRealTimePreviewUpdate(const UInt16Image &, const View &, int /*zoomlevel*/) const;
        virtual bool GenerateRealTimePreview( UInt16Image&, const View&, int zoomLevel, String& info ) const;

    private:

        ExternalToolInstance m_instance;

        /// for generating realtime previews
        class RealTimeThread : public Thread
        {
        public:

            UInt16Image m_image;

            RealTimeThread();

            void Reset( const UInt16Image&, const ExternalToolInstance& );

            virtual void Run();

        private:

            ExternalToolInstance m_instance;
        };

        /// for generating realtime previews
        mutable RealTimeThread* m_realTimeThread;

        struct GUIData
        {
            GUIData( ExternalToolInterface& );

            VerticalSizer     Global_Sizer;
            Label Executable_Label;
            HorizontalSizer Executable_Sizer;
            Edit Executable_Edit;
            ToolButton Executable_ToolButton;
            ToolButton Help_ToolButton;
            HorizontalSizer Options_Sizer;
            Label Options_Label;
            Edit Options_Edit;

            SectionBar Destination_SectionBar;
            Control Destination_Control;
            VerticalSizer Destination_VerticalSizer;
            HorizontalSizer ReadImage_HorizontalSizer;
            CheckBox ReadImage_CheckBox;
            CheckBox NewWindow_CheckBox;
            HorizontalSizer ReadFits_HorizontalSizer;
            CheckBox ReadFitsKeys_CheckBox;
            CheckBox RescaleResult_CheckBox;
            SectionBar Parameters_SectionBar;
            Control Parameters_Control;
            VerticalSizer Parameters_VerticalSizer;
            HorizontalSizer Parameter1_HorizontalSizer;
            CheckBox Parameter1_CheckBox;
            NumericControl Parameter1_NumericControl;
            HorizontalSizer Parameter2_HorizontalSizer;
            CheckBox Parameter2_CheckBox;
            NumericControl Parameter2_NumericControl;
            HorizontalSizer Parameter3_HorizontalSizer;
            CheckBox Parameter3_CheckBox;
            NumericControl Parameter3_NumericControl;

            SectionBar Options_SectionBar;
            Control Options_Control;
            VerticalSizer Options_VerticalSizer;
            HorizontalSizer HelpOptions_Sizer;
            Label HelpOptions_Label;
            Edit HelpOptions_Edit;
            HorizontalSizer GlobalOptions_Sizer;
            Label GlobalOptions_Label;
            Edit GlobalOptions_Edit;
            HorizontalSizer SourceOptions_Sizer;
            Label SourceOptions_Label;
            Edit SourceOptions_Edit;
            HorizontalSizer ResultOptions_Sizer;
            Label ResultOptions_Label;
            Edit ResultOptions_Edit;

            Timer UpdateRealTimePreview_Timer;
        };

        GUIData* GUI;

        void UpdateControls();
        void UpdateRealTimePreview();

        // Event Handlers
        void __NumericEditValueUpdated( NumericEdit& sender, double value );
        void __ButtonClicked( Button& sender, bool checked );
        void __EditCompleted(Edit &sender);
        void __TextUpdated(Edit &sender,const String & string);
        void __SectionToggle( SectionBar& sender, Control& section, bool start );
        void __UpdateRealTimePreview_Timer( Timer& );

        friend struct GUIData;
    };

// ----------------------------------------------------------------------------

    PCL_BEGIN_LOCAL
    extern ExternalToolInterface* TheExternalToolInterface;
    PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ExternalToolInterface_h
