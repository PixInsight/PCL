//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0200
// ----------------------------------------------------------------------------
// FluxCalibrationInterface.h - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __FluxCalibrationInterface_h
#define __FluxCalibrationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/NumericControl.h>
#include <pcl/ComboBox.h>
#include <pcl/ErrorHandler.h>

#include "FluxCalibrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FluxCalibrationInterface : public ProcessInterface
{
public:

   FluxCalibrationInterface();
   virtual ~FluxCalibrationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   FluxCalibrationInstance m_instance;

   struct GUIData;

   struct CalibrationParameter : public Control
   {
      CalibrationParameter() = default;

      void Build( FluxCalibrationInstance::CalibrationParameter& parameter,
                  const String& labelText, const String& toolTip, bool required = false )
      {
         m_parameter = &parameter;

         Value_NumericEdit.SetReal();
         Value_NumericEdit.SetRange( 0.0, 1.0e+10 );
         Value_NumericEdit.SetPrecision( 4 );
         Value_NumericEdit.label.SetMinWidth( m_valueLabelWidth );
         Value_NumericEdit.label.SetText( labelText );
         if ( required )
            Value_NumericEdit.label.SetStyleSheet( "QLabel { font-weight: bold; }" );
         Value_NumericEdit.edit.SetFixedWidth( m_valueEditWidth );
         Value_NumericEdit.sizer.AddStretch();
         Value_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&FluxCalibrationInterface::CalibrationParameter::__ValueUpdated, *this );

         Mode_ComboBox.AddItem( "Literal value" );
         Mode_ComboBox.AddItem( "Standard FITS keyword" );
         Mode_ComboBox.AddItem( "Custom FITS keyword" );
         Mode_ComboBox.SetMaxVisibleItemCount( 8 );
         Mode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&FluxCalibrationInterface::CalibrationParameter::__ItemSelected, *this );

         Keyword_Edit.SetMinWidth( m_keywordEditWidth );
         Keyword_Edit.OnEditCompleted( (Edit::edit_event_handler)&FluxCalibrationInterface::CalibrationParameter::__EditCompleted, *this );

         Parameter_Sizer.SetSpacing( 4 );
         Parameter_Sizer.Add( Value_NumericEdit );
         Parameter_Sizer.Add( Mode_ComboBox );
         Parameter_Sizer.Add( Keyword_Edit );

         SetSizer( Parameter_Sizer );

         SetToolTip( toolTip );
      }

      void Update()
      {
         switch ( m_parameter->mode )
         {
         default:
         case FCParameterMode::Literal:
            Value_NumericEdit.edit.Enable();
            Keyword_Edit.Disable();
            Keyword_Edit.SetText( m_parameter->keyword );
            break;
         case FCParameterMode::StandardKeyword:
            Value_NumericEdit.edit.Disable();
            Keyword_Edit.Disable();
            Keyword_Edit.SetText( m_parameter->stdKeyword );
            break;
         case FCParameterMode::CustomKeyword:
            Value_NumericEdit.edit.Disable();
            Keyword_Edit.Enable();
            Keyword_Edit.SetText( m_parameter->keyword );
            break;
         }

         Value_NumericEdit.SetValue( m_parameter->value );
         Mode_ComboBox.SetCurrentItem( m_parameter->mode );
      }

   private:

      FluxCalibrationInstance::CalibrationParameter* m_parameter = nullptr;

      HorizontalSizer   Parameter_Sizer;
         NumericEdit       Value_NumericEdit;
         ComboBox          Mode_ComboBox;
         Edit              Keyword_Edit;

      static int m_valueLabelWidth;
      static int m_valueEditWidth;
      static int m_keywordEditWidth;

      void __ValueUpdated( NumericEdit& sender, double value )
      {
         m_parameter->value = value;
      }

      void __ItemSelected( ComboBox& sender, int itemIndex )
      {
         m_parameter->mode = itemIndex;
         Update();
      }

      void __EditCompleted( Edit& sender )
      {
         String keyword = sender.Text().Trimmed().Uppercase();
         try
         {
            if ( !keyword.IsEmpty() )
            {
               if ( keyword.Length() < 2 || keyword.Length() > 8 )
                  throw Error( "Invalid keyword length: " + keyword );
               for ( size_type i = 0; i < keyword.Length(); ++i )
               {
                  char c = keyword[i];
                  if ( (c < 'A' || c > 'Z') && (c < '0' || c > '9') && c != '_' && c != '-' )
                     throw Error( "Illegal keyword character(s): " + keyword );
               }
            }
            sender.SetText( m_parameter->keyword = keyword );
         }
         ERROR_CLEANUP(
            sender.SetText( keyword );
            sender.Focus();
         )
      }

      friend struct FluxCalibrationInterface::GUIData;
   };

   struct GUIData
   {
      GUIData( FluxCalibrationInterface& );

      VerticalSizer        Global_Sizer;
         CalibrationParameter Wavelength_Parameter;
         CalibrationParameter Transmissivity_Parameter;
         CalibrationParameter FilterWidth_Parameter;
         CalibrationParameter Aperture_Parameter;
         CalibrationParameter CentralObstruction_Parameter;
         CalibrationParameter ExposureTime_Parameter;
         CalibrationParameter AtmosphericExtinction_Parameter;
         CalibrationParameter SensorGain_Parameter;
         CalibrationParameter QuantumEfficiency_Parameter;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern FluxCalibrationInterface* TheFluxCalibrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __FluxCalibrationInterface_h

// ----------------------------------------------------------------------------
// EOF FluxCalibrationInterface.h - Released 2018-11-23T18:45:58Z
