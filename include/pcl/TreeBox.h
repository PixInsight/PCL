//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/TreeBox.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_TreeBox_h
#define __PCL_TreeBox_h

/// \file pcl/TreeBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/Bitmap.h>
#include <pcl/Font.h>
#include <pcl/IndirectArray.h>
#include <pcl/ScrollBox.h>
#include <pcl/SortedArray.h>
#include <pcl/TextAlign.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class TreeBox
 * \brief Client-side interface to a PixInsight %TreeBox control
 *
 * ### TODO: Write a detailed description for %TreeBox.
 */
class PCL_CLASS TreeBox : public ScrollBox
{
public:

   /*!
    * \class pcl::TreeBox::Node
    * \brief Client-side interface to a PixInsight %TreeBox node
    *
    * ### TODO: Write a detailed description for %TreeBox::Node.
    */
   class PCL_CLASS Node : public UIObject
   {
   public:

      /*! #
       */
      Node();

      /*
       * ### N.B.: If we define a default parameter value index=-1 for the
       * following constructor (as expected), the static member function Null()
       * cannot be compiled with g++ 4.9.x. The compiler issues the error:
       *
       *    no matching function for call to
       *    'pcl::TreeBox::Node::Node(pcl::TreeBox::Node)'
       *
       * which is obviously incorrect and looks like an obscure compiler bug.
       * Note that the same problem exists with g++ 4.8.x (at least).
       */
      /*! #
       */
      Node( Node& parent, int index );

      /*! #
       */
      Node( TreeBox& parentTree, int index = -1 );

      /*!
       * Destroys a %TreeBox::Node object.
       */
      virtual ~Node();

      /*! #
       */
      static Node Null()
      {
         // ### See note above for Node::Node( Node&, int ).
         return Node( nullptr );
      }

      /*! #
       */
      const TreeBox& ParentTree() const;

      /*! #
       */
      TreeBox& ParentTree();

      /*! #
       */
      const Node* Parent() const;

      /*! #
       */
      Node* Parent();

      /*! #
       */
      int NumberOfChildren() const;

      /*! #
       */
      const Node* Child( int idx ) const;

      /*! #
       */
      Node* Child( int idx );

      /*! #
       */
      const Node* operator []( int idx ) const
      {
         return Child( idx );
      }

      /*! #
       */
      Node* operator []( int idx )
      {
         return Child( idx );
      }

      /*! #
       */
      int ChildIndex( const Node* ) const;

      /*! #
       */
      void Insert( int idx, Node* );

      /*! #
       */
      void Add( Node* node )
      {
         Insert( NumberOfChildren(), node );
      }

      /*! #
       */
      void Remove( int idx );

      /*! #
       */
      bool IsEnabled() const;

      /*! #
       */
      void Enable( bool = true );

      /*! #
       */
      void Disable( bool disable = true )
      {
         Enable( !disable );
      }

      /*! #
       */
      bool IsExpanded() const;

      /*! #
       */
      bool IsCollapsed() const
      {
         return !IsExpanded();
      }

      /*! #
       */
      void Expand( bool expand = true );

      /*! #
       */
      void Collapse( bool collapse = true )
      {
         Expand( !collapse );
      }

      /*! #
       */
      bool IsSelectable() const;

      /*! #
       */
      void SetSelectable( bool = true );

      /*! #
       */
      bool IsSelected() const;

      /*! #
       */
      void Select( bool = true );

      /*! #
       */
      void Unselect( bool unselect = true )
      {
         Select( !unselect );
      }

      /*! #
       */
      bool IsCheckable() const;

      /*! #
       */
      void SetCheckable( bool = true );

      /*! #
       */
      bool IsChecked() const;

      /*! #
       */
      void Check( bool = true );

      /*! #
       */
      void Uncheck( bool uncheck = true )
      {
         Check( !uncheck );
      }

      /*! #
       */
      String Text( int col ) const;

      /*! #
       */
      void SetText( int col, const String& );

      /*! #
       */
      Bitmap Icon( int col ) const;

      /*! #
       */
      void SetIcon( int col, const Bitmap& );

