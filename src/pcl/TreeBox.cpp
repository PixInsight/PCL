//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/TreeBox.cpp - Released 2017-08-01T14:23:38Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/AutoLock.h>
#include <pcl/TreeBox.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

TreeBox::TreeBox( Control& parent ) :
   ScrollBox( nullptr )
{
   TransferHandle( (*API->TreeBox->CreateTreeBox)( ModuleHandle(), this, parent.handle, 0/*flags*/ ) );
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBox" );

   m_viewport.TransferHandle( (*API->TreeBox->CreateTreeBoxViewport)( handle, &m_viewport ) );
   if ( m_viewport.IsNull() )
      throw APIFunctionError( "CreateTreeBoxViewport" );
}

TreeBox::TreeBox( void* h ) :
   ScrollBox( nullptr )
{
   TransferHandle( h );
   if ( !IsNull() )
   {
      m_viewport.TransferHandle( (*API->TreeBox->CreateTreeBoxViewport)( handle, &m_viewport ) );
      if ( m_viewport.IsNull() )
         throw APIFunctionError( "CreateTreeBoxViewport" );
   }
}

TreeBox::~TreeBox()
{
   child_node_list children( m_children );
   m_children.Clear();
   for ( auto node : children )
   {
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

TreeBox& TreeBox::NullTree()
{
   static TreeBox* nullTree = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullTree == nullptr )
      nullTree = new TreeBox( nullptr );
   return *nullTree;
}

// ----------------------------------------------------------------------------

int TreeBox::NumberOfChildren() const
{
   return (*API->TreeBox->GetTreeBoxChildCount)( handle );
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::Child( int idx ) const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxChild)( handle, idx ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::Child( int idx )
{
   return reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxChild)( handle, idx ) );
}

// ----------------------------------------------------------------------------

int TreeBox::ChildIndex( const TreeBox::Node* node ) const
{
   if ( node == nullptr )
      return -1;
   return (*API->TreeBox->GetTreeBoxChildIndex)( handle, node->handle );
}

// ----------------------------------------------------------------------------

void TreeBox::Insert( int idx, TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      PCL_CHECK( node->ParentTree().IsNull() )
      PCL_CHECK( node->Parent() == nullptr )
      if ( !m_children.Contains( node ) )
      {
         (*API->TreeBox->InsertTreeBoxNode)( handle, idx, node->handle );
         m_children << node;
      }
   }
}

// ----------------------------------------------------------------------------

