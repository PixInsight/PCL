//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.05.0094
// ----------------------------------------------------------------------------
// MergeCFAInterface.cpp - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "MergeCFAInterface.h"
#include "MergeCFAProcess.h"
#include "MergeCFAParameters.h"


#include <pcl/Settings.h>
#include <pcl/MessageBox.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

MergeCFAInterface* TheMergeCFAInterface = 0;

// ----------------------------------------------------------------------------

#include "MergeCFAIcon.xpm"

// ----------------------------------------------------------------------------

MergeCFAInterface::MergeCFAInterface() :
ProcessInterface(),
m_instance( TheMergeCFAProcess ),
GUI( 0 )
{
   TheMergeCFAInterface = this;
}

MergeCFAInterface::~MergeCFAInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString MergeCFAInterface::Id() const
{
   return "MergeCFA";
}

MetaProcess* MergeCFAInterface::Process() const
{
   return TheMergeCFAProcess;
}

const char** MergeCFAInterface::IconImageXPM() const
{
   return MergeCFAIcon_XPM;
}

InterfaceFeatures MergeCFAInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void MergeCFAInterface::ResetInstance()
{
   MergeCFAInstance defaultMergeCFAInstance( TheMergeCFAProcess );
   ImportProcess( defaultMergeCFAInstance );
}

bool MergeCFAInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "MergeCFA" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheMergeCFAProcess;
}

ProcessImplementation* MergeCFAInterface::NewProcess() const
{
   return new MergeCFAInstance( m_instance );
}

bool MergeCFAInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const MergeCFAInstance* r = dynamic_cast<const MergeCFAInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a MergeCFA instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool MergeCFAInterface::RequiresInstanceValidation() const
{
   return true;
}

bool MergeCFAInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void MergeCFAInterface::UpdateControls()
{
   GUI->CFA0_ViewList.GetMainViews();
   GUI->CFA1_ViewList.GetMainViews();
   GUI->CFA2_ViewList.GetMainViews();
   GUI->CFA3_ViewList.GetMainViews();

   GUI->CFA0_ViewList.SelectView(ImageWindow::WindowById(m_instance.p_viewId[0]).MainView());
   GUI->CFA1_ViewList.SelectView(ImageWindow::WindowById(m_instance.p_viewId[1]).MainView());
   GUI->CFA2_ViewList.SelectView(ImageWindow::WindowById(m_instance.p_viewId[2]).MainView());
   GUI->CFA3_ViewList.SelectView(ImageWindow::WindowById(m_instance.p_viewId[3]).MainView());
}

// ----------------------------------------------------------------------------
void MergeCFAInterface::__ViewList_ViewSelected( ViewList& sender, View& view )
{
   String id(view.Id());
   if ( sender == GUI->CFA0_ViewList )
   {
      // CFA0 selected -> try to find CFA 1,2,3 view automaticaly
      m_instance.p_viewId[0] = id;

      if (!id.EndsWith('0')) return;

      id.DeleteRight(id.Length()-1); //delete last char

      for (int cfaIndex = 3; cfaIndex > 0; --cfaIndex )
      {
         String cfaViewId( id + String( cfaIndex ) );

         View v = ImageWindow::WindowById( cfaViewId ).MainView();

         if (v.IsNull())
         {
            Console().WriteLn("not found id:" + cfaViewId);
         }
         else
         {
            Console().WriteLn("found id:" + cfaViewId);
            m_instance.p_viewId[cfaIndex] = cfaViewId;

            switch (cfaIndex)
            {
               case 1:
                  GUI->CFA1_ViewList.SelectView(v);
                  break;

               case 2:
                  GUI->CFA2_ViewList.SelectView(v);
                  break;

               case 3:
                  GUI->CFA3_ViewList.SelectView(v);
            }
         }
      }
   }
   else if( sender == GUI->CFA1_ViewList ) m_instance.p_viewId[1] = id;
   else if( sender == GUI->CFA2_ViewList ) m_instance.p_viewId[2] = id;
   else if( sender == GUI->CFA3_ViewList ) m_instance.p_viewId[3] = id;
}

// ----------------------------------------------------------------------------

MergeCFAInterface::GUIData::GUIData( MergeCFAInterface& w )
{
   CFA_Label.SetText( "CFA Source Images" );

   CFA0_Label.SetText( " 0 :" );
   CFA1_Label.SetText( " 1 :" );
   CFA2_Label.SetText( " 2 :" );
   CFA3_Label.SetText( " 3 :" );

   CFA0_Label.SetTextAlignment( TextAlign::VertCenter );
   CFA0_ViewList.OnViewSelected( (ViewList::view_event_handler)&MergeCFAInterface::__ViewList_ViewSelected, w );
   CFA0_ViewList.SetScaledMinWidth( 200 );

   MergeCFA0_Sizer.SetSpacing( 4 );
   MergeCFA0_Sizer.Add( CFA0_Label );
   MergeCFA0_Sizer.Add( CFA0_ViewList, 100 );

   CFA1_Label.SetTextAlignment( TextAlign::VertCenter );
   CFA1_ViewList.OnViewSelected( (ViewList::view_event_handler)&MergeCFAInterface::__ViewList_ViewSelected, w );
   CFA1_ViewList.SetScaledMinWidth( 200 );

   MergeCFA1_Sizer.SetSpacing( 4 );
   MergeCFA1_Sizer.Add( CFA1_Label );
   MergeCFA1_Sizer.Add( CFA1_ViewList, 100 );

   CFA2_Label.SetTextAlignment( TextAlign::VertCenter );
   CFA2_ViewList.OnViewSelected( (ViewList::view_event_handler)&MergeCFAInterface::__ViewList_ViewSelected, w );
   CFA2_ViewList.SetScaledMinWidth( 200 );

   MergeCFA2_Sizer.SetSpacing( 4 );
   MergeCFA2_Sizer.Add( CFA2_Label );
   MergeCFA2_Sizer.Add( CFA2_ViewList, 100 );

   CFA3_Label.SetTextAlignment( TextAlign::VertCenter );
   CFA3_ViewList.OnViewSelected( (ViewList::view_event_handler)&MergeCFAInterface::__ViewList_ViewSelected, w );

   MergeCFA3_Sizer.SetSpacing( 4 );
   MergeCFA3_Sizer.Add( CFA3_Label );
   MergeCFA3_Sizer.Add( CFA3_ViewList, 100 );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( CFA_Label );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( MergeCFA0_Sizer );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( MergeCFA1_Sizer );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( MergeCFA2_Sizer );
   Global_Sizer.AddSpacing( 6 );
   Global_Sizer.Add( MergeCFA3_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MergeCFAInterface.cpp - Released 2015/11/26 16:00:13 UTC
