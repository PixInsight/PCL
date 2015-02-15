// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// ExternalToolParameters.cpp - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the ExternalTool PixInsight module.
//
// Copyright (c) 20014-2015, Georg Viehoever. All Rights Reserved.
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

#include <pcl/MetaProcess.h>
#include "ExternalToolParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

	ExternalToolParamSExecutable*TheExternalToolParamSExecutable =0;
	ExternalToolParamSOptions *TheExternalToolParamSOptions =0;
	ExternalToolParamSHelpOptions * TheExternalToolParamSHelpOptions=0;
	ExternalToolParamSGlobalOptions * TheExternalToolParamSGlobalOptions=0;
	ExternalToolParamSSourceOptions * TheExternalToolParamSSourceOptions=0;
	ExternalToolParamSResultOptions *TheExternalToolParamSResultOptions =0;
	ExternalToolParamBReadImage * TheExternalToolParamBReadImage=0;
	ExternalToolParamBReadFitsKeys * TheExternalToolParamBReadFitsKeys=0;
	ExternalToolParamBNewWindow * TheExternalToolParamBNewWindow=0;
	ExternalToolParamBRescaleResult * TheExternalToolParamBRescaleResult=0;

	ExternalToolParamBool1* TheExternalToolParamBool1=0;
	ExternalToolParamBool2* TheExternalToolParamBool2=0;
	ExternalToolParamBool3* TheExternalToolParamBool3=0;
	ExternalToolParamDouble1 *TheExternalToolParamDouble1 =0;
	ExternalToolParamDouble2 *TheExternalToolParamDouble2 =0;
	ExternalToolParamDouble3 *TheExternalToolParamDouble3 =0;

	ExternalToolParamNExitCode *TheExternalToolParamNExitCode=0;
	ExternalToolParamSStdOut *TheExternalToolParamSStdOut=0;
	ExternalToolParamSStdErr *TheExternalToolParamSStdErr=0;


// ----------------------------------------------------------------------------

	ExternalToolParamString::ExternalToolParamString( MetaProcess* P )
			: MetaString( P )
	{
	}
	size_type ExternalToolParamString::MinLength() const
	{
		return 0;
	}

	ExternalToolParamSExecutable::ExternalToolParamSExecutable( MetaProcess* P )
			:ExternalToolParamString( P )
	{
		TheExternalToolParamSExecutable = this;
	}
	IsoString ExternalToolParamSExecutable::Id() const
	{
		return "sBinPath";
	}

	ExternalToolParamSOptions::ExternalToolParamSOptions( MetaProcess* P )
			:ExternalToolParamString( P )
	{
		TheExternalToolParamSOptions = this;
	}
	IsoString ExternalToolParamSOptions::Id() const
	{
		return "sOptions";
	}


	ExternalToolParamSHelpOptions::ExternalToolParamSHelpOptions(MetaProcess *process)
			:ExternalToolParamString(process)
	{
		TheExternalToolParamSHelpOptions=this;
	}
	IsoString ExternalToolParamSHelpOptions::Id() const
	{
		return "sHelpOptions";
	}
	String ExternalToolParamSHelpOptions::DefaultValue() const
	{
		return "-h";
	}

	ExternalToolParamSGlobalOptions::ExternalToolParamSGlobalOptions(MetaProcess *process)
			:ExternalToolParamString(process)
	{
		TheExternalToolParamSGlobalOptions=this;
	}
	IsoString ExternalToolParamSGlobalOptions::Id() const
	{
		return "sGlobalOptions";
	}
	String ExternalToolParamSGlobalOptions::DefaultValue() const
	{
		return "-g";
	}

	ExternalToolParamSSourceOptions::ExternalToolParamSSourceOptions(MetaProcess *process)
			:ExternalToolParamString(process)
	{
		TheExternalToolParamSSourceOptions=this;
	}
	IsoString ExternalToolParamSSourceOptions::Id() const
	{
		return "sSourceOptions";
	}
	String ExternalToolParamSSourceOptions::DefaultValue() const
	{
		return "-f";
	}

	ExternalToolParamSResultOptions::ExternalToolParamSResultOptions(MetaProcess *process)
			:ExternalToolParamString(process)
	{
		TheExternalToolParamSResultOptions =this;
	}
	IsoString ExternalToolParamSResultOptions::Id() const
	{
		return "sResultOptions";
	}
	String ExternalToolParamSResultOptions::DefaultValue() const
	{
		return "-o";
	}


	ExternalToolParamBReadImage::ExternalToolParamBReadImage( MetaProcess* P )
		: MetaBoolean( P )
	{
		TheExternalToolParamBReadImage = this;
	}
	IsoString ExternalToolParamBReadImage::Id() const
	{
		return "bReadImage";
	}
	bool ExternalToolParamBReadImage::DefaultValue() const
	{
		return true;
	}

	ExternalToolParamBReadFitsKeys::ExternalToolParamBReadFitsKeys( MetaProcess* P )
			: MetaBoolean( P )
	{
		TheExternalToolParamBReadFitsKeys = this;
	}
	IsoString ExternalToolParamBReadFitsKeys::Id() const
	{
		return "bReadFitsKeys";
	}
	bool ExternalToolParamBReadFitsKeys::DefaultValue() const
	{
		return true;
	}

	ExternalToolParamBNewWindow::ExternalToolParamBNewWindow( MetaProcess* P )
			: MetaBoolean( P )
	{
		TheExternalToolParamBNewWindow = this;
	}
	IsoString ExternalToolParamBNewWindow::Id() const
	{
		return "bNewWindow";
	}
	bool ExternalToolParamBNewWindow::DefaultValue() const
	{
		return false;
	}

	ExternalToolParamBRescaleResult::ExternalToolParamBRescaleResult( MetaProcess* P )
			: MetaBoolean( P )
	{
		TheExternalToolParamBRescaleResult = this;
	}
	IsoString ExternalToolParamBRescaleResult::Id() const
	{
		return "bRescaleResult";
	}
	bool ExternalToolParamBRescaleResult::DefaultValue() const
	{
		return false;
	}

