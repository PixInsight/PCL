//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0215
// ----------------------------------------------------------------------------
// GradientsMergeMosaicInstance.h - Released 2017-08-01T14:26:58Z
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

#ifndef __GradientsMergeMosaicInstance_h
#define __GradientsMergeMosaicInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>

#include "GradientsMergeMosaic.h" //for image types

namespace pcl
{

  class GradientsMergeMosaicInstance : public ProcessImplementation
  {
  public:

    GradientsMergeMosaicInstance( const MetaProcess* );
    GradientsMergeMosaicInstance( const GradientsMergeMosaicInstance& );

    virtual void Assign( const ProcessImplementation& );

    virtual bool CanExecuteOn( const View&, String& whyNot ) const;
    virtual bool IsHistoryUpdater( const View& v ) const;

    virtual bool CanExecuteGlobal( String& whyNot ) const;
    virtual bool ExecuteGlobal();

    virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
    virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
    virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;


  private: //functions
    /// create image window that has rImage as contents
    static ImageWindow CreateImageWindow( const IsoString& id, GradientsMergeMosaic::imageType_t const &rImage_p);

  private: //slots
    struct ImageItem
    {
      pcl_bool enabled; // if disabled, skip (ignore) this image
      String   path;    // absolute file path

    ImageItem( const String& p = String() ) : enabled( true ), path( p )
      {
      }

    ImageItem( const ImageItem& x ) : enabled( x.enabled ), path( x.path )
      {
      }

      bool IsValid() const
      {
	return !enabled || !path.IsEmpty();
      }
    };

    typedef Array<ImageItem>  image_list;

    /// The set of target frames to be calibrated
    image_list      targetFrames;

    /// type of overlay operation
    pcl_enum type;

    /// the radius in pixels of an erosion of the image borders
    int32 shrinkCount;

    /// the radius in pixels of a convolution of the image borders
    int32 featherRadius;

    /// black point value. Pixel Values < blackPoint are considered transparent
    double blackPoint;

    /// true if we need to generate a mask view
    pcl_bool generateMask;

    friend class GradientsMergeMosaicInterface;
  };

  // ----------------------------------------------------------------------------

} // pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaicInstance.h - Released 2017-08-01T14:26:58Z
