//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// GradientsModule.h - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2015. Licensed under LGPL 2.1
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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
// ----------------------------------------------------------------------------

#ifndef __GradientsModule_h
#define __GradientsModule_h

#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// GradientsModule
// ----------------------------------------------------------------------------

//// module definition for the gradients module
class GradientsModule : public MetaModule
{
public:

   GradientsModule();

   virtual const char* Version() const;
   virtual IsoString Name() const;
   virtual String Description() const;
   virtual String Company() const;
   virtual String Author() const;
   virtual String Copyright() const;
   virtual String TradeMarks() const;
   virtual String OriginalFileName() const;
   virtual void GetReleaseDate( int& year, int& month, int& day ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __GradientsModule_h

// ----------------------------------------------------------------------------
// EOF GradientsModule.h - Released 2015/11/26 16:00:13 UTC
