// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Image Process Module Version 01.02.08.0283
// ****************************************************************************
// DynamicPSFInterface.h - Released 2014/11/14 17:19:21 UTC
// ****************************************************************************
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __DynamicPSFInterface_h
#define __DynamicPSFInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Mutex.h>
#include <pcl/NumericControl.h>
#include <pcl/PArray.h>
#include <pcl/ProcessInterface.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Thread.h>
#include <pcl/TreeBox.h>
#include <pcl/View.h>

#include "DynamicPSFInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class Graphics;
class ImageWindow;

class PSFTreeNode;

// ----------------------------------------------------------------------------

class DynamicPSFInterface : public ProcessInterface
{
public:

   DynamicPSFInterface();
   virtual ~DynamicPSFInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& flags );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool IsDynamicInterface() const;

   virtual void ExitDynamicMode();

   virtual void DynamicMouseEnter( View& );
   virtual void DynamicMouseLeave( View& );
   virtual void DynamicMouseMove( View&, const DPoint&, unsigned buttons, unsigned modifiers );
   virtual void DynamicMousePress( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );
   virtual void DynamicMouseRelease( View&, const DPoint&, int button, unsigned buttons, unsigned modifiers );
   virtual void DynamicMouseDoubleClick( View&, const DPoint&, unsigned buttons, unsigned modifiers );
   virtual bool DynamicKeyPress( View& v, int key, unsigned modifiers );

   virtual bool RequiresDynamicUpdate( const View&, const DRect& ) const;
   virtual void DynamicPaint( const View&, Graphics&, const DRect& ) const;

   virtual void SaveSettings() const;
   virtual void LoadSettings();

   static void ExecuteInstance( DynamicPSFInstance& );

