//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0322
// ----------------------------------------------------------------------------
// AssignICCProfileInstance.cpp - Released 2017-07-09T18:07:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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

#include "AssignICCProfileInstance.h"
#include "AssignICCProfileParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ICCProfile.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AssignICCProfileInstance::AssignICCProfileInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   mode( AssignMode::Default )
{
}

// ----------------------------------------------------------------------------

AssignICCProfileInstance::AssignICCProfileInstance( const AssignICCProfileInstance& x ) :
   ProcessImplementation( x ),
   targetProfile( x.targetProfile ),
   mode( x.mode )
{
}

// -------------------------------------------------------------------------

void AssignICCProfileInstance::Assign( const ProcessImplementation& p )
{
   const AssignICCProfileInstance* x = dynamic_cast<const AssignICCProfileInstance*>( &p );
   if ( x != nullptr )
   {
      targetProfile = x->targetProfile;
      mode = x->mode;
   }
}

// ----------------------------------------------------------------------------

bool AssignICCProfileInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags AssignICCProfileInstance::UndoMode( const View& ) const
{
   return UndoFlag::ICCProfile;
}

// ----------------------------------------------------------------------------

bool AssignICCProfileInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "AssignICCProfile cannot be executed on previews.";
      return false;
   }

   if ( mode == AssignMode::AssignNewProfile && targetProfile.IsEmpty() )
   {
      whyNot = "This instance of AssignICCProfile cannot be executed because no target profile has been specified.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool AssignICCProfileInstance::ExecuteOn( View& view )
{
   static ICCProfile::profile_list profiles; // ### Warning: Not reentrant - thread-unsafe.

   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   ImageWindow w = view.Window();

   if ( mode == AssignMode::LeaveUntagged || mode == AssignMode::AssignDefaultProfile )
   {
      ICCProfile icc;
      w.GetICCProfile( icc );

      Console().Write( "<end><cbr>Previous ICC profile: " );

      if ( icc.IsProfile() )
      {
         w.DeleteICCProfile();
         Console().WriteLn( "<raw>" + icc.Description() + "</raw>" );
      }
      else
         Console().WriteLn( "&lt;* none *&gt;" );
   }

   if ( mode != AssignMode::LeaveUntagged )
   {
      String profilePath;

      if ( mode == AssignMode::AssignDefaultProfile )
         profilePath = PixInsightSettings::GlobalString( view.IsColor() ?
               "ColorManagement/DefaultRGBProfilePath" : "ColorManagement/DefaultGrayscaleProfilePath" );
      else // AssignMode::AssignNewProfile
      {
         if ( targetProfile.IsEmpty() )
            throw Error( "No profile specified!" );

         bool hadProfiles = !profiles.IsEmpty();

         if ( !hadProfiles )
            profiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::RGB|ICCColorSpace::Gray );

         ICCProfile::profile_list::const_iterator i = profiles.Search( ICCProfile::Info( targetProfile ) );

         if ( i == profiles.End() || !File::Exists( i->path ) )
         {
            if ( hadProfiles )
            {
               profiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::RGB|ICCColorSpace::Gray );
               i = profiles.Search( ICCProfile::Info( targetProfile ) );
            }
            else
               i = profiles.End();

            if ( i == profiles.End() )
               throw Error( "Couldn't find the '" + targetProfile + "' profile. "
                            "Either it has not been installed, or the corresponding disk file has been deleted." );
         }

         profilePath = i->path;
      }

      ICCProfile targetICC;

      try
      {
         targetICC.Load( profilePath );
      }
      ERROR_HANDLER

      if ( !targetICC.IsProfile() )
         throw Error( "Unable to load ICC profile: " + profilePath +
                      "The disk file could be corrupted, or it is not a valid ICC profile." );

      if ( w.MainView().IsColor() && !targetICC.IsRGB() )
         throw Error( '\'' + targetProfile + "' is a grayscale profile. "
                      "This profile cannot be assigned to a RGB color image." );

      w.SetICCProfile( targetICC );

      Console().WriteLn( "<end><cbr>Profile assigned: <raw>" + targetICC.Description() + "</raw>" );
      Console().WriteLn( "Profile path: <raw>" + targetICC.FilePath() + "</raw>" );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* AssignICCProfileInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheTargetProfileParameter )
      return targetProfile.Begin();
   if ( p == TheAssignModeParameter )
      return &mode;

   return nullptr;
}

bool AssignICCProfileInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheTargetProfileParameter )
   {
      targetProfile.Clear();
      if ( sizeOrLength > 0 )
         targetProfile.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type AssignICCProfileInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheTargetProfileParameter )
      return targetProfile.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssignICCProfileInstance.cpp - Released 2017-07-09T18:07:32Z
