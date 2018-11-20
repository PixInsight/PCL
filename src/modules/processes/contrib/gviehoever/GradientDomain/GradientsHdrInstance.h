//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0223
// ----------------------------------------------------------------------------
// GradientsHdrInstance.h - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2018. Licensed under LGPL 2.1
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#ifndef __GradientsHdrInstance_h
#define __GradientsHdrInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // for pcl_bool, pcl_enum

namespace pcl
{

class GradientsHdrInstance : public ProcessImplementation
{
public:

   GradientsHdrInstance( const MetaProcess* );
   GradientsHdrInstance( const GradientsHdrInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   ///  use pow(10,expGradient) to map gradient values
   double    expGradient;
   /// relative log10(maximum) gradient value that is clampled
   double    logMaxGradient;
   /// relative log10(minimum) gradient value that is clamped
   double    logMinGradient;
   /// rescale to range [0,1]? Otherwise rescale to original image range
   bool bRescale01;
   /// preserve relative proportions of RGB, otherwise just set luminance
   bool bPreserveColor;

   /// for realtime preview. Zoom is the zoomlevel used
   void ApplyClip16( UInt16Image& img, int zoom );

   friend class GradientsHdrEngine;
   friend class GradientsHdrProcess;
   friend class GradientsHdrInterface;
};


} // pcl

#endif   // __GradientsHdrInstance_h

// ----------------------------------------------------------------------------
// EOF GradientsHdrInstance.h - Released 2018-11-01T11:07:21Z