// ----------------------------------------------------------------------------

	ExternalToolParamBool::ExternalToolParamBool( MetaProcess* P )
			:MetaBoolean( P )
	{
	}
	bool ExternalToolParamBool::DefaultValue() const
	{
		return false;
	}

	ExternalToolParamBool1::ExternalToolParamBool1( MetaProcess *P)
			:ExternalToolParamBool(P)
	{
		TheExternalToolParamBool1=this;
	}
	IsoString ExternalToolParamBool1::Id() const
	{
		return "bParam1";
	}

	ExternalToolParamBool2::ExternalToolParamBool2( MetaProcess *P)
	:ExternalToolParamBool(P)
			{
					TheExternalToolParamBool2=this;
			}
	IsoString ExternalToolParamBool2::Id() const
	{
		return "bParam2";
	}

	ExternalToolParamBool3::ExternalToolParamBool3( MetaProcess *P)
	:ExternalToolParamBool(P)
			{
					TheExternalToolParamBool3=this;
			}
	IsoString ExternalToolParamBool3::Id() const
	{
		return "bParam3";
	}

// ----------------------------------------------------------------------------

	ExternalToolParamDouble::ExternalToolParamDouble( MetaProcess* P )
			: MetaDouble( P )
	{
	}
	int ExternalToolParamDouble::Precision() const
	{
		return 3;
	}
	double ExternalToolParamDouble::DefaultValue() const
	{
		return 0.0;
	}
	double ExternalToolParamDouble::MinimumValue() const
	{
		return 0.0;
	}
	double ExternalToolParamDouble::MaximumValue() const
	{
		return 1.0;
	}

	ExternalToolParamDouble1::ExternalToolParamDouble1( MetaProcess* P )
			: ExternalToolParamDouble( P )
	{
		TheExternalToolParamDouble1 = this;
	}
	IsoString ExternalToolParamDouble1::Id() const
	{
		return "dParam1";
	}

	ExternalToolParamDouble2::ExternalToolParamDouble2( MetaProcess* P )
			: ExternalToolParamDouble( P )
	{
		TheExternalToolParamDouble2 = this;
	}
	IsoString ExternalToolParamDouble2::Id() const
	{
		return "dParam2";
	}

	ExternalToolParamDouble3::ExternalToolParamDouble3( MetaProcess* P )
			: ExternalToolParamDouble( P )
	{
		TheExternalToolParamDouble3 = this;
	}
	IsoString ExternalToolParamDouble3::Id() const
	{
		return "dParam3";
	}

	// ----------------------------------------------------------------------------

	ExternalToolParamNExitCode::ExternalToolParamNExitCode( MetaProcess *P)
			:MetaInt32(P)
	{
		TheExternalToolParamNExitCode = this;
	}
	IsoString ExternalToolParamNExitCode::Id() const
	{
		return "nExitCode";
	}
	bool ExternalToolParamNExitCode::IsReadOnly() const
	{
		return true;
	};

	ExternalToolParamSStdOut::ExternalToolParamSStdOut( MetaProcess *P)
			:ExternalToolParamString(P) {
		TheExternalToolParamSStdOut = this;
	}
	IsoString ExternalToolParamSStdOut::Id() const {
		return "sStdOut";
	}
	bool ExternalToolParamSStdOut::IsReadOnly() const
	{
		return true;
	};

	ExternalToolParamSStdErr::ExternalToolParamSStdErr( MetaProcess *P)
			:ExternalToolParamString(P) {
		TheExternalToolParamSStdErr = this;
	}
	IsoString ExternalToolParamSStdErr::Id() const {
		return "sStdErr";
	}
	bool ExternalToolParamSStdErr::IsReadOnly() const
	{
		return true;
	};

} // pcl

// ****************************************************************************
