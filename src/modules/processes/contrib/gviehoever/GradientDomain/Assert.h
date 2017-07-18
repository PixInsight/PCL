//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0207
// ----------------------------------------------------------------------------
// Assert.h - Released 2017-07-18T16:14:19Z
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

#ifndef __Assert_h
#define __Assert_h

/*
 * PI/PCL friendly version of the C assert function.
 */

#ifdef DEBUG
#  ifndef __PCL_Exception_h
#    include <pcl/Exception.h>
#    include <pcl/String.h>
#  endif
#  define Assert( condition ) \
  if ( !(condition) )							\
      throw Error( String( "Assertion failed: \'" ) + #condition + "\' file:" + __FILE__ + " line:" + String( __LINE__ ) )
#else
#  define Assert( condition )
#endif

#endif   // __Assert_h

// ----------------------------------------------------------------------------
// EOF Assert.h - Released 2017-07-18T16:14:19Z