private:

   DynamicPSFInstance instance;

   struct GUIData
   {
      GUIData( DynamicPSFInterface& );

      VerticalSizer  Global_Sizer;
         TreeBox           Data_TreeBox;
         HorizontalSizer   StarTools_Sizer;
            Label             StarInfo_Label;
            ToolButton        ExpandAll_ToolButton;
            ToolButton        CollapseAll_ToolButton;
            ToolButton        DeleteStar_ToolButton;
            ToolButton        TrackStars_ToolButton;
            ToolButton        Regenerate_ToolButton;
            ToolButton        RegenerateAll_ToolButton;
            ToolButton        Sort_ToolButton;
            ToolButton        ExportPSF_ToolButton;
            ToolButton        AverageStars_ToolButton;
            ToolButton        ExportCSV_ToolButton;
         SectionBar        PSFModelFunctions_SectionBar;
         Control           PSFModelFunctions_Control;
         HorizontalSizer   PSFModelFunctions_Sizer;
            VerticalSizer     PSFModelFunctions1_Sizer;
               CheckBox          AutoPSF_CheckBox;
               CheckBox          Moffat6_CheckBox;
               CheckBox          CircularPSF_CheckBox;
            VerticalSizer     PSFModelFunctions2_Sizer;
               CheckBox          Gaussian_CheckBox;
               CheckBox          Moffat4_CheckBox;
               CheckBox          SignedAngles_CheckBox;
            VerticalSizer     PSFModelFunctions3_Sizer;
               CheckBox          Moffat_CheckBox;
               CheckBox          Moffat25_CheckBox;
            VerticalSizer     PSFModelFunctions4_Sizer;
               CheckBox          MoffatA_CheckBox;
               CheckBox          Moffat15_CheckBox;
            VerticalSizer     PSFModelFunctions5_Sizer;
               CheckBox          Moffat8_CheckBox;
               CheckBox          Lorentzian_CheckBox;
         SectionBar        StarDetectionParameters_SectionBar;
         Control           StarDetectionParameters_Control;
         VerticalSizer     StarDetectionParameters_Sizer;
            HorizontalSizer   SearchRadius_Sizer;
               Label             SearchRadius_Label;
               SpinBox           SearchRadius_SpinBox;
            NumericEdit       Threshold_NumericEdit;
            HorizontalSizer   AutoAperture_Sizer;
               CheckBox          AutoAperture_CheckBox;
         SectionBar        Scale_SectionBar;
         Control           Scale_Control;
         VerticalSizer     Scale_Sizer;
            HorizontalSizer   ScaleMode_Sizer;
               Label             ScaleMode_Label;
               ComboBox          ScaleMode_ComboBox;
            NumericEdit       ScaleValue_NumericEdit;
            HorizontalSizer   ScaleKeyword_Sizer;
               Label             ScaleKeyword_Label;
               Edit              ScaleKeyword_Edit;
   };

   GUIData* GUI;

   void UpdateControls();
   void UpdateStarInfo();
   void UpdateScaleItems();

   void RegenerateDataTree();
   void AdjustDataTreeColumns();

   /*
    * Event Handlers
    */
   void __CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __NodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col );
   void __NodeExpanded( TreeBox& sender, TreeBox::Node& node );
   void __NodeCollapsed( TreeBox& sender, TreeBox::Node& node );
   void __NodeSelectionUpdated( TreeBox& sender );
   void __Click( Button& sender, bool checked );
   void __NumericEdit_ValueUpdated( NumericEdit& sender, double value );
   void __SpinBox_ValueUpdated( SpinBox& sender, int value );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __EditCompleted( Edit& sender );

   // -------------------------------------------------------------------------

   typedef DynamicPSFInstance::PSFOptions PSFOptions;

   enum SortingCriterion
   {
      SortByStarId,
      SortByBackground,
      SortByAmplitude,
      SortBySigma,
      SortByAspectRatio,
      SortByRotationAngle,
      SortByAbsRotationAngle,
      SortByShape,
      SortByMAD
   };

   struct Star;
   struct PSFCollection;

   struct PSF : public PSFData
   {
      Star*          star;    // star being fitted
      PSFTreeNode*   node;

      PSF( const PSFData& data, Star* s ) : PSFData( data ), star( s ), node( 0 )
      {
      }

      virtual ~PSF()
      {
      }

      void AssignData( const PSFData& data )
      {
         *static_cast<PSFData*>( this ) = data;
      }
   };

   typedef PArray<PSF>  psf_list;

   struct Star : public StarData
   {
      typedef DynamicPSFInterface::psf_list psf_list;

      PSFCollection* collection;    // view to which this star pertains
      psf_list       psfs;          // fitted PSFs
      PSFTreeNode*   node;
      unsigned       uniqueId;
      bool           selected : 1;  // selection status

      Star( PSFCollection* c ) :
      StarData(), collection( c ), psfs(), node( 0 ), uniqueId( c->UniqueStarId() ), selected( false )
      {
      }

      Star( const StarData& data, PSFCollection* c ) :
      StarData( data ), collection( c ), psfs(), node( 0 ), uniqueId( c->UniqueStarId() ), selected( false )
      {
      }

      virtual ~Star()
      {
         psfs.Destroy();
      }

      void AssignData( const StarData& data )
      {
         *static_cast<StarData*>( this ) = data;
      }

      PSF* AddPSF( const PSFData& data )
      {
         PSF* psf = new PSF( data, this );
         psfs.Add( psf );
         rect |= psf->Bounds();
         return psf;
      }

      void Regenerate( const PSFOptions& options );
      void Regenerate( const ImageVariant&, const PSFOptions& options );

      void Regenerate( float threshold, bool autoAperture, const PSFOptions& options );
      void Regenerate( const ImageVariant&, float threshold, bool autoAperture, const PSFOptions& options );

      void Recalculate( float threshold, bool autoAperture );
      void Recalculate( const ImageVariant&, float threshold, bool autoAperture );

      void Update();

      void UpdateNodes();
      void DestroyPSFNodes();
      void CreatePSFNodes();

      double SortingValue( SortingCriterion ) const;
   };

   typedef PArray<Star> star_list;

   struct PSFCollection
   {
      typedef DynamicPSFInterface::star_list star_list;

      View           view;
      IsoString      viewId; // required in case view.IsNull()
      float          xScale; // image scale in arcsec/px
      float          yScale;
      Image          image;  // detection image, low-pass filtered
      star_list      stars;
      PSFTreeNode*   node;

      PSFCollection( const IsoString& id ) :
      view( View::ViewById( id ) ), viewId( id ),
      xScale( 0 ), yScale( 0 ), stars(), node( 0 ), nextStarId( 0 )
      {
         if ( !view.IsNull() )
         {
            view.AddToDynamicTargets();
            UpdateDetectionImage();
         }
      }

      PSFCollection( const View& v ) :
      view( v ), viewId( v.FullId() ), stars(), node( 0 ), nextStarId( 0 )
      {
         if ( !view.IsNull() )
         {
            view.AddToDynamicTargets();
            UpdateDetectionImage();
         }
      }

      virtual ~PSFCollection()
      {
         stars.Destroy();
         if ( !view.IsNull() )
            view.RemoveFromDynamicTargets();
      }

      IsoString ViewId() const
      {
         return view.IsNull() ? viewId : view.FullId();
      }

      Star* AddStar( const StarData& data )
      {
         Star* star = new Star( data, this );
         stars.Add( star );
         return star;
      }

      void Regenerate( float threshold, bool autoAperture, const PSFOptions& options );

      void Recalculate( float threshold, bool autoAperture );

      void Update();
      void Update( const Rect& );

      void UpdateImageScale( pcl_enum scaleMode, float scaleValue, const IsoString& scaleKeyword );

      void UpdateNodes();
      void DestroyStarNodes();
      void CreateStarNodes();

      unsigned UniqueStarId()
      {
         mutex.Lock();
         unsigned id = ++nextStarId;
         mutex.Unlock();
         return id;
      }

      void Sort( SortingCriterion );

   private:

      Mutex    mutex;
      unsigned nextStarId;

      void UpdateDetectionImage();

      class StarThread : public Thread
      {
      public:
         StarThread( const ImageVariant& _img, float _threshold, bool _autoAperture,
                     star_list::iterator _begin, star_list::iterator _end ) :
         img( _img ), threshold( _threshold ), autoAperture( _autoAperture ), begin( _begin ), end( _end )
         {
         }

      protected:
         const ImageVariant& img;
         float threshold;
         bool autoAperture;
         star_list::iterator begin, end;
      };

      class RegenerateThread : public StarThread
      {
      public:
         RegenerateThread( const ImageVariant& _img, float _threshold, bool _autoAperture, const PSFOptions& _options,
                           star_list::iterator _begin, star_list::iterator _end ) :
         StarThread( _img, _threshold, _autoAperture, _begin, _end ), options( _options )
         {
         }

         virtual void Run()
         {
            for ( star_list::iterator s = begin; s != end; ++s )
               s->Regenerate( img, threshold, autoAperture, options );
         }

      private:
         PSFOptions options;
      };

      class RecalculateThread : public StarThread
      {
      public:
         RecalculateThread( const ImageVariant& _img, float _threshold, bool _autoAperture,
                            star_list::iterator _begin, star_list::iterator _end ) :
         StarThread( _img, _threshold, _autoAperture, _begin, _end )
         {
         }

         virtual void Run()
         {
            for ( star_list::iterator s = begin; s != end; ++s )
               s->Recalculate( img, threshold, autoAperture );
         }
      };
   };

   typedef PArray<PSFCollection> psf_data_set;

   psf_data_set data;
   star_list    selectedStars;

   //

   PSFCollection* FindPSFCollection( const View& view )
   {
      for ( psf_data_set::iterator i = data.Begin(); i != data.End(); ++i )
         if ( i->view == view )
            return i;
      return 0;
   }

   const PSFCollection* FindPSFCollection( const View& view ) const
   {
      return const_cast<DynamicPSFInterface*>( this )->FindPSFCollection( view );
   }

   bool HasPSFCollection( const View& view ) const
   {
      return FindPSFCollection( view ) != 0;
   }

   PSFCollection* AcquirePSFCollection( View& );

   //

   star_list FindStars( const View& view, const DRect& rect, ImageWindow::display_channel channel );

   void SelectStars( const star_list& stars, bool addToSelection = false );

   void SelectStar( const Star* star )
   {
      if ( star != 0 )
      {
         star_list oneStarList; oneStarList.Add( star );
         SelectStars( oneStarList );
      }
   }

   void UnselectStars()
   {
      star_list emptyList;
      SelectStars( emptyList );
   }

   void TrackStar( const Star* );

   void Regenerate();

   void Recalculate();

   void ExportCSV( const String& filePath );

   void Update()
   {
      for ( psf_data_set::iterator c = data.Begin(); c != data.End(); ++c )
         c->Update();
   }

   void DrawStar( Graphics&, const Star*, ImageWindow&, const Rect& r0, const Point& p0 ) const;

   Star* StarFromTreeBoxNode( TreeBox::Node& );

   void DeleteTreeBoxNode( TreeBox::Node* );

   void ExpandTreeBoxNodes( TreeBox::Node* );
   void CollapseTreeBoxNodes( TreeBox::Node* );

   friend struct GUIData;
   friend class PSFCollectionNode;
   friend class StarNode;
   friend class PSFNode;
   friend class AverageStarDialog;
   friend class SortStarsDialog;
   friend class StarSortingBinaryPredicate;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern DynamicPSFInterface* TheDynamicPSFInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DynamicPSFInterface_h

// ****************************************************************************
// EOF DynamicPSFInterface.h - Released 2014/11/14 17:19:21 UTC