      /*! #
       */
      int Alignment( int col ) const;

      /*! #
       */
      void SetAlignment( int col, int align );

      /*! #
       */
      String ToolTip( int col ) const;

      /*! #
       */
      void SetToolTip( int col, const String& );

      /*! #
       */
      pcl::Font Font( int col ) const;

      /*! #
       */
      void SetFont( int col, const pcl::Font& );

      /*! #
       */
      RGBA BackgroundColor( int col ) const;

      /*! #
       */
      void SetBackgroundColor( int col, RGBA );

      /*! #
       */
      RGBA TextColor( int col ) const;

      /*! #
       */
      void SetTextColor( int col, RGBA );

   protected:

      /*!
       * \internal
       */
      Node( void* h ) : UIObject( h )
      {
      }

      /*!
       * \internal
       */
      Node( std::nullptr_t ) : UIObject( nullptr )
      {
      }

      /*!
       * \internal
       */
      virtual void* CloneHandle() const;

   private:

      typedef SortedArray<Node*> child_node_list;
      child_node_list m_children;
      bool            m_removed = false;

      friend class TreeBox;
   };

   // -------------------------------------------------------------------------

   /*!
    * Constructs a %TreeBox as a child control of \a parent.
    */
   TreeBox( Control& parent = Control::Null() );

   /*!
    * Destroys a %TreeBox control.
    */
   virtual ~TreeBox();

   /*! #
    */
   static TreeBox& NullTree();

   //

   /*! #
    */
   int NumberOfChildren() const;

   /*! #
    */
   const Node* Child( int idx ) const;

   /*! #
    */
   Node* Child( int idx );

   /*! #
    */
   const Node* operator []( int idx ) const
   {
      return Child( idx );
   }

   /*! #
    */
   Node* operator []( int idx )
   {
      return Child( idx );
   }

   /*! #
    */
   int ChildIndex( const Node* ) const;

   /*! #
    */
   void Insert( int idx, Node* );

   /*! #
    */
   void Add( Node* n )
   {
      Insert( NumberOfChildren(), n );
   }

   /*! #
    */
   void Remove( int idx );

   /*! #
    */
   void Clear();

   //

   /*! #
    */
   const Node* CurrentNode() const;

   /*! #
    */
   Node* CurrentNode();

   /*! #
    */
   void SetCurrentNode( Node* );

   //

   /*! #
    */
   bool AreMultipleSelectionsEnabled() const;

   /*! #
    */
   bool AreMultipleSelectionsDisabled() const
   {
      return !AreMultipleSelectionsEnabled();
   }

   /*! #
    */
   void EnableMultipleSelections( bool enable = true );

   /*! #
    */
   void DisableMultipleSelections( bool disable = true )
   {
      EnableMultipleSelections( !disable );
   }

   /*!
    * Returns a dynamic array of pointers to all selected nodes in this tree.
    * If this tree has no selected nodes, an empty array is returned.
    *
    * The returned object is an instantiation of IndirectArray for the
    * TreeBox::Node class: IndirectArray\<TreeBox::Node\>.
    */
   IndirectArray<Node> SelectedNodes() const;

   /*!
    * Selects all nodes in this tree, that is the entire %TreeBox contents.
    */
   void SelectAllNodes();

   /*!
    * Returns true if this %TreeBox object has one or more selected toplevel
    * nodes; false if no toplevel nodes are selected.
    */
   bool HasSelectedTopLevelNodes() const;

   //

   /*! #
    */
   const Node* NodeByPosition( const pcl::Point& p ) const
   {
      return NodeByPosition( p.x, p.y ); // in client coordinates
   }

   /*! #
    */
   Node* NodeByPosition( const pcl::Point& p )
   {
      return NodeByPosition( p.x, p.y );
   }

   /*! #
    */
   const Node* NodeByPosition( int x, int y ) const;

   /*! #
    */
   Node* NodeByPosition( int x, int y );

   /*! #
    */
   void SetNodeIntoView( Node* );

   /*! #
    */
   pcl::Rect NodeRect( const Node* ) const;

   //

   /*! #
    */
   int NumberOfColumns() const;

