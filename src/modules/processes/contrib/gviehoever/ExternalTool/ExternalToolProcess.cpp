// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// ExternalToolProcess.cpp - Released 2014/11/14 17:19:24 UTC
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

#include "ExternalToolInstance.h"
#include "ExternalToolInterface.h"
#include "ExternalToolParameters.h"
#include "ExternalToolProcess.h"
#include "ExternalToolIcon.xpm"

#include <pcl/Arguments.h>
#include <pcl/Console.h>
#include <pcl/Exception.h>
#include <pcl/View.h>


namespace pcl
{


// ----------------------------------------------------------------------------

    ExternalToolProcess* TheExternalToolProcess = 0;

// ----------------------------------------------------------------------------

    ExternalToolProcess::ExternalToolProcess() : MetaProcess()
    {
        TheExternalToolProcess = this;

        // Instantiate process parameters
        new ExternalToolParamSExecutable( this );
        new ExternalToolParamSOptions( this );
        new ExternalToolParamSGlobalOptions( this );
        new ExternalToolParamSHelpOptions( this );
        new ExternalToolParamSSourceOptions( this );
        new ExternalToolParamSResultOptions( this );

        new ExternalToolParamBReadImage( this );
        new ExternalToolParamBReadFitsKeys( this );
        new ExternalToolParamBNewWindow( this );
        new ExternalToolParamBRescaleResult( this );

        new ExternalToolParamBool1(this);
        new ExternalToolParamBool2(this);
        new ExternalToolParamBool3(this);
        new ExternalToolParamDouble1(this);
        new ExternalToolParamDouble2(this);
        new ExternalToolParamDouble3(this);

        new ExternalToolParamNExitCode(this);
        new ExternalToolParamSStdOut(this);
        new ExternalToolParamSStdErr(this);
    }

// ----------------------------------------------------------------------------

    IsoString ExternalToolProcess::Id() const
    {
        return "ExternalTool";
    }

// ----------------------------------------------------------------------------

    IsoString ExternalToolProcess::Category() const
    {
        return "Image";
    }

// ----------------------------------------------------------------------------

    uint32 ExternalToolProcess::Version() const
    {
        return 0x100; // required
    }

// ----------------------------------------------------------------------------

    String ExternalToolProcess::Description() const
    {
        return
                "<html><p>ExternalTool applies an external executable to the current image, or generates a new image on global execution.</p></html>";
    }

// ----------------------------------------------------------------------------

    const char** ExternalToolProcess::IconImageXPM() const
    {
        return ExternalToolIcon_XPM;
    }
// ----------------------------------------------------------------------------

    ProcessInterface* ExternalToolProcess::DefaultInterface() const
    {
        return TheExternalToolInterface;
    }
// ----------------------------------------------------------------------------

    ProcessImplementation* ExternalToolProcess::Create() const
    {
        return new ExternalToolInstance( this );
    }

// ----------------------------------------------------------------------------

    ProcessImplementation* ExternalToolProcess::Clone( const ProcessImplementation& p ) const
    {
        const ExternalToolInstance* instPtr = dynamic_cast<const ExternalToolInstance*>( &p );
        return (instPtr != 0) ? new ExternalToolInstance( *instPtr ) : 0;
    }

// ----------------------------------------------------------------------------

    bool ExternalToolProcess::CanProcessCommandLines() const
    {
        return true;
    }

// ----------------------------------------------------------------------------

    static void ShowHelp()
    {
        //FIXME
        Console().Write(
                "<raw>"
                        "Usage: ExternalTool [options] pathToBin [binOptions]"
                        "\n  Will call an external process pathToBin with a FITS file containing the current view, and a file name for a new image."
                        "\n  Expects the external process to create a new image with the given data. After call, will clean up files."
                        "\n  Not being able to launch pathToBin, not creating a result image (except with option -nr) or terminating with code!=0 "
                        "\n  is considered an error."
                        "\n  The target image (=source) is replaced with the result image, or a new windows is created (option -w). If possible (only if result "
                        "\n  goes into main view), the data type is adapted to what is found the result image. Also only for main views, FITS keys and "
                        "\n  ICC profile are also taken from the result file (disabled with option -nf)."
                        "\n  The external process is called with"
                        "\n      pathToBin [helpOpts] [globalOpts] [sourceOpts sourceFileName] [resultOpts resultFileName] binOptions"
                        "\n  and is expected to produce a result image named resultFile from the FITS image contained in sourceFileName. Note that the"
                        "\n  set of options passed depends on -g, -gh, -nr."
                        "\n"
                        "\n-v=[viewId] | --view=[viewId]"
                        "\n      id of existing view. Defaults to current view. Identifies the view that will be fed"
                        "\n      to the process and that, in the local case, will be updated with the result."
                        "\n-nr| --noRead"
                        "\n      do not read result image. If one is produced, it is ignored. It is no error if no image is produced."
                        "\n-w | --window"
                        "\n      result image is displayed in new image window instead of current view. Implied by -g."
                        "\n-r | --rescale"
                        "\n      rescale image to native range of image data type. Without this option, the value range is "
                        "\n      truncated if necessary."
                        "\n-nf | --noFitsKeys"
                        "\n      do not read FITS keys and ICC profile from result image. Without this option, FITS keys and"
                        "\n      ICC profile are read from the result image and inserted into the target image, if the target is a main view."
                        "\n-g | --global"
                        "\n      Does a global apply. The image created by the external process is loaded into a new window."
                        "\n      If not --global is not given, the result image replaces the current view."
                        "\n-gh | --getHelp"
                        "\n      Call external process with helpOpts, and display the resulting text output on stdOut."
                        "\n-go=\"globalOpts\" | --globalOpts=\"globalOpts\""
                        "\n      The options prepended to the binOptions to trigger a global computation with -g. Default \"-g\"."
                        "\n-so=\"sourceOpts\" | --sourceOpts=\"sourceOpts\""
                        "\n      The options prepended to binOptions to identify the source image file name. Default \"-f\"."
                        "\n-ro=\"resultOpts\" | --resultOpts=\"resultOpts\""
                        "\n      The options preprended to binOptions to identify the result image file name. Default \"-o\"."
                        "\n-ho=\"helpOpts\" | --helpOpts=\"helpOpts\""
                        "\n      The options prepended to the binOptions to get a help message from the external program with -gh. Default \"-h\"."
                        "\n-i | --interface"
                        "\n      Launches the interface of this process and ignores -g and -v."
                        "\n-h | --help"
                        "\n      Displays this help and exits."
                        "</raw>" );
    }

