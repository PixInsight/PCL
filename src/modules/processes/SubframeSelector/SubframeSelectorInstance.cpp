//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#include "SubframeSelectorInstance.h"
#include "SubframeSelectorParameters.h"

#include <pcl/Console.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorInstance::SubframeSelectorInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   subframes(),
   subframeScale( TheSSSubframeScaleParameter->DefaultValue() ),
   cameraGain( TheSSCameraGainParameter->DefaultValue() ),
   cameraResolution( SSCameraResolution::Default ),
   siteLocalMidnight( TheSSSiteLocalMidnightParameter->DefaultValue() ),
   scaleUnit( SSScaleUnit::Default ),
   dataUnit( SSDataUnit::Default )
{
}

SubframeSelectorInstance::SubframeSelectorInstance( const SubframeSelectorInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void SubframeSelectorInstance::Assign( const ProcessImplementation& p )
{
   const SubframeSelectorInstance* x = dynamic_cast<const SubframeSelectorInstance*>( &p );
   if ( x != nullptr )
   {
      subframes         = x->subframes;
      subframeScale     = x->subframeScale;
      cameraGain        = x->cameraGain;
      cameraResolution  = x->cameraResolution;
      siteLocalMidnight = x->siteLocalMidnight;
      scaleUnit         = x->scaleUnit;
      dataUnit          = x->dataUnit;
   }
}

bool SubframeSelectorInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
    whyNot = "SubframeSelector can only be executed in the global context.";
    return false;
}

bool SubframeSelectorInstance::IsHistoryUpdater( const View& view ) const
{
    return false;
}

bool SubframeSelectorInstance::CanExecuteGlobal( String &whyNot ) const {
   if ( subframes.IsEmpty()) {
       whyNot = "No subframes have been specified.";
       return false;
   }

   return true;
}

bool SubframeSelectorInstance::ExecuteGlobal() {
   /*
    * Start with a general validation of working parameters.
    */
   {
       String why;
       if ( !CanExecuteGlobal( why ))
           throw Error( why );

       for ( image_list::const_iterator i = subframes.Begin(); i != subframes.End(); ++i )
           if ( i->enabled && !File::Exists( i->path ))
               throw ("No such file exists on the local filesystem: " + i->path);
   }

   try {

       /*
        * For all errors generated, we want a report on the console. This is
        * customary in PixInsight for all batch processes.
        */
       Exception::EnableConsoleOutput();
       Exception::DisableGUIOutput();

       /*
        * Allow the user to abort the calibration process.
        */
       Console console;
       console.EnableAbort();

       Module->ProcessEvents();

       /*
        * Begin subframe measuring process.
        */

       int succeeded = 0;
       int failed = 0;
       int skipped = 0;

       /*
        * Fail if no images have been calibrated.
        */
//       if ( succeeded == 0 ) {
//           if ( failed == 0 )
//               throw Error( "No images were measured: Empty target frames list? No enabled target frames?" );
//           throw Error( "No image could be measured." );
//       }

       /*
        * Write the final report to the console.
        */
       console.NoteLn(
               String().Format( "<end><cbr><br>===== SubframeSelector: %u succeeded, %u failed, %u skipped =====",
                                succeeded, failed, skipped ));
       return true;
   } // try

   catch ( ... ) {
       /*
        * All breaking errors are caught here.
        */
       Exception::EnableGUIOutput( true );
       throw;
   }
}

void* SubframeSelectorInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSSubframeEnabledParameter )
      return &subframes[tableRow].enabled;
   if ( p == TheSSSubframePathParameter )
      return subframes[tableRow].path.Begin();

   if ( p == TheSSSubframeScaleParameter )
      return &subframeScale;
   if ( p == TheSSCameraGainParameter )
      return &cameraGain;
   if ( p == TheSSCameraResolutionParameter )
      return &cameraResolution;
   if ( p == TheSSSiteLocalMidnightParameter )
      return &siteLocalMidnight;
   if ( p == TheSSScaleUnitParameter )
      return &scaleUnit;
   if ( p == TheSSDataUnitParameter )
      return &dataUnit;

   return nullptr;
}

bool SubframeSelectorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSSubframesParameter )
   {
      subframes.Clear();
      if ( sizeOrLength > 0 )
         subframes.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheSSSubframePathParameter )
   {
      subframes[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         subframes[tableRow].path.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type SubframeSelectorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSSSubframesParameter )
      return subframes.Length();
   if ( p == TheSSSubframePathParameter )
      return subframes[tableRow].path.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.cpp - Released 2017-08-01T14:26:58Z
