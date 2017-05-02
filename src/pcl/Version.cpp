//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/Version.cpp - Released 2017-05-02T10:39:13Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/MetaModule.h>
#include <pcl/Version.h>

#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

int Version::Major()
{
   return 2;
}

int Version::Minor()
{
   return 1;
}

int Version::Release()
{
   return 3;
}

int Version::Build()
{
   return 823;
}

int Version::BetaRelease()
{
   return 0;
}

String Version::LanguageCode()
{
   return "eng";  // ISO 639.2
}

String Version::AsString()
{
   String v = String().Format( "PCL %02d.%02d.%02d.%04d", Major(), Minor(), Release(), Build() );
   if ( BetaRelease() > 0 )
      v.Append( String().Format( " beta %d", BetaRelease() ) );
   return v;
}

// ----------------------------------------------------------------------------

static int s_major = 0;
static int s_minor = 0;
static int s_release = 0;
static int s_build = 0;
static int s_beta = 0;
static bool s_confidential = false;
static bool s_le = false;
static String s_language;
static String s_codename;

static void Initialize()
{
   static AtomicInt s_initialized;
   static Mutex s_mutex;

   if ( !s_initialized )
   {
      volatile AutoLock lock( s_mutex );
      if ( s_initialized.Load() == 0 )
      {
         if ( API != nullptr )
         {
            uint32 M, m, r, b, beta, conf, le;
            char lang[ 8 ];

            (*API->Global->GetPixInsightVersion)( &M, &m, &r, &b, &beta, &conf, &le, lang );
            s_major = int( M );
            s_minor = int( m );
            s_release = int( r );
            s_build = int( b );
            s_beta = int( beta );
            s_confidential = conf != 0;
            s_le = le != 0;
            s_language = lang;

            char16_type* s = (*API->Global->GetPixInsightCodename)( ModuleHandle() );
            if ( s != nullptr )
            {
               s_codename = String( s );
               if ( Module != nullptr )
                  Module->Deallocate( s );
            }
         }

         s_initialized.Store( 1 );
      }
   }
}

int PixInsightVersion::Major()
{
   Initialize();
   return s_major;
}

int PixInsightVersion::Minor()
{
   Initialize();
   return s_minor;
}

int PixInsightVersion::Release()
{
   Initialize();
   return s_release;
}

int PixInsightVersion::Build()
{
   Initialize();
   return s_build;
}

int PixInsightVersion::BetaRelease() // > 0 = Beta, < 0 = RC, 0 = Release
{
   Initialize();
   return s_beta;
}

bool PixInsightVersion::Confidential()
{
   Initialize();
   return s_confidential;
}

bool PixInsightVersion::LE()
{
   Initialize();
   return s_le;
}

String PixInsightVersion::LanguageCode()
{
   Initialize();
   return s_language;  // ISO 639.2
}

String PixInsightVersion::Codename()
{
   Initialize();
   return s_codename;
}

String PixInsightVersion::AsString( bool withCodename )
{
   Initialize();
   String v = String().Format( "PixInsight %s%02d.%02d.%02d.%04d",
               LE() ? "LE " : "", Major(), Minor(), Release(), Build() );
   if ( BetaRelease() != 0 )
      v.Append( String().Format( " %s%d", (BetaRelease() < 0) ? "RC" : "beta ", Abs( BetaRelease() ) ) );
   if ( withCodename )
      v.Append( ' ' + Codename() );
   if ( Confidential() )
      v.Append( " (confidential)" );
   return v;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Version.cpp - Released 2017-05-02T10:39:13Z
