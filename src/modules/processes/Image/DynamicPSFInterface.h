//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0402
// ----------------------------------------------------------------------------
// DynamicPSFInterface.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#ifndef __DynamicPSFInterface_h
#define __DynamicPSFInterface_h

#include <pcl/Atomic.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Graphics.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/ReferenceArray.h>
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

class PSFCollectionNode;
class StarNode;
class PSFNode;

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
   virtual void DynamicPaint( const View&, VectorGraphics&, const DRect& ) const;

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

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateStarInfo();
   void UpdateScaleItems();

   void RegenerateDataTree();
   void AdjustDataTreeColumns();

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
      Star*    star; // star being fitted
      PSFNode* node = nullptr;

      PSF( const PSFData& data, Star* s ) :
         PSFData( data ),
         star( s )
      {
      }

      virtual ~PSF();

      void AssignData( const PSFData& data )
      {
         *static_cast<PSFData*>( this ) = data;
      }
   };

   typedef ReferenceArray<PSF>  psf_list;

   struct Star : public StarData
   {
      typedef DynamicPSFInterface::psf_list psf_list;

      PSFCollection* collection;     // the view to which this star pertains
      psf_list       psfs;           // fitted PSFs
      StarNode*      node = nullptr;
      unsigned       uniqueId = 0;
      bool           selected = false;

      Star( PSFCollection* c ) :
         StarData(),
         collection( c ),
         uniqueId( c->UniqueStarId() )
      {
      }

      Star( const StarData& data, PSFCollection* c ) :
         StarData( data ),
         collection( c ),
         uniqueId( c->UniqueStarId() )
      {
      }

      virtual ~Star();

      void AssignData( const StarData& data )
      {
         *static_cast<StarData*>( this ) = data;
      }

      PSF* AddPSF( const PSFData& data )
      {
         PSF* psf = new PSF( data, this );
         psfs << psf;
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

   typedef ReferenceArray<Star> star_list;

   struct PSFCollection
   {
      typedef DynamicPSFInterface::star_list star_list;

      View               view;
      IsoString          viewId; // required in case view.IsNull()
      float              xScale = 0; // image scale in arcsec/px
      float              yScale = 0;
      Image              image;  // detection image, low-pass filtered
      star_list          stars;
      PSFCollectionNode* node = nullptr;

      PSFCollection( const IsoString& id ) :
         view( View::ViewById( id ) ), viewId( id )
      {
         if ( !view.IsNull() )
         {
            view.AddToDynamicTargets();
            UpdateDetectionImage();
         }
      }

      template <class S> PSFCollection( const S& id ) :
         PSFCollection( IsoString( id ) )
      {
      }

      PSFCollection( const View& v ) :
         view( v ), viewId( v.FullId() )
      {
         if ( !view.IsNull() )
         {
            view.AddToDynamicTargets();
            UpdateDetectionImage();
         }
      }

      virtual ~PSFCollection();

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

      template <class S>
      void UpdateImageScale( pcl_enum scaleMode, float scaleValue, const S& scaleKeyword )
      {
         UpdateImageScale( scaleMode, scaleValue, IsoString( scaleKeyword ) );
      }

      void UpdateNodes();
      void DestroyStarNodes();
      void CreateStarNodes();

      unsigned UniqueStarId()
      {
         return unsigned( nextStarId.FetchAndAdd( 1 ) );
      }

      void Sort( SortingCriterion );

   private:

      AtomicInt nextStarId = 1;

      void UpdateDetectionImage();

      class StarThread : public Thread
      {
      public:

         StarThread( const ImageVariant& image, float threshold, bool autoAperture,
                     star_list::iterator begin,
                     star_list::iterator end ) :
            m_image( image ), m_threshold( threshold ), m_autoAperture( autoAperture ), m_begin( begin ), m_end( end )
         {
         }

      protected:

         const ImageVariant& m_image;
         float               m_threshold;
         bool                m_autoAperture;
         star_list::iterator m_begin;
         star_list::iterator m_end;
      };

      class RegenerateThread : public StarThread
      {
      public:

         RegenerateThread( const ImageVariant& image, float threshold, bool autoAperture, const PSFOptions& options,
                           star_list::iterator begin,
                           star_list::iterator end ) :
            StarThread( image, threshold, autoAperture, begin, end ), m_options( options )
         {
         }

         virtual void Run()
         {
            for ( star_list::iterator s = m_begin; s != m_end; ++s )
               s->Regenerate( m_image, m_threshold, m_autoAperture, m_options );
         }

      private:

         PSFOptions m_options;
      };

      class RecalculateThread : public StarThread
      {
      public:

         RecalculateThread( const ImageVariant& image, float threshold, bool autoAperture,
                            star_list::iterator begin,
                            star_list::iterator end ) :
            StarThread( image, threshold, autoAperture, begin, end )
         {
         }

         virtual void Run()
         {
            for ( star_list::iterator s = m_begin; s != m_end; ++s )
               s->Recalculate( m_image, m_threshold, m_autoAperture );
         }
      };
   };

   typedef ReferenceArray<PSFCollection> psf_collection_list;

   psf_collection_list m_collections;
   star_list           m_selectedStars;

   //

   PSFCollection* FindPSFCollection( const View& view )
   {
      for ( PSFCollection& collection : m_collections )
         if ( collection.view == view )
            return &collection;
      return nullptr;
   }

   const PSFCollection* FindPSFCollection( const View& view ) const
   {
      return const_cast<DynamicPSFInterface*>( this )->FindPSFCollection( view );
   }

   bool HasPSFCollection( const View& view ) const
   {
      return FindPSFCollection( view ) != nullptr;
   }

   PSFCollection* AcquirePSFCollection( View& );

   //

   star_list FindStars( const View& view, const DRect& rect, ImageWindow::display_channel channel );

   void SelectStars( const star_list& stars, bool addToSelection = false );

   void SelectStar( const Star* star )
   {
      if ( star != nullptr )
      {
         star_list list;
         list << star;
         SelectStars( list );
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
      for ( PSFCollection& collection : m_collections )
         collection.Update();
   }

   void DrawStar( VectorGraphics&, double penWidth, const Star&, ImageWindow&, const Rect& r0 ) const;

   Star* StarFromTreeBoxNode( TreeBox::Node& );

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

// ----------------------------------------------------------------------------
// EOF DynamicPSFInterface.h - Released 2017-08-01T14:26:58Z
