//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0394
// ----------------------------------------------------------------------------
// ImageIdentifierProcess.cpp - Released 2017-07-18T16:14:18Z
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

#include "ImageIdentifierProcess.h"
#include "ImageIdentifierParameters.h"
#include "ImageIdentifierInstance.h"
#include "ImageIdentifierInterface.h"

#include <pcl/Arguments.h>
#include <pcl/ImageWindow.h>
#include <pcl/View.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIdentifierProcess* TheImageIdentifierProcess = 0;

// ----------------------------------------------------------------------------

#include "ImageIdentifierIcon.xpm"

// ----------------------------------------------------------------------------

ImageIdentifierProcess::ImageIdentifierProcess() : MetaProcess()
{
   TheImageIdentifierProcess = this;

   // Instantiate process parameter
   new ImageIdentifier( this );
}

// ----------------------------------------------------------------------------

IsoString ImageIdentifierProcess::Id() const
{
   return "ImageIdentifier";
}

// ----------------------------------------------------------------------------

IsoString ImageIdentifierProcess::Category() const
{
   return "Image";
}

// ----------------------------------------------------------------------------

uint32 ImageIdentifierProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ImageIdentifierProcess::Description() const
{
   return

   "<html>"
   "<p>ImageIdentifier is the standard process to specify image identifiers in PixInsight. "
   "A valid PixInsight image identifier follows the general C identifier syntax rules: "
   "it may contain any combination of alphanumeric characters, starting with an alphabetic "
   "or underscore character. In formal terms this can be described as follows:</p>"

   "<code><pre>"
   "&lt;identifier&gt;: &lt;alphabetic&gt;|&lt;underscore&gt;[&lt;identifier_element&gt;[&lt;identifier&gt;]]\n\n"

   "&lt;alphabetic&gt;: one of: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n\n"

   "&lt;underscore&gt;: _\n\n"

   "&lt;identifier_element&gt;: &lt;alphabetic&gt;|&lt;underscore&gt;|&lt;decimal_digit&gt;\n\n"

   "&lt;decimal_digit&gt;: one of: 0123456789\n\n"
   "</pre></code>"

   "<p>PixInsight identifiers are case-sensitive ('AnyIdentifier' and 'anyidentifier' are two "
   "distinct, valid identifiers), and they must be unique in their naming context: there cannot "
   "be two images or two previews of the same image with the same identifier.</p>"

   "<p>There is no specific length limit for identifiers in PixInsight. You should always use "
   "meaningful identifiers of reasonable length. For example, 'M31_core' and 'SaturationTest_1' "
   "are probably two good choices as identifiers of images or previews.</p>"
   "</html>";
}

// ----------------------------------------------------------------------------

const char** ImageIdentifierProcess::IconImageXPM() const
{
   return ImageIdentifierIcon_XPM;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierProcess::NeedsValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

ProcessInterface* ImageIdentifierProcess::DefaultInterface() const
{
   return TheImageIdentifierInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ImageIdentifierProcess::Create() const
{
   return new ImageIdentifierInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ImageIdentifierProcess::Clone( const ProcessImplementation& p ) const
{
   const ImageIdentifierInstance* instPtr = dynamic_cast<const ImageIdentifierInstance*>( &p );
   return (instPtr != 0) ? new ImageIdentifierInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

bool ImageIdentifierProcess::CanProcessCommandLines() const
{
   return true;
}

// ----------------------------------------------------------------------------

static void ShowHelp()
{
   Console().Write(
"<raw>"
"Usage: ImageIdentifier [-id=<view_id>] [<view_list>]"
"\n"
"\n<view_id>"
"\n"
"\n      Is a valid view identifier to rename the target view(s)."
"\n"
"\n--interface"
"\n"
"\n      Launches the interface of this process."
"\n"
"\n--help"
"\n"
"\n      Displays this help and exits."
"</raw>" );
}

int ImageIdentifierProcess::ProcessCommandLine( const StringList& argv ) const
{
   ArgumentList arguments =
   ExtractArguments( argv, ArgumentItemMode::AsViews,
                     ArgumentOption::AllowWildcards|ArgumentOption::NoPreviews );

   ImageIdentifierInstance instance( this );

   bool launchInterface = false;
   int count = 0;

   for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
   {
      const Argument& arg = *i;

      if ( arg.IsNumeric() )
         throw Error( "Unknown numeric argument: " + arg.Token() );
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "id" )
         {
            instance.SetId( arg.StringValue() );
            if ( !instance.Id().IsValidIdentifier() )
               throw Error( "Invalid image identifier: " + arg.Token() );
         }
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
         throw Error( "Unknown switch argument: " + arg.Token() );
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "-interface" )
            launchInterface = true;
         else if ( arg.Id() == "-help" )
         {
            ShowHelp();
            return 0;
         }
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         ++count;

         if ( arg.Items().IsEmpty() )
         {
            Console().WriteLn( "No view(s) found: " + arg.Token() );
            continue;
         }

         for ( StringList::const_iterator j = arg.Items().Begin(); j != arg.Items().End(); ++j )
         {
            View v = View::ViewById( *j );
            if ( v.IsNull() )
               throw Error( "No such view: " + *j );
            instance.LaunchOn( v );
         }
      }
   }

   if ( launchInterface )
      instance.LaunchInterface();
   else if ( count == 0 )
   {
      if ( ImageWindow::ActiveWindow().IsNull() )
         throw Error( "There is no active image window." );
      instance.LaunchOnCurrentWindow();
   }

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ImageIdentifierProcess.cpp - Released 2017-07-18T16:14:18Z