   /*! #
    */
   void SetNumberOfColumns( int nCols );

   // Visibility

   /*! #
    */
   bool IsColumnVisible( int col ) const;

   /*! #
    */
   void ShowColumn( int col, bool show = true );

   /*! #
    */
   void HideColumn( int col, bool hide = true )
   {
      ShowColumn( col, !hide );
   }

   //

   /*! #
    */
   int ColumnWidth( int col ) const;

   /*! #
    */
   void SetColumnWidth( int col, int width );

   /*! #
    */
   int ScaledColumnWidth( int col ) const
   {
      return PhysicalPixelsToLogical( ColumnWidth( col ) );
   }

   /*! #
    */
   void SetScaledColumnWidth( int col, int width )
   {
      SetColumnWidth( col, LogicalPixelsToPhysical( width ) );
   }

   /*! #
    */
   void AdjustColumnWidthToContents( int col );

   //

   /*! #
    */
   String HeaderText( int col ) const;

   /*! #
    */
   void SetHeaderText( int col, const String& );

   //

   /*! #
    */
   Bitmap HeaderIcon( int col ) const;

   /*! #
    */
   void SetHeaderIcon( int col, const Bitmap& );

   //

   /*! #
    */
   int HeaderAlignment( int col ) const;

   /*! #
    */
   void SetHeaderAlignment( int col, int align );

   /*! #
    */
   bool IsHeaderVisible() const;

   /*! #
    */
   void ShowHeader( bool show = true );

   /*! #
    */
   void HideHeader( bool hide = true )
   {
      ShowHeader( !hide );
   }

   //

   /*! #
    */
   int IndentSize() const;

   /*! #
    */
   void SetIndentSize( int );

   /*! #
    */
   int ScaledIndentSize() const
   {
      return PhysicalPixelsToLogical( IndentSize() );
   }

   /*! #
    */
   void SetScaledIndentSize( int size )
   {
      SetIndentSize( LogicalPixelsToPhysical( size ) );
   }

   //

   /*! #
    */
   bool IsNodeExpansionEnabled() const;

   /*! #
    */
   void EnableNodeExpansion( bool = true );

   /*! #
    */
   void DisableNodeExpansion( bool disable = true )
   {
      EnableNodeExpansion( !disable );
   }

   //

   /*! #
    */
   bool IsRootDecorationEnabled() const;

   /*! #
    */
   void EnableRootDecoration( bool = true );

   /*! #
    */
   void DisableRootDecoration( bool disable = true )
   {
      EnableRootDecoration( !disable );
   }

   // Alternating row colors
   //    Even row color is Control::CanvasColor()
   //    Odd row color is Control::AlternateCanvasColor()

   /*! #
    */
   bool IsAlternateRowColorEnabled() const;

   /*! #
    */
   void EnableAlternateRowColor( bool = true );

   /*! #
    */
   void DisableAlternateRowColor( bool disable = true )
   {
      EnableAlternateRowColor( !disable );
   }

   //

   /*! #
    */
   bool IsUniformRowHeightEnabled() const;

   /*! #
    */
   void EnableUniformRowHeight( bool = true );

   /*! #
    */
   void DisableUniformRowHeight( bool disable = true )
   {
      EnableUniformRowHeight( !disable );
   }

   //

   /*! #
    */
   void GetIconSize( int& width, int& height ) const;

   /*! #
    */
   int IconWidth() const
   {
      int w, dum; GetIconSize( w, dum ); return w;
   }

   /*! #
    */
   int IconHeight() const
   {
      int dum, h; GetIconSize( dum, h ); return h;
   }

   /*! #
    */
   void SetIconSize( int width, int height );

   /*! #
    */
   void SetIconSize( int size )
   {
      SetIconSize( size, size );
   }

   /*! #
    */
   void GetScaledIconSize( int& width, int& height ) const
   {
      GetIconSize( width, height ); width = PhysicalPixelsToLogical( width ); height = PhysicalPixelsToLogical( height );
   }

   /*! #
    */
   int ScaledIconWidth() const
   {
      int width, dum; GetIconSize( width, dum ); return PhysicalPixelsToLogical( width );
   }