    int ExternalToolProcess::ProcessCommandLine( const StringList& argv ) const
    {
        ArgumentList arguments =
                ExtractArguments( argv, ArgumentItemMode::Ignore );

        ExternalToolInstance instance( this );

        bool bLaunchInterface = false;
        String sExecutable; //the name of the executable
        String sOptions;  //the options passed to the executable
        String sViewId;
        bool bGlobal=false;
        bool bHelp=false;
        for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
        {
            const Argument& arg = *i;

            if ( arg.IsSwitch() )
            {
                throw Error( "Unknown switch argument: " + arg.Token() );
            }
            else if ( arg.IsLiteral() ) {
                if (arg.Id() == "-global" || arg.Id() == "g") {
                    bGlobal = true;
                    instance.m_bNewWindow = true;
                } else if (arg.Id() == "-noRead" || arg.Id() == "nr") {
                    instance.m_bReadImage = false;
                } else if (arg.Id() == "-noFitsKeys" || arg.Id() == "nf") {
                    instance.m_bReadFitsKeys = false;
                } else if (arg.Id() == "-rescale" || arg.Id() == "r") {
                    instance.m_bRescaleResult=true;
                } else if (arg.Id() == "-getHelp" || arg.Id() == "gh") {
                    bHelp=true;
                } // These are standard parameters that all processes should provide.
                else if(arg.Id() == "-interface"){
                    bLaunchInterface = true;
                }
                else if (arg.Id() == "-help" || arg.Id() == "h") {
                    ShowHelp();
                    return 0;
                } else {
                    throw Error("Unknown literal argument: " + arg.Token());
                }
            }
            else if(arg.IsString()) {
                if(arg.Id()=="-view"|| arg.Id()=="v")
                {
                    sViewId=arg.StringValue();
                } else if (arg.Id()=="-globalOpts" ||arg.Id()=="go") { //so,to,ho,po
                    instance.m_sGlobalOptions = arg.StringValue();
                } else if (arg.Id()=="-sourceOpts" ||arg.Id()=="so") {
                    instance.m_sSourceOptions = arg.StringValue();
                } else if (arg.Id()=="-resultOpts" ||arg.Id()=="ro"){
                    instance.m_sResultOptions = arg.StringValue();
                } else if (arg.Id()=="-helpOpts" ||arg.Id()=="ho"){
                    instance.m_sHelpOptions=arg.StringValue();
                } else {
                    throw Error("Unknow string argument:"+arg.Token());
                }
            }
            else if ( arg.IsItemList() )
            {
                for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
                    if(sExecutable=="")
                    {
                        // this is the first "non-parametric" argument, expected to be the path to the external executable
                        sExecutable=*j;
                    } else {
                        // these are options for the executable
                        if(sOptions!=""){
                            sOptions+=" ";
                        }
                        sOptions+=*j;
                    }
            }
        }
        instance.m_sExecutable=sExecutable;
        instance.m_sOptions=sOptions;

        if ( bLaunchInterface ) {
            // launch UI
            instance.LaunchInterface();
        } else if(bHelp) {
            // help
            instance.ExecuteHelp();
        } else if(bGlobal){
            // global apply
            instance.LaunchGlobal();
        } else if(sViewId=="") {
            // local apply on current view
            if (ImageWindow::ActiveWindow().IsNull()) {
                throw Error("There is no active image window.");
            }
            instance.LaunchOnCurrentView();
        } else {
            // local apply on given view
            View v = View::ViewById( sViewId );
            instance.LaunchOn(v);
        }
        // FIXME should we return the value provided by the Launch() calls above?
        return 0;
    }

    bool ExternalToolProcess::CanBrowseDocumentation() const
    {
        return false;
    }

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
