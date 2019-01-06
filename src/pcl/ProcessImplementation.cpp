//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/ProcessImplementation.cpp - Released 2018-12-12T09:24:30Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Exception.h>
#include <pcl/ImageWindow.h>
#include <pcl/ProcessImplementation.h>
#include <pcl/View.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define MANDATORY( funcName )          \
   MandatoryError( meta->Id(), funcName )

static void MandatoryError( const IsoString& procId, const char* funcName )
{
   throw Error( String( procId ) + ": ProcessImplementation::" +
                funcName + "() must be reimplemented in descendant class." );
}

// ----------------------------------------------------------------------------

ProcessImplementation::ProcessImplementation( const MetaProcess* m ) : meta( m )
{
}

ProcessImplementation::ProcessImplementation( const ProcessImplementation& x ) : meta( x.meta )
{
}

ProcessImplementation::~ProcessImplementation()
{
}

// ----------------------------------------------------------------------------

void ProcessImplementation::Assign( const ProcessImplementation& )
{
   MANDATORY( "Assign" );
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::CanExecuteOn( const View&, String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

bool ProcessImplementation::CanExecuteOn( const ImageVariant&, String& whyNot ) const
{
   whyNot.Clear();
   return true;
}

bool ProcessImplementation::CanExecuteGlobal( String& whyNot ) const
{
   whyNot = String( meta->Id() ) + " instances cannot be executed in the global context.";
   return false;
}

// ----------------------------------------------------------------------------

void ProcessImplementation::Initialize()
{
   MANDATORY( "Initialize" );
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::Validate( String& /*info*/ )
{
   MANDATORY( "Validate" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ExecuteOn( View& )
{
   MANDATORY( "ExecuteOn" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ExecuteOn( ImageVariant&, const IsoString& )
{
   MANDATORY( "ExecuteOn" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ExecuteGlobal()
{
   MANDATORY( "ExecuteGlobal" );
   return false;
}

// ----------------------------------------------------------------------------

void* ProcessImplementation::LockParameter( const MetaParameter*, size_type /*tableRow*/ )
{
   MANDATORY( "LockParameter" );
   return 0;
}

// ----------------------------------------------------------------------------

void ProcessImplementation::UnlockParameter( const MetaParameter*, size_type /*tableRow*/ )
{
   MANDATORY( "UnlockParameter" );
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::ValidateParameter( void*, const MetaParameter*, size_type /*tableRow*/ ) const
{
   MANDATORY( "ValidateParameter" );
   return false;
}

// ----------------------------------------------------------------------------

bool ProcessImplementation::AllocateParameter( size_type /*size*/, const MetaParameter*, size_type /*tableRow*/ )
{
   MANDATORY( "AllocateParameter" );
   return false;
}

// ----------------------------------------------------------------------------

size_type ProcessImplementation::ParameterLength( const MetaParameter*, size_type /*tableRow*/ ) const
{
   MANDATORY( "ParameterLength" );
   return 0;
}

// ----------------------------------------------------------------------------

void ProcessImplementation::Launch() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::Default, 0 );
}

void ProcessImplementation::LaunchInterface() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::Interface, 0 );
}

void ProcessImplementation::LaunchGlobal() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::Global, 0 );
}

void ProcessImplementation::LaunchOnCurrentView() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::CurrentView, 0 );
}

void ProcessImplementation::LaunchOnCurrentWindow() const
{
   (*API->Global->LaunchProcessInstance)( meta, this, InstanceLaunchMode::CurrentWindow, 0 );
}

// ----------------------------------------------------------------------------

void ProcessImplementation::LaunchOn( View& v ) const
{
   (*API->Global->LaunchProcessInstanceOnView)( meta, this, v.handle, 0 );
}

void ProcessImplementation::LaunchOn( ImageWindow& w ) const
{
   View mv( w.MainView() );
   LaunchOn( mv );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ProcessImplementation.cpp - Released 2018-12-12T09:24:30Z