   /*! #
    */
   int ScaledIconHeight() const
   {
      int dum, height; GetIconSize( dum, height ); return PhysicalPixelsToLogical( height );
   }

   /*! #
    */
   void SetScaledIconSize( int width, int height )
   {
      SetIconSize( LogicalPixelsToPhysical( width ), LogicalPixelsToPhysical( height ) );
   }

   /*! #
    */
   void SetScaledIconSize( int size )
   {
      size = LogicalPixelsToPhysical( size );
      SetIconSize( size, size );
   }

   //

   /*! #
    */
   bool IsHeaderSortingEnabled() const;

   /*! #
    */
   void EnableHeaderSorting( bool = true );

   /*! #
    */
   void DisableHeaderSorting( bool disable = true )
   {
      EnableHeaderSorting( !disable );
   }

   /*! #
    */
   void Sort( int col = 0, bool ascending = true );

   //

   /*! #
    */
   bool IsNodeDraggingEnabled() const;

   /*! #
    */
   void EnableNodeDragging( bool = true );

   /*! #
    */
   void DisableNodeDragging( bool disable = true )
   {
      EnableNodeDragging( !disable );
   }

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnCurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   // void OnNodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   // void OnNodeUpdated( TreeBox& sender, TreeBox::Node& node, int col );
   // void OnNodeEntered( TreeBox& sender, TreeBox::Node& node, int col );
   // void OnNodeClicked( TreeBox& sender, TreeBox::Node& node, int col );
   // void OnNodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col );
   // void OnNodeExpanded( TreeBox& sender, TreeBox::Node& node );
   // void OnNodeCollapsed( TreeBox& sender, TreeBox::Node& node );
   // void OnNodeSelectionUpdated( TreeBox& sender );

   /*!
    * \defgroup tree_box_event_handlers TreeBox Event Handlers
    */

   /*! #
    * \ingroup tree_box_event_handlers
    */
   typedef void (Control::*tree_event_handler)( TreeBox& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   typedef void (Control::*node_event_handler)( TreeBox&, TreeBox::Node&, int );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   typedef void (Control::*node_expand_event_handler)( TreeBox&, TreeBox::Node& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   typedef void (Control::*node_navigation_event_handler)( TreeBox&, TreeBox::Node&, TreeBox::Node& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnCurrentNodeUpdated( node_navigation_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeActivated( node_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeUpdated( node_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeEntered( node_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeClicked( node_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeDoubleClicked( node_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeExpanded( node_expand_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeCollapsed( node_expand_event_handler, Control& );

   /*! #
    * \ingroup tree_box_event_handlers
    */
   void OnNodeSelectionUpdated( tree_event_handler, Control& );

private:

   struct EventHandlers
   {
      node_navigation_event_handler onCurrentNodeUpdated   = nullptr;
      node_event_handler            onNodeActivated        = nullptr;
      node_event_handler            onNodeUpdated          = nullptr;
      node_event_handler            onNodeEntered          = nullptr;
      node_event_handler            onNodeClicked          = nullptr;
      node_event_handler            onNodeDoubleClicked    = nullptr;
      node_expand_event_handler     onNodeExpanded         = nullptr;
      node_expand_event_handler     onNodeCollapsed        = nullptr;
      tree_event_handler            onNodeSelectionUpdated = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

   typedef SortedArray<Node*> child_node_list;
   child_node_list m_children;

   static pcl::Font FontFromHandle( void* h )
   {
      return pcl::Font( h );
   }

   static const void* HandleFromFont( const pcl::Font& f )
   {
      return f.handle;
   }

   static Bitmap BitmapFromHandle( void* h )
   {
      return Bitmap( h );
   }

   static const void* HandleFromBitmap( const Bitmap& p )
   {
      return p.handle;
   }

protected:

   /*!
    * \internal
    */
   TreeBox( void* );

   friend class Node;
   friend class TreeBoxEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_TreeBox_h

// ----------------------------------------------------------------------------
// EOF pcl/TreeBox.h - Released 2017-06-21T11:36:33Z
