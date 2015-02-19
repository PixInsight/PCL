// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// ExternalToolParameters.h - Released 2014/11/14 17:19:24 UTC
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

#ifndef __ExternalToolParameters_h
#define __ExternalToolParameters_h

#include <pcl/MetaParameter.h>

namespace pcl
{

    PCL_BEGIN_LOCAL

// ----------------------------------------------------------------------------

    //
    // executable path and command line options
    //

    ///string with MinLength==1 and default=""
    class ExternalToolParamString : public MetaString
    {
    public:
       ExternalToolParamString( MetaProcess* );
       virtual size_type MinLength() const;
    };

    /// Path to executable
    class ExternalToolParamSExecutable : public ExternalToolParamString
    {
    public:
       ExternalToolParamSExecutable( MetaProcess* );
       virtual IsoString Id() const;
    };
    extern ExternalToolParamSExecutable*TheExternalToolParamSExecutable;

    /// options for executable
    class ExternalToolParamSOptions : public ExternalToolParamString
    {
    public:
       ExternalToolParamSOptions( MetaProcess* );
       virtual IsoString Id() const;
    };
    extern ExternalToolParamSOptions *TheExternalToolParamSOptions;


    /// options for help
    class ExternalToolParamSHelpOptions : public ExternalToolParamString
    {
    public:
       ExternalToolParamSHelpOptions( MetaProcess* );
       virtual IsoString Id() const;
       /// -h
       virtual String DefaultValue() const;
    };
    extern ExternalToolParamSHelpOptions * TheExternalToolParamSHelpOptions;

    /// options for global execution
    class ExternalToolParamSGlobalOptions : public ExternalToolParamString
    {
    public:
       ExternalToolParamSGlobalOptions( MetaProcess* );
       virtual IsoString Id() const;
       /// -g
       virtual String DefaultValue() const;
    };
    extern ExternalToolParamSGlobalOptions * TheExternalToolParamSGlobalOptions;

    /// options for source file name
    class ExternalToolParamSSourceOptions : public ExternalToolParamString
    {
    public:
       ExternalToolParamSSourceOptions( MetaProcess* );
       virtual IsoString Id() const;
       /// -f
       virtual String DefaultValue() const;
    };
    extern ExternalToolParamSSourceOptions * TheExternalToolParamSSourceOptions;

    /// options for result file name
    class ExternalToolParamSResultOptions : public ExternalToolParamString
    {
    public:
       ExternalToolParamSResultOptions( MetaProcess* );
       virtual IsoString Id() const;
       /// -o
       virtual String DefaultValue() const;
    };
    extern ExternalToolParamSResultOptions *TheExternalToolParamSResultOptions;

    /// indicate if we expect process to produce image
    class ExternalToolParamBReadImage : public MetaBoolean
    {
    public:
        ExternalToolParamBReadImage( MetaProcess *);
        virtual bool DefaultValue() const ;
        virtual IsoString Id() const;
    };
    extern ExternalToolParamBReadImage * TheExternalToolParamBReadImage;

    /// indicate if we use the FITS Keywords that we find the in read file
    class ExternalToolParamBReadFitsKeys : public MetaBoolean
    {
    public:
        ExternalToolParamBReadFitsKeys( MetaProcess *);
        virtual bool DefaultValue() const ;
        virtual IsoString Id() const;
    };
    extern ExternalToolParamBReadFitsKeys * TheExternalToolParamBReadFitsKeys;

    /// indicate if we expect result being integrated into Apply() view, or if
    /// it should get a new Window
    class ExternalToolParamBNewWindow : public MetaBoolean
    {
    public:
        ExternalToolParamBNewWindow( MetaProcess *);
        virtual bool DefaultValue() const ;
        virtual IsoString Id() const;
    };
    extern ExternalToolParamBNewWindow * TheExternalToolParamBNewWindow;

    /// If true, rescale result to range [0,1]. Otherwise, clip result (if necessary)
    class ExternalToolParamBRescaleResult : public MetaBoolean
    {
    public:
        ExternalToolParamBRescaleResult( MetaProcess *);
        virtual bool DefaultValue() const ;
        virtual IsoString Id() const;
    };
    extern ExternalToolParamBRescaleResult * TheExternalToolParamBRescaleResult;
    //
    // additional parameters
    //

    /// Bool with default false
    class ExternalToolParamBool : public MetaBoolean
    {
    public:
       ExternalToolParamBool( MetaProcess* );
       virtual bool DefaultValue() const;
    };

    class ExternalToolParamBool1 : public ExternalToolParamBool
    {
    public:
       ExternalToolParamBool1( MetaProcess* );
       virtual IsoString Id() const;
    };

    class ExternalToolParamBool2 : public ExternalToolParamBool
    {
    public:
       ExternalToolParamBool2( MetaProcess* );
       virtual IsoString Id() const;
    };
    class ExternalToolParamBool3 : public ExternalToolParamBool
    {
    public:
       ExternalToolParamBool3( MetaProcess* );
       virtual IsoString Id() const;
    };

    extern ExternalToolParamBool1* TheExternalToolParamBool1;
    extern ExternalToolParamBool2* TheExternalToolParamBool2;
    extern ExternalToolParamBool3* TheExternalToolParamBool3;

    /// value of double parameter. Only valid if corresponding Bool is true
    class ExternalToolParamDouble : public MetaDouble
    {
    public:
       ExternalToolParamDouble( MetaProcess* );
       virtual int Precision() const;
       virtual double DefaultValue() const;
       virtual double MinimumValue() const;
       virtual double MaximumValue() const;
    };

    class ExternalToolParamDouble1 : public ExternalToolParamDouble
    {
    public:
       ExternalToolParamDouble1( MetaProcess* );
       virtual IsoString Id() const;
    };
    class ExternalToolParamDouble2 : public ExternalToolParamDouble
    {
    public:
       ExternalToolParamDouble2( MetaProcess* );
       virtual IsoString Id() const;
    };
    class ExternalToolParamDouble3 : public ExternalToolParamDouble
    {
    public:
       ExternalToolParamDouble3( MetaProcess* );
       virtual IsoString Id() const;
    };

    extern ExternalToolParamDouble1 *TheExternalToolParamDouble1;
    extern ExternalToolParamDouble2 *TheExternalToolParamDouble2;
    extern ExternalToolParamDouble3 *TheExternalToolParamDouble3;

    //
    // switches
    //

    //
    // results
    //
    /// Exit code of process
    class ExternalToolParamNExitCode : public MetaInt32
    {
    public:
        ExternalToolParamNExitCode( MetaProcess *);
        virtual IsoString Id() const;
        virtual bool IsReadOnly() const;
    };

    /// contents of stdOut
    class ExternalToolParamSStdOut : public ExternalToolParamString
    {
    public:
        ExternalToolParamSStdOut( MetaProcess *);
        virtual IsoString Id() const;
        virtual bool IsReadOnly() const;
    };

    /// contents of stdErr
    class ExternalToolParamSStdErr : public ExternalToolParamString
    {
    public:
        ExternalToolParamSStdErr( MetaProcess *);
        virtual IsoString Id() const;
        virtual bool IsReadOnly() const;
    };

    extern ExternalToolParamNExitCode *TheExternalToolParamNExitCode;
    extern ExternalToolParamSStdOut *TheExternalToolParamSStdOut;
    extern ExternalToolParamSStdErr *TheExternalToolParamSStdErr;

// ----------------------------------------------------------------------------
    PCL_END_LOCAL

} // pcl

#endif   // __ExternalToolParameters_h

// ****************************************************************************
