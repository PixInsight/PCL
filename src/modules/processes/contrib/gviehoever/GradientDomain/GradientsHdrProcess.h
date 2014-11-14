// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard GradientDomain Process Module Version 00.06.04.0098
// ****************************************************************************
// GradientsHdrProcess.h - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2014. Licensed under LGPL 2.1
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ****************************************************************************

#ifndef __GradientsHdrProcess_h
#define __GradientsHdrProcess_h

#include <pcl/MetaProcess.h>

namespace pcl
{

class GradientsHdrProcess : public MetaProcess
{
public:

   GradientsHdrProcess();

   virtual IsoString Id() const;
   virtual IsoString Aliases() const;
   virtual IsoString Category() const;

   virtual uint32 Version() const;

   virtual String Description() const;

   virtual const char** IconImageXPM() const;

   virtual ProcessInterface* DefaultInterface() const;

   virtual ProcessImplementation* Create() const;
   virtual ProcessImplementation* Clone( const ProcessImplementation& ) const;

   virtual bool CanProcessCommandLines() const;
   virtual int ProcessCommandLine( const StringList& ) const;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern GradientsHdrProcess* TheGradientsHdrProcess;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GradientsHdrProcess_h

// ****************************************************************************
// EOF GradientsHdrProcess.h - Released 2014/11/14 17:19:24 UTC
