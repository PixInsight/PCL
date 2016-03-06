#ifndef Voronoi_h
#define Voronoi_h

#include <list>
#include <queue>
#include <set>

namespace vor {

   class VParabola;

   /*
      A structure that stores 2D point
    */

   struct VPoint {
   public:

      double x, y;
      double c; // color

      /*
         Constructor for structure; x, y - coordinates
       */

      VPoint(double nx, double ny, double nc = 0) : c(nc) {
         x = nx;
         y = ny;
      }
   };

   /*
      A class that stores an edge in Voronoi diagram

      start		: pointer to start point
      end		: pointer to end point
      left		: pointer to Voronoi place on the left side of edge
      right		: pointer to Voronoi place on the right side of edge

      neighbour: some edges consist of two parts, so we add the pointer to another part to connect them at the end of an algorithm

      direction: directional vector, from "start", points to "end", normal of |left, right|
      f, g		: directional coeffitients satisfying equation y = f*x + g (edge lies on this line)
    */

   class VEdge {
   public:

      VPoint * start;
      VPoint * end;
      VPoint * direction;
      VPoint * left;
      VPoint * right;

      double f;
      double g;

      VEdge * neighbour;

      /*
         Constructor of the class

         s		: pointer to start
         a		: pointer to left place
         b		: pointer to right place
       */

      VEdge(VPoint * s, VPoint * a, VPoint * b) {
         start = s;
         left = a;
         right = b;
         neighbour = 0;
         end = 0;

         f = (b->x - a->x) / (a->y - b->y);
         g = s->y - f * s->x;
         direction = new VPoint(b->y - a->y, -(b->x - a->x));
      }

      /*
         Destructor of the class
         direction belongs only to the current edge, other pointers can be shared by other edges
       */

      ~VEdge() {
         delete direction;
      }

   };

   /*
      The class for storing place / circle event in event queue.

      point		: the point at which current event occurs (top circle point for circle event, focus point for place event)
      pe			: whether it is a place event or not
      y			: y coordinate of "point", events are sorted by this "y"
      arch		: if "pe", it is an arch above which the event occurs
    */

   class VEvent {
   public:
      VPoint * point;
      bool pe;
      double y;
      VParabola * arch;

      /*
         Constructor for the class
		
         p		: point, at which the event occurs
         pev		: whether it is a place event or not
       */

      VEvent(VPoint * p, bool pev) {
         point = p;
         pe = pev;
         y = p->y;
         arch = 0;
      }

      /*
         function for comparing two events (by "y" property)
       */

      struct CompareEvent : public std::binary_function<VEvent*, VEvent*, bool> {

         bool operator()(const VEvent* l, const VEvent * r) const {
            return (l->y < r->y);
         }
      };
   };


   /*
      Parabolas and events have pointers to each other, so we declare class VEvent, which will be defined later.
    */

   class VEvent;

   /*
      A class that stores information about an item in beachline sequence (see Fortune's algorithm). 
      It can represent an arch of parabola or an intersection between two archs (which defines an edge).
      In my implementation I build a binary tree with them (internal nodes are edges, leaves are archs).
    */

   class VParabola {
   public:

      /*
         isLeaf	: flag whether the node is Leaf or internal node
         site		: pointer to the focus point of parabola (when it is parabola)
         edge		: pointer to the edge (when it is an edge)
         cEvent	: pointer to the event, when the arch disappears (circle event)
         parent	: pointer to the parent node in tree
       */

      bool isLeaf;
      VPoint* site;
      VEdge* edge;
      VEvent* cEvent;
      VParabola* parent;

      /*
         Constructors of the class (empty for edge, with focus parameter for an arch).
       */

      VParabola();
      VParabola(VPoint * s);

      /*
         Access to the children (in tree).
       */

      void SetLeft(VParabola * p) {
         _left = p;
         p->parent = this;
      }

      void SetRight(VParabola * p) {
         _right = p;
         p->parent = this;
      }

      VParabola * Left() {
         return _left;
      }

      VParabola * Right() {
         return _right;
      }

      /*
         Some useful tree operations

         GetLeft			: returns the closest left leave of the tree
         GetRight			: returns the closest right leafe of the tree
         GetLeftParent	: returns the closest parent which is on the left
         GetLeftParent	: returns the closest parent which is on the right
         GetLeftChild	: returns the closest leave which is on the left of current node
         GetRightChild	: returns the closest leave which is on the right of current node
       */

      static VParabola * GetLeft(VParabola * p);
      static VParabola * GetRight(VParabola * p);
      static VParabola * GetLeftParent(VParabola * p);
      static VParabola * GetRightParent(VParabola * p);
      static VParabola * GetLeftChild(VParabola * p);
      static VParabola * GetRightChild(VParabola * p);

   private:

      VParabola * _left;
      VParabola * _right;
   };


   /*
      Useful data containers for Vertices (places) and Edges of Voronoi diagram
    */

   typedef std::list<VPoint *> Vertices;
   typedef std::list<VEdge *> Edges;

   /*
      Class for generating the Voronoi diagram
    */

   class Voronoi {
   public:

      /*
         Constructor - without any parameters
       */

      Voronoi();

      /*
         The only public function for generating a diagram
			
         input:
            v		: Vertices - places for drawing a diagram
            w		: width  of the result (top left corner is (0, 0))
            h		: height of the result
         output:
            pointer to list of edges
			
         All the data structures are managed by this class
       */

      Edges * GetEdges(Vertices * v, double w, double h);

   private:

      /*
                  places	: container of places with which we work
                  edges		: container of edges which will be teh result
                  width		: width of the diagram
                  height	: height of the diagram
                  root		: the root of the tree, that represents a beachline sequence
                  ly			: current "y" position of the line (see Fortune's algorithm)
       */

      Vertices * places;
      Edges * edges;
      double width, height;
      VParabola * root;
      double ly;

      /*
                  deleted	: set  of deleted (false) Events (since we can not delete from PriorityQueue
                  points	: list of all new points that were created during the algorithm
                  queue		: priority queue with events to process
       */

      std::set<VEvent *> deleted;
      std::list<VPoint *> points;
      std::priority_queue<VEvent *, std::vector<VEvent *>, VEvent::CompareEvent> queue;

      /*
                  InsertParabola		: processing the place event
                  RemoveParabola		: processing the circle event
                  FinishEdge			: recursively finishes all infinite edges in the tree
                  GetXOfEdge			: returns the current x position of an intersection point of left and right parabolas
                  GetParabolaByX		: returns the Parabola that is under this "x" position in the current beachline
                  CheckCircle			: checks the circle event (disappearing) of this parabola
                  GetEdgeInterse
       */

      void InsertParabola(VPoint * p);
      void RemoveParabola(VEvent * e);
      void FinishEdge(VParabola * n);
      double GetXOfEdge(VParabola * par, double y);
      VParabola * GetParabolaByX(double xx);
      double GetY(VPoint * p, double x);
      void CheckCircle(VParabola * b);
      VPoint * GetEdgeIntersection(VEdge * a, VEdge * b);
   };
}

#endif