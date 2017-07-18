//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0207
// ----------------------------------------------------------------------------
// GradientsHdrInterface.h - Released 2017-07-18T16:14:19Z
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

#ifndef __GradientsHdrInterface_h
#define __GradientsHdrInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/CheckBox.h>
#include <pcl/Timer.h>

#include "GradientsHdrInstance.h"

namespace pcl
{


  class GradientsHdrInterface : public ProcessInterface
  {
  public:

    GradientsHdrInterface();
    virtual ~GradientsHdrInterface();

    virtual IsoString Id() const;
    virtual IsoString Aliases() const;
    virtual MetaProcess* Process() const;
    virtual const char** IconImageXPM() const;
    virtual InterfaceFeatures Features() const;

    // realtime preview
    virtual void RealTimePreviewUpdated( bool active );
    virtual bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const;
    virtual bool GenerateRealTimePreview( UInt16Image&, const View&, int zoomLevel, String& info ) const;

    virtual void ApplyInstance() const;
    virtual void ResetInstance();

    virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

    virtual ProcessImplementation* NewProcess() const;

    virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
    virtual bool RequiresInstanceValidation() const;

    virtual bool ImportProcess( const ProcessImplementation& );

  private:

    GradientsHdrInstance instance;

    struct GUIData
    {
      GUIData( GradientsHdrInterface& );

      VerticalSizer     global_Sizer;
      NumericControl    logMaxGradient_NumericControl;
      NumericControl    logMinGradient_NumericControl;
      NumericControl    expGradient_NumericControl;
      HorizontalSizer   rescale01_Sizer;
      Label             rescale01_Label;
      CheckBox           rescale01_CheckBox;
      Label              preserveColor_Label;
      CheckBox           preserveColor_CheckBox;
      Timer UpdateRealTime_Timer;
    };

    GUIData* GUI;

    void UpdateControls();
    void UpdateRealTimePreview();

    // Event Handlers

    void __expGradientUpdated( NumericEdit& sender, double value );
    void __logMaxGradientUpdated( NumericEdit& sender, double value );
    void __logMinGradientUpdated( NumericEdit& sender, double value );
    void __rescale01Clicked( Button& sender, bool value );
    void __preserveColorClicked( Button& sender, bool value );
    void __UpdateRealTime_Timer( Timer& );


    friend struct GUIData;
  };

  // ----------------------------------------------------------------------------

  PCL_BEGIN_LOCAL
    extern GradientsHdrInterface* TheGradientsHdrInterface;
  PCL_END_LOCAL

    // ----------------------------------------------------------------------------

    } // pcl

#endif   // __GradientsHdrInterface_h

// ----------------------------------------------------------------------------
// EOF GradientsHdrInterface.h - Released 2017-07-18T16:14:19Z
