//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard AberrationInspector Process Module Version 01.01.02.0208
// ----------------------------------------------------------------------------
// AberrationInspectorProcess.cpp - Released 2016/02/29 00:00:00 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard AberrationInspector PixInsight module.
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

#include "AberrationInspectorProcess.h"
#include "AberrationInspectorParameters.h"
#include "AberrationInspectorInterface.h"

#include <pcl/Console.h>
#include <pcl/Arguments.h>
#include <pcl/View.h>
#include <pcl/Exception.h>

namespace pcl
{

	// ----------------------------------------------------------------------------

#include "AberrationInspectorIcon.xpm"

	// ----------------------------------------------------------------------------

	AberrationInspectorProcess* TheAberrationInspectorProcess = 0;

	// ----------------------------------------------------------------------------

	AberrationInspectorProcess::AberrationInspectorProcess() : MetaProcess()
	{
		TheAberrationInspectorProcess = this;

		// Instantiate process parameters
		new MosaicSize(this);
		new PanelSize(this);
		new Separation(this);
		new Brightness(this);
	}

	// ----------------------------------------------------------------------------

	IsoString AberrationInspectorProcess::Id() const
	{
		return "AberrationInspector";
	}

	// ----------------------------------------------------------------------------

	IsoString AberrationInspectorProcess::Category() const
	{
		return "ImageInspection";
	}

	// ----------------------------------------------------------------------------

	uint32 AberrationInspectorProcess::Version() const
	{
		return 0x100; // required
	}

	// ----------------------------------------------------------------------------

	String AberrationInspectorProcess::Description() const
	{
		return

			"<html>"
			"<p>AberrationInspector copy corners and central parts to new image. The module "
			"show how much aberation in your optics.</p>"
			"</html>";
	}

	// ----------------------------------------------------------------------------

	const char** AberrationInspectorProcess::IconImageXPM() const
	{
		return AberrationInspectorIcon_XPM; //---> put a nice icon here
	}
	// ----------------------------------------------------------------------------

	ProcessInterface* AberrationInspectorProcess::DefaultInterface() const
	{
		return TheInterface;
	}
	// ----------------------------------------------------------------------------

	ProcessImplementation* AberrationInspectorProcess::Create() const
	{
		return new AberrationInspectorInstance(this);
	}

	// ----------------------------------------------------------------------------

	ProcessImplementation* AberrationInspectorProcess::Clone(const ProcessImplementation& p) const
	{
		const AberrationInspectorInstance* instPtr = dynamic_cast<const AberrationInspectorInstance*>(&p);
		return (instPtr != 0) ? new AberrationInspectorInstance(*instPtr) : 0;
	}

	// ----------------------------------------------------------------------------

	bool AberrationInspectorProcess::CanProcessCommandLines() const
	{
		return true;
	}

	// ----------------------------------------------------------------------------

	inline void ShowHelp()
	{
		Console().Write(
			"<raw>"
			"Usage: AberrationInspector [<arg_list>] [<view_list>]"
			"\n"
			"\n--interface"
			"\n"
			"\n      Launches the interface of this process."
			"\n"
			"\n--help"
			"\n"
			"\n      Displays this help and exit."
			"\n"
			"\n*"
			"\n      Porcess all opened Images with default setings and exit."
			//"\n      Porcess all opened Images with curent setings and exit."
			"</raw>");
	}

	int AberrationInspectorProcess::ProcessCommandLine(const StringList& argv) const
	{
		ArgumentList arguments = ExtractArguments(argv, ArgumentItemMode::AsViews, ArgumentOption::AllowWildcards);

		AberrationInspectorInstance instance(this); //set default parameters.

		bool launchInterface = false;
		int count = 0;

		for (ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i)
		{
			const Argument& arg = *i;

			if (arg.IsLiteral()) // arguments start from '-' example: -abc --abc -123
			{
				// These are standard parameters that all processes should provide.
				if (arg.Id() == "-interface")
					launchInterface = true;
				else if (arg.Id() == "-i")
					launchInterface = true;
				else if (arg.Id() == "-help")
				{
					ShowHelp();
					return 0;
				}
				else
					throw Error("Unknown argument: " + arg.Token());
			}
			else if (arg.IsItemList()) // example: * _abc _123 abc 123
			{
				++count;

				if (arg.Items().IsEmpty())
				{
					throw Error("No view(s) found: " + arg.Token());
				}

				for (StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j)
				{
					View v = View::ViewById(*j);
					if (v.IsNull())
						throw Error("No such view: " + *j);
					instance.LaunchOn(v);
				}
			}
			else
			{		//IsNumeric, IsString, IsSwitch - arguments not allowed
				throw Error("Unknown argument: " + arg.Token());
			}
		}

		if (launchInterface)
			instance.LaunchInterface();

		else if (count == 0)
		{
			if (ImageWindow::ActiveWindow().IsNull())
				throw Error("There is no active image window.");
			instance.LaunchOnCurrentView();
		}
		return 0;
	}

	// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AberrationInspectorProcess.cpp - Released 2016/02/29 00:00:00 UTC