void TreeBox::Remove( int idx )
{
   Node* node = Child( idx );
   if ( node != nullptr )
   {
      (*API->TreeBox->RemoveTreeBoxNode)( handle, idx );
      m_children.Remove( node );
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

void TreeBox::Clear()
{
   (*API->TreeBox->ClearTreeBox)( handle );
   child_node_list children( m_children );
   m_children.Clear();
   for ( auto node : children )
   {
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::CurrentNode() const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxCurrentNode)( handle ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::CurrentNode()
{
   return reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxCurrentNode)( handle ) );
}

// ----------------------------------------------------------------------------

void TreeBox::SetCurrentNode( TreeBox::Node* node )
{
   if ( node != nullptr )
      (*API->TreeBox->SetTreeBoxCurrentNode)( handle, node->handle );
}

// ----------------------------------------------------------------------------

bool TreeBox::AreMultipleSelectionsEnabled() const
{
   return (*API->TreeBox->GetTreeBoxMultipleNodeSelectionEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableMultipleSelections( bool enable )
{
   (*API->TreeBox->SetTreeBoxMultipleNodeSelectionEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

IndirectArray<TreeBox::Node> TreeBox::SelectedNodes() const
{
   IndirectArray<TreeBox::Node> nodes;
   size_type n = 0;
   (*API->TreeBox->GetTreeBoxSelectedNodes)( handle, 0, &n );
   if ( n > 0 )
   {
      nodes.Add( nullptr, n );
      if ( (*API->TreeBox->GetTreeBoxSelectedNodes)( handle,
                     reinterpret_cast< ::api_handle*>( nodes.Begin() ), &n ) == api_false )
         throw APIFunctionError( "GetTreeBoxSelectedNodes" );
      nodes.Pack();
   }
   return nodes;
}

// ----------------------------------------------------------------------------

bool TreeBox::HasSelectedTopLevelNodes() const
{
   for ( int i = 0, n = NumberOfChildren(); i < n; ++i )
      if ( Child( i )->IsSelected() )
         return true;
   return false;
}

// ----------------------------------------------------------------------------

void TreeBox::SelectAllNodes()
{
   (*API->TreeBox->SelectAllTreeBoxNodes)( handle );
}

// ----------------------------------------------------------------------------

/*
 * ### TODO: Implement inline node editors.
 */

/*
void TreeBox::BeginNodeEdition( TreeBox::Node* node, int col )
{
   if ( node != nullptr )
      (*API->TreeBox->BeginTreeBoxNodeEdition)( handle, node->handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::EndNodeEdition( TreeBox::Node* node, int col )
{
   if ( node != nullptr )
      (*API->TreeBox->EndTreeBoxNodeEdition)( handle, node->handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::EditNode( TreeBox::Node* node, int col )
{
   if ( node != nullptr )
      (*API->TreeBox->EditTreeBoxNode)( handle, node->handle, col );
}
*/

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::NodeByPosition( int x, int y ) const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxNodeByPos)( handle, x, y ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::NodeByPosition( int x, int y )
{
   return reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxNodeByPos)( handle, x, y ) );
}

// ----------------------------------------------------------------------------

void TreeBox::SetNodeIntoView( TreeBox::Node* node )
{
   if ( node != nullptr )
      (*API->TreeBox->SetTreeBoxNodeIntoView)( handle, node->handle );
}

// ----------------------------------------------------------------------------

pcl::Rect TreeBox::NodeRect( const TreeBox::Node* node ) const
{
   if ( node == nullptr )
      return pcl::Rect( 0 );
   pcl::Rect r;
   (*API->TreeBox->GetTreeBoxNodeRect)( handle, node->handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

int TreeBox::NumberOfColumns() const
{
   return (*API->TreeBox->GetTreeBoxColumnCount)( handle );
}

// ----------------------------------------------------------------------------

void TreeBox::SetNumberOfColumns( int nCols )
{
   (*API->TreeBox->SetTreeBoxColumnCount)( handle, nCols );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsColumnVisible( int col ) const
{
   return (*API->TreeBox->GetTreeBoxColumnVisible)( handle, col ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::ShowColumn( int col, bool show )
{
   (*API->TreeBox->SetTreeBoxColumnVisible)( handle, col, show );
}

// ----------------------------------------------------------------------------

int TreeBox::ColumnWidth( int col ) const
{
   return (*API->TreeBox->GetTreeBoxColumnWidth)( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::SetColumnWidth( int col, int width )
{
   (*API->TreeBox->SetTreeBoxColumnWidth)( handle, col, width );
}

// ----------------------------------------------------------------------------

void TreeBox::AdjustColumnWidthToContents( int col )
{
   (*API->TreeBox->AdjustTreeBoxColumnWidthToContents)( handle, col );
}

// ----------------------------------------------------------------------------

String TreeBox::HeaderText( int col ) const
{
   size_type len = 0;
   (*API->TreeBox->GetTreeBoxHeaderText)( handle, col, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->TreeBox->GetTreeBoxHeaderText)( handle, col, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTreeBoxHeaderText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void TreeBox::SetHeaderText( int col, const String& text )
{
   (*API->TreeBox->SetTreeBoxHeaderText)( handle, col, text.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap TreeBox::HeaderIcon( int col ) const
{
   return Bitmap( (*API->TreeBox->GetTreeBoxHeaderIcon)( handle, col ) );
}

// ----------------------------------------------------------------------------

void TreeBox::SetHeaderIcon( int col, const Bitmap& icon )
{
   (*API->TreeBox->SetTreeBoxHeaderIcon)( handle, col, icon.handle );
}

// ----------------------------------------------------------------------------

int TreeBox::HeaderAlignment( int col ) const
{
   return (*API->TreeBox->GetTreeBoxHeaderAlignment)( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::SetHeaderAlignment( int col, int align )
{
   (*API->TreeBox->SetTreeBoxHeaderAlignment)( handle, col, align );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsHeaderVisible() const
{
   return (*API->TreeBox->GetTreeBoxHeaderVisible)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::ShowHeader( bool show )
{
   (*API->TreeBox->SetTreeBoxHeaderVisible)( handle, show );
}

// ----------------------------------------------------------------------------

int TreeBox::IndentSize() const
{
   return (*API->TreeBox->GetTreeBoxIndentSize)( handle );
}

// ----------------------------------------------------------------------------

void TreeBox::SetIndentSize( int szPx )
{
   (*API->TreeBox->SetTreeBoxIndentSize)( handle, szPx );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsNodeExpansionEnabled() const
{
   return (*API->TreeBox->GetTreeBoxNodeExpansionEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableNodeExpansion( bool enable )
{
   (*API->TreeBox->SetTreeBoxNodeExpansionEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsRootDecorationEnabled() const
{
   return (*API->TreeBox->GetTreeBoxRootDecorationEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableRootDecoration( bool enable )
{
   (*API->TreeBox->SetTreeBoxRootDecorationEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsAlternateRowColorEnabled() const
{
   return (*API->TreeBox->GetTreeBoxAlternateRowColorEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableAlternateRowColor( bool enable )
{
   (*API->TreeBox->SetTreeBoxAlternateRowColorEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsUniformRowHeightEnabled() const
{
   return (*API->TreeBox->GetTreeBoxUniformRowHeightEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableUniformRowHeight( bool enable )
{
   (*API->TreeBox->SetTreeBoxUniformRowHeightEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

void TreeBox::GetIconSize( int& width, int& height ) const
{
   (*API->TreeBox->GetTreeBoxIconSize)( handle, &width, &height );
}

// ----------------------------------------------------------------------------

void TreeBox::SetIconSize( int width, int height )
{
   (*API->TreeBox->SetTreeBoxIconSize)( handle, width, height );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsHeaderSortingEnabled() const
{
   return (*API->TreeBox->GetTreeBoxHeaderSortingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableHeaderSorting( bool enable )
{
   (*API->TreeBox->SetTreeBoxHeaderSortingEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

void TreeBox::Sort( int col, bool ascending )
{
   (*API->TreeBox->SortTreeBox)( handle, col, ascending );
}

// ----------------------------------------------------------------------------

bool TreeBox::IsNodeDraggingEnabled() const
{
   return (*API->TreeBox->GetTreeBoxNodeDraggingEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::EnableNodeDragging( bool enable )
{
   (*API->TreeBox->SetTreeBoxNodeDraggingEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------
// TreeBox::Node Implementation
// ----------------------------------------------------------------------------

TreeBox::Node::Node() :
   UIObject( (*API->TreeBox->CreateTreeBoxNode)( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBoxNode" );
}

TreeBox::Node::Node( TreeBox::Node& parent, int index ) :
   UIObject( (*API->TreeBox->CreateTreeBoxNode)( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBoxNode" );
   if ( !parent.IsNull() )
      parent.Insert( index, this );
}

TreeBox::Node::Node( TreeBox& parentTree, int index ) :
   UIObject( (*API->TreeBox->CreateTreeBoxNode)( ModuleHandle(), this ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateTreeBoxNode" );
   if ( !parentTree.IsNull() )
      parentTree.Insert( index, this );
}

// ----------------------------------------------------------------------------

TreeBox::Node::~Node()
{
   if ( !m_removed )
   {
      m_removed = true;
      Node* parentNode = reinterpret_cast<Node*>( (*API->TreeBox->GetTreeBoxNodeParent)( handle ) );
      if ( parentNode != nullptr )
      {
         parentNode->m_children.Remove( this );
         int idx = parentNode->ChildIndex( this );
         if ( idx >= 0 )
            (*API->TreeBox->RemoveTreeBoxNodeChild)( parentNode->handle, idx );
      }
      else
      {
         TreeBox* parentTree = reinterpret_cast<TreeBox*>( (*API->TreeBox->GetTreeBoxNodeParentBox)( handle ) );
         if ( parentTree != nullptr )
         {
            parentTree->m_children.Remove( this );
            int idx = parentTree->ChildIndex( this );
            if ( idx >= 0 )
               (*API->TreeBox->RemoveTreeBoxNode)( parentTree->handle, idx );
         }
      }
   }

   child_node_list children( m_children );
   m_children.Clear();
   for ( auto node : children )
   {
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

const TreeBox& TreeBox::Node::ParentTree() const
{
   TreeBox* tree = reinterpret_cast<TreeBox*>( (*API->TreeBox->GetTreeBoxNodeParentBox)( handle ) );
   return (tree != nullptr) ? *tree : NullTree();
}

// ----------------------------------------------------------------------------

TreeBox& TreeBox::Node::ParentTree()
{
   TreeBox* tree = reinterpret_cast<TreeBox*>( (*API->TreeBox->GetTreeBoxNodeParentBox)( handle ) );
   return (tree != nullptr) ? *tree : NullTree();
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::Node::Parent() const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxNodeParent)( handle ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::Node::Parent()
{
   return reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxNodeParent)( handle ) );
}

// ----------------------------------------------------------------------------

int TreeBox::Node::NumberOfChildren() const
{
   return (*API->TreeBox->GetTreeBoxNodeChildCount)( handle );
}

// ----------------------------------------------------------------------------

const TreeBox::Node* TreeBox::Node::Child( int idx ) const
{
   return const_cast<const TreeBox::Node*>(
      reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxNodeChild)( handle, idx ) ) );
}

// ----------------------------------------------------------------------------

TreeBox::Node* TreeBox::Node::Child( int idx )
{
   return reinterpret_cast<TreeBox::Node*>( (*API->TreeBox->GetTreeBoxNodeChild)( handle, idx ) );
}

// ----------------------------------------------------------------------------

int TreeBox::Node::ChildIndex( const TreeBox::Node* node ) const
{
   for ( int i = 0, n = NumberOfChildren(); i < n; ++i )
      if ( Child( i ) == node )
         return i;
   return -1;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Insert( int idx, TreeBox::Node* node )
{
   if ( node != nullptr )
   {
      PCL_CHECK( node->ParentTree().IsNull() )
      PCL_CHECK( node->Parent() == nullptr )
      if ( !m_children.Contains( node ) )
      {
         (*API->TreeBox->InsertTreeBoxNodeChild)( handle, idx, node->handle );
         m_children << node;
      }
   }
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Remove( int idx )
{
   TreeBox::Node* node = Child( idx );
   if ( node != nullptr )
   {
      (*API->TreeBox->RemoveTreeBoxNodeChild)( handle, idx );
      m_children.Remove( node );
      node->m_removed = true;
      delete node;
   }
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsEnabled() const
{
   return (*API->TreeBox->GetTreeBoxNodeEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Enable( bool enable )
{
   (*API->TreeBox->SetTreeBoxNodeEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsExpanded() const
{
   return (*API->TreeBox->GetTreeBoxNodeExpanded)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Expand( bool expand )
{
   (*API->TreeBox->SetTreeBoxNodeExpanded)( handle, expand );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsSelectable() const
{
   return (*API->TreeBox->GetTreeBoxNodeSelectable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetSelectable( bool enable )
{
   (*API->TreeBox->SetTreeBoxNodeSelectable)( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsSelected() const
{
   return (*API->TreeBox->GetTreeBoxNodeSelected)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Select( bool select )
{
   (*API->TreeBox->SetTreeBoxNodeSelected)( handle, select );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsCheckable() const
{
   return (*API->TreeBox->GetTreeBoxNodeCheckable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetCheckable( bool enable )
{
   (*API->TreeBox->SetTreeBoxNodeCheckable)( handle, enable );
}

// ----------------------------------------------------------------------------

bool TreeBox::Node::IsChecked() const
{
   return (*API->TreeBox->GetTreeBoxNodeChecked)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::Check( bool check )
{
   (*API->TreeBox->SetTreeBoxNodeChecked)( handle, check );
}

// ----------------------------------------------------------------------------

/*
 * ### TODO: Implement inline node editors.
 */

/*
bool TreeBox::Node::IsEditable() const
{
   return (*API->TreeBox->GetTreeBoxNodeEditable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetEditable( bool enable )
{
   (*API->TreeBox->SetTreeBoxNodeEditable)( handle, enable );
}
*/
// ----------------------------------------------------------------------------

String TreeBox::Node::Text( int col ) const
{
   size_type len = 0;
   (*API->TreeBox->GetTreeBoxNodeColText)( handle, col, 0, &len );

   String text;
   if ( len > 0 )
   {
      text.SetLength( len );
      if ( (*API->TreeBox->GetTreeBoxNodeColText)( handle, col, text.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTreeBoxNodeColText" );
      text.ResizeToNullTerminated();
   }
   return text;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetText( int col, const String& text )
{
   (*API->TreeBox->SetTreeBoxNodeColText)( handle, col, text.c_str() );
}

// ----------------------------------------------------------------------------

Bitmap TreeBox::Node::Icon( int col ) const
{
   return TreeBox::BitmapFromHandle( (*API->TreeBox->GetTreeBoxNodeColIcon)( handle, col ) );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetIcon( int col, const Bitmap& icon )
{
   (*API->TreeBox->SetTreeBoxNodeColIcon)( handle, col, TreeBox::HandleFromBitmap( icon ) );
}

// ----------------------------------------------------------------------------

int TreeBox::Node::Alignment( int col ) const
{
   return (*API->TreeBox->GetTreeBoxNodeColAlignment)( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetAlignment( int col, int align )
{
   (*API->TreeBox->SetTreeBoxNodeColAlignment)( handle, col, align );
}

// ----------------------------------------------------------------------------

String TreeBox::Node::ToolTip( int col ) const
{
   size_type len = 0;
   (*API->TreeBox->GetTreeBoxNodeColToolTip)( handle, col, 0, &len );

   String tip;
   if ( len > 0 )
   {
      tip.SetLength( len );
      if ( (*API->TreeBox->GetTreeBoxNodeColToolTip)( handle, col, tip.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetTreeBoxNodeColToolTip" );
      tip.ResizeToNullTerminated();
   }
   return tip;
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetToolTip( int col, const String& tip )
{
   (*API->TreeBox->SetTreeBoxNodeColToolTip)( handle, col, tip.c_str() );
}

// ----------------------------------------------------------------------------

pcl::Font TreeBox::Node::Font( int col ) const
{
   return TreeBox::FontFromHandle( (*API->TreeBox->GetTreeBoxNodeColFont)( handle, col ) );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetFont( int col, const pcl::Font& font )
{
   (*API->TreeBox->SetTreeBoxNodeColFont)( handle, col, TreeBox::HandleFromFont( font ) );
}

// ----------------------------------------------------------------------------

RGBA TreeBox::Node::BackgroundColor( int col ) const
{
   return (*API->TreeBox->GetTreeBoxNodeColBackgroundColor)( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetBackgroundColor( int col, RGBA color )
{
   (*API->TreeBox->SetTreeBoxNodeColBackgroundColor)( handle, col, color );
}

// ----------------------------------------------------------------------------

RGBA TreeBox::Node::TextColor( int col ) const
{
   return (*API->TreeBox->GetTreeBoxNodeColTextColor)( handle, col );
}

// ----------------------------------------------------------------------------

void TreeBox::Node::SetTextColor( int col, RGBA color )
{
   (*API->TreeBox->SetTreeBoxNodeColTextColor)( handle, col, color );
}

// ----------------------------------------------------------------------------

void* TreeBox::Node::CloneHandle() const
{
   throw Error( "Cannot clone a TreeBox::Node handle" );
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<TreeBox*>( hSender ))
#define node( n ) (reinterpret_cast<TreeBox::Node*>( n ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class TreeBoxEventDispatcher
{
public:

   static void CurrentNodeUpdated( control_handle hSender, control_handle hReceiver, api_handle hNNew, api_handle hNOld )
   {
      if ( handlers->onCurrentNodeUpdated != nullptr )
         (receiver->*handlers->onCurrentNodeUpdated)( *sender, *node( hNNew ), *node( hNOld ) );
   }

   static void NodeActivated( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeActivated != nullptr )
         (receiver->*handlers->onNodeActivated)( *sender, *node( hN ), col );
   }

   static void NodeUpdated( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeUpdated != nullptr )
         (receiver->*handlers->onNodeUpdated)( *sender, *node( hN ), col );
   }

   static void NodeEntered( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeEntered != nullptr )
         (receiver->*handlers->onNodeEntered)( *sender, *node( hN ), col );
   }

   static void NodeClicked( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeClicked != nullptr )
         (receiver->*handlers->onNodeClicked)( *sender, *node( hN ), col );
   }

   static void NodeDoubleClicked( control_handle hSender, control_handle hReceiver, api_handle hN, int32 col )
   {
      if ( handlers->onNodeDoubleClicked != nullptr )
         (receiver->*handlers->onNodeDoubleClicked)( *sender, *node( hN ), col );
   }

   static void NodeExpanded( control_handle hSender, control_handle hReceiver, api_handle hN )
   {
      if ( handlers->onNodeExpanded != nullptr )
         (receiver->*handlers->onNodeExpanded)( *sender, *node( hN ) );
   }

   static void NodeCollapsed( control_handle hSender, control_handle hReceiver, api_handle hN )
   {
      if ( handlers->onNodeCollapsed != nullptr )
         (receiver->*handlers->onNodeCollapsed)( *sender, *node( hN ) );
   }

   static void NodeSelectionUpdated( control_handle hSender, control_handle hReceiver )
   {
      if ( handlers->onNodeSelectionUpdated != nullptr )
         (receiver->*handlers->onNodeSelectionUpdated)( *sender );
   }
}; // TreeBoxEventDispatcher

#undef sender
#undef node
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void TreeBox::OnCurrentNodeUpdated( node_navigation_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxCurrentNodeUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::CurrentNodeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxCurrentNodeUpdatedEventRoutine" );
   m_handlers->onCurrentNodeUpdated = f;
}

void TreeBox::OnNodeActivated( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeActivatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeActivated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeActivatedEventRoutine" );
   m_handlers->onNodeActivated = f;
}

void TreeBox::OnNodeUpdated( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeUpdatedEventRoutine" );
   m_handlers->onNodeUpdated = f;
}

void TreeBox::OnNodeEntered( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeEnteredEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeEntered : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeEnteredEventRoutine" );
   m_handlers->onNodeEntered = f;
}

void TreeBox::OnNodeClicked( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeClickedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeClicked : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeClickedEventRoutine" );
   m_handlers->onNodeClicked = f;
}

void TreeBox::OnNodeDoubleClicked( node_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeDoubleClickedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeDoubleClicked : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeDoubleClickedEventRoutine" );
   m_handlers->onNodeDoubleClicked = f;
}

void TreeBox::OnNodeExpanded( node_expand_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeExpandedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeExpanded : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeExpandedEventRoutine" );
   m_handlers->onNodeExpanded = f;
}

void TreeBox::OnNodeCollapsed( node_expand_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeCollapsedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeCollapsed : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeCollapsedEventRoutine" );
   m_handlers->onNodeCollapsed = f;
}

void TreeBox::OnNodeSelectionUpdated( tree_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->TreeBox->SetTreeBoxNodeSelectionUpdatedEventRoutine)( handle, &receiver,
                  (f != nullptr) ? TreeBoxEventDispatcher::NodeSelectionUpdated : nullptr ) == api_false )
      throw APIFunctionError( "SetTreeBoxNodeSelectionUpdatedEventRoutine" );
   m_handlers->onNodeSelectionUpdated = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/TreeBox.cpp - Released 2017-08-01T14:23:38Z
