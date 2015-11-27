//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// GradientsHdrCompositionInterface.h - Released 2015/11/26 16:00:13 UTC
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

#ifndef __GradientsHdrCompositionInterface_h
#define __GradientsHdrCompositionInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/TreeBox.h>
#include <pcl/CheckBox.h>
#include <pcl/NumericControl.h>

#include "GradientsHdrCompositionInstance.h"

namespace pcl
{

  class GradientsHdrCompositionInterface: public ProcessInterface
  {
  public:

    GradientsHdrCompositionInterface();
    virtual ~GradientsHdrCompositionInterface();

    virtual IsoString Id() const;
    virtual IsoString Aliases() const;
    virtual MetaProcess* Process() const;
    virtual const char** IconImageXPM() const;

    InterfaceFeatures Features() const;

    virtual void ApplyInstance() const;
    virtual void ResetInstance();

    virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

    virtual ProcessImplementation* NewProcess() const;

    virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
    virtual bool RequiresInstanceValidation() const;

    virtual bool ImportProcess( const ProcessImplementation& );

    virtual void SaveSettings() const;

    // -------------------------------------------------------------------------

  private:

    GradientsHdrCompositionInstance instance;

    // -------------------------------------------------------------------------

    struct GUIData
    {
      GUIData(GradientsHdrCompositionInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        TargetImages_SectionBar;
      Control           TargetImages_Control;
      HorizontalSizer   TargetImages_Sizer;
       TreeBox           TargetImages_TreeBox;
      VerticalSizer     TargetButtons_Sizer;
       PushButton        AddFiles_PushButton;
       PushButton        SelectAll_PushButton;
       PushButton        InvertSelection_PushButton;
       PushButton        ToggleSelected_PushButton;
       PushButton        RemoveSelected_PushButton;
       PushButton        Clear_PushButton;
      CheckBox          FullPaths_CheckBox;

      SectionBar        Parameters_SectionBar;
       Control           Parameters_Control;
        VerticalSizer     Parameters_Sizer;
        HorizontalSizer   Bias_Sizer;
         NumericControl   LogBias_NumericControl;
        HorizontalSizer   KeepLog_Sizer;
         Label            KeepLog_Label;
         CheckBox         KeepLog_CheckBox;
        HorizontalSizer   GenerateMask_Sizer;
         Label            GenerateMask_Label;
         CheckBox         GenerateMask_CheckBox;
         Label            NegativeBias_Label;
         CheckBox         NegativeBias_CheckBox;

    };

    GUIData* GUI;

    // Interface Updates

    void UpdateControls();
    void UpdateTargetImageItem( size_type );
    void UpdateTargetImagesList();
    void UpdateImageSelectionButtons();
    void UpdateParameters();

    // Event Handlers

    void __TargetImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
    void __TargetImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
    void __TargetImages_NodeSelectionUpdated( TreeBox& sender );
    void __TargetImages_Click( Button& sender, bool checked );

    // void __CheckSection( SectionBar& sender, bool checked );
    void __ToggleSection( SectionBar& sender, Control& section, bool start );

    /// callback for logBias numeric control
    void __logBiasUpdated( NumericEdit& sender, double value );

    /// callback for keepLog checkbox
    void __KeepLogClicked( Button& sender, bool checked );

    /// callback for negativeBias checkbox
    void __NegativeBiasClicked( Button& sender, bool checked );

    /// callback for generate mask checkbox
    void __GenerateMaskClicked( Button& sender, bool checked );

    friend struct GUIData;
  };

  // ----------------------------------------------------------------------------

  PCL_BEGIN_LOCAL
    extern GradientsHdrCompositionInterface* TheGradientsHdrCompositionInterface;
  PCL_END_LOCAL

  // ----------------------------------------------------------------------------

} // pcl

#endif

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompositionInterface.h - Released 2015/11/26 16:00:13 UTC
